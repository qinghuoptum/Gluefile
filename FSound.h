#if !defined(AFX_WAVE_H__4AD31E57_A330_11D4_B5F0_00104B5F423D__INCLUDED_)
#define AFX_WAVE_H__4AD31E57_A330_11D4_B5F0_00104B5F423D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include  "stdafx.h"

#define  READ_MODE    0 
#define  WRITE_MODE	  1


#include  <stdio.h>
#include  <windows.h>
#include  <mmsystem.h>


typedef struct {
	char		ID[4];
	short		State;
	unsigned short VoiceType;
	long		DictLength;
	long		DictTime;
	long		CueLocation;
	long		InterruptLoc;
	char		Priority;
	char		reserve[3];
	char		NameFirst[16];
	char		NameLast[16];
} DFH;

#define	 DFHMARK	"DIC"
#define	 WAVE_FORMAT_UPCM	7

class vox 
{
	public:
		vox( );

	public:
		DFH	 VoxHeader;
		DWORD fileSize;
		int	 vocType;

	protected:
		int fd;
		int fileMode;

	public:
		int  Open ( char* name, int mode, int* bits, int* rate );
		int  Open( char* name, int mode );
		int  Close();

		int  Read ( char* buf, int size );
		int	 Write( char* buf, int size );
		int	 SeekPos( long pos );
		int  Eof(); 
};


#if !defined(FOURCC)
typedef DWORD FOURCC;
#endif

#if !defined(MAKEFOURCC)
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                      \
        ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
        ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

#if !defined(mmioFOURCC)
#define mmioFOURCC(ch0, ch1, ch2, ch3)  MAKEFOURCC(ch0, ch1, ch2, ch3)
#endif


#if !defined(MMCKINFO)
#endif

#if !defined(FOURCC_RIFF)

typedef struct _MMCKINFO
{ 
    FOURCC ckid; 
    DWORD  cksize; 
    FOURCC fccType; 
    DWORD  dwDataOffset; 
    DWORD  dwFlags; 
} MMCKINFO, FAR *LPMMCKINFO; 

#define	FOURCC_RIFF		MAKEFOURCC('R','I','F','F')

#endif 


BOOL GetChunk( CFile *fp, LPMMCKINFO lpck, UINT wFlags );
BOOL PutChunk( CFile *fp, LPMMCKINFO lpck, UINT wFlags );

#define		READ_MODE    0 
#define		WRITE_MODE	  1

#define		MSG_LEN		512


#define		POS_END			-1
#define		POS_PLAY		-2
#define		POS_WRITE		-3



class CWaveFile
{
	public:
		CWaveFile( );
		// WAVE		WaveHeader;
		// CHUNKDATA	WavePara;

		WAVEFORMATEX*	m_pwfx;          // Pointer to WAVEFORMATEX structure
		MMCKINFO	m_ckIO;				 // Multimedia RIFF chunk
		//MMIOINFO	m_mmioinfoOut;

	protected:
		CFile fd;
		int fileMode;
		MMCKINFO	m_ckIORiff;			 // Use in opening a WAVE file
		DWORD		m_dwPos;			 // Data position at begin action
		DWORD		m_dwPlayPos;		 //	Actual Played Position
		BOOL		openState;

		BOOL ReadMMIO( WAVEFORMATEX** ppwfxInfo );
		BOOL WriteMMIO( WAVEFORMATEX* pwfxDest );

		DWORD TimeToLen( DWORD nMilliseconds );
		DWORD LenToTime( DWORD nBytes );

		char m_szFname[MAX_PATH];
		char m_szMsg[MSG_LEN+1];

										 
	public:
		int  Open( TCHAR* name, int mode, WAVEFORMATEX *pwfx = NULL );
		int  Close();

		int  Read( char* buf,  int size);
		int	 Write( char* buf, int size);
		int	 SeekPos( long pos );
		int	 GetPos( long nType );
		int  SetPlayPos( long pos );
	 
		long  GoToStart();
		long  GoToEnd();

