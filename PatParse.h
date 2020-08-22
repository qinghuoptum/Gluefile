// PatParse.h: interface for the CPatParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATPARSE_H__18BF5413_25D5_11D4_AB1F_0010B508CE3E__INCLUDED_)
#define AFX_PATPARSE_H__18BF5413_25D5_11D4_AB1F_0010B508CE3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXNUMFLDS      400
#define MAXRPTLEN       120000
#define	MAXKEYLEN		32

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes


#ifdef		COMPLETE_VER
#include	"XMLBase.h"
#include	"LogFile.h"
#endif

/*
#ifndef		BOOL
#define		BOOL	int
#endif

#ifndef		CHAR
#define		CHAR	char
#endif

#ifndef		LPTSTR
typedef		char*			LPTSTR;
#endif

#ifndef		LPCTSTR
typedef		const char*		LPCTSTR;
#endif

#ifndef		TRUE
#define		TRUE	1
#endif		

#ifndef		FALSE
#define		FALSE	0
#endif		

#ifndef		NULL
#define		NULL	0
#endif		

#ifndef		UINT
typedef		unsigned int	UINT;
#endif		
*/

#if !defined( PATPARSE_DEF )

#define		PATPARSE_DEF	1
#define		MAXGROUP		12

typedef struct tagTFLItem
{
	char Events[5];
	char KeyName[20];
	char Type[14];
	char SubType[14];
	char StartPos[20];
	char Length[8];
	char Delimitor[8];
} TFLITEM;



#define		OPT_TFLGRP_CNTLMT		0x00000001		// Record Count limited
#define		OPT_TFLGRP_FLLWED		0x00000002		// To be followed by more group	 with '#' marks
#define		OPT_TFLGRP_RPTSTRT		0x00000004		// Start point of repeated froups  '$'
#define		OPT_TFLGRP_2VALID		0x00000008		// This group used until invalid
#define		OPT_TFLGRP_SKIP			0x00000010		// This record will be ignored


typedef struct tagTFLGROUP
{				   	  
	char Name[20];
	char IdStr[256];
	char Class[10];
	char Delimitor[40];
	int  nFields;
	TFLITEM Fields[MAXNUMFLDS];
	int  nCurr;		// Indicates the current group for sequential record reading
	int  nEffect;   // Indicates the current effect group to parse the current record
	int  nReqCnt, nPrsCnt;  // Required record count and processed record count
	int  nOption;
} TFLGROUP;


typedef struct tagEFTItem
{	
	char Type;  // N: NonLeafKeyword, L: LeafTitle, K: KeyName, T: Terminator
	LPTSTR Keyword;
	LPTSTR Replace;
	int  level; 
	int  next;
	int  child;
} EFTITEM;


typedef struct tagDBFSTRUCT
{
	int nPos;  // Start Position of each field
	int nLen;  // Length of each field
	char szName[20];  // Name of the each field
	char chType;  // Type of field
} DBFSTRUCT;



typedef struct tagSTRLIST {
	int nCnt;
	TCHAR szStrL[32];
} STRLIST;


#endif  // !defined( PATPARSE_DEF )


#if	defined(XML_DEF)

extern CXMLFile* gpXMLObj;
extern int gnXMLCnt;

bool CreateXMLObj();
void ReleaseXMLObj();
	
#endif	// defined(XML_DEF)



#define		HL7_FORMAT		0x0003
#define		TFL_FORMAT		0x0004



#define		IMP_REC_END			0x0000
#define		IMP_REC_NORMAL		0x0001
#define		IMP_REC_NONE		0x0002
#define		IMP_REC_MORE		0x0005


int ParseWord(char* cbuf);
BOOL ParseName( char *inBuf, char* pszLast, char* pszFirst, char* pszMid, char* pszTitle, char* pszCode=NULL );
long AdjustTime(time_t inTm, char *outBuf, char *adjustExp );
long ParseDateTime( char *inBuf, char *outBuf, int nDMY=0 );
long ParseCOleDateTime(char *inBuf, char *outBuf);
void ParseSectionByRule( char *pszRule, char *pszSection, char *pszLine, int nLineNo = -1 );
BOOL FindSectionEnding( char *pszRule, char *pszLine );
int GetCTimeStr( char* pszBuf, char* pszFmt, char* pszVal );
int GetChoiceStr( char* pszBuf, char* pszFmt, char* pszVal );
int GetDelimitedStr( char* pszVal, char* pszFormat, char* pszBuf );
// bool GetSysConst( char* pszVal, int nLmt, char* pszTmp );
int FormatForExp( char* pszNew, char* pszOld, char* szFormat, char* m_szWorkDir=NULL, char* szKTmp=NULL );
LPTSTR SrchByScanFmt(LPTSTR pzBuf, LPTSTR pzFmt, int nPos=0);
bool SplitFileName( char *pszFullPath, char *pszType, char *pszVal );
bool GetPrefixValue( char *pszSource, char *pszType, char *pszVal );


