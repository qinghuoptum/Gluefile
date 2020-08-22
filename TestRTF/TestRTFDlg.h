// TestRTFDlg.h : header file
//

#if !defined(AFX_TESTRTFDLG_H__B0FF29FF_2BD1_4F78_9CE5_BFC663A0D5D7__INCLUDED_)
#define AFX_TESTRTFDLG_H__B0FF29FF_2BD1_4F78_9CE5_BFC663A0D5D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestRTFDlg dialog

class CRTFProc;

class CTestRTFDlg : public CDialog
{
// Construction
public:
	CTestRTFDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestRTFDlg)
	enum { IDD = IDD_TESTRTF_DIALOG };
	CString	m_szSrc;
	CString	m_szDest;
	CString m_szRepl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestRTFDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	CRTFProc *pRtf;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestRTFDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonInit();
	afx_msg void OnButtonCvt();
	afx_msg void OnButtonClose();
	afx_msg void OnButtonV2w();
	afx_msg void OnButtonRepl();
	afx_msg void OnButtonInsp();
	afx_msg void OnButtonInsb();
	afx_msg void OnButton1();
	afx_msg void OnButtonV2w8();
	afx_msg void OnButtonRhtml();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTRTFDLG_H__B0FF29FF_2BD1_4F78_9CE5_BFC663A0D5D7__INCLUDED_)