		long  DataLen( ) { return m_ckIO.cksize; };
		long  DataPos( ) { return m_dwPos ; };

		long  GetLenInTime( ); //measure file size to get length
		long  GetPosInPercent( ) {	return m_dwPos*100/m_ckIO.cksize; };
		long  GetPosInTime( long nType = POS_PLAY );
		void  SetPosInPercent( int nPerc );
		void  SetPosInTime( int nSecs );
		long  Forward( long nLast );
		long  Rewind( long nLast );


		WORD  Channel( ); //{ return m_pRec->m_wfx.nchannels; };
		WORD  Format( );  //{ return m_pRec->m_wfx.FormatTag; };
		DWORD SamplesRate( );   // { return m_pRec->m_wfx.nSamplesPerSec; };
		WORD  BitsPerSample( ); // { return m_pRec->m_wfx.wBitsPerSample; }

		char* ErrorMsg() { return m_szMsg; };
};



// VoxCvt.h: interface for the VoxCvt class.
//
//////////////////////////////////////////////////////////////////////

class VoxCvt  
{
public:
	VoxCvt();
	void Reset( );
	int  Wave2Vox( char* buf_out, char* buf_in, int size );
	int  Vox2Wave( char* buf_out, char* buf_in, int size );
	int  Wave2Vox16( char* buf_out, char* buf_in, int size );
	int  Vox2Wave16( char* buf_out, char* buf_in, int size );
	int  Vox2Wave16Ex( char* buf_out, char* buf_in, int size, 
					   int sampleRateOut, int sampleRateIn 	);


private:
	int   value;
	short index;

	short ADPCM_Decode( unsigned char  code );
	char  ADPCM_Encode( short value );
	int InterValue(int k, int n, short prev, short next, short *wave );

protected:
	void FillGap( short* pGap, int q, int n, int start );
};



//////////////////////////////////////////////////////
// GENERAL PURPOSE DEFS FOR CREATING CUSTOM FILTERS //
//////////////////////////////////////////////////////
//  VocCvt.h

typedef struct riffspecialdata_t
{	HANDLE hSpecialData;	
	HANDLE hData;			// Actual data handle
	DWORD  dwSize;			// size of data in handle
	DWORD  dwExtra;			// optional extra data (usually a count)
	char   szListType[8];	// Parent list type (usually "WAVE" or "INFO", or "adtl")
	char   szType[8];		// Usually a four character code for data, but can be up to 7 chars
} SPECIALDATA;

// "CUE " dwExtra=number of cues, each cue is 8 bytes		([4] name [4] sample offset)
// "LTXT" dwExtra=number of items, each one is 8 bytes		([4] ltxt len [4] name [4] cue length [4] purpose [n] data)
// "NOTE" dwExtra=number of strings, each one is n bytes	([4] name [n-4] length zero term)
// "LABL" dwExtra=number of strings, each one is n bytes	([4] name [n-4] length zero term)
// "PLST" dwExtra=number if items, each one is 16 bytes		([4] name [4] dwLen [4] dwLoops [4] dwMode)


// For special data, .FLT must implement FilterGetFirstSpecialData and FilterGetNextSpecialData


typedef DWORD           FOURCC;         // a four character code

struct cue_type { DWORD dwName;
				  DWORD dwPosition;
				  FOURCC fccChunk;
				  DWORD dwChunkStart;
				  DWORD dwBlockStart;
				  DWORD dwSampleOffset;
				 };

struct play_type {DWORD dwName;
					 DWORD dwLength;
					 DWORD dwLoops;
				};