// Search the token to find a segment
int GetDataSegByExtStr( FILE *fp, LPTSTR pzToken, LPTSTR pzBuf, int nLmt );

// Translate from String to TFLGROUP structre
// Criteria -> [[[+|-][C|D|S]|[^]<String>]:<nnn>|@<String>|#<String>]...
int ValidateSegment( char* pCriteria, char* pData, int nIdx=0, void* pList=NULL );


#define		UPPER(x)	((x>='a' && x<='z')?x-32:x)
#define		LOWER(x)	((x>='A' && x<='Z')?x+32:x)



#define	OPT_NO_HIDDEN		0x00000001
#define OPT_MULTI_SEL		0x00000002
#define OPT_TWO_ITEMS		0x00000004
#define OPT_SET_VARS		0x00000008

//
// Interface Engine, Format Conversion Class
//
class CPatParse  
{
public:
    CPatParse( LPTSTR pszContFile = NULL, LPTSTR pszWorkDir = NULL );
    virtual ~CPatParse();

	// Record Operations
	void InitBuffer( LPTSTR pszContFile = NULL, LPTSTR pszWorkDir = NULL );
    int  GetOneRecord( LPTSTR pszTmplt, LPTSTR pszBuf = NULL, int nOption = 0, int nLen = 0 );
    void SetOneRecord( LPTSTR pszBuf, int nLen, int nOption = 1 );
    void ExpEvaluate( LPCTSTR pszOpt, LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, bool bStr=true );
	// SetAllText support the segments, 
	//    Return Value:  NULL no more segment or Ptr to start of next segment
	//    pszNext (not NULL) will point to the start of next segment
	//    nCriteria (!=0) is limitation: # of lines (<1000) or # of bytes (>1000)
	//    In the segments situation, three variables can be used
	//    <*CurSegmentNo>, <*PrevSegmentNo> and <*NextSegmentNo>
	//
    LPTSTR SetAllText  ( LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, LPTSTR pszBody = NULL, int nCriteria=0 );
	void InsertAllText ( LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, int nOpt=0 );
	void InsertAllTextEx ( LPTSTR pszValue, LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, int nOpt=0 );

	//  Add Extra manipulation on Current Record
	void ExtraManipulate( LPCTSTR pszOperStmt );
	// Selection format: ${<Item>=<Criteria>|...|}
	bool SelectPropItem( LPTSTR pszTmplt, LPTSTR pszBuf, int nLen, int nOpt=0 );

	// Field/Varables Operation
	void CreateEscStr( LPTSTR pDest, int nLmt, LPTSTR pSrc ); 
	void ParseBlockVal( LPTSTR pszBuffer, LPTSTR pzSeparator=NULL, LPTSTR pzDelimitor=NULL, BOOL bGroup=FALSE, BOOL bGlobal=FALSE );
    BOOL GetOneValue( LPTSTR pszKeyName, LPTSTR pszKeyValue, int nLen );
    BOOL GetOneValueIdx( int nIdx, LPTSTR pszKeyName, LPTSTR pszKeyValue, int nLen );
    BOOL GetAllValue( LPTSTR pszKeyValue, int nLen );
    void SetOneValue( LPTSTR pszKeyName, LPTSTR pszKeyValue, int nOver = 0 );
    void SetOneValueEx( LPTSTR pszKeyName, LPTSTR pszKeyValue );
	void InitValues( bool bRelease = true, bool bAll = false );
	void RegisterTask( LPTSTR pszTask );
	
	LPTSTR GetRptContent( ) { return m_pContent; }
	void SetRptContent( LPTSTR pszContent );

	// Regular File Operation
    BOOL OpenFile( LPCTSTR pszFileName, LPCTSTR pszMode = (LPCTSTR)"rb"  );
    BOOL CloseFile( );
	int Rmv1stRecord( LPTSTR pszTmplt, LPTSTR pszFile, int nOption=0 );

