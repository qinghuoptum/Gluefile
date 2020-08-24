// WRTFDoc.cpp: implementation of the CWRTFDoc class.
//
//////////////////////////////////////////////////////////////////////

// #include "stdafx.h"

#include "PatParse.h"
#include "WRTFDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


int GetCTimeStr1( char* pszBuf, char* pszFmt, char* pszVal )
{
	// pszVal is yyyymmddhhmmss|mm/dd/yyyy hh:mm|Month day, year|System Time
	struct tm x;
	char *ps, szTmp[100], szT0[10];
	time_t t = 0;
	COleDateTime *pDt=NULL;
	bool ss;
	
	if ( pszVal[0]==0 )
	{
		pszBuf[0] = 0;
		return 0;
	}
	else
	{
		ps = pszVal;
		while ( ps[0]==' ' ) ++ps; ps[30]=0;
		strcpy( szTmp, ps );
	}

	if (strcmp(pszFmt,"T")==0)
	{
		// Ticks counter
		clock_t nclock = clock();
		if (nclock > 0x000FFFFF ) nclock &= 0x0FFFFF;
		nclock *= 1000;
		nclock /= CLOCKS_PER_SEC;
		sprintf( pszBuf, "%ld", nclock );
		return strlen(pszBuf);
	}

	x.tm_isdst = -1;
	if ( strspn(szTmp,"0123456789")==strlen(szTmp) )
	{
		strncpy( szT0, szTmp, 4); szT0[4]=0;
		if (strlen(szTmp)==9 || (strlen(szTmp)==10 && atoi(szT0)<1900) )
		{
			t = atol(szTmp);
			pDt = new COleDateTime( t );
			ss = true;
		}
		else
		{
			if (strlen( szTmp )>12) 
				x.tm_sec = atoi( szTmp+12);
			else 
				x.tm_sec = 0;
			szTmp[12]=0;
			if (strlen( szTmp )>10) 
				x.tm_min = atoi( szTmp+10);
			else 
				x.tm_min = 0;
			szTmp[10]=0;
			if (strlen( szTmp )>8) 
				x.tm_hour = atoi( szTmp+8);
			else 
				x.tm_hour = 0;
			szTmp[8]=0;
			if (strlen( szTmp )>6) 
				x.tm_mday = atoi( szTmp+6);
			else 
				x.tm_mday = 0;
			szTmp[6]=0;
			if (strlen( szTmp )>4) 
				x.tm_mon = atoi( szTmp+4);
			else 
				x.tm_mon = 0;
			szTmp[4]=0;
			if ( strlen( szTmp )>0 && atoi(szTmp)>1800 && atoi(szTmp)<2200 ) 
				x.tm_year = atoi( szTmp);
			else 
				x.tm_year = 2000;
			
			pDt = new COleDateTime( x.tm_year, x.tm_mon, x.tm_mday, x.tm_hour, x.tm_min, x.tm_sec );
			ss = (pDt!=NULL);
		}
	}
	else
	{
		pDt = new COleDateTime();
		ss = pDt->ParseDateTime( szTmp );
	}
	
	if ( ss && pDt!=NULL ) 
	{
		if (stricmp(pszFmt,"System")==0)
		{
			COleDateTime start(1970,1,1,0,0,0);
			COleDateTimeSpan ts = (*pDt)-start;
			sprintf(pszBuf, "%lu", ts.GetTotalSeconds() );
		}
		else
		{
			strncpy( pszBuf, pDt->Format(pszFmt), 100);
			if (strncmp(pszBuf,"Invalid", 7)==0) pszBuf[0]=0;
		}
	}

	if ( pDt!=NULL ) delete pDt;
	if ( ss ) return strlen( pszBuf );
	else return 0;

}


int GetFormatStr( char* pszBuf, char* pszFmt, char* pszVal )
{
	char *pFmt;
	if (strncmp(pszFmt, "%u", 2)==0)
	{
		strupr(pszVal);
		pFmt = pszFmt+2;
	}
	else if (strncmp(pszFmt, "%l", 2)==0)
	{
		strlwr(pszVal);
		pFmt = pszFmt+2;
	}
	else
		pFmt = pszFmt;
	sprintf( pszBuf, pFmt, pszVal );
	return strlen( pszBuf );
}


int GetFormatNum( char* pszBuf, char* pszFmt, char* pszVal )
{
	sprintf( pszBuf, pszFmt, atol(pszVal) );
	return strlen( pszBuf );
}

int GetScanfStr( char* pszBuf, char* pszFmt, char* pszVal )
{
	char szTemp[100];
	memset( szTemp, 0, 100 );
	sscanf( pszVal, pszFmt, szTemp );
	sprintf( pszBuf, szTemp );
	return strlen( pszBuf );
}


int FormatForExp2( char* pzKeyTarVal, char* pzTmp, char *pzFmt )
{
	char szVaTmp[256]={0}, szFmt[64], *ps, szWork[20]={0};
	
	ps = strchr( pzFmt, '|' );
	if ( ps==NULL ) return FormatForExp( pzKeyTarVal, pzTmp, pzFmt, szWork );

	strncpy( szFmt, pzFmt, (ps-pzFmt) );
	szFmt[ps-pzFmt]=0; ++ps;
	
	FormatForExp( szVaTmp, pzTmp, szFmt, szWork );
	return FormatForExp2( pzKeyTarVal, szVaTmp, ps );
}


int CHKFIELD( int i, LPTSTR ps )
{
	int k=0;
	int ss=0;

	if (ps[i]=='<')
	{
		if ( ps[i+1]=='*' || (strncmp(ps+i+1,"start",5)==0) || (strncmp(ps+i+1,"end",3)==0) ) return 2;
		k++;
		while ( ps[i+k]>0 && ps[i+k]<=' ' ) ++k;
		if ( ps[i+k]=='*' || (strncmp(ps+i+k,"start",5)==0) || (strncmp(ps+i+k,"end",3)==0) ) return k+1;
		if ( ps[i+k]!='\\' ) return 0;
		
		while  ( ps[i+k]=='\\' )
		{
			while ( ps[i+k]>' ' ) ++k;
			while ( ps[i+k]>0 && ps[i+k]<=' ' ) ++k;
			if ( ps[i+k]=='*' || (strncmp(ps+i+k,"start",5)==0) || (strncmp(ps+i+k,"end",3)==0))
			{
				ss=k+1; break;
			}
			if ( ps[i+k]!='\\' ) break;
		}
	}
	return ss;
}	

/* ( m_szBuf[i]=='<' && ( (m_szBuf[i+1]=='*') || (strncmp(m_szBuf+i+1,"start",5)==0) \
								|| (strncmp(m_szBuf+i+1,"end",3)==0) ) ) 
*/


bool CHKHIDDEN( int i, LPTSTR ps )
{
	int k=0;
	bool ss=false;

	if (ps[i]=='{')
	{
		if ( ps[i+1]=='{' )	return ss;
		++k;

		while  ( ps[i+k] )
		{
			while ( ps[i+k]>0 && ps[i+k]<=' ' ) ++k;
			if ( ( ps[i+k] && ps[i+k]!='\\' ) || ps[i+k]=='}' || ps[i+k]=='{') break;
			while ( ps[i+k] && ps[i+k]!='  ' && ps[i+k]!='{' && ps[i+k]!='}' ) 
			{
				ss |= ( ps[i+k]=='\\' && ps[i+k+1]=='v' && 
					( ps[i+k+2]=='\\' || ps[i+k+2]<=' ' ));
				if (++k>2096) break;
			}
			if ( k>=2096 ) { ss=false; break; }
		}

		/* while (ps[i+k]>0 && ( (ps[i+k]!=' ') || (ps[i+k]==' ' && ps[i+k+1]=='\\') 
			   || (ps[i+k]==' ' && ps[i+k+1]=='\r' && ps[i+k+2]=='\n' && ps[i+k+3]=='\\') 
			   || (ps[i+k]==' ' && ps[i+k+1]=='\r' && ps[i+k+2]=='\r' && ps[i+k+3]=='\n' && ps[i+k+4]=='\\') ) )
		{
			ss |= ( ps[i+k]=='\\' && ps[i+k+1]=='v' && 
				( ps[i+k+2]=='\\' || ps[i+k+2]<=' ' ));
			if ( ps[i+k]=='}' ) break;
			k++;
		} */
	}
	else if ( ps[i]=='\\' )
	{
		while (ps[i+k]>' ' && ps[i+k]!='{' && ps[i+k]!='}' )
		{
			ss |= (ps[i+k]=='\\' && ps[i+k+1]=='v' && 
				   ( (ps[i+k+2]=='1' && (ps[i+k+3]=='\\' || ps[i+k+3]<=' ')) || 
				     ps[i+k+2]==' ' || ps[i+k+2]=='\\' ) ); 
			k++;
			if (k>2096) break;
		}
	}
	return ss;
}


bool CHKV0MARK( int i, LPTSTR ps )
{
	int k=0;
	bool ss=false;

	if ( ps[i]=='\\' )
	{
		while (ps[i+k]>0 && ps[i+k]!=' ')
		{
			ss |= (ps[i+k]=='\\' && ps[i+k+1]=='v' && ps[i+k+2]=='0');
			k++;
			if (ss) break;
			if (k>1024) break;
		}
	}
	return ss;
}


int KEEPOTHERTAG( int i, LPTSTR ps, long& nLmt )
{
	bool ss=false, ts=false; 
	int k, n=0, p=0;
	
	k = i;
	while ( k<nLmt && ps[k]>' ' ) 
	{ 
		if ( ps[k]=='\\' && ps[k+1]=='v' && !isalpha(ps[k+2]) ) 
		{ 
			// ps[k+1] = (ts?'0':'*'); p=k+1;
			// Eliminate \v
			if (ps[k+2]=='1' || ps[k+2]=='0') n=3; else n=2; 
			memmove(ps+k, ps+k+n, nLmt-k-n );
			ps[nLmt-n]=0;
			nLmt-=n;
		}
		ss |= ( ps[k]=='\\' && isalpha(ps[k+1]) && ps[k+1]!='v' );
		/*if ( strncmp(ps+k, "\\protect", 8 )==0 || strncmp(ps+k, "\\tab", 4 )==0) 
		{
			ts=true; if ( p>0 ) ps[p]='0';
		}*/
		if ( ps[k]==' ' ) break;
		++k; 
	}
	if ( k<nLmt && ss ) return ++k;
	return i;
}

/*-------------------------------------------------------------------------
	Search a string in RTF environment
	GvnLen - >0: actual length (or end with \space, \tab, \par, }, {)
	  	    =0:	start point
	ActPos = Actual Start Point
	ps = current position
	qs = string value
	retuen: Length from ps
----------------------------------------------------------------------------*/

int SCANFIELD( LPTSTR ps, int GvnLen,  int& ActPos, LPTSTR qs=NULL, int nOpt=0 )
{
	char szTmp[1024]={0};
	int k=0, nn=0;
	bool ss, first=true;

	while ( ps[k]>0 && ps[k]<' ' ) ++k;	if ( ps[k]==' ' ) ++k;
	ActPos = k;

	while ( ps[k] )
	{
		if ( ps[k]!='\\'  )
		{
			if ( first ) { ActPos = k; first=false; }
			if ( ( GvnLen==0 && ps[k]==' ' ) || ( ps[k]=='{' || ps[k]=='}' ) ) break;
			if ( ps[k]>=' ' ) szTmp[nn++] = ps[k++]; else k++;
			if ( nn>=GvnLen ) break;
		}
		else
		{
			ss = false;
			while  ( ps[k]=='\\' )
			{
				if ( strnicmp( ps+k, "\\par", 4 )==0 ||
					 strnicmp( ps+k, "\\tab", 4 )==0 ||
					 strnicmp( ps+k, "\\cell", 5 )==0 ) { ss=true; break; }
				++k;
				if ( ps[k]=='\\' ) { ++k; szTmp[nn++]='\\'; continue; }
				while ( ps[k]>' ' && ps[k]!='{' && ps[k]!='}' && strnicmp( ps+k, "\\par", 4 ) 
					&& strnicmp( ps+k, "\\tab", 4 ) && strnicmp( ps+k, "\\cell", 5 ) ) ++k;

				while ( ps[k]>0 && ps[k]<' ' ) ++k;	if ( ps[k]==' ' ) ++k;
			}
			if (ss) break;
		}
	}
	szTmp[nn]=0; 
	if (qs!=NULL && GvnLen>0 ) strcpy( qs, szTmp );
	return k;
}

  
bool SCANTOBLK( LPTSTR ps, int& SpcLn,  int& ActPos )
{
	int j, k = 0;
	bool ss = false;
	char *qs, szTmp[200]={0};

	while ( ps[k] )
	{
		if ( ps[k]=='\\' )
		{
			if ( ps[k+1]=='{' || ps[k+1]=='}' ) { ActPos=k; ss=true; break; } 
			j=0; ++k;
			while ( ps[k]>' ' && ps[k]!='\\' && ps[k]!='{' ) szTmp[j++]=ps[k++];
			szTmp[j]=0;
			if ( ps[k]==' ' ) ++k;
			if (  strcmp(szTmp, "par")==0 || strcmp(szTmp, "cell")==0 )
			{
				ActPos=k; break;
			}
		}
		else if ( ps[k]=='{' || ps[k]=='}' ) k++;
		else if ( ps[k]==' ' )
		{
			SpcLn++; k++;
		}
		else if ( ps[k]<' ' ) ++k;
		else { ActPos=k; ss=true; break; }
	}
	return ss;
}

//////////////////////////////////////////////////////////////////////
//		Construction/Destruction
//////////////////////////////////////////////////////////////////////


CWRTFDoc::CWRTFDoc()
{
	m_szBuf = NULL;
}


CWRTFDoc::~CWRTFDoc()
{
//	if ( m_res ) m_file.Close();
	if ( m_szBuf!=NULL ) delete m_szBuf;
}


BOOL CWRTFDoc::ReadFile( LPCTSTR pszFileName )
{
	m_res = m_file.Open( pszFileName, CFile::modeRead|CFile::shareDenyWrite );
	if (!m_res) return FALSE;

	m_total = m_file.GetLength();
	if (m_szBuf!=NULL) delete m_szBuf; 
	m_limit = m_total + 160000;
	m_szBuf = ( LPTSTR ) new CHAR [m_limit];
	m_total = m_file.Read( m_szBuf, m_total ); 
	m_file.Close();
	if (strncmp( m_szBuf, "{\\rtf", 5 )==0) return m_res;
	else
	{
		delete m_szBuf;
		m_szBuf=NULL;
		return false;
	}
}


void CWRTFDoc::WriteFile( LPCTSTR pszFileName )
{
	if ( !m_res ) return;
	bool ss = false;
	int nn = 0;

	while ( !ss )
	{
		ss = (m_file.Open( pszFileName, 
			CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite)>0);
		if ( !ss )
		{
			Sleep( 1000 ); ++nn;
			if (nn>=10) break;
		}
	}
	
	if ( ss )
	{
		m_file.Write( m_szBuf, m_total );
		m_file.Close();
		if ( m_szBuf!=NULL ) delete m_szBuf;
		m_szBuf = NULL;
		m_res = FALSE;
	}
	return;
}


void CWRTFDoc::ExportFile( LPCTSTR pszFileName )
{
	if ( !m_res ) return;
	int i, nn = 0;
	FILE *fp;

	fp = fopen( pszFileName, "w" );
	if ( fp!=NULL )
	{
		i=0;
		while ( m_szBuf[i] )
		{
			if ( m_szBuf[i]=='{' && i>0 )
			{
				fputc( '\n', fp ); nn = 0;
				fputc( '{', fp ); ++nn;
			}
			else if ( m_szBuf[i]=='}' )
			{
				fputc( '}', fp );
				fputc( '\n', fp ); nn =0;
			}
			else if ( m_szBuf[i]=='\n' )
			{
				if ( nn>120 ) { fputc( '\n', fp); nn=0; }
			}
			else if ( m_szBuf[i]!='\r' )
			{
				fputc( m_szBuf[i], fp ); ++nn;
			}
			++i;
		}
		fclose( fp );
	}
	return;
}


void CWRTFDoc::ProcessFile( LPCTSTR pszFileName, char chOpt, LPTSTR pzParm )
{
	char szKey[200], szVal[4000];

	if ( ReadFile( pszFileName ) )
	{
		if ( chOpt=='r' || chOpt=='R' )
		{
			ReplaceFields( pzParm );
		}
		else if ( chOpt=='c' || chOpt=='C' )
		{
			ExtendFunction( "", RTF_EXTFUNC_RMVLSTLN );
		}
		else if ( chOpt=='i' || chOpt=='I' )
		{
			szKey[0]=0; szVal[0]=0;
			sscanf( pzParm, "%[^=]%*c%[^\n]", szKey, szVal );
			InsertSignPic( szVal, szKey );
		}
		WriteFile( pszFileName );
	}
}

/*#define		CHKHIDDEN(i)	( ( m_szBuf[i]=='{' && m_szBuf[i+1]=='\\' && m_szBuf[i+2]=='v' && \
							    (m_szBuf[i+3]==' '||m_szBuf[i+3]=='\\') ) || \
							  (	m_szBuf[i]=='{' && m_szBuf[i+1]=='\r' && m_szBuf[i+2]=='\n' && m_szBuf[i+3]=='\\' \
								&& m_szBuf[i+4]=='v' && (m_szBuf[i+5]==' '||m_szBuf[i+5]=='\\') ) || \
							  (	m_szBuf[i]=='{' && m_szBuf[i+1]=='\r' && m_szBuf[i+2]=='\\' \
								&& m_szBuf[i+3]=='v' && (m_szBuf[i+4]==' '||m_szBuf[i+4]=='\\') ) || \
							  ( m_szBuf[i]=='\\' && m_szBuf[i+1]=='v' && (m_szBuf[i+2]=='1' ||m_szBuf[i+2]==' ')) )
*/


#define		ENDHIDDEN(i)	( ( m_szBuf[i-2]=='\\' && m_szBuf[i-1]=='v' && m_szBuf[i]=='0' && ch=='\\') || \
							  ( m_szBuf[i]=='}' && ch=='{' ) )
/*
#define		CHKFIELD(i)		( m_szBuf[i]=='<' && ( (m_szBuf[i+1]=='*') || (strncmp(m_szBuf+i+1,"start",5)==0) \
								|| (strncmp(m_szBuf+i+1,"end",3)==0) ) ) 
*/

#define		CHECKHF(i)		( strncmp(m_szBuf+i,"{\\header", 8)==0 || strncmp(m_szBuf+i,"{\\footer", 8)==0 )

// [/nnn/mmm ..... ] marking a RTF code	and should be replace with {\nnn\mmm  ... }
#define		CHKMARK(i)		( m_szBuf[i]=='[' && m_szBuf[i+1]=='/' && m_szBuf[i+2]!=' ' )


// \r \n is for DOS text file newline
void CWRTFDoc::HiddenFields()
{
	long i = 0, k, kk;
	long nLmt, nLev, nTmp;
	long nFldsStart, nFldsLen;
	CHAR szTmp[512];

	if ( !m_res ) return;

	nLmt = m_total;

	while ( i < nLmt )
	{
		if ( CHKMARK(i) )
		{
			k=i; nLev=1;
			do 
			{
				k++;
				if (m_szBuf[k]=='[') nLev++;
				if (m_szBuf[k]==']') nLev--;
			}
			while ( !(m_szBuf[k]==']' && nLev==0) && (k<nLmt) );
			
			if (( m_szBuf[k]==']' && nLev==0 ) && ( k<nLmt ))
			{
				m_szBuf[i]='{'; m_szBuf[i+1]='\\';
				while ( i<nLmt && m_szBuf[i]>' ' )
				{
					if ( m_szBuf[i]=='/' ) m_szBuf[i]='\\';
					++i;
				}
				if ( k<nLmt ) m_szBuf[k]='}';
			}
			else
				++i;
		}
		else if (CHKHIDDEN(i,m_szBuf))
		{
			// Alread in Hidden area, find the tail
			nLev=1;
			do 
			{
				i++;
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
			}
			while ( !(m_szBuf[i]=='}' && nLev==0) && (i<nLmt) ); 
			i++;
		}
		else if (CHKFIELD( i, m_szBuf )>0)
		{
			// Get the Field, find the tail and change the contents
			nFldsStart = i;
			nFldsLen = 1;
			do  
			{	i++; nFldsLen++; }
			while ( m_szBuf[i]!='>' && (nFldsLen<500) && (i<nLmt) ); 
			if (m_szBuf[i]=='>')
			{
				strcpy(szTmp,"{\\v ");
				strncpy(szTmp+4, m_szBuf+nFldsStart, nFldsLen);
				kk = nFldsLen+3;
				szTmp[++kk]='}';
				/* szTmp[++kk]='\\';
				szTmp[++kk]='i';
				szTmp[++kk]='n';
				szTmp[++kk]='s';
				szTmp[++kk]='r';
				szTmp[++kk]='s';
				szTmp[++kk]='i';
				szTmp[++kk]='d';
				szTmp[++kk]='0';
				szTmp[++kk]=' '; */
				szTmp[++kk]=0;
				nTmp = ReplaceOneFlds( nFldsStart, nFldsLen, kk, nLmt, szTmp );
				if ( nTmp )
				{
					nLmt = nTmp;
					i = nFldsStart+nFldsLen+5;
				}
			}
			else 
				i++;
		}
		else
			i++;
	}
	m_total = nLmt;
	return ;
}


