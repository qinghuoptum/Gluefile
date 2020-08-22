#include "stdafx.h"
#include "fsound.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <errno.h>

//////////////////////// Vox Class  /////////////////////////////////

//#define MULAW_SUPPORT		100



vox::vox()
{
	strcpy( VoxHeader.ID,   "DIC" );  
	VoxHeader.State = 0 ;		 
	strcpy( VoxHeader.reserve,  "   " ); 
	VoxHeader.VoiceType = 0 ;		 
	VoxHeader.DictLength = 0 ;		 
	VoxHeader.DictTime = 0 ;		 
	VoxHeader.CueLocation = 0 ;		 
	VoxHeader.InterruptLoc = 0 ;		 
	VoxHeader.Priority = '0' ;		 
	strcpy( VoxHeader.NameFirst,  "   " ); 
	strcpy( VoxHeader.NameLast,  "   " ); 

	fd = -1;
	fileSize = 0;
	fileMode = 0;
	vocType = 0;
	
}

int vox::Open( char* name, int mode )
{
	fileMode = mode;
	if ( mode == READ_MODE  ) {
		fd = open( name, _O_RDONLY|_O_BINARY, 0666 );
		if ( fd < 0 ) return fd;
	}
	if ( mode == WRITE_MODE ) {
		fd = open( name, _O_RDWR|_O_BINARY|_O_CREAT|_O_TRUNC, 0666 );
		if ( fd < 0 ) return fd;
	}
	fileSize = 0;
	return 1;
}

int vox::Open( char* name, int mode, int* bits, int* rate )
{
	int ct;
	fileMode = mode;
	if ( mode == READ_MODE  ) {
		fd = open( name, _O_RDONLY|_O_BINARY, 0666 );
		if ( fd < 0 ) return fd;
		ct = read( fd, &VoxHeader, sizeof(VoxHeader) );

		vocType = 0;
		switch ( VoxHeader.VoiceType ) {
		case 0:
		case 4:
			*rate =	6*1000; *bits = 4; break;
		case 1:
		case 5:
			*rate =	8*1000; *bits = 4; break;
		case 2:
		case 6:
			vocType = 1; *rate = 6*1000; *bits = 8; break;
		case 3:
		case 7:
			vocType = 1; *rate = 8*1000; *bits = 8; break;
		default:
			*rate =	6*1000; *bits = 4; break;
		}
		fileSize = filelength ( fd ) - sizeof(VoxHeader);

	}
	if ( mode == WRITE_MODE ) {
		fd = open( name, _O_RDWR|_O_BINARY|_O_CREAT|_O_TRUNC, 0666 );
		if ( fd < 0 ) return fd;

		*rate = 6*1000;
		*bits = 4;
		ct = write(fd, (char*)&VoxHeader, sizeof(VoxHeader) );
		fileSize = 0;
	}
	return 1;
}

int vox::Close(  )
{
	int flag = 1;
	if ( fd<0 ) return -1;

	close(fd);
	fd = -1;
	return flag;
}

int vox::Read( char* buf, int size )
{	int ct;
	if ( fd < 0 ) return -1;
	ct = read( fd, buf, size );
	// if ( ct >= 0 ) fileSize += ct;
	return ct;
}

int vox::Write( char* buf, int size )
{	int ct;
	if ( fd < 0 ) return -1;
	ct = write( fd, buf, size );
	// if ( ct >0 ) fileSize += ct;
	return ct;
}

int vox::SeekPos( long pos )
{
	if ( fd < 0 ) return -1;
	if ( lseek( fd, pos , SEEK_SET ) < 0 ) return -1;
	return 1;
}

int vox::Eof()
{
	if ( fd < 0 ) return -1;
	return eof( fd );
}


//////////////////////// Wave Class  /////////////////////////////////

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#define MIN( a, b)		((a)>(b)?(b):(a))

#define LOGSCALE(x,n)	(n<2?x:(n==2?x*3/2:(n==3?x*9/4:(n==4?x*3:(n==5?x*5:x*8)))))


CWaveFile::CWaveFile()
{
	fileMode = 0;
	openState = FALSE;
	m_pwfx = NULL;
}


int CWaveFile::Open( TCHAR* name, int mode, WAVEFORMATEX *pwfx )
{
	fileMode = mode;
	if ( mode == READ_MODE  ) {
		// fd = open( name, _O_RDONLY|_O_BINARY, 0666 );
		if ( !fd.Open( name, CFile::modeRead ) ) return 0;

		if ( !ReadMMIO( &m_pwfx ) )
		{
			fd.Close( );
			return 0;
		}
		m_dwPos = 0;	
		m_dwPlayPos = 0;
	}
	if ( mode == WRITE_MODE ) {
		//fd = open( name, _O_RDWR|_O_BINARY|_O_CREAT|_O_TRUNC, 0666 );
		/*if (fd.Open( name, CFile::modeReadWrite))
		{
			if ( !ReadMMIO( &m_pwfx ) )
			{
				fd.Close( );
				return 0;
			}
			fd.Seek(0L,CFile::end);
			m_dwPos = m_ckIO.cksize;	
			m_dwPlayPos = 0;
		}
		else
		{*/
			if ( !fd.Open( name, CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate ) ) 
				return 0;

			if( !WriteMMIO( pwfx ) )
			{
				fd.Close();
				return 0;
			}
			m_pwfx = new WAVEFORMATEX();
			*m_pwfx = *pwfx;
			m_dwPlayPos = 0;
			m_dwPos = 0;
		//}
	}
	openState = TRUE;
	return 1;
}


int CWaveFile::Close(  )
{	
	if ( !openState ) return 0;

	if ( fileMode == WRITE_MODE ) 
	{
		PutChunk( &fd, &m_ckIO, 1 );
		PutChunk( &fd, &m_ckIORiff, 1 );
	} 
	fd.Close();
	SAFE_DELETE(m_pwfx);
	openState = FALSE;
	m_dwPos = 0;
	m_dwPlayPos = 0;
	return 1;
}


