// TestRTFDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestRTF.h"
#include "TestRTFDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestRTFDlg dialog

CTestRTFDlg::CTestRTFDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestRTFDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestRTFDlg)
	m_szSrc = _T("C:\\TEMP\\TEST1.RTF");
	m_szDest = _T("C:\\TEMP\\TEST2.RTF");
	m_szRepl = _T("Notes=Comments|PatLast=Smith|PatFirst=John|InsBarCode=True|1stICD9=1234567890|2ndICD9=ABCDEFSGG|");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pRtf = NULL;
}

void CTestRTFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestRTFDlg)
	DDX_Text(pDX, IDC_EDIT_SRC, m_szSrc);
	DDX_Text(pDX, IDC_EDIT_DEST, m_szDest);
	DDX_Text(pDX, IDC_EDIT_REPL, m_szRepl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestRTFDlg, CDialog)
	//{{AFX_MSG_MAP(CTestRTFDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_INIT, OnButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_CVT, OnButtonCvt)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_V2W, OnButtonV2w)
	ON_BN_CLICKED(IDC_BUTTON_REPL, OnButtonRepl)
	ON_BN_CLICKED(IDC_BUTTON_INSP, OnButtonInsp)
	ON_BN_CLICKED(IDC_BUTTON_BCDS, OnButtonInsb)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON_V2W8, OnButtonV2w8)
	ON_BN_CLICKED(IDC_BUTTON_RHTML, OnButtonRhtml)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestRTFDlg message handlers

BOOL CTestRTFDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestRTFDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestRTFDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestRTFDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestRTFDlg::OnButtonInit() 
{
	UpdateData( TRUE );

	if ( pRtf==NULL )
	{
		pRtf= new CRTFProc( (LPTSTR)(LPCTSTR)m_szSrc, (LPTSTR)(LPCTSTR)m_szDest );
	}

	//UpdateData( TRUE );
}



void CTestRTFDlg::OnButtonCvt() 
{
	char szTemp[256];

	UpdateData( TRUE );
	if ( pRtf==NULL )
	{

		pRtf= new CRTFProc( (LPTSTR)(LPCTSTR)m_szSrc, (LPTSTR)(LPCTSTR)m_szDest );

		pRtf->ClearFields();
		// Convert RTFObject->szSourceFile -> szDestinationFile
		bool ss = pRtf->RtfExport( 1, szTemp );
		if ( !ss )
		{
			::AfxMessageBox( szTemp );
		}
		else
			AfxMessageBox( "Success!" );

		delete pRtf;
		pRtf = NULL;
	}
}


void CTestRTFDlg::OnButtonRepl() 
{
	UpdateData( TRUE );
	if ( pRtf==NULL )
	{

		pRtf= new CRTFProc( (LPTSTR)(LPCTSTR)m_szSrc, (LPTSTR)(LPCTSTR)m_szDest );

		// pRtf->ClearFields();
		// Convert RTFObject->szSourceFile -> szDestinationFile
		bool ss = pRtf->FieldsReplace( m_szRepl );
		if ( !ss )
		{
			::AfxMessageBox( "Failed" );
		}
		else
			AfxMessageBox( "Success!" );

		delete pRtf;
		pRtf = NULL;
	}
}


void CTestRTFDlg::OnButtonInsp() 
{
	UpdateData( TRUE );
	if ( pRtf==NULL )
	{

		pRtf= new CRTFProc( (LPTSTR)(LPCTSTR)m_szSrc, (LPTSTR)(LPCTSTR)m_szDest );

		// pRtf->ClearFields();
		// Convert RTFObject->szSourceFile -> szDestinationFile
		bool ss = pRtf->InsertSignPic( m_szRepl, 2 );
		if ( !ss )
		{
			::AfxMessageBox( "Failed" );
		}
		else
			AfxMessageBox( "Success!" );

		delete pRtf;
		pRtf = NULL;
	}
}


void CTestRTFDlg::OnButtonInsb() 
{
	UpdateData( TRUE );
	if ( pRtf==NULL )
	{
		pRtf= new CRTFProc( (LPTSTR)(LPCTSTR)m_szSrc, (LPTSTR)(LPCTSTR)m_szDest );

		// pRtf->ClearFields();
		// Convert RTFObject->szSourceFile -> szDestinationFile
		pRtf->InsertRTFBarCode( (LPTSTR)(LPCTSTR)m_szRepl );
		delete pRtf;
		pRtf = NULL;
	}
}