#define MAX_SPACE	70
#define MAX_BUF1	64000




void CWRTFDoc::ClearFields( int nOpt )
{
	long k, i=0, h, j = 0;
	long nLmt, nLev;
	long nFldsStart, nFldsLen;
	CHAR ch;
	bool ss;


	if ( !m_res ) return;

	nLmt = m_total;
	while ( i < nLmt )
	{
		if ( (CHKHIDDEN(i,m_szBuf)) && (nOpt & (RTF_CLS_HIDE|RTF_CLS_FLDS)))
		{

			// Get the Start Point of Hidden Text Section
			// Looking for the end of this hidden section
			ch = m_szBuf[i];
			if (ch=='{') nLev=1; else nLev=0;

			// Keep other tag beside \v
			k = i;
			KEEPOTHERTAG( i, m_szBuf, nLmt );
			//i = KEEPOTHERTAG( i, m_szBuf, nLmt );
			//if ( k==i ) k=0;
			nFldsStart = i;
			nFldsLen = 0;
			do 
			{
				ss |= (m_szBuf[i]=='<' && m_szBuf[i+1]=='*');
				nFldsLen ++; i++;
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
			}
			while ( !( ENDHIDDEN(i) && nLev==0 ) && (i<nLmt) ); 
			
			h=0;
			if ( m_szBuf[i]=='0' && m_szBuf[i-3]=='0')
			{
				// considering \protect0\v0
				h = i-3; while ( m_szBuf[h]!=' ' && m_szBuf[h]!='\\' ) --h;
				if ( m_szBuf[h]=='\\' )
				{
					memmove( m_szBuf+h+3, m_szBuf+h, i-h-2 );
					m_szBuf[h]='\\'; m_szBuf[h+1]='v'; m_szBuf[h+2]='0';
					i=h+3;
				}
				else
					h=0;
			}
			
			if (ch!='{' && h==0) { i++; if ( m_szBuf[i]<=' ' ) ++i; } 

			if (/*k==0 && */ch=='{' ) ++i;
			if (i>=nLmt) break;

			if ( (nOpt & RTF_CLS_HIDE) || ss )
			{
				memmove( m_szBuf+nFldsStart, m_szBuf+i, nLmt-i );
				nLmt -= ( i-nFldsStart );
				i = nFldsStart;
			}
			
		}
		else if  ( ((CHECKHF(i)) && (nOpt & RTF_CLS_HF)) ||
				   ((strncmp(m_szBuf+i,"{\\pict",6)==0) && (nOpt & RTF_CLS_PICT)) )
		{
			// Get the Field, find the tail and change the contents
			nFldsStart = i;
			nFldsLen = 1;
			nLev=1;
			do 
			{
				nFldsLen++; i++;
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
			}
			while ( !(m_szBuf[i]=='}' && nLev==0) && (i<nLmt) ); 
			i++;
			memmove( m_szBuf+nFldsStart, m_szBuf+i, nLmt-i+1 );
			i = nFldsStart;
			nLmt -= nFldsLen;
		}
		else if ( m_szBuf[i]==' ' && (nOpt & RTF_CLS_SPC) )
		{
			j = i; k = 1;
			while ( i<nLmt && m_szBuf[++i]>0 && m_szBuf[i]<=32 ) ++k;
			if ( k > MAX_SPACE )
			{
				memmove( m_szBuf+j+1, m_szBuf+i, nLmt-i );
				nLmt -= (i-j-1);
				i = j+1;
			}
			else
			{
			}
			if (j-i>1) --i;
		}
		else
			i++;
	}
	m_total = nLmt;
	return ;
}

	
void CWRTFDoc::ClearFieldsA(LPCTSTR pszFileName, int nOpt )
{

	ReadFile( pszFileName ); 
	ClearFields( nOpt );
	WriteFile( pszFileName );

	/*
	FILE *fp, *tp;
	TCHAR m_szBuf[MAX_BUF1+100]={0}, szDir[256]={0}, szFile[256]={0};
	long i = 0;
	long nn, spk, nLmt, nLev;
	bool s1, s2;
	CHAR ch;

	fp = fopen( pszFileName, "rb" );
	if (fp==NULL) return;

	GetTempPath(256, szDir);
	if (GetTempFileName(szDir, "GF__", 0, szFile))
		tp=fopen(szFile, "w+b");
	else
		tp=NULL;
	// tp = tmpfile();

	if (tp==NULL)
	{
		fclose( fp );
		return;
	}
	s1 = true; s2 = true;

	while (!feof(fp))
	{
		nn = fread(m_szBuf, 1, MAX_BUF1, fp);
		m_szBuf[nn]=0;
		if (nn == MAX_BUF1) 
		{
			nn -= 10;
			fseek(fp, -10L, SEEK_CUR );
		}

		nLmt = nn; i = 0;
		while ( i < nLmt )
		{
			if (s1 && s2 && (CHKHIDDEN(i,m_szBuf)) && (nOpt & RTF_CLS_HIDE))
			{
				s1 = false;
				ch = m_szBuf[i];
				// if (ch=='{') nLev=1; else nLev=0;
				nLev = 0;
			}

			if ( s1 && s2 &&  ( ((CHECKHF(i)) && (nOpt & RTF_CLS_HF)) || 
					((strncmp(m_szBuf+i,"{\\pict",6)==0) && (nOpt & RTF_CLS_PICT)) ) )
			{
				s2 = false;
				nLev=0;
			}

			if ( s1 && s2 )
			{
				if ( s1 && s2 && (nOpt & RTF_CLS_SPC))
				{
					if ( m_szBuf[i]>0 && m_szBuf[i]<=32 ) spk++; else spk=0;
				}
				else
					spk = 0;

				if ( spk<MAX_SPACE ) fputc( m_szBuf[i], tp );
			}

			if ( !s1 )
			{
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
				if ( (ENDHIDDEN(i)) && nLev==0 ) s1 = true;
			}
			if ( !s2 )
			{
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
				if ( (m_szBuf[i]=='}' && nLev==0) ) s2 = true; 
			}
			i++;
		}
	}
	fclose( fp );
	rewind( tp );

	fp = fopen( pszFileName, "wb" );
	if ( fp!=NULL)
	{
		while ((nn=fread(m_szBuf,1,MAX_BUF1,tp))>0)
			fwrite(m_szBuf,1,nn,fp);
		fclose( fp );
	}

	//_rmtmp();
	fclose(tp);
	DeleteFile( szFile );
	*/
}


#define		SCANONEITEM(p,q,x,t)	x=0; if ( p[0]=='"' || p[0]=='\'' ) x=(p++)[0];\
while((p[0])&&(x?(p[0]!=x):(p[0]!=t)))(q++)[0]=(p++)[0]; \
q[0]=0; while (p[0]==t||(x?p[0]==x:x)||p[0]==' '||p[0]=='\t') ++p;


int strmulcmp(LPTSTR pzSrc, LPTSTR *przTgt, int *pnItem)
{
	int i=0, ss=-1;

	while ( przTgt[i]!=NULL )
		if ( strncmp(pzSrc, przTgt[i], pnItem[i])==0 ) { ss=i; break; }
		else ++i;
	return ss;
}


//
// In place format adjustment, target string will be
// $L:<Locator>,  Locate the operation position
// $S:<SrchObj>,  Search string for remove or replace
// $P:<Prefix>,   Prefix appending String
// $I:<Insider>,  Inside token within the search string
// $R:<Replace>,  Replacement string 
int CWRTFDoc::FormatAdjustEx( LPCTSTR pszTgtStr )
{
	TCHAR szLocator[128]={0}, szSrchObj[128]={0}, szPrefix[128]={0};
	TCHAR szInsider[128]={0}, szReplace[128]={0};
	
	TCHAR *pzInsider[10]; 
	int nIsdItem[10];
	TCHAR ch, *ps, *qs;
	bool bs;

	long i = 0, k, nLev, nn, mm, l, nLmt;
	long nStart, nEnd;
	TCHAR *pszTmp;
	bool ss = false;


	if ( (ps=(TCHAR*)strstr( pszTgtStr, "$L:" ))!=NULL )
	{
		ps+=3; qs=szLocator; SCANONEITEM(ps,qs,ch,',')
	}
	if ( (ps=(TCHAR*)strstr( pszTgtStr, "$S:" ))!=NULL )
	{
		ps+=3; qs=szSrchObj; SCANONEITEM(ps,qs,ch,',')
	}
	if ( (ps=(TCHAR*)strstr( pszTgtStr, "$P:" ))!=NULL )
	{
		ps+=3; qs=szPrefix; SCANONEITEM(ps,qs,ch,',')
	}

	if ( (ps=(TCHAR*)strstr( pszTgtStr, "$R:" ))!=NULL )
	{
		ps+=3; qs=szReplace; SCANONEITEM(ps,qs,ch,',')
	}

	if ( (ps=(TCHAR*)strstr( pszTgtStr, "$I:" ))!=NULL )
	{
		ps+=3; qs=szInsider; SCANONEITEM(ps,qs,ch,',')
	}
	// Multiple Insider processing
	ps = szInsider; 
	for (i=0; i<10; i++) { pzInsider[i]=NULL; nIsdItem[i]=0; }
	i = 0; bs = true;
	while ( ps[0] )
	{
		if ( bs ) { if (i<10) pzInsider[i++]=ps; bs=false; }
		if ( ps[0]=='^' ) { ps[0]=0; bs=true; }
		++ps;
	}
	for (i=0; i<10; i++) 
		if ( pzInsider[i]!=NULL ) nIsdItem[i]=strlen(pzInsider[i]);

	if ( !m_res ) return 0;
	nn = strlen(szLocator); mm = strlen(szSrchObj); 
	ss = (nn==0);
	nLmt = m_total;
	while ( i < nLmt )
	{
		if (!ss) ss |= (strncmp(m_szBuf+i, szLocator, nn)==0 );

		if (ss)
		{
			if ( strncmp(m_szBuf+i, szSrchObj, mm)==0 )
			{
				// To find the search string
				nStart = i; nLev=0; i++;
				while ( (i < nLmt) && ( nLev>0 || m_szBuf[i]!='}' ) ) 
				{
					if ( m_szBuf[i]=='{' ) nLev++;
					if ( m_szBuf[i]=='}' ) nLev--;
					++i;
				}
				nEnd = (i<nLmt?++i:i); ss = false;
				
				pszTmp = new TCHAR[nEnd-nStart+100];
				qs = pszTmp;  qs[0]=0;
				if ( szPrefix[0] ) strcat( pszTmp, szPrefix );
				qs += strlen(qs);

				// To obtain the insider codes
				k = nStart;
				while ( k<nEnd && pzInsider[0]!=NULL )
				{
					if ( strmulcmp(m_szBuf+k, pzInsider, nIsdItem)>=0 )
					{
						nLev=0; (qs++)[0]=m_szBuf[k++];
						while ( (k<nEnd) && ( nLev>0 || m_szBuf[k]!='}' ) ) 
						{
							if ( m_szBuf[k]=='{' ) nLev++;
							if ( m_szBuf[k]=='}' ) nLev--;
							(qs++)[0]=m_szBuf[k++];
						}
						(qs++)[0]=m_szBuf[k++];
						qs[0]=0;
					}
					else
						++k;
				}

				// Append the replacement
				if ( szReplace[0] ) strcat( pszTmp, szReplace );

				l = strlen( pszTmp );
				memmove( m_szBuf+nStart+l, m_szBuf+nEnd, nLmt-nEnd );
				nLmt += (l-(nEnd-nStart));
				memmove( m_szBuf+nStart, pszTmp, l );
				delete pszTmp;
			}
			else
				i++;
		}
		else
			i++;
	}
	m_total = nLmt;
	return nLmt;
}


void CWRTFDoc::FormatAdjust(LPCTSTR pszFileName, int nOpt )
{
	FILE *fp, *tp;
	TCHAR m_szBuf[MAX_BUF1+100]={0}, szDir[256]={0}, szFile[256]={0};
	long i = 0, k, nLev;
	long nn, nLmt;
	bool s1, s2;

	fp = fopen( pszFileName, "rb" );
	if (fp==NULL) return;

	GetTempPath(256, szDir);
	if (GetTempFileName(szDir, "GF__", 0, szFile))
		tp=fopen(szFile, "w+b");
	else
		tp=NULL;
	// tp = tmpfile();

	if (tp==NULL)
	{
		fclose( fp );
		return;
	}

	s1 = false; 
	s2 = false;

	while (!feof(fp))
	{
		nn = fread(m_szBuf, 1, MAX_BUF1, fp);
		m_szBuf[nn]=0;
		if (nn == MAX_BUF1) 
		{
			nn -= 10;
			fseek(fp, -10L, SEEK_CUR );
		}

		nLmt = nn; i = 0;
		while ( i < nLmt )
		{
			if ( nOpt & RTF_FMTADJ_CTR )
			{
				if (strncmp(m_szBuf+i,"\\pard",5)==0) // change \tqc to \qc
				{
					k = i+5; s1 = false;
					while ( k<nLmt && m_szBuf[k]!='{' && m_szBuf[k]!='}' )
					{
						if (strncmp(m_szBuf+k,"\\tqc",4)==0)
						{
							s1 = true; 
							memmove(m_szBuf+k+1, m_szBuf+k+2, nLmt-(k+2));
							m_szBuf[nLmt-1]=0; --nLmt;
						}
						++k;
					}
					i = k;
				}
				else if ( s1 ) // Removing \tab
				{
					if (strncmp(m_szBuf+i," \\tab", 5)==0 || strncmp(m_szBuf+i,"\\tab ", 5)==0)
					{
						memmove(m_szBuf+i, m_szBuf+i+5, nLmt-(i+5));
						m_szBuf[nLmt-5]=0; nLmt -= 5;
					}
				}
			}

			if ( nOpt & RTF_FMTADJ_CLS )
			{
				if ( strncmp(m_szBuf+i,"{\\fonttbl",9)==0 ||
					 strncmp(m_szBuf+i,"{\\info",6)==0 ||
					 strncmp(m_szBuf+i,"{\\colortbl",10)==0 ||
					 strncmp(m_szBuf+i,"{\\stylesheet",12)==0 )
				{
					k = i;
					nLev=0; i++;
					while ( m_szBuf[i] && ( nLev>0 || m_szBuf[i]!='}' ) ) 
					{
						if ( m_szBuf[i]=='{' ) nLev++;
						if ( m_szBuf[i]=='}' ) nLev--;
						++i;
					}
					if ( m_szBuf[i]=='}' )
					{
						memmove(m_szBuf+k, m_szBuf+i+1, nLmt-(i+1-k));
						nLmt -= (i+1-k); i=(k-1);
					}
					else i=k;
				}
			}

			i++;
		}
		fwrite(m_szBuf, 1, nLmt, tp);
	}
	fclose( fp );
	rewind( tp );

	fp = fopen( pszFileName, "wb" );
	if ( fp!=NULL)
	{
		while ((nn=fread(m_szBuf,1,MAX_BUF1,tp))>0)
			fwrite(m_szBuf,1,nn,fp);
		fclose( fp );
	}

	//_rmtmp();
	fclose(tp);
	DeleteFile( szFile );
}



