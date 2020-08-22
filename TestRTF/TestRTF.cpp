// TestRTF.cpp : Defines the class behaviors for the application.
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
// CTestRTFApp

BEGIN_MESSAGE_MAP(CTestRTFApp, CWinApp)
	//{{AFX_MSG_MAP(CTestRTFApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestRTFApp construction

CTestRTFApp::CTestRTFApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTestRTFApp object

CTestRTFApp theApp;

typedef int (PASCAL * lpInitConverter32)(HWND hwndWord, char* m_sModule);
lpInitConverter32 InitConverter32;
 
// Global variable
extern CRITICAL_SECTION CriticalSection; 
extern int gnFirst;
extern HINSTANCE hLibrary;


/////////////////////////////////////////////////////////////////////////////
// CTestRTFApp initialization

BOOL CTestRTFApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	InitializeCriticalSection(&CriticalSection);
	gnFirst = 100;
	/*hLibrary = LoadLibrary("HTML32.cnv");
	//hLibrary = LoadLibrary("WPFT532.cnv");
	if ( hLibrary != NULL )
	{
		InitConverter32	= (lpInitConverter32) GetProcAddress(hLibrary, "InitConverter32");
		long result=InitConverter32 (NULL, NULL);
	}*/

	CTestRTFDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


int CTestRTFApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	DeleteCriticalSection(&CriticalSection);
	gnFirst = 0;
	/*FreeLibrary(hLibrary); 
	hLibrary=NULL;*/ 
	
	return CWinApp::ExitInstance();
}