	// DBF Operations
	BOOL SetDBFStruct( char *pStruct, int nLen, int nOption = 1 );
	BOOL OpenDBFWrite( LPCTSTR pszFileName );
	BOOL RtvlOneRecord( LPTSTR pszKey, LPTSTR pszValue, LPTSTR pszFieldList=NULL );
	void AddOneRecord( int nOption = 0 );
    BOOL DBFMoveFirst( );
    BOOL DBFMoveNext( );
    BOOL DBFRequery( );
    BOOL DBFUpdate( );
    BOOL DBFEdit( );
    BOOL DBFDelete( );
    BOOL DBFIsEOF( );
    BOOL CheckExpress( LPCTSTR pszExpr );
	void CloseDBFWrite();
	CString m_strFilter;

    // System Buffer
	// TCHAR m_szBuf[MAXRPTLEN];
	TCHAR *m_szBuf;
	
	int m_nBLen;
	bool m_bWantRels;

private:
	// int  ValidateGroup( char* pCriteria, char* pData );
	int ValidateGroup( char* pCriteria, char* pData, int nIdx=0, void* pList=NULL );
	void SetupStrList( char* pCriteria, char* pData );
	void ClearCurVars( );
	BOOL FindProperGroup( bool bValid );
	int GetProperKeyVal( char* pzKName, char* pzKVal, int nLmt, char* pzLine, int& LnNo, int& LnNoT, bool bRept );

    int m_nKeys, g_nKeys;
	int m_nSegment;
    FILE *m_pFile;
	int m_nCurGroup, m_nPrsCnt;
	int m_nIndexA;

	int m_nRecF;  // Index each record
	STRLIST m_StrList[16];

	LPTSTR m_pContent;
    CHAR m_cK1, m_cK2, m_cK3, m_cK4;
	TFLGROUP m_stGroup[MAXGROUP];
	int m_nGroup;

	DBFSTRUCT m_stDBFlds[MAXNUMFLDS];
	int m_nDBFlds, m_nDBFStart, m_nDBFLen;

    FILE *m_pwFile;
	DBFSTRUCT m_stwDBFlds[MAXNUMFLDS];
	int m_nwDBFlds, m_nwRecCnt, m_nwDBFStart, m_nwDBFLen;

	BOOL m_bSaveVar;
	BOOL m_bUseNull;

	TCHAR szVarNameSave[MAXNUMFLDS*40];
	TCHAR gchL;

	time_t m_nGTime;

#if	!defined(XML_DEF)
    TCHAR m_szKeyName[MAXNUMFLDS][MAXKEYLEN];
    LPTSTR m_szKeyValue[MAXNUMFLDS];

    TCHAR g_szKeyName[MAXNUMFLDS][MAXKEYLEN];
    LPTSTR g_szKeyValue[MAXNUMFLDS];
#endif

    BOOL ParseTemplate( LPTSTR pszTmplt );
    BOOL ParseTFLTmplt( LPTSTR pszTmplt );
	void SetTFLData( char* pTFLFormat );
    BOOL CheckHL7Event( LPTSTR pszBuf, LPTSTR pszEvent );

	CHAR m_szContFile[_MAX_PATH];
	CHAR m_szWorkDir[_MAX_PATH];
	CHAR m_szCurTask[32];

protected:
	void AdjustUnstName( char* pszLast, char* pszFirst );

};


//
// Extract Fields from Text File
//
class CEFTParse
{
public:
    CEFTParse();
    virtual ~CEFTParse();

    BOOL ParseOneFile( LPTSTR pszTmplt, LPTSTR pszFile, LPTSTR pzBuf=NULL, int nLmt=0 );
	BOOL SetupTemplate( LPTSTR pszTmplt );
	void ScanText( LPTSTR pszText, bool bReplace=false );
	BOOL ScanTitle( LPTSTR pszText );

	// Field/Variables
    BOOL GetOneValue( LPTSTR pszKeyName, LPTSTR pszKeyValue, int nLen );
    BOOL GetValueByIndex( int nIdx, LPTSTR pszKeyName, LPTSTR pszKeyValue, int nLen );
    void SetOneValue( LPTSTR pszKeyName, LPTSTR pszKeyValue );
	void InitValues( bool bRelease=false );
	void RegisterTask( LPTSTR pszTask );

