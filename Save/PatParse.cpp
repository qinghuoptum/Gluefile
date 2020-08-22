// PatParse.cpp: implementation of the CPatParse class.
//
//////////////////////////////////////////////////////////////////////

// #include "stdafx.h"	 // Changed for TParse, 

#include "PatParse.h"

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <process.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <sys/types.h>

		

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define		MAXVAL		100

typedef struct tagCCITEM
{
	int nStatus;
	char szName[30];
	char szValue[MAXVAL];
	char szSrch[30];
} CCITEM, *PCCITEM;



#define SRCHSTRLIST(x,y)	i = 0; while (i<16 && y[i].szStrL[0] && strcmp(y[i].szStrL, x)) ++i;

#define		SCANONEITEM(p,q,x,t)	x=0; if ( p[0]=='"' || p[0]=='\'' ) x=(p++)[0];\
		while((p[0])&&(x?(p[0]!=x):(p[0]!=t)))(q++)[0]=(p++)[0]; \
		q[0]=0; while (p[0]==t||(x?p[0]==x:x)||p[0]==' '||p[0]=='\t') { if(p[0]==t)x=0; ++p; }


static char PPadMap[26] = {'2','2','2','3','3','3','4','4','4','5','5','5','6','6','6',
'7','7','7','7','8','8','8','9','9','9','9'};


#if	defined(XML_DEF)

CXMLFile* gpXMLObj = NULL;
int gnXMLCnt = 0;

bool CreateXMLObj( )
{
	if (gpXMLObj==NULL)
	{
		gnXMLCnt = 1;
		gpXMLObj = new CXMLFile();
		gpXMLObj->SetSubTree( "Global" );
	}
	else
		++gnXMLCnt;
	return true;
}

void ReleaseXMLObj( )
{
	--gnXMLCnt;
	if (gnXMLCnt==0 && gpXMLObj!=NULL) 
	{
		delete gpXMLObj;
		gpXMLObj = NULL;
	}
}
	
#endif	// defined(XML_DEF)


char cszDefPubTmplt[] = {
	"Document=<*Document 00 3 0>\r\n"
	"HospCode=<*HospCode 00 3 0>\r\n"
	"JobNo=<*JobNo 00 3 0>\r\n"
	"RptCode=<*RptCode 00 3 0>\r\n"	
	"DictDT=<*DictDT 00 5 :=%m/%d/%Y %H:%M>\r\n"
	"PatLast=<*PatLast 00 3 0>\r\n"	
	"PatFirst=<*PatFirst 00 3 0>\r\n"
	"Dob=<*Dob 00 5 :=%m/%d/%Y %H:%M>\r\n"
	"Sex=<*Sex 00 3 0>\r\n"
	"Marriage=<*Marriage 00 3 0>\r\n"
	"Age=<*Age 00 3 0>\r\n"
	"SSN=<*SSN 00 3 0>\r\n"
	"MrNo=<*MrNo 00 3 0>\r\n"
	"CaseNo=<*CaseNo 00 3 0>\r\n"
	"VisitNo=<*VisitNo 00 3 0>\r\n"
	"MedicalNo=<*MedicalNo 00 3 0>\r\n"
	"RoomBed=<*RoomBed 00 3 0>\r\n"	
	"AdmtDT=<*AdmtDT 00 5 :=%m/%d/%Y>\r\n"
	"SrveDT=<*SrveDT 00 5 :=%m/%d/%Y>\r\n"
	"DschDT=<*DschDT 00 5 :=%m/%d/%Y>\r\n"
	"DeptCode=<*DeptCode 00 3 0>\r\n"
	"ShtCode=<*ShtCode 00 3 0>\r\n"
	"ShtDesc=<*ShtDesc 00 3 0>\r\n"	
	"TranCode=<*TranCode 00 3 0>\r\n"
	"TranDT=<*TranDT 00 5 :=%m/%d/%Y %H:%M>\r\n"
	"TranInit=<*TranInit 00 3 0>\r\n"
	"Appendix1=<*Appendix1 00 3 0>\r\n"
	"Appendix2=<*Appendix2 00 3 0>\r\n"
	"PatAddress1=<*PatAddress1 00 3 0>\r\n"
	"PatAddress2=<*PatAddress2 00 3 0>\r\n"
	"PatPhone=<*PatPhone 00 3 0>\r\n"
	"PDeptCode=<*PDeptCode 00 3 0>\r\n"
	"PShtCode=<*PShtCode 00 3 0>\r\n"
	"DictDrCode=<*DictDrCode 00 3 0>\r\n"
	"DictDrLast=<*DictDrLast 00 3 0>\r\n"
	"DictDrFirst=<*DictDrFirst 00 3 0>\r\n"
	"DictAddress1=<*DictAddress1 00 3 0>\r\n"
	"DictAddress2=<*DictAddress2 00 3 0>\r\n"
	"DictSpeciality=<*DictSpeciality 00 3 0>\r\n"
	"DictInit=<*DictInit 00 3 0>\r\n"
	"DictID=<*DictID 00 3 0>\r\n"
	"DictOffice=<*DictOffice 00 3 0>\r\n"
	"DictFaxNo=<*DictFaxNo 00 3 0>\r\n"
	"DictPhone=<*DictPhone 00 3 0>\r\n"
	"DictLicense_No=<*DictLicense_No 00 3 0>\r\n"	
	"AtndDrCode=<*AtndDrCode 00 3 0>\r\n"
	"AtndDrLast=<*AtndDrLast 00 3 0>\r\n"
	"AtndDrFirst=<*AtndDrFirst 00 3 0>\r\n"
	"AtndAddress1=<*AtndAddress1 00 3 0>\r\n"
	"AtndAddress2=<*AtndAddress2 00 3 0>\r\n"
	"AtndSpecialit=<*AtndSpeciality 00 3 0>\r\n"
	"AtndInit=<*AtndInit 00 3 0>\r\n"
	"AtndID=<*AtndID 00 3 0>\r\n"
	"AtndOffice=<*AtndOffice 00 3 0>\r\n"
	"AtndFaxNo=<*AtndFaxNo 00 3 0>\r\n"
	"AtndPhone=<*AtndPhone 00 3 0>\r\n"
	"AtndLicense_No=<*AtndLicense_No 00 3 0>\r\n"
	"AtndDrLast=<*AtndDrLast 00 3 0>\r\n"
	"AtndDrFirst=<*AtndDrFirst 00 3 0>\r\n"
	"RefrDrCode=<*RefrDrCode 00 3 0>\r\n"
	"RefrDrLast=<*RefrDrLast 00 3 0>\r\n"
	"RefrDrFirst=<*RefrDrFirst 00 3 0>\r\n"
	"RefrAddress1=<*RefrAddress1 00 3 0>\r\n"
	"RefrAddress2=<*RefrAddress2 00 3 0>\r\n"
	"RefrSpeciality=<*RefrSpeciality 00 3 0>\r\n"
	"RefrInit=<*RefrInit 00 3 0>\r\n"
	"RefrID=<*RefrID 00 3 0>\r\n"
	"RefrOffice=<*RefrOffice 00 3 0>\r\n"
	"RefrFaxNo=<*RefrFaxNo 00 3 0>\r\n"
	"RefrPhone=<*RefrPhone 00 3 0>\r\n"
	"RefrLicense_No=<*RefrLicense_No 00 3 0>\r\n"
	"RefrDrLast=<*RefrDrLast 00 3 0>\r\n"
	"RefrDrFirst=<*RefrDrFirst 00 3 0>\r\n"
	"AdmtDrCode=<*AdmtDrCode 00 3 0>\r\n"
	"AdmtDrLast=<*AdmtDrLast 00 3 0>\r\n"
	"AdmtDrFirst=<*AdmtDrFirst 00 3 0>\r\n"
	"AdmtAddress1=<*AdmtAddress1 00 3 0>\r\n"
	"AdmtAddress2=<*AdmtAddress2 00 3 0>\r\n"
	"AdmtSpeciality=<*AdmtSpeciality 00 3 0>\r\n"
	"AdmtInit=<*AdmtInit 00 3 0>\r\n"
	"AdmtID=<*AdmtID 00 3 0>\r\n"
	"AdmtOffice=<*AdmtOffice 00 3 0>\r\n"
	"AdmtFaxNo=<*AdmtFaxNo 00 3 0>\r\n"
	"AdmtPhone=<*AdmtPhone 00 3 0>\r\n"
	"AdmtLicense_No=<*AdmtLicense_No 00 3 0>\r\n"
	"AdmtDrLast=<*AdmtDrLast 00 3 0>\r\n"
	"AdmtDrFirst=<*AdmtDrFirst 00 3 0>\r\n"
	"MD_MrNo=<*MD_MrNo 00 3 0>\r\n"
	"MD_CaseNo=<*MD_CaseNo 00 3 0>\r\n"
	"MD_VisitNo=<*MD_VisitNo 00 3 0>\r\n"
	"MD_WorkType=<*MD_WorkType 00 3 0>\r\n"
	"MD_AccessNo=<*MD_AccessNo 00 3 0>\r\n"
	"CCPosition1=<*CCPostion1>\r\n"
	"CCDrCode1=<*CCDrCode1 00 3 0>\r\n"
	"CCDrCode2=<*CCDrCode2 00 3 0>\r\n"
	"CCDrCode3=<*CCDrCode3 00 3 0>\r\n"
	"CCDrCode4=<*CCDrCode4 00 3 0>\r\n"
	"CCDrCode5=<*CCDrCode5 00 3 0>\r\n"
	"CCDrLast1=<*CCDrLast1 00 3 0>\r\n"
	"CCDrLast2=<*CCDrLast2 00 3 0>\r\n"
	"CCDrLast3=<*CCDrLast3 00 3 0>\r\n"
	"CCDrLast4=<*CCDrLast4 00 3 0>\r\n"
	"CCDrLast5=<*CCDrLast5 00 3 0>\r\n"
	"CCDrLast6=<*CCDrLast6 00 3 0>\r\n"
	"CCDrLast7=<*CCDrLast7 00 3 0>\r\n"
	"CCDrLast8=<*CCDrLast8 00 3 0>\r\n"
	"CCDrFirst1=<*CCDrFirst1 00 3 0>\r\n"
	"CCDrFirst2=<*CCDrFirst2 00 3 0>\r\n"
	"CCDrFirst3=<*CCDrFirst3 00 3 0>\r\n"
	"CCDrFirst4=<*CCDrFirst4 00 3 0>\r\n"
	"CCDrFirst5=<*CCDrFirst5 00 3 0>\r\n"
	"CCDrFirst6=<*CCDrFirst6 00 3 0>\r\n"
	"CCDrFirst7=<*CCDrFirst7 00 3 0>\r\n"
	"CCDrFirst8=<*CCDrFirst8 00 3 0>\r\n"
	"CCDrName1=<*CCDrName1 00 3 0>\r\n"
	"CCDrName2=<*CCDrName2 00 3 0>\r\n"
	"CCDrName3=<*CCDrName3 00 3 0>\r\n"
	"CCTitle1=<*CCTitle1 00 3 0>\r\n"
	"CCTitle2=<*CCTitle2 00 3 0>\r\n"
	"CCTitle3=<*CCTitle3 00 3 0>\r\n"
	"CCTitle4=<*CCTitle4 00 3 0>\r\n"
	"CCTitle5=<*CCTitle5 00 3 0>\r\n"
	"CCTitle6=<*CCTitle6 00 3 0>\r\n"
	"CCTitle7=<*CCTitle7 00 3 0>\r\n"
	"CCTitle8=<*CCTitle8 00 3 0>\r\n"
	"ctrlRefrDrCode={*'<*RefrDrCode>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrCode1={*'<*CCDrCode1>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrCode2={*'<*CCDrCode2>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrCode3={*'<*CCDrCode3>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrCode4={*'<*CCDrCode4>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrCode5={*'<*CCDrCode5>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrCode6={*'<*CCDrCode6>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrCode7={*'<*CCDrCode7>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrCode8={*'<*CCDrCode8>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrName1={*'<*CCDrName1>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrName2={*'<*CCDrName2>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCDrName3={*'<*CCDrName3>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCTitle1={*'<*CCTitle1>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCTitle2={*'<*CCTitle2>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCTitle3={*'<*CCTitle3>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCTitle4={*'<*CCTitle4>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCTitle5={*'<*CCTitle5>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCTitle6={*'<*CCTitle6>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCTitle7={*'<*CCTitle7>'==''?FALSE:TRUE*}\r\n"
	"ctrlCCTitle8={*'<*CCTitle8>'==''?FALSE:TRUE*}\r\n"
	"\r\n\x0"
} ;


//
// Scan contents in ps and put to qs, looking for pairs "..", '..', {..}, (..), [..], <*..>,
// until get token, x in token
LPTSTR xsscanf( LPTSTR ps, LPTSTR qs, LPTSTR token, int nn=250 )
{
	char head, tail;
	int i=0, nLev=0;

	while ( ps[0] )
	{
		if ( strchr(token, ps[0])!=NULL ) break;
		head = 0;

		if ( ps[0]=='"' || ps[0]=='\'' ) { head=ps[0]; tail=ps[0]; }
		else if ( ps[0]=='(' ) { head=ps[0]; tail=')'; }
		else if ( ps[0]=='[' ) { head=ps[0]; tail=']'; }
		else if ( ps[0]=='{' ) { head=ps[0]; tail='}'; }
		else if ( ps[0]=='<' && ps[1]=='*' ) { head=ps[0]; tail='>'; }
		
		if ( head )
		{
			nLev = 1; 
			if ( qs!=NULL && i < nn-1 ) qs[i++]=(ps++)[0]; else ps++;
			while ( ps[0] && (nLev>0) ) 
			{
				if ( ps[0]==head && head!=tail && (head=='<'?ps[1]=='*':true)) nLev++;
				if (ps[0]==tail) nLev--;
				if ( nLev==0 ) break;
				if ( qs!=NULL && i < nn-1 )qs[i++]=(ps++)[0]; else ps++;
			}
			if ( nLev==0 && qs!=NULL && i < nn-1 ) qs[i++]=(ps++)[0]; else ps++;
		}
		else if (qs!=NULL && i < nn-1) qs[i++]=(ps++)[0]; else ps++;
	}

	if ( ps[0] && strchr(token, ps[0])!=NULL ) ++ps;
	if ( qs!=NULL ) qs[i++]=0;
	return ps;
}


// Using a detail config file to mapping the value(s).
// The each line in the file will be the following format:
//  [*_SscanfExp_|]{[#][%1|%2|%3][=|<|>]_OldVal_,}|{*_SprintfExp_[^%i%j%k]|_NewVal_}
//
//  _SscanfExp_: sscanf format express, take up to 3 substrings from pszOldV as
//    parameters, if no _SscanfExp_ presented, take pszOldV as the 1st parameter.
// [#][%1|%2|%3][=|<|>]_OldVal_: check these parameters(s) for given criteria;
//    here, #:numerical, %i:ith parameter, =|<|>:comparison of relation,
//    if none of above exists, just compare _OldVal_ for identical.
//  _SprintfExp_[^%i%j%k]: sprintf format express, output the new string by these 
//    parameter, %i%j%k gives order of the parameters used in sprintf.
//
//  For multiple lines, it will terminate at the line which is satisfied.
//
// 	e.g.  0012|CONSULATION
//		  Default|XYZ 
//        *%*3c%4.4s%*c%4.4s|%1=0012,%2>A000|*BBH%s%s^%2%1
//
//  In this function, pszFile start with '@' indicates a config string instead of
//  file name. 
void MappingValByFile( LPTSTR pszNewV, LPTSTR pszOldV, LPTSTR pszFile )
{
	LPTSTR pos, los, tos, cos, vos;
	TCHAR ch, szBuf[16000], szLine[800], szItem[200], szCrit[100], szParam[3][100];
	int nn, i1, i2, i3;
	bool bNum, bSuc;

	// Default Value
	pszNewV[0]=0;

	// Get buffer to hold configure data
	szBuf[0]=0;
	if ( pszFile[0]=='@' )
	{
		strncpy( szBuf, pszFile+1, sizeof(szBuf)-1 );
		szBuf[sizeof(szBuf)-1]=0;
	}
	else
	{
		// FILE *fp = fopen( pszFile, "r" );
		int fp = sopen(pszFile, O_RDWR|O_TEXT, SH_DENYNO, S_IREAD|S_IWRITE|S_IEXEC);
		if ( fp>0 )
		{
			nn = read( fp, szBuf, sizeof(szBuf)-1 );
			szBuf[nn]=0;
			close( fp );
		}
		else
			szBuf[0]=0;
	}

	// Parse the data line by line
	pos = szBuf;
	while ( pos[0] )
	{
		while ( pos[0] && (pos[0]==' '||pos[0]=='\t'||pos[0]=='\r'||pos[0]=='\n')) ++pos;
		if (pos[0]==0) break;
		szLine[0]=0; ch=0;
		sscanf( pos, "%799[^\r\n]%c", szLine, &ch );
		nn = (strlen(szLine)+(ch?1:0));
		if (nn==0) break;
		pos += nn;

		// Get the current line in szLine;
		los = szLine;

		// Processing the sscanf expression
		szParam[0][0]=0;
		szParam[1][0]=0;
		szParam[2][0]=0;
		if ( los[0]=='*' )	
		{
			++los;
			szItem[0]=0; ch=0;
			sscanf(	los, "%199[^|]%c", szItem, &ch );
			los += (strlen(szItem)+(ch?1:0));
			sscanf( pszOldV, (LPCTSTR)szItem, szParam[0], szParam[1], szParam[2] );
		}
		else
			strncpy(szParam[0], pszOldV, sizeof(szParam[0])-1);

		// Process the criteria
		bSuc = false;
		if ( los[0] )
		{
			// Get criteria partion
			szItem[0]=0; ch=0;
			sscanf(	los, "%199[^|]%c", szItem, &ch );
			los += (strlen(szItem)+(ch?1:0));

			tos = szItem; 
			bSuc = (tos[0]>0);
			while ( tos[0] )
			{
				szCrit[0]=0; ch=0;
				sscanf(	tos, "%99[^,]%c", szCrit, &ch );
				tos += (strlen(szCrit)+(ch?1:0));
				if ( strcmp( szCrit, "Default" )==0 )
				{
					bSuc = true;
					break;
				}

				// Process each criterion
				cos = szCrit; bNum=false;
				if (cos[0]=='#') { bNum=true; ++cos; }

				// Get proper parameters
				if (cos[0]=='%')
				{
					vos = szParam[((cos[1]-'0')<3?(cos[1]-'0'):0)];
					cos += 2;
				}
				else
					vos = szParam[0];

				// Check the relationship
				if (cos[0]=='<')
				{
					++cos;
					if ( bNum ) bSuc &= (atoi(vos)<atoi(cos));
					else bSuc &= ( strcmp(vos,cos)<0 );
				}
				else if (cos[0]=='>')
				{
					++cos;
					if ( bNum ) bSuc &= (atoi(vos)>atoi(cos));
					else bSuc &= ( strcmp(vos,cos)>0 );
				}
				else
				{
					if (cos[0]=='=') ++cos;
					if ( bNum ) bSuc &= (atoi(vos)==atoi(cos));
					else bSuc &= ( strcmp(vos,cos)==0 );
				}
			}  // while ( tos[0] )
		}  // if (los[0])

		// if current criteria satisfied, generate the output string
		if (bSuc)
		{
			szItem[0]=0; ch=0;
			sscanf(	los, "%199[^|]%c", szItem, &ch );
			los += (strlen(szItem)+(ch?1:0));
			tos = szItem;

			if (tos[0]=='*')
			{
				szCrit[0]=0; ch=0; ++tos;
				sscanf( tos, "%99[^\^]%c", szCrit, &ch);
				tos += (strlen(szCrit)+(ch?1:0));
				i1=0; i2=1; i3=2;
				if (ch>0)
				{
					if (tos[0]=='%') { i1=(tos[1]-'0');	tos += 2; }
					if (tos[0]=='%') { i2=(tos[1]-'0');	tos += 2; }
					if (tos[0]=='%') { i3=(tos[1]-'0');	tos += 2; }
				}
				sprintf(pszNewV, (LPCTSTR)szCrit, szParam[i1], szParam[i2], szParam[i3]);
			}
			else
				strcpy( pszNewV, tos );
			break;
		} // if (bSuc)
	}  // while ( pos[0] )
}


void GetUniqueNoByFile( LPTSTR szVTmp, LPTSTR szTmpF )
{
	TCHAR szTemp[20];
	int n,m;

	FILE *fp = fopen( szTmpF, "r+" );
	if (fp==NULL)
	{
		strcpy(szVTmp,"00000001");
		fp = fopen( szTmpF, "w" );
		fprintf( fp, "%s", szVTmp );
		fclose( fp );
	}
	else
	{
		szTemp[0]=0;
		fscanf( fp, "%s", szTemp );
		m = strlen(szTemp);
		n = atol( szTemp);
		strcpy(szVTmp, szTemp);
		fseek(fp, 0L, SEEK_SET);
		sprintf(szTemp, "%0*d", m, n+1);
		fprintf(fp, "%s", szTemp);
		fclose( fp );
	}
}


void GetDelimitedStr( LPTSTR szTemp, LPTSTR szResult, LPTSTR SubType, LPTSTR Length )
{
	bool bBef=false, bAft=false;
	TCHAR ch, ch1, *ps; 
	int kk, n, p;

	ch=SubType[0]; ch=(ch=='s'?' ':ch);
	if ( strchr(szTemp, ch)==NULL )
	{
		ch=SubType[1]; ch=(ch=='s'?' ':ch);
		if (ch && strchr(szTemp, ch)==NULL )
		{
			ch=SubType[2]; ch=(ch=='s'?' ':ch);
			if (ch && strchr(szTemp, ch)==NULL )
			{
				ch=SubType[3]; ch=(ch=='s'?' ':ch);
			}
		}
	}
	if (ch==0) ch=',';

	if (Length[0]=='-') { bBef=true; n=atoi(Length+1); }
	else { n=atoi(Length); bAft=(Length[strlen(Length)-1]=='-'); }
	ps=szTemp;
	
	for (p=0; p<n; p++)
		if (ps[0]=='"'||ps[0]=='\'')
		{
			++ps; ch1=ps[0];
			while (ps[0] && ps[0]!=ch1) ++ps;
			++ps;
			while (ps[0] && ps[0]!=ch) ++ps;
			if (!ps[0]) break;
			++ps;
		}
		else
		{
			while (ps[0] && ps[0]!=ch) ++ps;
			if (!ps[0]) break;
			++ps;
		}

	if ( bBef )
	{
		kk = (ps-szTemp); if ( ps[0] ) --kk;
		strncpy(szResult, szTemp, kk);
		szResult[kk]=0;
	}
	else
	{
		if ( ps[0] && bAft )
		{
			strcpy( szResult, ps );
		}
		else if (ps[0])
		{
			p=0;
			if (ps[0]=='"'||ps[0]=='\'')
			{
				ch1=ps[0]; ++ps;
				while (ps[0] && ps[0]!=ch1 && p<100)
					szResult[p++]=(ps++)[0]; 
				szResult[p]=0;
			}
			else
			{
				while (ps[0] && ps[0]!=ch && p<100)
					szResult[p++]=(ps++)[0]; 
				szResult[p]=0;
			}
		}
	}

}



BOOL FormatProcess( LPTSTR szResult, LPTSTR szTemp, LPTSTR KeyName, 
				    LPTSTR Type, LPTSTR pzSubType, LPTSTR Length, LPTSTR StartPos,
					LPTSTR pszWorkDir = NULL )
{
	char szLast[25], szFirst[25], szMid[25], szTitle[10], szTmpF[50], szFmt[50];
	char szNew[1200], szCode[20], szSubType[25];
	int /*i, nn, */r, s, p, n, seqno;
	long nt;
	char *ps, *ts, *rs, *SubType;
	FILE *fp;
	bool bFst, ss;

	szResult[0]=0;
	ts=pzSubType; rs=szSubType;
	while (ts[0]) { TCHEXPAND(ts,rs) }
	rs[0]=0; SubType=szSubType;

	if ( Type[0]=='I' )
	{
		// Binary Integer
		if ( atoi(SubType)==1 )
			sprintf( szResult, "%d", szTemp[0] );
		else if ( atoi(SubType)==2 )
			sprintf( szResult, "%d", *((short*)szTemp) );
		else if ( atoi(SubType)==4 )
			sprintf( szResult, "%ld", *((int*)szTemp) );
		else if ( atoi(SubType)==8 )
			sprintf( szResult, "%I64d", *((__int64*)szTemp) );
	}
	else if ( Type[0]=='F' )
	{
		// Binary Floating Number
		if ( atoi(SubType)==4 )
			sprintf( szResult, "%f", *((float*)szTemp) );
		else if ( atoi(SubType)==8 )
			sprintf( szResult, "%f", *((double*)szTemp) );
	}
	else if ( Type[0]=='D' )
	{	// Date&Time Process
		ps=szTemp;
		if ( SubType[0]=='O' )  // Ole Date Time
		{
			ParseCOleDateTime( ps, szResult );
		}
		else
		{
			while (ps[0]==' '||ps[0]=='\t'||ps[0]=='\r'||ps[0]=='\n') ++ps;
			if ( SubType[0]=='T' )  // Compact Time HHMM (not HH:MM)
			{
				for(s = 0; ps[0]; ++ps)
				if (ps[0]>='0' && ps[0]<='9') szResult[s++]=ps[0];
				szResult[s]=0;
			}
			else if ( SubType[0]=='S' )  // Use System Time (seconds from 01/01/1970)
				ParseDateTime( ps, szResult, 3 );
			else if ( SubType[0]=='D' )	 // Use dd/mm/yyyy
				ParseDateTime( ps, szResult, 1 );
			else if ( SubType[0]=='Y' )	 // Use yyyy/mm/dd
				ParseDateTime( ps, szResult, 4 );
			else if ( SubType[0]=='A' )	 // Get Age
				ParseDateTime( ps, szResult, 2 );
			else if ( SubType[0]=='C' )	 // Adjust the time
			{
				nt = ParseDateTime( ps, szResult );
				AdjustTime( nt, szResult, SubType+2 );
			}
			else if ( SubType[0]=='M' )	 // Create System Time
			{
				nt = ParseDateTime( ps, szResult );
				sprintf(szResult, "%lu", nt);
			}
			else
				ParseDateTime( ps, szResult );
		}
	}
	else if ( Type[0]=='N' && (SubType[0]=='Z' || SubType[0]=='0'
				|| SubType[0]==0 || SubType[0]=='N' ))
	{	
		// Numerical
		ps=szTemp;
		while (ps[0]==' '||ps[0]=='\t'||ps[0]=='\r'||ps[0]=='\n') ++ps;
		ps += atoi(StartPos);

		if ( SubType[0]=='Z')
		{
			sprintf( szFmt, "%%0%dld", atoi(Length));
			sprintf( szResult, szFmt, atol(ps));
		}
		else
		{
			if (atoi(Length)!=0)
			{
				sprintf( szFmt, "%%%dld", atoi(Length));
				sprintf( szResult, szFmt, atol(ps));
			}
			else
				sprintf( szResult,"%ld", atol(ps));
		}

		// Some subtype may be possible
		// 0 to Error
		if ( SubType[0]=='N' )
		{
			if ( atol(ps)==0 )
			{
				// m_nKeys = 0;
				// break;
				return 0;
			}
		}
	}
	else if ( Type[0]=='S' || Type[0]=='T' || Type[0]=='N' )
	{	
		if ( strcmp("NULL",szTemp)==0 ) { szResult[0]=0; return 1; }

		if ( Type[0]=='T' )
		{	
			// Trimed String
			ps=szTemp;
			while (ps[0]==' '||ps[0]=='\t'||ps[0]=='\r'||ps[0]=='\n') ++ps;
			strcpy( szResult, ps );
			ps=szResult+strlen(szResult)-1;
			while (ps[0]==' '||ps[0]=='\t'||ps[0]=='\r'||ps[0]=='\n') (ps--)[0]=0;
		}
		else if ( Type[0]=='N' )
		{
			ps=szTemp;
			while ( ps[0]==' ' || ps[0]=='\t' || ps[0]=='\r' || ps[0]=='\n' ) ++ps;
			sprintf( szResult, "%ld", atol(ps));
		}
		else if ( SubType[0]=='L' && szTemp[0]>=0 )
		{
			// (L,V) Expression
			r = szTemp[0];
			if (r>atoi(Length)) r=atoi(Length);
			strncpy( szResult, szTemp+1, r );
			szResult[r]=0;
		}
		else
			// Regular String
			strcpy( szResult, szTemp );
		
		if ( SubType[0]=='D' || SubType[1]=='D') // Digits Only
		{
			bFst = 	( SubType[1]=='+' || SubType[2]=='+');
			ps = szResult; s=strlen( szResult );
			
			if (bFst)
			{
				while ( ps[0] && ( ps[0]<'0' || ps[0]>'9')) ++ps;
				if (ps[0]==0) ps = szResult;
				else
				{
					r=0;
					while ( ps[0] && ( ps[0]>='0' && ps[0]<='9'))
						szResult[r++]=(ps++)[0];
					szResult[r++]=0;
				}
			}
			else
				for (r=0; r<s; r++)
					if (szResult[r]>='0' && szResult[r]<='9') 
						(ps++)[0] = szResult[r];
			ps[0]=0;
		}
		else if ( SubType[0]=='M' || SubType[1]=='M')	 // None Digits
		{
			bFst = 	( SubType[1]=='+' || SubType[2]=='+');
			ps = szResult; s=strlen( szResult );

			if (bFst)
			{
				while ( ps[0] && ( ps[0]>='0' && ps[0]<='9')) ++ps;
				if (ps[0]==0) ps = szResult;
				else
				{
					r=0;
					while ( ps[0] && ( ps[0]<'0' || ps[0]>'9'))
						szResult[r++]=(ps++)[0];
					szResult[r++]=0;
				}
			}
			else
				for (r=0; r<s; r++)
					if (szResult[r]<'0' || szResult[r]>'9') break; 
					else (ps++)[0] = szResult[r];
			ps[0]=0;
		}
		else if ( SubType[0]=='T' || SubType[1]=='T')	 // Stuff before Digits
		{
			ps = szResult; s=strlen( szResult );
			for (r=0; r<s; r++)
				if (szResult[r]>='0' && szResult[r]<='9') break; 
			while ( szResult[r] ) (ps++)[0] = szResult[r++];
			ps[0]=0;
		}
		else if ( SubType[0]=='B' || SubType[1]=='B')	 // B<*> Stuff before the mark <*>
		{
			ts= SubType+1; if (SubType[0]!='B') ++ts;
			ps = strstr( szResult, ts );
			if ( ps!=NULL ) ps[0]=0;
		}
		else if ( SubType[0]=='A' || SubType[1]=='A')	 // A<*> Stuff after the mark <*>
		{
			ts= SubType+1; if (SubType[0]!='A') ++ts;
			ps = strstr( szResult, ts );
			if ( ps!=NULL ) strcpy( szResult, ps+strlen(ts));
		}

	
		// Capitalization
		if ( SubType[0]=='C' || SubType[1]=='C')
			strupr( szResult );
		// Proper Format
		else if ( SubType[0]=='P' || SubType[1]=='P')
		{
			strlwr( szResult );	
			ps = szResult; ss = true;
			if (ss)
			{
				if (ps[0]>='a' && ps[0]<='z')
				{
					ps[0]=(ps[0]-32);
					ss = false;
				}
			}
			else
			{
				if (ps[0]<'a') ss = true;
			}
		}
		// Use PhonePad to convert Chars->Digits
		else if ( SubType[0]=='H' || SubType[1]=='H')
		{
			strupr( szResult );	ps = szResult;
			while ( ps[0] )
			{
				if ( isalpha(ps[0])) ps[0] = PPadMap[ps[0]-'A'];
				++ps;
			}
		}


		// Substring (r,s)
		r=0; s=0;
		if (Length[0]=='(')
			sscanf( Length, "(%d,%d)", &r, &s);
		else if ( strchr(Length,',')!=NULL)
			sscanf( Length, "%d,%d", &r, &s );
		else if (strlen(Length)>=4) 
			sscanf( Length, "%2d%2d", &r, &s );
		else
			s = atoi(Length);
		if (r==0) r = atoi(StartPos);

		p = strlen(szResult);
		if (r>0 && s>0 && p>r)
		{
			strncpy(szResult, szResult+r, s); 
			szResult[s]=0;
		}
		else if (r==0 && s>0)
		{
			if ( SubType[0]=='F' || SubType[1]=='F' )
			{
				if (Type[0]=='T' && p<=s) s=p;
				else
					for (r=0; r<s; r++)
						if (r>=p) szResult[r]=' ';
			}
			else if ( SubType[0]=='R' || SubType[1]=='R')
			{	
				if (Type[0]=='T' && p<=s) s=p;
				else if ( s>p )
				{

					for (r=s; r>0; --r)
						szResult[r-1]=((r-1>=s-p)?szResult[r-1-(s-p)]:' ');
				}
				else
				{
					for(r=0; r<s; r++)
						szResult[r]=szResult[r+(p-s)];
				}
			}
			else if ( SubType[0]=='r' || SubType[1]=='r')
			{	
				if (p<=s) s=0;
				else s = (p-s);
			}
			// else
			//	strncpy(szResult, szResult+strlen(szResult)-s-1, s);
			szResult[s]=0;
		}
		else if (r>0 && s==0)
		{
			s=strlen(szResult)-r; 
			if (s<0) s=0;
			else strncpy(szResult, szResult+r, s);
			szResult[s]=0;
		}
	
	
		// Empty/Null to Error
		if ( SubType[0]=='N' || SubType[1]=='N' || SubType[2]=='N')
		{
			ps=szResult;
			while (ps[0]==' '||ps[0]=='\t'||ps[0]=='\r'||ps[0]=='\n') ++ps;
			if ( ps[0]==0 )
			{
				// m_nKeys = 0;
				// break;
				return 0;
			}
		}

	}
	else if ( Type[0]=='M' || Type[0]=='L' )
	{	
		// Name Convention
		ps=szTemp;
		while ( ps[0] )
		{
			if ( ps[0]==' ' && ps[1]==',' ) { ps[0]=','; ps[1]=' '; }
			++ps;
		}

		ps=szTemp;
		if (Type[0]=='L')
		{	
			r=(unsigned)ps[0]; 
			if (r>atoi(Length)) r=atoi(Length);
			++ps; ps[r]=0;
		}
		while (ps[0]==' '||ps[0]=='\t'||ps[0]=='\r'||ps[0]=='\n') ++ps;
		
		ParseName( ps, szLast, szFirst, szMid, szTitle, szCode );
		if (Type[1]=='+')
		{
			strlwr(szLast); if (szLast[0]) szLast[0]-=32;
			strlwr(szFirst); if (szFirst[0]) szFirst[0]-=32;
			strlwr(szMid); if (szMid[0]) szMid[0]-=32;
		}
		switch ( SubType[0] )
		{
		case 'F':
			strcpy( szResult, szFirst );
			break;
		case 'L':
			strcpy( szResult, szLast );
			break;
		case 'M':
			strcpy( szResult, szMid );
			break;
		case 'T':
			strcpy( szResult, szTitle );
			break;
		case 'C':
			strcpy( szResult, szCode );
			break;
		case 'S':
			sprintf( szResult,"%s, %s", szLast, szTitle );
			break;
		case 'P':
			sprintf( szResult,"%s %s", szFirst, szMid );
			break;
		case 'I':
			if ( szMid[0] )
				sprintf( szResult,"%c%c%c", szFirst[0], szMid[0], szLast[0] );
			else
				sprintf( szResult,"%c%c", szFirst[0], szLast[0] );
			break;
		default:;
		}
	}
	else if ( Type[0]=='C' ) 
	// 2 Level Delimited, 
	// SubType[0] is 2nd Delimitor, 
	// atoi(Length) is 2nd Order, -<n>: anything before <n>, <n>-: anything after(include) <n> 
	{
		GetDelimitedStr( szTemp, szResult, SubType, Length );
	}
	else if ( Type[0]=='H' )
	// Mapping the Value to a new value
	{
		// strcpy( szResult, szTemp );
		if ( pszWorkDir!=NULL )
			sprintf( szTmpF, "%s%s.MAP", pszWorkDir, KeyName );
		else
			sprintf( szTmpF, "%s.MAP", KeyName );
		MappingValByFile( szResult, szTemp, szTmpF );
	}
	else if ( Type[0]=='U' )
	// Sequential Number
	{
		szResult[0]=0;
		strcpy( szResult, szTemp );
		if ( Type[1]!='+' || (Type[1]=='+' && szResult[0]==0))
		{
			n=atoi(Length);
			if (n<=0) n=5;
			if ( pszWorkDir!=NULL )
				sprintf( szTmpF, "%s%s.UNO", pszWorkDir, KeyName );
			else
				sprintf( szTmpF, "%s.UNO", KeyName );
			fp = fopen( szTmpF, "r+" );
			if (fp==NULL)
			{
				seqno=1;
				sprintf( szResult, "%0*d", n, seqno );
				fp = fopen( szTmpF, "w" );
				fprintf( fp, "%0*d", n, ++seqno );
				fclose( fp );
			}
			else
			{
				fscanf( fp, "%s", szNew );
				ps = szNew;
				while ( ps[0] && (ps[0]>'9'||ps[0]<'0') ) ++ps;
				seqno = atoi( ps )+1;
				n = strlen( ps ); n=(n>0?n:5);
				sprintf( ps, "%0*d", n, seqno );
				strcpy( szResult, szNew );
				fseek( fp, 0, SEEK_SET );
				fprintf( fp, "%s", szResult );
				fclose( fp );
			}
		}
	}
	else if ( Type[0]=='V' )
	{
		szResult[0]=0;
		strcpy( szResult, szTemp );
		if ( Type[1]!='+' || (Type[1]=='+' && szResult[0]==0))
		{
			if ( SubType[0]=='C' )
			{
				ParseDateTime( NULL, szResult );
			}
			else if ( SubType[0]=='N' )
			{
				strcpy( szResult, "<NULL>" );
			}
			else
			{
				n=atoi(Length);
				if (n>0) 
				{
					//STREXPAND(StartPos, szResult)
					//szResult[n]=0;
					ts=StartPos; rs=szResult;
					while (ts[0]) { TCHEXPAND( ts, rs ) }
					rs[0] = 0;
					//strncpy( szResult, StartPos, n );
					//szResult[n]='\0';
				}
				else 
				{
					ts=StartPos; rs=szResult;
					while (ts[0]) { TCHEXPAND( ts, rs ) }
					rs[0] = 0;
					//STREXPAND(StartPos, szResult)
					//strcpy( szResult, StartPos );
				}
			}
		}
	}
	else if ( Type[0]=='P' )
	{
		if (!SplitFileName( szTemp, SubType, szResult ))
			strcpy( szResult, szTemp );
	}
	else if ( Type[0]=='W' )
	{
		szResult[0]=0;
		if ( SubType[0]=='N' )
		{
			if (atol(szTemp)>0)
				strcpy( szResult, szTemp );
		}
		else if ( SubType[0]=='L' )
		{
			if (szTemp[0]>='A')
				strcpy( szResult, szTemp );
		}
		else
		{
			if (strncmp( szTemp, StartPos, strlen(StartPos))==0)
				strcpy( szResult, szTemp );
		}
	}
	return 1;
}


// Format Process for Export
int FormatForExp( char* pszNew, char* pszOld, char* szFormat, 
				   char* m_szWorkDir, char* szKTmp )
{
	TCHAR szTmp[1200], szVTmp[1200], szTmpF[1200];
	TCHAR szBitv[100], szTVal[100], szFVal[100];
	TCHAR szLast[100], szFirst[100], szMid[100], szTitle[100], szCode[100];
	LPTSTR pt, ps, qs, ts;
	CString x;
	int i, n, m, L;
	long nt;
	bool ss, bFst;

	// Move the value to tmp
 	strncpy( szTmp, pszOld, sizeof(szTmp));
	szVTmp[0] = 0;

 	switch ( szFormat[0] )
 	{
	case 'G': // Prefix item extraction G:<item> will get <value> from <item>=<value>, 
		GetPrefixValue( szTmp, szFormat+2, szVTmp );
		L = strlen(szVTmp);
 		break;
	case 'P':
		SplitFileName( szTmp, szFormat+2, szVTmp );
		L = strlen(szVTmp);
 		break;
 	case 'D':
 		sprintf( szVTmp, szFormat+2, atoi(szTmp));
		L = strlen(szVTmp);
 		break;
 	case 'S':
 		sprintf( szVTmp, szFormat+2, szTmp);
		L = strlen(szVTmp);
 		break;
 	case 'C':
 		sscanf( szTmp, szFormat+2, szVTmp);
		L = strlen(szVTmp);
 		break;
 	case 'T':
		ps=szTmp;
		while (ps[0]==' '||ps[0]=='\t'||ps[0]=='\r'||ps[0]=='\n') ++ps;
		// if ( szFormat[2]=='S' )  // Use System Time (seconds from 01/01/1970)
			// ParseDateTime( ps, szVTmp, 3 );
		if ( szFormat[2]=='A' )	 // Get Age
			ParseDateTime( ps, szVTmp, 2 );
		else if ( szFormat[2]=='M' )	 // Create System Time
		{
			nt = ParseDateTime( ps, szVTmp );
			sprintf(szVTmp, "%lu", nt);
		}
		else if ( szFormat[2]=='C' )	 // Adjust the time
		{
			nt = ParseDateTime( ps, szVTmp );
			AdjustTime( nt, szVTmp, szFormat+3 );
		}
		else
			GetCTimeStr( szVTmp, szFormat+2, ps );
		L = strlen(szVTmp);
 		break;
 	case 'W': // Bit switches control  <bbb>?<TTT>:<FFF>
		strcpy(szTmpF,"&"); strcat(szTmpF, szFormat+2);
 		GetChoiceStr( szVTmp, szTmpF, szTmp );
		L = strlen(szVTmp);
 		break;
 	case 'H': 
 		if ( szFormat[2]=='F' ) 
 		{   
			// Mapping with the file
			if ( szKTmp!=NULL )
			{
 				if ( m_szWorkDir!=NULL && m_szWorkDir[0]!=0 )
 					sprintf( szTmpF, "%s%s.MAP", m_szWorkDir, szKTmp );
 				else
 					sprintf( szTmpF, "%s.MAP", szKTmp );
			}
			else
			{
 				sprintf( szTmpF, "%s.MAP", szFormat+3 );
			}
 			MappingValByFile( szVTmp, szTmp, szTmpF );
 		}
 		else  // Key Choices <K1>-<V1>^<K2>-<V2>^...
 			GetChoiceStr( szVTmp, szFormat+2, szTmp );
		L = strlen(szVTmp);
 		break;
 	case 'M': // Name Convention
 		pt=szTmp;
 		while (pt[0]==' '||pt[0]=='\t'||pt[0]=='\r'||pt[0]=='\n') ++pt;
 		ParseName( pt, szLast, szFirst, szMid, szTitle, szCode );
 		switch ( szFormat[2] )
 		{
 		case 'F':
 			strncpy( szVTmp, szFirst, sizeof(szVTmp) );
 			break;
 		case 'L':
 			strncpy( szVTmp, szLast, sizeof(szVTmp) );
 			break;
 		case 'M':
 			strncpy( szVTmp, szMid, sizeof(szVTmp) );
 			break;
 		case 'T':
 			strncpy( szVTmp, szTitle, sizeof(szVTmp) );
 			break;
 		case 'C':
 			strncpy( szVTmp, szCode, sizeof(szVTmp) );
 			break;
 		case 'S':
 			sprintf( szVTmp,"%s, %s", szLast, szTitle );
 			break;
 		case 'P':
 			sprintf( szVTmp,"%s %s", szFirst, szMid );
 			break;
		case 'I':
			if ( szMid[0] )
				sprintf( szVTmp,"%c%c%c", szFirst[0], szMid[0], szLast[0] );
			else
				sprintf( szVTmp,"%c%c", szFirst[0], szLast[0] );
			break;
 		default:;
 		}
		L = strlen(szVTmp);
 		break;
 	case 'L': // Extened format 
 		switch ( szFormat[2] )
 		{
		case 'N':  // Normal Char
			for (i=0; i<(int)strlen(szTmp); i++)
				if (szTmp[i]=='\'') szVTmp[i]='`'; else szVTmp[i]=szTmp[i];
			szVTmp[i] = 0;
			break;
 		case 'C':  // Capitalized
 			strncpy( szVTmp, szTmp, sizeof(szVTmp) );
 			strupr( szVTmp );
 			break;
 		case 'L': // Lowercase
 			strncpy( szVTmp, szTmp, sizeof(szVTmp) );
 			strlwr( szVTmp );
 			break;
 		case 'P': // Proper
			ss = true;
 			strlwr( szTmp );
			for (i=0; i<(int)strlen(szTmp); i++)
			{
				if (ss)
				{
					if (szTmp[i]>='a' && szTmp[i]<='z')
					{
						szVTmp[i]=(szTmp[i]-32);
						ss = false;
					}
					else
						szVTmp[i]=szTmp[i];
				}
				else
				{
					szVTmp[i]=szTmp[i];
					if (szTmp[i]<'a') 
						ss = true;
				}
			}
			szVTmp[i]=0;
 			break;
 		case 'S': // Space dilimit each chars
 			pt = szTmp; i=0;
 			while ( pt[0] )
 			{
 				szVTmp[i++]=pt[0]; szVTmp[i++]=' '; ++pt;
 			}
 			szVTmp[i]=0;
 			break;
		case 'd':
 		case 'D': // Deleimited w/ <Begin>-<End>
 			GetDelimitedStr( szVTmp, szFormat, szTmp );
 			break;
 		case 'R': // R<Old>-<New> replace <Old> with <New>
			szTVal[0] = 0; szFVal[0] = 0;
 			sscanf(szFormat+3,"%[^-]%*c%[^\n]", szTVal, szFVal);
			
			qs = szTVal, ts = szTVal;
			while ( qs[0] )
			{
				TCHEXPAND( qs, ts )
			}
			ts[0]= 0;
			qs = szFVal, ts = szFVal;
			while ( qs[0] )
			{
				TCHEXPAND( qs, ts )
			}
			ts[0]= 0;

			x = szTmp;
			x.Replace( szTVal, szFVal );
			strcpy( szVTmp, x );
 			break;
		case 'M': // MidString w/ <n>-<m>
			n = 0; m = 0;
			sscanf(szFormat+3,"%d-%d", &n, &m);
			if ( m>=n && (unsigned)n<=strlen(szTmp) )
			{
				strncpy(szVTmp, szTmp+n, (m-n+1));
				szVTmp[m-n+1]=0;
			}
			else if ( m==0 && (unsigned)n<=strlen(szTmp) )
			{
				strncpy(szVTmp, szTmp+n, sizeof(szVTmp)-1);
				szVTmp[sizeof(szVTmp)-1]=0;
			}
			break;
		case 'l': // Left Substring (l-: Left Cut)
			n = 0;
			if (szFormat[3]=='-')
			{
				sscanf(szFormat+4,"%d", &n);
				strncpy(szVTmp, szTmp+n, sizeof(szVTmp)-1);
			}
			else
			{
				sscanf(szFormat+3,"%d", &n);
				strncpy(szVTmp, szTmp, sizeof(szVTmp)-1);
				szVTmp[n]=0;
			}
			break;
		case 'r': // Right Substring (r-: Right Cut)
			n = 0; m = strlen(szTmp);
			if (szFormat[3]=='-')
			{
				sscanf(szFormat+4,"%d", &n);
				if (n<m)
				{
					strncpy(szVTmp, szTmp, sizeof(szVTmp)-1);
					szVTmp[m-n]=0;
				}
			}
			else
			{
				sscanf(szFormat+3,"%d", &n);
				if (n<m)
					strncpy(szVTmp, szTmp+(m-n), sizeof(szVTmp)-1);
				else
					strncpy(szVTmp, szTmp, sizeof(szVTmp)-1);
			}
			break;
		case 'F': // Left Adjustment Space Fill
			n = 0;
			if ( szFormat[3] ) sscanf( szFormat+3,"%d", &n );
			if ( n==0 || n>sizeof(szVTmp)-1 ) n=strlen(szTmp);
			memset( szVTmp,	' ', n ); szVTmp[n]=0;
			if ( (int)strlen(szTmp)<n ) n=strlen(szTmp);
			memcpy( szVTmp, szTmp, n );
			break;
		case 'A': // All Trim
			n = strlen( szTmp );
			memset( szVTmp, ' ', n );
			szVTmp[n] = 0;
			m = 0; while (szTmp[m]==' ') ++m; i=0;
			for ( ;m<n;m++) szVTmp[i++]=szTmp[m];
			szVTmp[i] = 0;
			while (i>0 && szVTmp[i-1]==' ') szVTmp[--i]=0;
			break;
		case 'H': // Use PhonePad to convert Chars->Digits
			strupr( szTmp ); n = strlen( szTmp );
			for ( i=0; i<n; i++ )
				szVTmp[i]= ( isalpha(szTmp[i])?PPadMap[szTmp[i]-'A']:szTmp[i] );
			szVTmp[n]=0;
			break;
		case 't':  // <c>[-]<n>[-]: 2nd delimited string
			szBitv[0] = szFormat[3]; szBitv[1]=0;
			GetDelimitedStr( szTmp, szVTmp, szBitv, szFormat+4 );
			break;
		case 's':
			n = 0;
			while ( szTmp[n] )
			{
				unsigned int mx = ((unsigned int)szTmp[n] & 0x000000FF);
				if ( mx < 0x80 ) szVTmp[n] = szTmp[n];
				else if ( mx == 0x92 ) szVTmp[n] = '\'';
				else szVTmp[n] = ( szTmp[n] & 0x7F );
				++n ;
			}
			szVTmp[n] = 0;
			break;
		case 'h':
			qs=szVTmp; ps=szTmp; n=0;
			while ( ps[0] && n<sizeof(szVTmp)-1 )
			{
				if ( ps[0]=='|' ) { (qs++)[0]='\\'; (qs++)[0]='F'; (qs++)[0]='\\'; n+=3; }
				else if ( ps[0]=='^' ) { (qs++)[0]='\\'; (qs++)[0]='S'; (qs++)[0]='\\'; n+=3; }
				else if ( ps[0]=='~' ) { (qs++)[0]='\\'; (qs++)[0]='R'; (qs++)[0]='\\'; n+=3; }
				else if ( ps[0]=='&' ) { (qs++)[0]='\\'; (qs++)[0]='T'; (qs++)[0]='\\'; n+=3; }
				else if ( ps[0]=='\\' ) { (qs++)[0]='\\'; (qs++)[0]='E'; (qs++)[0]='\\'; n+=3; }
				else if ( (ps[0]>=' ' || ps[0]=='\t') )	{ (qs++)[0]=(ps)[0]; ++n; }
				ps++;
			}
			qs[0]=0; 
			break;

		case '*':
			if ( szFormat[3]=='D' ) // Digits Only
			{
				bFst = 	( szFormat[4]=='+' );
				ps = szTmp; n=strlen( szTmp );
				if (bFst)
				{
					while ( ps[0] && ( ps[0]<'0' || ps[0]>'9')) ++ps;
					if (ps[0]==0) szVTmp[0]=0;
					else
					{
						n=0;
						while ( ps[0] && ( ps[0]>='0' && ps[0]<='9'))
							szVTmp[n++]=(ps++)[0];
						szVTmp[n++]=0;
					}
				}
				else
				{
					n=0;
					while ( ps[0] )
					{
						if ( ps[0]>='0' && ps[0]<='9') szVTmp[n++]=(ps)[0];
						++ps;
					}
					szVTmp[n++]=0;
				}
			}
			else if ( szFormat[3]=='M' ) // None Digits
			{
				bFst = 	( szFormat[4]=='+' );
				ps = szTmp; n=strlen( szTmp );
				if (bFst)
				{
					while ( ps[0] && ( ps[0]>='0' && ps[0]<='9')) ++ps;
					if (ps[0]==0) szVTmp[0]=0;
					else
					{
						n=0;
						while ( ps[0] && ( ps[0]<'0' || ps[0]>'9'))
							szVTmp[n++]=(ps++)[0];
						szVTmp[n++]=0;
					}
				}
				else
				{
					n=0;
					while ( ps[0] )
					{
						if ( ps[0]<'0' || ps[0]>'9') szVTmp[n++]=(ps)[0];
						++ps;
					}
					szVTmp[n++]=0;
				}
			}
			else if ( szFormat[3]=='B' )	 // B<*> Stuff before the mark <*>
			{
				strcpy( szVTmp, szTmp );
				ps = strstr( szVTmp, szFormat+4 );
				if ( ps!=NULL ) ps[0]=0; 
			}
			else if ( szFormat[3]=='A' )	 // A<*> Stuff after the mark <*>
			{
				ps = strstr( szTmp, szFormat+4 );
				if ( ps!=NULL ) strcpy( szVTmp, ps+strlen(szFormat+4));
				else szVTmp[0]=0;
			}
			break;
 		default:
			strcpy(szVTmp, szTmp);
 		}
		L = strlen(szVTmp);
 		break;
 	case 'V': // (L,V) format
		n = atoi(szFormat+3);
		if ( szFormat[2] == 'B' )
		{
			L = strlen(szTmp); n += L; 
			*(UCHAR*)szVTmp = n;
			memcpy( szVTmp+1, szTmp, L);
			++L;
		}
		else if ( szFormat[2] == 'W' )
		{
			L = strlen(szTmp); n += L; 
			*(WORD*)szVTmp = n;
			memcpy( szVTmp+2, szTmp, L);
			L += 2;
		}	 
		else if ( szFormat[2] == 'D' )
		{
			L = strlen(szTmp); n += L; 
			*(DWORD*)szVTmp = n;
			memcpy( szVTmp+4, szTmp, L);
			L += 4;
		}
		else if ( szFormat[2] == 'L' )
		{
			L = strlen(szTmp);
			sprintf(szVTmp,"%d", L);
			L = strlen(szVTmp);
		}
		break;
	case 'I': // Output an integer number
		if ( szFormat[2]=='1' )
		{
			*(UCHAR*)szVTmp = (UCHAR)atoi(szTmp);
			L=1;
		}
		else if ( szFormat[2]=='2' )
		{
			*(unsigned short*)szVTmp = (unsigned char)atoi(szTmp);
			L=2;
		}
		else if ( szFormat[2]=='4' )
		{
			*(unsigned long*)szVTmp = (unsigned long)atol(szTmp);
			L=4;
		}
		else if ( szFormat[2]=='8' )
		{
			*((__int64*)szVTmp) = _atoi64(szTmp);
			L=8;
		}
		break;
	case 'F':  // Output a floating number
		if ( szFormat[2]=='4' )
		{
			*(float*)szVTmp = (float)atof(szTmp);
			L=4;
		}
		else if ( szFormat[2]=='8' )
		{
			*((double*)szVTmp) = atof(szTmp);
			L=8;
		}
		break;
#if	defined(CRYPT_DEF)
/*
	" S2:Standard 2 Way Function, C(18)->C(28)",
	" S1:Standard 1 Way Function, C(18)->C(26)",
	" DS:Standard Decryption",
	" DT:Standard Decryption w/ Trim appendix",
	" HN:Short 1 Way, C(10)->N(8)",
*/
	case 'E':  // Encryption/Decryption w/ Discrete Log/Permutation
		if ( szFormat[2]=='S' && szFormat[3]=='2' )
		{
			CCryptCode::Encode2Way( szTmp, szVTmp, 0 );
			L = 26;
		}
		else if ( szFormat[2]=='S' && szFormat[3]=='1' )
		{
			CCryptCode::Encode2Way( szTmp, szVTmp, 1 );
			L = 26;
		}
		else if ( szFormat[2]=='D' && szFormat[3]=='S' )
		{
			CCryptCode::Decode2Way( szVTmp, szTmp, 1 );
			L = 18;
		}
		else if ( szFormat[2]=='D' && szFormat[3]=='T' )
		{
			CCryptCode::Decode2Way( szVTmp, szTmp, 0 );
			L = strlen(szVTmp);
		}
		else if ( szFormat[2]=='H' && szFormat[3]=='N' )
		{
			CCryptCode::Encode1Way8Digits( szTmp, szVTmp );
			L = 8;
		}
		else
		{
			strcpy(szVTmp, szTmp);
			L = strlen(szVTmp);
		}
		break;
#endif
 	default:
		strcpy(szVTmp, szTmp);
		L = strlen(szVTmp);
 	}

	memcpy(pszNew, szVTmp, L );
	pszNew[L]=0;
	return L;
}


// Search CC or Others in the text file
// Criteria for CCs: 
// 1) 'CC: ' is the token to mark the CC line;
// 2) ';' can be used to seperated multiple names in one line;
// 3) '//' marks the end of names with others in this line.
void SearchTextForCC( LPTSTR pszBody, PCCITEM pCCItem, int nn, LPTSTR pzDlmt=NULL )
{
	TCHAR dmlt[32], scanstr[64], line[500], token[30], *pos, *ts, *rs, *lps, ch;
	int i, j, k, tt, ss;

	if ( pzDlmt==NULL )
	{
		strcpy( dmlt, ";\r\n" );
		strcpy( scanstr, "%499[^;\r\n]%c" );
	}
	else
	{
		ts = pzDlmt; rs = dmlt;
		while ( ts[0] ) { TCHEXPAND( ts, rs ) };
		rs[0]=0;
		sprintf( scanstr, "%%499[^%s]%%c", dmlt );
	}

	pos = pszBody;
	while ( pos[0] )
	{
		while ( pos[0] && (pos[0]=='\t' ||pos[0]==' '||pos[0]=='\r'||pos[0]=='\n')) ++pos;
		if ( pos[0]==0 ) break;
		while (strchr( dmlt, pos[0] )!=NULL) ++pos;
		if ( pos[0]==0 ) break;
		line[0]=0; 	ch=0;
		sscanf( pos, scanstr, line, &ch ); line[499]=0;
		if ( strlen(line)==499 ) pos += (499-64);
		else
		{
			pos += (strlen(line)+(ch?1:0));

			/* Look up line Carbon Copy or others */
			ss = 0;
			for (i=0; i<nn; i++)
			{
				if ( pCCItem[i].nStatus == 0 )
				{
					strncpy( token, pCCItem[i].szSrch, sizeof(token)-1 );
					token[sizeof(token)-1]=0;
	
					if ( (ts = strstr( line, token ))!=NULL )
					{	
						/* Found One line, check its value */
						lps = ts + strlen(token);

						while ( lps[0] && (lps[0]=='\t' ||lps[0]==' '||lps[0]=='\f'||lps[0]=='\r'||lps[0]=='\n')) ++lps;
						if ( ss && i<nn-1 ) ++i; // Move pointer
						tt = 1;
						for(j=0,k=0; lps[k]; k++)
						{
							if (tt==1 && lps[k]=='<' && lps[k+1]=='*') tt=0;
							if (tt==0 && lps[k]=='>') tt=1;
							if (tt) lps[j++]=(lps[k]<32?' ':lps[k]);
						//while ( lps!=NULL && lps[0] && 
						//	(lps[0]=='\t' ||lps[0]==' '||lps[0]=='\f')) ++lps;
						}
						lps[j++]=0;
						// while ( lps[0] && (lps[0]=='\t' ||lps[0]==' '||lps[0]=='\f')) ++lps;
				
						if ( lps!=NULL && lps[0] )
						{	/* Get its value */
							strncpy( pCCItem[i].szValue, lps, MAXVAL-1 );
							pCCItem[i].szValue[MAXVAL-1]=0;
							pCCItem[i].nStatus = 1;
							ss = 1;
						}
						// if (eds==NULL ||lps[0]==0) break;
						// else  lps=eds+1;
					}
					if ( ss ) break;
				}  // if ( pCCItem[i].nStatus == 0 )
			}	// for (i=0; i<nn; i++)
		}  // if (strlen(line)==499)
	}	// while ( pos[0] )
}


//
// Search a given buffer to detect a pattern given by the Scanf Format
// return the LPTSTR to its begin (or ending in nPos=1)
// Scanf Formt   ---  %[-|+|=][<nn>][[^]<Expr>]
// Extend Format ---  #[<Str>...]|{<Str>...}|(<Str>...)
//
LPTSTR SrchByScanFmt(LPTSTR pzBuf, LPTSTR pzFmt, int nPos)
{
	LPTSTR ps, qs, ts, rs, svps;
	TCHAR szTmp[128], szItem[8000], ch1, ch;
	int /*nn, i, p,*/ m, k, nLev;
	bool ss;

	ps = pzBuf; qs = pzFmt; szTmp[0] = 0;
	sscanf( qs, "%127[^%#]", szTmp ); szTmp[127]=0; k = strlen(szTmp);
	
	while ( ps[0] )
	{
		if ( strncmp(ps,qs,k)==0 )
		{
			svps = ps; ss = true;
			while ( ss && qs[0] )
			{
				if ( ps[0]==qs[0] && qs[0]!='%' && qs[0]!='#' )
				{
					++ps; ++qs;
				}
				else if ((ps[0]=='%' || qs[0]=='#') && qs[1]==qs[0] && ps[0]==qs[0] )
				{
					++ps; ++qs; ++qs;
				}
				else if ( qs[0]=='%' && qs[1]!='%' )
				{
					// Scanf Format Analysis
					szTmp[0]=0; 
					sscanf(qs,"%[^\]]%c", szTmp, &ch);
					if ( ch!=']' ) { ss = false; break; }
					strcat( szTmp, "]" );
					qs += strlen(szTmp);

					// Arrange th Format String
					// STREXPAND(szTmp, szTmp)
					ts=szTmp; rs=szTmp;
					while (ts[0]) { TCHEXPAND( ts, rs ) }
					rs[0] = 0;
					ts = szTmp+1;
					
					if ( ts[0]=='-' || ts[0]=='+' || ts[0]=='=' ) 
					{ 
						ch=ts[0]; ++ts;
						m = atoi( ts );
						while (isdigit(ts[0])) ++ts;
					}
					else 
						ch=0;

					if ( ts!=szTmp+1) strcpy( szTmp+1, ts );
					szItem[0] = 0;
					sscanf( ps, szTmp, szItem ); 
					ps += strlen( szItem );

					if ( ch=='-' )
					{
						ss = (strlen(szItem)<=(unsigned)m);
					}
					else if ( ch=='+' )
					{
						ss = (strlen(szItem)>=(unsigned)m);
					}
					else if ( ch=='=' )
					{
						ss = (strlen(szItem)==(unsigned)m);
					}
					else
						ss = true;
				}
				else if ( qs[0]=='#' && qs[1]!='#' )
				{
					// Locate [<Str>...], (<Str>...) or {<Str>...}
					szTmp[0]=0; ch1=qs[1];
					if (ch1=='[')
					{
						sscanf(qs+2,"%[^\]]%c", szTmp, &ch);
						if ( ch!=']' ) { ss = false; break; }
					}
					else if (ch1=='{')
					{
						sscanf(qs+2,"%[^}]%c", szTmp, &ch);
						if ( ch!='}' ) { ss = false; break; }
					}
					else if (ch1=='(')
					{
						sscanf(qs+2,"%[^)]%c", szTmp, &ch);
						if ( ch!=')' ) { ss = false; break; }
					}
					else { ss = false; break; }

					qs += (2+strlen(szTmp)+1);

					// Arrange th Format String
					ts=szTmp; rs=szTmp;
					while (ts[0]) { TCHEXPAND( ts, rs ) }
					rs[0];
					// STREXPAND(szTmp, szTmp)
					ts=strstr(szTmp,"...");
					if (ts!=NULL) ts[0]=0;
					if (ps[0]==ch1 && strncmp(ps+1, szTmp, strlen(szTmp))==0)
					{
						ps += (strlen(szTmp)+1);
						nLev=1;
						do 
						{
							ps++;
							if (ps[0]==ch1) nLev++;
							if (ps[0]==ch) nLev--;
						}
						while ( !(ps[0]==ch && nLev==0) && ps[0] );
						ss = (ps[0]==ch); ++ps;
					}
					else
						ss = false;
				}
				else
					ss =false;
			}
			if (ss) break;
			else 
			{
				ps = svps+1; qs = pzFmt; 
			}			 
		}
		else
		{
			ss = false;
			++ps;
		}
	}
	if ( !ss ) return NULL;
	if ( nPos ) return ps; else return svps;
}


//
//	<Pos> can be <nn> ( meanings <nn>th char position ) or
//		<nn>,<mm> or <nn>-<mm>, means <nn>th row and <mm>th column
//
//  if bLV is true, pzBuf will hold a multiple LV Segment,
//      and <nn> will indicates the Subsegment number.
//
bool GetFixedPosFlds(LPTSTR pzRes, LPTSTR pzBuf, LPTSTR pzPos, int nLen, bool bLV=false)
{
	TCHAR szX[20], szY[20], *ps;
	int k, nPos, nRow, nCol;

	pzRes[0] = 0;
	szX[0] = 0; szY[0] = 0;
	sscanf(pzPos,"%19[^,-.]%*c%19[^\x0]", szX, szY); szX[19]=0; szY[19]=0;
	if (szY[0]==0) 
	{
		// Single Lines
		nPos = atoi(szX);
		if (bLV)
		{
			k = *(unsigned int*)pzBuf;
			if ( nPos< k)
			{
				strncpy(pzRes, pzBuf+nPos+4, nLen);
				pzRes[nLen] = 0;
				pzRes[k-nPos] = 0;
				return true;
			}
			else
				return false;
		}
		else
		{
			//if ((unsigned)nPos<strlen(pzBuf))
			//{
				strncpy(pzRes, pzBuf+nPos, nLen);
				pzRes[nLen] = 0;
				return true;
			//}
			//else
			//	return false;
		}
	}
	else
	{
		// Multiple Lines or Multiple segemnt
		nRow = atoi(szX); nCol = atoi(szY);
		ps=pzBuf; k = nRow;

		if (bLV)
		{
			// Multiple LV SubSegments Case
			while ( *(unsigned int*)ps >0 && k>0 )
			{
				ps += (	*(unsigned int*)ps + 4 ); --k;
			}

			k = *(unsigned int*)ps;
			if ( nCol<k)
			{
				strncpy(pzRes, ps+nCol+4, nLen);
				pzRes[nLen] = 0;
				pzRes[k-nCol] = 0;
				return true;
			}
			else
				return false;
		}
		else
		{
			// Multiple Line Case
			while ( ps[0] && k>0 )
			{
				while (ps[0] && ps[0]!='\n') ++ps;
				if ( ps[0] )
				{
					++ps; --k;
				}
			}

			if ( (unsigned)nCol<strlen(ps))
			{
				strncpy(pzRes, ps+nCol, nLen);
				pzRes[nLen] = 0;
				return true;
			}
			else
				return false;
		}
	}
	return true;
}


//
// Evaluation of an Extended Expression, return false if it failed
// Extended Expression is a standard expression <Expr> or
//		<Expr>?<Tmplt_1>:(~)<Tmplt_2> ( if <Expr> then <Tmplt_1> else <Tmplt_2>
// 
bool ExtendEvaluate( LPTSTR pzOrgExpr, LPTSTR pzResult )
{
	CStdExpress expr;
	LPTSTR ps, rs, ts[60];
	TCHAR ch, ch1, szTmp[256]={0};
	int i, k;

	pzResult[0]=0;
	ps = strchr( pzOrgExpr, '?' );
	if (ps==NULL)
		return expr.Evaluate( pzOrgExpr, pzResult );

	// <Expr>?<Tmplt_1>:<Tmplt_2> format
	ps[0]=0; ++ps; rs = ps; i = 0; ch1 = 0;
	while ( ps[0] )
	{
		if ( ch1? ps[0]==ch1:(ps[0]==':' || ps[0]=='~') )
		{
			ch1=ps[0]; ts[++i]=rs; ps[0]=0; rs=++ps;
		}
		else if ( ps[0]=='\'' || ps[0]=='"' )
		{
			ch = ps[0]; ++ps; 
			while ( ps[0] && (ps[0]!=ch) ) ++ps;
			if ( ps[0]==ch ) ++ps;
		}
		else
			++ps;
	}
	ts[++i]=rs;

	if ( expr.Evaluate( pzOrgExpr, szTmp ) )
	{
		k = atoi(szTmp); strupr(szTmp);
		if (stricmp(szTmp,"TRUE")==0) strcpy( pzResult, ts[1] );
		else if (stricmp(szTmp,"FALSE")==0) strcpy( pzResult, ts[2] );
		else if ( k>0 && k<=i) strcpy( pzResult, ts[k] );
		else return false;
		return true;
	}
	else
		return false;
}


// Looking for a segment end by <ExtStr>[nn]*
// ( token <ExtStr>[nn]* is used to identify the end of Record
//   [nn] means <ExtStr> appear nn times as ending token
//   * means <ExtStr> is the initial token of a segment )
// or _LV_<SpeStr>  
// ( Use (L,V) for each recorder, <SpeStr> will be 
//   {B|W|D}... to specify how L for eacg subsegment to be given,  
int GetDataSegByExtStr( FILE *fp, LPTSTR pzToken, LPTSTR pzBuf, int nLmt )
{
	int nPos = 0, i, k, nn, nTmp;
	TCHAR ch, *ps, *ts, token[100], tmps[100];
	bool ss;

	if (feof(fp)) return IMP_REC_END;
	if ( pzBuf ) pzBuf[0] = 0;

	if (strncmp( pzToken, "_LV_", 4)==0)
	{
		ps = pzToken+4; ts = pzBuf;
		ss = true;  k=0;
		while ( ss && (ps[0]=='B' || ps[0]=='W' || ps[0]=='D') )
		{
			nTmp = 0;
			if (ps[0]=='B')
			{
				nn=fread((UCHAR*)&nTmp, 1, 1, fp);
				ss = (nn==1);
			}
			else if (ps[0]=='W')
			{
				nn=fread((unsigned short*)&nTmp, 1, 2, fp);
				ss = (nn==2);
			}
			else if (ps[0]=='D')
			{
				nn=fread((unsigned long*)&nTmp, 1, 4, fp);
				ss = (nn==4);
			}
			
			*(unsigned int*)ts= nTmp; ts+=4; k+=4;
			if (nTmp==0 || k+nTmp>nLmt) { ss=false; break; }

			if (ss && nTmp>0)
			{
				if ( ts!=NULL )
				{
					nn = fread( ts, 1, nTmp, fp);
					ts += nn;
					*(unsigned int*)ts = 0;
				}
				else  // Skip Only
				{
					fseek( fp, nTmp, SEEK_CUR );
					nn = nTmp;
				}
				ss = (nn==nTmp);
			}

			++ps;
		}
		if (ss) return IMP_REC_NORMAL; else return IMP_REC_END;
	}

	ps = pzToken; ts = token;
	while (ps[0]) { TCHEXPAND( ps, ts ) }
	ts[0] = 0;

	k=1;
	ss = (token[strlen(token)-1]=='*');
	if (ss) token[strlen(token)-1]=0;
	ps = strchr(token, '[');
	if (ps!=NULL && isdigit(ps[1]))
	{
		ps[0]=0; k=atoi(ps+1);
	}

	nn = strlen(token);
	for(i=0; i<=nn; i++) tmps[i]=0;

	while ( !feof(fp) && (nPos<nLmt-10) && ((nTmp=getc(fp)) != EOF) )
	{
		ch = nTmp;
		if ( pzBuf ) pzBuf[nPos++] = ch;
		for (i=0; i<nn-1; i++) tmps[i] = tmps[i+1];
		tmps[nn-1] = ch;
		if ( ss && nPos<=nn+2)
		{
		}
		else if (strncmp(tmps, token, nn)==0) 
		{
			if (--k==0) break;
		}
	}


	if (feof(fp))
	{
		if (ss)
		{
			if ( pzBuf ) pzBuf[nPos]=0;
			return IMP_REC_NORMAL;
		}
		else if ( nPos>0 )
		{
			if ( pzBuf ) pzBuf[nPos]=0;
			return IMP_REC_NORMAL;
		}
		else
			return IMP_REC_END;
	}
	else
	{
		if ( pzBuf ) pzBuf[(nPos>0?(nPos++)-nn:0)]=0;
		if (ss)
		{
			for(i=nn-1; i>=0; i--)
				ungetc( token[i], fp );
		}
		return IMP_REC_NORMAL;
	}
}


// Criteria -> [[[+|-][C|D|S]|%|[^]<String>]:<nnn>|@<String>|#<String>]...
int ValidateSegment( char* pCriteria, char* pData, int nIdx, void* pList )
{
 	char szTmp[30], szWord[30];
	char *pCur, *pos, *pFld, ch, *ts, *rs;
	int ss, i, tt, ns, nPos;
	STRLIST *pStrList;

	ss = TRUE;
	pCur = pCriteria;

	while ( pCur[0] && pCur[0]==' ' ) pCur++;
	
	// trival solution
	if ( !pCur[0] || strncmp(pCur,"ANY",3)==0 || strncmp(pCur,"ALL",3)==0) return ss;

	while (	 pCur[0] ) 
	{
		// Scan a string, may have "
		if (pCur[0]=='"')
		{	
			ch=0;
			sscanf( pCur+1, "%29[^\"]%c", szTmp, &ch); szTmp[29]=0;
			pCur += strlen( szTmp )+(ch?2:1);
		}
		else
		{
			ch = 0;
			sscanf( pCur, "%29[^\~|]&c", szTmp, &ch );	szTmp[29]=0;
			pCur += strlen( szTmp )+(ch?1:0);
		}

		// STREXPAND( szTmp, szTmp )
		ts = szTmp; rs = szTmp;
		while (ts[0]) { TCHEXPAND( ts, rs ) }
		rs[0] = 0;

		// Check '@' or '#' for re-location pData
		if (szTmp[0]=='@' ||szTmp[0]=='#')
		{
			pos = strstr( pData, szTmp+1 );
			if (pos!=NULL)
			{
				pos += strlen( szTmp+1 );
				ns = strlen(pos);
				memmove( pData, pos, ns+1 );
				pData[ns]=0;
			}
			else
				return (szTmp[0]=='#'?-1:0);
		}
		else if (szTmp[0]=='%')
		{
			ns = atoi(szTmp+2);
			pos = strchr(szTmp,'@');
			if ( pos==NULL  )
				ss &= ( ns == nIdx );
			else  if ( pList!=NULL )
			{
				++pos; pStrList=(STRLIST*)pList;
				SRCHSTRLIST(pos, pStrList);
				ss &= (i<16 && (ns == pStrList[i].nCnt));
			}
		}
		else
		{
			ns = 1;
			szWord[0] = 0;
			pFld = szTmp;
			if ( pFld[0]=='-' || pFld[0]=='+' ) 
			{ 
				if (pFld[0]=='-') ns=-1;
				ch = pFld[1];
				nPos = atoi( pFld+3 );
			}
			else 
			{
				if ( pFld[0]=='^' ) { ns=-1; pFld++; }
				ch=0;
				sscanf( pFld, "%29[^:]%*c%d", szWord, &nPos ); szWord[29]=0;
			}
			
			switch (ch)
			{
			case 'D':
				tt = ( nPos<=(int)strlen(pData) && pData[nPos]>='0' && pData[nPos]<='9' );
				break;
			case 'C':
				tt = ( nPos<=(int)strlen(pData) && ( pData[nPos]>='a' && pData[nPos]<='z' ||
					   pData[nPos]>='A' && pData[nPos]<='Z' ) );
				break;
			case 'S':
				tt = (nPos<=(int)strlen(pData) && pData[nPos]==' ');
				break;
			default:
				if (nPos>0 && nPos<=(int)strlen(pData))
					tt = (strncmp( pData+nPos, szWord, strlen(szWord) ) == 0);
				else
					tt = FALSE;
			}
			if (ns==-1) tt=!tt;
			ss = ss && tt;
		}

		while ( pCur[0] && (pCur[0]==' '||pCur[0]=='~') ) pCur++;
		if ( !pCur[0] ) break;
	}
	return ss;
}





#define		SETVALUE(x,y,z)	if ( x[0]!='#' ) SetOneValue( x, y ); \
							else \
							{	z[0]=0; GetOneValue( x+1, z, sizeof(z)); \
								if ( z[0]==0 ) SetOneValue( x+1, y ); \
							}


#define		SETVALUEEX(x,y,z)	if ( x[0]!='#' ) SetOneValueEx( x, y ); \
							else \
							{	z[0]=0; GetOneValue( x+1, z, sizeof(z)); \
								if ( z[0]==0 ) SetOneValueEx( x+1, y ); \
							}


//////////////////////////////////////////////////////////////////////
// CPatParse
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
// pszContFile is the file contain content from HL7 message
// pszWorkDir is the directory to hold some working files
CPatParse::CPatParse( LPTSTR pszContFile, LPTSTR pszWorkDir )
{
#ifdef		XML_DEF
	CreateXMLObj();
	g_nKeys = 0;
#endif
	InitBuffer( pszContFile, pszWorkDir );
	m_nCurGroup = 0;
	m_nBLen = 0;
	gchL = '|';
	m_nGTime = time(NULL);
	
	m_bWantRels = true;
}


CPatParse::~CPatParse( )
{
	if ( m_pFile != NULL ) fclose( m_pFile );
	m_pFile = NULL;
	if (m_pContent!=NULL) delete m_pContent;

	/*for ( i=0; i<MAXNUMFLDS; i++ ) 
	{
		if (m_szKeyValue[i] != NULL) delete m_szKeyValue[i];
		if (g_szKeyValue[i] != NULL) delete g_szKeyValue[i];
	}*/

#ifdef		XML_DEF
	if ( m_bWantRels ) ReleaseXMLObj();
	InitValues( m_bWantRels, false );
#else
	InitValues( true, true );
#endif
}


void CPatParse::InitBuffer( LPTSTR pszContFile, LPTSTR pszWorkDir )
{
	// if ( m_pFile != NULL ) fclose( m_pFile );
	m_pFile = NULL;
	// if (m_pContent!=NULL) delete m_pContent;
	m_pContent = NULL;

	m_pwFile = NULL;
	m_nRecF = 0;


	m_szBuf[0] = 0;
	m_nGroup = 0;
	m_nSegment = 0;
	m_nDBFlds = 0;
	m_nDBFStart = 0;
	m_nDBFLen = 0;

	m_strFilter = "";
	
	m_bSaveVar = false;
	memset(szVarNameSave, 0, sizeof(szVarNameSave));

	m_szCurTask[0]=0;
	sprintf( m_szCurTask, "S%04d", ((int)time(NULL))%10000 );

#ifdef		XML_DEF
	InitValues( false, false );
#else
	InitValues( false, true );
#endif

	if ( pszContFile==NULL )
		strcpy( m_szContFile, "RptBody.txt" );
	else
		strncpy( m_szContFile, pszContFile, _MAX_PATH-1 );

	if ( pszWorkDir==NULL )
		m_szWorkDir[0] = 0;
	else
		strncpy( m_szWorkDir, pszWorkDir, _MAX_PATH-1 );

}


BOOL CPatParse::OpenFile( LPCTSTR pszFileName, LPCTSTR pszMode  )
{
	char szTmp[120], *pszBuf, *pos;
	strcpy( szTmp, pszFileName );
	strupr( szTmp );

	if ( strcmp( pszMode, "DBF" )==0 || strstr(szTmp, ".DBF")!=NULL )
	{
		m_pFile = fopen( pszFileName, "rb" );
		if ( m_pFile == NULL ) return FALSE;
		fread( szTmp, 1, 32, m_pFile );
		m_nDBFStart = *(short*)(szTmp+8);
		m_nDBFLen = *(int*)(szTmp+10);
		pszBuf = new char[m_nDBFStart+32];
		memcpy( pszBuf, szTmp, 32 );
		fread( pszBuf+32,1,m_nDBFStart-32, m_pFile );
		SetDBFStruct( pszBuf, m_nDBFStart, 1 );
		delete pszBuf;
		fseek( m_pFile, m_nDBFStart, SEEK_SET );
		m_nCurGroup = 0;
		m_nPrsCnt = 0;
		return true;
	}
	else
	{
		m_pFile = fopen( pszFileName, pszMode );
		if ( m_pFile == NULL ) return FALSE;
		
		m_nDBFStart = 0;
		fread( szTmp, 1, 32, m_pFile );
		fseek( m_pFile, 0, SEEK_SET );
		szTmp[31]=0;
		pos = strstr( szTmp, "\x0BMSH" );
		if ( pos!=NULL )
		{
			gchL = 0x0B;
		}
		else
		{
			pos = strstr( szTmp, "MSH" );
			if ( pos!=NULL && !isalpha(pos[3]) )
			{
				gchL = pos[3];
			}
		}
		m_nRecF = 0;
		memset( m_StrList, 0, sizeof(m_StrList));
		m_nCurGroup = 0;
		m_nPrsCnt = 0;
		return TRUE;
	}
}


BOOL CPatParse::CloseFile( )
{
	if ( m_pFile!=NULL ) fclose( m_pFile );
	m_pFile = NULL;
	m_nCurGroup = 0;
	m_nPrsCnt = 0;
	m_nDBFStart = 0;
	return TRUE;
}


/////////////////////////////////////////////////////
///////////////  DBF Operation
//////////////////////////////////////////////////////
//////////////
BOOL CPatParse::OpenDBFWrite( LPCTSTR pszFileName )
{
	char szTmp[120];
	char pszBuf[8000];

	if (m_pwFile!=NULL) return false;

	m_pwFile = fopen( pszFileName, "rb+" );
	if ( m_pwFile == NULL ) return FALSE;
	fread( szTmp, 1, 32, m_pwFile );

	m_nwRecCnt = *(int*)(szTmp+4);
	m_nwDBFStart = *(short*)(szTmp+8);
	m_nwDBFLen = *(int*)(szTmp+10);

	// pszBuf = new char[m_nDBFStart+32];
	memcpy( pszBuf, szTmp, 32 );
	fread( pszBuf+32,1,m_nwDBFStart-32, m_pwFile );
	SetDBFStruct( pszBuf, m_nwDBFStart, 3 );
	// delete pszBuf;
	
	int nn = fseek( m_pwFile, m_nwDBFStart, SEEK_SET );
	return (nn==0);
}


void CPatParse::CloseDBFWrite()
{
	if ( m_pwFile!=NULL ) fclose( m_pwFile );
	m_pwFile = NULL;
}


//  Add Record to a DBF File, 
//    **  nOption - 0: Append to End, 1:  Update if exist
void CPatParse::AddOneRecord( int nOption )
{
	int k, l, p, m, ss=0;
	TCHAR chTs, szBuf[8000], szName[32], szValue[2000], szTemp[8000];

	if (m_nKeys==0 || m_pwFile==NULL ) return;

	memset( szBuf, 32, sizeof(szBuf) );
	for(p=0; p<m_nwDBFlds; p++)
		if (m_stwDBFlds[p].chType=='M')
			memset( szBuf+m_stwDBFlds[p].nPos, 0, 4 );

#ifdef		XML_DEF
	TCHAR szData[8000], *ps;
	TCHAR ch1, ch2;

	szData[0]=0; ps=szData;
	SetOneRecord( szData, sizeof(szData)-1, 0 );
	while ( ps[0] )
	{
		ch1=0; ch2=0; szName[0]=0; szValue[0]=0;
		sscanf( ps,"%[^=]%c%[^|]%c", szName, &ch1, szValue, &ch2 );
		ps += (strlen(szName)+strlen(szValue)+(ch1?1:0)+(ch2?1:0));
		while (ps[0]==' '||ps[0]=='|') ++ps;
#else
	int i;

	for ( i=0; i<m_nKeys; i++ )
	{
		strncpy( szName, m_szKeyName[i], 20);
		if ( m_szKeyValue[i]!= NULL )
			strncpy( szValue, m_szKeyValue[i], sizeof(szValue)-1);
		else
			szValue[0] = 0;
#endif
		strupr( szName );

		// Search the Position and Length
		k = 0; l=0;
		for (p=0; p<m_nwDBFlds; p++)
		{
			m = strlen(m_stwDBFlds[p].szName);
			if ( strlen(szName)<(unsigned)m) m=strlen(szName);
			if ( strncmp( m_stwDBFlds[p].szName, szName, m )==0 ) 
			{ 
				k=m_stwDBFlds[p].nPos;
				l=m_stwDBFlds[p].nLen;
				chTs=m_stwDBFlds[p].chType;
				break;
			}
		}

		// Form the Data Buffer
		if ( k>0 && szValue[0] )
		{
			if (chTs=='N'||chTs=='F')
			{
				m = strlen(szValue); 
				if ( m>=l )
					strncpy( szTemp, szValue, l );
				else
				{
					memset( szTemp, 32, l );
					strncpy( szTemp+(l-m), szValue, m );
				}
			}
			else if ( chTs=='D' )
			{
				ParseDateTime(szValue, szTemp);
			}
			else 
			{
				memset( szTemp, 32, l );
				memcpy( szTemp, szValue, (strlen(szValue)<(unsigned)l?strlen(szValue):l));
				szTemp[l] = 0;
			}

			memcpy( szBuf+k, szTemp, l );
			ss = 1;
		}
	}

	if ( ss )
	{
		if ( nOption == 1 )
		{
			int i;
			// Update this record
			fseek( m_pwFile, m_nwDBFStart, SEEK_SET );
			for (i=0; i<m_nwRecCnt; i++)
			{
				fread( szTemp, 1, m_nwDBFLen, m_pwFile );
				if (memcmp(szBuf, szTemp, m_nwDBFLen)==0)
				{ ss=0; break; }
			}
		}
		if ( ss )
		{
			// Append this record
			fseek( m_pwFile, 4, SEEK_SET );
			fread( &m_nwRecCnt, 1, 4, m_pwFile );
			long nNewPos = m_nwDBFStart+ m_nwRecCnt*m_nwDBFLen;
			++m_nwRecCnt;
			fseek( m_pwFile, 4, SEEK_SET );
			fwrite( &m_nwRecCnt, 1, 4, m_pwFile );
			fseek( m_pwFile, nNewPos, SEEK_SET );
			fwrite( szBuf, 1, m_nwDBFLen, m_pwFile );
		}
	}
}


// Retrieve one Record (or Field List) from the DBF where pszKey=<pszValue>
BOOL CPatParse::RtvlOneRecord( LPTSTR pszKey, LPTSTR pszValue, LPTSTR pszFieldList )
{
	int i, k, l, p, m, ss=0;
	char chTs, szBuf[8000], szTemp[200], szTitle[20];

	// File not opened
	if (m_pwFile==NULL) return false;
	
	k = 0; l=0;
	strncpy( szTitle, pszKey, 20);
	strupr( szTitle );
	strupr( pszFieldList );

	for (p=0; p<m_nwDBFlds; p++)
	{
		m = strlen(m_stwDBFlds[p].szName);
		if ( strlen(szTitle)<(unsigned)m) m=strlen(szTitle);
		if ( strncmp( m_stwDBFlds[p].szName, szTitle, m )==0 ) 
		{ 
			k=m_stwDBFlds[p].nPos;
			l=m_stwDBFlds[p].nLen;
			chTs=m_stwDBFlds[p].chType;
			break;
		}
	}

	// No this Key
	if (k==0) return false;

	int nMid, nSkip;
	ss = 0;
	if ( m_nwRecCnt<=4000 )
	{
		// Small DBF, search from Beginning
		fseek( m_pwFile, m_nwDBFStart, SEEK_SET );
		for (i=0; i<m_nwRecCnt; i++)
		{
			fread( szBuf, 1, m_nwDBFLen, m_pwFile );
			if (strncmp(szBuf+k, pszValue, strlen(pszValue))==0)
			{ ss=1; break; }
		}
	}
	else
	{
		// Large DBF, Search from Middle
		nMid =  m_nwRecCnt-4000;
		nSkip = (nMid-1)*m_nwDBFLen;

		fseek( m_pwFile, m_nwDBFStart+nSkip, SEEK_SET );
		for (i=nMid; i<m_nwRecCnt; i++)
		{
			fread( szBuf, 1, m_nwDBFLen, m_pwFile );
			if (strncmp(szBuf+k, pszValue, strlen(pszValue))==0)
			{ ss=1; break; }
		}
		if (!ss)  // If not found, Search the First part again
		{
			fseek( m_pwFile, m_nwDBFStart, SEEK_SET );
			for (i=0; i<nMid; i++)
			{
				fread( szBuf, 1, m_nwDBFLen, m_pwFile );
				if (strncmp(szBuf+k, pszValue, strlen(pszValue))==0)
				{ ss=1; break; }
			}
		}
	}

	// No This Value
	if (ss==0) return false;

	// Add the values of this record to current Object
	for (p=0; p<m_nwDBFlds; p++)
	{
		if ( pszFieldList==NULL || strcmp(pszFieldList,"ALL")==0 || 
			 strstr(pszFieldList, m_stwDBFlds[p].szName)!=NULL )
		{
			memset( szTemp, 0, 200 );
			k=m_stwDBFlds[p].nPos;
			l=m_stwDBFlds[p].nLen;
			strncpy( szTemp, szBuf+k, l );
			i = l-1;
			while (i>0 && szTemp[i]==' ') szTemp[i--]=0;
			if (szTemp[0] )
				SetOneValueEx( m_stwDBFlds[p].szName, szTemp );
		}
	}
	return true;
}


BOOL CPatParse::DBFMoveFirst( )
{
	int nn;
	bool ss;

	if (m_pwFile==NULL) return false;
	nn = fseek( m_pwFile, m_nwDBFStart, SEEK_SET );
	if ( nn ) return false;

	ss = true;
	while ( ss )
	{
		nn = ftell( m_pwFile );
		if (nn >= m_nwDBFStart+m_nwDBFLen*m_nwRecCnt || feof(m_pwFile)) 
		{ ss = false; break; }
		nn = fread( m_szBuf, 1, m_nwDBFLen, m_pwFile );
		if ( nn<m_nwDBFLen ) 
		{ ss=false; fseek(m_pwFile, -nn, SEEK_CUR); break; }
		if ( m_szBuf[0]!='*' ) 
		{
			if (m_strFilter.IsEmpty()) break;
			else
			{
				DBFRequery();
				if (CheckExpress(m_strFilter)) break;
			}
		}
	}
	if (ss) fseek(m_pwFile, -m_nwDBFLen, SEEK_CUR);
	return ss;
}


BOOL CPatParse::DBFMoveNext( )
{
	bool ss;
	int nn;
	if (m_pwFile==NULL) return false;
	ss = true;
	fseek(m_pwFile, m_nwDBFLen, SEEK_CUR);
	while ( ss )
	{
		nn = ftell( m_pwFile );
		if (nn >= m_nwDBFStart+m_nwDBFLen*m_nwRecCnt || feof(m_pwFile)) 
		{ ss = false; break; }
		nn = fread( m_szBuf, 1, m_nwDBFLen, m_pwFile );
		if ( nn<m_nwDBFLen ) 
		{ ss=false; fseek(m_pwFile, -nn, SEEK_CUR); break; }
		if ( m_szBuf[0]!='*' ) 
		{
			if (m_strFilter.IsEmpty()) break;
			else
			{
				DBFRequery();
				if (CheckExpress(m_strFilter)) break;
			}
		}
	}
	if (ss) fseek(m_pwFile, -m_nwDBFLen, SEEK_CUR);
	return ss;
}


BOOL CPatParse::DBFUpdate( )
{
	if (m_pwFile==NULL) return false;
	int mm = ftell(m_pwFile);
	if ( mm < m_nwDBFStart ) return false;
	fwrite( m_szBuf, 1, m_nwDBFLen, m_pwFile );
	fseek( m_pwFile, -m_nwDBFLen, SEEK_CUR );
	return true;
}


BOOL CPatParse::DBFDelete( )
{
	if (m_pwFile==NULL) return false;
	if ( ftell(m_pwFile) < m_nwDBFStart ) return false;
	m_szBuf[0]='*';
	fwrite( m_szBuf, 1, m_nwDBFLen, m_pwFile );
	fseek( m_pwFile, -m_nwDBFLen, SEEK_CUR );
	return true;
}


BOOL CPatParse::DBFRequery( )
{
	int i,k,l,p;
	char szTemp[500], *ps;

	if (m_pwFile==NULL) return false;
	InitValues(true, false);
	for (p=0; p<m_nwDBFlds; p++)
	{
		memset( szTemp, 0, 500 );
		k=m_stwDBFlds[p].nPos;
		l=m_stwDBFlds[p].nLen;
		if (l>0) strncpy( szTemp, m_szBuf+k, (l<500?l:499) );
		else szTemp[0]=0;
		i = l-1;
		while (i>0 && szTemp[i]==' ') szTemp[i--]=0;
		ps = szTemp;
		while (ps[0] && ps[0]==' ') ++ps;
		if (ps[0] )
			SetOneValueEx( m_stwDBFlds[p].szName, ps );
	}
	return true;
}


BOOL CPatParse::DBFEdit( )
{
	int k,l,m,p;
	char szTemp[500];

	if (m_pwFile==NULL) return false;
	for (p=0; p<m_nwDBFlds; p++)
	{
		k=m_stwDBFlds[p].nPos;
		l=m_stwDBFlds[p].nLen;
		memset( m_szBuf+k, ' ', l );
		GetOneValue( m_stwDBFlds[p].szName, szTemp, 500 );
		m = strlen(szTemp); 
		if ( l<m ) m=l;
		memcpy( m_szBuf+k, szTemp, m );
	}
	return true;
}


BOOL CPatParse::DBFIsEOF( )
{
	if (m_pwFile==NULL) return false;
	int nn = ftell( m_pwFile );
	return ( nn >= m_nwDBFStart+m_nwDBFLen*m_nwRecCnt || feof(m_pwFile));
}


// Evaluate the '&'(and) connected '=' express
BOOL CPatParse::CheckExpress( LPCTSTR pszExpr )
{
	LPTSTR pos, pks, pvs;
	char szKey[20], szEVal[100], szDVal[100], ch1, ch2;
	bool ss;
	int nn;

	pos = (LPTSTR)pszExpr; ss = true;
	while ( pos[0] && ss )
	{
		ch1=0; ch2=0; szKey[0]=0; szEVal[0]=0;
		sscanf( pos, "%[^=]%c%[^&]%c", szKey, &ch1, szEVal, &ch2 );
		pos += (strlen( szKey )+strlen( szEVal)+(ch1?1:0)+(ch2?1:0));
		
		strupr( szKey ); 
		pks=szKey; while (pks[0] && pks[0]==' ') ++pks;
		nn=strlen(pks)-1; while (pks[nn-1]==' ') pks[--nn]=0;
		pvs=szEVal; while (pvs[0] && pvs[0]==' ') ++pvs;
		nn=strlen(pvs)-1; while (pvs[nn-1]==' ') pvs[--nn]=0;

		GetOneValue( pks, szDVal, sizeof(szDVal) );
		ss &= (strcmp(pvs, szDVal )==0);
	}
	return ss;
}


BOOL CPatParse::FindProperGroup( bool bValid )
{
	int i, nsv;

	if ( (m_stGroup[m_nCurGroup].nOption & OPT_TFLGRP_CNTLMT) || bValid )
	{
		if (m_nPrsCnt >= m_stGroup[m_nCurGroup].nReqCnt || bValid)
		{
			nsv=m_nCurGroup;
			while ( ++m_nCurGroup<m_nGroup &&
					!(m_stGroup[m_nCurGroup].nOption & (OPT_TFLGRP_CNTLMT|OPT_TFLGRP_2VALID)) );
			if (m_nCurGroup>=m_nGroup)
			{
				// Go back to search the repeat start point
				i = 0; 
				while ( i<m_nGroup && !(m_stGroup[i].nOption & OPT_TFLGRP_RPTSTRT) ) ++i;
				if ( i >= m_nGroup || i==nsv ) return FALSE;
				m_nCurGroup = i;
			}
			m_nPrsCnt = 0;
		}
	}

	for ( i=0; i<m_nCurGroup; i++ ) m_stGroup[i].nEffect = 0;
	i = m_nCurGroup;
	m_stGroup[i].nEffect = 1;
	while ( ++i<m_nGroup && !(m_stGroup[i].nOption & (OPT_TFLGRP_CNTLMT|OPT_TFLGRP_2VALID)) )
		m_stGroup[i].nEffect = 1;
	return TRUE;
}



// Template may contain extra data start with #[Ext], Parse procedure
// Should ignore it.
int CPatParse::GetOneRecord( LPTSTR pszTmplt, LPTSTR pszBuf, int nOption, int nLen )
{
	TCHAR ch, *ps, szVar[128], szBuf[200];
	char szTmp[120], *pszTmp;
	int k;

	// InitValues( true, false );

	//* memset(szVarNameSave, 0, sizeof(szVarNameSave));
	if ( szVarNameSave[0] )
	{
		m_bSaveVar = false;

		ps = szVarNameSave;
		while ( ps[0] )
		{
			ch=0; szVar[0]=0;
			sscanf(ps, "%[^|]%c", szVar, &ch);
			ps += strlen(szVar)+(ch?1:0);
			SetOneValue( szVar, "", 1 );
		}

		memset(szVarNameSave, 0, sizeof(szVarNameSave));
	}

	if (m_pContent!=NULL)
	{
		delete m_pContent;
		m_pContent=NULL;
	}

	// Process the Buffer directly
	if ( pszBuf!=NULL )
	{
		if ( m_szBuf != pszBuf )
		{
			if ( nLen ) 
			{
				memcpy((CHAR*)m_szBuf, pszBuf, nLen);
				m_nBLen = nLen;
			}
			else
				strncpy((CHAR*)m_szBuf, pszBuf, sizeof(m_szBuf));
		}
		// strupr( (CHAR *)m_szBuf);

		if ( nOption == HL7_FORMAT )
		{
			m_bSaveVar = true;
			if (ParseTemplate( pszTmplt ))
			{
				++m_nRecF;
				ExtraManipulate( pszTmplt );

				m_bSaveVar = false;

				// Extra Validation
				szBuf[0]=0;
				GetOneValue( "ExtValid", szBuf, 100 );
				if ( strstr(pszTmplt,"ExtValid")!=NULL && atoi(szBuf)==0 )
					return IMP_REC_NONE;

				// Multiple Segments Checking
				szBuf[0]=0;
				GetOneValue( "SegCont", szBuf, 100 );
				if ( atoi( szBuf ) > 0 )
					return IMP_REC_MORE;
				else
					return IMP_REC_NORMAL;
			}
			else
			{
				m_bSaveVar = false;
				return IMP_REC_NONE;
			}
		}
		else if ( nOption == TFL_FORMAT )
		{
			m_bSaveVar = true;
			SetTFLData( pszTmplt );
			for (k=0; k<m_nGroup; k++) m_stGroup[k].nEffect = 1;
			if (ParseTFLTmplt( pszTmplt ))
			{
				++m_nRecF;
				if (m_nKeys>0) ExtraManipulate( pszTmplt );
				m_bSaveVar = false;
				return (m_nKeys>0?IMP_REC_NORMAL:IMP_REC_NONE);
			}
			else
			{
				m_bSaveVar = false;
				return IMP_REC_NONE;
			}
		}
		
		return IMP_REC_NONE;
	}

	if (m_pFile==NULL) return IMP_REC_END; 
	if (feof(m_pFile)) 
	{
		fclose(m_pFile);
		m_pFile=NULL;
		return IMP_REC_END;
	}
	else
	{
		// fscanf( m_pFile,"%[^\n]%c", m_szBuf, &ch );
		int nState = 0, nPos = 0, nTmp;
		char tmps[10]={0};

		if ( nOption == HL7_FORMAT )
		{
			while ( (nTmp=getc(m_pFile)) != EOF )
			{
				ch = nTmp;
				if ( nState && (nPos <= sizeof(m_szBuf)-5 ) ) m_szBuf[nPos++] = ch;
				tmps[0]=tmps[1]; tmps[1]=tmps[2];
				tmps[2]=tmps[3]; tmps[3]=ch;
				if ( gchL==0x0B )
				{
					if (strncmp(tmps+1, "MSH",3)==0 && tmps[0]==gchL) 
					{
						if ( nState )
						{
							ungetc( 'H', m_pFile );
							ungetc( 'S', m_pFile );
							ungetc( 'M', m_pFile );
							ungetc( gchL, m_pFile );
							nPos -= 4;
							break;
						}
						else
						{
							nState = 1;
							nPos = 4;
							strncpy( m_szBuf, tmps, 4 );
						}
					}
				}
				else
				{
					if (strncmp(tmps, "MSH",3)==0 && ch==gchL) 
					{
						if ( nState )
						{
							ungetc( ch, m_pFile );
							ungetc( 'H', m_pFile );
							ungetc( 'S', m_pFile );
							ungetc( 'M', m_pFile );
							nPos -= 4;
							break;
						}
						else
						{
							nState = 1;
							nPos = 5;
							m_szBuf[0]='\x0B';
							strncpy( m_szBuf+1, tmps, 4 );
						}
					}
				}
			}

			if ( m_szBuf[nPos-1]=='\x0B' )
				m_szBuf[--nPos]=0;
			m_szBuf[nPos] = 0; 
			if ( strchr( m_szBuf, '\x1C' )==NULL )
				m_szBuf[nPos++]='\x1C';
			m_szBuf[nPos++] = 0; 
			m_nBLen = nPos;

			// strupr( (CHAR *)m_szBuf);
			m_bSaveVar = true;
			if (ParseTemplate( pszTmplt ))
			{
				++m_nRecF;
				ExtraManipulate( pszTmplt );
				m_bSaveVar = false;

				// Extra Validation
				szBuf[0]=0;
				GetOneValue( "ExtValid", szBuf, 100 );
				if ( strstr(pszTmplt,"ExtValid")!=NULL && atoi(szBuf)==0 )
					return IMP_REC_NONE;

				// Multiple Segments Checking
				szBuf[0]=0;
				GetOneValue( "SegCont", szBuf, 100 );
				if ( atoi( szBuf ) > 0 )
					return IMP_REC_MORE;
				else
					return IMP_REC_NORMAL;
			}
			else
				return IMP_REC_NONE;
		}
		else if ( nOption == TFL_FORMAT )
		{
			char szEnds[100];
			int nn, mm, nRes;
			bool bMustRet;
			int nPos = 0;

			m_szBuf[0];
			SetTFLData( pszTmplt );
			nRes = IMP_REC_END;

			// Handle Sequential Case  Set the proper value for m_nCurGroup
			//
			bMustRet = false;
			if (!FindProperGroup( false )) return nRes;

			while ( !feof( m_pFile ) )
			{
ReScanRec:
				fseek( m_pFile, 0L, SEEK_CUR );
				nPos = ftell( m_pFile );

				if ( m_stGroup[m_nCurGroup].Class[0]=='B' || m_stGroup[m_nCurGroup].Class[0]=='b' )
				{
					// DBF Header processing if necessary
					if ( m_nDBFStart == 0 )
					{
						fread( szTmp, 1, 32, m_pFile );
						m_nDBFStart = *(short*)(szTmp+8);
						m_nDBFLen = *(int*)(szTmp+10);
						pszTmp = new char[m_nDBFStart+32];
						memcpy( pszTmp, szTmp, 32 );
						fread( pszTmp+32,1,m_nDBFStart-32, m_pFile );
						SetDBFStruct( pszTmp, m_nDBFStart, 1 );
						delete pszTmp;
						fseek( m_pFile, m_nDBFStart, SEEK_SET );
						m_nCurGroup = 0;
						m_nPrsCnt = 0;
					}
					while ( !feof(m_pFile) ) 
					{
						nn = fread( m_szBuf,1,m_nDBFLen,m_pFile );
						m_szBuf[nn]=0;
						if ( m_szBuf[0]!='*' && nn==m_nDBFLen ) break;
					}
					if (feof(m_pFile) || nn<m_nDBFLen) return IMP_REC_END;
					m_nBLen = nn;
				}
				else if ( m_stGroup[m_nCurGroup].Class[0]=='F' && atoi(m_stGroup[m_nCurGroup].Delimitor)>1)
				{
					nn = atoi(m_stGroup[m_nCurGroup].Delimitor);
					if ( m_stGroup[m_nCurGroup].nOption & OPT_TFLGRP_SKIP )
						fseek( m_pFile, nn, SEEK_CUR );
					else
					{
						mm = fread( m_szBuf,1,nn,m_pFile );
						m_szBuf[mm]=0;
						if (mm<nn) bMustRet = true;
					}
					m_nBLen = mm;
						// return IMP_REC_END;
				}
				else
				{
					// STREXPAND( m_stGroup[0].Delimitor, szEnds )
					strncpy(szEnds, m_stGroup[m_nCurGroup].Delimitor, sizeof(szEnds));
					nn = strlen( szEnds );
					if ( nn>0 && strcmp(szEnds, "*SngRec")!=0 )
					{
						if ( m_stGroup[m_nCurGroup].nOption & OPT_TFLGRP_SKIP )
							nn = GetDataSegByExtStr(m_pFile, m_stGroup[m_nCurGroup].Delimitor, NULL, 1000);
						else
						{
							m_szBuf[0] = 0;
							nn = GetDataSegByExtStr(m_pFile, m_stGroup[m_nCurGroup].Delimitor, m_szBuf, sizeof(m_szBuf));
							m_nBLen = strlen(m_szBuf);
						}
						// Notice the end of current Group
						//
						if (nn==IMP_REC_END) bMustRet = true;
							// return nn;
					}
					else if (strcmp(szEnds, "*SngRec")==0)
					{
						nn = fread(m_szBuf,1,sizeof(m_szBuf),m_pFile);
						if ( nn==0 ) return IMP_REC_END;
						m_szBuf[nn] = 0;
						m_nBLen = nn;
					}
					else
						if (fgets( m_szBuf, sizeof(m_szBuf), m_pFile )==NULL)
						{	
							m_nBLen = strlen(m_szBuf);
							return IMP_REC_END;
						}
				}	// if ( m_stGroup[0].Class[0]=='B' )
				
				// Handle Multiple Groups
				SetupStrList( pszTmplt, m_szBuf );
				if ( m_stGroup[m_nCurGroup].nOption & OPT_TFLGRP_2VALID )
				{	
					if ( bMustRet ) return IMP_REC_END;
					if ( !ValidateSegment
						 (m_stGroup[m_nCurGroup].IdStr, m_szBuf, m_nRecF, m_StrList) )
					{
						if (!FindProperGroup( true )) return IMP_REC_END;
						// Rewind the File
						fseek( m_pFile, nPos, SEEK_SET ); 
						goto ReScanRec; 
					}
				}
				else if (bMustRet) return IMP_REC_END;
				
				m_nPrsCnt++;
				if ( m_stGroup[m_nCurGroup].nOption & OPT_TFLGRP_SKIP )
				{	// No processing, go find next ParseRule
					++m_nRecF;
					if (!FindProperGroup( false )) return IMP_REC_END;
				}
				else 
				{
					m_bSaveVar = true;
					//Test
					//MessageBox(NULL, m_szBuf, "BUFFER", MB_OK );
					//MessageBox(NULL, pszTmplt, "TEMPLATE", MB_OK );

					if (ParseTFLTmplt( pszTmplt ))
					{
						++m_nRecF;
						if ((m_nKeys+g_nKeys)>0) ExtraManipulate( pszTmplt );
						m_bSaveVar = false;
						nRes = ((m_nKeys+g_nKeys)>0?IMP_REC_NORMAL:IMP_REC_NONE);
						break;
					}
				}
			}
			// return (m_nKeys>0?IMP_REC_NORMAL:IMP_REC_NONE);
			return nRes;
		}
		return IMP_REC_NONE;
	}
}

// Remove the first record in the import file, file must be closed
int CPatParse::Rmv1stRecord( LPTSTR pszTmplt, LPTSTR pszFile, int nOption )
{
	TCHAR ch, *ts, *rs;
	int i;
	FILE *tmpfp;

	if (m_pFile!=NULL) return IMP_REC_NONE;

	m_pFile = fopen( pszFile, "rb+" );
	if (m_pFile==NULL) return IMP_REC_END;

	if (feof(m_pFile)) {
		fclose(m_pFile);
		m_pFile=NULL;
		return IMP_REC_END;
	}
	else
	{
		// fscanf(m_pFile,"%[^\n]%c",m_szBuf, &ch);
		int nState = 0, nTmp;
		char tmps[10]={0};

		if ( nOption == HL7_FORMAT )
		{
			while ( (nTmp=getc(m_pFile)) != EOF )
			{
				ch = nTmp;
				tmps[0]=tmps[1]; tmps[1]=tmps[2];
				tmps[2]=tmps[3]; tmps[3]=ch;
				if (strncmp(tmps, "MSH|",4)==0)
				{
					if ( nState )
					{
						ungetc( '|', m_pFile );
						ungetc( 'H', m_pFile );
						ungetc( 'S', m_pFile );
						ungetc( 'M', m_pFile );
						break;
					}
					else
						nState = 1;
				}
			}
		}
		else if ( nOption == TFL_FORMAT )
		{
			char szEnds[20];
			int nn, nRes;

			m_szBuf[0];
			SetTFLData( pszTmplt );
			nRes = IMP_REC_END;

			if ( m_stGroup[0].Class[0]=='B' )
			{
				fclose(m_pFile);
				m_pFile=NULL;
				return IMP_REC_NONE;
			}
			else if ( m_stGroup[0].Class[0]=='F' && atoi(m_stGroup[0].Delimitor)>1)
			{
				nn = atoi(m_stGroup[0].Delimitor);
				fseek( m_pFile, nn, SEEK_CUR );
			}
			else
			{
				// STREXPAND( m_stGroup[0].Delimitor, szEnds )
				ts = m_stGroup[0].Delimitor; rs = szEnds;
				while (ts[0]) { TCHEXPAND( ts, rs ) }
				rs[0] = 0;
				nn = strlen( szEnds );
				if (nn>0)
				{
					for(i=0; i<nn-1; i++) tmps[i]=0;
					while ( !feof(m_pFile) 
							&& (nTmp=getc(m_pFile)) != EOF )
					{
						ch = nTmp;
						for (i=0; i<nn-1; i++) tmps[i] = tmps[i+1];
						tmps[nn-1] = ch;
						if (strncmp(tmps, szEnds, nn)==0)  break;
					}
				}
				else
					fseek( m_pFile, 0, SEEK_END );
			}	// if ( m_stGroup[0].Class[0]=='B' )
		}
		
		TCHAR szDir[256]={0}, szFile[256]={0}; 
		
		// tmpfp = tmpfile();
		GetTempPath(256, szDir);
		if (GetTempFileName(szDir, "GF__", 0, szFile))
			tmpfp=fopen(szFile, "w+b");
		else
			tmpfp=NULL;
		if (tmpfp==NULL) return IMP_REC_NONE; 
		
		while ( (nTmp = fread(m_szBuf, 1, sizeof(m_szBuf)-10, m_pFile))>0 )
			fwrite( m_szBuf, 1, nTmp, tmpfp );
		fclose( m_pFile );

		m_pFile = fopen( pszFile, "wb" );
		if (m_pFile==NULL) 
		{
			fclose( tmpfp);
			DeleteFile( szFile );
			return IMP_REC_NONE;
		}
		fseek( tmpfp, 0, SEEK_SET );
		while ( (nTmp = fread(m_szBuf, 1, sizeof(m_szBuf)-10, tmpfp))>0 )
			fwrite( m_szBuf, 1, nTmp, m_pFile );
		fclose( m_pFile );
		m_pFile = NULL;

		fclose( tmpfp);
		DeleteFile( szFile );

		return IMP_REC_NORMAL;
	}
}


#define OutputBody(x)	{ if ( szType[0]=='T' || szType[1]=='T' ) { fprintf( fp, "%s ", x ); \
if ( pS2[nBuf+1]=='~' ) { ++nBuf; fprintf( fp, "\r\n\r\n" ); } } \
else if ( szType[0]=='R' || szType[1]=='R' ) fprintf( fp, "%s\\par \r\n", x ); \
else fprintf( fp, "%s\r\n", x ); }


// In the Template, each item is as follows:
//    <Event> <KeyName> <Segment> <Field> <SubFlds> <SubFlds2> <Pos> <Len> 
//    <SubFlds> and <SubFlds2> are also can be served as <Type> and <SubType> if
//    puting the Letters
//    <Segment>=XXX, not extracting, formating, and padding to previous one
//    <Segment>=<sss>* Get all <sss> Segment if there are more
//    <Segment>=<sss>*n Get the nth <sss> segment if it exists
//    <KeyName> start w/ #, take the new value f no previous
 
BOOL CPatParse::ParseTemplate( LPTSTR pszTmplt )
{
	int i, nn, nBuf, ss, n1, n2, n3, nSv, k, kn;
	LPTSTR pCur, pS1, pS2, pS3, pos, ps, ts, pSv, pNv;
	CHAR szResult[400], szTmp[400], szKVal[200], szOld[400], szTempFN[200], ch;
	CHAR szEvent[20], szKName[20], szSegment[20], szMark[8], szMark2[8];
	CHAR szType[20], szSubType[20], szPos[20], szLen[20];
	CHAR szSegMk[4000]={0}, szTB[4000]={0};
	int nField, nSubF, nSubF2, nPos, nLen, nLmt, nSegC, nSegT;
	bool bEnd, bTItem;
	FILE *fp;

	//InitValues( true, false );
	memset(szVarNameSave, 0, sizeof(szVarNameSave));

	pSv = (LPTSTR)m_szBuf;
	nSv = strlen(pSv);

	// Prepare a file for report body
	fp = NULL;
	pCur = strstr( (LPTSTR)m_szBuf, "MSH");
	if (pCur==NULL) return FALSE;
	m_cK1=pCur[3];
	m_cK2=pCur[4];
	m_cK3=pCur[5];
	m_cK4=pCur[6];
	szMark[0]=m_cK1; szMark[1]='\x0D'; szMark[2]=0;
	szMark2[0]=m_cK1; szMark2[1]=0; 

	// AllList, parse entire text
	if ( strncmp( pszTmplt, "AllList", 7 )==0 || strncmp( pszTmplt+4, "AllList", 7 )==0 )
	{
		bTItem = true;
		while ( pCur[0] )
		{
			i = 0;
			while ( pCur[0] && pCur[0]!=m_cK1 ) szType[i++]=(pCur++)[0];
			szType[i]=0; if (pCur[0]==m_cK1) ++pCur;
			
			i=0;
			strcpy( szSegment, szType ); 
			strcat( szSegment, "," );
			while ( strstr( szSegMk, szSegment )!=NULL )
			{
				sprintf( szSegment, "%s-%d,", szType, ++i);
			}
			strcat( szSegMk, szSegment );
			if ( i>0 ) { strcpy( szType, szSegment); szType[strlen(szType)-1]=0; }
			n1 = 0; 
			if ( i>=3 )
			{
				while ( pCur[0]>=' ' ) ++pCur;
			}
			else
			// Processing one segment
			while ( pCur[0]>=' ' )
			{
				i = 0; nLmt = sizeof(szTmp)-1; 
				while ( pCur[0]>=' ' && pCur[0]!=m_cK1 ) if (i<nLmt) szTmp[i++]=(pCur++)[0];
				szTmp[i] = 0; if (pCur[0]==m_cK1) ++pCur; ++n1;

				if ( szTmp[0] )
				{
					if ( strchr( szTmp, m_cK2 ) == NULL )
					{
#if	defined(XML_DEF)
						sprintf( szKName, "HL7.%s.T%d", szType, n1 );
						SetOneValue( szKName, szTmp, 1 );
#endif
						sprintf( szKName, "%s_%d", szType, n1 );
						SetOneValue( szKName, szTmp, 1 );
					}
					else
					{
						n2=0; strcpy( szResult, szTmp ); ps = szResult;
						while ( ps[0] )
						{
							i = 0; nLmt=sizeof(szTmp)-1;
							while ( ps[0] && ps[0]!=m_cK2 ) if (i<nLmt) szTmp[i++]=(ps++)[0];
							szTmp[i] = 0; if (ps[0]==m_cK2) ++ps; ++n2;

							if ( szTmp[0] )
							{
								if ( strchr( szTmp, m_cK3 ) == NULL )
								{
#if	defined(XML_DEF)
									sprintf( szKName, "HL7.%s.T%d.S%d", szType, n1, n2 );
									SetOneValue( szKName, szTmp, 1 );
#endif
									sprintf( szKName, "%s_%d_%d", szType, n1, n2 );
									SetOneValue( szKName, szTmp, 1 );
								}
								else
								{
									n3=0; strcpy( szOld, szTmp ); ts = szOld;
									while ( ts[0] )
									{
										i = 0; nLmt=sizeof(szTmp)-1;
										while ( ts[0] && ts[0]!=m_cK3 ) if (i<nLmt) szTmp[i++]=(ts++)[0];
										szTmp[i] = 0; if (ts[0]==m_cK3) ++ts; ++n3;

										if ( szTmp[0] )
										{
#if	defined(XML_DEF)
											sprintf( szKName, "HL7.%s.T%d.S%d.P%d", szType, n1, n2, n3 );
											SetOneValue( szKName, szTmp, 1 );
#endif
											sprintf( szKName, "%s_%d_%d_%d", szType, n1, n2, n3 );
											SetOneValue( szKName, szTmp, 1 );
										}
									}
								}
							}
						}
					}
				}
			}

			while ( pCur[0]==0x0D || pCur[0]==0x0A ) ++pCur;
			if ( pCur[0]==0x1C ) break;
		}
		return TRUE;
	}

	// Check wheher to extract the TextItem
	bTItem = (strstr( pszTmplt, "TextItem" )!=NULL);
	while ( true ) // partition the buffer of contents into segments
	{
	  if ( bTItem ) // Get One Segment 
	  {
		pNv = strstr( pSv, szMark);
		if ( pNv!=NULL) nSv = (pNv-pSv)+2;
		else nSv = strlen(pSv);
	  }

	  pCur = pszTmplt; // Parse one segment by segment
	  while ( pCur[0] ) 
	  {
		bEnd = false;
		szTmp[0]=0;	ch=0;
		while ( pCur[0]=='|' ||	pCur[0]=='\n' || pCur[0]=='\r' ) ++pCur;
		if (pCur[0]==0) break;
		sscanf( pCur, "%[^|\n]%c", szTmp, &ch );
		pCur += strlen(szTmp)+(ch?1:0);
		
		szKName[0]=0; szSegment[0]=0;
		nField=0; nSubF=0; nSubF2=0; nPos=0; nLen=0;
		nn = sscanf( szTmp, "%s %s %s %d %s %s %s %s",
			szEvent, szKName, szSegment, &nField, szType, szSubType, szPos, szLen );
		nSubF = atoi(szType);
		nSubF2 = atoi(szSubType);
		nPos = atoi(szPos);
		nLen = atoi(szLen);

//		if ( nn==8 && (strcmp(szEvent,"000")==0 || strstr( (LPTSTR)m_szBuf, szEvent )!= NULL) )
		if ( nn==8 && CheckHL7Event((LPTSTR)m_szBuf, szEvent) )
		{
		  ss = false; nSegT=-1;	nSegC=0;
		  if ((pos=strchr(szSegment, '*'))!=NULL)
		  {
			  ss=true; nSegT=atoi(pos+1); pos[0]=0; 
		  }

		  // Start the buffer
		  if (bTItem)
		  {
			pos = pSv;
		  }
		  else												 
			pos = (LPTSTR)m_szBuf;

		  char tmpSegment[16]={0};
		  tmpSegment[0]=0x0A, tmpSegment[1]=0;
		  strcat(tmpSegment, szSegment);
		  strcat(tmpSegment, szMark2);

		  while ( ( (pS1 = strstr( pos, tmpSegment)) != NULL) || 
				  ( (tmpSegment[0]=0x0D, pS1 = strstr( pos, tmpSegment)) != NULL ) ||
				  ( (tmpSegment[0]=0x0B, pS1 = strstr( pos, tmpSegment)) != NULL ) ||
			      strcmp(szSegment,"XXX")==0 )  
		  {
			++nSegC;
			if (ss && (nSegT>0) && (nSegC!=nSegT)) 
			{ 
				pos = pS1 + strlen( tmpSegment );
				continue;
			}

			if (strcmp(szSegment,"XXX")!=0)
			{
			  if (bTItem && (pS1-pos)>=nSv) break;

			  pS1--;
			  ch = 0;
			  if (nField>0) 
			  {
				pS2 = pS1+strlen(szSegment);
				for(i=0; i<nField; i++) 
				{
					// pS2 = strchr( pS2, m_cK1 );
					while ( pS2[0] && pS2[0]!=m_cK1 && pS2[0]!='\n' && pS2[0]!='\r' ) ++pS2;
					if ( pS2[0]!=m_cK1 ) goto Loop_end;
					else pS2 += sizeof(m_cK1);
				}
				ch = m_cK1;
			  }
			  if (nSubF>0) 
			  {
				for(i=0; i<nSubF-1; i++) 
				{
					if (pS2[0]==m_cK1) goto Loop_end;
					pS3 = strchr( pS2, m_cK1 );
					pS2 = strchr( pS2, m_cK2 );
					if (pS2==NULL) goto Loop_end;
					else if (pS3!=NULL && pS2>pS3) goto Loop_end;
					else pS2 += sizeof(m_cK2);
				}
				ch = m_cK2;
			  }
			  if (nSubF2>0) 
			  {
				for(i=0; i<nSubF2-1; i++) 
				{
					if (pS2[0]==m_cK1 || pS2[0]==m_cK2 ) goto Loop_end;
					pS3 = strchr( pS2, m_cK1 );
					pS2 = strchr( pS2, m_cK3 );
					if (pS2==NULL) goto Loop_end;
					else if (pS3!=NULL && pS2>pS3) goto Loop_end;
					else pS2 += sizeof(m_cK3);
				}
				ch = m_cK3;
			  }
			}

			if ( pS2==NULL ) 
			{
				pos = pS1 + strlen( tmpSegment );
				continue;
			}

			// Add Contents Check, Extract the contents to a file
			if ( stricmp(szKName,"RptBody")==0 || stricmp(szKName,"RptLine")==0 )
			{
				if ( fp == NULL )
				{
					szTempFN[0]=0;
					GetOneValue( "OutputFN", szTempFN, sizeof(szTempFN));
					if ( szTempFN[0] )
						fp = fopen( szTempFN, "wb" );
					else
						fp = fopen( m_szContFile, "wb" );
				}

				if ( fp!=NULL )
				{
					nBuf = 0;
					fseek( fp, 0L, SEEK_END );
					if ( stricmp(szKName,"RptLine")==0 )
					{
						// Need Parse some special characters
						while ( pS2[nBuf] && pS2[nBuf]!=ch ) ++nBuf; 
						// strncpy( szTB, pS2, nBuf );
						// szTB[nBuf]=0;
						fwrite( pS2, 1, nBuf, fp );
						fprintf( fp, "\r\n" );
					}
					else
					{
						k = 0;
						while ( pS2[nBuf] && pS2[nBuf]!=ch ) 
						{
							if ( pS2[nBuf]=='~' || k>=sizeof(szOld)-1 ) 
							{
								szOld[k]=0;
								/* if ( szType[0]=='T' || szType[1]=='T' )
								{
									fprintf( fp, "%s ", szOld );
									if ( pS2[nBuf+1]=='~' )
									{
										++nBuf;
										fprintf( fp, "\r\n\r\n" );
									}
								}
								else if ( szType[0]=='R' || szType[1]=='R' )
									fprintf( fp, "%s\\par \r\n", szOld );
								else
									fprintf( fp, "%s\r\n", szOld );	*/
								OutputBody( szOld );
								++nBuf;
								k=0;
							}
							else if ( strnicmp(pS2+nBuf,"\\n\\",3 )==0 )
							{
								szOld[k]=0;
								OutputBody( szOld );
								k=0; nBuf+=3; 
							}
							else if ( strnicmp(pS2+nBuf,"\\n",2 )==0 )
							{
								szOld[k]=0;
								OutputBody( szOld );
								k=0; nBuf+=2; 
							}
							else if ( strnicmp(pS2+nBuf,"\\E\\n",4 )==0 )
							{
								szOld[k]=0;
								OutputBody( szOld );
								k=0; nBuf+=4; 
							}
							else if ( strnicmp(pS2+nBuf, "\\.br\\",5 )==0 )
							{
								szOld[k]=0;
								OutputBody( szOld );
								k=0; nBuf+=5; 
							}
							else if ( strnicmp(pS2+nBuf,"\\F",2 )==0 ) { szOld[k++]='|'; nBuf+=2; }
							else if ( strnicmp(pS2+nBuf,"\\S",2 )==0 ) { szOld[k++]='^'; nBuf+=2; }
							else if ( strnicmp(pS2+nBuf,"\\R",2 )==0 ) { szOld[k++]='~'; nBuf+=2; }
							else if ( strnicmp(pS2+nBuf,"\\T",2 )==0 ) { szOld[k++]='&'; nBuf+=2; }
							else if ( strnicmp(pS2+nBuf,"\\E",2 )==0 ) { szOld[k++]='\\'; nBuf+=2; }
							else
							{
								szOld[k++]=pS2[nBuf++];
							}
						}
						szOld[k]=0;
						if ( szType[0]=='R' || szType[1]=='R' )
							fprintf( fp, "%s\\par", szOld );
						else
							fprintf( fp, "%s\r\n", szOld );
						// fwrite( pS2, 1, nBuf, fp );
					}
					// SetOneValue( (LPTSTR)szKName, pS2 );
				}
			}
			else
			{
				if (strcmp(szSegment,"XXX")!=0)
				{
					strncpy( szKVal, pS2, sizeof(szKVal));
					szKVal[sizeof(szKVal)-1]=0;
					while ( TRUE )
					{
						pS2 = strchr( szKVal, ch);
						if (pS2!=NULL) pS2[0]=0;
						if (ch==0) break;
						if (ch==m_cK1) ch=0;
						if (ch==m_cK2) ch=m_cK1;
						if (ch==m_cK3) ch=m_cK2;
					}
					pS2 = strchr( szKVal, 13 );
					if (pS2!=NULL) pS2[0]=0;
					pS2 = strchr( szKVal, 10 );
					if (pS2!=NULL) pS2[0]=0;
				}
				else
				{
					GetOneValue( szKName, szKVal, sizeof(szKVal));
				}

				// szKVal contains the value, need format processing
				if ( szType[0]>='A' || szType[1]>='A' )
				{
					// Get the value and put into the buffer
					if (!FormatProcess( szResult, szKVal, szKName, 
						(szType[0]>='A'?szType:szType+1), 
						(szSubType[0]>='A'?szSubType:szSubType+1), szLen, szPos ))
					{
						// m_nKeys = 0; 
						// InitValues( true, false );
						ClearCurVars( );
						bEnd = true; break;
					}
					else
					{
						// Check whether to discard this item
						if ( szResult[0]=='~' )
						{
							GetOneValue( szKName, szOld, sizeof(szOld));
							if ( strncmp(szOld, szResult+1, strlen(szResult+1))==0 ) 
							{
								m_nKeys = 0; bEnd = true; break;
							}
						}
					}
				}
				else if (strcmp(szSegment,"XXX")!=0) // default HL7 (pos,len) formating
				{
					szResult[0]=0;
					ch = szKVal[0];
					if ( ch=='"' || ch=='\'' )
					{
						ts = szKVal+1;
						while ( ts[0] && ts[0]!=ch ) { ts[-1]=ts[0]; ++ts; }
						ts[-1]=0;
					}

					if ( szKVal[0] )
					{
						if ( nPos>0 && nPos>=(int)strlen(szKVal) )
						{
							szResult[0]=0;
						}
						else
						{
							if ( nPos>0 ) pS2 = (LPTSTR)szKVal+nPos; 
							else pS2=(LPTSTR)szKVal;
							if ( nLen>0 ) pS2[nLen]=0;
							strncpy( szResult, pS2, sizeof(szResult)-1 );
							szResult[sizeof(szResult)-1]=0;
						}
					}
				}
				else
					strcpy( szResult, szKVal );

				if ( strcmp( szKName, "TextItem" )==0 )
				{
					if ( fp == NULL )
					{
						szTempFN[0]=0;
						GetOneValue( "OutputFN", szTempFN, sizeof(szTempFN));
						if ( szTempFN[0] )
							fp = fopen( szTempFN, "wb" );
						else
							fp = fopen( m_szContFile, "wb" );
					}
					fputs( szResult, fp );
				}
				else
				{	// regular processing
					strupr( szKVal );
					if (strcmp(szSegment,"XXX")!=0) 
					{ 
						SETVALUE( szKName, szResult, szOld )
					}
					else
					{
						SETVALUEEX( szKName, szResult, szOld )
					}
				}
			}
			if ( bEnd ) break;

Loop_end:	if (!ss) break; 
			else 
			{
				if ( pS2!=NULL && pS2[0] && (nSegT==0) && 
					 ( stricmp(szKName,"RptBody") && stricmp(szKName,"RptLine") ) ) 
					 SetOneValue( (LPTSTR)szKName, "/" );
				pos = pS1 + strlen( szSegment );
			}
		  }	// while ((pS1 = strstr( pos, tmpSegment)) !=NULL ) 
		} // if ( nn==8 && CheckHL7Event((LPTSTR)m_szBuf, szEvent) )
		else
			 SetOneValue( (LPTSTR)szKName, "" );

		if ( bEnd ) break;
		if ( strncmp(pCur,"#[Ext]",6)==0 ) break;
	  } // while ( pCur[0] ) 
	  
	  if ( bEnd || !bTItem ) break;  // None TextItem case
	  if ( fp!=NULL ) fputs( "\r\n", fp );
	  if ( pNv==NULL ) break; else pSv = pNv+2;
	  if (pSv[0]==0) break;
	}

	// Adjust the Name Format for certain unstandard HL7 Interface
	if (strstr( pszTmplt, "NoAdjust" )==NULL)
	{
		AdjustUnstName( "AtndDrLast", "AtndDrFirst" );
		AdjustUnstName( "AdmtDrLast", "AdmtDrFirst" );
		AdjustUnstName( "RefrDrLast", "RefrDrFirst" );
	}
	if ( fp!=NULL ) fclose( fp );
	return (m_nKeys>0);
}


/****************************** TFL Format Template  **********************************

{<GrpName> <Class> <GDelmt> <IdStr>...|<FldsName> <StartPos> <Len> <Type> <SubType> <Delimitor>|...}

<Class> ::= [D|F|P|B]
	D - Delimited, sepeated by a delimitor
	F - Fix Length
	P - Prefix defined String (such as xxx=aaa)
	B - DBF Format
	L - LV Format, (L)(V)....
	S - Special Variables Assignment

<IdStr> ::= [[+|-]C|[+|-]N|[+|-]S|[+|-]<s>|%]:<nnn>|@<s>|#<s>|*<Expr>|ANY|ALL
	+C - Must be a letter at <nnn>
	-C - Must not be a letter at <nnn>
	+N - Must be a digit at <nnn>
	-N - Must not be a digit at <nnn>
	+S - Must be a space at <nnn>
	-S - Must not be a space at <nnn>
	%  - Must be the <nnn>th record within this file (or w/ @<s>, <s>'s <nnn>th appears 
	<s> - Must be a string <s> at <nnn>
	^<s> - Must not be a string <s> at <nnn>
	@<s> - Search <s> to position the start of this record
	#<s> - same as @, except for cancelling this record if not find <s>
	*<
	ANY|ALL - not checking any thing

<GDelmt> ::= [#|$][<ExtStr>|*SngRec][(<nn>)]
	*SngRec - Entire file to be a single record 
	Rule: B -> By DBF Default Length
		  F and atoi(IdStr)>0 -> Get a fixed length	of atoi(IdStr)
		  Other -> by <ExtStr> <ExtStr> is regular string with (\n\t\r\s)
				   or <ExtStr>[n], count <ExtStr> n times.

	# means this group will be used sequentially followed the previous one 
	$ means this group will be repeatly used after the following group
	<nn> means this group should be used to next <nn> records
	<nn>=='valid' means this group to be applied to the record until the Validation failed
    <nn>=='skip' means to skip certain this record without any processing
	<nn>==9999 means this group to be applied to the record until the Validation failed


<Type> ::= [M|T|S|N|D|E|C|U|V|H]
	T - Trimed String,
	S - Non-Trimed String,
		<SubType>: C-Capitalize, D-Digital Only, M-Digits until non-digits, 
			N-Null/Empty to Error, L-Length in 1st Byte, T-Take of first non-digits, 
		<Length>: (n,m) - substring, if length>=4(nnmm), nn is n, mm is m 
	N - Numerical,
	I - Integer (Binary expression), 
	    <SubType>: #Byte=1,2,4,8;
	F - Float (Binary expression),
		<SubType>: #Byte=4,8
	M - Name Format,
		<SubType>: L - Last, F - First, M - Middle, T - Title,
	L - Name Format, same as M, except for (L,V) String,	
	D - Date & Time
		<SubType>: T - DBF Time, filter ':', D - dd/mm/yy, C - COleDateTime
	C - The 2nd levels Delimitor 
		<SubType> is the delimitor(s) (at most 3, s for <space>) 
		  and <Length> is the 2nd Position.
	U - Unique Numbers with the length of <Length> (using Auxiliary File <FldsName>.UNO)
	V - Constant String given by <StartPos>
		<SubType> C - Current D/T
	H - Use a mapping list (<FldsName>.MAP - Format  <NewValue>|<OrgValue><\n> ...)


In the case of P class
	<StartPos> gives the Prefix (such as 'XXX=')
	<Delimitor> gives the ending sign

In the case of B class
	<StartPos> gives the DBF Fields Name (such as 'XXX')
	<Len> is sub position within a field

In the case of F class
	<GDelmt>=<nnn> means to read record with length of <nnn>
	<StartPos> can be <nn> ( meanings <nn>th char position ) or
		<nn>,<mm> or <nn>-<mm>, means <nn>th row and <mm>th column


{<GrpName> <Class> <GDelmt> <IdStr>...|<FldsName> <StartPos> <Len> <Type> <SubType> <Delimitor>|...}


When KeyName start with '#', the Key Value will be put into this field only when this
field is empty (inside control).

For the values, if they start with #, not update this field outside, 
(outside control). If it start with '~', cancel this item if the KeyValue
matches the following string (inside).

We are going to use Control Field to hold ExtraStmt for certain operations

Two Extra Operation have been defined
#DBF_Rtvl ...
#Add_Multiple ...

// This function to provide extra means to manipualate current record
// Implemented Statements:
// 1) #DBF_Rtvl [<Field1,Field2,...,Fieldn>|All] From <TableName> By <KeyField>
// 2) #Add_Multiple <Field_s> To <Field_m>
//  

When Control Field contain such data, it needs to be appended to Template field
start with #[Ext]
  
*************************************************************************************/

#define		DBFSRCH(x)	for (p=0; p<m_nDBFlds; p++) \
						if ( strcmp( m_stDBFlds[p].szName, x )==0 ) \
						{	m = strlen( szTemp ); \
							n = m_stDBFlds[p].nLen; \
							strncat( szTemp, m_szBuf+m_stDBFlds[p].nPos, n ); \
							szTemp[m+n] = 0; m+=n; \
							while (m>0 && szTemp[m-1]==' ') szTemp[--m]=0; break;  \
						}


// <KeyName> <StartPos> <Length> <Type> <SubType> <Delimitor>  
BOOL CPatParse::ParseTFLTmplt( LPTSTR pszTmplt )
{
	int i, k, p, n, m, nRes;
	TFLITEM sTflItem;
	char szDmlt[64], szTemp[1200], szResult[1200], szOld[1200], szTempFN[200], *ps, *ts, ch, ch1;
	//char szLast[25], szFirst[25], szMid[25], szTitle[10], szTmpF[50];
	//FILE *fp;
	BOOL bVals,bs=false;
	FILE *fp;

	//InitValues( true, false );
	memset(szVarNameSave, 0, sizeof(szVarNameSave));

	bVals = false;
	fp = NULL;

	i=0;
	while ( i<m_nGroup )
	{
		//if ((nRes=ValidateGroup(m_stGroup[i].IdStr, m_szBuf))>0) 
		if ( m_stGroup[i].nEffect &&
			(nRes=ValidateGroup(m_stGroup[i].IdStr, m_szBuf, m_nRecF, m_StrList))>0 ) 
		{
			bVals = true;
			for (k=0; k<m_stGroup[i].nFields; k++)
			{
				sTflItem = m_stGroup[i].Fields[k];
				if ( stricmp( sTflItem.KeyName, "RptBody" ) == 0 )
				{
					// Output the text file
					if ( fp == NULL )
					{
						szTempFN[0]=0;
						GetOneValue( "OutputFN", szTempFN, sizeof(szTempFN));
						if ( szTempFN[0] )
							fp = fopen( szTempFN, "a+b" );
						else
							fp = fopen( m_szContFile, "a+b" );
					}
					if ( fp!=NULL )
					{
						fseek( fp, 0L, SEEK_END );
						fwrite( m_szBuf, 1, strlen(m_szBuf), fp );
					}
				}
				else if ( strcmp( sTflItem.KeyName, "StdINIFmt" ) == 0 )
				{
					// Standard INI File format with Group name 
					ParseBlockVal( m_szBuf, NULL, sTflItem.Delimitor, TRUE );
				}
				else if ( strncmp( sTflItem.KeyName, "AllList", 7 ) == 0 )
				{
					// Directly take field's name for certain type of format
					if (m_stGroup[i].Class[0]=='P')  // Prefix defined Field
					{	
						ParseBlockVal( m_szBuf, NULL, sTflItem.Delimitor );
					}
					else if (m_stGroup[i].Class[0]=='B' || m_stGroup[i].Class[0]=='b')  // DBF Formated
					{
						for (p=0; p<m_nDBFlds; p++)
						{
							strcpy( szTemp, m_stDBFlds[p].szName );
							n = (m_stDBFlds[p].nLen & 0x0FFFF); 
							if ( sizeof(szResult)-1<n ) n = sizeof(szResult)-1;
							memcpy( szResult, m_szBuf+m_stDBFlds[p].nPos, n ); 
							szResult[n]=0;
							while (n>0 && szResult[n-1]==' ') szResult[--n]=0;
							if ( szResult[0] )
							{
								SetOneValueEx( szTemp, szResult);
							}
						}
					}
				}
				else // Individual Process
				{

					szTemp[0]=0;
					if (m_stGroup[i].Class[0]=='S')	 // Special Variables Assmt
					{
						GetOneValue( sTflItem.StartPos, szTemp, sizeof( szTemp ) );
					}
					else if (m_stGroup[i].Class[0]=='L')	 // LV Format Record
					{
						// StartPos will be n.m -> SubSegment(n), position(m)
						GetFixedPosFlds(szTemp, m_szBuf, sTflItem.StartPos, atoi(sTflItem.Length), true);
					}
					else if (m_stGroup[i].Class[0]=='F')	 // Fixed Length/Position 
					{	
						// Fixed Position 
						GetFixedPosFlds(szTemp, m_szBuf, sTflItem.StartPos, atoi(sTflItem.Length));
						// memcpy( szTemp, m_szBuf+atoi(sTflItem.StartPos), atoi(sTflItem.Length) );
						// szTemp[atoi(sTflItem.Length)]=0;
					}
					else if (m_stGroup[i].Class[0]=='D') // Delimited Fields 
					{	
						n = atoi(sTflItem.StartPos);
						ps = sTflItem.Delimitor; ts=szDmlt;
						while (ps[0])
						{
							TCHEXPAND(ps, ts);
						}
						if (ts[-1]=='*') { bs = true; ts[-1]=0; } else ts[0]=0;
						if (szDmlt[0]==0) strcpy( szDmlt, "," );
						m = strlen( szDmlt );

						ps=m_szBuf;
						if (bs) { while (ps[0]==' ') ++ps; }
						for (p=0; p<n; p++)
						{
							if (ps[0]=='"'||ps[0]=='\'')
							{
								ch1=ps[0]; ++ps; 
								while (ps[0] && ps[0]!=ch1) ++ps;
								++ps;
							}

							while ( ps[0] && strncmp(ps, szDmlt, m)!=0 ) ++ps;
							if (!ps[0]) break;
							ps += m; if (bs) { while (ps[0]==' ') ++ps; }
							if (!ps[0]) break;
						}

						if (ps[0])
						{
							p=0;
							if (ps[0]=='"'||ps[0]=='\'')
							{
								ch1=ps[0]; ++ps; 
								while (ps[0] && ps[0]!=ch1 && p<100)
									szTemp[p++]=(ps++)[0]; 
								szTemp[p]=0;
							}
							else
							{
								while (ps[0] && (strncmp(ps, szDmlt, m)!=0) && p<100)
									szTemp[p++]=(ps++)[0]; 
								szTemp[p]=0;
							}
						}
						else
							szTemp[0]=0;
					}
					else if (m_stGroup[i].Class[0]=='P')  // Prefix defined Field
					{	// Get Delimitor
						ps = sTflItem.Delimitor; ts=szTemp;
						TCHEXPAND(ps, ts);
						ch = szTemp[0];
						if (ch==0) ch=',';
					
						ps=SrchByScanFmt( m_szBuf, sTflItem.StartPos, 1 );
						// ps = strstr( m_szBuf, sTflItem.StartPos );
						if (ps==NULL) szTemp[0]=0;
						else
						{
							// ps += strlen(sTflItem.StartPos);
							p=0;
							if (ps[0]=='"'||ps[0]=='\'')
							{
								ch1=ps[0]; ++ps;
								while (ps[0] && ps[0]!=ch1 && p<100)  
									szTemp[p++]=(ps++)[0]; 
								szTemp[p]=0;
							}
							else
							{
								while (ps[0] && ps[0]!=ch && p<100)
									szTemp[p++]=(ps++)[0]; 
								szTemp[p]=0;
							}
						}
					}
					else if (m_stGroup[i].Class[0]=='B' || m_stGroup[i].Class[0]=='b')  // DBF Formated
					{
						strupr( sTflItem.StartPos );
						DBFSRCH( sTflItem.StartPos );

						// Looking for a special embeding fields in NetMed
						// ShortDesc -- */   and SupDesc -- *~
						if ( !szTemp[0] && ( strcmp( sTflItem.StartPos, "SHORTDESC" )==0 ||
											 strcmp( sTflItem.StartPos, "SUPDESC" )==0   ))
						{
							DBFSRCH( "COMMENTS" );
							DBFSRCH( "COMMENTS2" );
							DBFSRCH( "COMMENTS3" );
							ps = NULL;
							if (strcmp( sTflItem.StartPos, "SHORTDESC" )==0)
								ps = strstr( szTemp, "*/" );
							else if (strcmp( sTflItem.StartPos, "SUPDESC" )==0)
								ps = strstr( szTemp, "*~" );
							if (ps==NULL) szTemp[0]=0;
							else 
							{
								n = strlen( ps+2 );
								strncpy( szTemp, ps+2, n);
								szTemp[n]=0;
								ps = strstr( szTemp, "*/" ); if (ps!=NULL) ps[0]=0;
								ps = strstr( szTemp, "*~" ); if (ps!=NULL) ps[0]=0;
							}
						}
						else if ( szTemp[0] && (strncmp( sTflItem.StartPos, "COMMENTS", 8 )==0) )
						{
							ps = strstr( szTemp, "*/" ); 
							if (ps!=NULL) ps[0]=0;
							ps = strstr( szTemp, "*~" ); 
							if (ps!=NULL) ps[0]=0;
						}
					}
				
					//----- Always create a variable even it is empty
					//if ( szTemp[0] || sTflItem.Type[0]=='I' || 
					//	 sTflItem.Type[0]=='U' || sTflItem.Type[0]=='V' )
					//{

						// Get the value and put into the buffer
						if (!FormatProcess( szResult, szTemp, sTflItem.KeyName, 
								sTflItem.Type, sTflItem.SubType, 
								(m_stGroup[i].Class[0]=='F'?_T("0"):sTflItem.Length), 
								( ((m_stGroup[i].Class[0]=='F' || m_stGroup[i].Class[0]=='D' ||
								    m_stGroup[i].Class[0]=='L' ) &&
								   (sTflItem.Type[0]!='V') && (sTflItem.Type[0]!='W') )
								  ?_T("0"):sTflItem.StartPos) ))
						{
							// m_nKeys = 0;
							// InitValues( true, false );
							ClearCurVars( );
							bVals = false; break;
						}
						else
						{
							// Check whether to discard this item
							if ( szResult[0]=='~' )
							{
								GetOneValue( sTflItem.KeyName, szOld, sizeof(szOld));
								if ( strncmp(szOld, szResult+1, strlen(szResult+1))==0 ) 
								{
									// m_nKeys = 0; 
									// InitValues( true, false );
									ClearCurVars( );
									bVals = false; break;
								}
							}
							else if ( sTflItem.Type[0]!='I' || strcmp("0", szResult )!=0 )
							{
								// check if KeyName start with '#', check its existence
								SETVALUE( sTflItem.KeyName, szResult, szOld )
							}
						}

					//}	// szTemp[0] || sTflItem.Type[0]=='U' || sTflItem.Type[0]=='V'
				}	// if ( stricmp( sTflItem.KeyName, "RptBody" ) == 0 )
			}	// for (k=0; k<m_stGroup[i].nFields; k++)
			if ( !bVals ) break;
		}	// if ((nRes=ValidateGroup(m_stGroup[i].IdStr, m_szBuf))>0)

		/* else if (nRes<0) // One failure among multiple rules, keep doing other 
		{
			bVals = false;
			break;
		} 
		*/
		i++;
	}	// while ( i<m_nGroup )

	if (fp!=NULL) fclose(fp);
	return bVals; 
}


#define		GETCURSTR( L, s, x, y, n )	p=0; n=2; \
	while (ps[0]) { lch=(ps++)[0];if(x && strchr((CHAR*)x,lch)) {n=1; break;}\
		if(strchr((CHAR*)y,lch)){n=2;break;} \
		if(lch=='"'||lch=='\'') {while(ps[0] && p<=L && ps[0]!=lch) s[p++]=(ps++)[0];++ps;}\
		else s[p++]=lch; if(p>L){n=0;break;} }\
		s[p]=0;
 
#define		GETCURST2( L, s, x, y, n )	p=0; n=2; \
	while (ps[0]) { lch=(ps++)[0];if(x && strchr((CHAR*)x,lch)) {n=1; break;}\
		if(strchr((CHAR*)y,lch)){n=2;break;} \
		s[p++]=lch; if(p>L){n=0;break;} }\
		s[p]=0;
 
//
// Set multiple variables by parsing a given data block
// Format [<KeyName><Separator><KeyVal><Delimitor>]......
// <Separator> and <Delimitor> can be one of the multiple choices
// Default Separator:  ':' or '='
// Default Delimitor:  '\r', \n', ',', '^', 
// Separator contains '%' means ignore ' and ".
// KeyName must be shorten than 32 chars.
// bGroup indicates to use Group Name as Prefix of Key name
//
void CPatParse::ParseBlockVal( LPTSTR pszBuffer, LPTSTR pzSeparator, LPTSTR pzDelimitor, BOOL bGroup, BOOL bGlobal )
{
	TCHAR lch, *ps, *ts, *qs;
	TCHAR szPrfx[40]={0}, szKey[64], szTemp[1024], szVal[1024], szSepr[64], szDlmt[64];
	int n, p, k;
	bool bIGN = false ;

	if ( pzSeparator!=NULL )
	{
		ts=szSepr; qs=pzSeparator;
		while ( qs[0] )
		{
			TCHEXPAND( qs, ts );
		}
		ts[0]=0;
	}
	else
		strcpy( szSepr, ":=" );

	if ( pzDelimitor!=NULL )
	{
		ts=szDlmt; qs=pzDelimitor;
		bIGN = (strchr( qs, '%' )!=NULL);
		while (qs[0])
		{
			TCHEXPAND( qs, ts );
		}
		ts[0]=0;
	}
	else
		strcpy( szDlmt, ",\^\r\n" );

	ps = pszBuffer;
	while ( ps[0] )
	{
		while ( ps[0] && ps[0]<=' ' ) ++ps;
		szKey[0] = 0; szVal[0] = 0;
		
		if ( bIGN )
		{
			GETCURST2( 32, szTemp, szSepr, szDlmt, n )
		}
		else
		{
			GETCURSTR( 32, szTemp, szSepr, szDlmt, n )
		}
		
		if ( n>0 )	// within length limitor
		{
			// if keyname contains <sp>, replace it with '_'
			ts=szTemp; 
			while (ts[0]) { if (ts[0]==' ') ts[0]='_'; ++ts; }
			if ( n==1 ) // Separator
			{
				if ( bGlobal )
					strcpy( szKey, "Global." );
				else
					szKey[0] = 0;

				if ( bGroup )
				{
					strcat( szKey, szPrfx );
					strcat( szKey, "_" );
					strcat( szKey, szTemp );
				}
				else
					strcat(szKey, szTemp);

				szVal[0] = 0;
				if ( bIGN )
				{
					GETCURST2( 1023, szTemp, NULL, szDlmt, n )
				}
				else
				{
					GETCURSTR( 1023, szTemp, NULL, szDlmt, n )
				}
				if ( n==0 ) break;
				strcpy(szVal, szTemp);
				if ( lch==',' )
				{
					// multiple ',' situation
					ts = ps; k=0;
					while ( ps[0] )
					{
						if ( bIGN )
						{
							GETCURST2( 1023, szTemp, szSepr, szDlmt, n )
						}
						else
						{
							GETCURSTR( 1023, szTemp, szSepr, szDlmt, n )
						}
						if (n==2) 
						{
							k++; ts=ps; 
							strcat(szVal, "," ); strcat(szVal,szTemp);
						}
						else
							break;
					}
					ps = ts;
				}
				if ( szKey[0] )
				{
					if (strncmp(szVal,"*&*",3)==0) ts=szVal+3; else ts=szVal;
					SetOneValueEx( szKey, ts );
				}
			}
			else if ( bGroup ) // check the Group Name
			{
				--ts; // ts has been used before
				if ( szTemp[0]=='[' && ts[0]==']' )
				{
					ts[0]=0; 
					strncpy( szPrfx, szTemp+1, sizeof(szPrfx)-1 );
					szPrfx[sizeof(szPrfx)-1]=0;
				}
			}
		}
	}


}



// This function to provide extra means to manipualate current record
// Implemented Statements:
// 1) #DBF_Rtvl [<Field1,Field2,...,Fieldn>|All] From <TableName> By <KeyField>
// 2) #Add_Multiple <Field_s> To <Field_m>
// 3) #[Ext] PatMasControl{ }, PatVisitControl{ }, DocMasControl{ }, DeleteControl{  }  
void CPatParse::ExtraManipulate( LPCTSTR pszOperStmt )
{
	char szTemp[200];
	char szParam[5][200];
	char *pos;

	pos = (LPTSTR)pszOperStmt;
	while ( pos[0] )
	{
		while (pos[0] && pos[0]!='#') ++pos;
		if (pos[0]==0) break;
		++pos;
		if (strncmp(pos, "DBF_Rtvl", 8)==0)
		{
			pos += 9;
			sscanf( pos, "%s From %s By %s", szParam[0], szParam[1], szParam[2] );
			if (OpenDBFWrite( szParam[1] ))
			{
				if (GetOneValue( szParam[2], szTemp, 200 ))
					RtvlOneRecord( szParam[2], szTemp, szParam[0] );
				CloseDBFWrite();
			}
		}
		else if (strncmp(pos, "Add_Multiple", 12)==0)
		{
			pos += 12;
			sscanf( pos, "%s to %s", szParam[0], szParam[1]);
			GetOneValue( szParam[0], szTemp, sizeof(szTemp));
			SetOneValue( szParam[1], szTemp );
			SetOneValue( szParam[1], "," );
		}
		else
			++pos;
	}
}


// Same function as ValidateSegment, except for Expression evaluation
// Criteria -> [[[+|-][C|D|S]|%|[^]<String>]:<nnn>|@<String>|#<String>]...
// *<expression>
int CPatParse::ValidateGroup( char* pCriteria, char* pData, int nIdx, void* pList )
{
	char szExpr[256], szRes[128]={0};

	if ( pCriteria[0]=='*' )
	{
		strcpy(szExpr, "{*");
		strcat(szExpr, pCriteria+1);
		strcat(szExpr, "*}");
		SetAllText( szExpr, szRes, 128 );
		if ( strcmp(szRes,"TRUE")==0 ) return 1; else return 0;
	}
	else
		return ValidateSegment( pCriteria, pData, nIdx, pList); 
}



// In the Template, <*RptBody~> - Entire Report Body, ~ -> LF
//                  <*RptLine> - Single Report Line,
//                  <*LineNo>  - Line Number
//					<*CurDateTime> - Current Date Time
//					<*TextField|<Definition>> - Text related field, see <definition>
//					<Definition> - [<Text_i>^<Name_i>|...] take Name_i as the value
//						if Text_i appear in the text lines. used w/ RptLine
//						<Text_i> can be 'Default:', <Name_i> can be 'Takeoff'
//					<*xxxx|<Format>> - Present xxxx with <Format>
//						<Format> - D:<nnn> - numerical w/ print format <nnn>
//							 - S:<nnn> - string w/ printf format <nnn>
//							 - C:<nnn> - string from scanf w/ format <nnn>
//							 - T:<mmm> - Date&Time w/ CTime format <mmm>
//								 yyyymmddHHMM and its combination
//							 - M:<m>  - Name Format, <m> is sub-format
//									F: First, L: Last, M: Middle, T: Title
//									S: Last, M.D.,     P: First M.
//							 - W:<sss>?<TV>:<FV>  - Testing the value with bit value
//									<sss>, if its true, output <TV>, otherwise <FV>  
//							 - H:[#]<K1>-<V1>^<K2>-<V2>^...  - Select value by the Keyi
//									if value is Ki return Vi, if Ki has suffix ~ or @,
// 									it tests its value whether > Ki or < Ki;
//									if prefix # exists, it use numerical value for comparison; 
//						Multiple <Format> can be used if they are seperated by | 
// [* <Statement> *] - Repeat <Statement> for each lines in the report.
//					Within [*...*], multiple <*RptLIne> can be appear with
//					the separation of <> (which indicates skipping to next line) 
//
// {* <Expression> *} -  Evaluation of an expression
//
// {*<Expression>?<Tmplt_1>:(or ~)<Tmplt_2>*} - Evaluation of <Expression>,
//					if true, use <Tmplt_1>, otherwise, use <Tmplt_2>.
// 
// <*Check_Variable>...<*EndCheck> - define a paragraph of template appearing only if <*Variable>
//					is not NULL.
//
// SetAllText support the HL7 segments, 
//    Return Value:  NULL (no more segment) or 
//					 pszNext (not NULL) will point to the start of next segment
//    nCriteria (!=0) is limitation: # of lines (<1000) or # of bytes (>1000)
//    In the segments situation, three variables can be used
//    <*CurSegmentNo>, <*PrevSegmentNo> and <*NextSegmentNo>
//
//	  <*Find_Head_#> will search for 'Head ' in the text file, and take its follows
//    (until <CR>) as the value.
//    <*Flds_Delimitor> also gives the customized delimitor
// 
// Multiple Choices: ?<Key1>,<Key2>,...,<KeyN>
// if <Key(i)> is NULL, then use <Key(i+1)>, et al
//      
LPTSTR CPatParse::SetAllText( LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, 
							  LPTSTR pszBody,  int nCriteria )
{
	CHAR szKTmp[16000]={0}, szVTmp[16000]={0}, szTmp[16000]={0}, szOrgLine[16000]={0}, szLine[16000]={0}, ch;
	CHAR *ps, szFormat[1200]={0}, szFmtTpl[1200]={0}, szTmpF[200]={0}, szLE[10]={0};
	//CHAR szBitv[240], szLast[40], szFirst[40], szMid[40], szTitle[40], szCode[20];
	CHAR szSection[40]={0}, szSectSv[40]={0}, szSecRule[250]={0};
	CHAR szTVal[2048]={0}, szFVal[2048]={0};
	CHAR *ts, *qs, szEachK[240];
	LPTSTR pS, pT, pB, pBSv, pSave, pszNext;
	int nLineNo = 0, nLineNoS = 0, nLineNoT = 0, nPageNo = 0, nPageSize = 0, nn = 0, nLoop = 0, i, j, mm, nL, lcs;
	CTime t1;
	BOOL sRes, ss, bDisc = true, bBin = false, bExt = false;
	int nTmp;
	LPTSTR pzTmpBuf, pzResBuf;


	// Check Recordset Situation
	if ( strncmp( pszTmplt, "RECORDSET", 9 )==0 || strncmp( pszTmplt+1, "RECORDSET", 9 )==0)
	{
		SetOneRecord( pszBuf, nLen, (strncmp(pszTmplt+10,"XML",3)==0?1:0));
		return (LPTSTR)(strlen(pszBuf));
	}
	
	if (strncmp( pszTmplt, "STDRNGEXPR(", 11 )==0 || strncmp( pszTmplt+1, "STDRNGEXPR(", 11 )==0 )
	{
		sscanf( pszTmplt+11,"%[^)]", szTmpF );
		ps = strchr( pszTmplt, ':' );
		if ( ps!=NULL ) 
		{
			++ps;
			StdRngExprPrs( ps, szTmpF, STD_RNGE_GNRT, pszBuf );
		}
		return (LPTSTR)(strlen(pszBuf));
	}

	if ((ps=strstr( pszTmplt, "COMMENTS(*"))!=NULL)
	{
		ps += 10;
		ts = strstr(ps,"*)");
		if ( ts!=NULL )
			return SetAllText( ts+2, pszBuf, nLen, pszBody, nCriteria );
	}

	if ((ps=strstr( pszTmplt, "VARSCONV(*"))!=NULL)
	{
		ps += 10;
		ts = strstr(ps,"*)");
		if ( ts!=NULL )
		{
			pSave = ts+2;
			nTmp = (ts-ps);
			pzTmpBuf = (LPTSTR) new TCHAR[nTmp+20];
			pzResBuf = (LPTSTR) new TCHAR[nTmp+2000];
			if ( pzTmpBuf!=NULL && pzResBuf!=NULL )
			{
				strncpy( pzTmpBuf, ps, nTmp );
				pzTmpBuf[nTmp] = 0;
				pzResBuf[0] = 0;
				SetAllText( pzTmpBuf, pzResBuf, nTmp+2000 );
				ParseBlockVal( pzResBuf );
				Sleep( 500 );
				delete pzTmpBuf;
				delete pzResBuf;
			}
			return SetAllText( pSave, pszBuf, nLen, pszBody, nCriteria );
		}
	}

	// Get CC(or Other) Extraction List
	CCITEM m_pCCItem[20];
	int nItem = 0;
	LPTSTR pCur, pLast;

	pCur = pszTmplt;
	pCur = strstr( pCur, "<*Find_" );
	while ( pCur!=NULL )
	{
		pLast = strchr(pCur,'>');
		if (pLast!=NULL)
		{
			strncpy( szVTmp, pCur+2, pLast-pCur-2 );
			szVTmp[pLast-pCur-2]=0;
			pLast = strchr(szVTmp,'|');
			if (pLast!=NULL) pLast[0]=0;
			ss = false;
			for (i=0; i<nItem; i++)
				ss = ss || (strcmp(m_pCCItem[i].szName, szVTmp)==0);
			if (!ss)
			{
				szKTmp[0]=0;
				sscanf( szVTmp+5,"%[^_]", szKTmp );
				// strcat( szKTmp, " " );
				m_pCCItem[nItem].nStatus = 0;
				strncpy(m_pCCItem[nItem].szSrch, szKTmp, sizeof(m_pCCItem[nItem].szSrch)-1);
				strncpy(m_pCCItem[nItem].szName, szVTmp, sizeof(m_pCCItem[nItem].szName)-1);
				m_pCCItem[nItem].szValue[0]=0;
				++nItem;
			}
		}
		pCur = strstr( pCur+7, "<*Find_" );
	}

	// Search the report Body if necessary and put into the buffer
	if ( nItem>0 && pszBody!=NULL )
	{
		szTmp[0] = 0;
		GetOneValue( "Flds_Delimitor", szTmp, 32 );
		SearchTextForCC( pszBody, m_pCCItem, nItem, (szTmp[0]?szTmp:NULL));
		for (i=0; i<nItem; i++)
			if (m_pCCItem[i].nStatus)
				SetOneValueEx( m_pCCItem[i].szName, m_pCCItem[i].szValue );
	}

	// General Initialization
	t1=CTime::GetCurrentTime();	  
	pS = pszTmplt;
	pT = pszBuf;
	pT[0] = 0;
	pszNext = NULL;

	// Template start with '#' to keep <SP>/<NL>/<CR>...
	if (pS[0]=='#')
	{
		bDisc=false;
		++pS;
	}
	else if (pS[0]=='$')  // Template start with '$' to use binary representation
	{
		bBin= true;
		++pS;
	}

	// Prepare for TextField
	szSection[0]=0;	szSectSv[0]=0;	szSecRule[0]=0; 
	szLine[0]=0;  
	sRes = TRUE;
	if ( pszBody == NULL ) { pB = NULL; pBSv = NULL; } 
	else  
	{
		pB = pszBody;
		sscanf(pB, "%15990[^\r\n]", szLine );
		pBSv = pB;
	}

	bool bChkNextLine=false;
	int k, nLev, nReptFd=0, nSegSplit=0;
	LPTSTR pS1;

	while ( pS[0] && pT-pszBuf<nLen ) 
	{
		if ( (pS[0]=='[' && pS[1]=='*' || 
			  pS[0]=='<' && pS[1]=='>' ) /*&& pB !=NULL*/)
		{	
			// Report Segment enable
			if ( strstr( pS, "<*RptSegt" )!=NULL || strstr( pS, "<*rptsegt" )!=NULL || strstr( pS, "<*RPTSEGT" )!=NULL )
			{
				nSegSplit = 1; pSave = pS; pS+=2;
				nLineNoS++;
				if ( pB[0] ) nLoop = 1; else nLoop = 0;
			}
			else if ( strstr( pS, "<*Rept_" )!=NULL  )
			{
				pSave = pS; pS+=2; nLineNoS++;
				nReptFd = 1; nLoop = 0;
			}
			else
			{
				if ( pS[0]=='[' )
				{
					nLoop = 1; pSave = pS;
				}
				pS+=2; 

				strcpy( szSectSv, szSection );
			
				szOrgLine[0] = 0;
				szLine[0] = 0;
				ch = 0;
				if ( pB==NULL || pB[0]==0 ) nLoop = 0;
				else
				{
					if ( pB[0]=='\n' || pB[0]=='\r' || pB[0]=='\x1a' ) { ch=pB[0]; }
					else
						sscanf( pB, "%15990[^\n\r\x0a\x1a]%c", szOrgLine, &ch ); 
				
					szLine[0]=0;  
					strncpy( szLine, szOrgLine, sizeof(szLine)-1 );
					pB += (strlen(szLine)+(ch?1:0));
					nLineNoS++;
				
					if ( szSecRule[0] )  // Looking for Section Name
						ParseSectionByRule( szSecRule, szSection, szLine, nLineNoS );
			
					if ( pB[0]=='\r' || pB[0]=='\n' || pB[0]=='\x0a' || pB[0]=='\x1a' ) ++pB;
				}
			}
			// if ( ch==0 ) nLoop = 0;
		}
		else if ( pS[0]=='*' && pS[1]==']' /*&& pB!=NULL*/ ) 
		{
			if ( nReptFd>0 )
			{
				if ( nReptFd == 2 ) { pS += 2; nLineNoS=0; nLineNo=0; nReptFd=0; } 
				else pS = pSave;
				continue;
			}

			if ( nCriteria>0 && nCriteria<1000  && nSegSplit==0 && pB!=NULL )
			{
				if ( ++nn > nCriteria )
				{
					nLoop = 0;
					pszNext = pB;
				}
			}

			//
			if ( nLoop )
			{
				if ( pB==NULL || pB[0]==0 )
				{
					nLoop=0; 
					szSection[0]=0;
					if ( (nPageSize>0) && (nLineNoT>0) && ( nLineNoT % nPageSize < nPageSize-1 ))
						pS = pSave;
					else
						pS += 2; 
				}
				else
					pS = pSave; 
			}
			else
			{
				if ( (nPageSize>0) && (nLineNoT>0) && ( nLineNoT % nPageSize < nPageSize-1 ) )
					 // && ( pB!=NULL && pB[0] ) )
					pS = pSave;
				else
					pS += 2;
				
			}
		}
		else if ( pS[0]=='{' && pS[1]=='*' ) // Start of an expression
		{
			nLev=0; pS1=pS; pS +=2; 
			k=1; szTVal[0]='#';
			while ( pS[0] && (nLev>0 || pS[0]!='*' ||  pS[1]!='}' ) ) 
			{
				szTVal[k++]=pS[0];
				if ( pS[0]=='{' && pS[1]=='*' ) nLev++;
				if ( pS[0]=='*' && pS[1]=='}' ) nLev--;
				++pS;
			}
			if (pS[0]) pS += 2;  
			szTVal[k]=0; szFVal[0]=0; szVTmp[0]=0;
			SetAllText(szTVal, szFVal, sizeof(szFVal), (pszBody!=NULL?szLine:NULL));
			if ( ExtendEvaluate( szFVal, szVTmp ) )
			{
				strcpy( pT, szVTmp );
				pT = pT + strlen(szVTmp);
			}
		}
		else if ( strcmp(szSection, "Takeoff") == 0 && nLoop )
		{
			// Do nothing for a takeoff line
			++pS;
		}
		else if ( pS[0]=='<' && pS[1]=='*' ) 
		{

			pS1 = pS;  qs = szKTmp; pS += 2;
			pS = xsscanf( pS, qs, ">", sizeof(szKTmp)-1 );
			--pS;

			if (pS[0]=='>')	 // Get a Variable Item
			{
				++pS;
				if ( strncmp(szKTmp, "Check_", 6)==0 )
				{
					szVTmp[0] = 0; szKTmp[5]='#';
					SetAllText(szKTmp+5, szVTmp, sizeof(szVTmp), (pszBody!=NULL?szLine:NULL));
					// GetOneValue( szKTmp+6, szVTmp, 250 );
					if ( szVTmp[0]==0 || strcmp( szVTmp,"FALSE" )==0 )
					{
						while ( pS[0] && strncmp(pS, "<*EndCheck>", 11)!=0 ) ++pS;
						if ( pS[0] ) pS += 11;
					}
					// Goto Loop
				}
				else if ( strncmp(szKTmp, "Septr_", 6)==0 )
				{
					mm = 1;
					if ( szKTmp[6]=='#' )
					{
						nn = atoi( szKTmp+7 );
						if (nLineNoT>0 && nn>0 && (nLineNoT%nn==0)) mm=0;
					}
					else if ( strcmp(szKTmp+6,"Segment")==0 )
					{
						if (strcmp(szSection, szSectSv)!=0 || strlen(szSection)!=strlen(szSectSv)) mm=0;
					}

					if ( mm )
					{
						while ( pS[0] && strncmp(pS, "<*EndSeptr>", 11)!=0 ) ++pS;
						if ( pS[0] ) pS += 11;
					}
					else
						nPageNo++;
					// Goto Loop
				}
				else if  ( strncmp(szKTmp, "Rept_", 5)==0 )
				{
					// Repeated by a field
					GetOneValue( szKTmp+5, szTmp, sizeof(szTmp) );
					ch=0; szVTmp[0]=0; 
					sscanf( szTmp, "%[^|,]%c", szVTmp, &ch );
					nL=strlen(szVTmp);
					
					if ( szTmp[nL+(ch?1:0)]==0 ) nReptFd = 2;
					for (i=0; i<nL; i++) (pT++)[0]=szVTmp[i]; 
					pT[0]=0;
					SetOneValue( szKTmp+5, szTmp+(nL+(ch?1:0)), 1 );
				}
				else if ( strnicmp(szKTmp,"RptSegt",7)==0 && (pB != NULL) )
				{
					nn = atoi(szKTmp+8);
					if ( strlen(pB)<nn ) nn = strlen(pB);
					szLE[0]='\n'; szLE[1]=0;

					lcs = 0;
					if ( strcmp( szKTmp, "RPTSEGT" )==0 ) lcs = 1;
					else if ( strcmp( szKTmp, "rptsegt" )==0 ) lcs = 2;
					goto Continue;
					
				}
				else if ( strnicmp(szKTmp,"RptBody",7)==0 && (pB != NULL) && 
					      (pB[0] || szLine[0]) )
				{
					szLE[0]=0;	bExt=false;
					strncpy( szLE, szKTmp+7, sizeof(szLE) );
					if (szLE[0]==0 ) strcat( szLE, "~" );
					if (strcmp(szLE,"\\n")==0 ) strcpy( szLE, "\n" );
					if (strcmp(szLE,"\\s")==0 ) strcpy( szLE, " " );
					if (strcmp(szLE,"\\e")==0 ) szLE[0]=0;
					/* \h using ~ to separate and replace |~&^ */
					if (strcmp(szLE,"\\h")==0 ) { strcpy( szLE, "~" ); bExt=true; }


					lcs = 0;
					if ( strcmp( szKTmp, "RPTBODY" )==0 ) lcs = 1;
					else if ( strcmp( szKTmp, "rptbody" )==0 ) lcs = 2;
					
					// RptBody(O) -> take orginal input file format
					if ( strcmp( szLE, "(O)" )==0 && nCriteria==0 )
					{
						nn = strlen( pB );
						strncpy( pT, pB, nn );
						pT += nn;
					}
					else if ( strcmp( szLE, "(O)" )==0 && nCriteria>0 )
					{
						nn = strlen( pB );
						mm = ( nn<nCriteria?nn:nCriteria );
						strncpy( pT, pB, mm );
						pT += mm;  pB += mm; nn -= mm;
					}
					else if ( strcmp( szLE, "(F)" )==0 )
					{
						int nTmp, nLen;
						LPTSTR pzTmp;
						FILE* fp=fopen( pB, "rb" );
						if ( fp != NULL ) 
						{
							fseek( fp, 0L, SEEK_END );
							nLen = ftell( fp );
							pzTmp = new TCHAR[nLen+100];
							if (pzTmp!=NULL)
							{
								fseek( fp, 0L, SEEK_SET );
								nTmp = fread( pzTmp, sizeof(TCHAR), nLen+1, fp );
							}
							else
								nTmp = 0;
							pzTmp[nTmp] = 0;
							fclose( fp );
							memcpy( pT, pzTmp, nTmp );
							pT += nTmp;
							delete pzTmp;
						}
					}
					else
					{
						// Check to see whether segment is required
						nn = strlen( pB );
						if ( nCriteria >= 1000 && nn >= nCriteria)
							nn = nCriteria;
Continue:						
						mm = 0;
						ss = true;
						if (strcmp(szSection, "Takeoff")==0) pB+=nn;
						else
						for ( i=0; i < nn; i++ )
						{		

							// Removal of <*...>
							if ( ss && pB[0]=='<' && pB[1]=='*' ) ss=false;
							else if ( !ss && pB[0]=='>' ) ss=true; 
							else if ( pB[0]=='\n')
							{
								++mm;
								for (j=0; (UINT)j<strlen(szLE); j++)
									(pT++)[0] = szLE[j];
							}
							else if ( bExt && ss && pB[0]=='|' ) { (pT++)[0]='\\'; (pT++)[0]='F'; (pT++)[0]='\\'; }
							else if ( bExt && ss && pB[0]=='^' ) { (pT++)[0]='\\'; (pT++)[0]='S'; (pT++)[0]='\\'; }
							else if ( bExt && ss && pB[0]=='~' ) { (pT++)[0]='\\'; (pT++)[0]='R'; (pT++)[0]='\\'; }
							else if ( bExt && ss && pB[0]=='&' ) { (pT++)[0]='\\'; (pT++)[0]='T'; (pT++)[0]='\\'; }
							else if ( bExt && ss && pB[0]=='\\' ) { (pT++)[0]='\\'; (pT++)[0]='E'; (pT++)[0]='\\'; }
							else if ( ss && (pB[0]>=' ' || pB[0]=='\t') )
								(pT++)[0] = (lcs==1?UPPER(pB[0]):(lcs==2?LOWER(pB[0]):pB[0]));
							pB++;

							if ( nCriteria>0 && nCriteria<1000 && mm>nCriteria )
								break;

							if ( szSecRule[0] &&   // Looking for Section Ending
								 FindSectionEnding( szSecRule, pB )) break;
						}
					}
					pT[0] = 0;
				}
				else if ( stricmp(szKTmp,"RptLine")==0 && (pB != NULL) && strcmp(szSection, "Takeoff")==0 )
				{
				}
				else if ( stricmp(szKTmp,"RptLine")==0 && (pB != NULL) && strncmp(szLine, "??", 2)==0 )
				{
					strcpy( szSection, "Takeoff" );
				}
				else if ( strncmp(szKTmp,"TextField", 9)==0 && pB != NULL )
				{
					if ( szKTmp[9] )
					{
						strncpy( szSecRule, szKTmp+10, sizeof(szSecRule) );
						if ( !szSection[0] )  // First time, looking for default
							ParseSectionByRule( szSecRule, szSection, szLine, nLineNoS );
					}
					if (strcmp(szSection, "Takeoff")!=0)
					{
						strncat( pT, szSection, nLen-(pT-pszBuf)-1 );
						pT+=strlen( szSection );
					}
				}
				else if ( strcmp(szKTmp,"Repeat")==0 && pB != NULL )
				{
					pB = pBSv;
					nLineNo = 0;
					nLineNoS = 0;
					nLineNoT = 0;
				}
				else if ( strncmp(szKTmp,"PageSize",8)==0 && (pB != NULL) )
				{
					nPageSize = atoi(szKTmp+9);
				}
				else
				{
					// Item Analyze, _Item_|_Format_
					ps = szKTmp; ps = xsscanf( ps, NULL, "|\\" );
					strncpy(szFormat, ps, sizeof(szFormat));  
					if ( ps[0] ) ps[-1]=0; 
					
					// Allowing recursively use variables/expressions
					memmove( szKTmp+1, szKTmp, strlen(szKTmp)+1 );
					szKTmp[0]='#';
					SetAllText(szKTmp, szVTmp, sizeof(szVTmp), (pszBody!=NULL?szLine:NULL));
					strncpy(szKTmp, szVTmp, sizeof(szKTmp));
					
					nL = 0;	szVTmp[0] = 0;
					if ( szKTmp[0]=='?' )
					{
						// Multiple Choices: ?<Key1>,<Key2>,...,<KeyN>
						// if <Key(i)> is NULL, then use <Key(i+1)>, et al
						// if <Key1> is Boolean, TRUE-><Key2>, FALSE-><Key3>
						
						ts = szKTmp+1; nn = 0;
						while ( ts[0] )
						{
							szEachK[0]=0; qs=szEachK;
							ts = xsscanf( ts, qs, ",", sizeof(szEachK)-1 );
							
							if (strcmp(szEachK,"TRUE")==0) 
							{
								szEachK[0]=0; qs=szEachK;
								ts = xsscanf( ts, qs, ",", sizeof(szEachK)-1 );
								nn = 1;
							}
							else if (strcmp(szEachK,"FALSE")==0) 
							{
								// Recursive Boolean expression
								szEachK[0]=0; qs=szEachK;
								ts = xsscanf( ts, qs, ",", sizeof(szEachK)-1 );
								nn = 1;
								continue;
							}

							nL = GetProperKeyVal( szEachK, szVTmp, sizeof(szVTmp)-1, szLine, nLineNo, nLineNoT, pB!=NULL );
							// i = strlen( szVTmp )-1;
							while ( nL >=0 && szVTmp[nL-1]==' ' ) szVTmp[--nL]=0;
							if ( nL!=0 || nn ) break;
						}
					}
					else
					{
						if ( strcmp( szKTmp, "DefSeqNo" )==0 )
						{
							sprintf( szVTmp, "%06u", m_nIndexA++ );
						}
						else if ( strcmp( szKTmp, "CurPageNo" )==0 )
						{
							sprintf( szVTmp, "%d", nPageNo );
						}
						else if ( strcmp( szKTmp, "NextPageNo" )==0 )
						{
							sprintf( szVTmp, "%d", nPageNo+1 );
						}
						else if ( strcmp( szKTmp, "PrevPageNo" )==0 )
						{
							sprintf( szVTmp, "%d", nPageNo-1 );
						}
						else if ( strcmp(szKTmp,"CurSegmentNo")==0 && pB != NULL )
						{
							sprintf( szVTmp, "%d", m_nSegment );
						}
						else if ( strcmp(szKTmp,"PrevSegmentNo")==0 && pB != NULL )
						{
							if (m_nSegment==0)
							{
								ps = NULL; szVTmp[0]=0;
							}
							else
								sprintf( szVTmp, "%d", m_nSegment-1 );
						}
						else if ( strcmp(szKTmp,"NextSegmentNo")==0 && pB != NULL )
						{
							if (pB[0])
								sprintf( szVTmp, "%d", m_nSegment+1 );
							else
							{
								ps = NULL; szVTmp[0]=0;
							}
						}
						else
							nL = GetProperKeyVal( szKTmp, szVTmp, sizeof(szVTmp)-1, szLine, nLineNo, nLineNoT, pB!=NULL );
					}

					if ( nL==0 ) nL = strlen(szVTmp);
					
					// Format Conversion (Multiple Formats) 
					if ( szFormat[0] )
					{
						strcpy(	szFmtTpl, "#" ); strcat( szFmtTpl, szFormat );
						SetAllText(szFmtTpl, szTmp, sizeof(szTmp), (pszBody!=NULL?szLine:NULL));
						strncpy(szFormat, szTmp, sizeof(szFormat));

						while ( szFormat[0] && szFormat[1]==':' )
						{
							// Look for next item
							ps = szFormat; 
							ps = xsscanf( ps, NULL, "|" );
							if ( ps[0] ) ps[-1]=0; 
					
							strcpy( szTmp, szVTmp );
							nL = FormatForExp( szVTmp, szTmp, szFormat, m_szWorkDir, szKTmp );
								    
							if ( ps[0] )
								strncpy(szFormat, ps, sizeof(szFormat));
							else
								break;
						}
					}
					for (i=0; i<nL; i++) (pT++)[0]=szVTmp[i]; 
					pT[0]=0;
				}
			}
			else  // if (pS[0]=='>')
			{	
				// '<*' without '>'
				sprintf( szVTmp, "<Format Error!>" );
				strcpy( pT, szVTmp );
				pT += strlen( szVTmp );

				pS = pS1;
				if ( pS[0]<32) ++pS;
				else
				{
					pT[0]=pS[0]; pT[1]=0; ++pT; ++pS;
				}
			}
		}
		else  // if ( pS[0]=='<' && pS[1]=='*' )
		{
			if ( (pS[0]<=32) && bDisc ) ++pS;
			else
			{
				pT[0]=pS[0]; pT[1]=0; ++pT; ++pS;
			}
		}
	};

	if ( nCriteria ) ++m_nSegment;
	if (pszNext==NULL) pszNext=pB;
	if ( nCriteria == 0 )
		return (LPTSTR)(pT-pszBuf);
	else
		return pszNext;
}



int FormatForExp3( char* pzKeyTarVal, char* pzTmp, char *pzFmt )
{
	char szVaTmp[256]={0}, szFmt[64], *ps, szWork[20]={0};
	
	ps = strchr( pzFmt, '|' );
	if ( ps==NULL ) return FormatForExp( pzKeyTarVal, pzTmp, pzFmt, szWork );

	strncpy( szFmt, pzFmt, (ps-pzFmt) );
	szFmt[ps-pzFmt]=0; ++ps;
	
	FormatForExp( szVaTmp, pzTmp, szFmt, szWork );
	return FormatForExp3( pzKeyTarVal, szVaTmp, ps );
}




// Create a expend text with RTF Field format
/*************************************************************************************\
   * Replace ceitain fields within a RTF File with given string.
  
	Replaceed data format				 
	within the RTF File 				 	

    <*AcctNo 08 1 1>12345678			 
	 ------- --	- -	--------			 
	 A  	 B	C D	E					 
	~~~~~~~~~~~~~~~~~~~~~~~~			 

     A : Key Name
	 
	 B : Length of the Value String.
	       0 - Insert ReplaceStr, and fill the length of ReplaceStr here
		   n - a: C = 0 ~ 1 Use ReplaceStr to replace ValueStr.
		       b: C = 2		Delete ValueStr, Insert ReplaceStr in E and 
			                fill length of ReplaceStr in B.

	 C : PAD format
	       0 - PADLeft   
		   1 - PADRight   
		   2 - PADCenter
		   3 - No PAD, delete E which length in B, insert replace string, 
		       fill length of replace string in B 
		   4 - Same as 3, but keep the original string if G is null
		   5 - strftime Format date/time
		   6 - sprintf Formated String
		   7 - sprintf Formated Numerical
		   8 - sscanf Formated String
		   9 - Standard UniPipe Export Format

	 D : Character to be pad to the Replace String accordingly 

	 E : Current Value String in the story

	if (C==5,6,7,8,9), the hidden field will take the format of
    <*AcctNo 08 C :=_exprssion_>. _expression_ will be the
	strftime format expression (if C=5, value is given as yyyymmddhhmmss or
	mm/dd/yyyy hh:mm or Month day, year hh:mm:ss) 
	or sprintf format expression (if C=6,7)
	or sscanf format expression (if C=8)

    a) strftime format:

	%a 	Abbreviated weekday name
	%A 	Full weekday name
	%b 	Abbreviated month name
	%B 	Full month name
	%c 	Date and time representation appropriate for locale
	%d 	Day of month as decimal number (01 – 31)
	%H 	Hour in 24-hour format (00 – 23)
	%I 	Hour in 12-hour format (01 – 12)
	%j 	Day of year as decimal number (001 – 366)
	%m 	Month as decimal number (01 – 12)
	%M 	Minute as decimal number (00 – 59)
	%p 	Current locale’s A.M./P.M. indicator for 12-hour clock
	%S 	Second as decimal number (00 – 59)
	%U 	Week of year as decimal number, with Sunday as first day of week (00 – 53)
	%w 	Weekday as decimal number (0 – 6; Sunday is 0)
	%W 	Week of year as decimal number, with Monday as first day of week (00 – 53)
	%x 	Date representation for current locale
	%X 	Time representation for current locale
	%y 	Year without century, as decimal number (00 – 99)
	%Y 	Year with century, as decimal number
	%z, %Z  Time-zone name or abbreviation; no characters if time zone is unknown
	%% 	Percent sign

    As in the printf function, the # flag may prefix any formatting code. In that case, the meaning of the format code is changed as follows.
	Format Code Meaning 
	%#a, %#A, %#b, %#B, %#p, %#X, %#z, %#Z, %#% # flag is ignored. 
	%#c Long date and time representation, appropriate for current locale. For example: “Tuesday, March 14, 1995, 12:41:29”. 
	%#x Long date representation, appropriate to current locale. For example: “Tuesday, March 14, 1995”. 
	%#d, %#H, %#I, %#j, %#m, %#M, %#S, %#U, %#w, %#W, %#y, %#Y Remove leading zeros (if any). 
	
	b) sprintf format:
	
	%[flags] [width] [.precision] [{h | l | I64 | L}]type, see VC++ manual for detail
	
	c) sscanf format:

	%[*] [width] [{h | l | I64 | L}]type, see VC++ manual
	
	<*AcctNo 08 1 1> will be taken hidden text format in the WordFile

\*************************************************************************************/

void CPatParse::InsertAllText ( LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, int nOpt )
{
	LPTSTR ps, pos, pos1;
	char ch, ch1, ch2;
	char szKeyName[200]={0}, szKeyTarVal[1000]={0}, szTmp[2000]={0}, szT2[2000], szFmt[400]={0};
	int k=0, i, j, n, m, n1, n2, nValLen, nKeyLen;
	int nKVSrcLen, nKVTarLen;

	ps = pszTmplt;
	if ( ps[0]==0 ) ps=cszDefPubTmplt;

	while ( ps[0] && k < nLen-1 )
	{
		if ( ps[0]=='<' && ps[1]=='*' )
		{
			/* n=0;
			while ( ps[0] && ps[0]!='>' && n<255 ) szItem[n++]=(ps++)[0];
			if ( ps[0]=='>' ) szItem[n++]=(ps++)[0]; szItem[n]=0; */

			// Need more detail parsing for <*KeyName dd d d> or <*KeyName dd d :=Format String>
			pos = ps+2; j=0; 
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

			szTmp[0]=0;
			GetOneValue( szKeyName, szTmp, sizeof(szTmp) );
			nValLen = strlen( szTmp );
			
			if (nValLen==0 && ch1=='4') goto Loop; //keep original with a NULL

			// Get Format String to szFmt
			szFmt[0] = 0; m = 0;
			while ( pos1[0] && pos1[0]!='>' && ch2==':' && m<sizeof(szFmt)-1 )
				if ( pos1[0]>=' ' ) szFmt[m++]=(pos1++)[0]; else pos1++;
			if ( pos1[0]=='>' ) ++pos1;
			szFmt[m] = 0;
			// pos1 to end of strings
			

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
					nKVTarLen = GetCTimeStr( szKeyTarVal, szFmt, szTmp );
					break;
				}
			case '6':  // sprintf string expression
				if (ch=='=')
				{
					sprintf( szKeyTarVal, szFmt, szTmp );
					nKVTarLen = strlen( szKeyTarVal );
					break;
				}
			case '7':  // sprintf numerical expression
				if (ch=='=')
				{
					sprintf( szKeyTarVal, szFmt, atol(szTmp) );
					nKVTarLen = strlen( szKeyTarVal );
					break;
				}
			case '8':  // sscanf string expression
				if (ch=='=')
				{
					memset( szT2, 0, 100 );
					sscanf( szTmp, szFmt, szT2 );
					strcpy( szKeyTarVal, szT2 );
					nKVTarLen = strlen( szKeyTarVal );
					break;
				}
			case '9':  // standard UniPipe Export format
				if (ch=='=')
				{
					nKVTarLen = FormatForExp3( szKeyTarVal, szTmp, szFmt);
					break;
				}

			default: 
				nKVSrcLen = nKeyLen;
				nKeyLen = nValLen;
				nKVTarLen = nKeyLen;
				strncpy( szKeyTarVal, szTmp, sizeof(szKeyTarVal) );
			}
			szKeyTarVal[ nKVTarLen ] = 0;

			if ( nOpt & OPT_NO_HIDDEN )
			{
				n1 = 0;
				strcpy( szTmp, szKeyTarVal );
				ps = pos1;
			}
			else
			{
				n1 = (pos1-ps); n2 = (pos-ps);
				strncpy( szTmp, ps, n1 );
				sprintf( szT2, "%02d", nKVTarLen );
				strncpy( szTmp+n2, szT2, 2 );
				strncpy( szTmp+n1, szKeyTarVal, nKVTarLen );
				szTmp[ n1+nKVTarLen ] =0;
				ps += n1;
			}

			/* Convert to standard charset
			m=0; i=0;
			while ( i < n1+nKVTarLen )
			{
				if (szTmp[i]=='&') {
					szT2[m++]='&';
					szT2[m++]='a';
					szT2[m++]='m';
					szT2[m++]  ='p';
					szT2[m++]=';';
				}
				else if (szTmp[i]=='<') {
					szT2[m++]='&';
					szT2[m++]='l';
					szT2[m++]='t';
					szT2[m++]=';';
				}
				else if (szTmp[i]=='>') {
					szT2[m++]='&';
					szT2[m++]='g';
					szT2[m++]='t';
					szT2[m++]=';';
				}
				else if (szTmp[i]==0x27) {
					szT2[m++]='&';
					szT2[m++]='a';
					szT2[m++]='p';
					szT2[m++]='o';
					szT2[m++]='s';
					szT2[m++]=';';
				}
				else if (szTmp[i]=='"') {
					szT2[m++]='&';
					szT2[m++]='q';
					szT2[m++]='u';
					szT2[m++]='o';
					szT2[m++]='t';
					szT2[m++]=';';
				}
				else
					szT2[m++]=szTmp[i];
				++i;
			} 
			if ( k+m < nLen-1 ) strncpy( pszBuf+k, szT2, m );
			*/

			m = n1+nKVTarLen;
			if ( k+m < nLen-1 ) strncpy( pszBuf+k, szTmp, m );
			k += m;
			continue;
Loop:
			if ( ps[0] ) pszBuf[k++]=(ps++)[0];
		}
		else if ( ps[0]=='{' && ps[1]=='*' ) // Start of an expression
		{
			int nLev=1, t=3;
			char *ps1 = ps; 
			ps +=2; t=3; szTmp[0]='#'; szTmp[1]='{'; szTmp[2]='*';
			while ( ps[0] && t<1900 && nLev>0 ) 
			{
				if ( ps[0]=='{' && ps[1]=='*' ) nLev++;
				if ( ps[0]=='*' && ps[1]=='}' ) nLev--;
				if (nLev==0) break;
				szTmp[t++]=(ps++)[0];
			}
			
			if ( ps[0] && t<1900 )
			{
				szTmp[t++]=(ps++)[0]; szTmp[t++]=(ps++)[0]; 
				szTmp[t++]=0;
			
				szT2[0]=0;
				SetAllText(szTmp, szT2, sizeof(szT2));
				m = strlen( szT2 );
				if ( k+m < nLen-1 ) strncpy( pszBuf+k, szT2, m );
				k += m;
				continue;
			}
			else
			{
				ps = ps1;
				if ( ps[0] ) pszBuf[k++]=(ps++)[0];
			}
		}
		else
			if ( ps[0] ) pszBuf[k++]=(ps++)[0];
	}

	pszBuf[k<nLen?k:nLen-1]=0;
}


// Extended Version
void CPatParse::InsertAllTextEx( LPTSTR pszValue, LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, int nOpt )
{
	char *ps = pszValue;
	char ch, szItem[500]={0}, szKey[64]={0}, szVal[500]={0};

	while ( ps[0] )
	{
		szItem[0]=0; ch=0;
		sscanf( ps, "%[^|]%c", szItem, &ch );
		ps += (strlen(szItem)+(ch?1:0));
		
		szKey[0]=0; szVal[0]=0;
		sscanf( szItem, "%[^=]%*c%[^\n]", szKey, szVal );
		SetOneValue( szKey, szVal, 1 );
	}

	if ( nOpt & OPT_SET_VARS ) return;
	InsertAllText( pszTmplt, pszBuf, nLen, nOpt );
}



// Processing a standard item
int CPatParse::GetProperKeyVal( char* pzKName, char* pzKVal, int nLmt, char* pzLine, int& LnNo, int& LnNoT, bool bRept )
{
	char szTmpF[255], *qs, *ps;
	int nn = 0;
	DWORD nSize;

	pzKVal[0] = 0;
	if ( stricmp(pzKName,"RptLine")==0 && bRept )
	{
		if ( strcmp( pzKName, "RPTLINE" )==0 ) strupr( pzLine );
		else if ( strcmp( pzKName, "rptline" )==0 ) strlwr( pzLine );

		/* qs=pzKVal; ps=pzLine;
		while ( ps[0] && nn<nLmt-1 )
		{
			if ( ps[0]=='|' ) { (qs++)[0]='\\'; (qs++)[0]='F'; (qs++)[0]='\\'; nn+=3; }
			else if ( ps[0]=='^' ) { (qs++)[0]='\\'; (qs++)[0]='S'; (qs++)[0]='\\'; nn+=3; }
			else if ( ps[0]=='~' ) { (qs++)[0]='\\'; (qs++)[0]='R'; (qs++)[0]='\\'; nn+=3; }
			else if ( ps[0]=='&' ) { (qs++)[0]='\\'; (qs++)[0]='T'; (qs++)[0]='\\'; nn+=3; }
			else if ( ps[0]=='\\' ) { (qs++)[0]='\\'; (qs++)[0]='E'; (qs++)[0]='\\'; nn+=3; }
			else if ( (ps[0]>=' ' || ps[0]=='\t') )	{ (qs++)[0]=(ps)[0]; ++nn; }
			else ps;
			ps++;
		}
		qs[0]=0; */
		strncpy( pzKVal, pzLine, nLmt );
		LnNoT ++;
	}
	else if ( strcmp(pzKName, "LineNoS")==0 && bRept )
	{
		sprintf( pzKVal, "%d", LnNo );
	}
	else if ( strcmp(pzKName, "LineNo")==0 && bRept )
	{
		sprintf( pzKVal, "%d", ++LnNo );
	}
	else if ( strncmp( pzKName, "UniqueNo", 8)==0 )
	{
		if ( m_szWorkDir!=NULL && m_szWorkDir[0] )
			sprintf( szTmpF, "%s\\UNIQUENO.MAP", m_szWorkDir );
		else
			sprintf( szTmpF, "UNIQUENO.MAP" );
		GetUniqueNoByFile( pzKVal, szTmpF );
	}
	else if ( strncmp(pzKName, "CurDateTime", 11)==0 )
	{
		CTime tx = CTime::GetCurrentTime();
		strncpy( pzKVal, tx.Format("%Y%m%d%H%M%S"), nLmt );
	}
	else if ( strncmp(pzKName, "CurUniqueTime", 11)==0 )
	{
		CTime tx = m_nGTime++;
		strncpy( pzKVal, tx.Format("%Y%m%d%H%M%S"), nLmt );
	}
	else if ( strncmp( pzKName, "CurUserName", 11)==0 )
	{
		nSize = nLmt;
		GetUserName( pzKVal, &nSize ); 
	}
	else if ( strncmp( pzKName, "CurWorkDir", 10)==0 )
	{
		nSize = nLmt;
		GetCurrentDirectory( nSize, pzKVal );
	}
	else if ( strncmp( pzKName, "CurWinDir", 9)==0 )
	{
		nSize = nLmt;
		GetWindowsDirectory( pzKVal, nSize );
	}
	else if ( strncmp( pzKName, "CurSysDir", 9)==0 )
	{
		nSize = nLmt;
		GetSystemDirectory( pzKVal, nSize );
	}
	else if ( strncmp( pzKName, "CurTempDir", 10)==0 )
	{
		nSize = nLmt;
		GetTempPath( nSize, pzKVal );
	}
	else if ( strncmp( pzKName, "CurCompName", 11)==0 )
	{
		nSize = nLmt;
		GetComputerName( pzKVal, &nSize );
	}
	else if ( pzKName[0]=='\'' || pzKName[0]=='"' )
	{
		qs = pzKVal; szTmpF[0]=pzKName[0]; szTmpF[1]=0;
		xsscanf( pzKName+1, qs, szTmpF, nLmt );
	}
	else if ( pzKName[0]>='0' && pzKName[0]<='9' ) 
	{	
		// <nn> or <nn>^...^<nn>
		ps = pzKName;
		while ( ps[0] && nn<nLmt-1)
		{
			qs = szTmpF;
			ps = xsscanf( ps, qs, "^_" );
			pzKVal[nn++] = atoi( szTmpF );
		}
		pzKVal[nn]=0;
	}
	else 
		GetOneValue( pzKName, pzKVal, nLmt );

	return (nn==0?strlen( pzKVal ):nn);
}


// In the Template, <*PatLast> standard Key <PatLast>, <*nnn> -> CHR(nn)
void CPatParse::SetOneRecord( LPTSTR pszBuf, int nLen, int nOption )
{
#ifdef		XML_DEF
	if (gpXMLObj!=NULL && m_szCurTask[0]!=0 )
	{
		gpXMLObj->GetAllValue( m_szCurTask, pszBuf, nLen, nOption );
	}
#else
	CHAR szKTmp[1200], szKey[50], szVal[200];
	int i;

	pszBuf[0] = 0;

	i=0;																	    
	while (	GetOneValueIdx( i, szKey, szVal, 1200 ) )
	{
		sprintf( szKTmp, "%s=%s|", szKey, szVal	);
		strncat( pszBuf, szKTmp, nLen-strlen(pszBuf) );
		i++;
	}
#endif
}



//////////////////////////////////////////////////////////////
//				Variables Processing
//////////////////////////////////////////////////////////////


void CPatParse::RegisterTask( LPTSTR pszTask )
{
#ifdef		XML_DEF
	if (gpXMLObj!=NULL)
	{
		gpXMLObj->SetSubTree( pszTask );
	}
#endif
	strncpy(m_szCurTask, pszTask, sizeof(m_szCurTask)-1);
}


void CPatParse::InitValues( bool bRelease, bool bAll )
{
	memset(szVarNameSave, 0, sizeof(szVarNameSave));
	m_nIndexA = 0;  // Intern default Index
	m_nSegment = 0;

#ifdef		XML_DEF
	if (gpXMLObj==NULL) return;

	if ( bRelease )
		gpXMLObj->ReleaseSubTree( m_szCurTask );
	gpXMLObj->SetSubTree( m_szCurTask );
	m_nKeys = 0;

	if ( bAll )
	{
		if ( bRelease )
			gpXMLObj->ReleaseSubTree( "Global" );
		gpXMLObj->SetSubTree( "Global" );
		g_nKeys = 0;
	}
#else
	int i;

	m_nKeys = 0;
	for ( i=0; i<MAXNUMFLDS; i++ ) 
	{
		m_szKeyName[i][0] = 0;
		if ( bRelease && m_szKeyValue[i]!=NULL ) delete m_szKeyValue[i];
		m_szKeyValue[i] = NULL;
	}

	if ( bAll )
	{
		g_nKeys = 0;
		for ( i=0; i<MAXNUMFLDS; i++ ) 
		{
			g_szKeyName[i][0] = 0;
			if ( bRelease && g_szKeyValue[i]!=NULL ) delete g_szKeyValue[i];
			g_szKeyValue[i] = NULL;
		}
	}
#endif
}


BOOL CPatParse::GetAllValue( LPTSTR pszKeyValue, int nLen )
{
#ifdef		XML_DEF
	TCHAR szTemp[256];

	if (gpXMLObj==NULL || m_szCurTask[0]==0 ) 
	{
		pszKeyValue[0]=0;
		return false;
	}
	
	sprintf( szTemp, "%s", m_szCurTask );
	return gpXMLObj->GetAllValue( szTemp, pszKeyValue, nLen);
#else
	return false;
#endif
}


void CPatParse::CreateEscStr( LPTSTR pDest, int nLmt, LPTSTR pSrc ) 
{
	LPTSTR ps, qs;
	int nn=0;
	TCHAR chs[8];

	ps = pDest; qs = pSrc;
	while ( nn<nLmt && qs[0] )
	{
		sprintf( chs, "%02X", qs[0] ); 
		switch ( qs[0] )
		{
		//case '(':
		//case ')':
		//case '[':
		//case ']':
		case '{':
		case '}':
		case ' ':
			(ps++)[0]='\\'; (ps++)[0]='x'; 
			(ps++)[0]=chs[0]; (ps++)[0]=chs[1]; 
			qs++; break;
		default:
			(ps++)[0]=(qs++)[0]; break;
		}
	}
	ps[0]=0;
}


BOOL CPatParse::GetOneValue( LPTSTR pszKeyName, LPTSTR pszKeyValue, int nLen )
{
	bool ss;
#ifdef		XML_DEF
	TCHAR szTemp[1024];
	char  szKeyName[256]={0};
	bool  bt = true;

	CreateEscStr( szKeyName, 255, pszKeyName ); 
	pszKeyName = szKeyName;
	if ( strncmp(pszKeyName, "[A]", 3)==0 )
	{
		pszKeyName += 3;
		bt = false;
	}

	if (gpXMLObj==NULL || m_szCurTask[0]==0 ) 
	{
		pszKeyValue[0]=0;
		return false;
	}
	if ( strncmp(pszKeyName,"gv",2)==0 )
	{
		sprintf( szTemp, "Global.%s", pszKeyName );
		ss = (( bt?gpXMLObj->GetOneValue( szTemp, pszKeyValue, nLen ):
				gpXMLObj->GetAllValue( szTemp, pszKeyValue, nLen, 2 ) ) !=0 );
	}
	else if ( strchr(pszKeyName,'.')!=NULL )
	{
		ss = (( bt?gpXMLObj->GetOneValue( pszKeyName, pszKeyValue, nLen ):
				gpXMLObj->GetAllValue( pszKeyName, pszKeyValue, nLen, 2 )) !=0 );
	}
	else
	{
		sprintf( szTemp, "%s.%s", m_szCurTask, pszKeyName );
		ss = (( bt?gpXMLObj->GetOneValue( szTemp, pszKeyValue, nLen ):
				gpXMLObj->GetAllValue( szTemp, pszKeyValue, nLen, 2 )) !=0 );
	}
#else
	int i, n;
	char *p, buf[8000] = {0};

	ss = FALSE;
	memset(pszKeyValue,0, nLen);

	if ( strncmp(pszKeyName,"gv",2)!=0 )
	{
		for (i=0; i<m_nKeys; i++) 
		{
			if (strcmp( (LPTSTR)m_szKeyName[i], pszKeyName )==0)
			{
				strncpy( pszKeyValue, (LPTSTR)m_szKeyValue[i], nLen-1 );
				ss = TRUE;
				break;
			}
		}
	}
	else
	{
		for (i=0; i<g_nKeys; i++) 
		{
			if (strcmp( (LPTSTR)g_szKeyName[i], pszKeyName )==0)
			{
				strncpy( pszKeyValue, (LPTSTR)g_szKeyValue[i], nLen-1 );
				ss = TRUE;
				break;
			}
		}
	}

	if (ss)
	{
		// Filter for ["]
		strncpy( buf, pszKeyValue, sizeof(buf)-1 );
        p = buf;
        n = 0;
        for ( p = buf; (UINT)(p - buf) < strlen( buf ); p++ )
		{
        	if ( *p	!= '"' )
			{
			    buf[n] = *p;
        		n++;
			}
		}
        buf[n] = 0x0;
		strncpy( pszKeyValue, (LPCTSTR)buf, nLen-1 );
	}
#endif

	if (pszKeyValue[0]==0 && strcmp(pszKeyName,"gvCurFile")==0 )
		ss = (GetOneValue( "CurFile", pszKeyValue, nLen )!=0);
	return ss;
}


BOOL CPatParse::GetOneValueIdx( int nIdx, LPTSTR pszKeyName, LPTSTR pszKeyValue, int nLen )
{
#ifdef		XML_DEF
	pszKeyValue[0] = 0;
	return false;
#else
	int i,n;
	BOOL ss;
	char *p, buf[512] = {0} ;

	ss = FALSE;
	pszKeyName[0]=0;
	memset(pszKeyValue,0, nLen);

	i = nIdx;
	if (i<m_nKeys) 
	{
		strcpy( pszKeyName, (LPTSTR)m_szKeyName[i] );
		strncpy( pszKeyValue, (LPTSTR)m_szKeyValue[i], nLen-1 );
		ss = true;
	}
	else if (i>=m_nKeys && i<m_nKeys+g_nKeys)
	{
		i -= m_nKeys;
		strcpy( pszKeyName, (LPTSTR)g_szKeyName[i] );
		strncpy( pszKeyValue, (LPTSTR)g_szKeyValue[i], nLen-1 );
		ss = true;
	}

	if (ss)
	{
		// Filter for ["]
		strncpy( buf, pszKeyValue, sizeof(buf)-1 );
        p = buf;
        n = 0;
        for ( p = buf; (UINT)(p - buf) < strlen( buf ); p++ )
		{
        	if ( *p	!= '"' )
			{
			    buf[n] = *p;
        		n++;
			}
		}
        buf[n] = 0x0;
		strncpy( pszKeyValue, (LPCTSTR)buf, nLen-1 );
	}
	return ss;
#endif
}


#define		VALUE2BUF(x,y,n)	if (x!=NULL) delete x; x = new TCHAR[n+8]; strcpy(x,y); x[n]=0;


// Append the previous results
void CPatParse::SetOneValue( LPTSTR pszKeyName, LPTSTR pszKeyValue, int nOver )
{
	char  szKeyName[128]={0};

	if (stricmp(pszKeyName,"RptBody")==0||stricmp(pszKeyName,"RptLine")==0)
	{
		SetRptContent( pszKeyValue );
		return;
	}

	if ( m_bSaveVar )	
		// Determine whether overwrite (variable first appear) or append
	{
		if ( strstr(szVarNameSave, pszKeyName)==NULL )
		{
			strcat(szVarNameSave, pszKeyName);
			strcat(szVarNameSave, "|");
		}
	}


#ifdef		XML_DEF
	TCHAR szTemp[1200], *szBuf, *ps, *ts;
	int nn;

	if (gpXMLObj==NULL || m_szCurTask[0]==0 ) return;
	
	CreateEscStr( szKeyName, 127, pszKeyName ); 
	pszKeyName = szKeyName;

	nn = strlen(pszKeyValue); ++nn;
	szBuf =	new TCHAR[nn+2]; ps = szBuf; 
	ps[0] = 0; strncpy(ps, pszKeyValue, nn); ps[nn] = 0;

	if (ps[0]=='"')
	{
		ps++; ts=strchr(ps,'"'); if (ts!=NULL) ts[0]=0;
	}

	if (ps[0]=='\'')
	{
		ps++; ts=strchr(ps,'\''); if (ts!=NULL) ts[0]=0;
	}

	if ( strncmp(pszKeyName,"gv",2)==0 )
	{
		sprintf( szTemp, "Global.%s", pszKeyName );
		++g_nKeys;
		gpXMLObj->SetOneValue( szTemp, ps, (nOver!=0) );
		delete szBuf;
		return;
	}
	else if ( strcmp(pszKeyName,"SYSRESET")==0 )
	{
		gpXMLObj->ReleaseSubTree( ps );
		delete szBuf;
	}
	else if ( strchr(pszKeyName,'.')!=NULL )
	{
		gpXMLObj->SetOneValue( pszKeyName, ps, (nOver!=0) );
		delete szBuf;
		return;
	}
	else
	{
		sprintf( szTemp, "%s.%s", m_szCurTask, pszKeyName );
		++m_nKeys;
		gpXMLObj->SetOneValue( szTemp, ps, (nOver!=0) );
		delete szBuf;
		return;
	}
#else
	int i;
	bool bGlobal;
	TCHAR szTemp[8000]={0};

	strncpy( szKeyName, pszKeyName, MAXKEYLEN );
	szKeyName[MAXKEYLEN-1] = 0;
	pszKeyName = szKeyName;

	bGlobal = (strncmp(pszKeyName,"gv",2)==0);
	if ( bGlobal )
	{
		for (i=0; i<g_nKeys && i<MAXNUMFLDS; i++) 
			if (strcmp( (LPTSTR)g_szKeyName[i], (LPTSTR)pszKeyName)==0)
				break;
	}
	else
	{
		for (i=0; i<m_nKeys && i<MAXNUMFLDS; i++) 
			if (strcmp( (LPTSTR)m_szKeyName[i], (LPTSTR)pszKeyName)==0)
				break;
	}
	
	if (i<MAXNUMFLDS)
	{
		if ( bGlobal )
		{
			strncpy((LPTSTR)g_szKeyName[i], (LPTSTR)pszKeyName, sizeof(g_szKeyName[i])-1);
			if ( nOver ) 
			{ 
				VALUE2BUF( g_szKeyValue[i], pszKeyValue, strlen(pszKeyValue) ) 
			}
			else
			{
				if ( g_szKeyValue[i]!=NULL)
				{
					strncpy(szTemp, g_szKeyValue[i], 8000);
					strncat(szTemp, pszKeyValue, 8000-strlen(pszKeyValue));
				}
				else
					strncpy(szTemp, pszKeyValue, 8000);
				VALUE2BUF( g_szKeyValue[i], szTemp, strlen(szTemp) ) 
			}
			if (strcmp( (LPTSTR)g_szKeyValue[i],"\"\"")==0)	g_szKeyValue[i][0] = 0;
			if (i==g_nKeys) ++g_nKeys;
		}
		else
		{
			strncpy((LPTSTR)m_szKeyName[i], (LPTSTR)pszKeyName, sizeof(m_szKeyName[i])-1);
			if ( nOver ) 
			{ 
				VALUE2BUF( m_szKeyValue[i], pszKeyValue, strlen(pszKeyValue) ) 
			}
			else
			{
				if ( m_szKeyValue[i]!=NULL)
				{
					strncpy(szTemp, m_szKeyValue[i], 8000);
					strncat(szTemp, pszKeyValue, 8000-strlen(pszKeyValue));
				}
				else
					strncpy(szTemp, pszKeyValue, 8000);
				VALUE2BUF( m_szKeyValue[i], szTemp, strlen(szTemp) ) 
			}
			if (strcmp( (LPTSTR)m_szKeyValue[i],"\"\"")==0)	m_szKeyValue[i][0] = 0;
			if (i==m_nKeys) ++m_nKeys;
		}
	}
#endif
}


// Overwrite to previous value
void CPatParse::SetOneValueEx( LPTSTR pszKeyName, LPTSTR pszKeyValue )
{
	SetOneValue( pszKeyName, pszKeyValue, 1 );
}

// Clear up all variable adding within this time
void CPatParse::ClearCurVars( )
{
	LPTSTR ps;
	TCHAR szTemp[50], ch;

	ps = szVarNameSave;
	while ( ps[0] )
	{
		while ( ps[0]=='|' ) ++ps;
		szTemp[0]=0; ch=0;
		sscanf(ps, "%[^|]%c", szTemp, &ch);
		ps += strlen(szTemp)+(ch?1:0);
		if ( szTemp[0] )
			SetOneValue(szTemp, "", 1);
	}
	szVarNameSave[0] = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Save the report contents (from Interface) to the buffer for further retrieve
void CPatParse::SetRptContent( LPTSTR pszContent )
{
	FILE *fp;
	TCHAR szTempFN[_MAX_PATH], *fns;

	if (strlen( pszContent )>0)
	{
		szTempFN[0]=0;
		GetOneValue( "OutputFN", szTempFN, sizeof(szTempFN));
		if ( szTempFN[0] ) fns = szTempFN;
		else fns = m_szContFile;

		fp = fopen( fns, "wb+" );
		if ( fp!=NULL )
		{
			fseek( fp, 0L, SEEK_END );
			fwrite( pszContent, 1, strlen(pszContent), fp );
			fclose( fp );
		}
	}
}


// Evaluation of Express from the Control Field
void CPatParse::ExpEvaluate( LPCTSTR pszOpt, LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, bool bStr )
{
	CHAR szTBuf[1200], szTemp[256], *pos, *ts;
	int i, k, nLev;
		
	pszBuf[0]=0;
	if (bStr) strcpy( pszBuf, " where ("); else pszBuf[0]=0;
	pos = strstr( pszTmplt, pszOpt );
	if ( pos != NULL )
	{
		pos += strlen( pszOpt )+2;

		// sscanf( pos, "%[^}]", szTBuf );
		ts=szTBuf; k=0; nLev=0; ; 
		while ( pos[0] && ( nLev>0 || pos[0]!='}') ) 
		{
			ts[k++]=(pos[0]!='`'?pos[0]:39);
			if ( pos[0]=='{' ) nLev++;
			if ( pos[0]=='}' ) nLev--;
			++pos;
		}
		ts[k] = 0;  
		
		if ( bStr )
		{
			SetAllText( szTBuf, pszBuf+8, nLen-10 );
			for (i=0; pszBuf[i] && (i<1200); i++)
				if (pszBuf[i]=='`') pszBuf[i]= 39;
		}
		else
		{
			SetAllText( szTBuf, pszBuf, nLen );
			for (i=0; pszBuf[i] && (i<1200); i++)
				if (pszBuf[i]=='`') pszBuf[i]= 39;
			if ( ExtendEvaluate( pszBuf, szTemp ) ) strcpy( pszBuf, szTemp );
		}
	}
	if (bStr) strcat( pszBuf, " )");

	return;
}

// Select the proper item by criteria expression
bool CPatParse::SelectPropItem( LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, int nOpt )
{
	char szTmplt[4096]={0}, szRes[1024];
	char szCriteria[1024]={0}, szItem[1024]={0};
	char *ps, *qs, ch;
	int nn = 0;
	bool ss = false;

	pszBuf[0] = 0;
	ps = pszTmplt; qs = szTmplt;
	if ( ps[0]!='$' || ps[1]!='{' ) return false; 
	ps += 2;

	while ( ps[0] )
	{
		if (ps[0]=='\\' && ps[1]=='q' ) { (qs++)[0]='\''; ps+=2; }
		else (qs++)[0]=(ps++)[0];
	}
	qs[0]=0;

	ps = szTmplt;
	qs = strrchr(ps,'}');
	if ( qs==NULL ) return false; else qs[0]=0;

	while ( ps[0] )
	{
		szItem[0] = 0;
		qs = szItem;
		SCANONEITEM(ps, qs, ch, '=');
		
		szCriteria[0] = 0;
		qs = szCriteria;
		SCANONEITEM(ps, qs, ch, '|');

		SetAllText( szItem, szRes, 1024 );
		strcpy( szItem, szRes );
		if ( nOpt & OPT_TWO_ITEMS )
		{
			qs = szItem;
			while ( qs[0] ) { if (qs[0]=='^') qs[0]='='; ++qs; }
		}

		SetAllText( szCriteria, szRes, 1024 );
		if ( strstr(szRes, "TRUE")!=NULL ) 
		{ 
			ss = true;
			if ( strlen(pszBuf)+strlen(szItem)+1<nLen ) 
			{
				if ( strlen(pszBuf)>0 ) strcat( pszBuf, "|" );
				strcat( pszBuf, szItem );
			}
			if ((nOpt&OPT_MULTI_SEL)==0) break; 
		}
	}

	return ss;
}


/*
{<GrpName> <Class> <GDelmt> <IdStr>...|<FldsName> <StartPos> <Len> <Type> <SubType> <Delimitor>|...}

  <GDelmt> ::= [#|$][<ExtStr>|*SngRec][(<nn>)]
	*SngRec - Entire file to be a single record 
	Rule: B -> By DBF Default Length
		  F and atoi(IdStr)>0 -> Get a fixed length	of atoi(IdStr)
		  Other -> by <ExtStr> <ExtStr> is regular string with (\n\t\r\s)
				   or <ExtStr>[n], count <ExtStr> n times.

	# means this group will be used sequentially followed the previous one 
	$ means this group will be repeatly used after the following group
	<nn> means this group should be used to next <nn> records
	<nn>=='valid' means this group to be applied to the record until the Validation failed
    <nn>=='skip' means to skip certain this record without any processing
*/

void CPatParse::SetTFLData( char* pTFLFormat )
{
 	char szTmp[1200], szFlds[120], szName[50], szIdStr[256], szClass[20], szDltr[32];
	char *pCur, *pFld, *ps, ch;
	int npFlds, nPos=0, k;
	bool bSS;

	m_nGroup = 0;
	pCur = pTFLFormat;
	while ( pCur[0] && pCur[0]!='{' ) pCur++;
	if ( !pCur[0] ) return;

	while (	sscanf( pCur, "%*c%[^}]%*c", szTmp ) > 0 ) 
	{
		pCur += strlen( szTmp )+2;
		szName[0]=0; szClass[0]=0; szIdStr[0]=0;
		sscanf( szTmp, "%s %s %s %[^|]%*c", szName, szClass, szDltr, szIdStr );
		
		//*Mdy ValidStr	(imp)
		ps = szIdStr;
		while ( ps[0] )
		{
			if ( ps[0]=='\\' ) ps[0]='|';
			else if ( ps[0]=='`' ) ps[0]='\'';
			++ps;
		}

		pFld = strchr( szTmp, '|' )+1;
		strncpy( m_stGroup[m_nGroup].Name, szName, 
				 sizeof(m_stGroup[m_nGroup].Name) );	
		strncpy( m_stGroup[m_nGroup].IdStr, szIdStr, 
				 sizeof(m_stGroup[m_nGroup].IdStr) );	
		strncpy( m_stGroup[m_nGroup].Class, szClass, 
				 sizeof(m_stGroup[m_nGroup].Class) );

		// Need more precise process
		m_stGroup[m_nGroup].nCurr = 0;
		m_stGroup[m_nGroup].nEffect = 0;
		m_stGroup[m_nGroup].nOption = 0;
		m_stGroup[m_nGroup].nPrsCnt = 0;
		m_stGroup[m_nGroup].nReqCnt = 0;

		bSS = false;
		if ( szDltr[0]=='$' )
		{
			bSS = true;
			m_stGroup[m_nGroup].nOption |= OPT_TFLGRP_RPTSTRT;
			strcpy( szDltr, szDltr+1 );
		}
		else if ( szDltr[0]=='#' )
		{
			bSS = true;
			k = m_nGroup-1;
			while ( k>=0 && !(m_stGroup[k].nOption & (OPT_TFLGRP_CNTLMT|OPT_TFLGRP_2VALID)) ) --k;  
			if (k==0)
			{
				m_stGroup[k].nOption |= OPT_TFLGRP_CNTLMT;
				m_stGroup[k].nReqCnt = 1;
			}
			strcpy( szDltr, szDltr+1 );
		}
			
		ps = strrchr( szDltr, '(' );
		if ( ps!=NULL )
		{
			ps[0] = 0;
			m_stGroup[m_nGroup].nReqCnt = atoi(ps+1);
			//if ( m_stGroup[m_nGroup].nReqCnt == 9999 )
			if ( strncmp(ps+1,"valid",5)==0 )
			{
				m_stGroup[m_nGroup].nOption |= OPT_TFLGRP_2VALID;
				m_stGroup[m_nGroup].nReqCnt = 9999;
			}
			else if ( strncmp(ps+1,"skip",4)==0 )
			{
				m_stGroup[m_nGroup].nOption |= (OPT_TFLGRP_SKIP|OPT_TFLGRP_CNTLMT);
				m_stGroup[m_nGroup].nReqCnt = 1;
			}
			else
				m_stGroup[m_nGroup].nOption |= OPT_TFLGRP_CNTLMT;
		}
		else if ( bSS )
		{
			m_stGroup[m_nGroup].nReqCnt = 1;
			m_stGroup[m_nGroup].nOption |= OPT_TFLGRP_CNTLMT;
		}
		sprintf( m_stGroup[m_nGroup].Delimitor, szDltr, "" );

		if (m_stGroup[m_nGroup].Class[0]==0) m_stGroup[m_nGroup].Class[0]='F';
		if (m_stGroup[m_nGroup].IdStr[0]==0) 
			strcat(m_stGroup[m_nGroup].IdStr,"  ");
		else
		{
			ps = m_stGroup[m_nGroup].IdStr; k = 0;
			while ( ps[k] )
			{
				if (ps[k]=='/' && ps[k+1]=='/' ) { ps[k]=' '; ps[k+1]=' '; k+=2; }
				else ++k;
			}
		}

		npFlds = 0;	szFlds[0] = 0; ch=' ';
		while (	sscanf( pFld, "%[^|]%c", szFlds, &ch ) > 0 )
		{
			if (!szFlds[0]) break;
			pFld += strlen( szFlds )+1;
			if (pFld[0]=='|')  // '|' is a seperator
			{
				strcat( szFlds, "|" );
				++pFld;
			}
			if (pFld[0]=='\n') ++pFld;
			memset(&m_stGroup[m_nGroup].Fields[npFlds], 0x0, 
				sizeof(m_stGroup[m_nGroup].Fields[npFlds]));
			sscanf( szFlds, "%s %s %s %s %s %s",
				m_stGroup[m_nGroup].Fields[npFlds].KeyName,
				m_stGroup[m_nGroup].Fields[npFlds].StartPos,
				m_stGroup[m_nGroup].Fields[npFlds].Length,
				m_stGroup[m_nGroup].Fields[npFlds].Type,
				m_stGroup[m_nGroup].Fields[npFlds].SubType,
				m_stGroup[m_nGroup].Fields[npFlds].Delimitor );
			szFlds[0]=0;
			++npFlds;
			if (ch!='|') break; else ch=' ';
			while ( pFld[0]=='\r' || pFld[0]=='\n' || pFld[0]=='\t' || pFld[0]==' ' ) ++pFld;
		}
		m_stGroup[m_nGroup].nFields = npFlds;

		m_nGroup++;
		if (strncmp(pCur, "#[Ext]", 6)==0 ) break;
		while ( pCur[0] && pCur[0]!='{' ) pCur++;
		if ( !pCur[0] ) break;
	}
}


// Set DBF Header Structure for further processing
// nOption = 0:  pStruct ->  <FieldName1>=<Len1>|<FieldName2>=<Len2>|.....
//         = 1:  pStruct -> DBF's File Head with length of nLen  
//         = 3:  pStruct -> DBF's File Head with length of nLen, For Write  
BOOL CPatParse::SetDBFStruct(char *pStruct, int nLen, int nOption)
{
	char *ps;
	char szTmp1[30], szTmp2[30], ch1, ch2;
	int nPos =1, m, nL;

	if ( nOption == 0 )  // Text String Format: <FieldName>=<len>|...
	{
		m_nDBFlds = 0;
		ps = pStruct;
		while ( ps[0] )
		{
			ch1=0; ch2=0; szTmp1[0]=0; szTmp2[0]=0;
			sscanf(ps,"%19[^=]%c%[^|]%c", szTmp1, &ch1, szTmp2, &ch2);
			ps += (strlen(szTmp1)+(ch1?1:0)+strlen(szTmp2)+(ch2?1:0));
			if (ch1=='=')
			{
				nL = atoi( szTmp2 );
				strncpy( m_stDBFlds[m_nDBFlds].szName, szTmp1, 12 );
				m_stDBFlds[m_nDBFlds].nPos = nPos;
				m_stDBFlds[m_nDBFlds].nLen = nL;
				++m_nDBFlds;
				nPos += nL;
			}
			else
				break;
		}
		return (ch1=='=');
	}

	if ( nOption == 1 && pStruct[0]>0 )  // DBF File Head
	{
		m_nDBFlds = 0;
		ps = pStruct+32;
		while ( ps[0]>' ' )
		{
			strncpy( m_stDBFlds[m_nDBFlds].szName, ps, 11 );
			m =	*(int*)(ps+12);
			m_stDBFlds[m_nDBFlds].nPos = (m?m:nPos);
			if (ps[11]=='N')
				m_stDBFlds[m_nDBFlds].nLen = (int)ps[16];
			else
				m_stDBFlds[m_nDBFlds].nLen = *(int*)(ps+16);
			m_stDBFlds[m_nDBFlds].chType = ps[11];
			nPos += m_stDBFlds[m_nDBFlds].nLen;
			++m_nDBFlds;
			ps+=32;
		}
		return true;
	}

	if ( nOption == 2 )  // Output to the Text Format For Testing (For Testing)
	{
		int i;
		pStruct[0]=0;
		for ( i=0; i<m_nDBFlds; i++ )
		{
			sprintf( szTmp1, "%s=%d", m_stDBFlds[i].szName, m_stDBFlds[i].nLen );
			if (pStruct[0]) strcat(pStruct,"|");
			strcat( pStruct, szTmp1 );
		}
		return true;
	}

	if ( nOption == 3 && pStruct[0]>0 )  // DBF File Head, For Write
	{
		m_nwDBFlds = 0;
		ps = pStruct+32;
		while ( ps[0]>' ' )
		{
			strncpy( m_stwDBFlds[m_nwDBFlds].szName, ps, 11 );
			m =	*(int*)(ps+12);
			m_stwDBFlds[m_nwDBFlds].nPos = (m?m:nPos);
			if (ps[11]=='N')
				m_stwDBFlds[m_nwDBFlds].nLen = (int)ps[16];
			else
				m_stwDBFlds[m_nwDBFlds].nLen = *(int*)(ps+16);
			m_stwDBFlds[m_nwDBFlds].chType = ps[11];
			nPos += m_stwDBFlds[m_nwDBFlds].nLen;
			++m_nwDBFlds;
			ps+=32;
		}
		return true;
	}

	return false;
}




// Setup StrList for future %<nn>@<str> checking
void CPatParse::SetupStrList( char* pCriteria, char* pData )
{
	LPTSTR ps;
	TCHAR szTgt[32];
	int i;

	ps = pCriteria;
	while ( ps[0] )
	{
		if (ps[0]=='@' && (ps[-1]>='0' && ps[-1]<='9'))
		{
			szTgt[0] = 0;
			sscanf(ps+1,"%[^/ ]",szTgt);
			ps += strlen(szTgt)+1;

			if ( szTgt[0] )
			{
				if ( strstr(pData, szTgt)!=NULL )
				{	// This string appear in this record
					// Put szTgt into StrList and set its count to 0
					SRCHSTRLIST(szTgt, m_StrList)
					if (i<16)
					{
						strcpy(m_StrList[i].szStrL, szTgt);
						m_StrList[i].nCnt = 0;
					}
				}
				else // This string does not appear in this record
				{
					SRCHSTRLIST(szTgt, m_StrList)
					if (i<16) // this string appear before
						m_StrList[i].nCnt++;
				}
			}
		}
		else
			++ps;
	}

}


void CPatParse::AdjustUnstName(char *pszLast, char *pszFirst)
{
	char szBuf[120], szT1[50], szT2[50], szLast[25], szFirst[25], szMid[25], szTitle[25];
	BOOL ss;

	szT1[0]=0; szT2[0]=0;
	szLast[0]=0; szFirst[0]=0; szMid[0]=0; szTitle[0]=0;
	ss = GetOneValue( pszLast, szT1, 50 );
	ss |= GetOneValue( pszFirst, szT2, 50 );
	if (ss)
	{
		strcpy( szBuf, szT2 ); strcat( szBuf, " " ); 
		strcat( szBuf, szT1 ); // strcat( szBuf, " " ); 
		ParseName( szBuf, szLast, szFirst, szMid, szTitle );
		if ( szLast[0] && szFirst[0] )
		{
			SetOneValueEx( pszLast, szLast ); 
			SetOneValueEx( pszFirst, szFirst );
		}
	}
}

// Check if current message's event match the given event
// pszEvent -> given event, can contain ?
// Event will be in MSH-8
BOOL CPatParse::CheckHL7Event( LPTSTR pszBuf, LPTSTR pszEvent )
{
	LPTSTR ps;
	char szDisp[20], szEvn[100];
	int i;
	bool ss;

	if ( strcmp( pszEvent,"000" )==0) return true;
	if (!strncmp(pszBuf+1,"MSH",3))
	{
		// The Head of message: <0B>MSH|.....
		ps = pszBuf+1;  
		for (i=0; i<8; i++) 
		{
			while ( ps[0] && ps[0]!=pszBuf[4]) ++ps;
			if (ps[0]==0) break;
			++ps;
		}
		sprintf( szDisp, "%%12[^%c]", pszBuf[4] );
		if (ps[0])
   	   		sscanf(ps, szDisp, szEvn);
		else
			szEvn[0]=0;
		szEvn[12] = 0;

		ps = strstr( szEvn, pszEvent );
		if ( ps!=NULL ) return true;

		ps = strrchr( szEvn, pszEvent[0] );
		if ( ps!=NULL)
		{
			i=0; ss=true;
			while ( ss && pszEvent[i] )
			{
				ss &= (ps[i]==pszEvent[i]||pszEvent[i]=='?');
				++i;
			}
			return ss;
		}
		else
			return false;
	}
	else
		return false;
}


///////////////////////////////////////////////////////////////////////////////////
//      Implemetation of CEFTParse class - EFT: Extraction From Text
///////////////////////////////////////////////////////////////////////////////////
/*
	-----  EFT Tree	-----

	for text extraction:
	<MAIN>{<KeyS>[Address:I400122,T]
		   <#TITLE>[Name:  0000,]}<END>

	for text replacement:
	<MAIN>{<KeyS=_replace_>[Address:I400122=_replace_,T]
		   <#TITLE>[<szFldsName>]}<END>
	_replace_ is <constant>|*<variable>

	Search the Title, In the tree, 
	  <#TITLE> node defines a parse scheme from the Title, under this node.
      <#SSCANF> gives the Format String 
      <#nn>  gives the definition of the Item <nn>
  
	<szFldsName> ::= <szName>|..|<szName> 
	<szName> ::= <Name>:<Type>(1)<SubType>(1)<Length>(4)<StartPos>
	i.g. Name:  0000,

*/

CEFTParse::CEFTParse( )
{
	m_nCurNode = 0;
	m_nLastNode = 0;
	m_nLines=0;
	m_nChars=0;
	m_nLState=0;
	m_nCState=0;

	memset( m_sNode, 0, sizeof(m_sNode)); 
	m_szCurTask[0]=0;
	InitValues( false );

	bWantRels = true;
}



CEFTParse::~CEFTParse( )
{
	InitValues( bWantRels );
}


///////////////////////////////////////////////////////////////////
//		Variables Processing
void CEFTParse::RegisterTask( LPTSTR pszTask )
{
#ifdef		XML_DEF
	if (gpXMLObj!=NULL)
	{
		gpXMLObj->SetSubTree( pszTask );
	}
#endif
	strncpy(m_szCurTask, pszTask, sizeof(m_szCurTask)-1);
}


void CEFTParse::InitValues( bool bRelease )
{
	m_nKeys = 0;

#ifdef		XML_DEF
	if (gpXMLObj==NULL) return;

	if ( bRelease )
		gpXMLObj->ReleaseSubTree( m_szCurTask );
	gpXMLObj->SetSubTree( m_szCurTask );
#else
	int i;
	for ( i=0; i<MAXNUMFLDS; i++ ) 
	{
		m_szKeyName[i][0] = 0;
		if ( bRelease && m_szKeyValue[i]!=NULL ) delete m_szKeyValue[i];
		m_szKeyValue[i] = NULL;
	}
#endif

	m_szCurTask[0]=0;
	sprintf( m_szCurTask, "S%04d", ((int)time(NULL))%10000 );
}


BOOL CEFTParse::GetOneValue( LPTSTR pszKeyName, LPTSTR pszKeyValue, int nLen )
{
#ifdef		XML_DEF
	TCHAR szTemp[256];

	if (gpXMLObj==NULL || m_szCurTask[0]==0 ) 
	{
		pszKeyValue[0]=0;
		return false;
	}
	if ( strncmp(pszKeyName,"gv",2)==0 )
	{
		sprintf( szTemp, "Global.%s", pszKeyName );
		return gpXMLObj->GetOneValue( szTemp, pszKeyValue, nLen );
	}
	else if ( strchr(pszKeyName,'.')!=NULL )
	{
		return gpXMLObj->GetOneValue( pszKeyName, pszKeyValue, nLen );
	}
	else
	{
		sprintf( szTemp, "%s.%s", m_szCurTask, pszKeyName );
		return gpXMLObj->GetOneValue( szTemp, pszKeyValue, nLen );
	}
#else
	int i, n;
	BOOL ss;
	char *p, buf[8000] ;

	ss = FALSE;
	pszKeyValue[0]=0;
	for (i=0; i<m_nKeys; i++) 
	{
		if (strcmp( (LPTSTR)m_szKeyName[i], pszKeyName )==0)
		{
			strncpy( pszKeyValue, (LPTSTR)m_szKeyValue[i], nLen );
			// Filter for ["]
			strncpy( buf, pszKeyValue, sizeof(buf) );
        	p = buf;
        	n = 0;
         	for ( p = buf; (UINT)(p - buf) < strlen( buf ); p++ )
			{
        		if ( *p	!= '"' )
				{
				    buf[n] = *p;
        			n++;
				}
			}
        	buf[n] = 0x0;
			strncpy( pszKeyValue, (LPCTSTR)buf, nLen );
			
			ss = TRUE;
			break;
		}
	}
	return ss;
#endif
}


BOOL CEFTParse::GetValueByIndex( int nIdx, LPTSTR pszKeyName, LPTSTR pszKeyValue, int nLen )
{
#ifdef		XML_DEF
	pszKeyValue[0] = 0;
	return false;
#else
	int n;
	BOOL ss;
	char *p, buf[2000] ;

	ss = false;
	if ( nIdx<m_nKeys )
	{
		strcpy( pszKeyName,   (LPTSTR)m_szKeyName[nIdx] );
		strncpy( pszKeyValue, (LPTSTR)m_szKeyValue[nIdx], nLen );
		// Filter for ["]
		strncpy( buf, pszKeyValue, sizeof(buf) );
    	p = buf;
    	n = 0;
     	for ( p = buf; (UINT)(p - buf) < strlen( buf ); p++ )
		{
    		if ( *p	!= '"' )
			{
			    buf[n] = *p;
    			n++;
			}
		}
    	buf[n] = 0x0;
		strncpy( pszKeyValue, (LPCTSTR)buf, nLen );
		ss = true;
	}
	return ss;
#endif
}


void CEFTParse::SetOneValue( LPTSTR pszKeyName, LPTSTR pszKeyValue )
{
#ifdef		XML_DEF
	TCHAR szTemp[256];

	if (gpXMLObj==NULL || m_szCurTask[0]==0 ) return;
	if ( strncmp(pszKeyName,"gv",2)==0 )
	{
		sprintf( szTemp, "Global.%s", pszKeyName );
		gpXMLObj->SetOneValue( szTemp, pszKeyValue, true );
		return;
	}
	else if ( strchr(pszKeyName,'.')!=NULL )
	{
		gpXMLObj->SetOneValue( pszKeyName, pszKeyValue, true );
		return;
	}
	else
	{
		sprintf( szTemp, "%s.%s", m_szCurTask, pszKeyName );
		++m_nKeys;
		gpXMLObj->SetOneValue( szTemp, pszKeyValue, true );
		return;
	}
#else
	int i;
	TCHAR szTemp[8000];

	for (i=0; i<m_nKeys && i<MAXNUMFLDS; i++) 
		if (strcmp( (LPTSTR)m_szKeyName[i], (LPTSTR)pszKeyName)==0)
			break;
	
	if (i<MAXNUMFLDS)
	{
		strncpy((LPTSTR)m_szKeyName[i], (LPTSTR)pszKeyName, 
			sizeof(m_szKeyName[i]));

		if ( m_szKeyValue[i]!=NULL)
		{
			// strncmp(szTemp, m_szKeyValue[i], 8000);
			strncpy(szTemp, m_szKeyValue[i], 8000);
			strncat(szTemp, pszKeyValue, 8000-strlen(pszKeyValue));
		}
		else
			strncpy(szTemp, pszKeyValue, 8000);
		VALUE2BUF( m_szKeyValue[i], szTemp, strlen(szTemp) )
			
		if (strcmp( (LPTSTR)m_szKeyValue[i],"\"\"")==0)
			m_szKeyValue[i][0] = 0;
		if (i==m_nKeys) ++m_nKeys;
	}
#endif
}


////////////////////////////////////////////////////////////////////////


int CEFTParse::ScanTemplate( LPTSTR pszTmplt, LPTSTR *pszNew, int nLevel )
{
	CHAR szKeyword[120], szTemp[120], *pSave, *pCur, *ts, *ps, *pNew, *pNext;
	int nChild, nNext, nLast, p;

	// Skip the white chars
	pCur = pszTmplt;
	while ( pCur[0]==' ' || pCur[0]=='\r' || pCur[0]=='\n' || pCur[0]=='\t' ) ++pCur;

	// Ending Check
	if ( pCur[0]=='}' )
	{
		*pszNew = ++pCur;
		return 0;
	}

	// Read the first word
	if ( pCur[0]!='<' )
		return -1;  // Format Error
	++pCur;	p=0;

	pSave = pCur; ts = szKeyword;
	while (pCur[0] && pCur[0]!='>' && p<120)
	{
		TCHEXPAND(pCur, ts); ++p;
	}
	ts[0] = 0;
	if ( pCur[0]!='>' )
		return -2;
	
	// Ending Check
	if (strncmp( szKeyword, "END", 3)==0)
	{
		*pszNew = NULL;
		return 0;
	}

	//Create the node for this item	and obtain a code
	nLast = m_nLastNode++;
	m_sNode[ nLast ].Keyword = pSave;
	pCur[0]=0; ++pCur;
	// Extend to replace-tree, KeyName can be KeyName=Replace
	ps = strchr(pSave,'=');
	if (ps==NULL) 
		m_sNode[ nLast ].Replace = NULL;
	else
	{
		m_sNode[ nLast ].Replace = ps+1;
		ps[0] = 0;
	}
	m_sNode[ nLast ].child = 0;
	m_sNode[ nLast ].next = 0;
	m_sNode[ nLast ].level = nLevel;

	if (pCur[0]=='{')
	{
		// Get the child structure
		m_sNode[ nLast ].Type = 'N';

		// set the child pointer
		nChild = ScanTemplate( pCur+1, &pNew, nLevel+1 );
		if (nChild<0)
			return nChild;
		else if ( nChild>0 )
			m_sNode[ nLast ].child = nChild;
	}
	else if (pCur[0]=='[')
	{
		// Get the attribute of this item
		m_sNode[ nLast ].Type = 'L';

		// set the child pointer
		pSave = ++pCur; p = 0;
		while (pCur[0] && pCur[0]!=',' && p<120) 
			szTemp[p++]=(pCur++)[0];
		if ( pCur[0]!=',' )
			return -3;
		szTemp[p]=0;
		m_sNode[ m_nLastNode ].Keyword = pSave;
		pCur[0]=0; ++pCur;
		// Extend to replace-tree, KeyName can be KeyName=Replace
		ps = strchr(pSave,'=');
		if (ps==NULL) 
			m_sNode[ m_nLastNode ].Replace = NULL;
		else
		{
			m_sNode[ m_nLastNode ].Replace = ps+1;
			ps[0] = 0;
		}
		m_sNode[ m_nLastNode ].child = 0;
		m_sNode[ m_nLastNode ].next = 0;
		m_sNode[ m_nLastNode ].level = nLevel+1;
		m_sNode[ m_nLastNode ].Type = 'K';
		m_sNode[ nLast ].child = m_nLastNode;
		m_nLastNode++;

		pSave = pCur; p = 0;
		while (pCur[0] && pCur[0]!=']' && p<120) 
			szTemp[p++]=(pCur++)[0];
		if ( pCur[0]!=']' )
			return -4;
		szTemp[p]=0;
		m_sNode[ m_nLastNode ].Keyword = pSave;
		pCur[0]=0; ++pCur;
		m_sNode[ m_nLastNode ].Replace = NULL;
		m_sNode[ m_nLastNode ].child = 0;
		m_sNode[ m_nLastNode ].next = 0;
		m_sNode[ m_nLastNode ].level = nLevel+1;
		m_sNode[ m_nLastNode ].Type = 'T';
		m_sNode[ m_nLastNode-1 ].next = m_nLastNode;
		m_nLastNode++;
		pNew = pCur;
	}
	else
	{
		if (pCur[0]==',') ++pCur;
		pNew = pCur;
	}

	// Expand to next branch
	nNext = ScanTemplate( pNew, &pNext, nLevel );
	if ( nNext<0 )
		return nNext;
	else if ( nNext>0 )
		m_sNode[ nLast ].next = nNext;
	*pszNew = pNext;
	return nLast;
}


BOOL CEFTParse::SetupTemplate( LPTSTR pszTmplt )
{
	LPTSTR pszNew, ps, qs;

	strncpy( m_szTmplt, pszTmplt, sizeof(m_szTmplt) );
	ps = m_szTmplt; qs = ps;

	// Recovery the following 2 for Expanding function, not 100% comfirm
	while (ps[0]) TCHEXPAND(ps,qs);
	qs[0]=0;
	return ( ScanTemplate( m_szTmplt, &pszNew, 1)>=0 );
}


// Keyword is KEYWORD string (Find the match)
int CEFTParse::SearchChildren( LPTSTR pszBuf, int nCur )
{
	int n, p, row, col, nRes;
	LPTSTR ps;

	if ( m_sNode[nCur].Type!='N' ) return 0;
	p = m_sNode[nCur].child;
	nRes = 0;
	while (p>0)
	{
		ps = m_sNode[p].Keyword;
		if (ps[0]=='#' && ps[1]=='[')
		{
			sscanf(ps+2,"%d,%d", &row, &col);
			if (nRow==row && nCol==col)
			{
				nRes = p; break;
			}
			else
				p = m_sNode[p].next;
		}
		else
		{
			n = strlen( ps );
			if (strncmp( pszBuf, ps, n )==0)
			{
				nRes = p; break;
			}
			else
				p = m_sNode[p].next;
		}
	}
	return nRes;
}


LPTSTR CEFTParse::ReplaceText( LPTSTR pzHead, LPTSTR pzTail, LPTSTR pzValue )
{
	TCHAR szValue[120]={0};
	int nRest, nSrc, nDest;

	if (pzValue[0]=='*')
		GetOneValue(pzValue+1,szValue,120);
	else
		strncpy(szValue,pzValue,120);

	nSrc = (pzTail-pzHead); nDest = strlen(szValue);
	nRest = strlen(pzTail);
	memmove(pzHead+nDest, pzTail, nRest);
	memcpy(pzHead, szValue, nDest);
	pzHead[nDest+nRest]=0;
	return pzHead+nDest;
}


void CEFTParse::ScanText( LPTSTR pszText, bool bReplace )
{
	LPTSTR pBuf;
	int i, k, p, j, nn, m, node, nCurp; 
	BOOL ss, ts, do_it;
	TCHAR szTerminator[20], szFldsName[120], szFldsValue[200];
	TCHAR ch, *pos, *pDest, *pSave, szName[40], szTemp[200], szResult[200], szOld[120];
	TCHAR StartPos[20],Length[20],Type[20],SubType[20],KeyName[20];
	TCHAR *ps, *qs;

	//Initialization
	pBuf = pszText;
	for (i=0; i<10; i++) m_state[i]=0;
	k=0; nCurp=0;  do_it=FALSE; nRow=0; nCol=0;

	while ( pBuf[0] )
	{
		// while ( pBuf[0]==' ' || pBuf[0]=='\r' || pBuf[0]=='\n' || pBuf[0]=='\t' ) ++pBuf;
		if (pBuf[0]=='\n') { ++nRow; nCol=0; } else ++nCol;

		ss = FALSE;
		if ( (node = SearchChildren( pBuf, nCurp ))>0 )	 // Search children
		{
			m_state[++k] = node;
			nCurp = node; ss = TRUE;
		}
		else if ( (node = SearchChildren( pBuf, m_state[k-1] ))>0 )	// Search brothers
		{
			m_state[k] = node;
			nCurp = node; ss = TRUE;
		}
		else
		{
			// Search parents and grand parents
			p=k-2;
			while (p>=0)
			{
				node = SearchChildren( pBuf, m_state[p] );
				if ( node>0)
				{
					k=p+1; m_state[k] = node;
					nCurp = node; ss = TRUE; break;
				}
				else
					--p;
			}
		}

		if (ss)
		{
			// Found the node. to see whether need replace
			pSave=pBuf;
			if (m_sNode[nCurp].Keyword[0]!='#') pBuf += strlen( m_sNode[nCurp].Keyword );
			if (bReplace && m_sNode[nCurp].Replace!=NULL)
			{
				// Execute replacement:
				//   Source: [pSave, pBuf-pSave]
				//   Destine: m_sNode[nCurp].Replace
				//	 pBuf needs to be adjusted
				pSave = ReplaceText(pSave, pBuf, m_sNode[nCurp].Replace	);
				if (pSave!=NULL) pBuf = pSave;
			}
			
			if ( m_sNode[nCurp].Type=='L' )
			{
				// Now we are working on the field's value. 
				p = m_sNode[nCurp].child;
				szFldsName[0]=0; pDest=NULL;
				if ( m_sNode[p].Type=='K' )
				{
					strncpy( szFldsName, m_sNode[p].Keyword, sizeof(szFldsName) );
					pDest = m_sNode[p].Replace;
				}

				p = m_sNode[p].next;
				szTerminator[0]=0;
				if ( m_sNode[p].Type=='T' )
					strncpy( szTerminator, m_sNode[p].Keyword, sizeof(szTerminator) );
				
				// Converting Terminator
				ps = szTerminator, qs = szTerminator;
				while (ps[0]) TCHEXPAND(ps,qs);
				qs[0]=0; nn = strlen( szTerminator );

				// Retrieve the value
				i=0; pSave=pBuf;
				if (szTerminator[0]=='*')  // Multiple Terminator
				{
					while ( pBuf[0]==' ' || pBuf[0]=='\t' ) ++pBuf;
					while ( pBuf[0] && (strchr(szTerminator+1, pBuf[0])==NULL)
						    && (i<sizeof(szFldsValue)-1) )
					{
						szFldsValue[i++]=pBuf[0]; pBuf++;
					}
					szFldsValue[i]=0;
				}
				else if (szTerminator[0]=='#')  // Length Limited
				{
					m = atoi( szTerminator+1 );
					if (m<0) m=0;
					while (m>0 && i<sizeof(szFldsValue))
					{
						szFldsValue[i++]=pBuf[0]; pBuf++; --m;
					}
				}
				else  // Single Terminator
				{
					while ( pBuf[0]==' ' || pBuf[0]=='\t' ) ++pBuf;
					while ( pBuf[0] && (strncmp( pBuf, szTerminator, nn )!=0)
						    && (i<sizeof(szFldsValue)-1) )
					{
						szFldsValue[i++]=pBuf[0]; pBuf++;
					}
				}
				szFldsValue[i++]=0;

				// To see whether make a replacement
				if (bReplace && pDest!=NULL)
				{
					// Execute replacement:
					//   Source: [pSave, pBuf-pSave]
					//   Destine: pDest
					//	 pBuf needs to be adjusted 
					pSave = ReplaceText(pSave, pBuf, pDest);
					if (pSave!=NULL) pBuf = pSave;
				}

				// if no szFldsName, then replacemeny only, no extraction
				if ( szFldsName[0] )  
				{
					// Removal of <*...>
					m = i; ts = true;
					for (i=0,j=0; i<m; i++)
					{
						if ( ts && szFldsValue[i]=='<' && szFldsValue[i+1]=='*' ) ts=false;
						if ( ts ) szFldsValue[j++] = szFldsValue[i];
						if ( !ts && szFldsValue[i]=='>' ) ts = true;
					}
					szFldsValue[j]=0;
			
					// szFldsName ::= <Name>|..|<Name> 
					pos = szFldsName;
					while ( pos[0] ) 
					{
						ch = 0; szName[0]=0;
						sscanf( pos, "%[^|]%c", szName, &ch );
						pos += strlen(szName)+(ch?1:0);

						// szName can be <Name>:<Type>(1)<SubType>(1)<Length>(4)<StartPos>
						if (strchr(	szName, ':' )!=NULL)
						{
							KeyName[0]=0; Type[0]=0; SubType[0]=0; Length[0]=0; StartPos[0]=0;
							sscanf( szName, "%[^:]%*c%c%c%4s%s", 
									KeyName, Type, SubType, Length, StartPos );
							Type[1]=0; SubType[1]=0;
							if ( Type[0]!=' ' )
							{
								strncpy( szTemp, szFldsValue, sizeof(szTemp) );
								FormatProcess( szResult, szTemp, KeyName, 
										Type, SubType, Length, StartPos );
							}
							else
								strncpy( szResult, szFldsValue, sizeof(szResult) );
							SETVALUE( KeyName, szResult, szOld )
							// SetOneValue( KeyName, szResult );
						}
						else
						{
							SETVALUE( szName, szFldsValue, szOld )
							// SetOneValue( szName, szFldsValue );
						}
					}
				}
			}
		}
		else
			++pBuf;
	}
}


int CEFTParse::SearchNode( LPTSTR pszBuf, int nCur )
{
	int n, p, nRes;

	n = strlen( m_sNode[nCur].Keyword );
	if (strncmp( pszBuf, m_sNode[nCur].Keyword, n )==0)
		return nCur;

	p = m_sNode[nCur].child;
	if (p==0) return 0;
	nRes = SearchNode( pszBuf, p );
	if ( nRes>0 ) return nRes;

	p = m_sNode[nCur].next;
	if (p==0) return 0;
	nRes = SearchNode( pszBuf, p );
	return nRes;
}


//
// Search the Title, In the tree, 
//  <#TITLE> node defines a parse scheme from the Title, under this node.
//    <#SSCANF> gives the Format String 
//    <#nn>  gives the definition of the Item <nn>
//  
// szFldsName ::= <szName>|..|<szName> 
// szName can be <Name>:<Type>(1)<SubType>(1)<Length>(4)<StartPos>


BOOL CEFTParse::ScanTitle( LPTSTR pszText )
{
	int i, p, nn, m, nCurp;
	CHAR szItem[20][40], szFmt[120], szFldsValue[120], szFldsName[120], szOld[120];
	CHAR ch, *pos, szName[40], szTemp[200], szResult[200];
	CHAR StartPos[20],Length[20],Type[20],SubType[20],KeyName[20];

	//Initialization
	for (i=0; i<20; i++) szItem[i][0]=0;

	nCurp = 0;
	nn = SearchNode( "#TITLE", nCurp );
	if (nn==0 && strcmp(m_sNode[nCurp].Keyword, "#TITLE")) goto SrchConst;

	nCurp = nn;
	nn = SearchNode( "#SSCANF", nCurp );
	if (nn==0) return 0;
	p = m_sNode[nn].child;
	if ( p==0 && m_sNode[p].Type!='K' ) return 0;
	strncpy( szFmt, m_sNode[p].Keyword, sizeof(szFmt) );
	sscanf( pszText, szFmt, szItem[0], szItem[1], szItem[2], szItem[3], szItem[4],   
							szItem[5], szItem[6], szItem[7], szItem[8], szItem[9], 
							szItem[10], szItem[11], szItem[12], szItem[13], szItem[14], 
							szItem[15], szItem[16], szItem[17], szItem[18], szItem[19]); 

	p = m_sNode[nCurp].child;
	while ( p>0 )				    
	{
		// while ( pBuf[0]==' ' || pBuf[0]=='\r' || pBuf[0]=='\n' || pBuf[0]=='\t' ) ++pBuf;
		strncpy( szFmt, m_sNode[p].Keyword, sizeof(szFmt) );
		if (strcmp( szFmt, "#SSCANF"))
		{
			nn = atoi(szFmt+1);
			if (nn>=0 && nn<20)
			{
				szFldsValue[0]=0;
				strncpy(szFldsValue, szItem[nn], sizeof(szFldsValue));
			
				// Now we are working on the field's value. 
				m = m_sNode[p].child;
				szFldsName[0]=0;
				if ( m_sNode[m].Type=='K' )
					strncpy( szFldsName, m_sNode[m].Keyword, sizeof(szFldsName) );

				// szFldsName ::= <Name>|..|<Name> 
				pos = szFldsName;
				while ( pos[0] ) 
				{
					ch = 0; szName[0]=0;
					sscanf( pos, "%[^|]%c", szName, &ch );
					pos += strlen(szName)+(ch?1:0);

					// szName can be <Name>:<Type>(1)<SubType>(1)<Length>(4)<StartPos>
					if (strchr(	szName, ':' )!=NULL)
					{
						KeyName[0]=0; Type[0]=0; SubType[0]=0; Length[0]=0; StartPos[0]=0;
						sscanf( szName, "%[^:]%*c%c%c%4s%s", 
								KeyName, Type, SubType, Length, StartPos );
						strncpy( szTemp, szFldsValue, sizeof(szTemp) );
						FormatProcess( szResult, szTemp, KeyName, 
								Type, SubType, Length, StartPos );

						SETVALUE( KeyName, szResult, szOld )
						// SetOneValue( KeyName, szResult );
					}
					else
					{
						SETVALUE( szName, szFldsValue, szOld )
						// SetOneValue( szName, szFldsValue );
					}
				}
			}
		}
		p = m_sNode[p].next;
	}

SrchConst:
	nCurp = 0;
	nn = SearchNode( "#CONST", nCurp );
	if (nn==0 && strcmp(m_sNode[nCurp].Keyword, "#CONST")) return 0;
	p = nn;
	while ( p>0 )				    
	{
		// while ( pBuf[0]==' ' || pBuf[0]=='\r' || pBuf[0]=='\n' || pBuf[0]=='\t' ) ++pBuf;
		strncpy( szFmt, m_sNode[p].Keyword, sizeof(szFmt) );
		if (strcmp( szFmt, "#CONST")==0)
		{
			
			// Now we are working on the field's value. 
			m = m_sNode[p].child;
			szFldsName[0]=0;
			if ( m_sNode[m].Type=='K' )
				strncpy( szFldsName, m_sNode[m].Keyword, sizeof(szFldsName) );

			m = m_sNode[m].next;
			szFldsValue[0]=0;
			if ( m_sNode[m].Type=='T' )
				strncpy( szFldsValue, m_sNode[m].Keyword, sizeof(szFldsValue) );
			
			// szFldsName ::= <szName>|..|<szName> 
			pos = szFldsName;
			while ( pos[0] ) 
			{
				ch = 0; szName[0]=0;
				sscanf( pos, "%[^|]%c", szName, &ch );
				pos += strlen(szName)+(ch?1:0);

				// szName can be <Name>:<Type>(1)<SubType>(1)<Length>(4)<StartPos>
				if (strchr(	szName, ':' )!=NULL)
				{
					KeyName[0]=0; Type[0]=0; SubType[0]=0; Length[0]=0; StartPos[0]=0;
					sscanf( szName, "%[^:]%*c%c%c%4s%s", 
							KeyName, Type, SubType, Length, StartPos );
					strncpy( szTemp, szFldsValue, sizeof(szTemp) );
					if ( StartPos[0]==0 && Type[0]=='V')
					{
						strncpy( StartPos, szTemp, sizeof(StartPos));
						sprintf( Length, "%02d", strlen(StartPos) );
					}
					FormatProcess( szResult, szTemp, KeyName, 
							Type, SubType, Length, StartPos );

					SETVALUE( KeyName, szResult, szOld )
					// SetOneValue( KeyName, szResult );
				}
				else
				{
					SETVALUE( KeyName, szResult, szOld )
					// SetOneValue( szName, szFldsValue );
				}
			}
		}
		p = m_sNode[p].next;
	}

	return 1;
}


BOOL CEFTParse::ParseOneFile( LPTSTR pszTmplt, LPTSTR pszFile, LPTSTR pzBuf, int nLmt )
{
	TCHAR *pzTmp, *psTmp;
	FILE *fp;
	int k, nLen, nTmp;

	int nn = SetupTemplate( pszTmplt );
	if ( nn )
	{
		fp=fopen(pszFile,"rb");
		if (fp == NULL) return false;
		fseek( fp, 0L, SEEK_END );
		nLen = ftell( fp );
		pzTmp = new TCHAR[nLen+2000];
		if (pzTmp==NULL)
		{
			fclose( fp );
			return false;
		}
		fseek( fp, 0L, SEEK_SET );
		nTmp = fread( pzTmp, sizeof(TCHAR), nLen+1, fp );
		pzTmp[nTmp] = 0;
		fclose( fp );

		// Need to be updated
		ScanText( pzTmp );
		psTmp = strrchr( pszFile, '\\' );
		if ( psTmp!=NULL) ScanTitle( psTmp+1 );
		else ScanTitle( pszFile );

		if ( pzBuf!=NULL )
		{
			nLmt = (nLmt>0?nLmt-1:0);
			k = strlen(pzTmp);
			if (k>nLmt) k = nLmt;
			strncpy( pzBuf, pzTmp, k);
			pzBuf[k] = 0;
		}

		delete pzTmp;
		return true;
	}
	else
		return false;
}


/////////////////////////////////////////////////////////////////////////////////
// ***************************** Public Subroutine ***************************
/////////////////////////////////////////////////////////////////////////////////
//
#define	ADDMI(x,y)	if(x[0]) {strcat(x," ");strcat(x,y);} else strncpy(x,y,39-strlen(x));
//
//
BOOL ParseName(char *NBuf, char *pszLast, char *pszFirst, char *pszMid, char *pszTitle, char *pszCode )
{
	char *tp, *ps, *cbuf, szWord[100], Buf[100], Bufsv[100];
	char DocTl[40], DocLast[40], DocFirst[40], DocMI[40], DCode[20];
	int i, ns, state;
	bool ss = false;

	strncpy(Buf,NBuf,99);
	strncpy(Bufsv, Buf, 99);
	Buf[99]=0;
	Bufsv[99]=0;
	DocFirst[0]=0;
	DocLast[0]=0; DocLast[39]=0;
	DocMI[0]=0;	DocMI[39]=0;
	DocTl[0]=0;
	DCode[0]=0;
	state=0;

	// Check the general format -> Last, First or First Last
	tp = strchr( Buf, ',' );
	if (tp!=NULL )
	{
		++tp; while ( tp[0]==' ' || tp[0]=='\t' ) ++tp;
		cbuf = szWord; cbuf[0]=0;
		while ( tp[0] && tp[0]>' ' ) (cbuf++)[0]=(tp++)[0];
		cbuf[0]=0;
		ns = ParseWord(szWord);
		if ( ns==0||ns==1||ns==2 ) ss = true;
	}

/* 0:Normal Name, 1: Last, 2:X., 3:X.X. ..., 4:Jr. ..., 5: Code, 6: Other */
	tp = Buf;
	ps = tp; i = 0;
	while ( ps[0] )
	{
		if (isdigit(ps[0])) DCode[i++]=ps[0]; 
		if ( (ps[0]>='0' && ps[0]<='9' && ps[-1]!='#' && ps[-1]!='-') || ps[0]=='[' || ps[0]==']' ||
			ps[0]=='(' || ps[0]==')' || ps[0]=='/' ) ps[0]=' ';
		++ps;
	}
	DCode[i]=0;

	while ( tp[0]==' ' || tp[0]=='\t' ) ++tp;
	if ( (isalpha(tp[0])) && (strlen(tp)<=99) ) {
	
		while (1) {
			if (tp[0]==0 || tp[0]=='\n') break;
			cbuf = szWord; cbuf[0]=0;
			while (tp[0]!=',' && tp[0]!=' ' && tp[0]!='/' && tp[0]!='\t' && tp[0]!='\n' && tp[0]!=0) 
				(cbuf++)[0]=(tp++)[0];
			if (tp[0]==',') (cbuf++)[0]=(tp++)[0];
			cbuf[0]=0;
			cbuf = szWord;

			while ( cbuf[0] )
				if ( cbuf[0]=='(' || cbuf[0]=='[' )
				{
					cbuf[0]=0; break;
				}
				else
					++cbuf;
			cbuf = szWord;
			while ( tp[0]==' ' || tp[0]=='\t' ) ++tp;
			//if ( (cbuf = strtok(tp," \n"))==NULL) break;
			ns = ParseWord(cbuf);
	
			switch (state) {
			case 0:   /* Initial State */
			  if (ns==0) 
			  { 
				  if (ss) { state=2; strcpy(DocLast, cbuf); break; }  
				  else { state=1; strcpy(DocFirst,cbuf); break; }
			  }
			  if (ns==1) { state=2; ss=false; strcpy(DocLast,cbuf); break; }
			  if (ns==2) { state=7; strcpy(DocFirst,cbuf); break; }
			  if (ns==3) { ADDMI(DocTl,cbuf); break; }
			  if (ns==4||ns==6||ns==5) { state=5; break; }
			  break;
/* 0:Normal Name, 1: Last, 2:X., 3:X.X. ..., 4:Jr. ..., 5: Code, 6: Other */
			case 1:  /* First */
			  if (ns==0) 
			  { 
				state=6; strcpy(DocLast,cbuf); break;   
			  }
			  if (ns==1) 
			  { 
				/* if (SPLAST(DocFirst))
				{
				  strcpy(DocLast, DocFirst); strcat(DocLast," ");
				  strcat(DocLast, cbuf); DocFirst[0]=0; state=2; break;
				}
				else */{ state=3; strcpy(DocLast,cbuf); break; } 
			  }
			  if (ns==2) { ADDMI(DocMI,cbuf); break; }
			  if (ns==4||ns==3) { strcpy(DocLast, DocFirst); strcat(DocLast," ");
				       strcat(DocLast,cbuf); state=2; break; }
			  if (ns==6) { state=5; break; }
			  break;
			case 2: /* Last */
			  if (ns==0) { 
				  if ( ss ) { strcat(DocLast," "); strcat(DocLast, cbuf); break; }
				  else { state=4; strcpy(DocFirst, cbuf); break; }
			  }
			  if (ns==1) {
				  if ( ss ) { strcat(DocLast," "); strcat(DocLast, cbuf);  ss = false; break; }
				  else { state=4; strcpy(DocFirst, cbuf);break; }
			  }
			  if (ns==2) { state=10; strcpy(DocFirst, cbuf); break; }
			  if (ns==3) { ADDMI(DocTl,cbuf); ss = false; break; }
			  if (ns==4) { strcat(DocLast," "); strcat(DocLast, cbuf); ss = false; break; }
			  if (ns==6) { state=5; break; }
			  break;
			
			case 3: /* F L */
			  if ( ns==0||ns==2||ns==1 ) 
			  { 
				 if ( ns=0 /*SPLAST(DocFirst)*/ )
				 {	/* Change to <L1 L2, F> */
					 state=4; strcat(DocFirst," "); strcat(DocFirst,DocLast);
					 strcpy(DocLast, DocFirst); strcpy(DocFirst, cbuf);
				 }
				 else
					 state=5;
				 break;
			  }
			  if (ns==4) { strcat(DocLast," "); strcat(DocLast, cbuf); break; }
			  if (ns==6) { state=5; break; }
			  // if (ns==4) { strcat(DocLast," "); strcat(DocLast,cbuf); break; }
			  if (ns==3) { state=11; ADDMI(DocTl,cbuf); break; }
			  break;
			case 4: /* L, F */
			  if (ns==6) { state=5; break; }
			  if (ns==3) { state=11; ADDMI(DocTl,cbuf); break; }
			  if (ns==1) { strcpy(DocMI, cbuf); break; }
			  if (ns==2||ns==0) { ADDMI(DocMI,cbuf); break; }
			  break;
			case 5:
			  break;
			case 6: /* F L? */
			  if (ns==3) { ADDMI(DocTl,cbuf); state=11; break; }
			  if (ns==0||ns==1) {
				if ( ss /*SPLAST(DocLast)*/)
				{
					strcat(DocLast," "); strcat(DocLast,cbuf); 
					if (ns==1) state=3;	break;
				}
				else
				{
				    strcpy(DocMI, DocLast); strcpy(DocLast, cbuf); state=3; break;
				}
			  }
			  if (ns==4) { strcat(DocLast," "); strcat(DocLast, cbuf); state=3; break; }
			  break;
			case 7: /* F. */
			  if (ns==3) { ADDMI(DocTl,cbuf); break; }
			  if (ns==1) {
				strcpy(DocLast, cbuf); state=3; break;
			  }
			  if (ns==0) {
				strcpy(DocLast, cbuf); state=8; break;
			  }
			  if (ns==4) { state=9; ADDMI(DocMI,cbuf); break; }
			  break;
			case 8: /* F. L? */
			  if (ns==3) { ADDMI(DocTl,cbuf); break; }
			  if (ns==4) { strcat(DocLast," "); strncat(DocLast, cbuf, 39-strlen(DocLast)); break; }
			  if (ns==0||ns==1) {
		        strcpy(DocMI, DocLast); strcpy(DocLast, cbuf); state=3; break;
			  }
			  break;
			case 9: /* F. M. */
			  if (ns==3) { ADDMI(DocTl,cbuf); break; }
	    	  if (ns==0||ns==1) {
			    strcpy(DocLast, cbuf); state=3; break;
			  }
			  break;
			case 10: /* L, F. */
			  if (ns==3) { ADDMI(DocTl,cbuf); break; }
			  if (ns==1) { cbuf[strlen(cbuf)]=0; strcpy(DocMI, cbuf); break; }
			  if (ns==2||ns==0) { ADDMI(DocMI,cbuf); break; }
			  break;
			case 11: /* After Title */
			  if (ns==3) { ADDMI(DocTl,cbuf); break; }
			  break;
			default:
			  break;
			}
		} 
	}
	
/* Name Distinguish */
	tp = strchr( DocLast, ',');
	if (tp!=NULL) tp[0]=0;
	strcpy( pszLast, DocLast);
	strcpy( pszFirst, DocFirst);
	strcpy( pszMid, DocMI);
	strcpy( pszTitle, DocTl);
	if ( pszCode!=NULL ) strcpy( pszCode, DCode );
	return TRUE;
}


int ParseWord(char* cbuf)
{
/* 0:Normal Name, 1: Last, 2:X., 3:X.X. ..., 4:Jr. ..., 5: Code, 6: Other */
  int p, i;
  bool ss;
  char tbuf[100], *ps;

  p=strlen(cbuf);
  // strupr(cbuf);
  ss = FALSE;
  // Title may take M.D.,PhD,... format
  // ps = strchr(cbuf,',');
  // if (ps!=NULL && ps[1]!=0) return 3;
  if (p>0 && cbuf[p-1]==',') { cbuf[--p]=0; ss = TRUE; }
  if (p==1) return 2; 
  if ((p==2) && (cbuf[1]=='.')) return 2;
  if ((p==4)&&(cbuf[1]=='.')&&(cbuf[3]=='.')) return 3;
  if ((p==6)&&(cbuf[1]=='.')&&(cbuf[3]=='.')&&(cbuf[5]=='.')) return 3;
  if ((p==8)&&(cbuf[1]=='.')&&(cbuf[3]=='.')&&(cbuf[5]=='.')&&(cbuf[7]=='.')) return 3;
  if ((p==5)&&(cbuf[2]=='.')&&(cbuf[4]=='.')) return 3;
  if ((p==4)&&(cbuf[2]=='.')) return 3;
  if ((p>3) && cbuf[p-1]=='R' && cbuf[p-2]=='-' ) return 3;

  ps = tbuf;
  for (i=0; i<p; i++) 
	  if (cbuf[i]!='.') (ps++)[0]=cbuf[i];
  ps[0]=0;

  strupr(tbuf);
  if ((strncmp("JR",tbuf,p)==0)||(strncmp(tbuf,"SR",p)==0)) return 4;
  if ((strncmp("III",tbuf,p)==0)||(strncmp(tbuf,"II",p)==0)) return 4;
  if ((strncmp("IV",tbuf,p)==0)||(strncmp(tbuf,"V",p)==0)) return 4;
  if ((strncmp("VI",tbuf,p)==0)||(strncmp(tbuf,"VII",p)==0)) return 4;
  if ((strncmp("VIII",tbuf,p)==0)||(strncmp(tbuf,"IX",p)==0)) return 4;
  

  // Search predefined Titles
  for (i=0; i<MAX_TITLE; i++)
  {
	  if (strlen(gTitleList[i])==(unsigned)p && strcmp(gTitleList[i], tbuf)==0) return 3;
	  if (gTitleList[i][0]==0) break;
  }

  if (((cbuf[0]=='{')&&(cbuf[p-1]=='}')) || ((cbuf[0]=='[')&&(cbuf[p-1]==']'))) {
    if (isdigit(cbuf[1])) { cbuf[0]=' '; cbuf[p-1]='\0'; return 5; }
    else return 6;
  }
  if (isdigit(cbuf[0]) && (isdigit(cbuf[p-1]))) return 5;
  if (ss) { cbuf[p]=0; return 1; }
  else return 0;

}


long ParseCOleDateTime(char *inBuf, char *outBuf)
{
	COleDateTime tx;
	tx = *(COleDateTime*)inBuf;
	strcpy( outBuf, tx.Format("%Y%m%d%H%M%S"));
	return 1;
}


// nDMY=4: yyyy/mm/dd, nDMY=1: dd/mm/yyyy, nDMY=2: return the age, nDMY=3: use system time;
// Format: mm/dd/yyyy|yyyymmddHHMMSS|mm-dd-yyyy|Mon dd, yy|dd-Mon-yy|
//
long ParseDateTime(char *inBuf, char *outBuf, int nDMY )
{
	char *ps1, *ps2, szTemp[30], s1[30], s2[30], s3[30], s4[30], s5[30], s6[30], ch;
	int  k,d1,d2,d3,t1,t2,t3;
	long nTime;
	char szMonName[12][5]={ "JAN","FEB","MAR","APR","MAY","JUN",
				"JUL","AUG","SEP","OCT","NOV","DEC" };
	CTime ts;
	CTime tc = CTime::GetCurrentTime();
	int nCY = (tc.GetYear()-2000);
	
	strncpy( szTemp, inBuf, 30 );
	ps1 = inBuf;
	ps1[29] = 0;

	if (ps1==NULL)
	{
		ts = CTime::GetCurrentTime();
		if (outBuf!=NULL) strcpy( outBuf, ts.Format( "%Y%m%d%H%M%S" ));
		return ts.GetTime();
	}
	if (outBuf!=NULL) outBuf[0]=0;

	if ( nDMY==3 )
	{
		nTime = atol(inBuf);
		ts = nTime;
		if (outBuf!=NULL) strcpy(outBuf, ts.Format("%Y%m%d%H%M%S"));
		return nTime;
	}

	d1=0; d2=0; d3=0;
	t1=0; t2=0; t3=0;

	while (ps1[0]==' '||ps1[0]=='\t') ++ps1;
	strupr( ps1 );
	if ( strcmp( ps1, "(NULL)" )==0 || strlen(ps1)<5 ) return 0;

	for (k=0; k<12; k++)
	{
		if (strstr(ps1, szMonName[k])!=NULL) break;
		//ss = strncmp(ps1, szMonName[k], 3);
		//if ( !ss ) break;
	}

	if ( k < 12 )
	{
		s1[0]=0; s2[0]=0; s3[0]=0;
		sscanf(ps1,"%[^ -/]%*c%[^ -/]%*c%s", s1, s2, s3);
		ps1+=(strlen(s1)+1+strlen(s2)+1+strlen(s3));
		if (s1[0]>='A')
			sprintf(s1, "%d", k+1);
		else
		{
			strcpy(s2, s1);
			sprintf(s1, "%d", k+1);
		}
	}
	else if ((ps2=strchr(ps1,'/'))!=NULL && strchr(ps2,'/')!=NULL)
	{
		s1[0]=0; s2[0]=0; s3[0]=0;
		if ( nDMY==1 )
			sscanf(ps1, "%[^/]%*c%[^/]%*c%[0-9]", s2, s1, s3);
		else
		{
			sscanf(ps1, "%[^/]%*c%[^/]%*c%[0-9]", s1, s2, s3);
			if ( atoi(s1)>12 )
			{
				strcpy(s5, s3); strcpy(s3, s1); strcpy(s1, s2); strcpy(s2, s5);
			}
		}
		ps1+=(strlen(s1)+1+strlen(s2)+1+strlen(s3));
	}
	else if ((ps2=strchr(ps1,'-'))!=NULL && strchr(ps2,'-')!=NULL)
	{
		s1[0]=0; s2[0]=0; s3[0]=0;
		if ( nDMY==1 )
			sscanf(ps1, "%[^-]%*c%[^-]%*c%[0-9]", s2, s1, s3);
		else
		{
			sscanf(ps1, "%[^-]%*c%[^-]%*c%[0-9]", s1, s2, s3);
			if ( atoi(s1)>12 )
			{
				strcpy(s5, s3); strcpy(s3, s1); strcpy(s1, s2); strcpy(s2, s5);
			}
		}
		ps1+=(strlen(s1)+1+strlen(s2)+1+strlen(s3));
	}
	else if (strspn(ps1,"0123456789")==5)
	{
		strncpy(s1, ps1, 1); s1[1]=0;
		strncpy(s2, ps1+1,2); s2[2]=0;
		strncpy(s3, ps1+3,2); s3[2]=0;
		ps1+=5;
	}
	else if (strspn(ps1,"0123456789")==6)
	{
		if (strncmp(ps1,"00",2)==0 || strncmp(ps1,"13",2)>0)
		{
			strncpy(s3, ps1, 2);  s3[2]=0;
			strncpy(s1, ps1+2,2); s1[2]=0;
			strncpy(s2, ps1+4,2); s2[2]=0;
		}
		else
		{
			strncpy(s1, ps1, 2);  s1[2]=0;
			strncpy(s2, ps1+2,2); s2[2]=0;
			strncpy(s3, ps1+4,2); s3[2]=0;
		}
		ps1+=6;
	}
	else if (strspn(ps1,"0123456789")==7)
	{
		strncpy(s1, ps1, 1);  s1[1]=0;
		strncpy(s2, ps1+1,2); s2[2]=0;
		strncpy(s3, ps1+3,4); s3[4]=0;
		ps1+=7;
	}
	else if (strspn(ps1,"0123456789")==8)
	{
		if (strncmp(ps1,"20",2)==0 || strncmp(ps1,"19",2)==0)
		{
			strncpy(s3, ps1, 4);  s3[4]=0;
			strncpy(s1, ps1+4,2); s1[2]=0;
			strncpy(s2, ps1+6,2); s2[2]=0;
		}
		else
		{
			strncpy(s1, ps1, 2);  s1[2]=0;
			strncpy(s2, ps1+2,2); s2[2]=0;
			strncpy(s3, ps1+4,4); s3[4]=0;
		}
		ps1+=8;
	}
	else if (strspn(ps1,"0123456789")>8)
	{
		if (strncmp(ps1,"20",2)==0 || strncmp(ps1,"19",2)==0)
		{
			strncpy(s3, ps1, 4);  s3[4]=0;
			strncpy(s1, ps1+4,2); s1[2]=0;
			strncpy(s2, ps1+6,2); s2[2]=0;
			strncpy(s4, ps1+8,2); s4[2]=0;
			if (strspn(ps1,"0123456789")>10)
				strncpy(s5, ps1+10,2); s5[2]=0;
			if (strspn(ps1,"0123456789")>12)
				strncpy(s5, ps1+12,2); s6[2]=0;
		}
		else
		{
			strncpy(s3, ps1, 2);  s3[2]=0;
			strncpy(s1, ps1+2,2); s1[2]=0;
			strncpy(s2, ps1+4,2); s2[2]=0;
			strncpy(s4, ps1+6,2); s4[2]=0;
			strncpy(s5, ps1+8,2); s5[2]=0;
		}
		ps1+= strspn(ps1,"0123456789");
	}

	if ( nDMY == 1 )
	{
		d1 = atoi(s1); d2 = atoi(s2); d3 = atoi(s3);
	}
	else if ( nDMY == 4 )
	{
		d3 = atoi(s1); d1 = atoi(s2); d2 = atoi(s3);
	}
	else
	{
		s1[4]=0; s3[4]=0; 
		d1=atoi(s1);
		if (d1>31) { d3=atoi(s1); d1=atoi(s2); d2=atoi(s3); }
		else { d2=atoi(s2); d3=atoi(s3); }
	}

	if (d3>=0 && d3<=nCY) d3+=2000;
	else if (d3>nCY && d3<=99) d3+=1900;

	while (ps1[0] && (ps1[0]==' '||ps1[0]=='\t')) ++ps1;
	if ( strchr(ps1,':')!=NULL )
	{
		ch = 0;
		s4[0]=0; s5[0]=0; s6[0]=0;
		sscanf(ps1,"%[^:]%*c%[0-9]%[^a-zA-Z]%c", s4, s5, s6, &ch);

		if ( s6[0]==':' ) s6[0]='0';
		if ( ch=='a' || ch=='A' )
		{
			if (atoi(s4)==12) strcpy(s4,"00"); 
		}
		else if ( ch=='p' || ch=='P' )
		{
			if (atoi(s4)<12) 
				sprintf(s4,"%02d",atoi(s4)+12);
		}
	}
	else if (strspn(ps1,"0123456789")>=4)
	{
		strncpy(s4, ps1, 2); s4[2]=0;
		strncpy(s5, ps1+2, 2); s5[2]=0;
	}

	t1=atoi(s4); t2=atoi(s5); t3=atoi(s6);

	if ( (d1>0 && d1<=12) && (d2>0 && d2<=31) && (d3>=1900 || d3<=2100) &&
		 (t1>=0 && t1<=24 ) && (t2>=0 && t2<60) )
	{
		if (nDMY & 2)
		{
			int age;
			age = tc.GetYear()-d3;
			if ( (tc.GetMonth() < d1) || ( (tc.GetMonth()==d1) && (tc.GetDay() < d2) ) ) --age;
			if ( age>0 ) sprintf( outBuf, "%dY", age );
			else
			{
				age = tc.GetMonth()-d1;
				if ( age<0 ) age = 12+age;
				if ( age==0 && 	tc.GetYear()>d3 ) age = 11;
				sprintf( outBuf, "%dM", age );
			}

			/* COleDateTime tx( d3, d1, d2, 0, 0, 0 );
			COleDateTime dt = COleDateTime::GetCurrentTime();
			COleDateTimeSpan ts = dt - tx;
			if (outBuf!=NULL)
			{
				if ( ts.GetDays()>=365 )
					sprintf( outBuf, "%dY", ts.GetDays()/365 );
				else if ( ts.GetDays()>=30 )
					sprintf( outBuf, "%dM", ts.GetDays()/30 );
				else
					sprintf( outBuf, "%dD", ts.GetDays() );
				return ts.GetDays();
			} */
			return 1;
		}
		else
		{
			if (outBuf!=NULL) sprintf(outBuf,"%04d%02d%02d%02d%02d", d3, d1, d2, t1, t2);
		
			if (d3>=1970)
			{
				CTime ct(d3,d1,d2,t1,t2,t3);
				return ct.GetTime();
			}
			else 
				return 1;
		}
	}
	else
		return 0;
}

long AdjustTime(time_t inTm, char *outBuf, char *adjustExp )
{
	long nAdj = atoi(adjustExp);
	
	if ( strchr(adjustExp,'H')!=NULL ) nAdj *= 3600;
	else if ( strchr(adjustExp,'M')!=NULL ) nAdj *= 60;
	time_t nTmp = inTm + nAdj;
	CTime ct = nTmp;
	strcpy( outBuf, ct.Format( "%Y%m%d%H%M%S" ) );
	return nTmp;
}


int GetCTimeStr( char* pszBuf, char* pszFmt, char* pszVal )
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
	else if ( szTmp[4]=='-' || szTmp[4]=='/' )
	{
		pDt = new COleDateTime( (time_t)ParseDateTime( szTmp, NULL ) ); 
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

	/*
	if (strchr( szTmp, '/')!=NULL || strchr( szTmp, '-')!=NULL)
	{	
		szT1[0]=0; szT2[0]=0; szT3[0]=0; szT4[0]=0; szT5[0]=0; 
		sscanf( szTmp, "%[^/-]%*c%[^/-]%*c%[^ ]%*c%[^:]%*c%[^ ]%*[ ]%c",
			szT2, szT3, szT1, szT4, szT5, &ch );
		if ( (ps=strchr(szT5,':'))!=NULL ) x.tm_sec = atoi(ps+1); else x.tm_sec = 0;
		x.tm_min = atoi( szT5 );
		x.tm_hour= atoi( szT4 );
		if ( (ch=='p' || ch=='P') && x.tm_hour<12 ) x.tm_hour += 12;
		x.tm_mday= atoi( szT3 );
		x.tm_mon = atoi( szT2 )-1;
		x.tm_year= (atoi( szT1 )>100? atoi(szT1)-1900:atoi(szT1)+100);
	}
	else if ( szTmp[0]>='A' && szTmp[0]<='Z' )
	{
		szT1[0]=0; szT2[0]=0; szT3[0]=0; szT4[0]=0; szT5[0]=0;
		sscanf( szTmp, "%[^ ]%[ ]%[^ ]%[ ]%[^ ]%[ ]%[^:]%*c%[^ ]%*[ ]%c",
			szT2, szT0, szT3, szT0, szT1, szT0, szT4, szT5, &ch);
		x.tm_sec = 0;
		if (!strnicmp( szT2, "Jan", 3)) x.tm_mon=0;
		else if (!strnicmp( szT2, "Feb", 3)) x.tm_mon=1;
		else if (!strnicmp( szT2, "Mar", 3)) x.tm_mon=2;
		else if (!strnicmp( szT2, "Apr", 3)) x.tm_mon=3;
		else if (!strnicmp( szT2, "May", 3)) x.tm_mon=4;
		else if (!strnicmp( szT2, "Jun", 3)) x.tm_mon=5;
		else if (!strnicmp( szT2, "Jul", 3)) x.tm_mon=6;
		else if (!strnicmp( szT2, "Aug", 3)) x.tm_mon=7;
		else if (!strnicmp( szT2, "Sep", 3)) x.tm_mon=8;
		else if (!strnicmp( szT2, "Oct", 3)) x.tm_mon=9;
		else if (!strnicmp( szT2, "Nov", 3)) x.tm_mon=10;
		else if (!strnicmp( szT2, "Dec", 3)) x.tm_mon=11;
		else 
		{
			pszBuf[0]=0;
			return 0;
		}
		if ( (ps=strchr(szT5,':'))!=NULL ) x.tm_sec = atoi(ps+1); else x.tm_sec = 0;
		x.tm_min = atoi( szT5 );
		x.tm_hour= atoi( szT4 );
		if ( (ch=='p' || ch=='P') && x.tm_hour<12 ) x.tm_hour += 12;
		x.tm_mday= atoi( szT3 );
		x.tm_year= (atoi( szT1 )>100? atoi(szT1)-1900:atoi(szT1)+100);
	}
	else if (strspn(szTmp,"0123456789")==strlen(szTmp))
	{
		// Looking for a system time
		strncpy( szT0, szTmp, 4); szT0[4]=0;
		if (strlen(szTmp)==9 || (strlen(szTmp)==10 && atoi(szT0)<1900) )
		{
			t = atol(szTmp);
		}

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
			x.tm_mon = atoi( szTmp+4)-1;
		else 
			x.tm_mon = 0;
		szTmp[4]=0;
		if (strlen( szTmp )>0) 
			x.tm_year = atoi( szTmp)-1900;
		else 
			x.tm_year = 00;
	}
	else // illegal
	{
		pszBuf[0]=0;
		return 0;
	}

	if (t==0) t = mktime( &x );
	else x = *localtime( &t );

	if (t<=0)
	{
		if ( x.tm_mon>=0 && x.tm_mon<12 && x.tm_mday>0 && x.tm_hour<=31 )
		{
			COleDateTime tx( x.tm_year+1900, x.tm_mon+1, x.tm_mday, 0, 0, 0 );
			strncpy( pszBuf, tx.Format(pszFmt), 100 );
			if (strncmp(pszBuf,"Invalid", 7)==0) pszBuf[0]=0;
			return strlen( pszBuf );
		}
		else
		{
			pszBuf[0]=0;
			return 0;
		}
	}
	else
	{
		// x = *localtime( &t );
		if (stricmp(pszFmt,"System")==0)
			sprintf(pszBuf, "%lu", t);
		else if (strnicmp(pszFmt,"Add:",4)==0)
		{
			t += atoi(pszFmt+4);
			x = *localtime( &t );
			strftime( pszBuf, 100, "%Y%m%d%H%M%S", &x );
		}
		else
			strftime( pszBuf, 100, pszFmt, &x );
		return strlen( pszBuf );
	}
	*/
}							   


//
// Mapping the given value to a new value
// Key Choices [#|&]<K1>-<V1>^<K2>-<V2>^...  or  [#|&]<K1>?<V1>:<V2>
// # - numerical, & - bit value
// Ki can be standard range expression such as "[<StRnge>]" or *<nn> ( <= <nn> )
//
int GetChoiceStr( char* pszBuf, char* pszFormat, char* pszVal )
{
	char szKW[256], szVL[256], *svp, *pos, ch1, ch2;
	int n, ns, ss, k, nLev;
	bool bs, rgs;

	ns = 0;
	pszBuf[0] = 0;
	pos = pszFormat;
	if (pos[0]=='#')
	{
		ns=1; ++pos;
	}
	else if (pos[0]=='&')
	{
		ns=2; ++pos;
	}

	while (pos[0])
	{
		ch1=0; ch2=0;
		szKW[0]=0; szVL[0]=0;
		rgs = false;

		if ( pos[0]=='-' ) { ch1='-'; ++pos; }
		else if ( pos[0]=='?' ) { ch1='?'; ++pos; bs=true; }
		else if ( pos[0]=='[' )
		{
			pos++; nLev=0; svp=pos; k=0;
			while ( pos[0] && (nLev>0 || pos[0]!=']') ) 
			{
				szKW[k++]=pos[0];
				if (pos[0]=='[') nLev++;
				if (pos[0]==']') nLev--;
				++pos;
			}
			if ( pos[0]==0 ) return 1; else ++pos; 
			szKW[k] = 0; rgs = true;
			if ( pos[0]=='-' ) {ch1='-'; ++pos; }
			else if ( pos[0]=='?' ) {ch1='?'; ++pos; bs=true; }
			else { ch1='-'; }
		}
		else 
		{
			sscanf( pos, "%[^-?]%c", szKW, &ch1 ); 
			pos += strlen(szKW)+(ch1?1:0);
			bs = ( ch1=='?' );	
		}

		if ( pos[0]=='^' ) { ch2='^'; ++pos; }
		else if ( pos[0]==':' ) { ch2=':'; ++pos; }
		else 
		{
			sscanf( pos, "%[^:\^]%c", szVL, &ch2 ); 
			pos += strlen(szVL)+(ch2?1:0);
			bs &= ( ch2==':' );	
		}

		n = strlen(szKW); ss = 0;
		if (n>0 && szKW[0]=='*') { ss = -1; strcpy(szKW, szKW+1); }
		else if (n>0 && szKW[n-1]=='~') { ss = -1; szKW[n-1]=0; }
		else if (n>0 && szKW[n-1]=='@') { ss = 1; szKW[n-1]=0; }
		
		if ( (  rgs && StdRngExprPrs(szKW, pszVal, STD_RNGE_CHECK) ) ||
			 (  !rgs && ns==0 && ( (ss==0 && strncmp(pszVal, szKW, strlen(szKW))==0) ||
			   (ss==-1 && strncmp(pszVal, szKW, strlen(szKW))<=0) ||
			   (ss==1 && strncmp(pszVal, szKW, strlen(szKW))>=0) ) ) ||
			 (  !rgs && ns == 1 && ( (ss==0 && atoi(pszVal)==atoi(szKW)) ||
			   (ss==-1 && atoi(pszVal)<=atoi(szKW)) ||              
			   (ss==1 && atoi(pszVal)>=atoi(szKW)) ) ) ||
			 (  !rgs && ns==2 && ( atoi(pszVal) & atoi(szKW) ) ) ||
			 ( szKW[0]==0 ) )
		{
			strcpy( pszBuf, szVL );	break;
		}
		else if ( bs ) { strcpy( pszBuf, pos ); break; }
	}
	return 1;
}


//
// Key Delimited <Start>-<End>
int GetDelimitedStr( char* pszVal, char* pszFormat, char* pszBuf )
{
	char szST[100], szED[100], szTemp[250]={0}, *pos, *tos, *top;
	bool bFirst = true, bRight;

	pszVal[0]=0; szST[0]=0; szED[0]=0;
	bRight = ( pszFormat[2]=='d' );

	sscanf( pszFormat+3, "%[^-]%*c%s", szST, szED );
	
	pos = pszBuf;
	while ( pos[0]!=0 )
	{
		szTemp[0] = 0;
		if ( bRight )
		{
			// Search most right substring
			tos = strstr( pos, szST );
			top = NULL;
			while ( tos!=NULL) 
			{
				top = tos;
				tos = strstr(tos+1, szST);
			}
			tos = top;
		}
		else
			tos = strstr( pos, szST );
		if (tos!=NULL)
		{
			top = strstr( tos, szED );
			if ( top==NULL || szED[0]==0 )
			{
				if (bFirst) strcpy(szTemp, tos+strlen(szST));
				break;
			}
			else
			{
				strncpy( szTemp, tos+strlen(szST), top-tos-strlen(szST) );
				szTemp[top-tos-strlen(szST)]=0;
			}
			strcat( pszVal, szTemp );
			strcat( pszVal, " " );
			pos = ( top + strlen(szED));
		}
		else
			break;

		bFirst = false;
	}
	strcat( pszVal, szTemp );
	return 1;
}


// Split the File Name into each components
bool SplitFileName( char *pszFullPath, char *pszType, char *pszVal )
{
	char szDrv[256], szDir[256], szName[64], szExt[32];
	bool ss = true;
	WIN32_FIND_DATA wFind;
	HANDLE hFile;

	_splitpath( pszFullPath, szDrv, szDir, szName, szExt );
	
	if ( pszType[0]=='D' ) strcpy( pszVal, szDrv );
	else if ( pszType[0]=='F' ) strcpy( pszVal, szDir );
	else if ( pszType[0]=='N' ) strcpy( pszVal, szName );
	else if ( pszType[0]=='E' ) strcpy( pszVal, szExt );
	else if ( pszType[0]=='S' ) sprintf( pszVal, "%s.%s", szName, szExt );
	else if ( pszType[0]=='T' ) // This File's Time
	{
		hFile = FindFirstFile( pszFullPath, &wFind );
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			CTime x = wFind.ftLastWriteTime;
			strcpy( pszVal, x.Format( "%Y%m%d%H%M%S" ));
			FindClose( hFile );
		}
	}
	else if ( pszType[0]=='Z' ) // This File's size
	{
		hFile = FindFirstFile( pszFullPath, &wFind );
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			sprintf( pszVal, "%d", wFind.nFileSizeLow );
			FindClose( hFile );
		}
	}
	else ss = false;
	return ss;
}

// Parse the Source for prefix item such as <Item>=<Value>, return <value> 
bool GetPrefixValue( char *pszSource, char *pszItem, char *pszVal )
{
	char ch, *ps, *qs;

	pszVal[0]=0;
	ps = strstr( pszSource, pszItem );
	if ( ps==NULL ) return false;
	ps += strlen( pszItem );
	if ( ps[0]=='=' || ps[0]==':' ) ++ps;

	ch=0; if ( ps[0]=='"' || ps[0]=='\'' ) { ch=ps[0]; ++ps; }
	qs = pszVal;
	while ( ps[0] )
	{
		if ( ch ? (ps[0]==ch) : (ps[0]==','||ps[0]=='\n') ) break;
		(qs++)[0]=(ps++)[0];
	}
	qs[0]=0;
	return true;
}


//
// Format of the Rule:  _Ti_^_Si_|...|_Tj_^_Sj_|
// _Ti_ ::= F<n>|#<n>-<m>|<Keyword>
// F:<n> ::= First <n> lines (including blank line)
// #<n>-<m> ::= Line range <n>-<m>
// 
void ParseSectionByRule( char *pszRule, char *pszSection, char *pszLine, int nLineNo )
{
	char szKW[100], szSN[100];
	char *ps, ch;
	int n1, n2;

	ps = pszRule;
	while ( ps[0] )
	{
		szKW[0]=0; szSN[0]=0; ch=0;
		//sscanf( ps, "%[^\^]%*c%[^|]%c", szKW, szSN, &ch );
		//ps += strlen(szKW)+strlen(szSN)+(ch?2:1);
		if ( ps[0]=='^' ) ++ps;
		else 
		{
			sscanf(ps, "%[^\^]%c", szKW, &ch );
			ps += strlen(szKW)+(ch?1:0);
		}
		ch=0;
		if ( ps[0]=='|' ) ++ps;
		else 
		{
			sscanf(ps, "%[^|]%c", szSN, &ch );
			ps += strlen(szSN)+(ch?1:0);
		}

		// if (szKW[0]==0 && szSN[0]==0 ) break;
		if ( szKW[0]=='F' && szKW[1]==':' && isdigit(szKW[2]) )
		{
			if ( nLineNo>0 && nLineNo<=atoi(szKW+2) )
			{
				strcpy( pszSection, szSN ); 
				pszSection[0]=szSN[0]; break;
			}
			else if ( nLineNo == atoi(szKW+2)+1 )
			{
				if ( strcmp(pszSection,szSN)==0 )
					pszSection[0]=0;
			}
		}
		else if ( szKW[0]=='#' && szKW[1]==':' && isdigit(szKW[2]) )
		{
			n1=1; n2=0;
			sscanf( szKW+2, "%d-%d", &n1, &n2 );
			if ( n2==0 ) n2=9999;
			if ( nLineNo>0 && nLineNo>=n1 && nLineNo<=n2 )
			{
				strcpy( pszSection, szSN ); 
				pszSection[0]=szSN[0]; break;
			}
			else if ( nLineNo == n2+1 )
			{
				if ( strcmp(pszSection,szSN)==0 )
					pszSection[0]=0;
			}
		}
		else if (strstr( pszLine, szKW )!=NULL)
		{
			strcpy( pszSection, szSN ); 
			pszSection[0]=szSN[0]; break;
		}
		if (( strcmp( szKW, "Default:" )==0 || szKW[0]==0 ) && pszSection[0]==0 )
		{
			strcpy( pszSection, szSN );
			pszSection[0]=szSN[0]; 
		}
	}
}


BOOL FindSectionEnding( char *pszRule, char *pszLine )
{
	char szKW[30], szSN[20];
	char *ps, ch;
	int ss=0;

	ps = pszRule;
	if (pszLine[0]==0) return 1;

	while ( ps[0] )
	{
		szKW[0]=0; szSN[0]=0; ch=0;
		sscanf( ps, "%[^\^]%*c%[^|]%c", szKW, szSN, &ch );
		ps += strlen(szKW)+strlen(szSN)+(ch?2:1);
		if (szKW[0]==0 || szSN[0]==0 ) break;
		if (strncmp( pszLine, szKW, strlen(szKW) )==0)
		{
			ss = 1; break;
		}
	}
	return ss;
}




////////////////////////////////////////////////////////////////////////////////
//					CStdExpress Class
////////////////////////////////////////////////////////////////////////////////


int CStdExpress::ScanOneItem( TCHAR **ppBuf, TCHAR *pWord, int nOpt)
{
	TCHAR* pBuf = *ppBuf;
	int p, ss = 0, res;
	TCHAR ch, szTmp[2048];

	while ( pBuf[0] )
	{
		p = 0;  ss = 1;  res = 0;
		while ( pBuf[0]==' '||pBuf[0]=='\t'||pBuf[0]=='\n'||pBuf[0]=='\r' ) ++pBuf;
		if (pBuf[0]==0) { ss=0; break; }

		if (isalpha(pBuf[0]) || isdigit(pBuf[0]))	// Number or String
		{
			if ( isalpha(pBuf[0]) ) ss = ETYPE_STRING;
			else ss = ETYPE_NUMBER;
			while ( pBuf[0] && (isalpha(pBuf[0]) || isdigit(pBuf[0]) || pBuf[0]=='_') )
			{
				pWord[p++]=pBuf[0];
				++pBuf;
			}
			pWord[p]=0;

			strcpy(szTmp, pWord);
			strupr(szTmp);
			if ( strcmp(szTmp,"TRUE")==0 || strcmp(szTmp,"FALSE")==0 )
			{
				strcpy( pWord, szTmp );
				ss = ETYPE_BOOLEAN;
			}
		}
		else if ( pBuf[0]=='\'' || pBuf[0]=='\"' )  // String '...' or "..."
		{
			ch = pBuf[0]; ++pBuf; ss = ETYPE_STRING;
			while ( pBuf[0] && pBuf[0]!=ch )
			{
				pWord[p++]=pBuf[0];
				++pBuf;
			}
			++pBuf;
			pWord[p]=0;
		}
		else if ( pBuf[0]=='\\' && (pBuf[1]=='x'||pBuf[1]=='X') )
		{
			ss = ETYPE_STRING;
			pWord[p++]=atoi(pBuf+2);
			pBuf += 4;
			pWord[p]=0;
		}
		else 
		{
			ss = ETYPE_OPERATOR;
			ch = pBuf[0];
			pWord[p++]=pBuf[0]; ++pBuf;
			if ((ch=='<' && pBuf[0]=='=') ||
				(ch=='>' && pBuf[0]=='=') ||
				(ch=='=' && pBuf[0]=='=') ||
				(ch=='<' && pBuf[0]=='>') ||
				(ch=='&' && pBuf[0]=='&') ||
				(ch=='|' && pBuf[0]=='|') ||
				(ch=='^' && pBuf[0]=='^') ||
				(ch=='!' && pBuf[0]=='='))
			{
				pWord[p++]=pBuf[0]; ++pBuf;
			}
			pWord[p]=0;
		}
		if ( ss ) break;
	}
	*ppBuf = pBuf;
	return ss;
}


bool CStdExpress::Evaluate( LPTSTR pzExpr, LPTSTR pzResult)
{
	LPTSTR ps;
	TCHAR szWord[MAX_ITEM*4];
	EXPRES xItem,yItem;
	int nRes = 1, nPrev, nTmp;

	top = 0;
	ps = pzExpr;
	while ( nRes > 0 )
	{
		szWord[0]=0;
		nRes = ScanOneItem( &ps, szWord );
		if ( nRes > 0 )
		{
			xItem.nType = nRes;
			strncpy(xItem.szRes, szWord, sizeof(xItem.szRes)-1);
			nPrev = GetTopType();

			if ( nRes==ETYPE_NUMBER || nRes==ETYPE_STRING || nRes==ETYPE_BOOLEAN )
			{
				// Calculate the Item
				if ( nPrev!=ETYPE_OPERATOR && top>1)
				{
					nRes=-1; break;
				}
				Push( xItem );
			}
			else
			{
				// Process Operator
				if (Priority(xItem.szRes)==0)
				{
					nRes=-1; break;
				}

				if ( nPrev==ETYPE_OPERATOR )
				{
					if (strcmp(xItem.szRes,"(")==0 ||
						strcmp(xItem.szRes,"!")==0 ||
						strcmp(xItem.szRes,"~")==0 ) Push(xItem);
					else
					{
						nRes=-1; break;
					}
				}
				else 
				{
					// Previous One is Item
					if (top<=1) Push(xItem);
					else
					{
						GetSec(yItem);
						if (Compare(yItem,xItem,nTmp))
						{
							if (nTmp<0) Push(xItem);
							else
							{
								if (!ComputeTop(Priority(xItem.szRes))) 
								{
									nRes=-1; break;
								}
								
								if (strcmp(xItem.szRes,")")!=0)	Push(xItem);
								else
								{
									Pop( yItem );
									Pop( xItem );
									Push( yItem );
								}
							}
						}
						else
						{
							nRes=-1; break;
						}
					}
				}
			}
		}
	}

	if (nRes>=0)
	{
		if (!ComputeTop(0))
		{
			nRes=-1;
		}
	}

	if (nRes>=0)
	{
		GetTop(xItem);
		strcpy(pzResult, xItem.szRes);
	}

	return (nRes>=0);
}


int  CStdExpress::Priority( LPTSTR ps )
{
	if (strcmp(ps,"(")==0)  return 20;
	if (strcmp(ps,")")==0)  return 2;

	if (strcmp(ps,"!")==0)  return 18;
	if (strcmp(ps,"~")==0)  return 18;

	if (strcmp(ps,"&")==0)  return 17;
	if (strcmp(ps,"|")==0)  return 17;

	if (strcmp(ps,"^")==0)  return 16;

	if (strcmp(ps,"*")==0)  return 15;
	if (strcmp(ps,"/")==0)  return 15;
	if (strcmp(ps,"%")==0)  return 15;

	if (strcmp(ps,"+")==0)  return 14;
	if (strcmp(ps,"-")==0)  return 14;

	if (strcmp(ps,"<")==0)   return 12;
	if (strcmp(ps,"<=")==0)  return 12;
	if (strcmp(ps,">")==0)   return 12;
	if (strcmp(ps,">=")==0)  return 12;
	if (strcmp(ps,"<>")==0)  return 12;
	if (strcmp(ps,"=")==0)   return 12;
	if (strcmp(ps,"$")==0)   return 12;
	if (strcmp(ps,"==")==0)  return 12;
	if (strcmp(ps,"!=")==0)  return 12;

	if (strcmp(ps,"&&")==0)  return 10;
	if (strcmp(ps,"||")==0)  return 10;
	if (strcmp(ps,"^^")==0)  return 10;

	return 0;
}


bool CStdExpress::Compare( EXPRES x, EXPRES y, int& ns )
{
	int n1,n2;

	if (x.nType==ETYPE_OPERATOR && y.nType==ETYPE_OPERATOR)
	{
		n1=Priority(x.szRes);
		n2=Priority(y.szRes);
		if (n1==0 || n2==0) return false;
		ns=(n1<n2?-1:(n1==n2?0:1));
		return true;
	}
	else if (x.nType==ETYPE_NUMBER && y.nType==ETYPE_NUMBER )
	{
		n1=atoi(x.szRes);
		n2=atoi(y.szRes);
		ns=(n1<n2?-1:(n1==n2?0:1));
		return true;
	}
	else if (x.nType!=ETYPE_OPERATOR && y.nType!=ETYPE_OPERATOR )
	{
		ns=strcmp(x.szRes, y.szRes);
		return true;
	}
	return false;
}


bool CStdExpress::Compute2( EXPRES x, EXPRES y, EXPRES z, EXPRES& u )
{
	int mn, n1, n2, n3;
	bool s1, s2, ss;
	TCHAR szBuf[250];

	if (x.nType==ETYPE_OPERATOR ||
		y.nType==ETYPE_OPERATOR || 
		z.nType!=ETYPE_OPERATOR) return false;
	
	if (x.nType==ETYPE_NUMBER && y.nType==ETYPE_NUMBER)
	{
		n1=atoi(x.szRes);
		n2=atoi(y.szRes);
		n3=-99999999;

		if (strcmp(z.szRes,"^")==0) n3=(int)pow((float)n1,n2);
		else if (strcmp(z.szRes,"*")==0) n3=n1*n2;
		else if (strcmp(z.szRes,"/")==0) n3=n1/n2;
		else if (strcmp(z.szRes,"%")==0) n3=n1%n2;
		else if (strcmp(z.szRes,"+")==0) n3=n1+n2;
		else if (strcmp(z.szRes,"-")==0) n3=n1-n2;
		else if (strcmp(z.szRes,"&")==0) n3=(n1&n2);
		else if (strcmp(z.szRes,"|")==0) n3=(n1|n2);

		if (n3!=-99999999)
		{
			u.nType=ETYPE_NUMBER;
			sprintf(u.szRes,"%ld",n3);
			return true;
		}

		if (strcmp(z.szRes,"<")==0) {n3=0; ss=(n1<n2); }
		else if (strcmp(z.szRes,">")==0) {n3=0; ss=(n1>n2); }
		else if (strcmp(z.szRes,"=")==0) {n3=0; ss=(n1==n2); }
		else if (strcmp(z.szRes,"==")==0) {n3=0; ss=(n1==n2); }
		else if (strcmp(z.szRes,"<=")==0) {n3=0; ss=(n1<=n2); }
		else if (strcmp(z.szRes,">=")==0) {n3=0; ss=(n1>=n2); }
		else if (strcmp(z.szRes,"<>")==0) {n3=0; ss=(n1!=n2); }
		else if (strcmp(z.szRes,"!=")==0) {n3=0; ss=(n1!=n2); }

		if (n3!=-99999999)
		{
			u.nType=ETYPE_BOOLEAN;
			if (ss) sprintf(u.szRes,"TRUE");
			else sprintf(u.szRes,"FALSE");
			return true;
		}
	}
	else if ( (x.nType==ETYPE_STRING || x.nType==ETYPE_NUMBER) &&
			  (y.nType==ETYPE_STRING || y.nType==ETYPE_NUMBER) )
	{
		n3=-99999999;

		if (strcmp(z.szRes,"+")==0) 
		{
			u.nType=ETYPE_STRING;
			strcpy(u.szRes, x.szRes );
			strcat(u.szRes, y.szRes );
			return true;
		}
		else if (strcmp(z.szRes,"*")==0) 
		{
			u.nType=ETYPE_STRING;
			FormatForExp( szBuf, x.szRes, y.szRes );
			strncpy(u.szRes, szBuf, sizeof(u.szRes)-1);
			return true;
		}

		mn = strlen(x.szRes);
		if (strlen(y.szRes)<(unsigned)mn) mn=strlen(y.szRes);
		if (strcmp(z.szRes,"<")==0) {n3=0; ss=(strcmp(x.szRes,y.szRes)<0); }
		else if (strcmp(z.szRes,">")==0) {n3=0; ss=(strcmp(x.szRes,y.szRes)>0); }
		else if (strcmp(z.szRes,"=")==0) {n3=0; ss=(strncmp(x.szRes,y.szRes,mn)==0); }
		else if (strcmp(z.szRes,"==")==0) {n3=0; ss=(strcmp(x.szRes,y.szRes)==0 && strlen(x.szRes)==strlen(y.szRes)); }
		else if (strcmp(z.szRes,"<=")==0) {n3=0; ss=(strcmp(x.szRes,y.szRes)<=0); }
		else if (strcmp(z.szRes,">=")==0) {n3=0; ss=(strcmp(x.szRes,y.szRes)>=0); }
		else if (strcmp(z.szRes,"<>")==0) {n3=0; ss=(strcmp(x.szRes,y.szRes)!=0); }
		else if (strcmp(z.szRes,"!=")==0) {n3=0; ss=(strcmp(x.szRes,y.szRes)!=0); }
		else if (strcmp(z.szRes,"$")==0) {n3=0; ss=(strstr(x.szRes,y.szRes)!=NULL); }

		if (n3!=-99999999)
		{
			u.nType=ETYPE_BOOLEAN;
			if (ss) sprintf(u.szRes,"TRUE");
			else sprintf(u.szRes,"FALSE");
			return true;
		}
	
	}
	else if (x.nType==ETYPE_BOOLEAN && y.nType==ETYPE_BOOLEAN)
	{
		s1=(strcmp(x.szRes,"TRUE")==0);
		s2=(strcmp(y.szRes,"TRUE")==0);
		n3=-99999999;

		if (strcmp(z.szRes,"&&")==0) {n3=0; ss=(s1 && s2); }
		else if (strcmp(z.szRes,"||")==0) {n3=0; ss=(s1 || s2); }
		else if (strcmp(z.szRes,"^^")==0) {n3=0; ss=(s1 || s2); }

		if (n3!=-99999999)
		{
			u.nType=ETYPE_BOOLEAN;
			if (ss) sprintf(u.szRes,"TRUE");
			else sprintf(u.szRes,"FALSE");
			return true;
		}
	}
	return false;
}


bool CStdExpress::Compute1( EXPRES x, EXPRES z, EXPRES& u )
{
	int n1, n3;
	bool s1, ss;

	if (z.nType!=ETYPE_OPERATOR) return false;
	
	if (x.nType==ETYPE_NUMBER)
	{
		n1=atoi(x.szRes);
		n3=-99999999;
		if (strcmp(z.szRes,"~")==0) { n3=(~n1); }

		if (n3!=-99999999)
		{
			u.nType=ETYPE_NUMBER;
			sprintf(u.szRes,"%ld",n3);
			return true;
		}
	}
	else if (x.nType==ETYPE_STRING)
	{
	}
	else if (x.nType==ETYPE_BOOLEAN)
	{
		s1=(strcmp(x.szRes,"TRUE")==0);
		n3=-99999999;

		if (strcmp(z.szRes,"!")==0) { n3=0; ss=(!s1); }
		if (n3!=-99999999)
		{
			u.nType=ETYPE_BOOLEAN;
			if (ss) sprintf(u.szRes,"TRUE");
			else sprintf(u.szRes,"FALSE");
			return true;
		}
	}
	return false;
}


bool CStdExpress::ComputeTop( int nPrt )
{
	EXPRES x,y,z,u;
	bool suc;

	suc = true;
	while ( suc )
	{
		if (top==1)
		{
			GetTop(x);
			suc = (x.nType!=ETYPE_OPERATOR);
			break;
		}

		GetSec(x);
		if ( x.nType!=ETYPE_OPERATOR )
		{
			suc = false; break;
		}
		if (Priority(x.szRes)<nPrt) break;

		if (top==2)
		{
			Pop(y); Pop(z);
			if (strcmp(z.szRes,"(")==0)
			{
				Push(z); Push(y); suc=true; break;
			}
			else if (strcmp(z.szRes,"!")==0 || strcmp(z.szRes,"~")==0)
			{
				suc=Compute1(y,z,u); Push(u); break;
			}
			else
			{
				suc=false;  
			}
		}
		else if (top>2)
		{
			Pop(y);
			Pop(z);
			if ( strcmp(z.szRes,"(")==0 )
			{
				Push(z); Push(y); break; 
			}
			else if ( strcmp(z.szRes,"!")==0 || strcmp(z.szRes,"~")==0)
			{
				suc=Compute1(y,z,u); Push(u);
			}
			else
			{
				Pop(x);
				suc=Compute2(x,y,z,u);
				Push(u);
			}
		}
	}
	return suc;
}


//////////////////////////////////////////////////////////////////////
// CStdReplace Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStdReplace::CStdReplace()
{
	int i;
	for(i=0;i<MAX_REPLNUM;i++)
	{
		pzIdStr[i]=NULL; pzExist[i]=NULL; pzNew[i]=NULL; szQLook[i]=0;
	}
	m_nItem=0;
	m_nType=0;
}

CStdReplace::~CStdReplace()
{
	for(int i=0;i<MAX_REPLNUM;i++)
	{
		if (pzExist[i]!=NULL) delete pzExist[i];
		if (pzNew[i]!=NULL)   delete pzNew[i];
		if (pzIdStr[i]!=NULL) delete pzIdStr[i];
	}
}


//
// Format: {[$Regular:][IdStr@]<Exist>=<New>|...|[IdStr@]<Exist>=<New>}
//
bool CStdReplace::SetupTemplate(LPTSTR pzTmplt)
{
	TCHAR  *ps, *ts, *rs, ch1, szIds[256], szSrc[256], szTar[2048];
	int i, nSrc, nTar;

	ps = pzTmplt;
	if (ps[0]=='*') ++ps; if (ps[0]=='{') ++ps;

	if (strncmp(ps,"$Regular:",9)==0) { m_nType = 1; ps+=9; }
	i=0;
	
	do 
	{
		while (ps[0]=='|' || ps[0]==' ' || ps[0]=='\t') ++ps;
		if (ps[0]==0||ps[0]=='}') break;
		
		// szSrc[0]=0; szTar[0]=0; ch1=0; ch2=0;
		
		ts = szSrc;
		SCANONEITEM(ps, ts, ch1, '=');
		nSrc = strlen(szSrc);

		ts = szTar; 
		SCANONEITEM(ps, ts, ch1, '|');
		nTar = strlen(szTar);

		if ( m_nType==1 )
		{
			// UNIX Regular
		}
		else
		{
			// General Format
			ts=szSrc; rs=szSrc;
			while (ts[0]) { TCHEXPAND(ts,rs) }
			rs[0]=0; nSrc=strlen(szSrc);

			szQLook[i]=szSrc[0];

			ts = strchr(szSrc,'@');
			if ( ts==NULL ) szIds[0]=0;
			else { ts[0]=0; strcpy(szIds, szSrc); strcpy(szSrc,ts+1); }

			ts=szTar; rs=szTar;
			while (ts[0]) { TCHEXPAND(ts,rs) }
			rs[0]=0; nTar=strlen(szTar);

			if (pzIdStr[i]!=NULL) delete pzIdStr[i];
			if ( szIds[0]==0 ) pzIdStr[i]=NULL;
			else { pzIdStr[i] = new TCHAR[nSrc+10]; strcpy(pzIdStr[i], szIds); }
		}

		if (pzExist[i]!=NULL) delete pzExist[i];
		pzExist[i] = new TCHAR[nSrc+10];
		strcpy(pzExist[i], szSrc);

		if (pzNew[i]!=NULL) delete pzNew[i];
		pzNew[i] = new TCHAR[nTar+10];
		strcpy(pzNew[i], szTar);

		if (++i>m_nItem) m_nItem=i;
	} while ( ps[0] && ps[0]!='}' );

	szQLook[i]=0;
	return true;
}


// UNIX Regular Expression Replacement
int CStdReplace::RegExpReplace( LPTSTR pzSrcExp, LPTSTR pzTarExp, LPTSTR pzBuf, int nLen )
{
	LPTSTR ps;
	int nn, mm, idx, nLmt;
	TCHAR ch, szTarBuf[512]={0};

	ps = pzBuf;
	nLmt = strlen(pzBuf)+1;
	idx = 0;
	ch = 0;
	while ( ps[0] && idx<nLen )
	{
		m_nVar = 0;
		nn = RegExpCheck( ps, pzSrcExp, ch );
		if ( nn>=0 )
		{
			mm = CreateTarBuf( pzTarExp, szTarBuf );
			if ( mm>0 ) memmove(ps+mm, ps+nn, nLmt-idx );
			memcpy(ps, szTarBuf, mm);
			idx += (mm-nn);	nLmt += (mm-nn); ps+=mm; ch=ps[-1];
		}
		else
		{
			++idx; ch=(ps++)[0];
		}
	}
	return idx;
}


#define		RESTP(x)	(((nRes=(x))<0)?-2:nRes)

int CStdReplace::RegExpCheck( LPTSTR pzBuf, LPTSTR pzExp, TCHAR lch, LPTSTR lps, int nCnt )
{
	LPTSTR ps, rs, qs, ts, svp;
	int nn, nRes;
	TCHAR ch, szTmp[128];
	bool bs, bt;

	ps = pzBuf; rs = pzExp; svp = rs; 
	if ( rs[0]==0 ) return 0; // { if (nCnt==0) return 0; else return -1; }
	else if ( ps[0]==0 ) return -1;

	if ( rs[0]=='\\' )
	{
		ch = (++rs)[0];
		switch ( ch )
		{
		case '\\':
			if ( ps[0]==ch ) return RESTP(RegExpCheck( ps+1, rs+1, ch, svp, nCnt ))+1;
			else return -1;
		case 'd':
			if ( ps[0]>='0' && ps[0]<='9' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 'D':
			if ( ps[0]<'0' || ps[0]>'9' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 'f':
			if ( ps[0]=='\f' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 'n':
			if ( ps[0]=='\n' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 'r':
			if ( ps[0]=='\r' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 't':
			if ( ps[0]=='\t' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 'v':
			if ( ps[0]=='\v' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 's':
			if ( ps[0]<=' ' && ps[0]!='\n' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 'S':
			if ( ps[0]>' ' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 'w':
			if ( isalpha(ps[0]) || ps[0]=='_' ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		case 'W':
			if ( !isalpha(ps[0]) ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		default:
			if ( ps[0]==ch ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		}
	}
	else if ( rs[0]=='(' )
	{
		bt = false;
		do 
		{
			++rs; qs=szTmp;
			while (rs[0] && rs[0]!='|' && rs[0]!=')') (qs++)[0]=(rs++)[0];
			qs[0]=0;
			if ( strlen(szTmp)>0 && !bt )
			{
				nn = RegExpCheck(ps, szTmp, lch);
				if ( nn>=0 )
				{
					strncpy( gszVarList[m_nVar], ps, nn );
					gszVarList[m_nVar++][nn] = 0;
					ps += nn; bt = true;
				}
			}
		} while (rs[0] && rs[0]!=')');
		if (rs[0]==')' ) ++rs;
		if (bt && ((nRes=RegExpCheck(ps, rs, ps[-1], svp, nCnt ))>=0)) return nRes+nn;
		else return -1;
	}
	else if ( rs[0]=='[' )
	{
		ts = rs;
		bs = ((++rs)[0]=='^'); if (bs) ++rs;
		qs = szTmp;	while ( rs[0] && rs[0]!=']' ) (qs++)[0]=(rs++)[0]; qs[0]=0; 
		if ( rs[0]!=']')
		{
			rs = ts;
			if ( ps[0]==rs[0] ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
			else return -1;
		}

		++rs; qs = szTmp; 
		ch = ps[0]; bt = false;
		while (	qs[0] )
		{
			if (qs[0]=='-')
			{
				if ((isalpha(qs[-1])||isdigit(qs[-1])) &&
					(isalpha(qs[1])||isdigit(qs[1])) )
				{ bt |= (ch>=qs[-1] && ch<=qs[1]); qs+=2; }
				else bt |= ((qs++)[0]==ch);
			}
			else
				bt |= ((qs++)[0]==ch);
		}
		if (bs) bt = !bt;
		if (bt)	return RESTP(RegExpCheck( ps+1, rs, ch, svp, nCnt ))+1;
		else return -1;
	}
	else if ( rs[0]=='*' || rs[0]=='+' )
	{
		// Repleat zero(one) or more times of the last entry (by svp) 
		if ( rs[0]=='*' || nCnt>0 )
		{
			// Check next position
			nRes = RegExpCheck( ps, rs+1, lch, lps, 0 );
			if ( nRes>=0 ) return nRes;
		}
		// Repeat the last entry
		nRes = RegExpCheck( ps, lps, lch, lps, nCnt+1 );
		if (nRes>=0) return nRes;
		else return -1;
	}
	else if ( rs[0]=='^' )
	{
		if (lch=='\n') 
			return RESTP(RegExpCheck( ps, rs+1, lch, svp, nCnt ));
		else return -1;
	}
	else if ( rs[0]=='$' )
	{
		if (ps[0]=='\n') return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ));
		else return -1;
	}
	else if ( rs[0]=='.' )
	{
		return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
	}
	else
	{
		if ( ps[0]==rs[0] ) return RESTP(RegExpCheck( ps+1, rs+1, ps[0], svp, nCnt ))+1;
		else return -1;
	}
	return -1;
}



int CStdReplace::CreateTarBuf( LPTSTR pzExp, LPTSTR pzBuf )
{
	LPTSTR ps = pzExp;
	LPTSTR rs = pzBuf;
	TCHAR ch;
	int k, nn = 0;

	while ( ps[0] )
	{
		if ( ps[0]=='\\' )
		{
			++ps; ch = ps[0];
			if ( ch>='1' && ch<='9' )
			{
				k = ((int)ch-(int)'0');
				if ( k<=m_nVar ) 
				{ 
					strcpy(rs, gszVarList[k-1]);
					nn += strlen(gszVarList[k-1]);
					rs += strlen(gszVarList[k-1]);
					++ps;
				}
				else
					++ps;
			}
			else if ( ch=='0' )
			{
				(rs++)[0] = atoi(ps);
				while ( isdigit(ps[0]) ) ++ps;
			}
			else
			{
				++ps;
				switch ( ch )
				{
				case 'n': (rs++)[0]='\n'; ++nn; break;
				case 'r': (rs++)[0]='\r'; ++nn; break;
				case 'v': (rs++)[0]='\v'; ++nn; break;
				case 't': (rs++)[0]='\t'; ++nn; break;
				case 'f': (rs++)[0]='\f'; ++nn; break;
				default: (rs++)[0]=ch; ++nn; break;
				}
			}
		}
		else 
		{
			++nn; (rs++)[0]=(ps++)[0];
		}
	}

	rs[0] = 0;
	return nn;
}



int CStdReplace::Execute(void *pBuf, int nLen)
{
	LPTSTR pos;
	int i=0, j, k, m, nn, nRest, nDest, nSrc;
	bool ss = false;
	LPTSTR qs, ts, ps;
	TCHAR ch, szTarBuf[MAX_REPLNUM]={0};

	pos = (LPTSTR)pBuf; nn = nLen; ch = 0;

	if ( m_nType==1 )
	{
		while ( i<nn )
		{
			j = 0;
			while ( j<m_nItem )
			{
				m_nVar = 0;
				k = RegExpCheck( pos, pzExist[j], ch );
				if (k>=0) break;
				j++;
			}
			if ( k>=0 && j<m_nItem )
			{
				m = CreateTarBuf( pzNew[j], szTarBuf );
				if ( m>0 ) memmove(pos+m, pos+k, nn-i );
				memcpy(pos, szTarBuf, m);
				i += (m-k);	nn += (m-k); pos+=(m-k); ch=pos[-1];
			}
			else
			{
				++i; ch=(pos++)[0]; 
			}
		}
	}
	else
	{
		while (i<nn)
		{
			ts = szQLook; ch = pos[i]; ss = false;
			while (ch>0 && (qs=strchr(ts, ch))!=NULL)
			{
				k = (qs-szQLook);
				if ( pzIdStr[k]!=NULL )
				{
					nSrc = (int)strlen(pzIdStr[k]);
					if ( strncmp(pos+i, pzIdStr[k], nSrc)==0 )
					{
						ss = true; i += nSrc;
						ps = strstr( pos+i, (LPCTSTR)pzExist[k] );
						if ( ps!=NULL )
						{
							i += (ps-(pos+i));
							nSrc = (int)strlen(pzExist[k]);
							nDest = (int)strlen(pzNew[k]);
							nRest = nn-(i+nSrc);
							memmove(pos+i+nDest, pos+i+nSrc, nRest);
							memcpy(pos+i, pzNew[k], nDest);
							nn += (nDest-nSrc);
							i += (nDest-nSrc);
						}
						else
							ss = false;
					}
				}
				else
				{
					nSrc = (int)strlen(pzExist[k]); 
					// This operation can be more complicated
					if ( strncmp(pos+i, pzExist[k], nSrc)==0 )
					{
						ss = true;
						nDest = (int)strlen(pzNew[k]);
						nRest = nn-(i+nSrc);
						memmove(pos+i+nDest, pos+i+nSrc, nRest);
						memcpy(pos+i, pzNew[k], nDest);
						nn += (nDest-nSrc);
						i += (nDest-nSrc);
					}
				}
				if (ss) break; else ts=qs+1;
			}
			if (!ss) ++i;
		}
    
	}
	
	((TCHAR*)pBuf)[nn]=0;
	return nn;
}



//////////////////////////////////////////////////////////////////////
//		String Comprison Utility Procedures
//////////////////////////////////////////////////////////////////////

// Create its DEBUG-like binary representation block for a string
void CreateBinaryBlock(LPCTSTR psrc, LPTSTR pdest, int nLmt)
{
	int i, j, k, n;
	TCHAR szTmp[128], szTmp2[20];

	n = strlen(psrc);
	i = 0; j = 0;
	pdest[0] = 0;

	while (i<n)
	{
		memset(szTmp2,0,20);
		strncpy(szTmp2, psrc+i, 16);
		i = (i+16>=n?n:i+16);

		sprintf(szTmp,"%02X %02X %02X %02X %02X %02X %02X %02X-"
					  "%02X %02X %02X %02X %02X %02X %02X %02X  ",
	  				  szTmp2[0], szTmp2[1], szTmp2[2], szTmp2[3], 
					  szTmp2[4], szTmp2[5], szTmp2[6], szTmp2[7], 
					  szTmp2[8], szTmp2[9], szTmp2[10], szTmp2[11], 
					  szTmp2[12], szTmp2[13], szTmp2[14], szTmp2[15]);
		strncat( pdest, szTmp, nLmt-strlen(pdest) );

		for (k=0; k<16; k++) 
			if (szTmp2[k]==0) szTmp2[k]=' ';
			else if (szTmp2[k]<32) szTmp2[k]='.';
		sprintf(szTmp, "%8.8s-%8.8s\r\n", szTmp2, szTmp2+8);	
		strncat( pdest, szTmp, nLmt-strlen(pdest) );

		if (++j%4==0) strcat( pdest, "\r\n" );
	}
}


// LCS Finding with different Options
//   0: Initial LCS
//   1: Ending LCS
//   2: Anywhere LCS (without shifting)   
//	 3: Standard LCS (shift to shorter one to match another)
int FindLongestCommonSubstring( LPCTSTR pzStr1, LPCTSTR pzStr2, LPTSTR pzRes, int nOpt, int *ps1, int *ps2 )
{
	int i, p, q, sp, sq, m, n1, n2;
	bool ss;

	i=0;
	if (nOpt==0)
	{
		while (pzStr1[i] && pzStr1[i]==pzStr2[i])
		{
			pzRes[i]=pzStr1[i]; ++i;
		}
		pzRes[i]=0;
		return i;
	}

	if (nOpt==1)
	{
		pzRes[0] = 0;
		n1 = strlen(pzStr1); n2 = strlen(pzStr2); i=1;
		while (i<=n1 && i<=n2 && pzStr1[n1-i]==pzStr2[n2-i]) i++;
		strcpy(pzRes, pzStr1+(n1-i)+1);
		pzRes[i-1] = 0;
		return (i-1);
	}

	if (nOpt==2)
	{
		m = strlen(pzStr1); 
		if (strlen(pzStr2)<(unsigned)m) m = strlen(pzStr2);
		ss = false;	sq=0; sp=-1;
		for (i=0; i<m; i++)
		{
			if (pzStr1[i]==pzStr2[i])
			{
				if (!ss) // first round
				{
					p=i; q=1;
					ss = true;
				}
				else
					++q;
			}
			else
			{
				if (ss)
				{
					if (q>sq) { sp=p; sq=q; }
					ss = false;
				}
			}
		}
		if ( sp>=0 ) strncpy(pzRes, pzStr1+sp, sq);
		pzRes[sq]=0;
		return sq;
	}

	if (nOpt==3)
	{
		int k, l, p1, p2, sp1, sp2;
		bool bs = false;

		LPTSTR pzs1, pzs2;
		n1 = strlen(pzStr1); 
		n2 = strlen(pzStr2);
		
		if (n1>n2) { pzs1=(LPTSTR)pzStr1; pzs2=(LPTSTR)pzStr2; }
		else { bs=true; pzs2=(LPTSTR)pzStr1; pzs1=(LPTSTR)pzStr2; k=n2; n2=n1; n1=k; }

		sq=0; sp1=-1, sp2=-1;
		for (i=0; i<n1; i++)
		{
			l=n2; if (n1-i<l) l=(n1-i);
			ss = false;	
			for (k=0; k<l; k++)
			{
				if (pzs1[i+k]==pzs2[k])
				{
					if (!ss) // first round
					{
						p1=i+k, p2=k; q=1;
						ss = true;
					}
					else
						++q;
				}
				else
				{
					if (ss)	// End of a Common str
					{
						if (q>sq) { sp1=p1; sp2=p2; sq=q; }
						ss = false;
					}
				}
			}
			if (ss)	 // common string in the last
			{
				if (q>sq) { sp1=p1; sp2=p2; sq=q; }
			}
		}
		if ( sp1>=0 && pzRes!=NULL ) strncpy(pzRes, pzs1+sp1, sq); 
		if ( pzRes!=NULL) pzRes[sq]=0; 
		if (ps1!=NULL) (*ps1)=(bs?sp2:sp1); 
		if (ps2!=NULL) (*ps2)=(bs?sp1:sp2);
		return sq;
	}

	return 0;
}


// Find a sequence of common strings, with maximum of # of matched chars
int GetCommonStrSeq( LPCTSTR pzStr1, LPCTSTR pzStr2, 
					 int* pnPos1, int* pnPos2,  int* pnLen, int& np, int nLmt )
{
	CString x,y;
	
	int t1, t2, n1, n2, m1=0, m2=0, nn, k;

	n1 = strlen( pzStr1 ); n2 = strlen( pzStr2 );
	nn = FindLongestCommonSubstring( pzStr1, pzStr2, NULL, 3, &t1, &t2 );
	if (nn>=2)
	{
		if (np<nLmt-1)
		{
			if (pnPos1) pnPos1[np]=t1; 
			if (pnPos2) pnPos2[np]=t2; 
			if (pnLen) pnLen[np]=nn; 
			++np;			
		}
		if (t1>1 && t2>1)
		{
			x = pzStr1; y = pzStr2; x = x.Left(t1); y = y.Left(t2);
			m1 = GetCommonStrSeq( x, y, pnPos1, pnPos2, pnLen, np, nLmt );
		}
		if ( n1-(t1+nn) > 1 && n2-(t2+nn) > 1 )
		{
			x = pzStr1; y = pzStr2; x = x.Right(n1-(t1+nn)); y = y.Right(n2-(t2+nn));
			k = np;
			m2 = GetCommonStrSeq( x, y, pnPos1, pnPos2, pnLen, np, nLmt );
			for (;k<np;k++)
			{
				if (pnPos1) pnPos1[k] += t1+nn; 
				if (pnPos2) pnPos2[k] += t2+nn;
			}
		}
		return (m1+m2+(pnPos1!=NULL?1:nn));
	}
	else
		return 0;
}


// Create a sequence of CS (delimited by <ch>)
void CreateStrSeqByIndex( LPCTSTR pzSrc, LPTSTR pzStr, int* pnPos, int* pnLen, int nn, char dlmt )
{
	int i,j,k;

	for (i=0; i<nn; i++ )
		for(j=i+1; j<nn; j++)
			if ( pnPos[j]<pnPos[i] )
			{
				k = pnPos[j]; pnPos[j] = pnPos[i]; pnPos[i] = k;
				k = pnLen[j]; pnLen[j] = pnLen[i]; pnLen[i] = k;
			}

	i = 0; 
	for (k=0; k<nn; k++)
	{
		for(j=0; j<pnLen[k]; j++) pzStr[i++]=pzSrc[pnPos[k]+j];
		pzStr[i++] = dlmt;
	}
	pzStr[i]=0;
}


// Get Standard All common string w/ shift 
int GetAllCommonStringEx( LPCTSTR pzStr1, LPCTSTR pzStr2, LPTSTR pzRes, TCHAR ch )
{
	int np1[100], np2[100], nl[100], nn, nx=0;
	nn = GetCommonStrSeq( pzStr1, pzStr2, np1, np2, nl, nx, 100 );
	if (nn==0) pzRes[0]=0;
	else 
		CreateStrSeqByIndex( pzStr1, pzRes, np1, nl, nn, ch );
	return nn;
}


// Get All possible cs bewteen two given strings
int GetAllCommonString( LPCTSTR pzStr1, LPCTSTR pzStr2, int* pnPos, int* pnLen)
{
	int i, p, m;
	bool ss = false;

	m = strlen(pzStr1); 
	if (strlen(pzStr2)<(unsigned)m) m = strlen(pzStr2);
	p = -1;

	for (i=0; i<m; i++)
	{
		if (pzStr1[i]==pzStr2[i])
		{
			if (!ss) // first round
			{
				++p;
				pnPos[p]=i;
				pnLen[p]=1;
				ss = true;
			}
			else
				++pnLen[p];
		}
		else
			ss = false;

	}
	return (p+1);
}


// Check and Adjust the CS List	with new two target strings
int CheckCommonString( LPCTSTR pzStr1, LPCTSTR pzStr2, int* pnPos, int* pnLen, int nn)
{
	int i, k;

	for (i=0; i<nn; i++)
		if (strncmp(pzStr1+pnPos[i], pzStr2+pnPos[i], pnLen[i])!=0)	pnLen[i]=0;
	k=0;
	for(i=0; i<nn; i++)
		if (pnLen[i]>0) 
		{
			pnPos[k]=pnPos[i];
			pnLen[k]=pnLen[i];
			++k;
		}
	return k;
}


// Get number of appearance of the target string in given string
int GetSubstringNum( LPCTSTR pzStr, LPTSTR pzTar )
{
	LPTSTR ps = (LPTSTR)pzStr, ts;
	int nn=0;

	while (ps[0])
	{
		ts = strstr(ps, pzTar);
		if (ts==NULL) break;
		else
		{
			++nn;
			ps = ts+strlen(pzTar);
		}
	}
	return nn;
}



/*
#define		OPT_NAMECHK_SWITCH		0x00000001
#define		OPT_NAMECHK_EXACT		0x00000002
#define		OPT_NAMECHK_90PCT		0x00000004
#define		OPT_NAMECHK_80PCT		0x00000008
#define		OPT_NAMECHK_60PCT		0x00000010
#define		OPT_NAMECHK_SHTFM		0x00000020
#define		OPT_NAMECHK_CHONLY		0x00000040
#define		OPT_NAMECHK_CAPTL		0x00000080
*/ 

#define		MERGENM(x,y,b)		while (y!=NULL && y[0]) {if (isalpha(y[0]) || !(b)) (x++)[0]=(y++)[0]; else y++; }; 
			

bool NameCheckEx( LPCTSTR pzFirst1, LPCTSTR pzFirst2, LPCTSTR pzLast1, LPCTSTR pzLast2, int nOpt, LPTSTR pzRes )
{
	TCHAR szBuf1[200], szBuf2[200], *ps, *ts;
	int minl=0, nn=0, mm, n2, k = 0;
	bool bSuc;

	if ( nOpt==0 ) return true;

	ps = szBuf1; 
	ts = (LPTSTR)pzFirst1; MERGENM( ps, ts, nOpt&OPT_NAMECHK_CHONLY );
	(ps++)[0]=' ';
	ts = (LPTSTR)pzLast1; MERGENM( ps, ts, nOpt&OPT_NAMECHK_CHONLY );
	(ps++)[0]=0;
	if (nOpt&OPT_NAMECHK_CAPTL) strupr( szBuf1 );

	ps = szBuf2; 
	ts = (LPTSTR)pzFirst2; MERGENM( ps, ts, nOpt&OPT_NAMECHK_CHONLY );
	(ps++)[0]=' ';
	ts = (LPTSTR)pzLast2; MERGENM( ps, ts, nOpt&OPT_NAMECHK_CHONLY );
	(ps++)[0]=0;
	if (nOpt&OPT_NAMECHK_CAPTL) strupr( szBuf2 );

	//sprintf( szBuf1, "%s %s", pzFirst1, pzLast1 );
	//sprintf( szBuf2, "%s %s", pzFirst2, pzLast2 );

	minl = strlen( szBuf1 );
	if ( (int)strlen( szBuf2 )< minl ) minl = strlen( szBuf2 );

	if ( nOpt&OPT_NAMECHK_EXACT )
		mm = strcmp(szBuf1, szBuf2);
	else
		nn = GetCommonStrSeq( szBuf1, szBuf2, NULL, NULL, NULL, k, 200);

	if ( nOpt & OPT_NAMECHK_SWITCH )
	{
		//sprintf( szBuf1, "%s %s", pzLast1, pzFirst1 );
		//sprintf( szBuf2, "%s %s", pzFirst2, pzLast2 );

		ps = szBuf1; 
		ts = (LPTSTR)pzLast1; MERGENM( ps, ts, nOpt&OPT_NAMECHK_CHONLY );
		(ps++)[0]=' ';
		ts = (LPTSTR)pzFirst1; MERGENM( ps, ts, nOpt&OPT_NAMECHK_CHONLY );
		(ps++)[0]=0;
		if (nOpt&OPT_NAMECHK_CAPTL) strupr( szBuf1 );

		/*ps = szBuf2; 
		ts = (LPTSTR)pzLast2; MERGENM( ps, ts, nOpt&OPT_NAMECHK_CHONLY );
		(ps++)[0]=' ';
		ts = (LPTSTR)pzFirst2; MERGENM( ps, ts, nOpt&OPT_NAMECHK_CHONLY );
		(ps++)[0]=0;
		if (nOpt&OPT_NAMECHK_CAPTL) strupr( szBuf2 );
		*/

		if ( nOpt&OPT_NAMECHK_EXACT && mm )
			mm = strcmp(szBuf1, szBuf2);
		else
		{
			n2 = GetCommonStrSeq( szBuf1, szBuf2, NULL, NULL, NULL, k, 200);
			if ( n2>nn ) nn = n2;
		}
	}

	bSuc = (((nOpt & OPT_NAMECHK_EXACT) && mm==0 ) ||
		    ((nOpt & OPT_NAMECHK_90PCT) && nn*100/minl>=90 ) ||
		    ((nOpt & OPT_NAMECHK_80PCT) && nn*100/minl>=80 ) ||
		    ((nOpt & OPT_NAMECHK_60PCT) && nn*100/minl>=60 ));

	if ( pzRes!=NULL ) sprintf( pzRes, "%d:%d", nn, minl );
	return bSuc;
}


//
// Match Source and Destine string (with ? and * in destine) to create Result string
// if pzRes==NULL, do checking only with true/false 
bool strmatch( LPTSTR pzSrc, LPTSTR pzDest, LPTSTR pzRes, int nSign )
{
	TCHAR szKey[256]={0}, szSvK[256], *ps, *ts, szTmpS[256], szTmpD[256], szTmpR[256];
	int i,k,n;

	if (pzDest[0]==0) { if (pzRes!=NULL) pzRes[0]=0; return true; }

	// Get the first key segment from pzDest
	ps = pzDest;
	while ( ps[0] && (ps[0]=='?' || ps[0]=='*')) ++ps;
	szKey[0] = 0;
	sscanf(	ps,"%[^?*]", szKey );
	if ( strlen(szKey)==strlen(pzDest) )
	{
		// No wild characters
		if ( pzRes==NULL ) return (strcmp(pzSrc, pzDest)==0);
		else { strcpy(pzRes, pzDest); return true; }
	}			   
	
	// destine is ???. or .??? 
	if ( strchr(pzSrc, '.')!=NULL && strchr(pzDest, '.')!=NULL && strchr(pzDest, '*')==NULL )
	{
		strcpy( pzRes, pzDest );
		ps = strchr(pzSrc, '.'); ts = strchr(pzRes, '.');

		if ( (ts-pzRes)<=(ps-pzSrc) )
		{
			ps -= (ts-pzRes); ts = pzRes; 
		}
		else
		{
			ts -= (ps-pzSrc); ps = pzSrc;
		}

		while ( ts[0] ) { if (ts[0]=='?')	ts[0]=ps[0]; ++ts; ++ps; }
		ts = pzRes;
		while ( ts[0] )	{ if (ts[0]=='?') ts[0]='_'; ++ts; }
		return true;
	}

	// Looking for Key Segment from source string
	if ( szKey[0]==0 )
		ts = pzSrc+strlen(pzSrc);
	else
	{
		strcpy( szSvK, szKey );
		ts = strstr( pzSrc, szKey );
		while ( szKey[0] && ts==NULL )
		{
			szKey[strlen(szKey)-1]=0;
			if ( szKey[0] ) ts = strstr( pzSrc, szKey );
		}
		if ( szKey[0]==0 ) strcpy( szKey, szSvK );

		if ( ts==NULL )
		{
			// Key Segment is not in pzSrc
			if ( pzRes==NULL ) return false;
			else if ( pzSrc[0]==0 ) 
			{ 
				k=0; ps = pzDest;
				while ( ps[0] )
				{
					if ( ps[0]!='?' && ps[0]!='*' ) pzRes[k++]=ps[0];
					ps++;
				}
				pzRes[k]=0; return true; 
			}
			else
			{
				strcpy( pzRes, szKey );
				return strmatch(pzSrc, ps+strlen(szKey), pzRes+strlen(pzRes),1);
			}
		}
	}

	// (ps,ts) is the Key Segment point in the (source, destine)
	strncpy( szTmpS, pzSrc, ts-pzSrc ); szTmpS[ts-pzSrc]=0;
	strncpy( szTmpD, pzDest, ps-pzDest ); szTmpD[ps-pzDest]=0;
	if ( pzSrc[0] ) ts += strlen(szKey); 
	if ( pzDest[0] ) ps += strlen(szKey);

	if ( szTmpD[0]=='*' )
	{
		strcpy( szTmpR, szTmpS );
		if ( pzRes==NULL ) return strmatch( ts, ps, pzRes, 1 );
		strcat( szTmpR, szKey ); strcpy( pzRes, szTmpR );
		return strmatch( ts, ps, pzRes+strlen(pzRes), 1 );
	}
	else  // case ?..?
	{
		k = strlen(szTmpS)-strlen(szTmpD);
		if ( k == 0 )
		{
			strcpy( szTmpR, szTmpS );
			if ( pzRes==NULL ) return strmatch( ts, ps, pzRes, 1 );
			strcat( szTmpR, szKey ); strcpy( pzRes, szTmpR );
			return strmatch( ts, ps, pzRes+strlen(pzRes), 1 );
		}
		else if ( k<0 )
		{
			if ( pzRes==NULL ) return strmatch( ts, ps, pzRes, 1 );
			k=-k; n=1; for(i=0; i<k; i++) n*=10;
			sprintf( szTmpR, "%s%0*d", szTmpS, k, rand()%n );
			strcat( szTmpR, szKey ); strcpy( pzRes, szTmpR );
			return strmatch( ts, ps, pzRes+strlen(pzRes), 1 );
		}
		else  // k>0
		{
			n = strlen(szTmpD);
			if (nSign==1)
				strncpy(szTmpR, szTmpS, n);
			else
				strncpy(szTmpR, szTmpS+k, n);
			szTmpR[n]=0;
			if ( pzRes==NULL ) return false;
			strcat( szTmpR, szKey ); strcpy( pzRes, szTmpR );
			return strmatch( ts, ps, pzRes+strlen(pzRes), 1 );
		}
	}

	return true;
}


//
//#define		STD_RNGE_ENUM		0
//#define		STD_RNGE_CHECK		1
//#define		STD_RNGE_GNRT		2
//Standard Range Expression Processing
//

#define		SRCHRANGE( ps, qs )		qs[0]=0; while ( ps[0] && ps[0]<=32) ++ps;\
	if ( ps[0]=='"' || ps[0]=='\'' ) { ch=ps[0]; ++ps; } else ch=0;\
	while ( ps[0] && (ch?(ps[0]!=ch):(ps[0]!='-' && ps[0]!=',' && ps[0]!='[' && ps[0]!=']'))) (qs++)[0]=(ps++)[0];\
	qs[0]=0; if (ch && ps[0]==ch) ++ps;
		
//
//
//#define		TESTING		1
 
bool StdRngExprPrs( LPTSTR pzTemplt, LPTSTR pzVar, int nOpt, LPTSTR pzRes, int nLmt )
{
	TCHAR szLwr[512], szUpr[512], szTmp[512];
	TCHAR szSfx[512], szPrx[512], szVal[64], *ps, *qs, *ts, *rs, ch;
	bool ss = false, bSng, bCur;
	int nn, np=0, nl=0;

	ps = pzTemplt;
	bCur = (pzVar[0]==0);
	szPrx[0] = 0;

#ifndef		TESTING
	if ( pzRes!=NULL && (nOpt & STD_RNGE_GNRT) ) strcpy(pzRes,"(");

	if (nOpt & (STD_RNGE_GNRT|STD_RNGE_STRICT))
	{
		ps = strchr( pzTemplt, '-' );
		if ( ps!=NULL )
		{
			ts = ps-1; ++ps;
			while ( ts>=pzTemplt && ts[0]!='[' ) --ts;
			while ( ps[0] && ps[0]!=']' ) ps++;
			ss = ( ts>=pzTemplt && ts[0]=='[' && ps[0]==']' );
		}
#endif
		if (!ss)
		{
			if ( pzVar[0]==0 )
			{
				ps = strchr( pzTemplt, ',' );
				if ( ps==NULL )
				{
					if ( pzTemplt[0] )
					{
						// Normal sentence
						// strcpy( pzVar, pzTemplt );
						// For final testing 09/02/04
						strncpy( pzVar, pzTemplt, nLmt ); 
						// pzVar[nLmt]=0;
						return true;
					}
					else
						return false;
				}
				else if ((ps-pzTemplt)<nLmt)
				{	
					strncpy( pzVar, pzTemplt, (ps-pzTemplt) );
					pzVar[ps-pzTemplt]=0;
					return true;
				}
				else
					return false;
//#endif
			}
			else
			{
				// strcpy( pzVar, pzTemplt ); return true;
				if ( (int)strlen(pzVar)>nLmt) return false;
				ps = strstr( pzTemplt, pzVar );
				if ( ps==NULL ) return false;
				ps += strlen(pzVar); 
				while ( ps[0] && (ps[0]==',' || ps[0]<=' ')) ++ps;
				if ( ps[0]==0 ) return false;
				ts = strchr( ps, ',' );
				if ( ts==NULL )
				{
					strncpy( pzVar, ps, nLmt );
					pzVar[nLmt]=0;
					return true; 
				}
				else if ((ts-ps)<nLmt)
				{
					strncpy( pzVar, ps, (ts-ps) );
					pzVar[(ts-ps)]=0;
					return true;
				}
				else
					return false;
			}
		}
		ps = pzTemplt;

#ifndef		TESTING
	}
#else
	return false;
#endif
	
	while ( ps[0] )
	{
		// Parse a range item
		bSng = true;
		qs = szLwr;
		szUpr[0] = 0;

		SRCHRANGE( ps, qs )
		// Prefix processing
		if (ps[0]=='[') 
		{
			++ps; strcpy( szPrx, szLwr );
			qs = szLwr;
			SRCHRANGE( ps, qs )
		}		  

		while ( ps[0] && ps[0]<=32 ) ++ps;
		if ( ps[0]=='-' )
		{
			bSng = false;
			qs = szUpr;	++ps;
			SRCHRANGE( ps, qs )
			while ( ps[0] && ps[0]<=32 ) ++ps;
		}

		np = strlen(szPrx);
		nl = strlen(szLwr);

		if ( szPrx[0] )
		{
			strcpy(szTmp, szLwr); strcpy(szLwr,szPrx); strcat(szLwr,szTmp);
			strcpy(szTmp, szUpr); strcpy(szUpr,szPrx); strcat(szUpr,szTmp);
		}
		
		szSfx[0] = 0;  
		if (ps[0]==']') 
		{ 
			szPrx[0] = 0; ++ps;
			if ( ps[0]>32 && ps[0]!=',' )
			{
				qs = szSfx;
				SRCHRANGE( ps, qs )
				while ( ps[0] && ps[0]<=32 ) ++ps;
				if ( szSfx[0] )
				{
					strcat( szLwr, szSfx );
					strcat( szUpr, szSfx );
				}
			}
		}
		
		while ( ps[0] && ps[0]<=32 ) ++ps;
		if ( ps[0] && ps[0]!=',' ) { ss = false; break; } else if (ps[0]==',') ++ps;

		// Obtaining a range item
		if ( bCur && (nOpt & STD_RNGE_ENUM) )
		{
			strcpy( pzVar, szLwr );
			ss = true; break;
		}
		else if ( (nOpt & STD_RNGE_GNRT) && pzRes!=NULL )
		{
			if ( bSng )
				sprintf(szTmp,"(%s='%s')", pzVar, szLwr);
			else
				sprintf(szTmp,"(%s>='%s' and %s<='%s')", pzVar, szLwr, pzVar, szUpr);

			if ( pzRes[strlen(pzRes)-1]=='(' )
				strcat( pzRes, szTmp );
			else
			{
				strcat( pzRes, " or " );
				strcat( pzRes, szTmp );
			}
		}
		else if ( (nOpt & STD_RNGE_CHECK) || (nOpt & STD_RNGE_ENUM) )
		{
			if ( bSng )
			{			  
				ss = (strcmp( pzVar, szLwr )==0 && strlen(pzVar)==strlen(szLwr) );
				if (!ss && isdigit(pzVar[np]) && strlen(pzVar)==strlen(szLwr) ) 
					ss = (atoi(pzVar+np)==atoi(szLwr+np));
				if (!ss && (nOpt & STD_RNGE_CHECK) && (strchr(szLwr,'?')!=NULL || strchr(szLwr,'*')!=NULL))
				{
					ss = strmatch( pzVar, szLwr, NULL );
				}
			}
			else
			{
				ss = (strcmp(pzVar, szLwr)>=0 && strcmp(pzVar, szUpr)<=0 && strlen(pzVar)==strlen(szLwr));
				if (!ss && isdigit(pzVar[np]) && strlen(pzVar)==strlen(szLwr)) 
					ss = (atoi(pzVar+np)>=atoi(szLwr+np) && atoi(pzVar+np)<=atoi(szUpr+np));
			}

			if (ss)
			{
				if ( nOpt & STD_RNGE_CHECK ) break;
				if ( bSng ) { ss = false; bCur = true; }
				else
				{
					if ( isdigit(pzVar[np]) )
					{
						// Numerical Range, such as 0001-0013
						sprintf( szTmp, "%0*lu", nl, atoi(pzVar+np)+1 );
						memcpy( pzVar+np, szTmp, nl );
						if ( atoi(szTmp) <= atoi(szUpr+np) ) break; 
						else { ss = false; bCur = true; }
					}
					else
					{
						// Mixed Range, such as TEST01-TEST03
						strncpy( szTmp, pzVar, sizeof(szTmp)-1 );
						ts = szTmp+np;
						while ( ts[0] && !isdigit(ts[0]) ) ++ts;
						if ( ts[0]==0 ) 
						{	// No numbers in given range, find the first differential char
							ts = szTmp, rs = szUpr;
							while ( ts[0] && ts[0]==rs[0] ) { ++ts; ++rs; }
							if ( ts[0]==0 ) { ss = false; bCur = true; }
							else { ts[0]++; strcpy( pzVar, szTmp );	break; }
						}
						else
						{
							// 
							nn = 0; rs = ts; 
							while ( rs[0] && isdigit(rs[0]) ) { ++rs; ++nn; }
							sprintf( szVal, "%0*lu", nn, atoi(ts)+1 );
							memcpy( ts, szVal, nn );
							if ( strcmp( szTmp, szUpr )<=0 ) 
							{ strcpy( pzVar, szTmp );	break; }
							else { ss = false; bCur = true; }
						}
					}
				}
			}
		}
	}

	if ( (nOpt & STD_RNGE_GNRT) && pzRes!=NULL )
	{
		if ( pzRes[strlen(pzRes)-1]=='(' )
			strcat( pzRes, " true )" );
		else
			strcat( pzRes, ")" );
	}

	return ss;
}


#define		MAX_DTF_BUF				2048

#define		DELTA(p,b)		(p>=b?p-b:MAX_DTF_BUF-b+p)

#define		CLSLINE(f,x,r)	{ if (f!=NULL && x[0] ) fprintf(f,"*%d|%s",r,x); delete x; x=NULL; }

#define		CLEARBUF(p,b,ps,r)	if (b<p) { for(k=b;k<p;k++) CLSLINE(fpout,ps[k],r) } \
							else if (b>p) {  for(k=b; k<MAX_DTF_BUF; k++) CLSLINE(fpout,ps[k],r) \
									for(k=0; k<p; k++) CLSLINE(fpout,ps[k],r) }

#define		CROSCHK(x,y,b,p)	i=b; ss=false; while(!ss && i!=p) \
					{ss = (strcmp(x[i],y)==0); if (ss) x[i][0]=0; if(++i>=MAX_DTF_BUF) i=0; }

#define		ADDLINE(x,y,p)		if(x[p]!=NULL) delete x[p]; \
					x[p]=new TCHAR[strlen(y)+20], strcpy(x[p],y); \
					if (++p>=MAX_DTF_BUF) p=0;

#define		SHREKLST(ps1,ps2,n1,n2,p1,p2,b1,b2,r1,r2)		n1+=DELTA((i-1),b1); n2+=DELTA(p2,b2); ++nn;\
					CLEARBUF(i,b1,ps1,r1); CLEARBUF(p2,b2,ps2,r2);\
					b1=i; b2=p2;

#define		SHREKONE(ps,n,p,b,r)	n+=DELTA(p,b); CLEARBUF(p,b,ps,r); b=p;


//#define		DTF_TMPLT_2TO1			0x0001
//#define		DTF_TMPLT_1TO2			0x0002
//#define		DTF_TMPLT_COMM			0x0004


// Check the difference between two files
int	DiffTextFile( LPTSTR pzFile1, LPTSTR pzFile2, LPTSTR pzRes, int nOpt )
{
	FILE *fp1, *fp2, *fpout;
	int i, k, n1=0, n2=0, nn=0, b1=0, b2=0, p1=0, p2=0;
	BOOL ss, s1, s2;
	TCHAR szBuf1[4000], szBuf2[4000];
	LPTSTR pzSv1[MAX_DTF_BUF], pzSv2[MAX_DTF_BUF];


	fp1 = fopen( pzFile1, "r" );
	if ( fp1==NULL ) return 0;

	fp2 = fopen( pzFile2, "r" );
	if ( fp2==NULL ) { fclose(fp1); return 0; }

	if ( pzRes!=NULL && pzRes[0] && (nOpt & DTF_TMPLT_COMM) )
	{
		fpout = fopen( pzRes, "w" );
	}
	else
		fpout = NULL;

	for(i=0; i<MAX_DTF_BUF; i++) { pzSv1[i]=NULL; pzSv2[i]=NULL; }

	// start check
	while ( !feof(fp1) || !feof(fp2) )
	{
		szBuf1[0]=0;
		while (!(s1 = feof(fp1)))
		{
			fgets( szBuf1, sizeof(szBuf1)-1, fp1 );
			szBuf1[sizeof(szBuf1)-1]=0;
			// Removal of empty line
			if ( szBuf1[0] && (strspn(szBuf1, " \t\r\n")!=strlen(szBuf1)) ) break;
		}
		
		szBuf2[0]=0;
		while (!(s2 = feof(fp2)))
		{
			fgets( szBuf2, sizeof(szBuf2)-1, fp2 );
			szBuf2[sizeof(szBuf2)-1]=0;
			// Removal of empty line
			if ( szBuf2[0] && (strspn(szBuf2, " \t\r\n")!=strlen(szBuf2)) ) break;
		}

		if ( s1 && !s2 )
		{
			if ( p1==b1 ) 
			{ 
				if ( p2!=b2 )
				{
					SHREKONE(pzSv2,n2,p2,b2,2);
				}
				++n2; if (fpout!=NULL) fprintf(fpout,"*2|%s",szBuf2); 
			}
			else
			{
				CROSCHK(pzSv1, szBuf2, b1, p1)
				if ( ss )
				{	
					// Found match pzSv1[i]-szBuf2
					SHREKLST(pzSv1,pzSv2,n1,n2,p1,p2,b1,b2,1,2)
					if ( fpout!=NULL ) fprintf( fpout, szBuf2 );
				}
				else
				{
					ADDLINE( pzSv2, szBuf2, p2 );
				}
			}
		}
		else if ( s2 && !s1 )
		{
			if ( p2==b2 ) 
			{ 
				if ( p1!=b1 )
				{
					SHREKONE(pzSv1,n1,p1,b1,1);
				}
				++n1; if (fpout!=NULL) fprintf(fpout,"*1|%s",szBuf1); 
			}
			else
			{
				CROSCHK(pzSv2, szBuf1, b2, p2)
				if ( ss )
				{	
					// Found match pzSv2[i]-szBuf1
					SHREKLST(pzSv2,pzSv1,n2,n1,p2,p1,b2,b1,2,1)
					if ( fpout!=NULL ) fprintf( fpout, szBuf1 );
				}
				else
				{
					ADDLINE( pzSv1, szBuf1, p1 );
				}
			}
		}
		else if (!s1 && !s2)
		{
			if ( strcmp(szBuf1, szBuf2)==0 )
			{
				++nn;			
				SHREKONE(pzSv1,n1,p1,b1,1);
				SHREKONE(pzSv2,n2,p2,b2,2);
				if ( fpout!=NULL ) fprintf( fpout, szBuf1 );
			}
			else
			{
				CROSCHK(pzSv1, szBuf2, b1, p1)
				if ( ss )
				{	
					// Found match pzSv1[i]-szBuf2
					SHREKLST(pzSv1,pzSv2,n1,n2,p1,p2,b1,b2,1,2)
					ADDLINE( pzSv1, szBuf1, p1 );
					if ( fpout!=NULL ) fprintf( fpout, szBuf2 );
				}
				else
				{
					CROSCHK(pzSv2, szBuf1, b2, p2)
					if ( ss )
					{	
						// Found match pzSv2[i]-szBuf1
						SHREKLST(pzSv2,pzSv1,n2,n1,p2,p1,b2,b1,2,1)
						ADDLINE( pzSv2, szBuf2, p2 );
						if ( fpout!=NULL ) fprintf( fpout, szBuf1 );
					}
					else
					{
						ADDLINE( pzSv1, szBuf1, p1 );
						ADDLINE( pzSv2, szBuf2, p2 );
					}
				}
			}
		}
	}
	SHREKONE(pzSv1,n1,p1,b1,1);
	SHREKONE(pzSv2,n2,p2,b2,2);
	fclose(fp1); 
	fclose(fp2);

	if ( fpout!=NULL ) fclose( fpout );

	for(i=0; i<MAX_DTF_BUF; i++) 
	{ 
		if ( pzSv1[i]!=NULL) delete pzSv1[i]; 
		if ( pzSv2[i]!=NULL) delete pzSv2[i];
	}

	if ( nOpt & DTF_TMPLT_1TO2 ) return n1;
	if ( nOpt & DTF_TMPLT_2TO1 ) return n2;
	return nn;
}