int CWRTFDoc::ExtendFunction( LPTSTR pszKey, int nOpt )
{
	long nLmt, i = 0, nLev = 0;
	int k, j, nn, nRes = 0;
	bool ss, inHF;

	int nFont=-1;
	char szTmp[16]={0};
	bool bBody = false;

	if ( !m_res ) return -1;

	nn = strlen( pszKey );
	nLmt = m_total;
	inHF = false;

	if ( nOpt & RTF_EXTFUNC_RMVLSTLN )
	{
		i = m_total; --i; nLev = 0;
		while ( i>0 && m_szBuf[i] != '\n' ) 
		{
			if ( m_szBuf[i]=='}' ) ++nLev;
			if ( m_szBuf[i]=='{' ) --nLev;
			--i;
		}
		if ( i>0 ) 
		{
			m_szBuf[++i]=' '; 
			if (nLev>0) for ( k=0; k<nLev; k++) m_szBuf[++i]='}'; 
			m_szBuf[++i]=0;
			m_total = i;
		}
		return 1;
	}

	while ( i < nLmt ) 
	{
		if ( m_szBuf[i]=='{' && (i==0 || m_szBuf[i-1]!='\\')) 
		{
			++nLev;
			if ( (strncmp(m_szBuf+i, "{\\header", 8 )==0 ||
				  strncmp(m_szBuf+i, "{\\footer", 8 )==0) && nLev==2 ) inHF = true;
		}
		else if ( m_szBuf[i]=='}' && m_szBuf[i-1]!='\\' ) 
		{
			--nLev;
			if ( nLev==1 && inHF ) inHF = false;
		}
		
		if ( ( nOpt & RTF_EXTFUNC_RMVREST ) && (strncmp( m_szBuf+i, pszKey, nn ) == 0) )
		{
			i += nn;
			for ( k=0; k<nLev; k++ ) m_szBuf[i++]='}';
			nLmt = i; nRes = nLmt;
			break;
		}
		else if ( ( nOpt & RTF_EXTFUNC_KWORDCHK ) && m_szBuf[i]==pszKey[0] )
		{
			if (strncmp( m_szBuf+i, pszKey, nn )==0 )
			{
				nRes++; i += nn;
			}
		}
		else if ( ( nOpt & (RTF_EXTFUNC_RMVSPACE | RTF_EXTFUNC_CHKSPACE) ) && (!inHF) && m_szBuf[i]==' ' )
		{
			int kk, kn, p;
			j = i; k = 1;
			if ( pszKey==NULL || atoi(pszKey)==0 ) kk=0; else kk = atoi(pszKey); 

			while ( i<nLmt && m_szBuf[++i]==' ' ) ++k;

			if (kk>0) 
			{
				if (k>kk)
				{
					if ( nOpt & RTF_EXTFUNC_RMVSPACE )
					{
						memmove( m_szBuf+j+1, m_szBuf+i, nLmt-i );
						nLmt -= (i-j-1);
					}
					else
					{
						nRes = k;
						break;
					}
				}
				else
					i=j+k-1;
			}
			else
			{
				if (k>5)
				{
					kn = k; 
					if (!SCANTOBLK( m_szBuf+i, kn, p ))
					{
						if ( nOpt & RTF_EXTFUNC_RMVSPACE )
						{
							memmove( m_szBuf+j+1, m_szBuf+i, nLmt-i );
							nLmt -= (i-j-1);
						}
						else
						{
							nRes = kn;
							break;
						}
					}
					else
						i=j+k-1;
				}
				else
					i=j+k-1;
			}
		}
		else if ( ( nOpt & RTF_EXTFUNC_BLNKCHK ) && m_szBuf[i]=='[' )
		{
			++i; ss = true;
			while ( m_szBuf[i] )
			{
				if ( m_szBuf[i]=='{' ) { ++i; ++nLev; }
				else if ( m_szBuf[i]=='}' ) { ++i; --nLev; }
				else if ( m_szBuf[i]=='\\' )
				{
					++i; while ( m_szBuf[i]!=' ' && m_szBuf[i]!='\\' && m_szBuf[i]!='{' && m_szBuf[i]!='}' ) ++i;
					if ( m_szBuf[i]==' ' ) ++i;
				}
				if ( m_szBuf[i]==']' ) break;
				if ( m_szBuf[i]!='_' && m_szBuf[i]!='*' ) ss = false;
				++i;
			}
			if ( ss ) nRes++;
		}
		else if ( ( nOpt & RTF_EXTFUNC_RMVKEYW) && (strncmp( m_szBuf+i, pszKey, nn ) == 0) )
		{
			memmove( m_szBuf+i, m_szBuf+i+nn, nLmt-i-nn );
			nLmt -= nn;
		}
		else if ( nOpt & RTF_EXTFUNC_RMVSPEC1 )
		{
			nn = 0;
			//if (  strncmp( m_szBuf+i, "{\\info", 6)==0 || strncmp( m_szBuf+i, "{\\header", 8)==0 ||
			//	  strncmp( m_szBuf+i, "{\\footer", 8)==0 )
				  bBody = true;
			if ( bBody && (strncmp(m_szBuf+i, "\\f", 2)==0) && isdigit( m_szBuf[i+2] ) )
			{
				nFont = atoi( m_szBuf+i+2 );
			}
			else if ( bBody && strncmp( m_szBuf+i, "\\afs", 4)==0 && isdigit( m_szBuf[i+4] ) )
			{
				k=4; while ( isdigit(m_szBuf[i+k]) ) ++k;
				if ( m_szBuf[i+k]==' ' && (m_szBuf[i+k+1]=='\\' || m_szBuf[i+k+1]<32 || m_szBuf[i-1]=='{' )) ++k;
				memmove( m_szBuf+i, m_szBuf+i+k, nLmt-i-k );
				nLmt -= k; ++nRes; --i;
			}
			else if ( bBody && strncmp( m_szBuf+i, "\\af", 3)==0 && isdigit( m_szBuf[i+3] ) )
			{
				// Remove all
				k=3; while ( isdigit(m_szBuf[i+k]) ) ++k;
				if ( m_szBuf[i+k]==' ' && (m_szBuf[i+k+1]=='\\' || m_szBuf[i+k+1]<32 || 
					 m_szBuf[i-1]=='{')) ++k;
				memmove( m_szBuf+i, m_szBuf+i+k, nLmt-i-k );
				nLmt -= k; ++nRes; --i;
			}
			else if ( bBody && strncmp( m_szBuf+i, "\\ab", 3)==0 && (m_szBuf[i+3]=='\\' || m_szBuf[i+3]<=32))
			{
				k=3;
				if ( m_szBuf[i+k]==' ' && (m_szBuf[i+k+1]=='\\' || m_szBuf[i+k+1]<32 || m_szBuf[i-1]=='{')) ++k;
				memmove( m_szBuf+i, m_szBuf+i+k, nLmt-i-k );
				nLmt -= k; ++nRes;	--i;
			}
			else if ( bBody && strncmp( m_szBuf+i, "\\rtlch", 6)==0 )
			{
				if ( m_szBuf[i+6]==' ' && (m_szBuf[i+7]=='\\' || m_szBuf[i+7]<32 || m_szBuf[i-1]=='{')) nn = 7; else nn = 6;
				memmove( m_szBuf+i, m_szBuf+i+nn, nLmt-i-nn );
				nLmt -= (nn); ++nRes;  --i;
			}
			else if ( bBody && strncmp( m_szBuf+i, "\\deleted", 8)==0 )
			{
				if ( m_szBuf[i+8]==' ' && (m_szBuf[i+9]=='\\' || m_szBuf[i+9]<32 || m_szBuf[i-1]=='{')) nn = 9; else nn = 8;
				memmove( m_szBuf+i, m_szBuf+i+nn, nLmt-i-nn );
				nLmt -= (nn); ++nRes; --i;
			}
		}
		++i;
	}
	m_szBuf[nLmt]=0;
	m_total = nLmt;
	return nRes;
}

	


void CWRTFDoc::SetProtect( int nOpt )
{
	long nLmt, i = 0;
	bool ss;

	if ( !m_res ) return;

	ss = true;
	nLmt = m_total;
	while ( (i < nLmt) && ss )
	{
		if ( nOpt )
		{
			if (strncmp( m_szBuf+i, "\\widowctrl", 10 )==0)
			{
				while (m_szBuf[i]>0 && m_szBuf[i]!=' ' && m_szBuf[i]!='\t')
				{
					if (strncmp(m_szBuf+i, "\\formprot", 9)==0)
					{
						ss = false;
						break;
					}
					else
						++i;
				}
				if (m_szBuf[i]>0)
				{
					memmove( m_szBuf+i+9, m_szBuf+i, nLmt-i+1 );
					memcpy(m_szBuf+i,"\\formprot",9);
					ss = false;
					nLmt += 9;
					i += 9;
				}
			}
			else
				++i;
		}
		else 
		{
			if (strncmp(m_szBuf+i, "\\formprot", 9)==0)
			{
				memmove( m_szBuf+i, m_szBuf+i+9, nLmt-i-9+1 );
				nLmt -= 9;
			}
			else
				++i;
		}
	}
	m_szBuf[nLmt]=0;
	m_total = nLmt;
	return ;
}

	
long CWRTFDoc::ReplaceOneFlds(long nPos, long nOL, long nNL, long nLmt, LPTSTR psTmp)
{
				
	if ( nLmt + (nNL-nOL) < m_limit ) 
	{
		memmove( m_szBuf+nPos+nNL, m_szBuf+nPos+nOL, nLmt-(nPos+nOL) );
		memcpy( m_szBuf+nPos, psTmp, nNL ); 
		return (nLmt+(nNL-nOL));
	}
	else
		return 0;
}


// Replace the RTF Body with pszParam (it is xxx=vvv|yyy=uuu|...)
// if nOnce & RTF_ACT_ONCE, RTF Body may contains multiples <*xxx>, but it replace one each time
//    and this <*xxx> will not be replaced next time
int CWRTFDoc::ReplaceFields( LPTSTR pszParam, int nOnce )
{
	long i = 0, k = 0, j, nn = 0, mm;
	long nLmt, nLev, nTmp;
	long nFldsStart, nFldsLen;
	TCHAR szTmp[1200], szFmt[200], ch, ch0;
	LPTSTR psV1, psV2, ps, pos, pos1, pos2;

	TCHAR szKeyName[200], ch1, ch2;
	TCHAR szKeyTarVal[1200];
	TCHAR szParam[24000], *pms;
	int nKeyLen, nValLen, nKVSrcLen, nKVTarLen, nK, pn, qn;
	unsigned int ux;

	if ( !m_res ) return 0;

	if ( nOnce & RTF_ACT_ONCE )
	{
		strncpy( szParam, pszParam, sizeof(szParam));
		pms = szParam;
	}
	else
		pms = pszParam;

	if ((nOnce & RTF_ACT_SHOW_KEY) || (nOnce & RTF_ACT_SHOW_VAL))
		pms[0]=0;

	nLmt = m_total;
	while ( i < nLmt )
	{
		if (CHKHIDDEN(i,m_szBuf))
		{
			// Get the Start Point of Hidden Text Section

			// Looking for the end of this hidden section
			nFldsStart = i;
			nFldsLen = 1;
			ch = m_szBuf[i];
			if (ch=='{') nLev=1; else nLev=0;
			do 
			{
				nFldsLen ++; i++;
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
			}
			while ( !( ENDHIDDEN(i) && nLev==0 ) && (i<nLmt) );
			
			i++;
			if ( ch!='{' && m_szBuf[i]==' ' ) i++;
			while (m_szBuf[i]=='\n'||m_szBuf[i]=='\r') i++;
			
			if ( (m_szBuf[i]!='{' || !CHKHIDDEN(i,m_szBuf)) && ch=='{' )
			{
				if ( m_szBuf[i]=='{' ) i++;
				// i++;
				while (m_szBuf[i]=='\n'||m_szBuf[i]=='\r') i++;
				while ( m_szBuf[i]=='\\' && ( m_szBuf[i+1]!='\'' && m_szBuf[i+1]!='_' && m_szBuf[i+1]!='{' && m_szBuf[i+1]!='}' 
					&& strncmp(m_szBuf+i+1,"par",3) && strncmp(m_szBuf+i+1,"tab",3) && strncmp(m_szBuf+i+1,"cell",4)))
				{
					while (m_szBuf[i] && m_szBuf[i]!=' ') i++;
					i++;
					while ( m_szBuf[i]=='\n' || m_szBuf[i]=='\r' ) i++;
				}
			}

			if (i>=nLmt) break;
			// So far, (nFldsStart, nFldsLen) give out a Hidden Section, i -> next pos

			// For RTF_ACT_SHOW_KEY,
			if ( nOnce & RTF_ACT_SHOW_KEY )
			{
				strncpy(szTmp, m_szBuf+nFldsStart, nFldsLen);
				szTmp[nFldsLen]=0;
				strcat( pms, szTmp ); strcat( pms, "\r\n" );
				goto Loop;
			}

			// For RTF_ACT_FIX, try to fix it properly
			if ( nOnce & RTF_ACT_FIX )
			{
				strncpy(szTmp, m_szBuf+nFldsStart, nFldsLen);
				szTmp[nFldsLen]=0;

				if ( (pos = strstr( szTmp, "<*" ))==NULL )
				{	// Hidden Text without a Field, Cancel it
					pos1 = strstr( szTmp, "\\v" );
					if (pos1!=NULL)
					{
						pos1[1]='*'; // \* is a mark for null
						strncpy( m_szBuf+nFldsStart, szTmp, nFldsLen );
					}
				}
				else
				{	// Hidden Text with the	Field, check any garbage
					pos1 = strchr( pos, '>' );
					if (pos1==NULL)
					{	// Incomplete Field definition, cancel it,
						pos1 = strstr( szTmp, "\\v" );
						if (pos1!=NULL)
						{
							pos1[1]='*'; // \* is a mark for null
							strncpy( m_szBuf+nFldsStart, szTmp, nFldsLen );
						}
					}
					else
					{
						// Removal of garbage inside the field
						ps = pos;
						while ( pos[0] && pos[0]!='>' )
						{
							if ( pos[0]=='\\' )
							{
								while ( pos[0] && pos[0]!=' ' ) ++pos;
								if ( pos[0]==' ' ) ++pos;
							}
							if ( pos[0]==0 || pos[0]=='>' ) break;
							(ps++)[0]=(pos++)[0];
						}
						if ( pos[0]=='>' ) (ps++)[0] = (pos++)[0]; 
						while ( ps<pos ) (ps++)[0]=' '; 

						// Check the heads before the Field
						pos = szTmp;
						while ( pos[0] && pos[0]!=' ') ++pos;
						++pos;
						if ( strncmp(pos, "<*",2) && ((pos2=strstr(pos,"<*"))!=NULL))
						{
							// Some hedden stuff exist before the field, move them out
							strncpy( szKeyTarVal, pos, (pos2-pos));
							strncpy( szKeyTarVal+(pos2-pos), szTmp, (pos-szTmp));
							strncpy( szTmp, szKeyTarVal, (pos2-szTmp));
							strncpy( m_szBuf+nFldsStart, szTmp, nFldsLen );
						}

						// Check the tails after the Field
						++pos1;
						while ( pos1[0] && pos1[0]<=' ') ++pos1;
						if ( pos1[0] && pos1[0]!='}' && !CHKV0MARK(0,pos1) )
						{	
							// Some hidden stuff exist after the field, move them out
							// In this case, we need search some code after this 
							// hidden text
							k=0;
							while (pos1[0] && pos1[0]!='}' && !CHKV0MARK(0,pos1) )
							{	
								szKeyTarVal[k++]=pos1[0];
								pos1[0]=' '; ++pos1;
							}
							szKeyTarVal[k]=0;
							// szKeyTarVal is the rest content after the field
							strncpy( m_szBuf+nFldsStart, szTmp, nFldsLen );

							if (m_szBuf[i]=='{'||m_szBuf[i]=='\\')
							{
								while (m_szBuf[i]!=' ' && i<nLmt) i++;
							}
							if (m_szBuf[i]==' ') i++;
				
							// Add on the rest content to normal text
							nTmp = ReplaceOneFlds( i, 0, k, nLmt, szKeyTarVal );
							if (nTmp ) nLmt = nTmp;
						}
					}
				}

				goto Loop;  // no more operations for fixing
			}

			if (i>=nLmt) break;
			// So far, (nFldsStart, nFldsLen) give out a Hidden Section, i -> next pos

			// Parse Key String within this Hidden Section to get all parameters
			k= nFldsStart;
			while (! ((pn=CHKFIELD(k, m_szBuf))>0) && (k < nFldsStart+nFldsLen)) k++;
			if ( k >= nFldsStart+nFldsLen )  goto Loop;
			
			// Removal of garbage inside the field
			pos = m_szBuf+k+pn; ps = pos;
			while ( pos[0] && pos[0]!='>' && pos[0]!='}' )
			{
				if ( pos[0]=='\\' )
				{
					while ( pos[0] && pos[0]!=' ' ) ++pos;
					if ( pos[0]==' ' ) ++pos;
				}
				if ( pos[0]==0 || pos[0]=='>' || pos[0]=='}' ) break;
				(ps++)[0]=(pos++)[0];
			}
			if ( pos[0]=='>' ) (ps++)[0] = (pos++)[0]; 
			while ( ps<pos ) (ps++)[0]=' '; 

			ch=' ';	ch0=' '; ch1=' '; ch2=' '; 
			szKeyName[0]=0;	nKeyLen=0;

			// Need more detail parsing for <*KeyName dd d d> or <*KeyName dd d :=Format String>
			/* sscanf( m_szBuf+k+2,"%199[^> ]%c%d %c%*c%c%c",
					szKeyName, &ch0, &nKeyLen, &ch1, &ch2, &ch );
			if (ch0=='>' || (ch!='>' && ch!='=' ) || nKeyLen<0 || nKeyLen>99 ) goto Loop;
			pos = m_szBuf+(k+2)+strlen(szKeyName)+1;
			while ( pos[0] && pos[0]<=' ' ) ++pos;
			pos1 = pos + 7; 
			/*/
			pos = m_szBuf+k+pn; j=0;
			while ( pos[0] && j<199 && pos[0]>' ' && pos[0]!='>' ) szKeyName[j++]=(pos++)[0];
			szKeyName[j]=0; if (pos[0]=='>' || j>=199 ) goto Loop;
			while (pos[0] && pos[0]<=' ') ++pos;  if (pos[0]==0) goto Loop;
			pos1 = pos;
			nKeyLen = atoi(pos1); if ( nKeyLen>99 ) goto Loop;
			while (pos1[0]>' ') ++pos1;
			while (pos1[0] && pos1[0]<=' ') ++pos1;  if (pos1[0]==0) goto Loop;
			ch1 = pos1[0]; ++pos1;
			while (pos1[0] && pos1[0]<=' ') ++pos1;  if (pos1[0]==0) goto Loop;
			ch2 = pos1[0]; ++pos1; ch = pos1[0]; ++pos1;
			if (ch!='>' && ch!='=' ) goto Loop;
			

			if ( nOnce & RTF_ACT_SHOW_VAL )
			{
				strcpy( szTmp, szKeyName );
				strcat( szTmp, "=" );
				nKVSrcLen = nKeyLen;
				nKVTarLen = 0;
			}
			else
			{

			   /*	 C : PAD format
			   0 - PADLeft   
			   1 - PADRight   
			   2 - PADCenter
			   3 - No PAD, delete E which length in B, insert replace string, 
			       fill length of replace string in B 
			   4 - Same as 3, but keep the original string if G is null
			   5 - strftime Expression
			   6 - sprintf Formated String (%u,%l -> Upper/Lower case)
			   7 - sprintf Formated Numerical
			   8 - sscanf Formated String
			   9 - Standard UniPipe Export Format String
			   */

			// Get the string to replace for within	pszParam
			if (pms==NULL) goto Loop;  // Nothing to replace, FIX only
			strcpy( szTmp, szKeyName );
			strcat( szTmp, "=" );
			psV1 = strstr( pms, szTmp );
			if (psV1==NULL) goto Loop; // This Key does not exist in pszParam
			
			// for RTF_ACT_ONCE, cancel this KeyName from the Param String
			// to prevent further processing
			if ( nOnce & RTF_ACT_ONCE ) { psV1[0]='-', psV1[1]='-'; }
			psV2 = psV1;

			psV1 += strlen( szKeyName )+1;
			nValLen = 0;
			while ( psV1[0] && psV1[0]!=']' && psV1[0]!='|' && nValLen<sizeof(szTmp)-1 )
			{
				ux = (unsigned int)psV1[0] & 0x0FF;
				if ( ux<128 ) szTmp[nValLen++]=(psV1++)[0];
				else 
				{
					szTmp[nValLen++]='\\'; szTmp[nValLen++]='\'';
					sprintf( szTmp+nValLen, "%02x", ux ); nValLen+=2;
					psV1++;
				}
			}
			szTmp[nValLen]=0;

			//szTmp[0]=0;
			//sscanf( psV1, "%800[^\]|]", szTmp );
			//nValLen = strlen( szTmp );

			if (nValLen==0 && ch1=='4') goto Loop; //keep original with a NULL
			++nn;  // Counting the fields to be replaced.

			// Get Format String to szFmt
			szFmt[0] = 0; k = 0;
			while ( pos1[0] && pos1[0]!='>' && ch2==':' && k<sizeof(szFmt)-1 )
				if ( pos1[0]>=' ' ) szFmt[k++]=(pos1++)[0]; else pos1++;
			szFmt[k] = 0;

			// Formating the target string (replace for)
			nKVSrcLen = nKeyLen;
			nKVTarLen = nKeyLen;
			switch (ch1)
			{
			case '0':  // Pad Left
				if ( nValLen>=nKeyLen ) 
					strncpy( szKeyTarVal, szTmp, nKeyLen );
				else
				{
					memset( szKeyTarVal, ch2, nKeyLen );
					strncpy( szKeyTarVal+(nKeyLen-nValLen), szTmp, nValLen);
				}
				break;
			case '1':  // Pad Right
				if ( nValLen>=nKeyLen ) 
					strncpy( szKeyTarVal, szTmp, nKeyLen );
				else
				{
					memset( szKeyTarVal, ch2, nKeyLen );
					strncpy( szKeyTarVal, szTmp, nValLen);
				}
				break;
			case '2':  // Pad Middle
				if ( nValLen>=nKeyLen ) 
					strncpy( szKeyTarVal, szTmp, nKeyLen );
				else
				{
					memset( szKeyTarVal, ch2, nKeyLen );
					strncpy( szKeyTarVal+(nKeyLen-nValLen)/2, szTmp, nValLen);

				}
				break;
			case '5':  // strftime expression
				if (ch=='=')
				{
					// sscanf( pos1, "%[^>\n]", szFmt );
					nKVTarLen = GetCTimeStr1( szKeyTarVal, szFmt, szTmp );
					break;
				}
			case '6':  // sprintf string expression
				if (ch=='=')
				{
					// sscanf( pos1, "%[^>\n]", szFmt );
					nKVTarLen = GetFormatStr( szKeyTarVal, szFmt, szTmp );
					break;
				}
			case '7':  // sprintf numerical expression
				if (ch=='=')
				{
					// sscanf( pos1, "%[^>\n]", szFmt );
					nKVTarLen = GetFormatNum( szKeyTarVal, szFmt, szTmp );
					break;
				}
			case '8':  // sscanf string expression
				if (ch=='=')
				{
					// sscanf( pos1, "%[^>\n]", szFmt );
					nKVTarLen = GetScanfStr( szKeyTarVal, szFmt, szTmp );
					break;
				}
			case '9':  // standard UniPipe Export format
				if (ch=='=')
				{
					// sscanf( pos1, "%[^>\n]", szFmt );
					nKVTarLen = FormatForExp2( szKeyTarVal, szTmp, szFmt);
					break;
				}

			default: 
				nKVSrcLen = nKeyLen;
				nKeyLen = nValLen;
				nKVTarLen = nKeyLen;
				strncpy( szKeyTarVal, szTmp, sizeof(szKeyTarVal) );
			}
			szKeyTarVal[ nKVTarLen ] = 0;
			} // For none RTF_ACT_SHOW_VAL

			// Search the Source String	(to be replaced ) in RTF
			mm = nKVTarLen;
			// Control to 99 chars
			if ( mm>99 ) { mm = 99; szKeyTarVal[ mm ] = 0; }

			// nn = ScanField( StartPos, GvnLen,  nOpt ) 
			// 
			if (nKVSrcLen>0)
			{
				k = 0; nK = 0;

				/*
				if ( m_szBuf[i]=='\\' )
				{
					while ( i<nLmt )
					{
						if (strnicmp( m_szBuf+i, "\\par", 4 )==0 ||
							strnicmp( m_szBuf+i, "\\tab", 4 )==0 ||
							strnicmp( m_szBuf+i, "\\cell", 5 )==0) break;
						if ( m_szBuf[i]==' ' ) { ++i; break; }
						++i;
					}
				}

				if (strnicmp( m_szBuf+i, "\\par", 4 )==0 || 
					strnicmp( m_szBuf+i, "\\tab", 4 )==0 ||
					strnicmp( m_szBuf+i, "\\cell", 5 )==0 ) 
				{
					//memmove(szKeyTarVal+1, szKeyTarVal, nKVTarLen);
					szKeyTarVal[nKVTarLen]=' ';
					szKeyTarVal[++nKVTarLen]=0;
					if ( !( m_szBuf[i-1]=='\n' && m_szBuf[i-2]=='\r' && m_szBuf[i-3]==' ' ||
						    m_szBuf[i-1]=='\n' && m_szBuf[i-2]==' ' || m_szBuf[i-1]==' ' ) ) 
					{
						memmove(szKeyTarVal+1, szKeyTarVal, nKVTarLen-1);
						szKeyTarVal[0]=' ';
					}
				}

				nFldsStart = i;
				while ( k<nKVSrcLen && i<nLmt )
				{
					// In Word 2003 above, a field may be splitted
					if ( m_szBuf[i]=='}' )
					{
						j=1; while (m_szBuf[i+j] && m_szBuf[i+j]<32) ++j;
					}
					else j=0;
					if ( m_szBuf[i]=='}' && m_szBuf[i+j]=='{' ) 
					{
						i+=(j+1); 
						while ( m_szBuf[i]=='\\' )
						{
							while ( i<nLmt )
							{
								if ( strnicmp( m_szBuf+i, "\\par", 4 )==0 ||
									 strnicmp( m_szBuf+i, "\\tab", 4 )==0 ||
									 strnicmp( m_szBuf+i, "\\cell", 5 )==0 ) break;
								if ( m_szBuf[i]==' ' ) { ++i; break; }
								++i;
							}
							if ( strnicmp( m_szBuf+i, "\\par", 4 )==0 ||
								 strnicmp( m_szBuf+i, "\\tab", 4 )==0 ||
								 strnicmp( m_szBuf+i, "\\cell", 5 )==0 ) break;
						}
					}
					
					if (m_szBuf[i]=='}' || m_szBuf[i]=='{') break;
					if (m_szBuf[i]=='\\')
					{
						if (m_szBuf[i+1]=='_' || m_szBuf[i+1]=='{' || m_szBuf[i+1]=='}') { i+=2; k+=2; ++nKVSrcLen; }
						else if (m_szBuf[i+1]=='\'') { k++; i++; }
						else break;
					}
					else
					{
						if (m_szBuf[i]<32) ++nKVSrcLen; k++; i++;
					}
				}
				nKVSrcLen = k; nK = (i-nFldsStart);
				*/
				qn = 0;
				k = SCANFIELD( m_szBuf+i, nKVSrcLen, qn );
				nFldsStart = i+qn;
				nKVSrcLen = k; nK = k-qn;
			}
			else
			{
				nFldsStart = i;	nK=0;
				if (strnicmp( m_szBuf+i, "\\par", 4 )==0) 
				{
					szKeyTarVal[nKVTarLen]=' ';
					szKeyTarVal[++nKVTarLen]=0;
					if ( !( m_szBuf[i-1]=='\n' && m_szBuf[i-2]=='\r' && m_szBuf[i-3]==' ' ||
						    m_szBuf[i-1]=='\n' && m_szBuf[i-2]==' ' || m_szBuf[i-1]==' ' ) ) 
					{
						memmove(szKeyTarVal+1, szKeyTarVal, nKVTarLen-1);
						szKeyTarVal[0]=' ';
					}
				}
			}
			// So far, (nFldsStart, nK) gives out KeyValue (source string)
			
			// Do show value 
			if ( nOnce & RTF_ACT_SHOW_VAL )
			{
				//strncpy(szKeyTarVal, m_szBuf+nFldsStart, nKVSrcLen);
				//szKeyTarVal[nKVSrcLen]=0;
				strncpy(szKeyTarVal, m_szBuf+nFldsStart, nK);
				szKeyTarVal[nK]=0;
				strcat( szTmp, szKeyTarVal );
				strcat( pms, szTmp );
				strcat( pms, "\r\n" );
				goto Loop;
			}

			// Replace the source string with target
			if ( nK/*VSrcLen*/ == nKVTarLen )
				strncpy( m_szBuf+nFldsStart, szKeyTarVal, nK/*VSrcLen*/ );
			else
			{
				nTmp = ReplaceOneFlds( nFldsStart, nK/*VSrcLen*/, nKVTarLen, nLmt, szKeyTarVal );
				if ( nTmp )
				{
					nLmt = nTmp;
					i = nFldsStart+nKVTarLen;
					sprintf( szTmp, "%02d", mm ); 
					strncpy( pos, szTmp, 2);

					// For RTF_ACT_ONCE, cancel this field for furthering processing
					if ( nOnce & RTF_ACT_ONCE || strstr( szKeyTarVal, "\\par" )!=NULL )
					{ (pos-2)[0]='#'; (pos-3)[0]='#'; }					
				}
			}
		}
		else
			i++;
Loop:	;	
	}
	m_total = nLmt;

	ps = strstr( pszParam, "|InsBarCode=True|" );
	if ( ps!=NULL )
	{
		ps += strlen("|InsBarCode=True|");
		InsertBarCode( ps );
	} 
	return nn;
}