	bool bWantRels;

private:
	int SearchNode( LPTSTR pszBuf, int nCur );
	int SearchChildren( LPTSTR pszBuf, int nCur );
	int ScanTemplate( LPTSTR pszTmplt, LPTSTR *pszNew, int nLevel );
	LPTSTR ReplaceText( LPTSTR pzHead, LPTSTR pzTail, LPTSTR pzValue );
	// void ClearCurVars( );
	
	CHAR m_szTmplt[8000];
	CHAR m_szCurTask[32];

	EFTITEM m_sNode[250];
	int m_state[10];
	int m_nCurNode, m_nLastNode;
	int m_nLines, m_nChars;
	int m_nLState, m_nCState;
	int nRow, nCol;

	// TCHAR szVarNameSave[MAXNUMFLDS*32];

#if	!defined(XML_DEF)
    CHAR m_szKeyName [MAXNUMFLDS][32];
    LPTSTR m_szKeyValue[MAXNUMFLDS];
#endif
	int m_nKeys;
};



#define		ETYPE_ILLEAGE		0
#define		ETYPE_NUMBER		1
#define		ETYPE_STRING		2
#define		ETYPE_BOOLEAN		3
#define		ETYPE_CHCODE		4
#define		ETYPE_OPERATOR		5


#define		MAX_STACK			40
#define		MAX_ITEM			250


typedef struct tagEXPRES
{
	int nType; //
	TCHAR szRes[MAX_ITEM];
} EXPRES;


class CStdExpress
{
public:
	CStdExpress(){ top = 0; };
	virtual	~CStdExpress(){};
	bool Evaluate( LPTSTR pzExpr, LPTSTR pzResult);
	int ScanOneItem( TCHAR **ppBuf, TCHAR *pWord, int nOpt = 0 );

private:
	EXPRES stack[MAX_STACK];
	int top;
	
	int  Priority( LPTSTR ps );
	bool Compare( EXPRES x, EXPRES y, int& ns );
	bool Compute1( EXPRES x, EXPRES z, EXPRES& u );
	bool Compute2( EXPRES x, EXPRES y, EXPRES z, EXPRES& u );
	bool ComputeTop( int nPrt );

	void Push( EXPRES x ){ if (top<MAX_STACK-1) stack[top++]=x; };
	void Pop( EXPRES& x) { if (top>=0) x=stack[--top]; };
	bool GetTop( EXPRES& x) { bool ss=top>0; if (ss) x = stack[top-1]; return ss; };
	int  GetTopType() { if (top>0) return stack[top-1].nType; else return 0; };
	void GetSec( EXPRES& x) { bool ss=top>1; if (ss) x = stack[top-2]; };
};



#define		MAX_TITLE			256

static char gTitleList[MAX_TITLE][12] = 
{ 
"A/ANP",
"ACNP",
"AGAF",
"ANP",
"ANP-C",
"APN",
"ARRN",
"ARNP",
"BSN",
"CCIV",
"CCM",
"CFNP",
"CF-A",
"CGC",
"CM",
"CNIM",
"CNM",
"CNP",
"CNRP",
"COT",
"CPE",
"CRNF",
"CRNFA",
"CRNP",
"CRS",
"DC",
"DDS",
"DMD",
"DO",
"DPM",
"DR",
"DWDC",
"FACP",
"FACS",
"FACSC",
"FCCP",
"FCP",
"FELLOW",
"FNP",
"FNPC",
"FNP-C",
"FRCPC",
"JD",
"LAC",
"LCSW",
"LLC",
"LPN",
"M.D.F.A.C.",
"MBBS",
"MD",
"MD-FCP",
"MD-PH",
"MFCC",
"MPH",
"MRC"
"MS",
"MSN",
"MSNP",
"MSW",
"NCM",
"ND",
"NP",
"NP-C",
"NPC",
"OCN",
"OD",
"OT",
"P.A.-C",
"PA",
"PA-C",
"PAC",
"PAS",
"PHD",
"PSY",
"PT",
"Ph.D.",
"PhD",
"R#1",
"R#2",
"R#3",
"R#4",
"R-1",
"R-2",
"R-3",
"R-4",
"RCP",
"RDH",
"RN",
"RN-BSN",
"RN/SA",
"RNCNO",
"RNFA",
"RNP",
"RPA-C",
"RPT",
"SA",
"ST",
"STU",
"WHNP",
"\0" 
};

// X. X.X.  X.X.X.  X.X.X.X.