BOOL CWaveFile::ReadMMIO( WAVEFORMATEX** ppwfxInfo )
{
    MMCKINFO        ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.       

    *ppwfxInfo = NULL;

    if( !GetChunk( &fd, &m_ckIORiff, 0 ) )
        return FALSE;

    if( (m_ckIORiff.ckid != FOURCC_RIFF) ||
        (m_ckIORiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
        return FALSE;

    // Search the input file for for the 'fmt ' chunk.
    // ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if( !GetChunk( &fd, &ckIn, 0 ) )
        return FALSE;
    if (ckIn.ckid != mmioFOURCC('f', 'm', 't', ' '))
		return FALSE;

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
    if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
		return FALSE;

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( fd.Read( (HPSTR) &pcmWaveFormat, 
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
        return FALSE;

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        if( NULL == ( *ppwfxInfo = new WAVEFORMATEX ) )
            return FALSE;

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( *ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        (*ppwfxInfo)->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( fd.Read( (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
            return FALSE;

        *ppwfxInfo = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
        if( NULL == *ppwfxInfo )
            return FALSE;

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( *ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        (*ppwfxInfo)->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( fd.Read( (CHAR*)(((BYTE*)&((*ppwfxInfo)->cbSize))+sizeof(WORD)),
                      cbExtraBytes ) != cbExtraBytes )
        {
            delete *ppwfxInfo;
            *ppwfxInfo = NULL;
            return FALSE;
        }
    }

	// Skip this chunk to next one
    if( !GetChunk( &fd, &ckIn, 1 ) )
        return FALSE;

    // Ascend the input file out of the 'fmt ' chunk.
	while ( TRUE )
	{
		if( !GetChunk( &fd, &m_ckIO, 0 ) )
		{
			delete *ppwfxInfo;
			*ppwfxInfo = NULL;
			return FALSE;
		}
		if (m_ckIO.ckid == mmioFOURCC('d', 'a', 't', 'a')) break;
		if (m_ckIO.ckid == mmioFOURCC('f', 'a', 'c', 't')) 
			fd.Seek( (LONG)m_ckIO.cksize, CFile::current );
		else
		{
			delete *ppwfxInfo;
			*ppwfxInfo = NULL;
			return FALSE;
		}
	}
    return TRUE;
}


BOOL CWaveFile::WriteMMIO( WAVEFORMATEX* pwfxDest )
{
    DWORD    dwFactChunk; // Contains the actual fact chunk. Garbage until WaveCloseWriteFile.
    MMCKINFO ckOut;
    
    dwFactChunk = (DWORD)-1;

    // Create the output file RIFF chunk of form type 'WAVE'.
    m_ckIORiff.ckid = mmioFOURCC('R', 'I', 'F', 'F');       
    m_ckIORiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');       
    m_ckIORiff.cksize = 0;

    if( !PutChunk( &fd, &m_ckIORiff, 0 ) )
        return FALSE;
    
    // We are now descended into the 'RIFF' chunk we just created.
    // Now create the 'fmt ' chunk. Since we know the size of this chunk,
    // specify it in the MMCKINFO structure so MMIO doesn't have to seek
    // back and set the chunk size after ascending from the chunk.
    ckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');
    ckOut.cksize = sizeof(PCMWAVEFORMAT);   

    if( !PutChunk( &fd, &ckOut, 0 ) )
        return FALSE;
    
    // Write the PCMWAVEFORMAT structure to the 'fmt ' chunk if its that type.
	if( pwfxDest->wFormatTag == WAVE_FORMAT_PCM)
    {
        fd.Write((HPSTR)pwfxDest, sizeof(PCMWAVEFORMAT));
    }   
    else 
    {
        // Write the variable length size.
        fd.Write( (HPSTR) pwfxDest, sizeof(*pwfxDest) + pwfxDest->cbSize+2 );
		ckOut.cksize += pwfxDest->cbSize+2;
    }  
    
    // Ascend out of the 'fmt ' chunk, back into the 'RIFF' chunk.
    if( !PutChunk( &fd, &ckOut, 1 ) )
        return FALSE;
    
    m_ckIO.ckid = mmioFOURCC('d', 'a', 't', 'a');       
    m_ckIO.fccType = 0;       
    m_ckIO.cksize = 0;

    if( !PutChunk( &fd, &m_ckIO, 0 ) )
        return FALSE;
    
	m_ckIORiff.cksize = 4 + (ckOut.cksize+8) + 8;
    return TRUE;
}



BOOL GetChunk( CFile *fp, LPMMCKINFO lpck, UINT wFlags )
{
	if ( wFlags==0)
	{
		if (fp->Read(&(lpck->ckid), 4) !=4 ) return FALSE;
		if (fp->Read(&(lpck->cksize), 4) !=4 ) return FALSE;
		lpck->dwDataOffset = fp->Seek(0L, CFile::current);
		if ( lpck->ckid == FOURCC_RIFF )
			if (fp->Read(&(lpck->fccType), 4) !=4 ) return FALSE;
		return TRUE;
	}
	else if ( wFlags==1 )
	{
		// To be programmed
		fp->Seek( lpck->dwDataOffset+lpck->cksize, CFile::begin );
		return TRUE;
	}
	else
		return FALSE;

}


BOOL PutChunk( CFile *fp, LPMMCKINFO lpck, UINT wFlags )
{
	if ( wFlags==0)
	{
		fp->Write(&(lpck->ckid), 4);
		fp->Write(&(lpck->cksize), 4);
		lpck->dwDataOffset = fp->Seek(0L, CFile::current);
		if ( lpck->ckid == FOURCC_RIFF )
			fp->Write(&(lpck->fccType), 4);
		return TRUE;
	}
	else if ( wFlags==1 )
	{
		// To be programmed
		fp->Seek( lpck->dwDataOffset-8, CFile::begin );
		fp->Write(&(lpck->ckid), 4);
		fp->Write(&(lpck->cksize), 4);
		fp->Seek( lpck->cksize, CFile::current );
		return TRUE;
	}
	else
		return FALSE;

}


int CWaveFile::Read( char* buf, int size )
{	
	int ct;
	if (!openState) return 0;
	ct = fd.Read( (char*)buf, size );
	m_dwPos += ct;
	return ct;
}


int CWaveFile::Write( char* buf, int size )
{	
	if (!openState) return 0;
	fd.Write((char*)buf, size );
	m_dwPos += size;
	if (m_dwPos > m_ckIO.cksize)
	{
		m_ckIORiff.cksize += (m_dwPos-m_ckIO.cksize);
		m_ckIO.cksize = m_dwPos;
	}
	return size;
}


int CWaveFile::SeekPos( long pos )
{
	if (!openState) return 0;
	if ( pos==POS_END) 
	{
		fd.SeekToEnd();
		m_dwPos = m_ckIO.cksize;
	}
	else if ( pos==POS_PLAY )
	{
		if (fd.Seek( m_ckIO.dwDataOffset+m_dwPlayPos, CFile::begin ) < 0 ) return 0;
		m_dwPos = m_dwPlayPos;
	}
	else 
	{ 
		pos &= 0xFFFFFFFE;
		if (fd.Seek( m_ckIO.dwDataOffset+pos, CFile::begin ) < 0 ) return 0;
		m_dwPos = pos;
	}
	return 1;
}


int	CWaveFile::GetPos( long nType )
{
	if (nType == POS_PLAY )
		return m_dwPlayPos;
	else
		return m_dwPos;
}


int  CWaveFile::SetPlayPos( long playLen )
{
	if (!openState) return 0;
	m_dwPlayPos += playLen;
	if ( m_dwPlayPos > m_ckIO.cksize ) m_dwPlayPos = m_ckIO.cksize;
	return 1;
}


long  CWaveFile::GoToStart()
{
	if (!openState) return 0;
	SeekPos( 0 );
	m_dwPos = 0;
	m_dwPlayPos = 0;
	return m_dwPos;
}


long  CWaveFile::GoToEnd()
{
	if (!openState) return 0;
	SeekPos( POS_END );
	m_dwPos = m_ckIO.cksize;
	m_dwPlayPos = m_dwPos;
	return m_dwPos;
}
                    

long  CWaveFile::GetLenInTime( ) //measure file size to get length
{
	return LenToTime( m_ckIO.cksize );
}


long  CWaveFile::GetPosInTime( long nType )
{
	if (nType == POS_PLAY )
		return LenToTime(m_dwPlayPos);
	else
		return LenToTime(m_dwPos);
}

void  CWaveFile::SetPosInPercent( int nPerc )
{
	if ( nPerc<0 || nPerc>100 ) return;
	m_dwPos = (nPerc*m_ckIO.cksize/100) & 0xFFFFFFFE;
	m_dwPlayPos = m_dwPos;
	SeekPos( m_dwPos );
	return;
}


void  CWaveFile::SetPosInTime( int nMillSecs )
{
	DWORD dwPos;
	dwPos = TimeToLen( nMillSecs ) & 0xFFFFFFFE;   
	if ( dwPos > m_ckIO.cksize ) return;
	m_dwPos = dwPos;
	m_dwPlayPos = m_dwPos;
	SeekPos( m_dwPos );
	return;
}


long  CWaveFile::Forward( long nLast )
{
	long nStep1, nStep2, nStep;

	m_dwPos = m_dwPlayPos;
	nStep1 = m_ckIO.cksize*4/100;
	nStep2 = TimeToLen(3000);
	nStep = MIN( nStep1, nStep2 );
	nStep = LOGSCALE( nStep, nLast );
	m_dwPos += nStep;
	m_dwPos &= 0xFFFFFFFE;
	if (m_dwPos>m_ckIO.cksize) m_dwPos = m_ckIO.cksize;
	m_dwPlayPos = m_dwPos;
	SeekPos( m_dwPos );
	return nStep;
}


long  CWaveFile::Rewind( long nLast )
{
	DWORD nStep1, nStep2, nStep;

	m_dwPos = m_dwPlayPos;
	nStep1 = m_ckIO.cksize*4/100;
	nStep2 = TimeToLen(3000);
	nStep = MIN( nStep1, nStep2 );
	nStep = LOGSCALE( nStep, nLast );
	m_dwPos = (m_dwPos<nStep?0:m_dwPos-nStep);
	m_dwPos &= 0xFFFFFFFE;
	m_dwPlayPos = m_dwPos;
	SeekPos( m_dwPos );
	return (long)nStep;
}


WORD  CWaveFile::Channel( ) { return m_pwfx->nChannels; };
WORD  CWaveFile::Format( ) { return m_pwfx->wFormatTag; };
DWORD CWaveFile::SamplesRate( )   { return m_pwfx->nSamplesPerSec; };
WORD  CWaveFile::BitsPerSample( ) { return m_pwfx->wBitsPerSample; }


DWORD CWaveFile::TimeToLen(  DWORD nMilliseconds )
{
   // nSamplesPerSec is dynamic, is adjusted to play speed
   double bytes = (double)nMilliseconds;
   if (bytes==0) return 0;
   bytes = bytes*(1.0*m_pwfx->nSamplesPerSec*m_pwfx->nChannels*m_pwfx->wBitsPerSample/8 )/1000.0;
   return (DWORD)bytes;
}


DWORD CWaveFile::LenToTime( DWORD nBytes )
{  
   double bytes = (double)nBytes;
   if (bytes==0) return 0;
   bytes = 1.0*bytes/(1.0*m_pwfx->nSamplesPerSec*m_pwfx->nChannels*m_pwfx->wBitsPerSample/8 )*1000.0;
   return (DWORD)bytes;
}



// VoxCvt.cpp: implementation of the VoxCvt class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VoxCvt::VoxCvt()
{
	value = 0;  
	index = 1;  
}



void VoxCvt::Reset( )
{
	value = 0;  
	index = 1;  
}

//convert from 16 bits PCM wave format
int  VoxCvt::Wave2Vox16( char* buf_out, char* buf_in, int size )
{
	short* wave = (short*)buf_in;
	char* code = buf_out;
	int i;
	unsigned char code_h, code_l;
	if ( size==0 ) return 0;
	if ( size & 0x01 ) size--;
	size /= 4;
	for ( i = 0; i< size; i++ ) {
		code_h=  ADPCM_Encode( (short)wave[ 2*i  ]/16 );
		code_l=  ADPCM_Encode( (short)wave[ 2*i+1]/16 );
		(char)code[i] = (code_h<<4) | code_l ;
	}
	return i;

}


#define pad_overflowe16(x) ( (x>0)? ( x<2048 ? x*16:0x7fff) : ( x<-2047 ? 0x8fff:x*16 ) ) 

//convert to 16 bits PCM wave format
int  VoxCvt::Vox2Wave16( char* buf_out, char* buf_in, int size )
{
	short*  wave = (short*)buf_out;
	char*   code = buf_in;
	int i;
	unsigned char code_h, code_l;
	short xh,xl;
	if ( size==0 ) return 0;
	for ( i = 0; i< size; i++ ) {
		code_h = ( code[i]>>4 ) & 0x0f;
		code_l = ( code[i] ) & 0x0f;
		xh = ADPCM_Decode( code_h ) ;
		xl = ADPCM_Decode( code_l ) ;
		wave[ 2*i  ] = pad_overflowe16( xh );
		wave[ 2*i+1] = pad_overflowe16( xl );
	}
	return i*2*2;
}



//convert to 16 bits PCM wave format with different Sample Rate
int  VoxCvt::Vox2Wave16Ex( char* buf_out, char* buf_in, int size,
						   int sampleRateOut, int sampleRateIn )
{
	short* wave = (short*)buf_out;
	short  current = 0;
	char*  code = buf_in;
	int i, k, idx, r, q;
	unsigned char code_h, code_l;
	short xh, xl;
	short* ptrGap = NULL;

	if ( size==0 ) return 0;

	ptrGap = (short*)malloc( sampleRateIn*2 );
	if ( ptrGap == NULL ) {
		return 0;
	};
	for(i=0; i<sampleRateIn; i++) ptrGap[i]=0;

	q = sampleRateOut / sampleRateIn;
	r = sampleRateOut % sampleRateIn;
	FillGap( ptrGap, r, sampleRateIn, 0);

	k = 0;
	idx = 0;
	for ( i = 0; i< size; i++ ) {
		
		code_h = ( code[i]>>4 ) & 0x0f;
		code_l = ( code[i] ) & 0x0f;
		xh = ADPCM_Decode( code_h ) ;
		xl = ADPCM_Decode( code_l ) ;

		k += InterValue(k,(int)ptrGap[idx]+q,current,xh,wave);
		idx = ++idx % sampleRateIn;
		k += InterValue(k,(int)ptrGap[idx]+q,xh,xl,wave);
		idx = ++idx % sampleRateIn;
		current = xl;
		// wave[ 2*i  ] = pad_overflowe16( xh );
		// wave[ 2*i+1] = pad_overflowe16( xl );

	}

	if (ptrGap!=NULL) free(ptrGap);
	return 2*k;
}


// Interpoly
int VoxCvt::InterValue(int k, int n, short prev, short next, short *wave )
{
	int i, h;

	for (i=1; i<=n; i++) {
		h = ((n-i)*prev + i*next) / n;
		wave[k++] = pad_overflowe16( (short)h);
	}
	return n;
}

//convert from 8 bits PCM wave format
int  VoxCvt::Wave2Vox( char* buf_out, char* buf_in, int size )
{
	char* wave = buf_in;
	char* code = buf_out;
	int i;
	unsigned char code_h, code_l;
	short xh,xl;
	if ( size==0 ) return 0;
	if ( size & 0x01 ) size--;
	size /= 2;
	for ( i = 0; i< size; i++ ) {
		xh = (short)((char)(wave[ 2*i   ] - 128)*16);
		xl = (short)((char)(wave[ 2*i+1 ] - 128)*16);
		code_h=  ADPCM_Encode( xh );
		code_l=  ADPCM_Encode( xl );
		(char)code[i] = (code_h<<4) | code_l ;
	}
	return i;

}


//convert to 8 bits PCM wave format
#define pad_overflowe8(x) ( x > 127 ? 127 : ( x<-128 ? -128 : x ) ) 

int  VoxCvt::Vox2Wave( char* buf_out, char* buf_in, int size )
{
	char*   wave = buf_out;
	char*   code = buf_in;
	int i;
	unsigned char code_h, code_l;
	short xh,xl;
	if ( size == 0 ) return 0;
	for ( i = 0; i< size; i++ ) {
		code_h = ( code[i]>>4 ) & 0x0f;
		code_l = ( code[i] ) & 0x0f;
		xh = ADPCM_Decode( code_h ) ;
		xl = ADPCM_Decode( code_l ) ;
		xh >>= 4; 
		xl >>= 4; 
		xh = pad_overflowe8( xh )-128;
		xl = pad_overflowe8( xl )-128;
		wave[ 2*i  ] = (char)xh;
		wave[ 2*i+1] = (char)xl;
	}
	return i*2;

}


/*
#define pad_overflowe16(x) ( (x>0)? ( x<2048 ? x*16:0x7fff) : ( x<-2047 ? 0x8fff:x*16 ) ) 

//convert to 16 bits PCM wave format
int  VoxCvt::Vox2Wave16( char* buf_out, char* buf_in, int size )
{
	short*  wave = (short*)buf_out;
	char*   code = buf_in;
	int i;
	unsigned char code_h, code_l;
	short xh,xl;
	if ( size==0 ) return 0;
	for ( i = 0; i< size; i++ ) {
		code_h = ( code[i]>>4 ) & 0x0f;
		code_l = ( code[i] ) & 0x0f;
		xh = ADPCM_Decode( code_h ) ;
		xl = ADPCM_Decode( code_l ) ;
		wave[ 2*i  ] = pad_overflowe16( xh );
		wave[ 2*i+1] = pad_overflowe16( xl );
	}
	return i*2*2;
}


*/


static  short MTable[] = {	-1, -1, -1, -1, +2, +4, +6, +8 };


//static short StepSize[49+1] = {0, 16, 17, 19, 21, 23, 25, 28, 31 ,34, 37, 41,
//    45,  50,  55,  60,  66, 73,
//	80, 88, 97, 107,118,130,143,157,173,190,209, 230, 253, 279, 307, 337,371,
//	408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1408,1552 
//};

static short StepSize[49+1] = {   
	                  0,   16,   17,   19,   21,   23,   25,   28,   31,
                     34,   37,   41,   45,   50,   55,   60,   66,   73,
                     80,   88,   97,  107,  118,  130,  143,  157,  173,
                    190,  209,  230,  253,  279,  307,  337,  371,  408,
                    449,  494,  544,  598,  658,  724,  796,  876,  963,
                   1060, 1166, 1282, 1411, 1552
};



short VoxCvt::ADPCM_Decode( unsigned char  code )
{	
	short ss = 0;
	short dn;

	ss = StepSize[ index ];

	dn = ss/8;
	if ( code & 0x01 ) 	dn += ss/4;
	if ( code & 0x02 ) 	dn += ss/2;
	if ( code & 0x04 ) 	dn += ss;
	if ( code & 0x08 )  dn = -dn;

	value += dn ;


	if ( value > 2047  ) value = 2047;
	else 
	if ( value < -2048 ) value = -2048; 


	// Calculation of Step Size
	index += MTable[ code & 0x07 ];
	if ( index < 1 ) index = 1;
	if ( index >49 ) index = 49;


	return value;
}

/*
char VoxCvt::ADPCM_Encode( short wave )
{
	short dn,  ss;
	char  code;

	dn = wave - value;

	ss = StepSize[ index ];

	code = 0;
	if ( dn < 0     ) { code |= 0x08; dn = -dn;   }
	if ( dn >= ss   ) { code |= 0x04; dn -= ss;   }
	if ( dn >= ss/2 ) { code |= 0x02; dn -= ss/2; }
	if ( dn >= ss/4 ) { code |= 0x01; }

	value = ADPCM_Decode( code );

	return (char)code;

}

*/

char VoxCvt::ADPCM_Encode( short wave )
{
	short dn,  ss;
	char  code;

	dn = wave - value;

	ss = StepSize[ index ];

	code = 0;
	if ( dn < 0     ) { code |= 0x08; dn = -dn;   }
	if ( dn >= ss   ) { code |= 0x04; dn -= ss;   }
	if ( dn >= ss/2 ) { code |= 0x02; dn -= ss/2; }
	if ( dn >= ss/4 ) { code |= 0x01; }

	dn = ss/8;
	if ( code & 0x01 ) 	dn += ss/4;
	if ( code & 0x02 ) 	dn += ss/2;
	if ( code & 0x04 ) 	dn += ss;
	if ( code & 0x08 )  dn = -dn;
	value += dn;
	if ( value > 2047  ) value = 2047;
	else 
	if ( value < -2048 ) value = -2048; 

	// Calculation of Step Size
	index += MTable[ code & 0x7 ];
	if ( index < 1 ) index = 1;
	if ( index >49 ) index = 49;

	return (char)code;
}


// Evenly distribute the q into n locations
void VoxCvt::FillGap(short *pGap, int q, int n, int start)
{
	int k, s, i, j;

	if (q<=0) return;
	s = (q > n/2 ? n/2 : n/q);
	k = n / s;
	for (i=0; i<s; i++) {
	  j = i*k+start;
	  while (pGap[j] && j<n-1) j++;
	  pGap[j] = 1;
	}
	FillGap( pGap, q-s, n, start+1 );
	return;
}


/*
** This routine converts from ulaw to 16 bit linear.
**
** Craig Reese: IDA/Supercomputing Research Center
** 29 September 1989
**
** References:
** 1) CCITT Recommendation G.711  (very difficult to follow)
** 2) MIL-STD-188-113,"Interoperability and Performance Standards
**     for Analog-to_Digital Conversion Techniques,"
**     17 February 1987
**
** Input: 8 bit ulaw sample
** Output: signed 16 bit linear sample
*/

//int ulaw2linear( unsigned char ulawbyte )
short muLawPCM_Decode( unsigned char  ulawbyte )
{
  static int exp_lut[8] = {0,132,396,924,1980,4092,8316,16764};
  int sign, exponent, mantissa, sample;

  ulawbyte = ~ulawbyte;
  sign = (ulawbyte & 0x80);
  exponent = (ulawbyte >> 4) & 0x07;
  mantissa = ulawbyte & 0x0F;
  sample = exp_lut[exponent] + (mantissa << (exponent + 3));
  if (sign != 0) sample = -sample;

  return(sample);
}

/*
** This routine converts from linear to ulaw
**
** Craig Reese: IDA/Supercomputing Research Center
** Joe Campbell: Department of Defense
** 29 September 1989
**
** References:
** 1) CCITT Recommendation G.711  (very difficult to follow)
** 2) "A New Digital Technique for Implementation of Any
**     Continuous PCM Companding Law," Villeret, Michel,
**     et al. 1973 IEEE Int. Conf. on Communications, Vol 1,
**     1973, pg. 11.12-11.17
** 3) MIL-STD-188-113,"Interoperability and Performance Standards
**     for Analog-to_Digital Conversion Techniques,"
**     17 February 1987
**
** Input: Signed 16 bit linear sample
** Output: 8 bit ulaw sample
*/

#define ZEROTRAP    /* turn on the trap as per the MIL-STD */
#define BIAS 0x84   /* define the add-in bias for 16 bit samples */
#define CLIP 32635

// unsigned char linear2ulaw(int sample)
unsigned char muLawPCM_Encode( int sample )
{
  static int exp_lut[256] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
                             4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                             5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                             5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};
  int sign, exponent, mantissa;
  unsigned char ulawbyte;

  /* Get the sample into sign-magnitude. */
  sign = (sample >> 8) & 0x80;		/* set aside the sign */
  if (sign != 0) sample = -sample;		/* get magnitude */
  if (sample > CLIP) sample = CLIP;		/* clip the magnitude */

  /* Convert from 16 bit linear to ulaw. */
  sample = sample + BIAS;
  exponent = exp_lut[(sample >> 7) & 0xFF];
  mantissa = (sample >> (exponent + 3)) & 0x0F;
  ulawbyte = ~(sign | (exponent << 4) | mantissa);
#ifdef ZEROTRAP
  if (ulawbyte == 0) ulawbyte = 0x02;	/* optional CCITT trap */
#endif

  return(ulawbyte);
}


//////////////////////////////////////////////////////////////////////////////////////////
typedef struct output_tag  // any special vars associated with output file
	{short nFile;         
	 long lSize;
	 DWORD dwDataOffset;
	 long lSamprate;
	 WORD wBitsPerSample;
	 WORD wChannels;
	 DWORD dwFormat;
	 BOOL bWroteHeader;
	 char szNAME[256];
	} MYOUTPUT;

typedef struct input_tag // any special vars associated with input file
{	short nFile;
	long lSize;    
	DWORD dwFormat;  
	WORD wChannels;
	DWORD dwDataOffset;		   
	WORD wBitsPerSample;
	char szNAME[256];
} MYINPUT;

int CVocCvt::CreatVox2WavAPI( HWND hWnd )
{
	return CreatCvtAPI( hWnd, "vox.dll", "wav.dll" );
}

int CVocCvt::CreatPcm2WavAPI( HWND hWnd )
{
	return CreatCvtAPI( hWnd, "pcm.dll", "wav.dll" );
}


int CVocCvt::ConvertVox2Wave( LPCTSTR lpszInFile, LPCTSTR lpszOutFile, 
					  LONG lSampleRateIn, LONG lSampleRateOut,
					  WORD wBitsPerSample , DWORD Offset
					)
{
	int retcode = 1;
	COOLQUERY cq;
	LONG  SampleRate;
	WORD  BitsPerSample;
	WORD  Channel;
	LONG  ChunkSizeIn, ChunkSizeOut, FileSize, ChunkSize;
	BYTE* Buf1=NULL;
	BYTE* Buf2=NULL;
	DWORD Counter;
	DWORD Option;
	int flag;
	double r = 1.0*lSampleRateIn/lSampleRateOut;
	double deltaValue;
	HANDLE Outfd = NULL;
	HANDLE Infd =  NULL;

	m_bCvtProcessAbort = FALSE;
	m_bCvtProcessQuit  = FALSE ; 

	flag = (*pfnQueryCoolFilter)( &cq );

	flag = (*pfnFilterUnderstandsFormat)( lpszInFile );
	if ( flag == 0 ) {
		// Vox file just check file name, so skip it
	   //	sprintf( (char*)m_szBuf, "Can't recognize format.");
	   //	retcode =  -1;
	   //	goto loc_end;
	};

	Channel = 1;
	SampleRate = lSampleRateIn;
	BitsPerSample = wBitsPerSample;
	Infd = (*pfnOpenFilterInput)( lpszInFile, &SampleRate, 
							&BitsPerSample, &Channel, 
							m_hWnd, &ChunkSizeIn );
	if ( Infd == NULL ) {
		sprintf( (char*)m_szBuf, "Can't open %s file.", lpszInFile );
		retcode =  -1;
		goto loc_end;
	};

	if ( wBitsPerSample != 16 ) wBitsPerSample = 8;

	Channel = 1;
	FileSize = pfnFilterGetFileSize( Infd )*lSampleRateOut/lSampleRateIn;;
	BitsPerSample = wBitsPerSample;
	Option = 00; //00:PCM WAVE; 01:CCITT u-law PCM
	if ( wBitsPerSample == 8 ) FileSize /= 2;   //default Vox.dll decode ADPCM to 16 bits
	Outfd = (*pfnOpenFilterOutput)( lpszOutFile, lSampleRateOut, 
							wBitsPerSample, Channel, FileSize,
							&ChunkSizeOut,  Option );


	if ( Outfd == NULL ) {
		sprintf( (char*)m_szBuf, "Can't open %s file.", lpszOutFile );
		retcode =  -1;
		goto loc_end;
	};

	ChunkSize = max( ChunkSizeIn,ChunkSizeOut ) ;

	Buf1 = (BYTE*)malloc( ChunkSize );
	if ( Buf1 == NULL ) {
		sprintf( (char*)m_szBuf, "Can't allocate memory.");
		retcode =  -1;
		goto loc_end;
	};

	Buf2 = (BYTE*)malloc( ChunkSize );
	if ( Buf2 == NULL ) {
		sprintf( (char*)m_szBuf, "Can't allocate memory.");
		retcode =  -1;
		goto loc_end;
	};


	ChunkSize = min( ChunkSizeIn,ChunkSizeOut ) ;
	short Value;
	int i, k;
	//int n=0, j, m ;
	r = 1.0*lSampleRateIn/lSampleRateOut;
	
	if ( r < 1 )	ChunkSize = (long)(ChunkSize*r) ;
	long size;
	if ( wBitsPerSample == 8 ) {
		SHORT* BufIn;
		CHAR* BufOut;
		BufIn = (SHORT*)Buf1; 
		BufOut= (CHAR*)Buf2;
		Counter = (*pfnReadFilterInput)(  Infd, (unsigned char*)BufIn,  Offset ); //skiping Header of file
		while ( 1 ) {
			Counter = (*pfnReadFilterInput)(  Infd, (unsigned char*)BufIn,  ChunkSize );
			if ( Counter==0 ) break;
			i=0; 
			while ( 1 ) {
				k =(int)(r*i);
				if ( k+1 >= (int)(Counter/sizeof(short)) - 1 ) break;
				deltaValue  = (r*i-k)*((short)BufIn[k+1]-(short)BufIn[k]);
				Value = (short)BufIn[k]+(short)deltaValue;;
				BufOut[i] = HIBYTE(Value)-0x7f;
				i++;
			}

			size = i;
			Counter = (*pfnWriteFilterOutput)( Outfd, (unsigned char*)BufOut, size  );
		}
	} else {
		short* BufIn;
		short* BufOut;
		BufIn = (short*)Buf1; 
		BufOut= (short*)Buf2;
		while ( 1 ) {
			if ( m_bCvtProcessAbort  ) {
				retcode = 0;
				goto loc_end;	
			}

			Counter = (*pfnReadFilterInput)(  Infd, (unsigned char*)BufIn,  ChunkSize );
			if ( Counter==0 ) break;
			i = 0;
			while ( 1 ) {
				k =(int)(r*i);
				if ( k+1 >= (int)(Counter/sizeof(short)) - 1 ) break;
				deltaValue  = (r*i-k)*((short)BufIn[k+1]-(short)BufIn[k]);
				Value = (short)BufIn[k]+(short)deltaValue;;
				BufOut[i] = Value;
				i++;
			}
			size = i*sizeof(short);
			Counter = (*pfnWriteFilterOutput)( Outfd, (unsigned char*)BufOut, size  );
		}
	}
loc_end:	
	if ( Infd !=  NULL )  (*pfnCloseFilterInput)( Infd );
	if ( Outfd != NULL )  (*pfnCloseFilterOutput)( Outfd );

	if ( Buf1 != NULL ) free( Buf1 );
	if ( Buf2 != NULL ) free( Buf2 );
	m_bCvtProcessQuit  = TRUE; 

	return retcode;
}


int CVocCvt::ConvertPcm2Wave( LPCTSTR lpszInFile, LPCTSTR lpszOutFile, 
					  LONG lSampleRateIn, LONG lSampleRateOut,
					  WORD wBitsPerSample , DWORD Offset
					)
{
	int retcode = 1;
	COOLQUERY cq;
	LONG  SampleRate;
	WORD  BitsPerSample;
	WORD  Channel;
	LONG  ChunkSizeIn, ChunkSizeOut, FileSize, ChunkSize;
	BYTE* Buf1=NULL;
	BYTE* Buf2=NULL;
	DWORD Counter;
	DWORD Option;
	int flag;
	HANDLE Outfd = NULL;
	HANDLE Infd =  NULL;

	m_bCvtProcessAbort = FALSE;
	m_bCvtProcessQuit  = FALSE ; 

	flag = (*pfnQueryCoolFilter)( &cq );

	//flag = (*pfnFilterUnderstandsFormat)( lpszInFile );
	//if ( flag == 0 ) {
		// Vox file just check file name, so skip it
	   //	sprintf( (char*)m_szBuf, "Can't recognize format.");
	   //	retcode =  -1;
	   //	goto loc_end;
	//};
	Channel = 1;
	SampleRate = lSampleRateIn;
	BitsPerSample = wBitsPerSample;
	Infd = (*pfnOpenFilterInput)( lpszInFile, &SampleRate, 
							&BitsPerSample, &Channel, 
							m_hWnd, &ChunkSizeIn );
	if ( Infd == NULL ) {
		sprintf( (char*)m_szBuf, "Can't open %s file.", lpszInFile );
		retcode =  -1;
		goto loc_end;
	};

	DWORD dwOldOption;
	Channel = 1;
	if ( SampleRate == 0 && pfnSetOptions != NULL )
		dwOldOption = (*pfnSetOptions)( Infd, 4, lSampleRateIn, Channel, wBitsPerSample );

	if ( wBitsPerSample != 16 ) wBitsPerSample = 8;

	Channel = 1;
	FileSize = pfnFilterGetFileSize( Infd )*lSampleRateOut/lSampleRateIn;;
	BitsPerSample = wBitsPerSample;
	Option = 01; //00:PCM WAVE; 01:CCITT u-law PCM
	Outfd = (*pfnOpenFilterOutput)( lpszOutFile, lSampleRateOut, 
							wBitsPerSample, Channel, FileSize,
							&ChunkSizeOut,  Option );


	if ( Outfd == NULL ) {
		sprintf( (char*)m_szBuf, "Can't open %s file.", lpszOutFile );
		retcode =  -1;
		goto loc_end;
	};

	ChunkSize = min( ChunkSizeIn,ChunkSizeOut ) ;

	Buf1 = (BYTE*)malloc( ChunkSize );
	if ( Buf1 == NULL ) {
		sprintf( (char*)m_szBuf, "Can't allocate memory.");
		retcode =  -1;
		goto loc_end;
	};


	CHAR* BufIn;
	CHAR* BufOut;
	BufIn = (CHAR*)Buf1; 
	BufOut= (CHAR*)Buf1;
	Counter = (*pfnReadFilterInput)(  Infd, (unsigned char*)BufIn,  Offset ); //skiping Header of file
	while ( 1 ) 
	{
		if ( m_bCvtProcessAbort  ) {
			retcode = 0;
			goto loc_end;	
		}
		Counter = (*pfnReadFilterInput)(  Infd, (unsigned char*)BufIn,  ChunkSize );
		if ( Counter==0 ) break;
		Counter = (*pfnWriteFilterOutput)( Outfd, (unsigned char*)BufOut, Counter  );
	}

loc_end:	
	if ( Infd !=  NULL )	(*pfnCloseFilterInput)( Infd );
	if ( Outfd != NULL )    (*pfnCloseFilterOutput)( Outfd );

	if ( Buf1 != NULL ) free( Buf1 );
	if ( Buf2 != NULL ) free( Buf2 );

	m_bCvtProcessQuit  = TRUE; 

	return retcode;
}



//call from another thread to cancel convert process
int  CVocCvt::Abort( BOOL bBlock )
{
	m_bCvtProcessAbort = TRUE;
	while ( bBlock && !m_bCvtProcessQuit ) {
		Sleep( 1000 );
	}
	if ( m_bCvtProcessQuit ) return 1;
	if ( bBlock ) return 0;
	return -1;
}

int  CVocCvt::CreatCvtAPI( HWND hWnd, LPCTSTR lpszIn, LPCTSTR lpszOut )         
{
	m_hWnd = hWnd;
	m_hHandleIn = LoadLibrary( lpszIn );
	m_hHandleOut= LoadLibrary( lpszOut );
	if ( m_hHandleIn == NULL || m_hHandleOut == NULL ) {
		if ( m_hHandleIn == NULL  ) sprintf( (char*)m_szBuf, "Can't Load libary %s", lpszIn  );
		if ( m_hHandleOut == NULL ) sprintf( (char*)m_szBuf, "Can't Load libary %s", lpszOut );
		FreeCvtAPI( );
		return -1;
	}
	(FnQueryCoolFilter)pfnQueryCoolFilter 
		= (FnQueryCoolFilter)GetProcAddress( m_hHandleIn, "QueryCoolFilter" );
	if ( pfnQueryCoolFilter == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get QueryCoolFilter function Entry");
		FreeCvtAPI( );
		return -1;
	}

	(FnFilterUnderstandsFormat)pfnFilterUnderstandsFormat 
		= (FnFilterUnderstandsFormat)GetProcAddress( m_hHandleIn, "FilterUnderstandsFormat" );
	if ( pfnFilterUnderstandsFormat == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get FilterUnderstandsFormat function Entry");
		FreeCvtAPI( );
		return -1;
	}

	(FnFilterGetFileSize)pfnFilterGetFileSize
		 =(FnFilterGetFileSize)GetProcAddress( m_hHandleIn, "FilterGetFileSize" );
	if ( pfnFilterGetFileSize == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get FilterGetFileSize function Entry");
		FreeCvtAPI( );
		return -1;
	}

	(FnOpenFilterInput)pfnOpenFilterInput			
		= (FnOpenFilterInput)GetProcAddress( m_hHandleIn, "OpenFilterInput" );
	if ( pfnOpenFilterInput == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get OpenFilterInput function Entry");
		FreeCvtAPI( );
		return -1;
	}


	(FnReadFilterInput)pfnReadFilterInput
		= (FnReadFilterInput)GetProcAddress( m_hHandleIn, "ReadFilterInput" );
	if ( pfnReadFilterInput == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get ReadFilterInput function Entry");
		FreeCvtAPI( );
		return -1;
	}


	(FnCloseFilterInput)pfnCloseFilterInput
		= (FnCloseFilterInput)GetProcAddress( m_hHandleIn, "CloseFilterInput" );
	if ( pfnCloseFilterInput == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get CloseFilterInput function Entry");
		FreeCvtAPI( );
		return -1;
	}

	(FnSetOptions)pfnSetOptions						  
		= (FnSetOptions)GetProcAddress( m_hHandleIn, "FilterSetOptions" );

	

	(FnOpenFilterOutput)pfnOpenFilterOutput			
		= (FnOpenFilterOutput)GetProcAddress( m_hHandleOut, "OpenFilterOutput" );
	if ( pfnOpenFilterOutput == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get OpenFilterOutput function Entry");
		FreeCvtAPI( );
		return -1;
	}

	(FnWriteFilterOutput)pfnWriteFilterOutput
		= (FnWriteFilterOutput)GetProcAddress( m_hHandleOut, "WriteFilterOutput" );
	if ( pfnWriteFilterOutput == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get WriteFilterOutput function Entry");
		FreeCvtAPI( );
		return -1;
	}


	(FnCloseFilterOutput)pfnCloseFilterOutput
		= (FnCloseFilterOutput)GetProcAddress( m_hHandleOut, "CloseFilterOutput" );
	if ( pfnCloseFilterOutput == NULL ) {
		sprintf( (char*)m_szBuf, "Can't Get CloseFilterOutput function Entry");
		FreeCvtAPI( );
		return -1;
	}

	return 1;
}		 						

void CVocCvt::FreeCvtAPI ( )                                         
{
	if ( m_hHandleIn == NULL  ) FreeLibrary( m_hHandleIn  );
	if ( m_hHandleOut == NULL ) FreeLibrary( m_hHandleOut );
}

int  CVocCvt::ReadRawInput(  HANDLE hInput, BYTE* lpbBuf, LONG Size )
{
	if (hInput==NULL) {
		sprintf( (char*)m_szBuf, "Invalid File Handle");
		return -1;
    }

	DWORD readed=0L;
	MYINPUT far *mo;
	 	
	mo=(MYINPUT far *)GlobalLock( hInput );
	readed = _lread(mo->nFile, (LPVOID)lpbBuf, Size);
	GlobalUnlock(hInput);

	return readed;
}

int  CVocCvt::WriteRawOutput(  HANDLE hOutput, BYTE* lpbBuf, LONG Size )
{
	if (hOutput==NULL) {
		sprintf( (char*)m_szBuf, "Invalid File Handle");
		return -1;
    }

	DWORD written=0L;
	MYOUTPUT far *mo;
	 	
	mo=(MYOUTPUT far *)GlobalLock( hOutput );
	written = _lwrite(mo->nFile, (LPCSTR)lpbBuf, Size);
	GlobalUnlock(hOutput);

	return written;
}

LONG CVocCvt::SeekPosOutput( HANDLE hOutput, LONG Offset, int Origin )
{
	if ( hOutput==NULL ) {
		sprintf( (char*)m_szBuf, "Invalid File Handle");
		return -1;
    }

	DWORD    pos;
	MYOUTPUT far *mo;
	mo=(MYOUTPUT far *)GlobalLock( hOutput );
	pos = _lseek( mo->nFile, Offset, Origin );	
	GlobalUnlock( hOutput );

	return pos;	
}

LONG CVocCvt::SeekPosInput( HANDLE hInput, LONG Offset, int Origin )
{
	if ( hInput==NULL ) {
		sprintf( (char*)m_szBuf, "Invalid File Handle");
		return -1;
    }

	DWORD    pos;
	MYOUTPUT far *mo;
	mo=(MYOUTPUT far *)GlobalLock( hInput );
	pos = _lseek( mo->nFile, Offset, Origin );	
	GlobalUnlock( hInput );

	return pos;	
}

/*
int CDlgDict::VocCvt( char* SrcPath, char* FileName, char* TempPath, char* Format, char** pMsg, int WaveBits )
{
	int flag;
	char drive[_MAX_DRIVE]={0}; char dir[_MAX_DIR]={0}; char ext[_MAX_EXT]={0};
	char fname[_MAX_FNAME]={0}; 
	char tar_path[_MAX_PATH]={0};
	static char Msg[_MAX_PATH+128]={0};
	int VoiceType;
	class CPcm2Wav pcm2wave;

	Msg[0] = 0x0;
	*pMsg = Msg;
	FileName[0] = 0;
	_splitpath( SrcPath, drive, dir, fname, ext );
	_makepath( (char*)tar_path, "", TempPath, fname, Format );

	if ( m_pVocCvtV2W == NULL )
	{
		sprintf( Msg, "Voice Converter is not avaliable"  ); 
		return -3;
	}
	
	VoiceType = GetVocFormat( SrcPath );
	if ( VoiceType < 0 )
	{
		if ( VoiceType == -1 )
			sprintf( Msg, "can't open file %s", SrcPath  ); 
		else
			sprintf( Msg, "file format is wrong %s", SrcPath  ); 
		return -2;
	}

	//cvt format 
	//****************** for demo only *****************
	switch ( VoiceType )
	{
	case 4:
	case 0:
			flag = m_pVocCvtV2W->ConvertVox2Wave( SrcPath, (char*)tar_path, 6000, 6000, WaveBits, sizeof(DFH) );
			if ( flag < 0 )
				sprintf( Msg, "Voice Converting (6000Hz 8bit VOX->Wave) failed! "  ); 

		break;
	case 5:
	case 1:
			flag = m_pVocCvtV2W->ConvertVox2Wave( SrcPath, (char*)tar_path, 8000, 8000, WaveBits, sizeof(DFH) );
			if ( flag < 0 )
				sprintf( Msg, "Voice Converting (8000Hz 8bit VOX->Wave) failed! "  ); 

		break;
	case 6:
	case 2:
			flag = pcm2wave.ConvertPcm2Wav( SrcPath, (char*)tar_path, 150, sizeof(DFH) );
			if ( flag < 0 )
				sprintf( Msg, "Voice Converting Dialogic PCM (6000Hz 8bit PCM->Wave) failed! "  ); 

		break;
	case 7:
	case 3:
			flag = pcm2wave.ConvertPcm2Wav( SrcPath, (char*)tar_path, 150, sizeof(DFH) );
			if ( flag < 0 )
				sprintf( Msg, "Voice Converting Dialogic PCM (8000Hz 8bit PCM->Wave) failed! "  ); 

		break;
	default: flag = -1;
		sprintf( Msg, "Voice Converter desn't support voc format (format id:%d)", VoiceType  ); 
	}

	if ( flag < 0 ) return flag;

	_makepath( (char*)FileName, "", "", fname, Format );

	return 1;
}
*/


// Basic Operation, Converting from Voc to Wav
int ConvertVox2Wave( LPCTSTR lpszInFile, LPCTSTR lpszOutFile, char* pszBuf, int nOpt ) 
{
	int flag, retcode = 1, nres;
	int BitsPerSampleIn, SampleRateIn;
	LONG  ChunkSizeIn, ChunkSizeOut;
	BYTE* Buf1=NULL;
	BYTE* Buf2=NULL;
	CWaveFile mWave;
	vox  mVox;
	VoxCvt mVoxCvt;
	CVocCvt mVoxCvt2;

	WAVEFORMATEX m_wfx;
	BOOL bS = false, fs = false;
	HWND hWnd;

/*
#define		OPT_AUTO_CONV			0x00000000
// ADPCM->WAV, If DLL exists, use DLL, otherwise use Algorithm
#define		OPT_UPCM_CONV			0x00000001
// uPCM->WAV,
#define		OPT_ADPCM_DLL			0x00000002
// Use DLL conv ADPCM to WAV
#define		OPT_ADPCM_ALGO			0x00000004
// Use Algorithm conv ADPCM to WAV
*/

	if ( ( nOpt == OPT_AUTO_CONV ) || ( nOpt & OPT_ADPCM_DLL ) )
	{
		hWnd = NULL; // ::AfxGetMainWnd()->m_hWnd;
		fs = ( mVoxCvt2.CreatVox2WavAPI( hWnd ) > 0 );
		if ( ( nOpt & OPT_ADPCM_DLL ) && !fs )
		{
			if (pszBuf!=NULL)
				sprintf( (char*)pszBuf, "Can't load DLL Library." );
			return -1;
		}
	}

	if ( mVox.Open( (char*)lpszInFile, READ_MODE, &BitsPerSampleIn, &SampleRateIn )<0 )
	{
		if ( pszBuf!=NULL )
			sprintf( (char*)pszBuf, "Can't open %s file.", lpszInFile );
		retcode = -1;
		goto loc_end;
	}

	if ( fs && ( mVox.vocType==0 ) )
	{
		mVox.Close();
		flag = mVoxCvt2.ConvertVox2Wave( lpszInFile, lpszOutFile, SampleRateIn, SampleRateIn, 8, sizeof(DFH) );
		if ( flag < 0 )	
		{
			if ( pszBuf!=NULL )
				sprintf( pszBuf, "Voice Converting w/ DLL ( ADPCM->Wave ) failed! "  );
			retcode = -1;
		}
		goto loc_end;
	}

	if ( (mVox.vocType==1) && (nOpt & OPT_UPCM_CONV) ) bS = true;

	if ( bS )
	{
		m_wfx.cbSize = 0;
		m_wfx.wFormatTag = WAVE_FORMAT_PCM;
		m_wfx.nChannels = 1;
		m_wfx.nSamplesPerSec = SampleRateIn;
		m_wfx.wBitsPerSample = 16;
		m_wfx.nBlockAlign = m_wfx.nChannels * m_wfx.wBitsPerSample / 8;
		m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;
	}
	else
	{
		m_wfx.cbSize = 0;
#ifdef	MULAW_SUPPORT
		m_wfx.wFormatTag = ( (mVox.vocType==1)?WAVE_FORMAT_UPCM:WAVE_FORMAT_PCM);
#else
		m_wfx.wFormatTag = WAVE_FORMAT_PCM;
#endif
		m_wfx.nChannels = 1;
		m_wfx.nSamplesPerSec = SampleRateIn;
		m_wfx.wBitsPerSample = 8;
		m_wfx.nBlockAlign = m_wfx.nChannels * m_wfx.wBitsPerSample / 8;
		m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;
	}

	if ( mWave.Open( (TCHAR*)lpszOutFile, WRITE_MODE, &m_wfx ) <= 0 )
	{
		if (pszBuf!=NULL)
			sprintf((LPTSTR)pszBuf, _T("Open File %s Failed!"), lpszOutFile);
		retcode = -1;
		goto loc_end;
	}

	ChunkSizeIn = mVox.fileSize;
	if ( ChunkSizeIn >= 100*1024*1024 ) ChunkSizeIn = 100*1024*1024; 
	if ( bS )
		ChunkSizeOut = ChunkSizeIn * 2; 
	else
		ChunkSizeOut = ChunkSizeIn * ( (BitsPerSampleIn==4)?2:1 ); 

	Buf1 = (BYTE*)malloc( ChunkSizeIn );
	if ( Buf1 == NULL ) {
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "Can't allocate memory.");
		retcode =  -1;
		goto loc_end;
	};

	Buf2 = (BYTE*)malloc( ChunkSizeOut );
	if ( Buf2 == NULL ) {
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "Can't allocate memory.");
		retcode = -1;
		goto loc_end;
	};

	if ( mVox.Read( (char*)Buf1, ChunkSizeIn) <= 0 ) {
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "File Read Error.");
		retcode =  -1;
		goto loc_end;
	}


	// Converting procedure
	if ( bS )  // Convert uPCM->PCM
	{
		short*  wave = (short*)Buf2;
		char*   code = (char*)Buf1;
		int i;
		for (i = 0; i<ChunkSizeIn; i++) 
			wave[i] = muLawPCM_Decode( code[i] );
		nres = 0;
	}
	else if ( mVox.vocType == 1 )  // Keep the uPCM
	{
#ifdef	MULAW_SUPPORT
		memcpy( Buf2, Buf1, ChunkSizeIn );
#else
		char*  wave = (char*)Buf2;
		char*   code = (char*)Buf1;
		int i;
		short tmp, max = 0;
		
		for (i = 0; i<ChunkSizeIn; i++) 
		{
			tmp = muLawPCM_Decode( code[i] ); 
			wave[i] = (unsigned char)((tmp >> 8) + 128);
		}
#endif
		nres = 0;
	}
	else  // Convert ADPCM->PCM
		nres = mVoxCvt.Vox2Wave( (char*)Buf2, (char*)Buf1, ChunkSizeIn );

	if (nres==0) retcode = 0;

	if ( mWave.Write( (char*)Buf2, ChunkSizeOut ) <= 0 ) {
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "File Write Error.");
		retcode =  -1;
		goto loc_end;
	}

loc_end:	
	mVox.Close();
	mWave.Close( );
	if ( Buf1 != NULL ) free( Buf1 );
	if ( Buf2 != NULL ) free( Buf2 );
	if ( fs )  mVoxCvt2.FreeCvtAPI();
	return retcode;
}


// Basic Operation, Converting from Vox(8K Data) or PCM (8K UPCM Data) to Wav
// nOpt=0 -> VOX
// nOpt=1 -> PCM
int ConvertVox2WaveSP( LPCTSTR lpszInFile, LPCTSTR lpszOutFile, char* pszBuf, int nOpt ) 
{
	int flag, retcode = 1, nres;
	int BitsPerSampleIn, SampleRateIn;
	LONG  ChunkSizeIn, ChunkSizeOut;
	BYTE* Buf1=NULL;
	BYTE* Buf2=NULL;
	CWaveFile mWave;
	vox  mVox;
	VoxCvt mVoxCvt;
	CVocCvt mVoxCvt2;

	WAVEFORMATEX m_wfx;
	BOOL bS = false, fs = false;
	HWND hWnd;

	hWnd = NULL; // ::AfxGetMainWnd()->m_hWnd;
	fs = ( mVoxCvt2.CreatVox2WavAPI( hWnd ) > 0 );
	if ( !fs )
	{
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "Can't load DLL Library." );
		return -1;
	}

	if ( mVox.Open( (char*)lpszInFile, READ_MODE, &BitsPerSampleIn, &SampleRateIn )<0 )
	{
		if ( pszBuf!=NULL )
			sprintf( (char*)pszBuf, "Can't open %s file.", lpszInFile );
		retcode = -1;
		goto loc_end;
	}
	else
	{
		if ( nOpt==0 )
		{
			mVox.vocType=1; 
			SampleRateIn=8000;
			BitsPerSampleIn=4;
			m_wfx.wFormatTag = WAVE_FORMAT_UPCM; // PCM
		}
		else
		{
			mVox.vocType=3; 
			SampleRateIn=8000;
			BitsPerSampleIn=8;
			m_wfx.wFormatTag = WAVE_FORMAT_UPCM;
		}
	}

	m_wfx.cbSize = 0;
	m_wfx.nChannels = 1;
	m_wfx.nSamplesPerSec = SampleRateIn;
	m_wfx.wBitsPerSample = 8;
	m_wfx.nBlockAlign = m_wfx.nChannels * m_wfx.wBitsPerSample / 8;
	m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;

	if ( mWave.Open( (TCHAR*)lpszOutFile, WRITE_MODE, &m_wfx ) <= 0 )
	{
		if (pszBuf!=NULL)
			sprintf((LPTSTR)pszBuf, _T("Open File %s Failed!"), lpszOutFile);
		retcode = -1;
		goto loc_end;
	}

	ChunkSizeIn = mVox.fileSize;
	if ( ChunkSizeIn >= 100*1024*1024 ) ChunkSizeIn = 100*1024*1024; 
	ChunkSizeOut = ChunkSizeIn * ( (BitsPerSampleIn==4)?2:1 ); 

	Buf1 = (BYTE*)malloc( ChunkSizeIn );
	if ( Buf1 == NULL ) {
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "Can't allocate memory.");
		retcode =  -1;
		goto loc_end;
	};

	Buf2 = (BYTE*)malloc( ChunkSizeOut );
	if ( Buf2 == NULL ) {
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "Can't allocate memory.");
		retcode = -1;
		goto loc_end;
	};

	if ( mVox.Read( (char*)Buf1, ChunkSizeIn) <= 0 ) {
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "File Read Error.");
		retcode =  -1;
		goto loc_end;
	}


	// Converting procedure
	if ( mVox.vocType == 3 )  // Keep the uPCM
	{
		memcpy( Buf2, Buf1, ChunkSizeIn );
		// nres = 0;
	}
	else 
	{
		// Convert ADPCM->PCM
		nres = mVoxCvt.Vox2Wave( (char*)Buf2, (char*)Buf1, ChunkSizeIn );
		// Convert PCM->UPCM
		char* wave = (char*)Buf2;
		unsigned char* code = (unsigned char*)Buf2;
		int i;
		short tmp;
		
		for (i = 0; i<ChunkSizeOut; i++) 
		{
			tmp = ((unsigned)wave[i]-128) << 8;
			// tmp = wave[i]<<8;
			code[i] = muLawPCM_Encode( tmp ); 
		}
	}
	if (nres==0) retcode = 0;

	if ( mWave.Write( (char*)Buf2, ChunkSizeOut ) <= 0 ) {
		if (pszBuf!=NULL)
			sprintf( (char*)pszBuf, "File Write Error.");
		retcode =  -1;
		goto loc_end;
	}

loc_end:	
	mVox.Close();
	mWave.Close( );
	if ( Buf1 != NULL ) free( Buf1 );
	if ( Buf2 != NULL ) free( Buf2 );
	if ( fs )  mVoxCvt2.FreeCvtAPI();
	return retcode;
}