// Read the content after the given field, such as <*HospName 00 3 1>
// until eoln or \xxx or }
// pszKey=NULL means for all fields
void CWRTFDoc::RetrieveField( LPTSTR pszKey, LPTSTR pszValue, int nVLen, BOOL nOption, void* ptr )
{
	long j, i = 0, k = 0;
	long nLmt, nLev, nPos, pn;
	long nFldsStart, nFldsLen;
	TCHAR ch, chs;
	LPTSTR pos;

	TCHAR szRtvlKey[64], szKeyName[200], ch1, ch2;
	int nKeyLen, nKeys = 0;

	if ( !m_res ) return;
	nPos = 0;
	pszValue[0] = 0;
	nLmt = m_total;

	while ( i < nLmt && nPos<nVLen )
	{
		if (CHKHIDDEN(i,m_szBuf))
		{
			// Get the Start Point of Hidden Text Section

			// Looking for the end of this hidden section
			nFldsStart = i;
			nFldsLen = 1;
			ch = m_szBuf[i];
			if (ch=='{') nLev=1; else nLev=0;
			do 
			{
				nFldsLen ++; i++;
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
			}
			while ( !( ENDHIDDEN(i) && nLev==0 ) && (i<nLmt) ); 

			if ( ch=='{' ) i++;
			else if ( ch!='{' && m_szBuf[i]==' ' ) i++;
			// if ( ch!='{' && m_szBuf[i]==' ' ) i++;
			while (m_szBuf[i]=='\n'||m_szBuf[i]=='\r') i++;
			if ( (m_szBuf[i]!='{' || !CHKHIDDEN(i,m_szBuf)) && ch=='{' )
			{
				if ( m_szBuf[i]=='{' ) i++;
				while ( m_szBuf[i]=='\n'||m_szBuf[i]=='\r' ) i++;
				while ( m_szBuf[i]=='\\' && ( m_szBuf[i+1]!='\'' && m_szBuf[i+1]!='_' && m_szBuf[i+1]!='{' && m_szBuf[i+1]!='}' 
					&& strncmp(m_szBuf+i+1,"par",3) && strncmp(m_szBuf+i+1,"tab",3) && strncmp(m_szBuf+i+1,"cell",4)))
				{
					while (m_szBuf[i] && m_szBuf[i]!=' ') i++;
					i++;
					while ( m_szBuf[i]=='\n' || m_szBuf[i]=='\r' ) i++;
				}
			}

			if (i>=nLmt) break;
			// So far, (nFldsStart, nFldsLen) give out a Hidden Section, i -> next pos

			// Parse Key String within this Hidden Section to get all parameters
			k= nFldsStart;
			while (! ((pn=CHKFIELD(k, m_szBuf))>0) && (k < nFldsStart+nFldsLen)) k++;
			if ( k == nFldsStart+nFldsLen )  goto Loop;
			ch=' ';	szKeyName[0]=0;	nKeyLen=0;
			sscanf( m_szBuf+k+pn,"%199[^> ]%c%d %c %c%c",
					szKeyName, &chs, &nKeyLen, &ch1, &ch2, &ch );
			
			// To see whether the obtained field is what we are looking for
			if ( pszKey!=NULL && strcmp( pszKey, szKeyName )!=0 )
				goto Loop; 

			// Multiple fields case
			if ( pszKey==NULL)
			{
				strncat(pszValue, szKeyName, nVLen-strlen(pszValue));
				strncat(pszValue,"=",nVLen-1);
				nPos += strlen(szKeyName)+1;
			}

			if (chs=='>')
			{
				// After <*XXXX>, srch for \par, skip <*Code>
				// nPos = 0;
				if (nPos>0) pszValue[nPos-1]=';';
				while (i<nLmt && nPos<nVLen)
				{
					if (strncmp(m_szBuf+i,"\\par",4)==0) break;
					else if (CHKHIDDEN(i,m_szBuf))
					{
						// Skip sub Hidden Text Section

						// Looking for the end of this hidden section
						ch = m_szBuf[i];
						if (ch=='{') nLev=1; else nLev=0;
						do 
						{
							i++;
							if (m_szBuf[i]=='{') nLev++;
							if (m_szBuf[i]=='}') nLev--;
						}
						while ( !( ENDHIDDEN(i) && nLev==0 ) && (i<nLmt) ); 
						i++;
					}
					else if ((m_szBuf[i]=='{' && m_szBuf[i+1]=='\\')||m_szBuf[i]=='\\')
					{
						while (m_szBuf[i]!=' ' && i<nLmt) 
						{
							if (strncmp(m_szBuf+i,"\\par",4)==0) break;
							i++;
						}
						if (strncmp(m_szBuf+i,"\\par",4)==0) break;
						i++; 
					}
					else if (m_szBuf[i]=='{')
					{
						i++;
					}
					else if (m_szBuf[i]=='}' ||m_szBuf[i]=='\r'||m_szBuf[i]=='\n') 
					{
						pszValue[nPos++] = ' ';
						i++;
					}
					else pszValue[nPos++] = m_szBuf[i++];

				}
				pszValue[nPos++] = 0;
			}
			else
			{
				if ( ch!='>' || nKeyLen<0 || nKeyLen>99 ) goto Loop;
				pos = m_szBuf+(k+2)+strlen(szKeyName)+1;

				// Skip unwanted codes in RTF
				if (m_szBuf[i]=='{'||m_szBuf[i]=='\\')
				{
					while ( i<nLmt )
					{
						if (strnicmp( m_szBuf+i, "\\par", 4 )==0) break;
						if ( m_szBuf[i]==' ' ) { ++i; break; }
						++i;
					}
					// while (m_szBuf[i]!=' ' && i<nLmt) i++;
					// i++; 
				}	

				k=0;
				nFldsStart = i;
				while ((k<nKeyLen || nKeyLen==0) && i<nLmt && nPos<nVLen )
				{
					if (m_szBuf[i]=='{' || m_szBuf[i]=='}') break;
					if (m_szBuf[i]=='\\')
					{
						if (m_szBuf[i+1]=='_' || m_szBuf[i+1]=='{' || m_szBuf[i+1]=='}') { i+=2; k+=2; }
						else if (m_szBuf[i+1]=='\'') { i+=4; k+=4; }
						else break;
					}
					else
					{
						k++; i++;
					}
				}
				if (nVLen<k) k=nVLen;

				// So far, (nFldsStart, k) gives out KeyValue (source string)
				if ( ptr == NULL )
				{
					if ( nPos!=0 && nPos<nVLen ) pszValue[nPos++]=';'; 
					for (j=0; j<k && nPos<nVLen; j++)
							pszValue[nPos++]=(m_szBuf[nFldsStart+j]<32?32:m_szBuf[nFldsStart+j]);
					pszValue[nPos++] = 0;
				}
				else
				{
					++nKeys;
					sprintf( szRtvlKey, "CCName#%02d", nKeys );
					strncpy( szKeyName, m_szBuf+nFldsStart, k ); szKeyName[k] = 0;
					((CPatParse*)ptr)->SetOneValue(	szRtvlKey, szKeyName );
				}
			}

			if (!nOption) break; else if (m_szBuf[i]!='{') i++;
		}
		else
			i++;
Loop:	;	
	}
	m_total = nLmt;
	if ( ptr != NULL )
	{
		sprintf( szRtvlKey, "%02d", nKeys );
		((CPatParse*)ptr)->SetOneValue(	"CCNumber", szRtvlKey );
	}
	return ;
}


// Retrieve field value from the User Property Section
// pszKey=NULL means for all fields
void CWRTFDoc::RetrieveField2( LPTSTR pszKey, LPTSTR pszValue, int nVLen, BOOL nOpt, void* ptr  )
{
	LPTSTR ts, ps;
	CHAR szKeyName[128], szCurKey[128], szTemp[1024];

	pszValue[0]=0;
	if ( !m_res ) return;
	if ( (ts=strstr(m_szBuf, "{\\*\\userprops"))==NULL ) return;

	sprintf(szKeyName, "{\\propname %s", (pszKey==NULL?"":pszKey));
	ps = ts;

	while ( ps!=NULL )
	{
		ps = strstr(ts, szKeyName);
		if ( ps==NULL ) break;

		ts = ps+strlen(szKeyName);
		szCurKey[0]=0;
		if ( pszKey==NULL || pszKey[0]==0 )
		{
			sscanf(ts, "%[^}{\\]", szCurKey);
			strcat(szCurKey,"=");
		}
		ps = strstr(ts, "{\\staticval ");
		if ( ps==NULL ) break;
		ts = ps+12;
		szTemp[0]=0;
		sscanf(ts, "%[^}{\\]", szTemp);

		strncat(pszValue, szCurKey, nVLen-strlen(pszValue));
		strncat(pszValue, szTemp, nVLen-strlen(pszValue));
		if ( szCurKey[0] ) strncat(pszValue, "\r\n", nVLen-strlen(pszValue));
	}
	return ;
}


//
//	Fmt of proper string <Key>=<Value>|(\r)...
//
void CWRTFDoc::InsertProperty( LPTSTR pzPropStr, int nOpt )
{
	TCHAR szBuf[16000]={0}, szTemp[600]={0}, szKey[64], szVal[512], *ps, *ts, ch;
	int nSeg=0, nLev=0;

	// Create the userprops data buffer in szBuf
	if ( nOpt==0 )
	{
		ps=pzPropStr;

		strcpy( szBuf, "{\\*\\userprops " );

		while ( ps[0] )
		{
			while ( ps[0] && ps[0]<=' ' ) ++ps;
			if ( ps[0]==0 ) break;
			ch=0; szTemp[0]=0;
			sscanf( ps,	"%[^|\r\n]%c", szTemp, &ch );
			ps += strlen(szTemp)+(ch?1:0);

			szKey[0] = 0; szVal[0] = 0;
			sscanf( szTemp, "%[^=]%*c%[^\r]", szKey, szVal );
			
			if ( szKey[0] && szVal[0] )
			{
				sprintf( szTemp, "{\\propname %s}\\proptype30{\\staticval %s}", szKey, szVal );
				strncat( szBuf, szTemp, sizeof(szBuf)-strlen(szBuf) );
			}
		}

		strcat( szBuf, "}" );
	}
	else
		strcpy(szBuf, pzPropStr );

	ts = strstr(m_szBuf, "{\\*\\userprops");
	if ( ts!=NULL )
	{
		ps = ts+2; nLev=0;
		while ( ps[0] && (nLev>0 || ps[0]!='}') ) 
		{
			if (ps[0]=='{') nLev++;
			if (ps[0]=='}') nLev--;
			++ps;
		}
		++ps;
	}
	else
	{
		ts = strstr(m_szBuf, "{\\info");
		if ( ts!=NULL )
		{
			ps = ts+2; nLev=0;
			while ( ps[0] && (nLev>0 || ps[0]!='}') ) 
			{
				if (ps[0]=='{') nLev++;
				if (ps[0]=='}') nLev--;
				++ps;
			}
			ts = ++ps;
		}
		else
			return;
	}

	// [ts-ps] specify the segment for insertion
	long nLmt = m_total;
	long nTmp = ReplaceOneFlds( ts-m_szBuf, (ps-ts), strlen(szBuf), nLmt, szBuf );
	m_total = nTmp;
	return;
}


//
void CWRTFDoc::DeleteFields( )
{
	long i = 0, k = 0;
	long nLmt, nLev;
	long nFldsStart, nFldsLen;
	UCHAR ch;

	if ( !m_res ) return;
	nLmt = m_total;

	while ( i < nLmt )
	{
		if (CHKHIDDEN(i,m_szBuf))
		{
			// Get the Start Point of Hidden Text Section
			// Looking for the end of this hidden section
			ch = m_szBuf[i];
			if (ch=='{') nLev=1; else nLev=0;

			// Keep other tag beside \v
			i = KEEPOTHERTAG( i, m_szBuf, nLmt );
			nFldsStart = i;
			nFldsLen = 1;
			do 
			{
				nFldsLen ++; i++;
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
			}
			while ( !( ENDHIDDEN(i) && nLev==0 ) && (i<nLmt) ); 
			
			//i++;
			if (ch!='{') i++;
			if (i>=nLmt) break;

			// So far, (nFldsStart, nFldsLen) give out a Hidden Section, i -> next pos
			// We will delete it
			memmove( m_szBuf+nFldsStart, m_szBuf+i, nLmt-i ); 
			nLmt -= ( i-nFldsStart );
			i = nFldsStart;
		}
		else
			i++;
	}
	m_total = nLmt;
	return ;
}


