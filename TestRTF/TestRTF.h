// TestRTF.h : main header file for the TESTRTF application
//

#if !defined(AFX_TESTRTF_H__A208F289_2FA1_4CCA_AD05_31F95C0C6D49__INCLUDED_)
#define AFX_TESTRTF_H__A208F289_2FA1_4CCA_AD05_31F95C0C6D49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "RTFProc.h"
#include "FSound.h"


/////////////////////////////////////////////////////////////////////////////
// CTestRTFApp:
// See TestRTF.cpp for the implementation of this class
//


class CTestRTFApp : public CWinApp
{
public:
	CTestRTFApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestRTFApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestRTFApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTRTF_H__A208F289_2FA1_4CCA_AD05_31F95C0C6D49__INCLUDED_)