#define		SPLAST(x)	 (x[0]=='M'||x[0]=='O'||x[0]=='S'||x[0]=='L'||x[0]=='V') \
							&& strlen(x)<=3 && x[strlen(x)-1]!='.'


#define		MAX_REPLNUM		128

/********************************************************
General Replace Template:

{[$Regular:]<ReplaceItem>|...|}

$Regular: stands for the UNIX Regular Expression for this template.

  <ReplaceItem> ::= <Exist>=<New>

  for $Regular mode, both <Exist> and <New>	are UNIX R.E.
  (if '|' used inside expression, use "" for certain content)
  for general mode, <Exist> can be <OldVal> or <IdStr>@<OldVal>
  (look for <IdStr> before replace <OldVal>). 


//********************* UNIX Regular Expression Replacement *******************

Regular Expressions (Unix Syntax):

Symbol	Function
\	Marks the next character as a special character. "n" matches the character "n". "\n" matches a linefeed or newline character.
^	Matches/anchors the beginning of line.
$	Matches/anchors the end of line.
*	Matches the preceding character zero or more times.
+	Matches the preceding character one or more times.
.	Matches any single character except a newline character.
(expression)	Brackets or tags an expression to use in the replace command.
	A regular expression may have up to 9 tagged expressions, numbered according 
	to their order in the regular expression.The corresponding replacement expression 
	is \x, for x in the range 1-9.  Example: If (h.*o) (f.*s) matches "hello folks", 
	\2 \1 would replace it with "folks hello".
[xyz]	A character set. Matches any characters between brackets.
[^xyz]	A negative character set. Matches any characters NOT between brackets.
\d	Matches a digit character. Equivalent to [0-9].
\D	Matches a nondigit character. Equivalent to [^0-9].
\f	Matches a form-feed character.
\n	Matches a linefeed character.
\r	Matches a carriage return character.
\s	Matches any white space including space, tab, form-feed, etc but not newline.
\S	Matches any nonwhite space character but not newline.
\t	Matches a tab character.
\v	Matches a vertical tab character.
\w	Matches any word character including underscore.
\W	Matches any nonword character.

************************************************************************************/


class CStdReplace  
{
public:
	int Execute( void* pBuf, int nLen );
	int RegExpReplace( LPTSTR pzSrcExp, LPTSTR pzTarExp, LPTSTR pzBuf, int nLen = 2048 );
	bool SetupTemplate( LPTSTR pzTmplt );
	CStdReplace();
	virtual ~CStdReplace();

private:
	int RegExpCheck( LPTSTR pzBuf, LPTSTR pzExp, TCHAR lch = 0, LPTSTR lps = NULL, int nOcr = 0 );
	int CreateTarBuf( LPTSTR pzExp, LPTSTR pzBuf );

	LPTSTR pzIdStr[MAX_REPLNUM], pzExist[MAX_REPLNUM], pzNew[MAX_REPLNUM];
	TCHAR szQLook[MAX_REPLNUM];
	TCHAR gszVarList[20][MAX_REPLNUM];
	int m_nVar;
	int m_nItem;
	int m_nType;
};



// ===============================================================================
// ===================   Advanced String Processing   ============================
// ===============================================================================



// LCS Finding with different Options
//   0: Initial LCS
//   1: Ending LCS
//   2: Anywhere LCS (without shifting)   
//	 3: Standard LCS (shift to shorter one to match another)
int FindLongestCommonSubstring( LPCTSTR pzStr1, LPCTSTR pzStr2, LPTSTR pzRes, 
							    int nOpt=0, int* p1=NULL, int* p2=NULL );

// Get All possible common substrings bewteen two given strings
int GetAllCommonString( LPCTSTR pzStr1, LPCTSTR pzStr2, int* pnPos, int* pnLen);

// Get Standard All common string w/ shift 
int GetAllCommonStringEx( LPCTSTR pzStr1, LPCTSTR pzStr2, LPTSTR pzRes, TCHAR ch );

// Find a sequence of common strings, with maximum of # of matched chars
int CheckCommonString( LPCTSTR pzStr1, LPCTSTR pzStr2, int* pnPos, int* pnLen, int nn);

// Check and Adjust the CS List	with new two target strings
int GetCommonStrSeq( LPCTSTR pzStr1, LPCTSTR pzStr2, 
					 int* pnPos1, int* pnPos2,  int* pnLen, int& np, int nLmt);

// Get number of appearance of the target string in given string
int GetSubstringNum( LPCTSTR pzStr, LPTSTR pzTar );