// Insert a file or a string (started with '*') into the position marked by szKeyString
// Remove nRvLns lines after insertion
void CWRTFDoc::InsertSignPic(LPCTSTR signPicFileName, LPCTSTR szKeyString, int nRvLns )
{
	char szBuf[200000], szKeyName[200];  
	FILE *fpSignPicFileName;

	long i = 0, k = 0;
	long nLmt, nLev, nn, pn;
	long nFldsStart, nFldsLen;
	CHAR ch, *pos;

	if ( !m_res ) return;

	nLmt = m_total;
	
	if ( stricmp( "Append2End", szKeyString )==0  )
	{
		i = nLmt-1;
		while ( i>0 && m_szBuf[i]!='}' ) i--;
		
		// Get the inserting Document Content
		if ( signPicFileName[0]=='*')
		{
			//sprintf(szBuf,"%s ",(LPCTSTR)signPicFileName+1 );
			strncpy( szBuf, (LPCTSTR)signPicFileName+1, sizeof(szBuf)-2 );
			strcat( szBuf, " " );
			nn = strlen( szBuf );
		}
		else
		{
			fpSignPicFileName=fopen(signPicFileName,"r");
			if ( fpSignPicFileName==NULL ) goto Loop;
			nn = 0;
			nn = fread(szBuf,1,sizeof(szBuf),fpSignPicFileName);
			szBuf[nn]=0;
			fclose(fpSignPicFileName);
		}
		strcpy( m_szBuf+i, "\\par \r\n" );
		strncpy( m_szBuf+i+7, szBuf, nn );
		strcpy( m_szBuf+i+nn+7, "}" );
		nLmt = i+nn+8;
	}
	else 
	while ( i < nLmt )
	{
		if (CHKHIDDEN(i,m_szBuf))
		{
			// Get the Start Point of Hidden Text Section

			// Looking for the end of this hidden section
			nFldsStart = i;
			nFldsLen = 1;
			ch = m_szBuf[i];
			if (ch=='{') nLev=1; else nLev=0;
			do 
			{
				nFldsLen ++; i++;
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
			}
			while ( !( ENDHIDDEN(i) && nLev==0 ) && (i<nLmt) );
			
			i++;
			if ( ch!='{' && m_szBuf[i]==' ' ) i++;
			while (m_szBuf[i]=='\n'||m_szBuf[i]=='\r') i++;
			
			if ( m_szBuf[i]!='{' || !CHKHIDDEN(i,m_szBuf) )
			{
				i++;
				while (m_szBuf[i]=='\n'||m_szBuf[i]=='\r') i++;
			
				while ( m_szBuf[i]=='\\' && ( m_szBuf[i+1]!='\'' && m_szBuf[i+1]!='_' && 
										  m_szBuf[i+1]!='{' && m_szBuf[i+1]!='}' && strncmp(m_szBuf+i+1,"par",3)))
				{
					while (m_szBuf[i] && m_szBuf[i]!=' ') i++;
					i++;
					while ( m_szBuf[i]=='\n' || m_szBuf[i]=='\r' ) i++;
				}
			}

			
			// if ( m_szBuf[i]=='}' && ch=='{' ) i++;
			// else if ( m_szBuf[i]!='\\' ) i++;
			
			if (i>=nLmt) break;
			// So far, (nFldsStart, nFldsLen) give out a Hidden Section, i -> next pos

			// Parse Key String within this Hidden Section to get all parameters
			k= nFldsStart;
			while (!((pn=CHKFIELD(k, m_szBuf))>0) && (k < nFldsStart+nFldsLen)) k++;
			if ( k == nFldsStart+nFldsLen )  goto Loop;
			ch=' ';	szKeyName[0]=0;
			sscanf( m_szBuf+k+pn, "%199[^ >\n]%c", szKeyName, &ch );
			if (ch!='>' || strncmp(szKeyName, szKeyString, strlen(szKeyString))!=0 ) goto Loop;
			// Cancel this field inside Document for further replacement
			m_szBuf[k+2]='-';

			// Get the inserting Document Content
			if ( signPicFileName[0]=='*')
			{
				//sprintf(szBuf,"%s ",(LPCTSTR)signPicFileName+1 );
				strncpy( szBuf, (LPCTSTR)signPicFileName+1, sizeof(szBuf)-2 );
				strcat( szBuf, " " );
				nn = strlen( szBuf );
			}
			else
			{
				fpSignPicFileName=fopen(signPicFileName,"r");
				if ( fpSignPicFileName==NULL ) goto Loop;
				nn = 0;
				nn = fread(szBuf,1,sizeof(szBuf),fpSignPicFileName);
				szBuf[nn]=0;
				pos = strstr( szBuf, "\\rtf" );
				if ( pos!=NULL ) { pos[1]='x'; pos[2]='x'; pos[3]='x'; }
				fclose(fpSignPicFileName);
			}

			// Skip different Codes
			if (m_szBuf[i]=='{'||m_szBuf[i]=='\\')
			{
				while (m_szBuf[i]!=' ' && strnicmp(m_szBuf+i, "\\par",4) && i<nLmt) i++;
				if (m_szBuf[i]==' ') i++; 
			}
			
			// Insertion
			memmove( m_szBuf+i+nn, m_szBuf+i, nLmt-i+10 );
			memmove( m_szBuf+i, szBuf, nn );
			i += nn;
			nLmt += nn;
			m_szBuf[ nLmt ]=0;

			while ( nRvLns>0 )
			{
				if ( m_szBuf[i]==0 || i>=nLmt-4 ) break;
				if ( strnicmp(m_szBuf+i, "\\par",4)==0 && 
					 ( m_szBuf[i+4]<=' ' || m_szBuf[i+4]=='\\' || m_szBuf[i+4]=='{'	|| m_szBuf[i+4]=='}') )
				{
					memmove( m_szBuf+i, m_szBuf+i+4, nLmt-i-3 );
					nLmt-=4; --nRvLns;
				}
				else 
					++i;
			}

			m_szBuf[ nLmt ]=0;
			break;
		}
		else
			i++;
Loop:	;	
	}
	m_total = nLmt;
	return ;
}


#define OPT_CUT_BY_EMPTY_LINE		0x00000001
#define OPT_CUT_BY_EOLN				0x00000002

// Read plain text from a RTF data block
int GetTextFromRTF( LPTSTR pzBuf, LPTSTR pzText, int nLmt, int nOpt=0 )
{
	LPTSTR ps = pzBuf;
	LPTSTR qs = pzText;
	int nm = 0, nn = 0;

	while ( ps[0] && nn<nLmt )
	{
		if ( ps[0]=='}' || ps[0]=='{') ++ps;
		else if (  ps[0]=='\\' )
		{
			if ( strncmp(ps,"\\par",4)==0 )
			{
				if ( nOpt & OPT_CUT_BY_EOLN ) break;
				ps += 4; (qs++)[0]='\n'; ++nn;
				if ( nm==0 && (nOpt & OPT_CUT_BY_EMPTY_LINE) ) break;
				nm = 0;
			}
			else if ( strncmp( ps, "\\field", 6 )==0 )
			{
				break;
			}
			else
			{
				++ps; while ( ps[0] && ps[0]!=' ' && ps[0]!='\\' ) ++ps;
			}
			if ( ps[0]==' ' ) ++ps;
		}
		else
		{
			if ( ps[0]>' ' ) ++nm;
			(qs++)[0]=(ps++)[0]; ++nn;
		}
	}
	qs[0] = 0;


	return nn;
}



// Read the content after the given Keyword, such as CC:
// until eoln or \xxx or }
// pszKey=NULL means for all fields
void CWRTFDoc::RetrieveField3( LPTSTR pszKey, LPTSTR pszValue, int nVLen, BOOL nOption, void* ptr  )
{
	int i = 0, k = 0, nn, nKeys;
	long nLmt, nPos;
	CHAR szRtvlKey[64];
	CHAR szLine[1000], szTemp[8000]={0}, *ps, *qs;

	if ( !m_res || pszKey==NULL || pszKey[0]==0 ) return;
	
	i = 0; nPos = 0;
	pszValue[0] = 0;
	nKeys = 0;
	nLmt = m_total;

	while ( i < nLmt && m_szBuf[i] && nPos<nVLen )
	{
		if ( strncmp(m_szBuf+i, "{\\title", 7) == 0 )
		{
			i += 7;
			while ( i < nLmt &&  m_szBuf[i] && m_szBuf[i]!='}' ) i++;
		}
		else if ( strnicmp(m_szBuf+i, pszKey, strlen(pszKey)) == 0 )
		{
			nn = GetTextFromRTF( m_szBuf+i+strlen(pszKey), szLine, sizeof(szLine)-1, OPT_CUT_BY_EOLN );
			i += nn;

			ps = szLine; qs = szTemp; 
			while ( ps[0] )
			{
				if ( strnicmp( ps, pszKey, strlen(pszKey) )==0 )
				{
					ps += 3;
				}
				else if ( strncmp(ps,"<*",2)==0 )
				{
					while ( ps[0] && ps[0]!='>' ) ++ps; if ( ps[0] ) ++ps;
				}
				else
				{
					if ( ps[0]>=' ' ) (qs++)[0] = ps[0];
					++ps;
				}
			
			}

			if ( ps[0]=='\n' ) ++ps; qs[0] = 0;
			qs = szTemp;
			while ( qs[0]==' ' ) ++qs;

			// fprintf( fop, "%s\t%c\t%s\n", szDoc, ch, qs );
			if ( ptr == NULL )
			{
				while ( qs[0] && nPos<nVLen ) pszValue[nPos++] = (qs++)[0];
				if ( nPos<nVLen ) pszValue[nPos++] = ';';
			}
			else
			{
				++nKeys;
				sprintf( szRtvlKey, "CCName#%02d", nKeys );
				((CPatParse*)ptr)->SetOneValue(	szRtvlKey, qs );
			}
		}
		else 
			++i;
	}

	if ( nPos<nVLen ) pszValue[nPos++] = 0;

	if ( ptr != NULL )
	{
		sprintf( szRtvlKey, "%02d", nKeys );
		((CPatParse*)ptr)->SetOneValue(	"CCNumber", szRtvlKey );
	}

}





#define		ANYSPACE(i)		(m_szBuf[i]==' '||m_szBuf[i]=='\n'||m_szBuf[i]=='\r'||m_szBuf[i]=='\t')
	
BOOL CWRTFDoc::GetNextNode( long nPos, long nLmt, RTFNODE *pRNode )
{
	long i;
	long nFldsStart, nFldsLen;
	CHAR szName[20];
	int nLev, nType;

	nType = 0;
	i = nPos;

	while ANYSPACE(i) ++i;

	if (m_szBuf[i]=='{')
	{
		// Looking for the end this hidden section
		nType |= RN_SEGMENT;
		nFldsStart = i;
		nFldsLen = 1;
		nLev=1;
		do 
		{
			nFldsLen++; i++;
			if (m_szBuf[i]=='{') nLev++;
			if (m_szBuf[i]=='}') nLev--;
		}
		while ( !( m_szBuf[i]=='}' && nLev==0 ) && (i<nLmt) );
		if (m_szBuf[nFldsStart+1]=='\\')
		{
			nType |= RN_ATTRIBUTE;
			sscanf( m_szBuf+nFldsStart+2,"%20[^\\ ]", szName );
			szName[19]=0;
		}
		else
		{
			nType |= RN_TEXT;
			strcpy( szName, "RNText" );
		}
	}
	else if (m_szBuf[i]=='\\')
	{
		nType |= RN_ATTRIBUTE;
		nFldsStart = i;
		nFldsLen = 1;
		do
		{
			nFldsLen++; i++;
		}
		while ( !(ANYSPACE(i)||m_szBuf[i]=='{') &&  i<nLmt );
		if (m_szBuf[i]=='{') --nFldsLen;
		sscanf( m_szBuf+nFldsStart+1,"%20[^\\ ]", szName );
		szName[19]=0;
	}
	else
	{
		nType |= RN_TEXT;
		nFldsStart = i;
		nFldsLen = 0;
		do
		{
			nFldsLen++; i++;
		}
		while ( !(m_szBuf[i]=='\\'||m_szBuf[i]=='{'||m_szBuf[i]=='}') &&  i<nLmt ); 
		strcpy( szName, "RNText" );
	}
	// ( nFldsStart, nFldsLen ) -> RTFNode
	
	pRNode->m_nLen = nFldsLen;
	pRNode->m_nPos = nFldsStart;
	pRNode->m_nType = nType;
	strncpy( pRNode->m_szName, szName, sizeof(szName));
	return ( i< nLmt);
}



void CWRTFDoc::TestNodes( )
{
	RTFNODE sRNode;
	long i,k;

	i = 1;
	while (i<m_total)
	{
		k = GetNextNode( i, m_total, &sRNode );
		i = sRNode.m_nPos+sRNode.m_nLen;
	}
}

// RTF Line Count
BOOL CWRTFDoc::CountLines(LPCTSTR pszFmt, int* nLines, int* nPages)
{
	LCSTAT cur;
	LPTSTR pEnd;

	if ( !m_res ) return false;

	CountSegment( m_szBuf, &pEnd, &cur, 1 );
	return true;
}



void CWRTFDoc::CountSegment( LPTSTR pStart, LPTSTR *pEnd, LCSTAT *pRes, int nLevl )
{
	CHAR szType[50], szBuf[50];
	LCSTAT cur, tmp;
	LPTSTR pos, pSegEnd;
	BOOL ss;

	memset( &cur, 0, sizeof( LCSTAT ) );
	pos = pStart+1;

	while (pos[0]=='\r' && pos[0]=='\n' && pos[0]=='\t') ++pos;
	if (pos[0]=='\\')
	{
		sscanf( pos+1, "%[^ \\{}]", szType );
		pos += strlen(szType)+1;
		strncpy( cur.m_szType, szType, sizeof(cur.m_szType) );
		cur.m_nCode++;
		cur.m_nByte += strlen( szBuf )+1;
	}

	ss = true;
	while ( pos[0] && ss )
	{
		switch(pos[0])
		{
		case '\r':
		case '\n':
		case '\t':
		case ' ':
			++pos;
			break;
		case '\\': // Code(s) follows
			sscanf( pos+1, "%[^ \\{}]", szBuf );
			cur.m_nCode++;
			cur.m_nByte += strlen( szBuf )+1;
			if (strcmp(szBuf, "par")==0 && szBuf[3] == 0 )
			{
				// newline
				if (cur.m_nLastLn > 0 )
				{
					cur.m_nLines++;
					cur.m_nLastLn = 0;
				}
			}
			pos += strlen( szBuf )+1;
			break;
		case '{':  // Next level of segment
			CountSegment( pos, &pSegEnd, &tmp, nLevl+1 );
			pos = pSegEnd+1;

			// Merge  tmp to cur ....
			if ( nLevl==1 && strcmp(tmp.m_szType, "header")==0 &&
				 strcmp(tmp.m_szType, "footer")==0 ) // HF Results 
			{
				// Processing Header/Footer
			}
			else
			{
				if ( true ) //	Some condiftions
				{
					cur.m_nByte += tmp.m_nByte+2;
					cur.m_nChar += tmp.m_nChar;
					cur.m_nCode += tmp.m_nCode;
					cur.m_nLines += tmp.m_nLines;
					cur.m_nLastLn += tmp.m_nLastLn;
				}
			}
			break;
		case '}':  // End of this segemnt
			*pEnd = pos;
			ss = false;
			break;
		default:  // Char(s) follows
			pos++;
			cur.m_nByte++;
			cur.m_nChar++;
			while ( pos[0] && pos[0]!='\\' && pos[0]!='{' && pos[0]!='}' )
			{
				if (pos[0]>' ') 
				{
					cur.m_nLastLn++;
					cur.m_nChar++;
				}
				cur.m_nByte++;
				++pos;
			}
		}
	}
	
	(*pRes) = cur;
	return;
}

#include <io.h>	   

// Copy Template file to the Document first, then generate Labels inside it
// When all block has been filled out, it will duplicate a new page by its
// Template as long as the Template contains <*ContinuePosition> in the end
BOOL CWRTFDoc::LabelGenerate(LPCTSTR pszDoc, LPCTSTR pszTmplt, LPTSTR pszParam)
{
	int nn;
	long hFnds;
	struct _finddata_t fftmp;

	hFnds = _findfirst(pszTmplt, &fftmp);
	if (hFnds==-1)
		return false;
	else
		_findclose( hFnds );

	hFnds = _findfirst(pszDoc, &fftmp);
	if (hFnds==-1) 
		CopyFile( pszTmplt, pszDoc, false );
	else
		_findclose(hFnds);

	if (!ReadFile( pszDoc )) return false;

	nn = ReplaceFields( pszParam, 1 );
	if (nn==0)
	{
		InsertSignPic( pszTmplt, "ContinuePosition" );
		nn = ReplaceFields( pszParam, 1);
	}

	WriteFile( pszDoc );
	return true;
}

#define		MAXINSKEY		64

//
// <*InsMk_[C]Keyword> will marks the position for each keyword and the paragraph
// Text file will include such keywords to mark each paragraph.
// [C] is optional, indicates the Capitalization
void CWRTFDoc::MergeFromText( LPCTSTR pzRtfFile, LPCTSTR pzTxtFile )
{
	char szDspBuf[8096];
	int i, p, nTK = 0;
	char szKey[MAXINSKEY][64], szPrx[MAXINSKEY][16], szTmp[64];
	char* pzCont[MAXINSKEY];
	char *ps, *pzText, *pzPara;
	bool bRels = false;
	CFile file;
	int res, total;

	memset( szKey, 0, sizeof(szKey) );
	memset( szPrx, 0, sizeof(szKey) );
	for (i=0; i<MAXINSKEY; i++) pzCont[i]=NULL;

	if ( pzRtfFile!=NULL ) ReadFile( pzRtfFile );
	ReplaceFields( szDspBuf, RTF_ACT_SHOW_KEY );

	strcpy( szKey[nTK++], "Default" );
	ps = szDspBuf;
	while ( ps[0] )
	{
		if ( strncmp( ps, "InsMk_", 6 )==0 )
		{
			szKey[nTK][0] = 0;
			if ( ps[6]=='[' )
			{
				p = 0; ps += 6;
				while ( ps[0] && (p<15) && ps[0]!=']' && ps[0]!='>' )
					szPrx[nTK][p++]=(ps++)[0];
				if (ps[0]==']' )
				{
					szPrx[nTK][p++]=']'; szPrx[nTK][p++]=0; ++ps;
				}
				else
				{
					szPrx[nTK][0]=0; ps -= p;
				}
			}
			else
				ps += 6;
			sscanf( ps, "%[^>]", szKey[nTK] );
			++nTK;
			++ps;
		}
		else
			++ps;
	}

	// Get Text buffer
	if ( pzTxtFile[0]!='*' )
	{
		// Read from Text File
		res = file.Open( pzTxtFile, CFile::modeRead|CFile::shareDenyWrite );
		if (!res) pzText = NULL;
		else
		{
			total = file.GetLength();
			pzText = new char [total+32];
			if ( pzText==NULL ) return;
			total = file.Read( pzText, total );
			pzText[total] = 0;
			file.Close();
			bRels = true;
		}
	}
	else
	{
		pzText = (char*)pzTxtFile+1;
		total = strlen(pzText);
	}

	if ( pzText==NULL ) return;
	
	// Search the text buffer to find each paragraph
	ps = pzText;
	pzCont[0] = ps;
	i=1; 
	while ( ps[0] )
	{
		p = i;
		if ( p>=nTK ) break;
		while ( p<nTK )
		{
			if ( strncmp( ps, szKey[p], strlen(szKey[p]))==0 )
			{
				pzCont[p] = ps;
				ps += strlen(szKey[p]);
				break;
			}
			else 
				++p;
		}
		if ( p<nTK ) i=p+1;
		++ps;
	}

	// Set the tail of each paragraph
	for ( i=1; i<nTK; i++ )
	{
		if ( pzCont[i]!=NULL )
		{
			pzCont[i][0]=0;
			pzCont[i] += strlen( szKey[i] );
		}
	}
	
	pzPara = new char[total+32];
	if ( pzPara==NULL ) return;
	for ( i=0; i<nTK; i++ )
	{
		if ( pzCont[i]!=NULL )
		{
			if ( i==0 )
				strcpy( szTmp, "TextInsertion" );
			else
				sprintf( szTmp, "InsMk_%s%s", szPrx[i], szKey[i] );

			strcpy( pzPara, "*" );
			strcpy( pzPara+1, pzCont[i] );
			if ( strchr(szPrx[i],'C')!=NULL ) strupr( pzPara );
			InsertSignPic( pzPara, szTmp );
		}
	}
	delete pzPara;
	
	if ( pzRtfFile!=NULL ) WriteFile( pzRtfFile );

	// Release the space;
	if ( bRels ) delete( pzText );

}



void CWRTFDoc::MergeRTFFile( LPCTSTR pzDesFile, LPCTSTR pzSrcFile )
{
	FILE *fpo, *fpi;
	char szTemp[40], *ps;
	char szBuf[64000];
	int nn;

	fpo = fopen( pzDesFile, "r+" );
	if (fpo==NULL) return ;

	fseek( fpo, -10, SEEK_END );
	nn = fread( szTemp, 1, 12, fpo );
	szTemp[nn] = 0;
	ps = strrchr( szTemp, '}' );
	if ( ps!=NULL )
	{
		ps[0]=' '; ps[1]=0;
		
		fseek( fpo, -10, SEEK_END );
		strcat( szTemp, "\\pard \\sectd \\page \n" );
		fwrite( szTemp, 1, strlen(szTemp), fpo );

		fpi = fopen( pzSrcFile, "r" );
		nn = 0;
		while ( (nn=fread(szBuf,1,64000,fpi))>0 )
			fwrite(szBuf, 1, nn, fpo);
		fclose( fpi );

		strcpy(szTemp,"}\n");
		fwrite(szTemp,1,2,fpo);
	}
	fclose( fpo );

}

