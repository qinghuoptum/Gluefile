// FnetRTF.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"


#include <richedit.h>
#include <commdlg.h>
#include <stdio.h>

#include "RTFProc.h"

void LogViewOper( char* pszFile, char* pszMode ){};

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


static int nEnd;
HWND ghRTF=NULL;
char *szBuf=NULL, *pzBuf=NULL, *pSvp=NULL, szPrvFile[300]={0};
int nn;
HINSTANCE m_hLibRTF;

/*	char szBuf[600]={ "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033{\\fonttbl"
				   "{\\f0\\fnil\\fcharset0 Times New Roman;}}"
				   "\\viewkind4\\uc1\\pard\\f0\\fs20 Test\\b\\i  "
				   "Document \\b0\\i0 ABC\\par Test\\par Hidden \\v1 No Seen \\v0 Seen "
					"\\par \\par <*HS>Test Line\\par {\\v <*HS>}.... Test \\par" };
//	char szBuf[600]={ "{\\rtf1\\b Test!!!}Test \r\n Test \r\n Test d......df.df df df"	};
	nn = strlen( szBuf );
	char *szBuf;
	int nn;
	
	if (--nEnd==0) 
	{
		*pcb=0;
		return 0;
	}
*/

DWORD CALLBACK EditStreamCallback(
  DWORD dwCookie, // application-defined value
  LPBYTE pbBuff,  // pointer to a buffer
  LONG cb,        // number of bytes to read or write
  LONG *pcb       // pointer to number of bytes transferred
)
{
	if (nn==0)
	{
		*pcb=0;
		return 0;
	}
	else if (nn<cb)
	{
		strcpy( (LPTSTR)pbBuff, pzBuf);
		*pcb = nn;
		nn = 0;
		return 0;
	}
	else
	{
		strncpy( (LPTSTR)pbBuff, pzBuf, cb);
		pzBuf += cb;
		nn -= cb;
		*pcb = cb;
		return 0;
	}
}
 
DWORD CALLBACK EditStreamCallbackOut(
  DWORD dwCookie, // application-defined value
  LPBYTE pbBuff,  // pointer to a buffer
  LONG cb,        // number of bytes to read or write
  LONG *pcb       // pointer to number of bytes transferred
)
{
	strncpy( pzBuf, (LPTSTR)pbBuff, cb);
	pzBuf[cb]=0;
	pzBuf += cb;
	nn += cb;
	*pcb = cb;
	return 0;
}
 

/*
{\rtf1\ansi\ansicpg1252\deff0\deflang1033\deflangfe2052{\fonttbl{\f0\fswiss\fprq2\fcharset0 Arial;}}
{\colortbl ;\red255\green0\blue255;}
\viewkind4\uc1\pard\b\f0\fs24\par

\cf1 ELIPIDIO A. \cf0 ABREU, M.D.\par
*/

#define		MOVEPOS(p,n)	n=0; while (p[0]&&(n>0||p[0]!='}')) \
{ if(p[0]=='{') n++; if (p[0]=='}') n--; ++p; } ++p;


#define		REPLACE(p,t,s,b,l,k,m,n)	k=strlen(s),t=p+l;m=nn-(p-b)-l;\
	memmove(p+k,p+l,m);n+=(k-l);p[k+m]=0;memmove(p,s,k);


char szColTbl[]={"{\\colortbl ;\\red255\\green0\\blue255;}"};

	
// Hide=0/Show=1 Hidden Text
void HideShowBuffer( char* pszBuf, int nOption )
{
	//SendMessage( ghRTF, EM_HIDESELECTION, (WPARAM)nOption, 0 ); 
	int ll, mm, kk, nLev;
	char *ps, *ts, szTmp[10];

	ts = strstr(pszBuf, "{\\fonttbl");
	if ( ts==NULL ) return;
	
	ps = ts+2;
	MOVEPOS(ps,nLev)
	
	while ( ps[0] && ps[0]<=' ' ) ++ps;
	ts = ps;
	// ps pointer to the proper position
	if ( nOption==0 && (strncmp( ts, "{\\colortbl", 10 )==0) )
	{
		// Remove this table
		/*MOVEPOS(ps,nLev)
		mm = (ps-ts);
		kk = nn - (ts-pszBuf) - mm;
		memmove( ts, ps, kk );
		ts[kk]=0;
		nn -= mm;*/
	}
	else if ( nOption==1 && (strncmp(ts, "{\\colortbl", 10)!=0) )
	{
		// Insert this table
		REPLACE(ps,ts,szColTbl,pszBuf,0,kk,mm,nn)
			
		/*mm = strlen(szColTbl);
		ts = ps+mm;
		kk = nn - (ps-pszBuf);
		memmove( ts, ps, kk );
		ts[kk]=0;
		nn += mm;*/
	}

	// Replace all \v with \cf1, \v0 with \cf0
	ll = 0;
	ps = pszBuf; 
	while ( ps[0] )
	{
		if ( nOption )
		{	// Show Hiden Contents
			if ( strncmp(ps, "\\v",2)==0 )
			{
				if (ps[2]==' '||ps[2]=='\\'||ps[2]=='1') { strcpy(szTmp,"\\cf1"); ll=2; }
				else if (ps[2]=='0') { strcpy(szTmp,"\\cf0"); ll=3; }
				else szTmp[0]=0;
				
				if ( szTmp[0] )
				{
					REPLACE(ps,ts,szTmp,pszBuf,ll,kk,mm,nn)
					ps += ll;
				}
			}
		}
		else
		{	// Hide	Hiden Contents
			if ( strncmp(ps, "\\cf",3)==0 )
			{																	 				if (ps[3]=='1') { strcpy(szTmp,"\\v"); ll=4; }
				else if (ps[3]=='0') { strcpy(szTmp,"\\v0"); ll=4; }
				else szTmp[0]=0;
				
				if ( szTmp[0] )
				{
					REPLACE(ps,ts,szTmp,pszBuf,ll,kk,mm,nn)
				}
				ps += ll;
			}
		}
		++ps;
	}

}