typedef struct coolquery_tag
	{char szName[24];
	 char szCopyright[80];
	 
	 // rate table, bits are set for modes that can be handled
	 WORD Quad32;  // Quads are 3-D encoded
	 WORD Quad16;
	 WORD Quad8;
	 WORD Stereo8;    		// rates are from lowest bit:
	 WORD Stereo12;   		// bit 0 set: 5500 (5512.5)
	 WORD Stereo16;   		// bit 1 set: 11025 (11K)
	 WORD Stereo24;   		// bit 2 set: 22050 (22K)
	 WORD Stereo32;   		// bit 3 set: 32075 (32K, or 32000)
	 WORD Mono8;      		// bit 4 set: 44100 (44K)
	 WORD Mono12;	  		// bit 5 set: 48000 (48K)
	 WORD Mono16;	  		// bit 6 set: 88200 (88K)   (future ultra-sonic rates?)
	 WORD Mono24;	  		// bit 7 set: 96000 (96K)
	 WORD Mono32;     		// bit 8 set: 132300 (132K)
	 				  		// bit 9 set: 176400 (176K)
	 DWORD dwFlags;
	 char szExt[4];
	 long lChunkSize;
	 char szExt2[4];
	 char szExt3[4];
	 char szExt4[4];
	} COOLQUERY;

#define R_5500   1
#define R_11025  2
#define R_22050  4
#define R_32075  8
#define R_44100  16
#define R_48000  32
#define R_88200  64
#define R_96000  128
#define R_132300 256
#define R_176400 512

#define C_VALIDLIBRARY 1154

#define QF_RATEADJUSTABLE		0x001   // if can handle non-standard sample rates
										// if not, only rates in bit rate table understood
#define QF_CANSAVE				0x002		  
#define QF_CANLOAD				0x004
#define QF_UNDERSTANDSALL		0x008   // will read ANYTHING, so it is the last resort if no other
										// formats match
#define QF_READSPECIALFIRST		0x010	// read special info before trying to read data
#define QF_READSPECIALLAST		0x020	// read special info after reading data
#define QF_WRITESPECIALFIRST	0x040	// when writing a file, special info is sent to DLL before data
#define QF_WRITESPECIALLAST		0x080	// when writing, special info is sent to DLL after data
#define QF_HASOPTIONSBOX		0x100	// set if options box implemented
#define QF_NOASKFORCONVERT		0x200	// set to bypass asking for conversion if original in different rate, auto convert
#define QF_NOHEADER				0x400	// set if this is a raw data format with no header
#define QF_CANDO32BITFLOATS		0x800	// set if file format can handle 32-bit sample data for input
#define QF_CANOPENVIRTUAL		0x1000	// Set if data is in Intel 8-bit or 16-bit sample format, or floats
										// and the GetDataOffset() function is implemented

// special types are read from and written to DLL in the order below
/*
// special types (particular to Windows waveforms)
#define SP_IART  20
#define SP_ICMT  21
#define SP_ICOP  22
#define SP_ICRD  23
#define SP_IENG  24
#define SP_IGNR  25
#define SP_IKEY  26
#define SP_IMED  27
#define SP_INAM  28
#define SP_ISFT  29
#define SP_ISRC  30
#define SP_ITCH  31
#define SP_ISBJ  32
#define SP_ISRF  33
#define SP_DISP  34
#define SP_CUE   40 // returns number of cues of size cue_type 
#define SP_LTXT  41 // returns number of adtl texts of size 8 (4,id and 4,len)
#define SP_NOTE  42 // returns LO=size, HI=number of strings (sz sz sz...)
#define SP_LABL	 43 // returns LO=size, HI=number of strings (sz sz sz...)
#define SP_PLST  44 // returns number of playlist entries size play_type 
*/
///////////////////////////////////////////////////////////////////////////////

typedef	int  ( WINAPI EXPORT *FnQueryCoolFilter)( COOLQUERY* lpcq );
typedef	int  ( WINAPI EXPORT *FnFilterUnderstandsFormat )( LPCTSTR );
typedef long ( WINAPI EXPORT *FnFilterGetFileSize)(HANDLE hInput);

typedef	HANDLE ( WINAPI EXPORT * FnOpenFilterInput)( LPCTSTR lpFname, LONG* lplSampleRate, 
								   WORD* lpwBitsPerSample, WORD* lpwChannel, 
								   HWND hWnd, LONG* lplChunkSize );