//
// Insert a barcode	in RTF File
// Definion of pszFlds
// Opt=<nn>|<Mark>=<String>|...|<Mark>=<String>
//   atmost four barcode
void CWRTFDoc::InsertBarCode( LPTSTR pszFlds )
{
	char *ps, Mark[64], Value[64], ch1, ch2;
	char szBuf[64000];
	int nOpt, nRv=0;

	nOpt = BARC_OPT_WITH_CHAR;
	

	ps = pszFlds;
	
	while ( ps[0] )
	{
		Mark[0] = 0; Value[0] = 0; ch1=0; ch2=0;
		while ( ps[0] && ps[0]<=32 ) ++ps;
		sscanf( ps, "%63[^=|]%c%63[^|]%c", Mark, &ch1, Value, &ch2 );
		
		ps += strlen( Mark )+(ch1?1:0)+ strlen( Value )+(ch2?1:0);
		if ( Mark[0] && Value[0]==0 )
		{
			strcpy( Value, Mark );
			strcpy( Mark, "BarCode" );
		}
		if ( Mark[0]==0 && Value[0]==0 ) break;
		if ( strcmp( Mark, "Opt" )==0 ) nOpt = atoi(Value);
		else if ( strcmp( Mark, "RvLns" )==0 ) nRv = atoi(Value);
		else 
		{
			strcpy( szBuf, "*" );
			GeneBar( Value, szBuf, sizeof(szBuf)-1, nOpt );
			InsertSignPic( szBuf, Mark, nRv );
		}
	}
}


//
// Export the RTF Formatted File by given requirement
// Format:
// RTF$_<_RTFStatement_>, ..., <_RTFStatement_> 
// <_RTFStatement_> is one of the following: 
//   INS_[TEXT[=<Mark>]|FIELD|PROP]:[<DataBody>]
//   CLS_[NO|HF|FLDS|HIDE]
//   TO_[RTF|TEXT|WORD|WP51]
// <DataBody>:
//   _AnyText_|[<Field>=<Value>]...
//   
bool RTFExport( LPTSTR pszSrcFile, LPTSTR pszDestFile, LPTSTR pszData, CPatParse* m_pParse, int* pnOpt, LPTSTR pszResult )
{
	TCHAR *qs, *ps, *ts, ch, szIns[100]={0}, szCls[60]={0}, szTo[60]={0}, szMark[60]={0}; 
	TCHAR szTmplt[10000]={0}, szFormat[16000]={0};   
	int nOpt = 0, nLev;

	qs = pszData;
	CWRTFDoc *prtf = new CWRTFDoc();

	if (prtf->ReadFile(pszSrcFile))
	{
		while ( qs[0] )
		{
			if ( strncmp(qs, "INS_", 4)== 0 )
			{
				qs += 4; ch = 0; szIns[0] = 0;
				sscanf( qs,"%99[^,:]%c", szIns, &ch );
				qs += strlen(szIns)+(ch?1:0);

				ps = strchr( szIns, '=' );
				if ( ps!=NULL )	{ ps[0]=0; strcpy( szMark, ps+1 ); }

				szTmplt[0] = 0; ps = szTmplt;
				if ( ch==':' && qs[0]=='[' )
				{
					if ( m_pParse!=NULL ) (ps++)[0]='#';
					++qs; nLev = 1;
					while ( qs[0] )
					{
						if (qs[0]=='[') ++nLev; else if (qs[0]==']') --nLev;
						if ( nLev==0 && qs[0]==']' ) break;
						(ps++)[0] = (qs++)[0];
					}
					ps[0]=0;

					if (strcmp( szIns,"TEXT" )==0) 
					{ szFormat[0]='*', ts=szFormat+1; }
					else ts = szFormat;

					if ( m_pParse!=NULL )
						m_pParse->SetAllText( szTmplt, ts, sizeof(szFormat) );
					else
						strcpy( ts, szTmplt );
				}
				else if ( m_pParse!=NULL )
					m_pParse->SetOneRecord(szFormat, sizeof(szFormat), 0);
				else
					szFormat[0] = 0;

				if ( strcmp( szIns,"TEXT" )==0 && szMark[0] )
				{
					prtf->InsertSignPic( szFormat, szMark );
					if ( pszResult!=NULL )
					{
						sprintf( pszResult, "Insert to text in %s for %s", szMark, pszSrcFile );
					}
				}
				else if ( strcmp( szIns, "FIELD" )==0 )
				{
					prtf->ReplaceFields( szFormat );
					if ( pszResult!=NULL )
					{
						sprintf( pszResult, "Replace fields for %s", pszSrcFile );
					}
				}
				else if ( strcmp( szIns, "PROP" )==0 )
				{
					prtf->InsertProperty( szFormat );
					if ( pszResult!=NULL )
					{
						sprintf( pszResult, "Insert property for %s", pszSrcFile );
					}
				}
			}
			else if ( strncmp(qs, "CLS_", 4)== 0 )
			{
				qs += 4; ch = 0; szCls[0] = 0; nOpt = 0;
				sscanf( qs,"%59[^,:]%c", szCls, &ch );
				qs += strlen(szCls)+(ch?1:0);

				if ( strstr( szCls,"HF" )!=NULL ) nOpt |= RTF_CLS_HF;
				if ( strstr( szCls,"FLDS" )!=NULL ) nOpt |= RTF_CLS_FLDS;
				if ( strstr( szCls,"HIDE" )!=NULL ) nOpt |= RTF_CLS_HIDE;
				if ( strstr( szCls,"PICT" )!=NULL ) nOpt |= RTF_CLS_PICT;
				if ( nOpt )	prtf->ClearFields( nOpt );
			}
			else if ( strncmp(qs, "TO_", 3)== 0 )
			{
				qs += 3; ch = 0; szTo[0] = 0;
				sscanf( qs,"%59[^,:]%c", szTo, &ch );
				qs += strlen(szTo)+(ch?1:0);

				if ( strcmp( szTo,"TEXT" )==0 && pnOpt!=NULL ) *pnOpt=RTF_CONV_TEXT;
				else if ( strcmp( szTo,"WORD" )==0 && pnOpt!=NULL ) *pnOpt=RTF_CONV_WORD;
				else if ( strcmp( szTo,"WP51" )==0 && pnOpt!=NULL ) *pnOpt=RTF_CONV_WP51;
			}
			else
				++qs;
		}

		prtf->WriteFile( pszDestFile );
		delete prtf;
	}
	else
	{
		delete prtf;
		return false;
	}
	return true;
}



bool RTFImport( LPTSTR pszSrcFile, LPTSTR pszData, CPatParse* m_pParse )
{
	TCHAR szFormat[16000]={0};

	CWRTFDoc *prtf = new CWRTFDoc();
	bool ss = (prtf->ReadFile( pszSrcFile )!=0);
	if ( ss )
	{
		prtf->ReplaceFields( szFormat, RTF_ACT_SHOW_VAL );
		prtf->RetrieveField2( NULL, szFormat+strlen(szFormat), sizeof(szFormat)-strlen(szFormat)-1 );
		delete prtf;
		if ( pszData==NULL || pszData[0]==0 )
			m_pParse->GetOneRecord( "{Prefix P *SngRec ALL//|AllList = 0 0 0 \\r|}", 
									szFormat, TFL_FORMAT );
		else
			m_pParse->GetOneRecord( pszData, szFormat, TFL_FORMAT ); 
	}
	return ss;
}


//////////////////////////////////////////////////////////////
//////        BarCode Processing
//////////////////////////////////////////////////////////////

/*********************************************************************

Code 39 Details

Char 	Format1 	Format2
* 	NwNnWnWnN 	bWbwBwBwb
- 	NwNnNnWnW 	bWbwbwBwB
$ 	NwNwNwNnN 	bWbWbWbwb
% 	NnNwNwNwN 	bwbWbWbWb
(sp) NwWnNnWnN 	bWBwbwBwb
. 	WwNnNnWnN 	BWbwbwBwb
/ 	NwNwNnNwN 	bWbWbwbWb
+ 	NwNnNwNwN 	bWbwbWbWb
0 	NnNwWnWnN 	bwbWBwBwb
1 	WnNwNnNnW 	BwbWbwbwB
2 	NnWwNnNnW 	bwBWbwbwB
3 	WnWwNnNnN 	BwBWbwbwb
4 	NnNwWnNnW 	bwbWBwbwB
5 	WnNwWnNnN 	BwbWBwbwb
6 	NnWwWnNnN 	bwBWBwbwb
7 	NnNwNnWnW 	bwbWbwBwB
8 	WnNwNnWnN 	BwbWbwBwb
9 	NnWwNnWnN 	bwBWbwBwb
A 	WnNnNwNnW 	BwbwbWbwB
B 	NnWnNwNnW 	bwBwbWbwB
C 	WnWnNwNnN 	BwBwbWbwb
D 	NnNnWwNnW 	bwbwBWbwB
E 	WnNnWwNnN 	BwbwBWbwb
F 	NnWnWwNnN 	bwBwBWbwb
G 	NnNnNwWnW 	bwbwbWBwB
H 	WnNnNwWnN 	BwbwbWBwb
I 	NnWnNwWnN 	bwBwbWBwb
J 	NnNnWwWnN 	bwbwBWBwb
K 	WnNnNnNwW 	BwbwbwbWB
L 	NnWnNnNwW 	bwBwbwbWB
M 	WnWnNnNwN 	BwBwbwbWb
N 	NnNnWnNwW 	bwbwBwbWB
O 	WnNnWnNwN 	BwbwBwbWb
P 	NnWnWnNwN 	bwBwBwbWb
Q 	NnNnNnWwW 	bwbwbwBWB
R 	WnNnNnWwN 	BwbwbwBWb
S 	NnWnNnWwN 	bwBwbwBWb
T 	NnNnWnWwN 	bwbwBwBWb
U 	WwNnNnNnW 	BWbwbwbwB
V 	NwWnNnNnW 	bWBwbwbwB
W 	WwWnNnNnN 	BWBwbwbwb
X 	NwNnWnNnW 	bWbwBwbwB
Y 	WwNnWnNnN 	BWbwBwbwb
Z 	NwWnWnNnN 	bWBwBwbwb


0 	NUL 	%U 		32 	[sp] [sp]	64 	@ 	%V 		96 		` 	%W
1 	SOH 	$A 		33 	! 	/A 		65 	A 	A 		97 		a 	+A
2 	STX 	$B 		34 	" 	/B 		66 	B 	B 		98 		b 	+B
3 	ETX 	$C 		35 	# 	/C 		67 	C 	C 		99 		c 	+C
4 	EOT 	$D 		36 	$ 	/D 		68 	D 	D 		100 	d 	+D
5 	ENQ 	$E 		37 	% 	/E 		69 	E 	E 		101 	e 	+E
6 	ACK 	$F 		38 	& 	/F 		70 	F 	F 		102 	f 	+F
7 	BEL 	$G 		39 	' 	/G 		71 	G 	G 		103 	g 	+G
8 	BS 		$H 		40 	( 	/H 		72 	H 	H 		104 	h 	+H
9 	HT 		$I 		41 	) 	/I 		73 	I 	I 		105 	i 	+I
10 	LF 		$J 		42 	* 	/J 		74 	J 	J 		106 	j 	+J
11 	VT 		$K 		43 	+ 	/K 		75 	K 	K 		107 	k 	+K
12 	FF 		$L 		44 	, 	/L 		76 	L 	L 		108 	l 	+L
13 	CR 		$M 		45 	- 	- 		77 	M 	M 		109 	m 	+M
14 	SO 		$N 		46 	. 	. 		78 	N 	N 		110 	n 	+N
15 	SI 		$O 		47 	/ 	/O 		79 	O 	O 		111 	o 	+O
16 	DLE 	$P 		48 	0 	0 		80 	P 	P 		112 	p 	+P
17 	DC1 	$Q 		49 	1 	1 		81 	Q 	Q 		113 	q 	+Q
18 	DC2 	$R 		50 	2 	2 		82 	R 	R 		114 	r 	+R
19 	DC3 	$S 		51 	3 	3 		83 	S 	S 		115 	s 	+S
20 	DC4 	$T 		52 	4 	4 		84 	T 	T 		116 	t 	+T
21 	NAK 	$U 		53 	5 	5 		85 	U 	U 		117 	u 	+U
22 	SYN 	$V 		54 	6 	6 		86 	V 	V 		118 	v 	+V
23 	ETB 	$W 		55 	7 	7 		87 	W 	W 		119 	w 	+W
24 	CAN 	$X 		56 	8 	8 		88 	X 	X 		120 	x 	+X
25 	EM 		$Y 		57 	9 	9 		89 	Y 	Y 		121 	y 	+Y
26 	SUB 	$Z 		58 	: 	/Z 		90 	Z 	Z 		122 	z 	+Z
27 	ESQ 	%A 		59 	; 	%F 		91 	[ 	%K 		123 	{ 	%P
28 	FS 		%B 		60 	< 	%G 		92 	\ 	%L 		124 	| 	%Q
29 	GS 		%C 		61 	= 	%H 		93 	] 	%M 		125 	} 	%R
30 	RS 		%D 		62 	> 	%I 		94 	^ 	%N 		126 	~ 	%S
31 	US 		%E 		63 	? 	%J 		95 	_ 	%O 		127 	DEL %T

*/

/*****************************************************

POSTNET

	1  iii||
	2  ii|i|
	3  ii||i
	4  i|ii|
	5  i|i|i
	6  i||ii
	7  |iii|
	8  |ii|i
	9  |i|ii
	0  ||iii

   1. Add up all the digits being encoded, including the delivery point code if it is
   being used. If the encoded digit is a 0, add 0 (rather than 11, to which 0 is encoded). 
   If you are sending a letter to somewhere in Young America, Minnesota, you might be 
   sending to 55555-1234, which would have the sum of 35
   2. Find the number that would need to be added to this number to make it evenly divisible 
   by 10, in this case 5, which is your check digit.


*/

#define CLen	16
#define FontH	10
#define FontW	8
#define MaxPic 	1024
#define BarH	20

int CodeW, CodeN;
unsigned char Code39[128][CLen];
unsigned char POSTNET[12][6];
unsigned char Encode39[128][4];
short Fonts[128][FontH];
unsigned char line[MaxPic];
int cnt;


void FillBits(int x, int w) {
  int h, b, p, q;

  for (h=0; h<w; h++) {
    p = cnt/8;
    q = cnt%8;
    b = (unsigned int)line[p];
    switch (q) {
      case 0:
		b &= ((x<<7)|0x7F);
		break;
      case 1:
		b &= ((x<<6)|0xBF);
		break;
      case 2:
		b &= ((x<<5)|0xDF);
		break;
      case 3:
		b &= ((x<<4)|0xEF);
		break;
      case 4:
		b &= ((x<<3)|0xF7);
		break;
      case 5:
		b &= ((x<<2)|0xFB);
		break;
      case 6:
		b &= ((x<<1)|0xFD);
		break;
      case 7:
		b &= x|0xFE;
		break;
      default:;
    }
    line[p]=(unsigned char)b;
    cnt++;
  }
}

void POSTNETInit()
{
  CodeW = 20;
  CodeN = 5;
  memset( POSTNET, 0, sizeof(POSTNET) );

  strcpy((char*)POSTNET[0], "lliii");
  strcpy((char*)POSTNET[1], "iiill");
  strcpy((char*)POSTNET[2], "iilil");
  strcpy((char*)POSTNET[3], "iilli");
  strcpy((char*)POSTNET[4], "iliil");
  strcpy((char*)POSTNET[5], "ilili");
  strcpy((char*)POSTNET[6], "illii");
  strcpy((char*)POSTNET[7], "liiil");
  strcpy((char*)POSTNET[8], "liili");
  strcpy((char*)POSTNET[9], "lilii");
  strcpy((char*)POSTNET[10],"    l");
  strcpy((char*)POSTNET[11],"l    ");
}


int CreateEnPOSTNET( char *ps, char *ts )
{
	int k=0, nn=0;
	(ts++)[0] = ':'; k++;
	while ( ps[0] && k<10 )
	{
		if ( isdigit( ps[0] ) )
		{
			nn += (ps[0]-'0');
			(ts++)[0]=ps[0];
			k++;
		}
		++ps;
	}

	while ( k<10 )
	{
		(ts++)[0]='0'; k++;
	}
	nn = 10-nn%10;	if ( nn==10 ) nn = 0;
	(ts++)[0] = nn+'0';	k++;

	(ts++)[0] = ';'; k++;
	ts[0]=0;
	return k;
}


