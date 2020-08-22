// RTFProc.cpp: implementation of the CRTFProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PatParse.h"
#include "RTFProc.h"

				   
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Global variable
CRITICAL_SECTION CriticalSection; 
int gnFirst = 0;
HINSTANCE hLibrary = NULL;


int GetFormatStr( char* pszBuf, char* pszFmt, char* pszVal )
{
	sprintf( pszBuf, pszFmt, pszVal );
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


int	GetNormalCode( char* szTmp )
{
	char *destp, *srcp;
	int ncode;

	destp = szTmp; srcp = szTmp;

	while ( srcp[0] )
	{
		if (srcp[0]=='\\')
		{
			if ( srcp[1]=='\'' )
			{
				sscanf( srcp+2,"%x", &ncode );
				ncode %= 256;
				(destp++)[0] = ncode;
				srcp += 4;
			}
			else
				(destp++)[0]=(srcp++)[0];
		}
		else
			(destp++)[0]=(srcp++)[0];
	}
	destp[0]=0;
	return 0;
}


int FormatForExp2( char* pzKeyTarVal, char* pzTmp, char *pzFmt )
{
	char szTmp[256]={0}, szVaTmp[256]={0}, szFmt[256]={0}, *ps, szWork[20]={0};
	
	strncpy( szFmt, pzFmt, sizeof(szFmt)-1 );
	strncpy( szVaTmp, pzTmp, sizeof(szVaTmp)-1 );

	while ((ps = strchr( szFmt, '|' ))!=NULL)
	{
		ps[0]=0; ++ps;
		FormatForExp( szTmp, szVaTmp, szFmt );
		strcpy( szFmt, ps );
		strcpy( szVaTmp, szTmp );
	}
	return FormatForExp( pzKeyTarVal, szVaTmp, szFmt );
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRTFProc::CRTFProc()
{
  fpSourceFile=NULL;
  fpDestinationFile=NULL;
  fpTempFile=NULL;
  szSourceFile[0]='\x0';
  szDestinationFile[0]='\x0';

  if ( gnFirst++==0 )
  {
	  InitializeCriticalSection(&CriticalSection);
  }
}


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




/*
#define		CHKHIDDEN(i)	( ( m_szBuf[i]=='{' && m_szBuf[i+1]=='\\' && m_szBuf[i+2]=='v' && \
							    (m_szBuf[i+3]==' '||m_szBuf[i+3]=='\\') ) || \
							  (	m_szBuf[i]=='{' && m_szBuf[i+1]=='\n' && m_szBuf[i+2]=='\\' \
								&& m_szBuf[i+3]=='v' && (m_szBuf[i+4]==' '||m_szBuf[i+4]=='\\') ) || \
							  (	m_szBuf[i]=='{' && m_szBuf[i+1]=='\r' && m_szBuf[i+2]=='\n' && m_szBuf[i+3]=='\\' \
								&& m_szBuf[i+4]=='v' && (m_szBuf[i+5]==' '||m_szBuf[i+5]=='\\') ) || \
							  ( m_szBuf[i]=='\\' && m_szBuf[i+1]=='v' && (m_szBuf[i+2]=='1' ||m_szBuf[i+2]==' ')) )
*/


int CHKFIELD( int i, LPTSTR ps )
{
	int k=0;
	int ss=0;

	if (ps[i]=='<')
	{
		if ( ps[i+1]=='*' || (strncmp(ps+i+1,"start",5)==0) || (strncmp(ps+i+1,"end",3)==0) ) return 2;
		k++;
		while ( ps[i+k]>0 && ps[i+k]<=' ' ) ++k;
		if ( ps[i+k]=='*' || (strncmp(ps+k+1,"start",5)==0) || (strncmp(ps+k+1,"end",3)==0) ) return k+1;
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

  

CRTFProc::CRTFProc( char* SourceFileName, char* DestinationFileName )
{
  char szDir[256]={0};
  bool ss = false;

  fpSourceFile=NULL;
  fpDestinationFile=NULL;
  fpTempFile=NULL;
  szTempFile[0]=0;

  if ( strcmp( SourceFileName, DestinationFileName )==0 ||
	   stricmp( SourceFileName+(strlen(SourceFileName)-5), ".html" )==0 )
  {
	  GetTempPath(256, szDir);
	  if (GetTempFileName(szDir, "HTMLRTF__", 0, szTempFile))
	  {
		 ss = CopyFile( DestinationFileName, szTempFile, false );
		 if ( ss ) strncpy(szSourceFile, szTempFile, 255);
		 else szTempFile[0]=0;
		 Sleep( 500 );
	  }
  }
  if ( !ss )  strncpy(szSourceFile, SourceFileName, 255);

  strncpy(szOldFile, szSourceFile, 255);
  strncpy(szOrginalFile, szSourceFile, 255);
  strncpy(szDestinationFile, DestinationFileName, 255);

  if ( gnFirst++==0 )
  {
	  InitializeCriticalSection(&CriticalSection);
  }

}


CRTFProc::~CRTFProc()
{
	if ( szTempFile[0] )  DeleteFile( szTempFile );

	if ( --gnFirst==0 )
	{
		DeleteCriticalSection(&CriticalSection);
	}
}


#define		ENDHIDDEN(i)	( ( m_szBuf[i-2]=='\\' && m_szBuf[i-1]=='v' && m_szBuf[i]=='0' && ch=='\\') || \
							  ( m_szBuf[i]=='}' && ch=='{' ) )

/* #define		CHKFIELD(i)		( m_szBuf[i]=='<' && ( (m_szBuf[i+1]=='*') || (strncmp(m_szBuf+i+1,"start",5)==0) \
								|| (strncmp(m_szBuf+i+1,"end",3)==0) ) ) 
*/

#define		CHECKHF(i)		( strncmp(m_szBuf+i,"{\\header", 8)==0 || strncmp(m_szBuf+i,"{\\footer", 8)==0 )

// [/nnn/mmm ..... ] marking a RTF code	and should be replace with {\nnn\mmm  ... }
#define		CHKMARK(i)		( m_szBuf[i]=='[' && m_szBuf[i+1]=='/' && m_szBuf[i+2]!=' ' )




extern void LogViewOper( char* pszFile, char* pszMode );
#define		LOGFILE(x,y)		LogViewOper(x,y)


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
int CRTFProc::FormatAdjustEx( LPCTSTR pszTgtStr, LPTSTR m_szBuf, long& m_total )
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


	if ( (ps=strstr( pszTgtStr, "$L:" ))!=NULL )
	{
		ps+=3; qs=szLocator; SCANONEITEM(ps,qs,ch,',')
	}
	if ( (ps=strstr( pszTgtStr, "$S:" ))!=NULL )
	{
		ps+=3; qs=szSrchObj; SCANONEITEM(ps,qs,ch,',')
	}
	if ( (ps=strstr( pszTgtStr, "$P:" ))!=NULL )
	{
		ps+=3; qs=szPrefix; SCANONEITEM(ps,qs,ch,',')
	}

	if ( (ps=strstr( pszTgtStr, "$R:" ))!=NULL )
	{
		ps+=3; qs=szReplace; SCANONEITEM(ps,qs,ch,',')
	}

	if ( (ps=strstr( pszTgtStr, "$I:" ))!=NULL )
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

	// if ( !m_res ) return 0;
	nn = strlen(szLocator); mm = strlen(szSrchObj); 
	ss = (nn==0);
	i = 0; nLmt = m_total;
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
				if ( pszTmp == NULL )
				{
					m_total = nLmt;
					return nLmt;
				}
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
	m_szBuf[nLmt] = 0;
	m_total = nLmt;
	return nLmt;
}


int CRTFProc::GetHiddenSegment( int& i, LPTSTR m_szBuf, int nLmt, FIELDKEY *ptr )
{
    int nLev, nFldsStart, nFldsLen;
    char ch;

	if (CHKHIDDEN(i, m_szBuf))
	{
		// Get the Start Point of Hidden Text Section

		// Looking for the end of this hidden section
		nFldsStart = i;
		nFldsLen = 1;
		ch = m_szBuf[i];
		if (ch=='{') nLev=1; else nLev=0;
		do
		{
			nFldsLen++; i++;
			if (m_szBuf[i]=='{') nLev++;
			if (m_szBuf[i]=='}') nLev--;
		}
		while ( !( ENDHIDDEN(i) && nLev==0 ) && (i<nLmt) );
		i++;
		if ( ch!='{' && m_szBuf[i]==' ' ) i++;
		while ( m_szBuf[i]=='\n'|| m_szBuf[i]=='\r' ) i++;

		if ( (m_szBuf[i]!='{' || !CHKHIDDEN(i,m_szBuf)) && ch=='{' )
		{
			if (m_szBuf[i]=='{') i++;
			while ( m_szBuf[i]=='\n' || m_szBuf[i]=='\r') i++;
			while ( m_szBuf[i]=='\\' && ( m_szBuf[i+1]!='\'' && m_szBuf[i+1]!='_' && m_szBuf[i+1]!='{' && m_szBuf[i+1]!='}'
					&& strncmp(m_szBuf+i+1,"par",3) && strncmp(m_szBuf+i+1,"tab",3) && strncmp(m_szBuf+i+1,"cell",4) ))
			{
				while (m_szBuf[i] && m_szBuf[i]!=' ') i++;
				i++;
				while ( m_szBuf[i]=='\n' || m_szBuf[i]=='\r' ) i++;
			}
		}

		if (i>=nLmt) return false;
		else
		{
		    // So far, (nFldsStart, nFldsLen) give out a Hidden Section, i -> next pos
            ptr->nPos = nFldsStart;
            ptr->nLen = nFldsLen;
            return true;
		}
    }
    else
        return false;
}


bool CRTFProc::ParseOneFieldKey( LPTSTR m_szBuf, FIELDKEY *ptr )
{
    int k, pn, nFldsStart, nFldsLen, nValLen;
    char ch;
    char szKeyName[200], szFmt[200];
    LPTSTR ps, pos, pos1, pzLmt;

    nFldsStart = ptr->nPos;
    nFldsLen = ptr->nLen;

	// Parse Key String within this Hidden Section to get all parameters
    k= nFldsStart;
    while (! ((pn=CHKFIELD(k, m_szBuf))>0) && (k < nFldsStart+nFldsLen)) k++;
    if ( k >= nFldsStart+nFldsLen )  return false;

    // Removal of garbage inside the field
    pos = m_szBuf+k+pn;
    ps = pos; pzLmt = m_szBuf+(nFldsStart+nFldsLen);

    while ( (pos<pzLmt) && pos[0]!='>' && pos[0]!='}' ) // Considering remove '}' for multiple hidden segments
    {
        while ( pos[0]=='\\' || pos[0]=='\n' || pos[0]=='\r' )
        {
            while ( (pos<pzLmt) && pos[0]!=' ' && pos[0]!='\n' && pos[0]!='\r') ++pos;
            if ( pos[0]==' ' ) ++pos;
            else while ( pos[0]=='\n' || pos[0]=='\r') ++pos;
        }
        if ( (pos>=pzLmt) || pos[0]=='>' || pos[0]=='}' ) break;
        (ps++)[0]=(pos++)[0];
    }
    if ( pos[0]=='>' ) (ps++)[0] = (pos++)[0];
    while ( ps<pos ) (ps++)[0]=' ';


    ch=' ';
    szKeyName[0]=0;	nValLen=0;

    // Need more detail parsing for <*KeyName dd d d> or <*KeyName dd d :=Format String>
    /* sscanf( m_szBuf+k+2,"%199[^> ]%c%d %c%*c%c%c",
            szKeyName, &ch0, &nKeyLen, &ch1, &ch2, &ch );
    if (ch0=='>' || (ch!='>' && ch!='=' ) || nKeyLen<0 || nKeyLen>99 ) goto Loop;
    pos = m_szBuf+(k+2)+strlen(szKeyName)+1;
    while ( pos[0] && pos[0]<=' ' ) ++pos;
    pos1 = pos + 7;
    */
    pos = m_szBuf+k+pn; ps = pos; j=0;
    while ( (pos<pzLmt) && j<199 && pos[0]>' ' && pos[0]!='>' ) szKeyName[j++]=(pos++)[0];
    szKeyName[j]=0;
    if ( j>=199 ) return false;
    if ( pos[0]=='>' )
    {
        ptr->nType = 3;
        strcpy( ptr->szKeyName, szKeyName);
        return true;
    }

    if ( strcmp( szKeyName, "Start") == 0 )
    {
        ptr->nType = 2;
        ptr->pzLenPos = NULL;
        while ((pos<pzLmt) && pos[0]<=' ') ++pos;  if (pos>=pzLmt) return false;
        ptr->nValLen = 0;
        j=0; ps=pos;
        while ( (pos<pzLmt) && j<199 && pos[0]>' ' && pos[0]!='>' ) szKeyName[j++]=(pos++)[0];
        if (j>=199 || (pos>=pzLmt) ) return false;
        szKeyName[j]=0;
        strcpy( ptr->szKeyName, szKeyName);
    }
    else
    {
        ptr->nType = 1;
        strcpy( ptr->szKeyName, szKeyName);
        while ((pos<pzLmt) && pos[0]<=' ') ++pos;  if (pos>=pzLmt) return false;
        nValLen = atoi(pos); if ( nValLen>99 ) return false;
        ptr->nValLen = nValLen;
        ptr->pzLenPos = pos;
        while ((pos<pzLmt) && pos[0]>' ') ++pos;
    }

    ptr->pzNamePos = ps;
    while ((pos<pzLmt) && pos[0]<=' ') ++pos;  if (pos>=pzLmt) return false;
    ptr->cPad = pos[0]; ++pos;
    while ((pos<pzLmt) && pos[0]<=' ') ++pos;  if (pos>=pzLmt) return false;
    ptr->cFmtCode = pos[0]; ++pos;
    ch = pos[0]; ++pos;
    if ( ch=='>' ) return true;
    else if ( ch=='=' && ptr->cFmtCode == ':' ) {
		// Get Format String to szFmt
		szFmt[0] = 0; j = 0;
		while ( (pos<pzLmt) && pos[0]!='>' && j<sizeof(szFmt)-1 )
		    if ( pos[0]>=' ' ) szFmt[j++]=(pos++)[0]; else pos++;
		szFmt[j] = 0;
		if ( (pos>=pzLmt) || j>=sizeof(szFmt)-1 ) return false;
		ptr->szFormat = szFmt;
        return true;
    }
    else
        return false;

}

bool CRTFProc::GetOneSourceValue( LPTSTR pzPrm, FIELDKEY *ptrk, FIELDVAL *ptrv, bool nOnce )
{
       /*	 C : PAD format
       0 - PADLeft
       1 - PADRight
       2 - PADCenter
       3 - No PAD, delete E which length in B, insert replace string,
           fill length of replace string in B
       4 - Same as 3, but keep the original string if G is null
       5 - strftime Expression
       6 - sprinf Formated String
       7 - sprintf Formated Numerical
       8 - sscanf Formated String
       9 - ScanSheet Circled Expression
       */
    // Get the string to replace for within	pszParam
    char szKeyNmX[200], szTmp[2000];
    LPTSTR psV1, psV2;
    LPTSTR pzv;
    LPTSTR szFmt;
    int nTgt, nVal, ux, nKeyLen, nKVTarLen;
    char ch1, ch2;

    ch1 = ptrk->cPad;
    ch2 = ptrk->cFmtCode;

    strcpy( szKeyNmX, ptrk->szKeyName ); strcat( szKeyNmX, "=" );
    psV1 = strstr( pszParam, szKeyNmX );
    if (psV1==NULL) return false; // This Key does not exist in pszParam
    // for nOnce=1, we need cancel this KeyName from the Param String
    // to prevent further processing
    if (nOnce) { psV1[0]='-', psV1[1]='-'; }


    psV1 += strlen( szKeyNmX );
    nVal = 0;
    while ( psV1[0] && psV1[0]!=']' && psV1[0]!='|' && nVal<sizeof(szTmp)-1 )
    {
        ux = (unsigned int)psV1[0] & 0x0FF;
        if ( ux<128 ) szTmp[nVal++]=(psV1++)[0];
        else
        {
            szTmp[nVal++]='\\'; szTmp[nVal++]='\'';
            sprintf( szTmp+nVal, "%02x", ux ); nVal+=2;
            psV1++;
        }
    }
    szTmp[nVal]=0;

    if (nVal==0 && ch1=='4') return false; //keep original with a NULL
    // ++nn;  // Counting the fields to be replaced.

    // Formating the target string (replace for)
    nKeyLen = ptrk->nValLen;
    pzv = ptrv->szTarget;
    szFmt = ptrk->szFormat;

    switch (ch1)
    {
        case '0':  // Pad Left
            if ( nVal>=nKeyLen )
                strncpy( pzv, szTmp, nKeyLen );
            else
            {
                memset( pzv, ch2, nKeyLen );
                strncpy( pzv+(nKeyLen-nVal), szTmp, nVal);
            }
            nKVTarLen = nKeyLen;
            break;
        case '1':  // Pad Right
            if ( nVal>=nKeyLen )
                strncpy( pzv, szTmp, nKeyLen );
            else
            {
                memset( pzv, ch2, nKeyLen );
                strncpy( pzv, szTmp, nVal);
            }
            nKVTarLen = nKeyLen;
            break;
        case '2':  // Pad Middle
            if ( nVal>=nKeyLen )
                strncpy( pzv, szTmp, nKeyLen );
            else
            {
                memset( pzv, ch2, nKeyLen );
                strncpy( pzv+(nKeyLen-nVal)/2, szTmp, nVal);
            }
            nKVTarLen = nKeyLen;
            break;
        case '5':  // Date/Time expression
            //sscanf( pos1, "%[^>\n]", szFmt );
            nKVTarLen = GetCTimeStr( pzv, szFmt, szTmp );
            break;
        case '6':  // sprintf string expression
            //sscanf( pos1, "%[^>\n]", szFmt );
            nKVTarLen = GetFormatStr( pzv, szFmt, szTmp );
            break;
        case '7':  // sprintf numerical expression
            //sscanf( pos1, "%[^>\n]", szFmt );
            nKVTarLen = GetFormatNum( pzv, szFmt, szTmp );
            break;
        case '8':  // sscanf string expression
            //sscanf( pos1, "%[^>\n]", szFmt );
            nKVTarLen = GetScanfStr( pzv, szFmt, szTmp );
            break;
         case '9':  // ScanSheet Ciecled Expression
            //sscanf( pos1, "%[^>\n]", szFmt );
            nKVTarLen = FormatForExp2( pzv, szTmp, szFmt );
            break;
        default:
            nKeyLen = nVal;
            nKVTarLen = nKeyLen;
            strncpy( pzv, szTmp, sizeof(szTmp) );
    }
    pzv[ nKVTarLen ] = 0;
    ptrv->nTgtLen = nKVTarLen;
}


bool CRTFProc::GetOneFieldValue( int& i, LPTSTR m_szBuf, int nLmt, FIELDKEY *ptrk, FIELDVAL *ptrv, bool bCopy )
{
    int k, nk, qn;
    LPTSTR pzv;
    int nTgt;

    if ( ptrk->nType == 1)
    {
        // Search a seg with given length
        if ( ptrk->nValLen > 0 )
        {
            k=0;
            qn = 0;
            k = SCANFIELD( m_szBuf+i, ptrk->nValLen, qn, (bCopy? ptrv->szTarget: NULL) );
            ptrv->nSrcPos = i+qn;
            ptrv->nSrcLen = k-qn;
            i += k;
        }
        else
        {
            ptrv->nSrcPos = i;
            ptrv->nSrclen = 0;
            // add a space in front/end of target value if it meets \par immediately
            if ( strnicmp( m_szBuf+i, "\\par", 4 )==0 && !bCopy )
            {
                pzv = ptrv->szTarget;
                nTgt = ptrv->nTgtLen;
                pzv[nTgt] =' ';
                pzv[++nTgt]=0;
                if ( !( m_szBuf[i-1]=='\n' && m_szBuf[i-2]=='\r' && m_szBuf[i-3]==' ' ||
                        m_szBuf[i-1]=='\n' && m_szBuf[i-2]==' ' || m_szBuf[i-1]==' ' ) )
                {
                    memmove(pzv+1, pzv, nTgt-1);
                    pzv[0]=' ';
                }
                ++(ptrv->nTgtLen);
            }
        }
        // So far, (ptr->nSrcPos, ptr->nSrcLen) gives out KeyValue (source string)
    }
    else if ( ptrk->nType == 2)
    {
        // Search a segment by looking for the end tag
        bool ss = fasle;
        int j;
        k=i;
        while ( k<nLmt )
        {
            j=k;
            if (CHKHIDDEN(k, m_szBuf))
            {
                if ( GetHiddenSegment( k, m_szBuf, nLmt, &subKey ) && ParseOneFieldKey( m_szBuf, &subKey ))
                {
                    if ( (strcmp(subKey.szKeyName, "End")==0) && subKey.nType==2 )
                    {
                        ss = true;
                        ptrv->nSrcPos = i;
                        ptrv->nSrcLen = (j-i);
                        i = k;
                    }
                    break;
                }
            }
            else
                k++;
        }
        return ss;
    }
    else
        return false;
}


// Clear Some attached fields
void CRTFProc::ClearFields( int nOpt )
{
	long i = 0;
	long nLmt, nLev, nTmp;
	long nFldsStart, nFldsLen;
	CHAR ch;
	bool ss;
	LPTSTR m_szBuf;

	// LOGFILE( "  ... Inside -- ", "0" );
	fpSourceFile=fopen(szSourceFile,"rb");
	if (fpSourceFile == NULL) return;
	fseek( fpSourceFile, 0L, SEEK_END );
	nLmt = ftell( fpSourceFile );
	m_szBuf = new CHAR[nLmt+2000];
	if (m_szBuf==NULL)
	{
		fclose( fpSourceFile );
		return;
	}
	// LOGFILE( "  ... Refreash -- ", szSourceFile );
	fseek( fpSourceFile, 0L, SEEK_SET );
	nTmp = fread( m_szBuf,sizeof(CHAR), nLmt+1, fpSourceFile );
	m_szBuf[nTmp] = 0;
	fclose( fpSourceFile );
	// sprintf( szBuf, " F.Len %ul", nTmp );
	nLmt = nTmp;

	while ( i < nLmt )
	{
		if ( (CHKHIDDEN(i,m_szBuf)) && (nOpt & (RTF_CLS_HIDE|RTF_CLS_FLDS)))
		{

			// Looking for the end of this hidden section
			nFldsStart = i;
			nFldsLen = 1;
			ch = m_szBuf[i];
			ss = false;
			if ( ch=='{' ) nLev=1; else nLev=0;
			if ( ch=='{' && m_szBuf[i+1]=='{' ) ++i;
			do 
			{
				ss |= (m_szBuf[i]=='<' && m_szBuf[i+1]=='*');
				nFldsLen ++; i++;
				if (m_szBuf[i]=='{') nLev++;
				if (m_szBuf[i]=='}') nLev--;
			}
			while ( !( ENDHIDDEN(i) && nLev==0 ) && (i<nLmt) ); 
			i++;
			if ( (nOpt & RTF_CLS_HIDE) || ss )
			{
				memmove( m_szBuf+nFldsStart, m_szBuf+i, nLmt-i+1 );
				i = nFldsStart;
				nLmt -= nFldsLen;
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
		else
			i++;
	}
	m_szBuf[nLmt]=0;

	CFile f;
	if (f.Open(szDestinationFile, 
		CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
	{
		f.Write( m_szBuf, nLmt );
		f.Close( );
		// LOGFILE( "  ... Suc Write -- ", szDestinationFile );
		strcpy(szSourceFile,szDestinationFile);
		delete m_szBuf;
	}
	else
	{
		// LOGFILE( "  ... Fail -- ", szDestinationFile );
		delete m_szBuf;
	}
	return ;
}



// \r \n is for DOS text file newline
// Replace the RTF Body with pszParam (it is xxx=vvv|yyy=uuu|...)
// if nOnce=1, RTF Body may contains multiples <*xxx>, but it replace one each time
//    and this <*xxx> will not be replaced next time
//  Adding Barcode function - Append "|BarCode=True|..." to the regular String
//
BOOL CRTFProc::FieldsReplace( LPCTSTR pszParam, int nOnce )
{
	long i = 0, k = 0, mm, np = 0, j, nn = 0;
	long nTmp;
	CHAR szBuf[64000], szTmp[2000], szFmt[200], ch;

    FIELDKEY sKey;
    FIELDVAL sVal;

    memset( &sKey, 0, sizeof(sKey));
    memset( &sVal, 0, sizeof(sVal));

	// LOGFILE( "  ... Inside -- ", "0" );
	fpSourceFile=fopen(szSourceFile,"rb");
	if (fpSourceFile == NULL) return FALSE;
	fseek( fpSourceFile, 0L, SEEK_END );
	nLmt = ftell( fpSourceFile );

	m_szBuf = new CHAR[nLmt+160000];
	if (m_szBuf==NULL)
	{
		fclose( fpSourceFile );
		return FALSE;
	}

	LOGFILE( "  ... Refreash -- ", szSourceFile );
	
	fseek( fpSourceFile, 0L, SEEK_SET );
	nTmp = fread( m_szBuf,sizeof(CHAR), nLmt+1, fpSourceFile );
	m_szBuf[nTmp] = 0;
	fclose( fpSourceFile );
	sprintf( szBuf, " F.Len %ul", nTmp );
// {\*\shpinst {\shprslt, {\pict\
// Adding Format adjustment	
	FormatAdjustEx( "$L:<*#-ctatorSignPic,"
		"$P:\\pard \\li0\\ri0\\widctlpar\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0\\itap0 ,"
		"$S:{\\rtf1,$I:{\\*\\shppict^{\\nonshppict^{\\pict\\pics", m_szBuf, nTmp );
	FormatAdjustEx( "$L:<*#-tendSignPic,"
		"$P:\\pard \\li0\\ri0\\widctlpar\\aspalpha\\aspnum\\faauto\\adjustright\\rin0\\lin0\\itap0 ,"
		"$S:{\\rtf1,$I:{\\*\\shppict^{\\nonshppict^{\\pict\\pics", m_szBuf, nTmp );
	// FormatAdjustEx( "$S:{\\stylesheet,", m_szBuf, nTmp );
	m_szBuf[nTmp] = 0;
	nLmt = nTmp;
    nn = 0;

	while ( i < nLmt )
	{
		if (CHKHIDDEN(i,m_szBuf))
		{
		    if (GetHiddenSegment( i, m_szBuf, nLmt, &sKey ) && ParseOneFieldKey( m_szBuf, &sKey ))
		    {
		        if ( GetOneSourceValue( m_szPrm, &sKey, &sVal, nOnce ) )
		        {
		            if ( GetOneFieldValue( i, m_szBuf, nLmt, &sKey, &sVal, false ) )
		            {
                        // Replace the source string with target
                        // Source: ptr->nSrcPos, ptr->nSrcLen
                        // Target: ptr->szTarget, ptr->nTgtLen
                        int nSrc, nTgt, nSrcPos;
                        LPTSTR pzSrc, pzTgt;

                        nSrcPos = ptr->nSrcPos;
                        nSrc = ptr->nSrcLen;
                        pzSrc = m_szBuf + nSrcPos;
                        nTgt = ptr->nTgtLen;
                        pzTgt = ptr->szTarget;
                        if ( sKey.nType==1 && nTgt>99 ) nTgt=99;

                        nn++;  // Count number of replacement
                        if ( nSrc == nTgt )
                            strncpy( pzSrc, pzTgt, nSrc );
                        else
                        {
                            memmove( pzSrc+nTgt, pzSrc+nSrc, nLmt-(nSrcPos+nSrc) );
                            memcpy( pzSrc, pzTgt, nTgt );
                            // memmove( m_szBuf+nFldsStart+nKVTarLen, m_szBuf+nFldsStart+nK,nLmt-(nFldsStart+nK) );
                            // memcpy( m_szBuf+nFldsStart, szKeyTarVal, nKVTarLen );
                            nTmp = nLmt+(nTgt-nSrc);
                            // np += (nKVTarLen-nK);
                            // nTmp = ReplaceOneFlds( nFldsStart, nKVSrcLen, nKVTarLen, nLmt, szKeyTarVal );

                            // Write target len in the field key if necessary
                            sprintf( szTmp, "%02d", nTgt);
                            if ( sKey.nType==1 ) strncpy( sKey.pzLenPos, szTmp, 2);
                            // For nOnce=1, we need cancel this field for furthering processing
                            if ( nOnce ) strncpy( sKey.pzNamePos, "##", 2)
                        }
		            }
		        }
		    }
		}
		else
		    ++i;
    }

	LOGFILE( "  ... Inside -- ", "2" );

	char Mark[64], Value[64];
	int nOpt, nRv=0;
	// char szBuf[64000];

	nOpt = BARC_OPT_WITH_CHAR;
	ps = strstr( pszParam, "|InsBarCode=True|" );
	if ( ps!=NULL )
	{ 
		ps += strlen("|InsBarCode=True|");
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
				InsertSignPic( szBuf, Mark, "", nRv );
			}
		}
	}

	CFile f;
	if (f.Open(szDestinationFile, 
		CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
	{
		f.Write( m_szBuf, nLmt );
		f.Close( );
		strcpy(szSourceFile,szDestinationFile);
		delete m_szBuf;
		return nn+1;
	}
	else
	{
		delete m_szBuf;
		return FALSE;
	}
}


BOOL CRTFProc::LinesCount(LPCTSTR pszInputParams, LPCTSTR pszOutputParams, DWORD nLength)
{
 return false;
}

BOOL CRTFProc::FieldsSearch(LPCTSTR pszInputParams, LPCTSTR pszOutputParams, DWORD nLength )
{
	char szTemp[1200];
    CString csText, csOutput;
	const DWORD nOutputSize = 200;
    char pszOutput[nOutputSize];

	fpDestinationFile=fopen(szSourceFile,"r");
	while(!feof(fpDestinationFile))
	{
  	    fgets(szTemp,1024,fpDestinationFile);
		csText=szTemp;
		GetSearchInOneRange( pszInputParams, csText, pszOutput, nOutputSize ); 
		csOutput = csOutput + pszOutput;
		*szTemp='\0';
	}
	fclose(fpDestinationFile);
	strncpy( ( char* )pszOutputParams, ( LPCTSTR )csOutput, nLength );
	return true;
}

BOOL CRTFProc::GenerateTextFile( LPCTSTR pszInputParams, LPCTSTR pszOutputParams, DWORD nLength)
{
  return false;
}

///////////////////////////////////////////////////////////////////////////////
//   Auxiliary procedures for FieldsReplace
///////////////////////////////////////////////////////////////////////////////
//   PadString : Pad <csTarget> string in length <nStrLength> with 
//               character <chPad>
//   <padType> : 0:Left   1:Right   2:Center   3:No Pad 
///////////////////////////////////////////////////////////////////////////////
BOOL CRTFProc::PadString( CString& csTarget, int nStrLength, int nPadType, char chPad )
{
	int n1, n2;

	if ( csTarget.GetLength( ) >= nStrLength ) 
	{
		csTarget = csTarget.Left( nStrLength );
		return TRUE;
	}
	if ( nPadType == 0 ) 
	{
		n1 = nStrLength - csTarget.GetLength( );
		n2 = 0;
	}
	else if ( nPadType == 1 )
	{
		n1 = 0;
		n2 = nStrLength - csTarget.GetLength( );
	}
	else if ( nPadType == 2 )
	{
		n1 = ( nStrLength - csTarget.GetLength( ) ) / 2;
		n2 = ( nStrLength - csTarget.GetLength( ) ) / 2 + ( nStrLength - csTarget.GetLength( ) ) % 2;
	}
	else 
		return FALSE;
	CString csTmp1( chPad, n1 );
	CString csTmp2( chPad, n2 );
	csTarget = csTmp1 + csTarget + csTmp2;
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//   Auxiliary procedures for FieldsReplace
///////////////////////////////////////////////////////////////////////////////
//   ScanString : to extract input string, and get KeyWord put in <pszKey>
//                get ValueStr put in <pszValue>
///////////////////////////////////////////////////////////////////////////////
BOOL CRTFProc::ScanString( LPCTSTR pszInputStr, int index, char* pszKey, char* pszValue )
{
    char ch, szTmp[256], *pos;
    int i = 0;

    if ( index < 0 ) return FALSE;

    pos = ( char* )pszInputStr;
    szTmp[0] = 0;
	while ( *pos == '|' ) pos++;
    while ( i++ < index )
    {
    	szTmp[0] = 0;
	    sscanf( pos, "%[^|]%c", szTmp, &ch );
	    pos += strlen( szTmp );
	    while ( *pos == '|' ) pos++;
    }
																 
    if ( szTmp[0] )
    {
	    pszKey[0] = 0;
	    pszValue[0] = 0;
	    sscanf( szTmp, "%[^=]%c%[^=]", pszKey, &ch, pszValue );
	    return true;
    }
    else
	    return false;
}

BOOL CRTFProc::DuplicationFile(FILE *SourceFile, FILE *DestFile)
{
	char szTemp[8002];
	long int nn = 0;

	rewind(SourceFile);
	rewind(DestFile);
	while ( (nn = fread( szTemp,1,8000, SourceFile ))>0 )
		fwrite( szTemp, 1, nn, DestFile );
	fflush( DestFile );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
//   Auxiliary procedures for FieldsSearch
///////////////////////////////////////////////////////////////////////////////
//   GetSearchInOneRange : Search one range and put the result in <pszOutput>
///////////////////////////////////////////////////////////////////////////////
BOOL CRTFProc::GetSearchInOneRange( LPCTSTR pszParameters, LPCTSTR pszRange, LPCTSTR pszOutput, DWORD nLength )
{
    char pszKey[100], pszValue[100];
	CString csBuf, csOutput, keyString, csTmp, csValue;
	int nKeyIndex, nPos;

	csOutput.Empty( );
	csBuf = pszRange;
	nKeyIndex = 1;
    while ( ScanString( pszParameters, nKeyIndex, pszKey, pszValue ) )
	{
   		nKeyIndex++;
	    
		keyString = pszKey;
		nPos = csBuf.Find( keyString );
		while ( nPos >= 0 )
		{
			csBuf = csBuf.Right( csBuf.GetLength( ) - nPos - keyString.GetLength( ) );
			//  Search for return to get one line
			nPos = csBuf.Find( "\r" );
			if ( nPos >= 0 ) 
			{
				csTmp = csBuf.Left(	nPos );
			    csBuf = csBuf.Right( csBuf.GetLength( ) - nPos );
			}
			else
				csTmp = csBuf;

			//  Search for "/" to erase instruction
			nPos = csTmp.Find( "/" );
			if ( nPos >= 0 ) 
				csTmp = csTmp.Left(	nPos );

			//  Search for ";" to separate different value
			nPos = csTmp.Find( ";" );
			while ( nPos >= 0 )
			{
				csValue = csTmp.Left( nPos );
				csValue.TrimLeft( );
				csValue.TrimRight( );
				if ( ! csValue.IsEmpty( ) )
   				    csOutput = csOutput + keyString + "=" + csValue + "|" ;
				csTmp = csTmp.Right( csTmp.GetLength() - nPos - 1 );
    		    nPos = csTmp.Find( ";" );
			}
			// Process last value
			csValue = csTmp;
			csValue.TrimLeft( );
			csValue.TrimRight( );
			if ( ! csValue.IsEmpty( ) )
			    csOutput = csOutput + keyString + "=" + csValue +"|" ;

    		nPos = csBuf.Find( keyString + " " );
		}
	}
	strncpy( ( char* )pszOutput, ( LPCTSTR )csOutput, nLength );

	return TRUE ;
}

BOOL CRTFProc::FieldsValue(LPCTSTR pszInputParams, LPCTSTR pszOutputParams, DWORD nLength)
{
	char szTemp[1200];
    CString csText, csOutput;
	const DWORD nOutputSize = 200;
    char pszOutput[nOutputSize];

	fpDestinationFile=fopen(szSourceFile,"r");
	while(!feof(fpDestinationFile))
	{
 	    fgets(szTemp,1024,fpDestinationFile);
		csText=szTemp;
		GetFieldValueInOneRange( pszInputParams, csText, pszOutput, nOutputSize ); 
		csOutput = csOutput + pszOutput;
		*szTemp='\0';
	}
	fclose(fpDestinationFile);
	if (!csOutput.IsEmpty())
		csOutput=csOutput.Left(csOutput.GetLength()-1);
	strncpy( ( char* )pszOutputParams, ( LPCTSTR )csOutput, nLength );
	return true;
}


BOOL CRTFProc::GetFieldValueInOneRange( LPCTSTR pszParameters, LPCTSTR pszRange, LPCTSTR pszOutput, DWORD nLength )
{
    char pszKey[100], pszValue[100];
	CString csBuf, csOutput, keyString, csTmp, csValue,csTag;
	int nKeyIndex, nPos,mPos,valuelength;

	csOutput.Empty( );
	csBuf = pszRange;
	nKeyIndex = 1;
    while ( ScanString( pszParameters, nKeyIndex, pszKey, pszValue ) )
	{
   		nKeyIndex++;
	    
		csTmp = csBuf;
		keyString = pszKey;
		nPos = csBuf.Find( keyString );
		while ( nPos >= 0 )
		{
			csTag = csTmp.Mid(nPos-5,5);
			csTmp = csTmp.Right( csTmp.GetLength( ) - nPos - keyString.GetLength( )-1 );
			valuelength = atoi((LPCTSTR)csTmp.Left(2));


			nPos = csTmp.Find( ">\\v0 " );
			if ( nPos >= 0 )
			{
				mPos = csTag.Find("<*");
				if (mPos >0)
				{
				 csValue = csTmp.Right( csTmp.GetLength()-nPos-5 );
				 csValue = csValue.Left(valuelength);
				 csValue.TrimLeft( );
				 csValue.TrimRight( );
				}
				else
				{
					csValue = csTmp.Left(nPos);
					csValue.TrimLeft();
					csValue.TrimRight();
				}
				csTag.Empty();
				if ( ! csValue.IsEmpty( ) )
   				    csOutput = csOutput + keyString + "=" + csValue+"|";
			}
    		nPos = csTmp.Find( keyString + " " );
		}
		csTmp.Empty();
	}

	strncpy( ( char* )pszOutput, ( LPCTSTR )csOutput, nLength );

	return TRUE ;
}


int CRTFProc::InsertSignPic( LPCTSTR signPicFileName, int signatoryID, int nChkOnly )
{
	BOOL ns, nTmp;
	TCHAR szTemp[200];

	fpSourceFile=fopen(szSourceFile,"rb");
	if (fpSourceFile == NULL) return FALSE;
	fseek( fpSourceFile, 0L, SEEK_END );
	nLmt = ftell( fpSourceFile );
	m_szBuf = new CHAR[nLmt+125000];
	if (m_szBuf==NULL)
	{
		fclose( fpSourceFile );
		return FALSE;
	}

	fseek( fpSourceFile, 0L, SEEK_SET );
	nTmp = fread( m_szBuf,sizeof(CHAR), nLmt+1, fpSourceFile );
	m_szBuf[nTmp] = 0;
	fclose( fpSourceFile );
	m_szBuf[nTmp] = 0;
	nLmt = nTmp;

	switch (signatoryID)
	{
	case 1:
		ns =  InsertSignPic( signPicFileName, "DictatorSignPic", "DictatorSignDate", nChkOnly );
		break;
	case 2:
		ns =  InsertSignPic( signPicFileName, "AttendSignPic", "AttendSignDate", nChkOnly );
		break;
	case 3:
		ns =  InsertSignPic( signPicFileName, "SupervisorSignPic", "SupervisorSignDate", nChkOnly );
		break;
	case 4:
		ns =  InsertSignPic( signPicFileName, "NormalPosition", "", nChkOnly );
		break;
	case 5:
		ns =  InsertSignPic( signPicFileName, "NormalPosition2", "", nChkOnly );
		break;
	case 6:
		ns =  InsertSignPic( signPicFileName, "NormalPosition3", "", nChkOnly );
		break;
	case 7:
		ns =  InsertSignPic( signPicFileName, "MarkPosition", "", nChkOnly );
		break;
	case 8:
		ns =  InsertSignPic( signPicFileName, "MarkPosition2", "", nChkOnly );
		break;
	case 9:
		ns =  InsertSignPic( signPicFileName, "MarkPosition3", "", nChkOnly );
		break;
	case 10:
		ns =  InsertSignPic( signPicFileName, "ContinuePosition", "", nChkOnly );
		break;
	case 11:
		ns =  InsertSignPic( signPicFileName, "CCPosition", "", nChkOnly );
		break;
	case 12:
		ns =  InsertSignPic( signPicFileName, "ReferSignPic", "ReferSignDate", nChkOnly );
		break;
	case 100:
		ns =  InsertSignPic( signPicFileName, "Append2End", "", nChkOnly );
		break;
	default:
		ns =  InsertSignPic( signPicFileName, "DictatorSignPic", "DictatorSignDate", nChkOnly );
		break;
	}

	if (nChkOnly & 0x0001)
	{
		delete m_szBuf;
		if ( ns>0 ) return ns+1; else return -1;
	}
	
	sprintf( szTemp, "%d-Insert", ns );
	LOGFILE( szTemp, szDestinationFile );

	CFile f;
	if (f.Open( szDestinationFile, 
		CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive) )
	{
		f.Write( m_szBuf, nLmt );
		f.Close( );
		strcpy(szSourceFile,szDestinationFile);
		delete m_szBuf;
		return ns+1;
	}
	else
	{
		Sleep(1000);
		if (f.Open( szDestinationFile, 
					CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive) )
		{
			f.Write( m_szBuf, nLmt );
			f.Close( );
			strcpy(szSourceFile,szDestinationFile);
			delete m_szBuf;
			return ns+1;
		}

		sprintf( szTemp, "File Write Failed" );
		LOGFILE( szTemp, szDestinationFile );
		delete m_szBuf;
		return -1;
	}
	return TRUE;
}


int CRTFProc::InsertSignPic( LPCTSTR signPicFileName, LPTSTR pzPic, LPTSTR pzDate, int nChkOnly )
{
	int   nTmp, pn;
	TCHAR szTemp[32]={0}, szKeyString[32]={0}, szKeyString2[32]={0};

	strcpy( szKeyString, pzPic );
	strcpy( szKeyString2, pzDate );
	
	LOGFILE( szKeyString, szSourceFile );
	
	char szBuf[120000], szKeyName[200];  
	FILE *fpSignPicFileName;
	long i = 0, k = 0;
	long nLev, nn, nOpt=0, ns;
	long nFldsStart, nFldsLen;
	CHAR ch, *pos;
	CTime ts = CTime::GetCurrentTime();

	ns = 0;
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
			nn = fread(szBuf,1,120000,fpSignPicFileName);
			szBuf[nn]=0;
			fclose(fpSignPicFileName);
		}
		strcpy( m_szBuf+i, "\\par \r\n" );
		strncpy( m_szBuf+i+7, szBuf, nn );
		strcpy( m_szBuf+i+nn+7, "}" );
		nLmt = i+nn+8;
		ns = 1;
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
			
			if ( (m_szBuf[i]!='{' || !CHKHIDDEN(i,m_szBuf)) && ch=='{' )
			{
				if ( m_szBuf[i]=='{' ) i++;
				while (m_szBuf[i]=='\n'||m_szBuf[i]=='\r') i++;
			
				while ( m_szBuf[i]=='\\' && ( m_szBuf[i+1]!='\'' && m_szBuf[i+1]!='_' 
				    && m_szBuf[i+1]!='{' && m_szBuf[i+1]!='}' && strncmp(m_szBuf+i+1,"par",3)))
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
			ch=' ';	szKeyName[0]=0;
			sscanf( m_szBuf+k+pn,"%199[^ >\n]%c", szKeyName, &ch );

			if (ch!='>') goto Loop;
			
			if ( strncmp(szKeyName, szKeyString, strlen(szKeyString))==0 ) {nOpt=1;}
			else if	( szKeyString2[0] && strcmp(szKeyName, szKeyString2)==0 ) {nOpt=2;}
			else goto Loop;
			
			// Cancel this field inside Document for further replacement
			m_szBuf[k+2]='-';
			ns++;

			// Get the inserting Document Content
			if ( nOpt==2 )
			{
				sprintf( szBuf, "%s", (LPCTSTR)ts.Format(" %B %d, %Y %H:%M"));
				nn = strlen( szBuf );
			}
			else if ( signPicFileName[0]=='*')
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
				nn = fread(szBuf,1,120000,fpSignPicFileName);
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
			nLmt += nn;
			m_szBuf[ nLmt ]=0;
			i++;
		}
		else
			i++;
Loop:	;	
	}

	return ns;
}


/*
BOOL CRTFProc::InsertSignPic( LPCTSTR signPicFileName, int signatoryID, int nChkOnly )
{
	char szTemp[10000], szTmpFile[250], szPic[1200], szBuf[400], *ps, *tps, ch;  
	FILE *fpSignPicFileName;
	CString csBuf, csKeyString, csKeyString2, csTmp;
    int nCnt=0, nOpt, nLen;
	BOOL Success=false;
	CTime ts = CTime::GetCurrentTime();
				
	strncpy( szTmpFile, szSourceFile, sizeof(szTmpFile)-1);
	szTmpFile[strlen(szTmpFile)-1]='_';
	szTmpFile[strlen(szTmpFile)-2]='_';

	fpSourceFile=fopen(szSourceFile,"rb");
	if (fpSourceFile == NULL)
		return false;

	if ( (nChkOnly & 0x0001)==0 )
	{
		fpTempFile=fopen(szTmpFile,"wb");
		if (fpTempFile==NULL)
		{
			fclose(fpSourceFile);
			return false;
		}
	}

	csKeyString2.Empty();
	switch (signatoryID)
	{
	case 1:
		csKeyString="<*DictatorSignPic>";
		csKeyString2="<*DictatorSignDate>";
		break;
	case 2:
		csKeyString="<*AttendSignPic>";
		csKeyString2="<*AttendSignDate>";
		break;
	case 3:
		csKeyString="<*SupervisorSignPic>";
		csKeyString2="<*SupervisorSignDate>";
		break;
	case 4:
		csKeyString="<*NormalPosition>";
		break;
	case 5:
		csKeyString="<*NormalPosition2>";
		break;
	case 6:
		csKeyString="<*NormalPosition3>";
		break;
	case 7:
		csKeyString="<*MarkPosition>";
		break;
	case 8:
		csKeyString="<*MarkPosition2>";
		break;
	case 9:
		csKeyString="<*MarkPosition3>";
		break;
	case 10:
		csKeyString="<*ContinuePosition>";
		break;
	case 11:
		csKeyString="<*CCPosition>";
		break;
	case 12:
		csKeyString="<*ReferSignPic>";
		csKeyString2="<*ReferSignDate>";
		break;
	default:
		csKeyString="<*DictatorSignPic>";
		csKeyString2="<*DictatorSignDate>";
		break;
	};

	szTemp[0]=0;
	while (!feof(fpSourceFile))
	{
		if (fgets(szTemp,1024*9,fpSourceFile)==NULL) break;
		szTemp[1024*9]=0;
		nOpt = 0; ps = NULL;

		ps=strstr(szTemp, (LPCTSTR)csKeyString);
		if (ps!=NULL) { nOpt=1;	++nCnt; nLen=strlen(csKeyString); }
		else 
		{
			if (!csKeyString2.IsEmpty()) ps=strstr(szTemp, (LPCTSTR)csKeyString2);
			if (ps!=NULL) { nOpt=2; nLen=strlen(csKeyString2); }
		}

		if (nChkOnly & 0x0001)
		{
		}
		else if ( ps==NULL)
		{
			fputs((LPCTSTR)szTemp, fpTempFile);
		}
		else
		{
			ps[2]='#'; ps[3]='-'; ps += nLen;

			// Search the location for insertion, more detailed
			// while ( ps[0] && ps[0]!=' ' && ps[0]!='\n' && strnicmp(ps,"\\par",4) ) ++ps;
			while ( ps[0] && (ps[0]!='}') && ( ps[0]!='\\' || ps[1]!='v' || ps[2]!='0' )) ++ps;
			if ( ps[0]=='}' ) ++ps; else ps+=3;
			if ( ps[0]==' ' ) ++ps;

			while (ps[0]=='\n'||ps[0]=='\r') ps++;
			if ( ps[0]=='{' ) ps++; 
			while (ps[0]=='\n'||ps[0]=='\r') ps++;
			
			while ( ps[0]=='\\' && ( ps[0]!='\'' && ps[1]!='_' && strncmp(ps+1,"par",3)))
			{
				while (ps[0] && ps[0]!=' ') ps++;
				ps++;
				while ( ps[0]=='\n' || ps[0]=='\r' ) ps++;
			}

			if ( ps[0]==0 ) { fputs((LPCTSTR)szTemp, fpTempFile); continue; }

			ch=ps[0]; ps[0]=0;
			fputs((LPCTSTR)szTemp, fpTempFile);
			ps[0]=ch;
			
			if (nOpt==1)
			{
				// Insert Actual Value instead of File
				if ( signPicFileName[0]=='*')
				{
				   sprintf(szBuf," %s ",(LPCTSTR)signPicFileName+1 ); 
				   fputs( szBuf, fpTempFile);
				   Success=true;
				}
				else
				{
				   fpSignPicFileName=fopen(signPicFileName,"r");
				   if (fpSignPicFileName != NULL)
				   {
					  while (!feof(fpSignPicFileName))
					  {
						fgets(szPic,1024,fpSignPicFileName);
						tps = strstr( szPic, "\\rtf" );
						if ( tps!=NULL ) { tps[1]='x'; tps[2]='x'; tps[3]='x'; }
						fputs( szPic, fpTempFile );
						*szPic='\0';
					  }
					  fclose( fpSignPicFileName );
					  Success=true;
				   }
				}
			}
			else if (nOpt==2)
			{
				fputs( (LPCTSTR)ts.Format(" %B %d, %Y %H:%M"), fpTempFile);
			}
			fputs((LPCTSTR)ps, fpTempFile);
		}

		*szTemp='\0';
	}

	fclose(fpSourceFile);
	if ( (nChkOnly & 0x0001)==0 ) fclose(fpTempFile);

	if (nChkOnly & 0x0001)
	{
		if ( nCnt>0 ) return 1; else return -1;
	}

	int nn=0;
	while (nn<3)
	{
		++nn;
		Sleep( 500 );
		if ( CopyFile( szTmpFile, szDestinationFile, false ) ) break;
	}

	if ( nn<3 )
	{
		DeleteFile( szTmpFile );
		if ( Success ) return 1;  else return -1;
	}
	else 
		return false;
	
}
*/


/*
BOOL CRTFProc::GetRTFHeader()
{
	FILE * fpRTFHeader;
	int ch;
	int ec;
	int cGroup=0;
	char szHeaderFile[255];

	strncpy(szHeaderFile,szDestinationFile,strlen(szDestinationFile)-4);

	szHeaderFile[strlen(szDestinationFile)-4]='\0';
	strcat(szHeaderFile, ".hdr");
	szHeaderFile[strlen(szDestinationFile)]='\0';

	fpSourceFile=fopen(szSourceFile,"r");
	if (fpSourceFile!=NULL)
	{
		fpRTFHeader=fopen(szHeaderFile,"w");

		if (fpRTFHeader==NULL)
		{
			fclose(fpSourceFile);
			return false;
		}

		bool Inputchar=true;
		bool findfooter=false;
		bool findheader=false;
		bool EndHeader = false;
		bool EndFooter = false;
		bool Addflag = false;
		bool savefile=false;

		while (((ch = getc(fpSourceFile)) != EOF)&&Inputchar)
		{
			switch (ch)
			{
			case '{':
				cGroup++;	
 				if (findheader && (cGroup==2))
					Addflag = true;
				else
					fputc(ch,fpRTFHeader);
				break;
			case '}':
				cGroup--;	
 				fputc(ch,fpRTFHeader);
				if (findheader && (cGroup==1))
					EndHeader = true;
				if (findfooter && (cGroup==1))
					EndFooter = true;
					// Inputchar = false;
				break;
			case '\\':
				ec = ParseRtfKeyword(fpSourceFile);
				switch(ec)
				{
				case ecEndOfFile:
					fclose(fpSourceFile);
					fclose(fpRTFHeader);
					return false;
					break;
				case ecFindFooter:
					savefile=true;
					findfooter=true;
					break;
				case ecFindHeader:
					savefile=true;
					findheader=true;
					break;
				}
				if (EndHeader && (!findfooter))
					Inputchar = false;
				TempString = "\\"+TempString;
				if (Addflag)
				{
					TempString = "{"+TempString;
					Addflag =false;
				}
				if (Inputchar)
					fputs((LPCTSTR)TempString,fpRTFHeader);
				break;
			default:
 				fputc(ch,fpRTFHeader);
			}       // switch
	    }       

		fclose(fpSourceFile);
		fclose(fpRTFHeader);
		if (!savefile)
			DeleteFile(szHeaderFile);

		return true;
	}
	else
		return false;
}
*/

#define  MAX_SEGMENT		63

BOOL CRTFProc::GetRTFHeader()
{
	FILE * fpRTFHeader;
	int ch, lch;
	int cGroup=0;
	char szHeaderFile[255];

	strncpy(szHeaderFile,szDestinationFile,strlen(szDestinationFile)-4);
	szHeaderFile[strlen(szDestinationFile)-4]='\0';
	strcat(szHeaderFile, ".hdr");
	szHeaderFile[strlen(szDestinationFile)]='\0';

	fpSourceFile=fopen(szSourceFile,"rb");
	if (fpSourceFile!=NULL)
	{
		fpRTFHeader=fopen(szHeaderFile,"wb");

		if (fpRTFHeader==NULL)
		{
			fclose(fpSourceFile);
			return false;
		}

		char szLast[MAX_SEGMENT+1], *ps;
		bool savefile=false;
		int nLev = 0, nMark = 1;
		bool AfterParse = true;
		bool InputChar = true;
		bool InHeadFoot = false, OutHeadFoot = false;

		for(int n=0; n<MAX_SEGMENT; n++) szLast[n]=' ';
		lch=' ';

		//bool findfooter=false;
		//bool findheader=false;
		//bool EndHeader = false;
		//bool EndFooter = false;
		//bool Addflag = false;
		while ((ch = getc(fpSourceFile)) != EOF) 
		{
			if ( ch>30 )
			{
				if (ch==' '||(ch=='\\'&&lch!='*')||ch=='{'||ch=='}')
				{   
					//  check the {\  
					szLast[MAX_SEGMENT]=0;
					ps = strstr( szLast, "{\\" );
					if (ps!=NULL)
					{
						if ( strncmp( ps, "{\\header", 8) == 0 ||
							 strncmp( ps, "{\\footer", 8) == 0 )
						{
							nMark = nLev-1;
							savefile = true;
							InputChar = true;
							InHeadFoot = true;
							OutHeadFoot = false;
						}
						else if ( strncmp(ps, "{\\shp", 5) == 0 && ps[5]==0 && !InHeadFoot )
						{
							nMark = ( savefile ? nLev-1: nLev-2 );
							// nMark = 1; // Force to place it in the second level
							savefile = true;
							InputChar = true;
							InHeadFoot = true;
							OutHeadFoot = false;
						}
						else
							InputChar = (!OutHeadFoot) && InputChar;

						if (InputChar)
						{
							fputs((LPTSTR)ps,fpRTFHeader);
							AfterParse = true;
						}
						ps[0] = ' ';
					}
					else
					{	
						InputChar = (!OutHeadFoot) && InputChar;
						ps = strrchr( szLast, '{' );
						if ( ps !=NULL && (ch!='\\' || lch!='{'))
						{
							if ( InputChar ) fputs((LPTSTR)ps,fpRTFHeader);
							AfterParse = true;
							ps[0] = ' '; 
						}
					}
				}
				
				memmove(szLast, szLast+1, MAX_SEGMENT-1);
				szLast[MAX_SEGMENT-1]=ch;
				lch = ch;
			}

			if (ch=='{')
			{
				++nLev;
				/*if (!InHeadFoot)*/ AfterParse = false;
			}
			else if (ch=='}')
			{
				--nLev;
				if ( nLev==nMark && InHeadFoot )
				{
					InHeadFoot = false;
					OutHeadFoot = true;
				}
			}

			if ( AfterParse && InputChar )
				fputc(ch,fpRTFHeader);
		}

		fclose(fpRTFHeader);
		fclose(fpSourceFile);
		if (!savefile)
			DeleteFile(szHeaderFile);

		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////
// This Function use to scan source file to find the header 
// segment and footer segment and the logo sharp,
// which is the important part of the template document.
// If exist header or footer or log return true else
// return false
//
BOOL CRTFProc::HeaderExist()
{
	int ch, lch;

	fpSourceFile=fopen(szSourceFile,"rb");
	if (fpSourceFile!=NULL)
	{
		char szLast[MAX_SEGMENT+1], *ps;
		bool bHeadExist=false;

		for(int n=0; n<MAX_SEGMENT; n++) szLast[n]=' ';
		lch=' ';

		while ((ch = getc(fpSourceFile)) != EOF) 
		{
			if ( ch>30 )
			{
				if (ch==' '||(ch=='\\'&&lch!='*')||ch=='{'||ch=='}')
				{   
					//  check the {\  
					szLast[MAX_SEGMENT]=0;
					ps = strstr( szLast, "{\\" );
					if (ps!=NULL)
					{
						if ( strncmp( ps, "{\\header", 8) == 0 ||
							 strncmp( ps, "{\\footer", 8) == 0 )
						{
							bHeadExist = true;
							break;
						}
						else if ( strncmp(ps, "{\\shp", 5) == 0 && ps[5]==0 && !bHeadExist )
						{
							bHeadExist = true;
							break;
						}
					}
				}
				
				memmove(szLast, szLast+1, MAX_SEGMENT-1);
				szLast[MAX_SEGMENT-1]=ch;
				lch = ch;
			}
		}

		fclose(fpSourceFile);
		return bHeadExist;
	}
	else
		return false;
}

BOOL CRTFProc::SetRTFHeader()
{
	if (HeaderExist()) return false;
	char szTmp[2100], pzDir[256]={0}, pzFile[256]={0};  
	char szHeaderFile[255]={0};
	FILE *fpRTFHeader;
	int ch,n;
	int cGroup=0, nHeadFoot=0;
	bool Inputchar=true;
	bool FindFontTbl=false;
	bool InHeadFoot=false;
	bool FindShape=false;
	CRTFFont* pRTFFont;
	long int fpos = 0, tpos, fpos2 = 0;

	strncpy(szHeaderFile,szOldFile,strlen(szOldFile)-4);
	szHeaderFile[strlen(szOldFile)-4]='\0';
	strcat(szHeaderFile, ".hdr");
	szHeaderFile[strlen(szOldFile)]='\0';

	WIN32_FIND_DATA finddata;
	HANDLE hresult;
	int ec;

	if ((hresult=FindFirstFile(szHeaderFile,&finddata))==INVALID_HANDLE_VALUE)
	{
		FindClose(hresult);
		return false;
	}

	GetTempPath(256, pzDir);

	fpRTFHeader=fopen(szHeaderFile,"rb");
	if (fpRTFHeader!=NULL)
	{
		//fpTempFile = tmpfile();
		//pzFile = tmpnam( "GF__" );
		//pzFile = tempnam( NULL, "GF__" );

		if (GetTempFileName(pzDir, "GF__", 0, pzFile))
			fpTempFile=fopen(pzFile, "w+b");
		else
			fpTempFile=NULL;

		if (fpTempFile==NULL)
		{
			fclose(fpRTFHeader);
			return false;
		}

		fontname = "";
		while (Inputchar&&((ch = getc(fpRTFHeader))!=EOF))
		{
			fputc(ch,fpTempFile);
			switch(ch)
			{
			case '{':
				cGroup++;	
				break;
			case '}':
				cGroup--;
				if (FindFontTbl && (cGroup==2))
				{
					AddRtfFontTable();				
				}
				if (FindFontTbl && (cGroup==1))
				{
					FindFontTbl = false;
					// Inputchar = false;
				}
				break;
			case '\\':
				ec = ParseRtfKeyword(fpRTFHeader);
				if (ec == ecFonttbl) FindFontTbl = true;
				else if (ec == ecShape) FindShape = true; 
				fputs((LPCTSTR)TempString,fpTempFile);
				break;
			case 0x0d:
			case 0x0a:          // cr and lf are noise characters...
				  break;
			default:
				if (FindFontTbl && (cGroup==3))
					fontname = fontname + (char)ch;
			}       // switch
		} 

		while (fgets(szTmp,2048,fpRTFHeader)!=NULL)
		{
			fputs(szTmp,fpTempFile);
			*szTmp='\0';
		}
		fclose(fpRTFHeader);

		Inputchar = true;
		cGroup=0;
		FindFontTbl = false;
		BOOL bFindViewkind = false;

		fpSourceFile=fopen(szSourceFile,"rb");
		if (fpSourceFile==NULL)
		{
			// rmtmp();
			fclose(fpTempFile);
			DeleteFile( pzFile );

			for (n = 0; n < HeadFontArray.GetSize(); n++)
			{
				pRTFFont = HeadFontArray[n];
				delete pRTFFont;
			}
			return false;
		}

		while (Inputchar&&((ch = getc(fpSourceFile)) != EOF))
		{
			switch (ch)
			{
			case '{':
				cGroup++;	
				break;
			case '}':
				cGroup--;
				if (FindFontTbl && (cGroup==2))
				{
					RebuildRtfFont();				
				}
				if (FindFontTbl && (cGroup==1))
				{
					FindFontTbl = false;
					fpos = ftell(fpSourceFile );
				}
				if (InHeadFoot && (cGroup==1))
				{
					InHeadFoot = false;
					fpos = ftell(fpSourceFile );
				}
				break;
			case '\\':
				tpos = ftell(fpSourceFile);
				ec = ParseRtfKeyword(fpSourceFile);
				switch(ec)
				{
				case (ecViewkind):
					// Inputchar = false;
					bFindViewkind = true;
					fpos2 = tpos;
					TempString = "\\"+TempString;
					// fputs((LPCTSTR)TempString,fpTempFile);
					break;
				case (ecFonttbl):
					FindFontTbl = true;
					break;
				case (ecShape):
				case (ecFindHeader):
				case (ecFindFooter):
					InHeadFoot = true;
					++nHeadFoot;
					break;
				}
				break;
			case 0x0d:
			case 0x0a:          // cr and lf are noise characters...
				  break;
			default:
				if (FindFontTbl && (cGroup==3))
					fontname = fontname + (char)ch;
			}       // switch
		} 

		if (!bFindViewkind || nHeadFoot>0 )
			fseek(fpSourceFile, fpos, SEEK_SET );
		else
		{
			if ( FindShape )
				fseek(fpSourceFile, fpos2+9, SEEK_SET );
			else
				fseek(fpSourceFile, fpos2-1, SEEK_SET );
		}

/*		while ((ch=fgetc(fpSourceFile))!=EOF)
		{
			if (ch!=0x0d)
	  		fputc(ch,fpTempFile);

			if (ch=='\\')
			{
				ec =ParseRtfKeyword(fpSourceFile); 
				fputs((LPCTSTR)TempString,fpTempFile);

				if (ec==ecFont)
				{
					for (n = 0; n < HeadFontArray.GetSize(); n++)
					{
						pRTFFont = HeadFontArray[n];
						if (strcmp(fontnumber,pRTFFont->Oldfontnumber)==0)
						{
							fputs("\\f",fpTempFile);
							fputs(pRTFFont->fontnumber,fpTempFile);
							break;
						}
					}
				}
			}
		}
*/
		int bDone;
		while ((ch=fgetc(fpSourceFile))!=EOF)
		{
			if (ch!=0x0d)
	  		fputc(ch,fpTempFile);

			if (ch=='\\')
			{
				ec = ParseRtfKeyword(fpSourceFile); 
				// fputs((LPCTSTR)TempString,fpTempFile);

				/*/ Removing following part temporarily, it may be added later for
				//  handling the case of changing font name, need more testing */
				if (ec==ecFont)
				{
					bDone = false;
					for (n = 0; n < HeadFontArray.GetSize(); n++)
					{
						pRTFFont = HeadFontArray[n];
						if (strcmp(fontnumber,pRTFFont->Oldfontnumber)==0)
						{
							bDone = true;
							fputc('f',fpTempFile);
							fputs(pRTFFont->fontnumber,fpTempFile);
							if (TempString.Right(1)==" ")
								fputc(' ',fpTempFile);
							break;
						}
					}
					if (!bDone)
						fputs((LPCTSTR)TempString,fpTempFile);
				}
				else
					fputs((LPCTSTR)TempString,fpTempFile);
			}
		}
		fclose(fpSourceFile);

		fpDestinationFile=fopen(szDestinationFile,"wb");
		if (fpDestinationFile==NULL)
		{
			// rmtmp();
			for (n = 0; n < HeadFontArray.GetSize(); n++)
			{
				pRTFFont = HeadFontArray[n];
				delete pRTFFont;
			}
			fclose(fpTempFile);
			DeleteFile( pzFile ); 
			return false;
		}

		DuplicationFile(fpTempFile,fpDestinationFile);
		fclose(fpDestinationFile);

		//	strcpy(szSourceFile,szDestinationFile);
		// rmtmp();
		fclose(fpTempFile);
		// Test Only
		DeleteFile( pzFile ); 

		for (n = 0; n < HeadFontArray.GetSize(); n++)
		{
			pRTFFont = HeadFontArray[n];
			delete pRTFFont;
		}
		return true;		
	}
	else
		return false;
}


int CRTFProc::CheckValidate( char* szFile )
{
	char szTmp[2100];
	BOOL ss;
	FILE *fp;

	strcpy( szTmp, szFile );
	strupr( szTmp );
	if (strstr( szTmp, ".LST" )!=NULL ) return 1;
	if (strstr( szTmp, ".TXT" )!=NULL ) return 1;


	fp=fopen( szFile,"r" );
	if (fp==NULL)
	{
		Sleep(500);
		fp=fopen( szFile,"r" );
		if (fp==NULL)
		{
			Sleep(500);
			fp=fopen( szFile,"r" );
			if (fp==NULL)
			{
				Sleep(5000);
				fp=fopen( szFile,"r" );
				if ( fp==NULL ) return -1;
			}
		}
	}

	szTmp[0]=0;
	fread( szTmp, 1, 10, fp );
	ss = (strncmp( szTmp, "{\\rtf", 5 )==0);
	fclose( fp );
	if (ss) return 1; else return 0;
}


/* All special process for RTF file format
   new function will be added to it

#define		RTF_EXTFUNC_RMVREST			0x00000001
  remove the rest part from current position
#define		RTF_EXTFUNC_BLNKCHK			0x00000002
  check the special blank inside document
#define		RTF_EXTFUNC_RMVKEYW			0x00000004
  remove the keyword
#define		RTF_EXTFUNC_RMVSPEC1		0x00000008
  remove some new non-compatible codes added by Word 2003
#define		RTF_EXTFUNC_RMVSPACE		0x00000010

*/

int CRTFProc::ExtendFunction( LPTSTR pszKey, int nOpt )
{
	long nLmt, nTmp, i = 0, j, nLev = 0;
	int k, nn, nRes = 0;
	bool ss, inHF = false;
	CHAR *m_szBuf;

	int nFont=-1;
	char szTmp[16]={0};
	bool bBody = false;


	fpSourceFile=fopen(szSourceFile,"rb");
	if (fpSourceFile == NULL) return -1;

	fseek( fpSourceFile, 0L, SEEK_END );
	nLmt = ftell( fpSourceFile );

	m_szBuf = new CHAR[nLmt+8000];
	if (m_szBuf==NULL)
	{
		fclose( fpSourceFile );
		return -1;
	}
	fseek( fpSourceFile, 0L, SEEK_SET );
	nTmp = fread( m_szBuf,sizeof(CHAR), nLmt+1, fpSourceFile );
	m_szBuf[nTmp] = 0;
	fclose( fpSourceFile );

	nn = strlen( pszKey );
	nLmt = nTmp;
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
		else if ( ( nOpt & (RTF_EXTFUNC_RMVSPACE | RTF_EXTFUNC_CHKSPACE) ) && (!inHF) && m_szBuf[i]==' ' )
		{
			int kk;
			j = i; k = 1;
			if ( pszKey==NULL || atoi(pszKey)==0 ) kk=15; else kk = atoi(pszKey); 

			while ( i<nLmt && m_szBuf[++i]==' ' ) ++k;
			if ( k > kk )
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
			{
				i=j;
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
			if (  strncmp( m_szBuf+i, "{\\info", 6)==0 || strncmp( m_szBuf+i, "{\\header", 8)==0 ||
				  strncmp( m_szBuf+i, "{\\footer", 8)==0 )
				  bBody = true;
			else if ( bBody && (strncmp(m_szBuf+i, "\\f", 2)==0) && isdigit( m_szBuf[i+2] ) )
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
				nLmt -= (nn);
				++nRes;
			}
			else if ( bBody && strncmp( m_szBuf+i, "\\deleted", 8)==0 )
			{
				if ( m_szBuf[i+8]==' ' && (m_szBuf[i+9]=='\\' || m_szBuf[i+9]<32 || m_szBuf[i-1]=='{')) nn = 9; else nn = 8;
				memmove( m_szBuf+i, m_szBuf+i+nn, nLmt-i-nn );
				nLmt -= (nn);
				++nRes;
			}
		}
		++i;
	}

	m_szBuf[nLmt]=0;

	//Modified by solomon, 03/31/2007
	fpDestinationFile=fopen(szDestinationFile,"w");
	if ( fpDestinationFile == NULL ) return -1;
	fwrite(m_szBuf, 1, nLmt, fpDestinationFile);
	fclose(fpDestinationFile);
	// CopyFile(szSourceFile, szDestinationFile, false);
	strcpy( szSourceFile, szDestinationFile );

	delete m_szBuf;
	return nRes;
}



	
//
// Insert a barcode	in RTF File
// Definion of pszFlds
// Opt=<nn>|<Mark>=<String>|...|<Mark>=<String>
//   atmost four barcode
void CRTFProc::InsertRTFBarCode( LPTSTR pszFlds )
{
	BOOL ns;
	int nTmp, pn;
	TCHAR szTemp[200];

	fpSourceFile=fopen(szSourceFile,"rb");
	if (fpSourceFile == NULL) return;
	fseek( fpSourceFile, 0L, SEEK_END );
	nLmt = ftell( fpSourceFile );
	m_szBuf = new CHAR[nLmt+64000];
	if (m_szBuf==NULL)
	{
		fclose( fpSourceFile );
		return;
	}

	fseek( fpSourceFile, 0L, SEEK_SET );
	nTmp = fread( m_szBuf,sizeof(CHAR), nLmt+1, fpSourceFile );
	m_szBuf[nTmp] = 0;
	fclose( fpSourceFile );
	m_szBuf[nTmp] = 0;
	nLmt = nTmp;

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
			InsertSignPic( szBuf, Mark, "", nRv );
		}
	}

	CFile f;
	if (f.Open(szDestinationFile, 
		CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
	{
		f.Write( m_szBuf, nLmt );
		f.Close( );
		strcpy(szSourceFile,szDestinationFile);
	}
	delete m_szBuf;
}


HGLOBAL hpBuf;
LPSTR pBuf; 
FILE *fp;


long CALLBACK OutConvert(long cchBuff, long nPercent)
{
	pBuf = ( char* ) GlobalLock( hpBuf );
	fwrite( pBuf, 1, cchBuff, fp );
	GlobalUnlock( hpBuf );

	return (long)0;
}

long CALLBACK InConvert(long rgfOptions, long a)
{
	pBuf = ( char* ) GlobalLock( hpBuf );
	long nActualByte=fread( pBuf, 1, 1024, fp );
	GlobalUnlock( hpBuf );
	return nActualByte;
}

/*
#define		RTF_EXP_HTML		1
#define		RTF_EXP_TEXT		2
#define		RTF_EXP_LYTX		3
#define		RTF_EXP_WORD		4
#define		RTF_EXP_WP51		5
*/

BOOL CRTFProc::RtfExport( int nFmt, LPTSTR pzRes ) 
{
	BOOL ss = false, bc = false, bl = false;
	char szLFile[256]={0}, pzDir[256]={0}, pzFile[256]={0}, *ps;  
	char m_szExt[124]={0}, szTemp[512];
	
	if ( strcmp( szSourceFile, szDestinationFile )==0 )
	{
		GetTempPath(256, pzDir);
		if (GetTempFileName(pzDir, "HTF__", 0, pzFile))
		{
			ss = CopyFile( szSourceFile, pzFile, false );
			Sleep( 500 );
		}
		
		if (!ss)
		{
			strcpy( m_szExt, "Create Temp File Failed" ); goto Endx;
		}
		else
		{
			ss = false; bc = true;
			strcpy( szSourceFile, pzFile );
		}
	}

	// Beep(500,400);

	typedef int (PASCAL * lpInitConverter32)(HWND hwndWord, char* m_sModule);
	lpInitConverter32 InitConverter32;
 
	typedef long (PASCAL *PFN_RTF)(long, long);
	typedef short (PASCAL * lpRtfToForeign32)(HANDLE ghszFile, IStorage *pstgForeign,
		     HANDLE ghBuff, HANDLE ghszClass, PFN_RTF lpfnIn);
	lpRtfToForeign32 RtfToForeign32;

	typedef short (PASCAL * lpForeignToRtf32)( HANDLE ghszFile, IStorage* pstgForeign, 
		HANDLE ghBuff, HANDLE ghszClass, HANDLE ghszSubset, PFN_RTF lpfnOut );
	lpForeignToRtf32 ForeignToRtf32;

	typedef short (PASCAL * lpIsFormatCorrect32)(HANDLE haszFile, HANDLE haszClass);
	lpIsFormatCorrect32 IsFormatCorrect32;

	typedef long (PASCAL * lpCchFetchLpszError)( long fce, char *lpszError, long cb );
	lpCchFetchLpszError CchFetchLpszError;

	typedef void (PASCAL * lpGetReadNames)(HANDLE haszClass, HANDLE haszDescrip, HANDLE haszExt );
	lpGetReadNames GetReadNames;

	typedef void (PASCAL * lpGetWriteNames)(HANDLE haszClass, HANDLE haszDescrip, HANDLE haszExt );
	lpGetWriteNames GetWriteNames;

	// HINSTANCE hLibrary;
	LPTSTR pClass, pDescrip, pExt;
	HGLOBAL hpFileName, hpClass, hpDescrip, hpExt;
	// char m_szModule[32];
	long result;

	fp = fopen( szSourceFile, "r");
	if (fp==NULL) { strcpy( m_szExt, "Open Source File Failed" ); goto Endx; } 

	// Critic Section
	EnterCriticalSection(&CriticalSection); 

	if ( hLibrary==NULL )
	{
		if ( nFmt == RTF_EXP_WP51 )
			hLibrary = LoadLibrary("WPFT532.cnv");
		else if ( nFmt == RTF_EXP_HTML )
			hLibrary = LoadLibrary("HTML32.cnv");
		else if ( nFmt == RTF_EXP_LYTX )
			hLibrary = LoadLibrary("TXTLYT32.cnv");
		else if ( nFmt == RTF_EXP_WORD )
			hLibrary = LoadLibrary("MSWRD832.cnv");
		// hLibrary = LoadLibrary("WRITE32.cnv");
		// hLibrary = LoadLibrary("recovr32.cnv");
		if ( hLibrary==NULL )
		{
			GetModuleFileName( GetModuleHandle(_T("GlueFiles")), szLFile, 256);
			ps = strrchr( szLFile, '\\' );
			if ( ps != NULL )
			{
				++ps;
				if ( nFmt == RTF_EXP_WP51 )	strcpy(ps, _T("\\WPFT532.cnv"));
				else if ( nFmt == RTF_EXP_HTML ) strcpy(ps, _T("HTML32.cnv"));
				else if ( nFmt == RTF_EXP_LYTX ) strcpy(ps, _T("TXTLYT32.cnv"));
				else if ( nFmt == RTF_EXP_WORD ) strcpy(ps, _T("MSWRD832.cnv"));
				hLibrary = LoadLibrary( szLFile );
			}
		}

		if ( hLibrary!=NULL )
		{
			InitConverter32	= (lpInitConverter32) GetProcAddress(hLibrary, "InitConverter32");
			result=InitConverter32 (/*m_hWnd*/NULL, /*m_szModule*/NULL);
			bl = true;
		}
		else
			bl = false;
	}
	else
		bl = false;

	if (hLibrary != NULL)
	{
	  InitConverter32	= (lpInitConverter32) GetProcAddress(hLibrary, "InitConverter32");
	  RtfToForeign32	= (lpRtfToForeign32) GetProcAddress(hLibrary, "RtfToForeign32");
	  ForeignToRtf32	= (lpForeignToRtf32) GetProcAddress(hLibrary, "ForeignToRtf32");
	  IsFormatCorrect32	= (lpIsFormatCorrect32) GetProcAddress(hLibrary, "IsFormatCorrect32");
	  CchFetchLpszError	= (lpCchFetchLpszError) GetProcAddress(hLibrary, "CchFetchLpszError");
	  GetWriteNames	    = (lpGetWriteNames) GetProcAddress(hLibrary, "GetWriteNames");
	  GetReadNames	    = (lpGetReadNames)  GetProcAddress(hLibrary, "GetReadNames");

	  hpFileName = GlobalAlloc(GHND|GMEM_DDESHARE, 256);
	  hpClass    = GlobalAlloc(GHND|GMEM_DDESHARE, 256);
	  hpBuf      = GlobalAlloc(GHND|GMEM_DDESHARE, 16*1024);
	  hpDescrip  = GlobalAlloc(GHND|GMEM_DDESHARE, 256);
	  hpExt      = GlobalAlloc(GHND|GMEM_DDESHARE, 256);

	  sprintf( szTemp, "%s\0\0", szDestinationFile );
	  memcpy( GlobalLock( hpFileName ), szTemp, 256 );
	  GlobalUnlock( hpFileName );

	  pClass = ( char* ) GlobalLock( hpClass );

	  if ( nFmt == RTF_EXP_WP51 )
		  memcpy( pClass, "WrdPrfctDos\0", 20 );
	  else if ( nFmt == RTF_EXP_WORD )
	      memcpy( pClass, "MSWord8\0", 10 );
	  else if ( nFmt == RTF_EXP_HTML )
		  memcpy( pClass, "HTML\0", 10);
	  else if ( nFmt == RTF_EXP_LYTX )
		  memcpy( pClass, "Layout\0", 10);  // Not sure this name
	  GlobalUnlock( hpClass );

	  short re;
	  // For converting back to RTF
	  // re = IsFormatCorrect32( hpFileName, hpClass);
	  // re = ForeignToRtf32( hpFileName, NULL, hpBuf, hpClass, NULL, &OutConvert );
	  // if (re < 0)
	  //     result = CchFetchLpszError( re, m_szExt, 100 );
	  // Beep(500,400);

	  // result=InitConverter32 (/*m_hWnd*/NULL, /*m_szModule*/NULL);
	  TRY 
	  {
		re=RtfToForeign32(hpFileName, NULL, hpBuf, hpClass, &InConvert);
		if (re < 0)
		{
		  result = CchFetchLpszError( re, m_szExt, 100 );
		}
		else
		  ss = true;
	  }
	  CATCH_ALL(e)
	  {
		// result = CchFetchLpszError( re, m_szExt, 100 );
	    // e.GetErrorMessage( m_szExt, 100 );
	  }
	  END_CATCH_ALL

	  /*
	  GetReadNames( hpClass, hpDescrip, hpExt );
	  pClass = ( char* ) GlobalLock( hpClass );
	  GlobalUnlock( hpClass );
	  pDescrip = ( char* ) GlobalLock( hpDescrip );
	  GlobalUnlock( hpDescrip );
	  pExt = ( char* ) GlobalLock( hpExt );
	  GlobalUnlock( hpExt );

	  GetWriteNames( hpClass, hpDescrip, hpExt );
	  pClass = ( char* ) GlobalLock( hpClass );
	  GlobalUnlock( hpClass );
	  pDescrip = ( char* ) GlobalLock( hpDescrip );
	  GlobalUnlock( hpDescrip );
	  pExt = ( char* ) GlobalLock( hpExt );
	  GlobalUnlock( hpExt );
	  */
	  
	  fclose( fp );
	  GlobalFree( hpFileName );
	  GlobalFree( hpClass );
	  GlobalFree( hpBuf );
	  GlobalFree( hpDescrip );
	  GlobalFree( hpExt );

	  if ( bl ) { FreeLibrary(hLibrary); hLibrary=NULL; }
	}
	else
		strcpy( m_szExt, "Load Lib Failed!" );

	// Leave Critic
    LeaveCriticalSection(&CriticalSection);

	if ( bc )
	{
		DeleteFile( szSourceFile );
		strcpy( szSourceFile, szDestinationFile );
	}

Endx:
	if ( pzRes!=NULL ) strcpy( pzRes, m_szExt );
	return ss;	
}


BOOL CRTFProc::RtfImport( int nFmt, LPTSTR pzRes ) 
{
	BOOL ss = false, bc = false, bl = false;
	char pzDir[256]={0}, pzFile[256]={0};  
	char m_szExt[124]={0}, szTemp[512];
	
	// Beep(500,400);
	typedef int (PASCAL * lpInitConverter32)(HWND hwndWord, char* m_sModule);
	lpInitConverter32 InitConverter32;
 
	typedef long (PASCAL *PFN_RTF)(long, long);
	typedef short (PASCAL * lpRtfToForeign32)(HANDLE ghszFile, IStorage *pstgForeign,
		     HANDLE ghBuff, HANDLE ghszClass, PFN_RTF lpfnIn);
	lpRtfToForeign32 RtfToForeign32;

	typedef short (PASCAL * lpForeignToRtf32)( HANDLE ghszFile, IStorage* pstgForeign, 
		HANDLE ghBuff, HANDLE ghszClass, HANDLE ghszSubset, PFN_RTF lpfnOut );
	lpForeignToRtf32 ForeignToRtf32;

	typedef short (PASCAL * lpIsFormatCorrect32)(HANDLE haszFile, HANDLE haszClass);
	lpIsFormatCorrect32 IsFormatCorrect32;

	typedef long (PASCAL * lpCchFetchLpszError)( long fce, char *lpszError, long cb );
	lpCchFetchLpszError CchFetchLpszError;

	typedef void (PASCAL * lpGetReadNames)(HANDLE haszClass, HANDLE haszDescrip, HANDLE haszExt );
	lpGetReadNames GetReadNames;

	typedef void (PASCAL * lpGetWriteNames)(HANDLE haszClass, HANDLE haszDescrip, HANDLE haszExt );
	lpGetWriteNames GetWriteNames;

	// HINSTANCE hLibrary;
	LPTSTR pClass, pDescrip, pExt;
	HGLOBAL hpFileName, hpClass, hpDescrip, hpExt;
	// char m_szModule[32];
	long result;

	fp = fopen( szDestinationFile, "w");
	if (fp==NULL) { strcpy( m_szExt, "Open Source File Failed" ); goto Endx; } 

	// Critic Section
	EnterCriticalSection(&CriticalSection); 

	if ( hLibrary==NULL )
	{
		if ( nFmt == RTF_EXP_WP51 )
			hLibrary = LoadLibrary("WPFT532.cnv");
		else if ( nFmt == RTF_EXP_HTML )
			hLibrary = LoadLibrary("HTML32.cnv");
		else if ( nFmt == RTF_EXP_LYTX )
			hLibrary = LoadLibrary("TXTLYT32.cnv");
		else if ( nFmt == RTF_EXP_WORD )
			hLibrary = LoadLibrary("MSWRD832.cnv");
		// hLibrary = LoadLibrary("WRITE32.cnv");
		// hLibrary = LoadLibrary("recovr32.cnv");
		if ( hLibrary!=NULL )
		{
			InitConverter32	= (lpInitConverter32) GetProcAddress(hLibrary, "InitConverter32");
			result=InitConverter32 (/*m_hWnd*/NULL, /*m_szModule*/NULL);
			bl = true;
		}
		else
			bl = false;
	}
	else
		bl = false;

	if (hLibrary != NULL)
	{
	  InitConverter32	= (lpInitConverter32) GetProcAddress(hLibrary, "InitConverter32");
	  RtfToForeign32	= (lpRtfToForeign32) GetProcAddress(hLibrary, "RtfToForeign32");
	  ForeignToRtf32	= (lpForeignToRtf32) GetProcAddress(hLibrary, "ForeignToRtf32");
	  IsFormatCorrect32	= (lpIsFormatCorrect32) GetProcAddress(hLibrary, "IsFormatCorrect32");
	  CchFetchLpszError	= (lpCchFetchLpszError) GetProcAddress(hLibrary, "CchFetchLpszError");
	  GetWriteNames	    = (lpGetWriteNames) GetProcAddress(hLibrary, "GetWriteNames");
	  GetReadNames	    = (lpGetReadNames)  GetProcAddress(hLibrary, "GetReadNames");

	  hpFileName = GlobalAlloc(GHND|GMEM_DDESHARE, 256);
	  hpClass    = GlobalAlloc(GHND|GMEM_DDESHARE, 256);
	  hpBuf      = GlobalAlloc(GHND|GMEM_DDESHARE, 2*1024);
	  hpDescrip  = GlobalAlloc(GHND|GMEM_DDESHARE, 256);
	  hpExt      = GlobalAlloc(GHND|GMEM_DDESHARE, 256);

	  sprintf( szTemp, "%s\0\0", szSourceFile );
	  memcpy( GlobalLock( hpFileName ), szTemp, 256 );
	  GlobalUnlock( hpFileName );

	  pClass = ( char* ) GlobalLock( hpClass );

	  if ( nFmt == RTF_EXP_WP51 )
		  memcpy( pClass, "WrdPrfctDos\0", 20 );
	  else if ( nFmt == RTF_EXP_WORD )
	      memcpy( pClass, "MSWord8\0", 10 );
	  else if ( nFmt == RTF_EXP_HTML )
		  memcpy( pClass, "HTML\0", 10);
	  else if ( nFmt == RTF_EXP_LYTX )
		  memcpy( pClass, "Layout\0", 10);  // Not sure this name
	  GlobalUnlock( hpClass );

	  short re;
	  // For converting back to RTF
	  // re = IsFormatCorrect32( hpFileName, hpClass);
	  // re = ForeignToRtf32( hpFileName, NULL, hpBuf, hpClass, NULL, &OutConvert );
	  // if (re < 0)
	  //     result = CchFetchLpszError( re, m_szExt, 100 );
	  // Beep(500,400);

	  // result=InitConverter32 (/*m_hWnd*/NULL, /*m_szModule*/NULL);
	  TRY 
	  {
	    re = ForeignToRtf32( hpFileName, NULL, hpBuf, hpClass, NULL, &OutConvert );
		//re=RtfToForeign32(hpFileName, NULL, hpBuf, hpClass, &InConvert);
		if (re < 0)
		{
		  result = CchFetchLpszError( re, m_szExt, 100 );
		}
		else
		  ss = true;
	  }
	  CATCH_ALL(e)
	  {
		// result = CchFetchLpszError( re, m_szExt, 100 );
	    // e.GetErrorMessage( m_szExt, 100 );
	  }
	  END_CATCH_ALL

	  /*
	  GetReadNames( hpClass, hpDescrip, hpExt );
	  pClass = ( char* ) GlobalLock( hpClass );
	  GlobalUnlock( hpClass );
	  pDescrip = ( char* ) GlobalLock( hpDescrip );
	  GlobalUnlock( hpDescrip );
	  pExt = ( char* ) GlobalLock( hpExt );
	  GlobalUnlock( hpExt );

	  GetWriteNames( hpClass, hpDescrip, hpExt );
	  pClass = ( char* ) GlobalLock( hpClass );
	  GlobalUnlock( hpClass );
	  pDescrip = ( char* ) GlobalLock( hpDescrip );
	  GlobalUnlock( hpDescrip );
	  pExt = ( char* ) GlobalLock( hpExt );
	  GlobalUnlock( hpExt );
	  */
	  
	  fclose( fp );
	  GlobalFree( hpFileName );
	  GlobalFree( hpClass );
	  GlobalFree( hpBuf );
	  GlobalFree( hpDescrip );
	  GlobalFree( hpExt );

	  if ( bl ) { FreeLibrary(hLibrary); hLibrary=NULL; }
	}
	else
		strcpy( m_szExt, "Load Lib Failed!" );

	// Leave Critic
    LeaveCriticalSection(&CriticalSection);

Endx:
	if ( pzRes!=NULL ) strcpy( pzRes, m_szExt );
	return ss;	
}



BOOL CRTFProc::RebuildRtfFont()
{
	CRTFFont* pRTFFont;
	for (int n = 0; n < HeadFontArray.GetSize(); n++)
	{
		pRTFFont = HeadFontArray[n];
		if (strcmp(fontname,pRTFFont->fontname)==0)
		{
			pRTFFont->Oldfontnumber = fontnumber;
			break;
		}
	}
    fontnumber.Empty();
	fontname.Empty();
	return true;
}


BOOL CRTFProc::AddRtfFontTable()
{
	CRTFFont* pRTFFont = new CRTFFont;
	if (pRTFFont==NULL) return false;
	pRTFFont->fontnumber = fontnumber;
	pRTFFont->fontname = fontname;
	HeadFontArray.Add(pRTFFont);
    fontnumber.Empty();
	fontname.Empty();
	return true;
}


int CRTFProc::ParseRtfKeyword(FILE *fp)
{
    int ch;
    char fParam = false;
    char *pch;
    char szKeyword[30];
    char szParameter[20];

    szKeyword[0] = '\0';
    szParameter[0] = '\0';
    if ((ch = getc(fp)) == EOF)
        return ecEndOfFile;
	
    if (!isalpha(ch))           // a control symbol; no delimiter.
    {
        szKeyword[0] = (char) ch;
        szKeyword[1] = '\0';

		TempString = (char)ch;

        return ecOK;
    }

    for (pch = szKeyword; isalpha(ch); ch = getc(fp))
      *pch++ = (char) ch;
    *pch = '\0';

	TempString = szKeyword;
    if (ch == '-')
    {
		TempString = TempString + (char)ch;

        if ((ch = getc(fp)) == EOF)
		 return ecEndOfFile;
    }
    if (isdigit(ch))
    {
        fParam = true;         // a digit after the control means we have a parameter
        for (pch = szParameter; isdigit(ch); ch = getc(fp))
            *pch++ = (char) ch;

        *pch = '\0';
    }
	TempString = TempString + szParameter;

// For Testing changed
    if (ch != ' ')
		ungetc(ch, fp);
	else
	{
		TempString = TempString + (char)ch;
	}
    
	if (strcmp(szKeyword,"footerl") ==0 )
		return ecFindFooter;
	else if (strcmp(szKeyword,"footerr") == 0)
		return ecFindFooter;
	else if (strcmp(szKeyword,"footerf") == 0)
		return ecFindFooter;
	else if (strcmp(szKeyword, "footer") == 0)
		return ecFindFooter;

	if (strcmp(szKeyword,"headerr") == 0)
		return ecFindHeader;
	else if (strcmp(szKeyword,"headerl") == 0)
		return ecFindHeader;
	else if (strcmp(szKeyword,"headerf") == 0)
		return ecFindHeader;
	else if (strcmp(szKeyword,"header") == 0)
		return ecFindHeader;

	else if (strcmp(szKeyword,"fonttbl")==0)
		return ecFonttbl;
	else if (strcmp(szKeyword,"f")==0)
	{
		fontnumber = szParameter;
		return ecFont;
	}
	else if (strcmp(szKeyword,"viewkind")==0)
		return ecViewkind;
	else if (strcmp(szKeyword,"shp")==0)
		return ecShape;
	else
		return ecOK;
}

CRTFFont::CRTFFont()
{
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
		"FFF8000088130000a00f0000020000000200000000000000ffffff00\r\n", h,w,h,w,w,h );
//		 4005000088130000a00f0000020000000200000000000000
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