void CTestRTFDlg::OnButtonClose() 
{
	if ( pRtf!=NULL )
	{
		delete pRtf;
		pRtf = NULL;
	}
}


void LogViewOper( char* pszFile, char* pszMode )
{
	FILE *fp;
	
	fp = fopen( "TestLog.txt", "w" );
	fprintf(fp, "%s %s\n", pszMode, pszFile );
	fclose( fp );
}

void CTestRTFDlg::OnButtonV2w() 
{
	char szTemp[256];

	UpdateData( TRUE );
	ConvertVox2Wave( (LPCTSTR)m_szSrc, (LPCTSTR)m_szDest, NULL, 1 ); 
}



#include <stdlib.h>
#include <io.h>



void CTestRTFDlg::OnButtonV2w8() 
{
	// TODO: Add your control notification handler code here
	char szTemp[256];

	UpdateData( TRUE );

	int goon, ns;
	long hFnds;
	struct _finddata_t fftmp;
	CHAR Lstfn[_MAX_PATH], SrcFolder[_MAX_PATH], DestFolder[_MAX_PATH], *ps;
	CHAR SrcFile[_MAX_PATH], DestFile[_MAX_PATH];

	strcpy( SrcFolder, (LPCTSTR)m_szSrc );
	strcpy( Lstfn, (LPCTSTR)m_szSrc );
	ps = strrchr( SrcFolder, '\\' );
	if ( ps!=NULL ) ps[0]=0; else strcpy( SrcFolder, "." );

	strcpy( DestFolder, (LPCTSTR)m_szDest );
	ps = strrchr( DestFolder, '\\' );
	if ( ps!=NULL ) ps[0]=0; else strcpy( DestFolder, "." );


	hFnds = _findfirst(Lstfn, &fftmp);
	if (hFnds==-1) return;
	goon = 0;
	while ( goon != -1 ) {
		if (!(fftmp.attrib & _A_SUBDIR))
		{
			sprintf( SrcFile, "%s\\%s", SrcFolder, fftmp.name );
			sprintf( DestFile, "%s\\%s", DestFolder, fftmp.name );
			ns = ((strstr(fftmp.name,"vox")!=NULL || strstr(fftmp.name,"VOX")!=NULL) ? 0:1);
			ps = strrchr( DestFile, '.' );
			if ( ps!=NULL ) strcpy( ps, ".wav" ); else strcat( DestFile, ".wav" );
			ConvertVox2WaveSP( (LPCTSTR)SrcFile, (LPCTSTR)DestFile, NULL, ns ); 
		}
	    goon = _findnext(hFnds, &fftmp);
	}
	_findclose(hFnds);

}

void CTestRTFDlg::OnButton1() 
{
	UpdateData( TRUE );
	if ( pRtf==NULL )
	{

		pRtf= new CRTFProc( (LPTSTR)(LPCTSTR)m_szSrc, (LPTSTR)(LPCTSTR)m_szDest );

		// pRtf->ClearFields();
		// Convert RTFObject->szSourceFile -> szDestinationFile
		bool ss = ( pRtf->ExtendFunction( "", RTF_EXTFUNC_RMVSPEC1 )!=-1 );
		if ( !ss )
		{
			::AfxMessageBox( "Failed" );
		}
		else
			AfxMessageBox( "Success!" );

		delete pRtf;
		pRtf = NULL;
	}
	
}


void CTestRTFDlg::OnButtonRhtml() 
{
	char szTemp[500]={0};
	char szDisp[1000];
	UpdateData( TRUE );
	if ( pRtf==NULL )
	{

		pRtf= new CRTFProc( (LPTSTR)(LPCTSTR)m_szSrc, (LPTSTR)(LPCTSTR)m_szDest );

		// pRtf->ClearFields();
		// Convert RTFObject->szSourceFile -> szDestinationFile
		bool ss = ( pRtf->RtfExport(1, szTemp) );
		if ( !ss )
		{
			sprintf( szDisp, "Failed:%s", szTemp );
			::AfxMessageBox( szDisp );
		}
		else
			::AfxMessageBox( "Success!" );

		delete pRtf;
		pRtf = NULL;
	}
	
}