void Code39Init() 
{
  CodeW = 10+3*3;
  CodeN = 10;
  memset( Code39, 0, sizeof(Code39) );

  strncpy((char*)Code39['*'],"0-0 1 1 0 ",10);
  strncpy((char*)Code39['0'],"0 0-1 1 0 ",10);
  strncpy((char*)Code39['1'],"1 0-0 0 1 ",10);
  strncpy((char*)Code39['2'],"0 1-0 0 1 ",10);
  strncpy((char*)Code39['3'],"1 1-0 0 0 ",10);
  strncpy((char*)Code39['4'],"0 0-1 0 1 ",10);
  strncpy((char*)Code39['5'],"1 0-1 0 0 ",10);
  strncpy((char*)Code39['6'],"0 1-1 0 0 ",10);
  strncpy((char*)Code39['7'],"0 0-0 1 1 ",10);
  strncpy((char*)Code39['8'],"1 0-0 1 0 ",10);
  strncpy((char*)Code39['9'],"0 1-0 1 0 ",10);
  strncpy((char*)Code39['A'],"1 0 0-0 1 ",10);
  strncpy((char*)Code39['B'],"0 1 0-0 1 ",10);
  strncpy((char*)Code39['C'],"1 1 0-0 0 ",10);
  strncpy((char*)Code39['D'],"0 0 1-0 1 ",10);
  strncpy((char*)Code39['E'],"1 0 1-0 0 ",10);
  strncpy((char*)Code39['F'],"0 1 1-0 0 ",10);
  strncpy((char*)Code39['G'],"0 0 0-1 1 ",10);
  strncpy((char*)Code39['H'],"1 0 0-1 0 ",10);
  strncpy((char*)Code39['I'],"0 1 0-1 0 ",10);
  strncpy((char*)Code39['J'],"0 0 1-1 0 ",10);
  strncpy((char*)Code39['K'],"1 0 0 0-1 ",10);
  strncpy((char*)Code39['L'],"0 1 0 0-1 ",10);
  strncpy((char*)Code39['M'],"1 1 0 0-0 ",10);
  strncpy((char*)Code39['N'],"0 0 1 0-1 ",10);
  strncpy((char*)Code39['O'],"1 0 1 0-0 ",10);
  strncpy((char*)Code39['P'],"0 1 1 0-0 ",10);
  strncpy((char*)Code39['Q'],"0 0 0 1-1 ",10);
  strncpy((char*)Code39['R'],"1 0 0 1-0 ",10);
  strncpy((char*)Code39['S'],"0 1 0 1-0 ",10);
  strncpy((char*)Code39['T'],"0 0 1 1-0 ",10);
  strncpy((char*)Code39['U'],"1-0 0 0 1 ",10);
  strncpy((char*)Code39['V'],"0-1 0 0 1 ",10);
  strncpy((char*)Code39['W'],"1-1 0 0 0 ",10);
  strncpy((char*)Code39['X'],"0-0 1 0 1 ",10);
  strncpy((char*)Code39['Y'],"1-0 1 0 0 ",10);
  strncpy((char*)Code39['Z'],"0-1 1 0 0 ",10);
  strncpy((char*)Code39['-'],"0-0 0 1 1 ",10);
  strncpy((char*)Code39['.'],"1-0 0 1 0 ",10);
  strncpy((char*)Code39['/'],"0-0-0 0-0 ",10);
  strncpy((char*)Code39['+'],"0-0 0-0-0 ",10);
  strncpy((char*)Code39['$'],"0-0-0-0 0 ",10);
  strncpy((char*)Code39['%'],"0 0-0-0-0 ",10);
  strncpy((char*)Code39[' '],"0-1 0 1 0 ",10);

  memset( Encode39, 0, sizeof(Encode39) );
  strcpy((char*)Encode39[0 ],"%U");strcpy((char*)Encode39[32]," " );strcpy((char*)Encode39[64],"%V");strcpy((char*)Encode39[96 ],"%W");
  strcpy((char*)Encode39[1 ],"$A");strcpy((char*)Encode39[33],"/A");strcpy((char*)Encode39[65],"A" );strcpy((char*)Encode39[97 ],"+A");
  strcpy((char*)Encode39[2 ],"$B");strcpy((char*)Encode39[34],"/B");strcpy((char*)Encode39[66],"B" );strcpy((char*)Encode39[98 ],"+B");
  strcpy((char*)Encode39[3 ],"$C");strcpy((char*)Encode39[35],"/C");strcpy((char*)Encode39[67],"C" );strcpy((char*)Encode39[99 ],"+C");
  strcpy((char*)Encode39[4 ],"$D");strcpy((char*)Encode39[36],"/D");strcpy((char*)Encode39[68],"D" );strcpy((char*)Encode39[100],"+D");
  strcpy((char*)Encode39[5 ],"$E");strcpy((char*)Encode39[37],"/E");strcpy((char*)Encode39[69],"E" );strcpy((char*)Encode39[101],"+E");
  strcpy((char*)Encode39[6 ],"$F");strcpy((char*)Encode39[38],"/F");strcpy((char*)Encode39[70],"F" );strcpy((char*)Encode39[102],"+F");
  strcpy((char*)Encode39[7 ],"$G");strcpy((char*)Encode39[39],"/G");strcpy((char*)Encode39[71],"G" );strcpy((char*)Encode39[103],"+G");
  strcpy((char*)Encode39[8 ],"$H");strcpy((char*)Encode39[40],"/H");strcpy((char*)Encode39[72],"H" );strcpy((char*)Encode39[104],"+H");
  strcpy((char*)Encode39[9 ],"$I");strcpy((char*)Encode39[41],"/I");strcpy((char*)Encode39[73],"I" );strcpy((char*)Encode39[105],"+I");
  strcpy((char*)Encode39[10],"$J");strcpy((char*)Encode39[42],"/J");strcpy((char*)Encode39[74],"J" );strcpy((char*)Encode39[106],"+J");
  strcpy((char*)Encode39[11],"$K");strcpy((char*)Encode39[43],"/K");strcpy((char*)Encode39[75],"K" );strcpy((char*)Encode39[107],"+K");
  strcpy((char*)Encode39[12],"$L");strcpy((char*)Encode39[44],"/L");strcpy((char*)Encode39[76],"L" );strcpy((char*)Encode39[108],"+L");
  strcpy((char*)Encode39[13],"$M");strcpy((char*)Encode39[45],"-" );strcpy((char*)Encode39[77],"M" );strcpy((char*)Encode39[109],"+M");
  strcpy((char*)Encode39[14],"$N");strcpy((char*)Encode39[46],"." );strcpy((char*)Encode39[78],"N" );strcpy((char*)Encode39[110],"+N");
  strcpy((char*)Encode39[15],"$O");strcpy((char*)Encode39[47],"/O");strcpy((char*)Encode39[79],"O" );strcpy((char*)Encode39[111],"+O");
  strcpy((char*)Encode39[16],"$P");strcpy((char*)Encode39[48],"0" );strcpy((char*)Encode39[80],"P" );strcpy((char*)Encode39[112],"+P");
  strcpy((char*)Encode39[17],"$Q");strcpy((char*)Encode39[49],"1" );strcpy((char*)Encode39[81],"Q" );strcpy((char*)Encode39[113],"+Q");
  strcpy((char*)Encode39[18],"$R");strcpy((char*)Encode39[50],"2" );strcpy((char*)Encode39[82],"R" );strcpy((char*)Encode39[114],"+R");
  strcpy((char*)Encode39[19],"$S");strcpy((char*)Encode39[51],"3" );strcpy((char*)Encode39[83],"S" );strcpy((char*)Encode39[115],"+S");
  strcpy((char*)Encode39[20],"$T");strcpy((char*)Encode39[52],"4" );strcpy((char*)Encode39[84],"T" );strcpy((char*)Encode39[116],"+T");
  strcpy((char*)Encode39[21],"$U");strcpy((char*)Encode39[53],"5" );strcpy((char*)Encode39[85],"U" );strcpy((char*)Encode39[117],"+U");
  strcpy((char*)Encode39[22],"$V");strcpy((char*)Encode39[54],"6" );strcpy((char*)Encode39[86],"V" );strcpy((char*)Encode39[118],"+V");
  strcpy((char*)Encode39[23],"$W");strcpy((char*)Encode39[55],"7" );strcpy((char*)Encode39[87],"W" );strcpy((char*)Encode39[119],"+W");
  strcpy((char*)Encode39[24],"$X");strcpy((char*)Encode39[56],"8" );strcpy((char*)Encode39[88],"X" );strcpy((char*)Encode39[120],"+X");
  strcpy((char*)Encode39[25],"$Y");strcpy((char*)Encode39[57],"9" );strcpy((char*)Encode39[89],"Y" );strcpy((char*)Encode39[121],"+Y");
  strcpy((char*)Encode39[26],"$Z");strcpy((char*)Encode39[58],"/Z");strcpy((char*)Encode39[90],"Z" );strcpy((char*)Encode39[122],"+Z");
  strcpy((char*)Encode39[27],"%A");strcpy((char*)Encode39[59],"%F");strcpy((char*)Encode39[91],"%K");strcpy((char*)Encode39[123],"%P");
  strcpy((char*)Encode39[28],"%B");strcpy((char*)Encode39[60],"%G");strcpy((char*)Encode39[92],"%L");strcpy((char*)Encode39[124],"%Q");
  strcpy((char*)Encode39[29],"%C");strcpy((char*)Encode39[61],"%H");strcpy((char*)Encode39[93],"%M");strcpy((char*)Encode39[125],"%R");
  strcpy((char*)Encode39[30],"%D");strcpy((char*)Encode39[62],"%I");strcpy((char*)Encode39[94],"%N");strcpy((char*)Encode39[126],"%S");
  strcpy((char*)Encode39[31],"%E");strcpy((char*)Encode39[63],"%J");strcpy((char*)Encode39[95],"%O");strcpy((char*)Encode39[127],"%T");
}

void FontInit() {

  memset( Fonts, 0, sizeof(Fonts) );
  
  Fonts[0][0]  =  0; Fonts[0][1]  =  0;  Fonts[0][2]  =  0;  Fonts[0][3]  =  0; Fonts[0][4]  =  0; 
  Fonts[0][5]  =  0; Fonts[0][6]  =  0;  Fonts[0][7]  =  0;  Fonts[0][8]  =  0; Fonts[0][9]  =  0;

  Fonts['0'][0]=  0x3C;  Fonts['0'][1]=  0x66;  Fonts['0'][2]=  0xC3;  Fonts['0'][3]=  0xC3;  Fonts['0'][4]=  0xC3;
  Fonts['0'][5]=  0xC3;  Fonts['0'][6]=  0xC3;  Fonts['0'][7]=  0xC3;  Fonts['0'][8]=  0x66;  Fonts['0'][9]=  0x3C;

  Fonts['1'][0]=  0x18;  Fonts['1'][1]=  0x38;  Fonts['1'][2]=  0x18;  Fonts['1'][3]=  0x18;  Fonts['1'][4]=  0x18;
  Fonts['1'][5]=  0x18;  Fonts['1'][6]=  0x18;  Fonts['1'][7]=  0x18;  Fonts['1'][8]=  0x18;  Fonts['1'][9]=  0x7E;

  Fonts['2'][0]=  0x3C;  Fonts['2'][1]=  0x66;  Fonts['2'][2]=  0xC3;  Fonts['2'][3]=  0xC3;  Fonts['2'][4]=  0x06;
  Fonts['2'][5]=  0x0C;  Fonts['2'][6]=  0x18;  Fonts['2'][7]=  0x30;  Fonts['2'][8]=  0x60;  Fonts['2'][9]=  0xFF;

  Fonts['3'][0]=  0xFF;  Fonts['3'][1]=  0x06;  Fonts['3'][2]=  0x0C;  Fonts['3'][3]=  0x3C;  Fonts['3'][4]=  0x06;
  Fonts['3'][5]=  0x03;  Fonts['3'][6]=  0x03;  Fonts['3'][7]=  0xC3;  Fonts['3'][8]=  0x66;  Fonts['3'][9]=  0x3C;

  Fonts['4'][0]=  0x06;  Fonts['4'][1]=  0x0E;  Fonts['4'][2]=  0x1E;  Fonts['4'][3]=  0x36;  Fonts['4'][4]=  0x66;
  Fonts['4'][5]=  0xC6;  Fonts['4'][6]=  0xFF;  Fonts['4'][7]=  0x06;  Fonts['4'][8]=  0x06;  Fonts['4'][9]=  0x0F;

  Fonts['5'][0]=  0xFF;  Fonts['5'][1]=  0xC3;  Fonts['5'][2]=  0xC0;  Fonts['5'][3]=  0xFC;  Fonts['5'][4]=  0xC6;
  Fonts['5'][5]=  0x83;  Fonts['5'][6]=  0x03;  Fonts['5'][7]=  0xC3;  Fonts['5'][8]=  0xC6;  Fonts['5'][9]=  0x7C;

  Fonts['6'][0]=  0x3E;  Fonts['6'][1]=  0x63;  Fonts['6'][2]=  0xC0;  Fonts['6'][3]=  0xD8;  Fonts['6'][4]=  0xE6;
  Fonts['6'][5]=  0xC3;  Fonts['6'][6]=  0xC3;  Fonts['6'][7]=  0xC3;  Fonts['6'][8]=  0x66;  Fonts['6'][9]=  0x3C;

  Fonts['7'][0]=  0xFF;  Fonts['7'][1]=  0xC6;  Fonts['7'][2]=  0x0C;  Fonts['7'][3]=  0x0C;  Fonts['7'][4]=  0x18;
  Fonts['7'][5]=  0x18;  Fonts['7'][6]=  0x30;  Fonts['7'][7]=  0x30;  Fonts['7'][8]=  0x60;  Fonts['7'][9]=  0x60;

  Fonts['8'][0]=  0x3C;  Fonts['8'][1]=  0x66;  Fonts['8'][2]=  0xC3;  Fonts['8'][3]=  0xC3;  Fonts['8'][4]=  0x7E;
  Fonts['8'][5]=  0xC3;  Fonts['8'][6]=  0xC3;  Fonts['8'][7]=  0xC3;  Fonts['8'][8]=  0x66;  Fonts['8'][9]=  0x3C;

  Fonts['9'][0]=  0x3C;  Fonts['9'][1]=  0x66;  Fonts['9'][2]=  0xC3;  Fonts['9'][3]=  0xC3;  Fonts['9'][4]=  0xC3;
  Fonts['9'][5]=  0x67;  Fonts['9'][6]=  0x3B;  Fonts['9'][7]=  0x03;  Fonts['9'][8]=  0x06;  Fonts['9'][9]=  0x7C;

  Fonts[' '][0]=  0x00;  Fonts[' '][1]=  0x00;  Fonts[' '][2]=  0x00;  Fonts[' '][3]=  0x00;  Fonts[' '][4]=  0x00;
  Fonts[' '][5]=  0x00;  Fonts[' '][6]=  0x00;  Fonts[' '][7]=  0x00;  Fonts[' '][8]=  0x00;  Fonts[' '][9]=  0x00;

  Fonts['A'][0]=  0x18;  Fonts['A'][1]=  0x00;  Fonts['A'][2]=  0x00;  Fonts['A'][3]=  0x00;  Fonts['A'][4]=  0x00;
  Fonts['A'][5]=  0x00;  Fonts['A'][6]=  0x00;  Fonts['A'][7]=  0x00;  Fonts['A'][8]=  0x00;  Fonts['A'][9]=  0x00;

  Fonts['A'][0]=  0x18;  Fonts['A'][1]=  0x24;  Fonts['A'][2]=  0x24;  Fonts['A'][3]=  0x42;  Fonts['A'][4]=  0x42;
  Fonts['A'][5]=  0x7E;  Fonts['A'][6]=  0x81;  Fonts['A'][7]=  0x81;  Fonts['A'][8]=  0x81;  Fonts['A'][9]=  0x81;

  Fonts['B'][0]=  0xFE;  Fonts['B'][1]=  0x81;  Fonts['B'][2]=  0x82;  Fonts['B'][3]=  0x84;  Fonts['B'][4]=  0xFC;
  Fonts['B'][5]=  0x82;  Fonts['B'][6]=  0x81;  Fonts['B'][7]=  0x81;  Fonts['B'][8]=  0x81;  Fonts['B'][9]=  0xFE;

  Fonts['C'][0]=  0x7E;  Fonts['C'][1]=  0x81;  Fonts['C'][2]=  0x80;  Fonts['C'][3]=  0x80;  Fonts['C'][4]=  0x80;
  Fonts['C'][5]=  0x80;  Fonts['C'][6]=  0x80;  Fonts['C'][7]=  0x80;  Fonts['C'][8]=  0x81;  Fonts['C'][9]=  0x7E;

  Fonts['D'][0]=  0xFE;  Fonts['D'][1]=  0x81;  Fonts['D'][2]=  0x81;  Fonts['D'][3]=  0x81;  Fonts['D'][4]=  0x81;
  Fonts['D'][5]=  0x81;  Fonts['D'][6]=  0x81;  Fonts['D'][7]=  0x81;  Fonts['D'][8]=  0x82;  Fonts['D'][9]=  0xFC;

  Fonts['E'][0]=  0xFF;  Fonts['E'][1]=  0x80;  Fonts['E'][2]=  0x80;  Fonts['E'][3]=  0x80;  Fonts['E'][4]=  0xFE;
  Fonts['E'][5]=  0x80;  Fonts['E'][6]=  0x80;  Fonts['E'][7]=  0x80;  Fonts['E'][8]=  0x80;  Fonts['E'][9]=  0xFF;

  Fonts['F'][0]=  0xFF;  Fonts['F'][1]=  0x80;  Fonts['F'][2]=  0x80;  Fonts['F'][3]=  0x80;  Fonts['F'][4]=  0xFC;
  Fonts['F'][5]=  0x80;  Fonts['F'][6]=  0x80;  Fonts['F'][7]=  0x80;  Fonts['F'][8]=  0x80;  Fonts['F'][9]=  0x80;

  Fonts['G'][0]=  0x7E;  Fonts['G'][1]=  0x80;  Fonts['G'][2]=  0x80;  Fonts['G'][3]=  0x80;  Fonts['G'][4]=  0x80;
  Fonts['G'][5]=  0x8F;  Fonts['G'][6]=  0x81;  Fonts['G'][7]=  0x81;  Fonts['G'][8]=  0x81;  Fonts['G'][9]=  0x7E;

  Fonts['H'][0]=  0x81;  Fonts['H'][1]=  0x81;  Fonts['H'][2]=  0x81;  Fonts['H'][3]=  0x81;  Fonts['H'][4]=  0xFF;
  Fonts['H'][5]=  0x81;  Fonts['H'][6]=  0x81;  Fonts['H'][7]=  0x81;  Fonts['H'][8]=  0x81;  Fonts['H'][9]=  0x81;

  Fonts['I'][0]=  0x38;  Fonts['I'][1]=  0x10;  Fonts['I'][2]=  0x10;  Fonts['I'][3]=  0x10;  Fonts['I'][4]=  0x10;
  Fonts['I'][5]=  0x10;  Fonts['I'][6]=  0x10;  Fonts['I'][7]=  0x10;  Fonts['I'][8]=  0x10;  Fonts['I'][9]=  0x7C;

  Fonts['J'][0]=  0x7E;  Fonts['J'][1]=  0x08;  Fonts['J'][2]=  0x08;  Fonts['J'][3]=  0x08;  Fonts['J'][4]=  0x08;
  Fonts['J'][5]=  0x08;  Fonts['J'][6]=  0x08;  Fonts['J'][7]=  0x48;  Fonts['J'][8]=  0x48;  Fonts['J'][9]=  0x30;

  Fonts['K'][0]=  0x82;  Fonts['K'][1]=  0x84;  Fonts['K'][2]=  0x88;  Fonts['K'][3]=  0xD0;  Fonts['K'][4]=  0xA0;
  Fonts['K'][5]=  0x90;  Fonts['K'][6]=  0x88;  Fonts['K'][7]=  0x84;  Fonts['K'][8]=  0x82;  Fonts['K'][9]=  0x81;

  Fonts['L'][0]=  0x80;  Fonts['L'][1]=  0x80;  Fonts['L'][2]=  0x80;  Fonts['L'][3]=  0x80;  Fonts['L'][4]=  0x80;
  Fonts['L'][5]=  0x80;  Fonts['L'][6]=  0x80;  Fonts['L'][7]=  0x80;  Fonts['L'][8]=  0x80;  Fonts['L'][9]=  0xFF;

  Fonts['M'][0]=  0x81;  Fonts['M'][1]=  0xC3;  Fonts['M'][2]=  0xA5;  Fonts['M'][3]=  0xA5;  Fonts['M'][4]=  0x99;
  Fonts['M'][5]=  0x99;  Fonts['M'][6]=  0x81;  Fonts['M'][7]=  0x81;  Fonts['M'][8]=  0x81;  Fonts['M'][9]=  0x81;

  Fonts['N'][0]=  0x81;  Fonts['N'][1]=  0xC1;  Fonts['N'][2]=  0xA1;  Fonts['N'][3]=  0x91;  Fonts['N'][4]=  0x89;
  Fonts['N'][5]=  0x85;  Fonts['N'][6]=  0x83;  Fonts['N'][7]=  0x81;  Fonts['N'][8]=  0x81;  Fonts['N'][9]=  0x81;

  Fonts['O'][0]=  0x7E;  Fonts['O'][1]=  0x81;  Fonts['O'][2]=  0x81;  Fonts['O'][3]=  0x81;  Fonts['O'][4]=  0x81;
  Fonts['O'][5]=  0x81;  Fonts['O'][6]=  0x81;  Fonts['O'][7]=  0x81;  Fonts['O'][8]=  0x81;  Fonts['O'][9]=  0x7E;

  Fonts['P'][0]=  0xFC;  Fonts['P'][1]=  0x82;  Fonts['P'][2]=  0x81;  Fonts['P'][3]=  0x81;  Fonts['P'][4]=  0x82;
  Fonts['P'][5]=  0xFC;  Fonts['P'][6]=  0x80;  Fonts['P'][7]=  0x80;  Fonts['P'][8]=  0x80;  Fonts['P'][9]=  0x80;

  Fonts['Q'][0]=  0x7E;  Fonts['Q'][1]=  0x81;  Fonts['Q'][2]=  0x81;  Fonts['Q'][3]=  0x81;  Fonts['Q'][4]=  0x81;
  Fonts['Q'][5]=  0x81;  Fonts['Q'][6]=  0x8D;  Fonts['Q'][7]=  0x82;  Fonts['Q'][8]=  0x85;  Fonts['Q'][9]=  0x79;

  Fonts['R'][0]=  0xFC;  Fonts['R'][1]=  0x82;  Fonts['R'][2]=  0x81;  Fonts['R'][3]=  0x82;  Fonts['R'][4]=  0x9C;
  Fonts['R'][5]=  0xE0;  Fonts['R'][6]=  0x90;  Fonts['R'][7]=  0x88;  Fonts['R'][8]=  0x84;  Fonts['R'][9]=  0x83;

  Fonts['S'][0]=  0x7F;  Fonts['S'][1]=  0x80;  Fonts['S'][2]=  0x80;  Fonts['S'][3]=  0x40;  Fonts['S'][4]=  0x3E;
  Fonts['S'][5]=  0x01;  Fonts['S'][6]=  0x01;  Fonts['S'][7]=  0x01;  Fonts['S'][8]=  0x02;  Fonts['S'][9]=  0xFC;

  Fonts['T'][0]=  0xFF;  Fonts['T'][1]=  0x08;  Fonts['T'][2]=  0x08;  Fonts['T'][3]=  0x08;  Fonts['T'][4]=  0x08;
  Fonts['T'][5]=  0x08;  Fonts['T'][6]=  0x08;  Fonts['T'][7]=  0x08;  Fonts['T'][8]=  0x08;  Fonts['T'][9]=  0x08;

  Fonts['U'][0]=  0x81;  Fonts['U'][1]=  0x81;  Fonts['U'][2]=  0x81;  Fonts['U'][3]=  0x81;  Fonts['U'][4]=  0x81;
  Fonts['U'][5]=  0x81;  Fonts['U'][6]=  0x81;  Fonts['U'][7]=  0x81;  Fonts['U'][8]=  0x42;  Fonts['U'][9]=  0x3C;

  Fonts['V'][0]=  0x81;  Fonts['V'][1]=  0x81;  Fonts['V'][2]=  0x81;  Fonts['V'][3]=  0x81;  Fonts['V'][4]=  0x81;
  Fonts['V'][5]=  0x42;  Fonts['V'][6]=  0x42;  Fonts['V'][7]=  0x24;  Fonts['V'][8]=  0x24;  Fonts['V'][9]=  0x18;

  Fonts['W'][0]=  0x81;  Fonts['W'][1]=  0x81;  Fonts['W'][2]=  0x81;  Fonts['W'][3]=  0x91;  Fonts['W'][4]=  0x91;
  Fonts['W'][5]=  0x52;  Fonts['W'][6]=  0x52;  Fonts['W'][7]=  0x52;  Fonts['W'][8]=  0x5A;  Fonts['W'][9]=  0x24;

  Fonts['X'][0]=  0x81;  Fonts['X'][1]=  0x81;  Fonts['X'][2]=  0x42;  Fonts['X'][3]=  0x24;  Fonts['X'][4]=  0x18;
  Fonts['X'][5]=  0x24;  Fonts['X'][6]=  0x42;  Fonts['X'][7]=  0x81;  Fonts['X'][8]=  0x81;  Fonts['X'][9]=  0x81;

  Fonts['Y'][0]=  0x81;  Fonts['Y'][1]=  0x41;  Fonts['Y'][2]=  0x22;  Fonts['Y'][3]=  0x14;  Fonts['Y'][4]=  0x08;
  Fonts['Y'][5]=  0x08;  Fonts['Y'][6]=  0x08;  Fonts['Y'][7]=  0x08;  Fonts['Y'][8]=  0x08;  Fonts['Y'][9]=  0x08;

  Fonts['Z'][0]=  0xFF;  Fonts['Z'][1]=  0x02;  Fonts['Z'][2]=  0x04;  Fonts['Z'][3]=  0x08;  Fonts['Z'][4]=  0x08;
  Fonts['Z'][5]=  0x10;  Fonts['Z'][6]=  0x10;  Fonts['Z'][7]=  0x20;  Fonts['Z'][8]=  0x40;  Fonts['Z'][9]=  0xFF;

  Fonts['-'][0]=  0x00;  Fonts['-'][1]=  0x00;  Fonts['-'][2]=  0x00;  Fonts['-'][3]=  0x00;  Fonts['-'][4]=  0xFF;
  Fonts['-'][5]=  0x00;  Fonts['-'][6]=  0x00;  Fonts['-'][7]=  0x00;  Fonts['-'][8]=  0x00;  Fonts['-'][9]=  0x00;

  Fonts['.'][0]=  0x00;  Fonts['.'][1]=  0x00;  Fonts['.'][2]=  0x00;  Fonts['.'][3]=  0x00;  Fonts['.'][4]=  0x00;
  Fonts['.'][5]=  0x00;  Fonts['.'][6]=  0x00;  Fonts['.'][7]=  0x00;  Fonts['.'][8]=  0x18;  Fonts['.'][9]=  0x18;

  Fonts['/'][0]=  0x01;  Fonts['/'][1]=  0x01;  Fonts['/'][2]=  0x02;  Fonts['/'][3]=  0x04;  Fonts['/'][4]=  0x08;
  Fonts['/'][5]=  0x10;  Fonts['/'][6]=  0x20;  Fonts['/'][7]=  0x40;  Fonts['/'][8]=  0x80;  Fonts['/'][9]=  0x80;

  Fonts['+'][0]=  0x08;  Fonts['+'][1]=  0x08;  Fonts['+'][2]=  0x08;  Fonts['+'][3]=  0x08;  Fonts['+'][4]=  0xFF;
  Fonts['+'][5]=  0x08;  Fonts['+'][6]=  0x08;  Fonts['+'][7]=  0x08;  Fonts['+'][8]=  0x08;  Fonts['+'][9]=  0x08;

  Fonts['$'][0]=  0x10;  Fonts['$'][1]=  0x3E;  Fonts['$'][2]=  0x51;  Fonts['$'][3]=  0x90;  Fonts['$'][4]=  0x7C;
  Fonts['$'][5]=  0x12;  Fonts['$'][6]=  0x11;  Fonts['$'][7]=  0x11;  Fonts['$'][8]=  0x7E;  Fonts['$'][9]=  0x10;

  Fonts['%'][0]=  0x61;  Fonts['%'][1]=  0x91;  Fonts['%'][2]=  0xA2;  Fonts['%'][3]=  0x44;  Fonts['%'][4]=  0x08;
  Fonts['%'][5]=  0x10;  Fonts['%'][6]=  0x22;  Fonts['%'][7]=  0x45;  Fonts['%'][8]=  0x89;  Fonts['%'][9]=  0x86;

  Fonts['#'][0]=  0x24;  Fonts['#'][1]=  0x24;  Fonts['#'][2]=  0xFF;  Fonts['#'][3]=  0x24;  Fonts['#'][4]=  0x24;
  Fonts['#'][5]=  0x24;  Fonts['#'][6]=  0x24;  Fonts['#'][7]=  0xFF;  Fonts['#'][8]=  0x24;  Fonts['#'][9]=  0x24;

}


