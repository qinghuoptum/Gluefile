//////////////////////////////////////////////////////////////////////
//
// WRTFDoc.h: interface for the CWRTFDoc class.
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
		   9 - Standard UniPipe Export Format

	 D : Character to be pad to the Replace String accordingly 

	 E : Current Value String in the story

     F : Key String to be searched in the story

	 G : Replace String to  replace F

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



#if !defined(AFX_WRTFDOC_H__19F706AF_104E_11D4_92C8_005004A914E0__INCLUDED_)
#define AFX_WRTFDOC_H__19F706AF_104E_11D4_92C8_005004A914E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


// A RTF file can be partitioned into RTFNodes, each Node can be of the following:

#define		RN_SEGMENT		0x0001		// Any conetent within {...}
#define		RN_ATTRIBUTE	0x0002		// a segment such as '\***\*** '
#define		RN_TEXT			0x0004		// Regular content until next \***

// the name of a RTFNode is the name of first item (for RN_ATTRIBUTe) or 'RNText'
// for RN_TEXT 

typedef struct tagRTFNode
{
	short int m_nType;
	long m_nPos;  // the start position 
	long m_nLen;  // the length of this node
	CHAR m_szName[20];  
} RTFNODE;



typedef struct tagLCSTAT
{
	CHAR m_szType[20];
	int  m_nLevel;
	int  m_nLines;
	int  m_nPages;
	int  m_nLastLn;
	int  m_nChar;
	int  m_nCode;
	int  m_nByte;
} LCSTAT;



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
#define		RTF_CLS_SPC			0x00000100
// Clear all unnecessary space


#define		RTF_CONV_NONE			0x00000100
#define		RTF_CONV_WORD			0x00000200
#define		RTF_CONV_TEXT			0x00000400
#define		RTF_CONV_WP51			0x00000800


#define		RTF_FMTADJ_CTR			0x00000001
#define		RTF_FMTADJ_CLS			0x00000002


#define		RTF_EXTFUNC_RMVREST			0x00000001
#define		RTF_EXTFUNC_BLNKCHK			0x00000002
#define		RTF_EXTFUNC_RMVKEYW			0x00000004
#define		RTF_EXTFUNC_RMVSPEC1		0x00000008
#define		RTF_EXTFUNC_RMVSPACE		0x00000010
#define		RTF_EXTFUNC_KWORDCHK		0x00000020
#define		RTF_EXTFUNC_RMVLSTLN		0x00000040
#define		RTF_EXTFUNC_CHKSPACE		0x00000080
// for RMVSPACE and CHKSPACE, first field gives the limits, such as  "20" 

class CWRTFDoc  
{
public:
	BOOL LabelGenerate( LPCTSTR pszDoc, LPCTSTR pszTmplt, LPTSTR pszParam );
	BOOL CountLines( LPCTSTR pszFmt, int* nLines, int* nPages );
	void WriteFile( LPCTSTR pszFileName );
	BOOL ReadFile( LPCTSTR pszFileName );
	void ExportFile( LPCTSTR pszFileName );
	void ProcessFile( LPCTSTR pszFileName, char chOpt, LPTSTR pzParm );

	void HiddenFields( );
	void DeleteFields( );
	int FormatAdjustEx( LPCTSTR pszTgtStr );
	void FormatAdjust( LPCTSTR pszFileName,  int nOpt = 0 );
	void InsertProperty( LPTSTR pzPropStr, int nOpt=0 );
	void SetProtect( int nOpt );
	void ClearFields( int nOpt = (RTF_CLS_HF|RTF_CLS_HIDE) );
	void ClearFieldsA(LPCTSTR pszFileName, int nOpt = (RTF_CLS_HF|RTF_CLS_HIDE) );
	int  ReplaceFields( LPTSTR pszFlds, int nOnce=0 );
	void RetrieveField( LPTSTR pszKey, LPTSTR pszValue, int nVLen, BOOL nOption = 0, void* ptr = NULL );
	void RetrieveField2( LPTSTR pszKey, LPTSTR pszValue, int nVLen, BOOL nOption = 0, void* ptr = NULL );
	void RetrieveField3( LPTSTR pszKey, LPTSTR pszValue, int nVLen, BOOL nOption = 0, void* ptr = NULL );
	void TestNodes( );
	void InsertSignPic( LPCTSTR signPicFileName, LPCTSTR szKeyString, int nRvLns=0 );
	void InsertBarCode( LPTSTR pszFlds );
	int  ExtendFunction( LPTSTR pszKey, int nOpt=0 );

	CWRTFDoc();
	virtual ~CWRTFDoc();

	static void MergeRTFFile( LPCTSTR pzDesFile, LPCTSTR pzSrcFile );
	void MergeFromText( LPCTSTR pzRtfFile, LPCTSTR pzTxtFile );

protected:
	long ReplaceOneFlds( long nPos, long nOL, long nNL, long nTmp, LPTSTR psTmp );
	BOOL GetNextNode( long nPos, long nLmt, RTFNODE *pRNode );
	LONG m_limit;
	BOOL m_res;
	LPTSTR m_szBuf;
	CFile m_file;
	LONG  m_total, m_start;

	void CountSegment( LPTSTR pStart, LPTSTR *pEnd, LCSTAT *pRes, int nLevl );
};


class CPatParse;

extern bool CHKHIDDEN(int i, LPTSTR ps );

extern bool RTFExport( LPTSTR pszSrcFile, LPTSTR pszDestFile, LPTSTR pszData, CPatParse* m_pParse, int* nOpt=NULL, LPTSTR pszResult=NULL );

extern bool RTFImport( LPTSTR pszSrcFile, LPTSTR pszData, CPatParse* m_pParse );

/////////////////////////////////////////////////////////////////////

#define		BARC_OPT_WITH_CHAR		0x00000001
#define		BARC_OPT_LONGER			0x00000002
#define		BARC_OPT_LONGEST		0x00000004
#define		BARC_OPT_TIGHTER		0x00000008
#define		BARC_OPT_TIGHTEST		0x00000010
#define		BARC_OPT_POSTNET		0x00000100

void GeneBar( char *pzCodeStr, char *pzDataBuf, int nLen=8000, int nOpt = BARC_OPT_WITH_CHAR ); 

int CreateACTable( char* pzOrgFile, char* pzTargetFile );


#endif // !defined(AFX_WRTFDOC_H__19F706AF_104E_11D4_92C8_005004A914E0__INCLUDED_)