void GetBuffer()
{
	EDITSTREAM xx;

	pzBuf=szBuf;
	nn=0;
	xx.dwCookie = 0;
	xx.dwError = 0;
	xx.pfnCallback = EditStreamCallbackOut;
	SendMessage( ghRTF, EM_STREAMOUT, SF_RTF, (LPARAM)(EDITSTREAM FAR *)&xx ); 
}


void PutBuffer()
{
	EDITSTREAM xx;

	pzBuf=szBuf;
	xx.dwCookie = 0;
	xx.dwError = 0;
	xx.pfnCallback = EditStreamCallback;
	SendMessage( ghRTF, EM_STREAMIN, SF_RTF, (LPARAM)(EDITSTREAM FAR *)&xx ); 
}



bool LoadFile( HWND hDlg )
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260], szTmp[260];       // buffer for file name

	// Initialize OPENFILENAME
	szFile[0] = 0;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0RTF\0*.RTF\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		
	if ( GetOpenFileName( &ofn) )
	{
		sprintf( szTmp, "RTF View - %s", ofn.lpstrFile );
		SetWindowText( hDlg, szTmp );

		CRTFProc *prtf = new CRTFProc( ofn.lpstrFile, ofn.lpstrFile );
		prtf->GetRTFHeader();
		delete prtf;

		FILE *fp;
		fp=fopen(ofn.lpstrFile,"rb");
		if (fp == NULL) return 0;
		fseek( fp, 0L, SEEK_END );
		nn = ftell( fp );

		szBuf = new CHAR[nn+8000];
		if (szBuf==NULL)
		{
			fclose( fp );
			return 0;
		}
		fseek( fp, 0L, SEEK_SET );
		nn = fread( szBuf,sizeof(CHAR), nn+1, fp );
		szBuf[nn] = 0;
		fclose( fp );
		
		pzBuf = szBuf;
		pSvp = szBuf;
		strncpy( szPrvFile, ofn.lpstrFile, sizeof(szPrvFile)-1);

		return true;

		//CWRTFDoc *prtf = new CWRTFDoc();
		//prtf->ReadFile( ofn.lpstrFile );

	}
	else
		return false;
}


bool SaveFile(HWND hDlg)
{
	GetBuffer();
	HideShowBuffer( szBuf, 0 );

	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	szFile[0] = 0;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0RTF\0*.RTF\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST ;
		
	if ( GetSaveFileName( &ofn) )
	{
		FILE *fp;
		fp=fopen(ofn.lpstrFile,"wb");
		if (fp == NULL) return false;
		nn = fwrite( szBuf,sizeof(CHAR), nn, fp );
		fclose( fp );
		pzBuf = szBuf;

		CRTFProc *prtf = new CRTFProc( szPrvFile, ofn.lpstrFile );
		prtf->SetRTFHeader();
		delete prtf;
		return true;
	}
	else
		return false;
}


bool SaveFile2(HWND hDlg)
{
	GetBuffer();
	HideShowBuffer( szBuf, 0 );

	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	szFile[0] = 0;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0RTF\0*.RTF\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST ;
		
	if ( GetSaveFileName( &ofn) )
	{
		FILE *fp;
		fp=fopen(ofn.lpstrFile,"wb");
		if (fp == NULL) return false;
		nn = fwrite( szBuf,sizeof(CHAR), nn, fp );
		fclose( fp );
		pzBuf = szBuf;

		//CRTFProc *prtf = new CRTFProc( szPrvFile, ofn.lpstrFile );
		//prtf->SetRTFHeader();
		//delete prtf;
		return true;
	}
	else
		return false;
}



void ClearHouse()
{
	if ( pSvp!=NULL )
	{
		delete pSvp;
		pSvp=NULL;
	}

	if (ghRTF != NULL)
	{
		DestroyWindow( ghRTF );
		ghRTF = NULL;
	}

	if ( szPrvFile[0] ) szPrvFile[0]=0;
}