int CreateEncode39( char *ps, unsigned char *gs, unsigned char *ts )
{
	int nn=0;
	char *ss;

	(gs++)[0]='*'; (ts++)[0]=' '; ++nn;
	while ( ps[0] )
	{
		ss = (char*)Encode39[ps[0]];
		if ( strlen(ss)==2 )
		{
			(gs++)[0]=ss[0]; (gs++)[0]=ss[1];
			(ts++)[0]=ps[0]; (ts++)[0]=' ';
			nn += 2;
		}
		else
		{
			(gs++)[0]=ss[0];
			(ts++)[0]=ss[0];
			++nn;
		}
		ps++;
	}
	(gs++)[0]='*'; (ts++)[0]=' '; ++nn;
	gs[0]=0;  ts[0]=0;
	return nn;
}


void CreateRTFHeader( int Width, int Height, char *ps, unsigned int np, int xr=25, int yr=25 )
{
	int BlkL, nx, ny;
	char szBuf[1024], w[8],h[8],x[16],y[16];
	
	BlkL = Width/4;  BlkL = (BlkL%8?BlkL/8+1:BlkL/8)*8;
	nx = BlkL*Height/4+74;
	// if ( nx>10000 ) nx /=14; else nx /= 16;

	sprintf(x, "%02x%02x%02x%02x", *(unsigned char*)&nx, *((unsigned char*)&nx+1),
			*((unsigned char*)&nx+2), *((unsigned char*)&nx+3) );
	ny = nx-36;
	sprintf(y, "%02x%02x%02x%02x", *(unsigned char*)&ny, *((unsigned char*)&ny+1),
			*((unsigned char*)&ny+2), *((unsigned char*)&ny+3) );
	sprintf(w, "%02x%02x", *(unsigned char*)&Width, *((unsigned char*)&Width+1) );
	sprintf(h, "%02x%02x", *(unsigned char*)&Height, *((unsigned char*)&Height+1) );
	
	sprintf(szBuf,"{\\pict\\picscalex100\\picscaley100\\piccropl0\\piccropr0\\piccropt0\\piccropb0"
		"\\picw%d\\pich%d\\picwgoal%d\\pichgoal%d\\wmetafile8\r\n", Width*xr, Height*yr, 
		Width*(xr>20?xr-10:xr-5), Height*(yr>20?yr-10:yr-5) );
	if ( strlen( ps ) + strlen( (const char*)szBuf ) < np ) strcat( ps, (const char*)szBuf );
	sprintf(szBuf, "010009000003%8s0000%8s00000400000003010800050000000b020000000005000000"
		"0c02%4s%4s05000000070104000000%8s430f2000\r\n",x,y,h,w,y);
	if ( strlen( ps ) + strlen( (const char*)szBuf ) < np ) strcat( ps, (const char*)szBuf );
	sprintf(szBuf, "cc000000%4s%4s00000000%4s%4s0000000028000000%4s0000%4s00000100010000000000"
		"FFF800008813000000000000020000000200000000000000ffffff00\r\n", h,w,h,w,w,h );
	if ( strlen( ps ) + strlen( (const char*)szBuf ) < np ) strcat( ps, (const char*)szBuf );
}


void CreateRTFTailer( char *ps, unsigned int np )
{
	char szBuf[1024];
	sprintf(szBuf, "05000000070101000000030000000000}\r\n" );
	if ( strlen( ps ) + strlen( (const char*)szBuf ) < np ) strcat( ps, (const char*)szBuf );
}


#define ChrX(a)	( a<10 ? a+48 : (a-10)+'a' )

void BLK_write( unsigned char *obuf, int Len, char *ps, unsigned int np ) 
{
  unsigned char lbuf[MaxPic*2], *ts;
  int BlkL, i, u;
  
  lbuf[0] = 0; ts = lbuf;

  BlkL = Len*2;  BlkL = (BlkL%8?BlkL/8+1:BlkL/8)*8;

  for ( i=0; i<Len; i++ )
  {
	u = (unsigned int)obuf[i];
	(ts++)[0] = ChrX( (u / 16) );
	(ts++)[0] = ChrX( (u % 16) );
  }
  i=Len*2; while ( i<BlkL ) { (ts++)[0] = '0'; i++; }
  (ts++)[0] = '\r';
  (ts++)[0] = '\n';
  (ts++)[0] = 0;
  if ( strlen( ps ) + strlen( (const char*)lbuf ) < np ) strcat( ps, (const char*)lbuf );
}




void GeneBar( char *CodeStr, char *pzDataBuf, int nLen, int nOption ) 
{
  int i, j, k, CurC, CodeL, LineL, PicL, PicH, tmp, m1, m2;
  int Width  = 1, Height = BarH, xr = 25, yr = 25;
  unsigned char T_Code[128], C_Code[128];

  if ( nOption & BARC_OPT_LONGER )  Height+=5;
  else if ( nOption & BARC_OPT_LONGEST )  Height+=10;
  if ( nOption & BARC_OPT_TIGHTER )  xr=20;
  else if ( nOption & BARC_OPT_TIGHTEST )  xr=10;

  if ( nOption & BARC_OPT_POSTNET )
  {
	Height -= 5;
    POSTNETInit();

	nOption &= (~BARC_OPT_WITH_CHAR);
	CodeL = CreateEnPOSTNET( CodeStr, (char*)T_Code );
    LineL = CodeL*Width*(CodeW)+16;
    PicL = (LineL%8?LineL/8+1:LineL/8);
    LineL = PicL*8;
    PicH = Height + 8;
  }
  else
  {
    Code39Init();
    FontInit();

    CodeL = CreateEncode39( CodeStr, T_Code, C_Code );
    LineL = CodeL*Width*(CodeW)+16;
    PicL = (LineL%8?LineL/8+1:LineL/8);
    LineL = PicL*8;
    PicH = Height + ((nOption & BARC_OPT_WITH_CHAR)? FontH+4:0) + 8;
  }

  // LineL and PicH is the Width and Height of the Pictures
  // Prepare Head Information
  CreateRTFHeader( LineL, PicH, pzDataBuf, nLen, xr, yr );

  for( i=0; i<MaxPic; i++ ) line[i]=(unsigned char)255;
  for( i=0; i<4; i++ ) BLK_write( line, PicL, pzDataBuf, nLen );

  if (nOption & BARC_OPT_WITH_CHAR) { /* Print Characters */

    for(i=0; i<MaxPic; i++) line[i]=(unsigned char)255;
    BLK_write(line, PicL, pzDataBuf, nLen );

    for(j=FontH-1; j>=0; j--) {
      tmp = FontH;
      for( i=0; i<MaxPic; i++ ) line[i]=(unsigned char)255;
      cnt = 8;

      for ( i=0; i<CodeL; i++ ) {
		CurC = C_Code[i];
		tmp = (CodeW-FontW);
		m1 = tmp/2;
		m2 = tmp-m1;

		for( k=0; k<m1; k++ ) FillBits( 1, Width );

		for(k=0; k<FontW; k++) {
		  tmp = Fonts[CurC][j];
		  tmp >>= (FontW-1-k);
		  tmp = !(tmp&1);
		  FillBits(tmp, Width);
		}

		for( k=0; k<m2; k++ ) FillBits( 1, Width );
      }

      BLK_write( line, PicL, pzDataBuf, nLen );
    }

    for( i=0; i<MaxPic; i++ ) line[i] = (unsigned char)255;
    BLK_write( line, PicL, pzDataBuf, nLen );
    BLK_write(line, PicL, pzDataBuf, nLen );
    BLK_write(line, PicL, pzDataBuf, nLen );
  }

  for(j=0; j<Height; j++) {
    for(i=0; i<MaxPic; i++) line[i]=(unsigned char)255;
    cnt = 8;
    for ( i=0; i<CodeL; i++ ) {
      CurC = T_Code[i];
      for( k=0; k<CodeN; k++ ) {
		if ( nOption & BARC_OPT_POSTNET )
		{
			switch ( POSTNET[CurC-'0'][k] )
			{
			case 'i':
				if ( j*3>=Height ) FillBits( 1, 4*Width );
				else { FillBits( 0, 2*Width ); FillBits( 1, 2*Width ); }
				break;
			case 'l':	
				FillBits( 0, 2*Width ); FillBits( 1, 2*Width ); 
				break;
			default:
				FillBits( 1, 4*Width );
				break;
			}
		}
		else
			switch ( Code39[CurC][k] ) 
			{
			case '1':
				FillBits(0, 4*Width);
				break;
			case '0':
				FillBits(0, Width);
				break;
			case ' ':
				FillBits(1, Width);
				break;
			case '-':
				FillBits(1, 4*Width);
				break;
			default:
				FillBits(1, Width);
			}
      }
    }
    BLK_write(line, PicL, pzDataBuf, nLen );
  }

  for(i=0; i<MaxPic; i++) line[i] = (unsigned char)255;
  for(i=0; i<4; i++) BLK_write( line, PicL, pzDataBuf, nLen );

  CreateRTFTailer( pzDataBuf, nLen );
}


char gszACTabHead[]={
"{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033\\deflangfe2052{\\fonttbl{\\f0\\froman\\fprq2\\fcharset0 "
"Times New Roman;}{\\f1\\fnil\\fprq2\\fcharset2 Wingdings;}}\n"
"{\\*\\generator Msftedit 5.41.21.2506;}\\viewkind4\\uc1\\pard\\b\\f0\\fs28 AutoCorrect Backup Document\\par\n"
"\\par \n"
"\\trowd\\trgaph108\\trleft-108\\trbrdrl\\brdrs\\brdrw10 \\trbrdrt\\brdrs\\brdrw10 \\trbrdrr\\brdrs\\brdrw10 "
"\\trbrdrb\\brdrs\\brdrw10 \\trpaddl108\\trpaddr108\\trpaddfl3\\trpaddfr3\n"
"\\clbrdrl\\brdrw10\\brdrs\\clbrdrt\\brdrw10\\brdrs\\clbrdrr\\brdrw10\\brdrs\\clbrdrb\\brdrw10\\brdrs \\cellx2844"
"\\clbrdrl\\brdrw10\\brdrs\\clbrdrt\\brdrw10\\brdrs\\clbrdrr\\brdrw10\\brdrs\\clbrdrb\\brdrw10\\brdrs \\cellx5796"
"\\clbrdrl\\brdrw10\\brdrs\\clbrdrt\\brdrw10\\brdrs\\clbrdrr\\brdrw10\\brdrs\\clbrdrb\\brdrw10\\brdrs \\cellx8748"
"\\pard\\intbl\\b0\\fs24 Name\\cell Value\\cell RTF\\cell\\row\\trowd\\trgaph108\\trleft-108\\trbrdrl\\brdrs\\brdrw10 "
"\\trbrdrt\\brdrs\\brdrw10 \\trbrdrr\\brdrs\\brdrw10 \\trbrdrb\\brdrs\\brdrw10 \\trpaddl108\\trpaddr108\\trpaddfl3\\trpaddfr3\n"
};

char gszACTab1stC[]={
"\\clbrdrl\\brdrw10\\brdrs\\clbrdrt\\brdrw10\\brdrs\\clbrdrr\\brdrw10\\brdrs\\clbrdrb\\brdrw10\\brdrs \\cellx2844\\clbrdrl"
"\\brdrw10\\brdrs\\clbrdrt\\brdrw10\\brdrs\\clbrdrr\\brdrw10\\brdrs\\clbrdrb\\brdrw10\\brdrs \\cellx5796\\clbrdrl\\brdrw10"
"\\brdrs\\clbrdrt\\brdrw10\\brdrs\\clbrdrr\\brdrw10\\brdrs\\clbrdrb\\brdrw10\\brdrs \\cellx8748\\pard\\intbl "
};

char gszACTab2ndC[]={"\\cell "};

char gszACTab3rdC[]={
"\\cell False\\cell\\row\\trowd\\trgaph108\\trleft-108\\trbrdrl\\brdrs\\brdrw10 \\trbrdrt\\brdrs\\brdrw10 \\trbrdrr\\brdrs"
"\\brdrw10 \\trbrdrb\\brdrs\\brdrw10 \\trpaddl108\\trpaddr108\\trpaddfl3\\trpaddfr3\n"
};

char gszACTabTail[]={"}\n"};


int CreateACTable( char* pzOrgFile, char* pzTargetFile )
{
	FILE *fpi, *fpo;
	char ch, szHead[256], szBuf[16000], *ts, *ps, *qs;
	int k, nn;

	fpi = fopen( pzOrgFile, "rb" );
	if ( fpi==NULL ) return 0;
	fread(szHead,1,32,fpi);
	fseek(fpi,0L,SEEK_SET);

	fpo = fopen( pzTargetFile, "w" );
	if ( fpo==NULL ) { fclose(fpi); return 0; }

	fprintf( fpo, "%s", gszACTabHead );

	szBuf[0]=0;
	// PRD Format
	if ( strncmp(szHead,"PSI",3)==0 )
	{
		fread(szHead,1,6,fpi);
		while (1)
		{
			ch=0;
			if ( fread( &ch, 1, 1, fpi )==0 || ch==0 ) break;
			k = fread(szHead, 1, ch, fpi );
			if ( k==0 ) break; else szHead[k]=0;
			ts = strchr( szHead, '-' );
			if ( ts!=NULL ) ts[0] = 0;
			nn = 0;
			k = fread(&nn, 1, 2, fpi);
			k = fread(szBuf, 1, nn, fpi );
			szBuf[k] = 0; szBuf[255] = 0;
			if ( szHead[0] && szBuf[0] )
				fprintf( fpo, "%s%s%s%s%s", gszACTab1stC, szHead, gszACTab2ndC, szBuf, gszACTab3rdC ); 
		}
	}
	else
	while ( fgets(szBuf, sizeof(szBuf), fpi) != NULL )
	{
		ts = szBuf;
		while ( ts[0] && ts[0]<=' ' ) ++ts;
		ps = ts;
		while ( ts[0] && ts[0]>' ' && ts[0]!='|' ) ++ts;
		if ( ts[0] )
		{
			ts[0]=0; ++ts;
			while ( ts[0] && ts[0]<=' ' ) ++ts;
			qs = ts;
			while ( ts[0] && ts[0]!='\r' && ts[0]!='\n' ) ++ts;
			if ( ts[0] ) ts[0]=0;
			if ( ps[0] && qs[0] )
			{
				qs[255]=0;
				fprintf( fpo, "%s%s%s%s%s", gszACTab1stC, ps, gszACTab2ndC, qs, gszACTab3rdC ); 
			}
		}
		szBuf[0]=0;
	}

	fprintf( fpo, "%s", gszACTabTail );
	fclose( fpi );
	fclose( fpo );
	return 1;
}


