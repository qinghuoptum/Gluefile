// RTFProc.h: interface for the CRTFProc class.
//
//////////////////////////////////////////////////////////////////////

/*************************************************************************************\
  CWRTFDoc is a RTF Document Utility Class programmed 

  Usage:
  
  void HiddenFields()
       * Set all fields (within <* >) to be hidden text.

  void ReplaceFields( LPCTSTR pszParameters ) 
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	   * Replace ceitain fields within a RTF File with given string.
  
	Replaceed data format				 pszParameters' format
	within the RTF File 				 Parameters import from outside	

    <*AcctNo 08 1 1>12345678			 "AcctNo=87654321|JobNo=1234|..."
	 ------- --	- -	--------			 ------- --------
	 A  	 B	C D	E					 F  	   G
	~~~~~~~~~~~~~~~~~~~~~~~~			 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
           9 - ScanSheet Circled Expression
		   
	 D : Character to be pad to the Replace String accordingly 

	 E : Current Value String in the story

     F : Key String to be searched in the story

	 G : Replace String to  replace F

	if (C==5,6,7,8), the hidden field will take the format of
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
	%d 	Day of month as decimal number (01 � 31)
	%H 	Hour in 24-hour format (00 � 23)
	%I 	Hour in 12-hour format (01 � 12)
	%j 	Day of year as decimal number (001 � 366)
	%m 	Month as decimal number (01 � 12)
	%M 	Minute as decimal number (00 � 59)
	%p 	Current locale�s A.M./P.M. indicator for 12-hour clock
	%S 	Second as decimal number (00 � 59)
	%U 	Week of year as decimal number, with Sunday as first day of week (00 � 53)
	%w 	Weekday as decimal number (0 � 6; Sunday is 0)
	%W 	Week of year as decimal number, with Monday as first day of week (00 � 53)
	%x 	Date representation for current locale
	%X 	Time representation for current locale
	%y 	Year without century, as decimal number (00 � 99)
	%Y 	Year with century, as decimal number
	%z, %Z  Time-zone name or abbreviation; no characters if time zone is unknown
	%% 	Percent sign

    As in the printf function, the # flag may prefix any formatting code. In that case, the meaning of the format code is changed as follows.
	Format Code Meaning 
	%#a, %#A, %#b, %#B, %#p, %#X, %#z, %#Z, %#% # flag is ignored. 
	%#c Long date and time representation, appropriate for current locale. For example: �Tuesday, March 14, 1995, 12:41:29�. 
	%#x Long date representation, appropriate to current locale. For example: �Tuesday, March 14, 1995�. 
	%#d, %#H, %#I, %#j, %#m, %#M, %#S, %#U, %#w, %#W, %#y, %#Y Remove leading zeros (if any). 
	
	b) sprintf format:
	
	%[flags] [width] [.precision] [{h | l | I64 | L}]type, see VC++ manual for detail
	
	c) sscanf format:

	%[*] [width] [{h | l | I64 | L}]type, see VC++ manual
	
	<*AcctNo 08 1 1> will be taken hidden text format in the WordFile

    d) ScanSheet Format:

    M:###, Code for marked, U:###, Code for unmarked,
    L:A[-Z], Specify the row for a specific letter,
	

\*************************************************************************************/




#if !defined(AFX_RTFPROC_H__AD9F5AD7_D3CB_11D3_A70A_000021E20B2D__INCLUDED_)
#define AFX_RTFPROC_H__AD9F5AD7_D3CB_11D3_A70A_000021E20B2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <asptlb.h>         // Active Server Pages Definitions


#define		RTF_ACT_ONCE		0x00000001
// File may contain multiple fields, replace one and cancel it afterwards
#define		RTF_ACT_FIX			0x00000002
// Cancel all hidden fields without a field and take out \par from hidden section
#define		RTF_ACT_SHOW_KEY	0x00000004
// Show all Hidden Text (Key Name), pszFlds gives DispBuf
#define		RTF_ACT_SHOW_VAL	0x00000008
// Show all Fields Value, pszFlds gives DispBuf


#define		RTF_CLS_HF			0x00000010
// Clear the Header/Footer
#define		RTF_CLS_HIDE		0x00000020
// Clear the Hidden Text
#define		RTF_CLS_PICT		0x00000040
// Clear the Pictures
#define		RTF_CLS_FLDS		0x00000080
// Clear the user defined fields


#define		RTF_EXP_HTML		1
#define		RTF_EXP_TEXT		2
#define		RTF_EXP_LYTX		3
#define		RTF_EXP_WORD		4
#define		RTF_EXP_WP51		5


#define		RTF_EXTFUNC_RMVREST			0x00000001
#define		RTF_EXTFUNC_BLNKCHK			0x00000002
#define		RTF_EXTFUNC_RMVKEYW			0x00000004
#define		RTF_EXTFUNC_RMVSPEC1		0x00000008
#define		RTF_EXTFUNC_RMVSPACE		0x00000010
#define		RTF_EXTFUNC_KWORDCHK		0x00000020
#define		RTF_EXTFUNC_RMVLSTLN		0x00000040
#define		RTF_EXTFUNC_CHKSPACE		0x00000080



#define ecOK 0                      // Everything's fine!
#define ecFindFooter 1
#define ecFonttbl 2
#define ecFont 3
#define ecViewkind 4
#define ecFindHeader 5
#define ecEndOfFile  7       // End of file reached while reading RTF
#define ecShape  8       


