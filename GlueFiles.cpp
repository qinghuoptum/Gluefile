// GlueFiles.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f GlueFilesps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "GlueFiles.h"

#include "GlueFiles_i.c"
#include "CreFnetFile.h"
#include "RTFProc.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_CreFnetFile, CCreFnetFile)
END_OBJECT_MAP()

class CGlueFilesApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlueFilesApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGlueFilesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CGlueFilesApp, CWinApp)
	//{{AFX_MSG_MAP(CGlueFilesApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGlueFilesApp theApp;

typedef int (PASCAL * lpInitConverter32)(HWND hwndWord, char* m_sModule);
lpInitConverter32 InitConverter32;
 


extern CRITICAL_SECTION CriticalSection; 
extern int gnFirst;
extern HINSTANCE hLibrary;



BOOL CGlueFilesApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_GLUEFILESLib);
	// RTF Conversion Critic Section
	InitializeCriticalSection(&CriticalSection);
	gnFirst = 100;
	/*hLibrary = LoadLibrary("HTML32.cnv");
	if ( hLibrary != NULL )
	{
		InitConverter32	= (lpInitConverter32) GetProcAddress(hLibrary, "InitConverter32");
		long result=InitConverter32 (NULL, NULL);
	}*/
    return CWinApp::InitInstance();
}

int CGlueFilesApp::ExitInstance()
{
    _Module.Term();
	DeleteCriticalSection(&CriticalSection);
	gnFirst = 0;
	/*FreeLibrary(hLibrary); 
	hLibrary=NULL;*/
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}