typedef	HANDLE ( WINAPI EXPORT * FnOpenFilterOutput)( LPCTSTR lpFname, LONG lSampleRate, 
								   WORD wBitsPerSample, WORD wChannel, LONG lSize,
								   LONG* lplChunkSize, DWORD dwOptions );

typedef	DWORD ( WINAPI EXPORT * FnSetOptions)( HANDLE hInput, WORD Option, LONG lSamplerate,WORD wChannels, WORD wBPS  );

typedef	DWORD ( WINAPI EXPORT * FnReadFilterInput)( HANDLE hInput, BYTE* lpbBuf, LONG ChunkSize );
typedef	DWORD ( WINAPI EXPORT * FnWriteFilterOutput)( HANDLE hInput, BYTE* lpbBuf, LONG ChunkSize );

typedef	void  ( WINAPI EXPORT * FnCloseFilterInput)( HANDLE hInput );
typedef	void  ( WINAPI EXPORT * FnCloseFilterOutput)( HANDLE hInput );


class CVocCvt
{
public:
	int  CreatVox2WavAPI( HWND hWnd );
	int  ConvertVox2Wave( LPCTSTR lpszInFile, LPCTSTR lpszOutFile, 
					  LONG lSampleRateIn, LONG lSampleRateOut,
					  WORD wBitsPerSample ,DWORD Offset
					);

	int  CreatPcm2WavAPI( HWND hWnd );
	int  ConvertPcm2Wave( LPCTSTR lpszInFile, LPCTSTR lpszOutFile, 
					  LONG lSampleRateIn, LONG lSampleRateOut,
					  WORD wBitsPerSample ,DWORD Offset
					);



	void FreeCvtAPI ( );

	LPCTSTR ErrorMsg( ) { return (LPCTSTR)m_szBuf; };

	int  ReadRawInput(  HANDLE hInput, BYTE* lpbBuf, LONG Size );
	int  WriteRawOutput(  HANDLE hInput, BYTE* lpbBuf, LONG Size );
	LONG SeekPosOutput( HANDLE hInput, LONG Offset, int Origin );
	LONG SeekPosInput( HANDLE hInput, LONG Offset, int Origin );

	int  Abort( BOOL bBlock );

	BOOL m_bCvtProcessAbort;
	BOOL m_bCvtProcessQuit;

private:
	HINSTANCE m_hHandleIn ;
	HINSTANCE m_hHandleOut;
	HWND m_hWnd;
	char m_szBuf[128];

	int  CreatCvtAPI(HWND hWnd, LPCTSTR lpszIn, LPCTSTR lpszOut );

	FnQueryCoolFilter pfnQueryCoolFilter;
	FnFilterUnderstandsFormat pfnFilterUnderstandsFormat;

	FnFilterGetFileSize pfnFilterGetFileSize;

	FnSetOptions pfnSetOptions;

	FnOpenFilterInput  pfnOpenFilterInput;

	FnOpenFilterOutput pfnOpenFilterOutput;

	FnReadFilterInput  pfnReadFilterInput;
	FnWriteFilterOutput pfnWriteFilterOutput;

	FnCloseFilterInput  pfnCloseFilterInput;
	FnCloseFilterOutput pfnCloseFilterOutput;

};



#define		OPT_AUTO_CONV			0x00000000
// ADPCM->WAV, If DLL exists, use DLL, otherwise use Algorithm
#define		OPT_UPCM_CONV			0x00000001
// uPCM->WAV,
#define		OPT_ADPCM_DLL			0x00000002
// Use DLL conv ADPCM to WAV
#define		OPT_ADPCM_ALGO			0x00000004
// Use Algorithm conv ADPCM to WAV

int ConvertVox2Wave( LPCTSTR lpszInFile, LPCTSTR lpszOutFile, LPTSTR pszBuf = NULL, int opt = 0 ); 
int ConvertVox2WaveSP( LPCTSTR lpszInFile, LPCTSTR lpszOutFile, char* pszBuf, int nOpt=0 );


#endif // !defined(AFX_WAVE_H__4AD31E57_A330_11D4_B5F0_00104B5F423D__INCLUDED_)