void ProtectSec()
{
	CHARFORMAT2 cf2;

	if (ghRTF != NULL)
	{
		cf2.cbSize	= sizeof(CHARFORMAT2);
		cf2.dwMask	= CFM_PROTECTED;
		cf2.dwEffects = CFE_PROTECTED;
		SendMessage( ghRTF, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2 ); 
	}
}


void FindMark()
{
/*	char szTemp[50];
	CHARRANGE rg;
	FINDTEXT ft;
	int nn;

	::SendMessage(m_wndRTF.m_hWnd, EM_EXGETSEL,0,(LPARAM)((CHARRANGE FAR *)&rg));
	rg.cpMax = -1;
	ft.chrg = rg;
	ft.lpstrText = szTemp;
	// sprintf( szTemp, "Range: %d - %d", rg.cpMin,rg.cpMax );
	// ::MessageBox(m_wndRTF.m_hWnd, szTemp,"FnetEdit",0);
	sprintf( szTemp, "<*HS>" );
	nn = ::SendMessage(m_wndRTF.m_hWnd, EM_FINDTEXT, FR_DOWN, (LPARAM)(FINDTEXT FAR *)&ft );
	if (nn>=0)
	{
		// sprintf( szTemp, "Index: %d", nn );
		// ::MessageBox(m_wndRTF.m_hWnd, szTemp,"FnetEdit",0);
		rg.cpMin=nn+6;
		rg.cpMax=nn+6;
		::SendMessage(m_wndRTF.m_hWnd, EM_EXSETSEL,0,(LPARAM)((CHARRANGE FAR *)&rg));
	}
	else
	{
		// ::MessageBox(m_wndRTF.m_hWnd, "End of Search","FnetEdit",0);
	}

	::SendMessage(m_wndRTF.m_hWnd, WM_SETFOCUS, (WPARAM)NULL, 0);
*/
}




void CreateRTFWnd( HWND hDlg, int nOpt=1 )
{
	nEnd = 2;
	ClearHouse();

	DWORD dwStyle;
	RECT  rc;

	dwStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_WANTRETURN | ES_MULTILINE 
		      | ES_AUTOVSCROLL ;

	// GetWindowRect( hDlg, &rc );
	GetClientRect( hDlg, &rc );

	if ( nOpt==1 )
	{
		m_hLibRTF = LoadLibrary(_T("RICHED20.DLL"));
		ghRTF = CreateWindow(_T("RichEdit20A")/*RICHEDIT_CLASS*/, NULL, dwStyle,
						 rc.left, rc.top, rc.right, rc.bottom,
						 hDlg, NULL, hInst, NULL);

	}
	else if ( nOpt==2 )
	{
		m_hLibRTF = LoadLibrary(_T("MSFTEDIT.DLL"));
		ghRTF = CreateWindow(_T("RICHEDIT50W")/*RICHEDIT_CLASS*/, NULL, dwStyle,
						 rc.left, rc.top, rc.right, rc.bottom,
						 hDlg, NULL, hInst, NULL);

/*		m_hLibRTF = LoadLibrary(_T("RICHED32.DLL"));
		ghRTF = CreateWindow(_T("RichEdit"), NULL, dwStyle,
						 rc.left, rc.top, rc.right, rc.bottom,
						 hDlg, NULL, hInst, NULL); */
	}
	if ( ghRTF==NULL) return;

	ShowWindow(ghRTF, SW_SHOW);
	UpdateWindow( ghRTF );

	long eventmask = SendMessage( ghRTF, EM_GETEVENTMASK,0,0) | ENM_PROTECTED | ENM_SELCHANGE ;
	SendMessage( ghRTF, EM_SETEVENTMASK, 0, (LPARAM) eventmask );

	if ( LoadFile( hDlg ) )
	{
		PutBuffer();
	}

}	


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	//if ( m_hLibRTF==NULL ) 
	//	return FALSE;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FNETRTF, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FNETRTF);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_FNETRTF);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_FNETRTF;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)

{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_LOAD:
				   CreateRTFWnd(hWnd);
				   break;
				case IDM_LOAD1:
				   CreateRTFWnd(hWnd,2);
				   break;
				case IDM_SAVE:
				   if (SaveFile(hWnd)) ClearHouse();
				   break;
				case IDM_SAVE2:
				   if (SaveFile2(hWnd)) ClearHouse();
				   break;
				case IDM_SHOW:
				   GetBuffer();
				   HideShowBuffer( szBuf, 0 );
				   PutBuffer();
				   break;
				case IDM_HIDE:
				   GetBuffer();
				   HideShowBuffer( szBuf, 1 );
				   PutBuffer();
				   break;
				case IDM_PROTECT:
				   ProtectSec();
				   break;
				case IDM_FIND:
				   FindMark();
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			ClearHouse();
			PostQuitMessage(0);
			break;
		case WM_NOTIFY:
			if ( ((LPNMHDR)lParam)->code == EN_PROTECTED )
			{
				Beep(400,400);
				return 1;
			}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