typedef struct tagFIELDKEY
{
	int nType;   // 1->Length, 2->Bracket, 3-NameOnly
	char szKeyName[200];
	int nPos;
	int nLen;
	LPTSTR pzNamePos;
	LPTSTR pzLenPos;
	int nValLen;
	char cPad;
	char cFmtCode;
	char szFmtStr[200];
} FIELDKEY;


typedef struct tagFIELDVAL
{
	int nType;   // 1->Length, 2->Bracket, 3-NameOnly
	int nSrcPos;
	int nSrcLen;
	int nTgtLen;
	char szTarget[4000];
} FIELDVAL;




class CRTFFont: public CObject
{
public:
	CRTFFont();
public:
		CString fontnumber;
		CString fontname;
		CString Oldfontnumber;
};

typedef CTypedPtrArray<CObArray,CRTFFont*> CRTFFontArray;

class CRTFProc  
{
public:
	char szSourceFile[256], szDestinationFile[256], szOrginalFile[256];

	int InsertSignPic( LPCTSTR signPicFileName, int signatoryID, int nChkOnly=0 );
	int InsertSignPic( LPCTSTR signPicFileName, LPTSTR pzPic, LPTSTR pzDate, int nChkOnly=0 );
	CRTFProc();
	CRTFProc(char * SourceFileName,char * DestinationFileName);

	BOOL GenerateTextFile( LPCTSTR pszInputParams, LPCTSTR pszOutputParams, DWORD nLength );
	BOOL FieldsReplace( LPCTSTR pszParameters, int nOnce=0 );
	int FormatAdjustEx( LPCTSTR pszTgtStr, LPTSTR m_szBuf, long& m_total );
	BOOL LinesCount( LPCTSTR pszInputParams, LPCTSTR pszOutputParams, DWORD nLength );
	BOOL FieldsSearch( LPCTSTR pszInputParams, LPCTSTR pszOutputParams, DWORD nLength );
	BOOL FieldsValue(LPCTSTR pszInputParams, LPCTSTR pszOutputParams, DWORD nLength);
	int ParseRtfKeyword(FILE *fp);
	BOOL GetRTFHeader();
	BOOL HeaderExist();
	BOOL SetRTFHeader();
	int  CheckValidate( char* szFile );
	int  ExtendFunction( LPTSTR pszKey, int nOpt );
	void ClearFields( int nOpt = (RTF_CLS_FLDS|RTF_CLS_HF) );
	BOOL RtfExport( int nFmt=1, LPTSTR pzRes=NULL );
	BOOL RtfImport( int nFmt=1, LPTSTR pzRes=NULL );
	void InsertRTFBarCode( LPTSTR pszFlds );

	CComPtr<IResponse> m_piResponse;				//Response Object

	virtual ~CRTFProc();

protected:
	BOOL DuplicationFile(FILE *SourceFile,FILE *DestFile);

	// SubFunctions for FieldsReplace
	BOOL ScanString( LPCTSTR inputString, int index, char* pszKey, char* pszValue );
    BOOL PadString( CString& csTarget, int nStrLength, int nPadType, char chPad );

	// SubFunctions for LinesCount
//    int  SpcLinesInString( LPCTSTR pszBuf );
//	BOOL GetLineCountParam( LPCTSTR pszParam, char sch, int* pnValue, int* pnPercVal );

	// SubFunctions for FieldsSearch
    BOOL GetSearchInOneRange( LPCTSTR pszParameters, LPCTSTR pszRange, LPCTSTR pszOutput, DWORD nOutputSize );
    BOOL GetFieldValueInOneRange( LPCTSTR pszParameters, LPCTSTR pszRange, LPCTSTR pszOutput, DWORD nOutputSize );
	BOOL AddRtfFontTable();
	BOOL RebuildRtfFont();

private:
	FILE *fpTempFile;
	FILE *fpSourceFile;
	FILE *fpDestinationFile;
	CRTFFontArray HeadFontArray;
	CString fontnumber;
	CString fontname;
	CString TempString;
	char szOldFile[256];
	char szTempFile[256];

    bool GetHiddenSegment( int& i, LPTSTR pzBuf, int nLmt, FIELDKEY *ptr );
    bool ParseOneFieldKey( LPTSTR pzBuf, FIELDKEY *ptr );
    bool GetOneFieldValue( int& i, LPTSTR pzBuf, int nLmt, FIELDKEY *ptrk, FIELDVAL *ptrv, bool bCopy );
    bool GetOneSourceValue( LPTSTR pzPrm, FIELDKEY *ptrk, FIELDVAL *ptrv, int nOnce );


	int nLmt;
	LPTSTR m_szBuf;
};


/////////////////////////////////////////////////////////////////////

#define		BARC_OPT_WITH_CHAR		0x00000001
#define		BARC_OPT_LONGER			0x00000002
#define		BARC_OPT_LONGEST		0x00000004
#define		BARC_OPT_TIGHTER		0x00000008
#define		BARC_OPT_TIGHTEST		0x00000010
#define		BARC_OPT_POSTNET		0x00000100

void GeneBar( char *pzCodeStr, char *pzDataBuf, int nLen=8000, int nOpt = BARC_OPT_WITH_CHAR ); 

int CreateACTable( char* pzOrgFile, char* pzTargetFile );

#endif // !defined(AFX_RTFPROC_H__AD9F5AD7_D3CB_11D3_A70A_000021E20B2D__INCLUDED_)