// Create its DEBUG-like binary representation block for a string
void CreateBinaryBlock(LPCTSTR psrc, LPTSTR pdest, int nLmt);

// Create a sequence of CS (delimited by <ch>)
void CreateStrSeqByIndex( LPCTSTR pzSrc, LPTSTR pzStr, int* pnPos, int* pnLen, int nn, char dlmt );



// Advanced Name Check Algorithm

#define		OPT_NAMECHK_SWITCH		0x00010000
#define		OPT_NAMECHK_EXACT		0x00020000
#define		OPT_NAMECHK_90PCT		0x00040000
#define		OPT_NAMECHK_80PCT		0x00080000
#define		OPT_NAMECHK_60PCT		0x00100000
#define		OPT_NAMECHK_SHTFM		0x00200000
#define		OPT_NAMECHK_CHONLY		0x00400000
#define		OPT_NAMECHK_CAPTL		0x00800000


bool NameCheckEx( LPCTSTR pzFirst1, LPCTSTR pzFirst2, LPCTSTR pzLast1, LPCTSTR pzLast2, int nOpt, LPTSTR pzRes = NULL ); 



#define		DTF_TMPLT_2TO1			0x0001
#define		DTF_TMPLT_1TO2			0x0002
#define		DTF_TMPLT_COMM			0x0004

int	DiffTextFile( LPTSTR pzFile1, LPTSTR pzFile2, LPTSTR pzRes=NULL, int nOpt=DTF_TMPLT_1TO2 );

//
// Standard Range Expression Processing
// Std Range Expr ::= <RangeItem>,...,
//   <RangeItem> ::= <Value> | <Value>-<Value> | <Prefix>[<Value>-<Value>]<Suffix> |
//   <Value> ::= <String> | "<String>" | Number | %<String>
//
//  
// To enumerate next value, pzVar=''|0 stands for the first entry
#define		STD_RNGE_ENUM		0x0001
// To check given value is in the range of given expr
#define		STD_RNGE_CHECK		0x0002
// To generate the standard boolean expr according to the range items
#define		STD_RNGE_GNRT		0x0004
// To replace certain 

#define		STD_RNGE_STRICT		0x1000

bool StdRngExprPrs( LPTSTR pzTemplt, LPTSTR pzVar, int nOpt, LPTSTR pzRes=NULL, int nLmt=_MAX_PATH-1 );

bool strmatch( LPTSTR pzSrc, LPTSTR pzDest, LPTSTR pzRes, int nSign = 0 );


#define		TCHEXPAND(x,y)		if (x[0]!='\\') (y++)[0]=(x++)[0]; \
								else switch (x[1]) \
								{\
								case 'r': (y++)[0]='\r'; x+=2; break; \
								case 'n': (y++)[0]='\n'; x+=2; break; \
								case 'f': (y++)[0]='\f'; x+=2; break; \
								case 't': (y++)[0]='\t'; x+=2; break; \
								case 's': (y++)[0]=' '; x+=2; break;  \
								case 'b': (y++)[0]='|'; x+=2; break;  \
								case '\\': (y++)[0]='\\'; x+=2; break; \
								default: if (isdigit(x[1])) { (y++)[0]=atoi(x+1); ++x; while (isdigit(x[0])) ++x; } else (y++)[0]=(x++)[0]; \
								}  

#define		TCHUNEXPD(x,y)		if (x[0]>=' ') (y++)[0]=(x++)[0]; \
								else switch (x[0]) \
								{\
								case '\r': (y++)[0]='\\'; (y++)[0]='r'; ++x; break; \
								case '\n': (y++)[0]='\\'; (y++)[0]='n'; ++x; break; \
								case '\f': (y++)[0]='\\'; (y++)[0]='f'; ++x; break; \
								case '\t': (y++)[0]='\\'; (y++)[0]='t'; ++x; break; \
								default: sprintf(y,"\\%02d",x); y+=3; ++x; \
								}  




// For Auto Format
// Search Template in word levels 
// <Target> ::= <MatchItem>|...|<MatchItem>
// <MatchItem> is regulat string with wide character ?(any single character) or *(any string)
//   or $[n] (any no more than n words )
int TargetSearch( LPTSTR pzTemplate, LPTSTR pzTarget, int nOption, LPTSTR pzResults );


#endif // !defined(AFX_PATPARSE_H__18BF5413_25D5_11D4_AB1F_0010B508CE3E__INCLUDED_)
