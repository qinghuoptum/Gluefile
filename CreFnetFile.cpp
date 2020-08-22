// CreFnetFile.cpp : Implementation of CCreFnetFile
#include "stdafx.h"
#include "GlueFiles.h"
#include "CreFnetFile.h"

#include "FSound.h"


/*  Batch_id   (987654321) (Batch)


0x0001:	Batch Sign Enable;
0x0002:	Image Button Enable;
0x0004:	Local Save as Enable;
0x0008:	Enable modify the Hidden Fields;
0x0010:	Enable sign&GoToNext button and disable sign button;
0x0020:	Enable Popup message windows;
0x0040:	Not replace the Hidden fields;  not used in FnetEdit
0x0080:	Hide the voice recognition button;
0x0100: EnableASP.NET, , it's defined for fnetedit to determine ASP or ASPX page be called.
0x0200: Hide all buttons except the save button, it's defined for onlinehelpdesk
0x0400: Enable media player control, it's defined for onlinehelpdesk
0x0800: In ASP.NET enable exit button, it's defined for onlinehelpdesk
0x1000: Generate the RTF file w/ all other functions, fneteditex/fnetwordex need not rtf file, can not be gluefile
0x2000: Check Sign field for information, not doing real sign


Batch Processing Request (FileList)

0x01:	Batch Print;
0x02:	Batch Download;
0x04:	Display Message in the client side;
0x08:	Batch download w/ Given Path;

Security (Security)
0x01:   Delete Temp file

*/


void LogViewOper( char* pszFile, char* pszMode )
{
	char szPath[MAX_PATH]={"C:\\TEMP\\GlueFile.LOG"};
	
	GetWindowsDirectory( szPath, MAX_PATH );
	strcat( szPath, "\\GlueFile.LOG" );
	
	CTime t = CTime::GetCurrentTime();
	FILE *fp;
	
	fp = fopen( szPath, "r+" );
	// fp = fopen( szPath, "a+" );
	if (fp==NULL) return;
	fseek(fp, 0L, SEEK_END);
	fprintf(fp, "%s  %s -- %s\n", t.Format("%c"), pszMode, pszFile);
	fclose( fp );
	return;
}


void LOGCONTENT( char* pszFile )
{
	char szPath[MAX_PATH]={"C:\\TEMP\\GlueFile.LOG"};
	char szBuf[512];
	int i, nn;

	GetWindowsDirectory( szPath, MAX_PATH );
	strcat( szPath, "\\GlueFile.LOG" );
	
	FILE *fp;

	fp = fopen( pszFile, "rb" );
	if ( fp==0 ) { strcpy( szBuf, "Access Deny!" ); nn = strlen(szBuf); }
	else
	{
		nn = fread( szBuf, 1, 32, fp );
		fclose( fp );
	}
	
	fp = fopen( szPath, "r+" );
	if (fp==NULL) return;
	fseek(fp, 0L, SEEK_END);
	fprintf(fp, "%s -- ", pszFile);
	for (i=0; i<nn; i++) fprintf(fp, "%02x ", szBuf[i] );
	fprintf( fp, " -- ", pszFile);
	fprintf( fp, szBuf );
	fprintf( fp, "\n" );
	fclose( fp );
	return;
}

#define		LOGFILE(x,y)		LogViewOper(x,y)


/////////////////////////////////////////////////////////////////////////////
// CCreFnetFile

STDMETHODIMP CCreFnetFile::OnStartPage (IUnknown* pUnk)  
{

	if(!pUnk)
		return E_POINTER;

	HRESULT hResult= CoInitialize(NULL);

/*	CComPtr<IScriptingContext> spContext;
	HRESULT hr;

	// Get the IScriptingContext Interface
	hr = pUnk->QueryInterface(IID_IScriptingContext, (void **)&spContext);
	if(FAILED(hr))
		return hr;

	// Get Request Object Pointer
	hr = spContext->get_Request(&m_piRequest);
	if(FAILED(hr))
	{
		spContext.Release();
		return hr;
	}

	// Get Response Object Pointer
	hr = spContext->get_Response(&m_piResponse);
	if(FAILED(hr))
	{
		m_piRequest.Release();
		return hr;
	}
	
	// Get Server Object Pointer
	hr = spContext->get_Server(&m_piServer);
	if(FAILED(hr))
	{
		m_piRequest.Release();
		m_piResponse.Release();
		return hr;
	}
	
	// Get Session Object Pointer
	hr = spContext->get_Session(&m_piSession);
	if(FAILED(hr))
	{
		m_piRequest.Release();
		m_piResponse.Release();
		m_piServer.Release();
		return hr;
	}

	// Get Application Object Pointer
	hr = spContext->get_Application(&m_piApplication);
	if(FAILED(hr))
	{
		m_piRequest.Release();
		m_piResponse.Release();
		m_piServer.Release();
		m_piSession.Release();
		return hr;
	}
*/
	m_bOnStartPageCalled = TRUE;
	RTFObject=NULL;
	return S_OK;
}

STDMETHODIMP CCreFnetFile::OnEndPage ()  
{
	if (RTFObject!=NULL)
		delete RTFObject;
	m_bOnStartPageCalled = FALSE;
	CoUninitialize();

	// Release all interfaces
/*	m_piRequest.Release();
	m_piResponse.Release();
	m_piServer.Release();
	m_piSession.Release();
	m_piApplication.Release();
*/
	return S_OK;
}


STDMETHODIMP CCreFnetFile::GetFilename(BSTR SourceFileName, BSTR DestinationFileName)
{
	// LOGFILE( "  ...*** ", "First In" );
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char szSrcFile[256],szTarFile[256];

	char szDir[256]={0};
	bool ss = false;

	int len=SysStringLen(SourceFileName);
	wcstombs(szSrcFile, SourceFileName, len+1);
	szSrcFile[len]='\0';
	
	len=SysStringLen(DestinationFileName);
	wcstombs(szTarFile, DestinationFileName, len+1);
	szTarFile[len]='\0';

	// Processing html or same src/dest
    if ( strcmp( szSrcFile, szTarFile )==0 ||
	     stricmp( szSrcFile+(strlen(szSrcFile)-5), ".html" )==0 )
	{
		if ( m_szTempFile[0]==0 )
		{
			GetTempPath(256, szDir);
			GetTempFileName(szDir, "HTMLRTF__", 0, m_szTempFile);
		}
		ss = CopyFile( szTarFile, m_szTempFile, false );
		if ( ss ) strncpy( szSrcFile, m_szTempFile, 255 );
		else m_szTempFile[0]=0;
		Sleep( 500 );
	}
	
	//VARIANT hintmsg;
	//::VariantClear(&hintmsg);
	//hintmsg.vt = VT_BSTR;
	char szBuf[250];
    WCHAR wcBuf[256];

	LOGFILE( "  ...*** ", "Find First" );

	WIN32_FIND_DATA finddata;
	HANDLE hresult;
	if ((hresult=FindFirstFile(szSrcFile,&finddata))==INVALID_HANDLE_VALUE)
	{
		FindClose(hresult);
		LOGFILE( "  ...*** ", "Not Found First" );
		strcpy(szBuf,"Source file not found! ");

	    memset(wcBuf, 0, sizeof(WCHAR)*256);
	    ::MultiByteToWideChar(CP_ACP, 0, szBuf, strlen(szBuf), wcBuf, 256);
   
	    // hintmsg.bstrVal = ::SysAllocString(wcBuf);
		// m_piResponse->Write(hintmsg);
  	    // ::SysFreeString(hintmsg.bstrVal);
		// LOGFILE( "  ...*** ", "After Response" );
		return S_FALSE;
	}
	else	
	{
		LOGFILE( "  ...*** ", "Found First" );
		FindClose(hresult);

		strcpy( m_szSrcFile, szSrcFile );
		strcpy( m_szTarFile, szTarFile );
		
		if (RTFObject!=NULL) delete RTFObject;
		
		RTFObject = new CRTFProc(szSrcFile, szTarFile);
		// RTFObject->m_piResponse = m_piResponse;
		if ( RTFObject == NULL ) 
		{
			LOGFILE( "  ...*** ", "RTF Memory Error" );
			return S_FALSE;
		}
		return S_OK;
	}
}


STDMETHODIMP CCreFnetFile::RefreshField(BSTR FieldValue)
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char pszfieldvalue[8100]={0};
	char szTmpL[60]={0};
	int nn;

	if (RTFObject!=NULL)
	{
		int len=SysStringLen(FieldValue);
		nn = wcstombs(pszfieldvalue, FieldValue, 8100);
		if (nn>=0) pszfieldvalue[nn]='\0';
		
		sprintf( szTmpL, "Length=%d, Copies=%d", len, nn );

		// LOGFILE( "  Length  ", szTmpL );
		// LOGFILE( "  Variables ", pszfieldvalue );

		/*/ For testing only
		_bstr_t bstrStart(FieldValue);
		LOGFILE(" Variable BSTR", (LPCTSTR)bstrStart );
		strncpy( pszfieldvalue, (LPCTSTR)bstrStart, 8000 ); */ 

		if (RTFObject->FieldsReplace(pszfieldvalue, 0))
			return S_OK;
		else
		    return S_FALSE;
	}
	else
		return S_FALSE;
}


STDMETHODIMP CCreFnetFile::InsertSignPic(BSTR PicFileName,short signatoryID)
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CHAR pszPicFile[256];
	int nRes; 

	if (RTFObject!=NULL)
	{
		int len=SysStringLen(PicFileName);
		wcstombs(pszPicFile, PicFileName, len+1);
		pszPicFile[len]='\0';

		WIN32_FIND_DATA finddata;
		HANDLE hresult;

		LOGFILE( "  SignPic  ", pszPicFile );
		hresult=INVALID_HANDLE_VALUE;
		if ( (pszPicFile[0]!='*') && 
		     (hresult=FindFirstFile(pszPicFile,&finddata))==INVALID_HANDLE_VALUE )
		{
			FindClose(hresult);		
			return S_FALSE;
		}
		else	
		{
			// Add condition
			if (hresult!=INVALID_HANDLE_VALUE) FindClose(hresult);	
			nRes = RTFObject->InsertSignPic( pszPicFile, signatoryID, nChkOnly );
			if ( nRes > 1 )
				return S_OK;
			else if ( nRes == 1 || nRes == 0 )
				return E_FAIL;
			else
				return S_FALSE;
		}
	}
	else
	  return S_FALSE;
}

STDMETHODIMP CCreFnetFile::GlueFile(short edit,short sign,BSTR usercode,BSTR password,short batch, BSTR fields)
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	LPSTORAGE pStorage=NULL;
	LPSTREAM pStream=NULL;
	CLSID const ccslid=
//	{0x93901785,0x436B,0x11D0,
//			{0xB9,0x65,0,0,0,0,0,0}};  
    {0x89D122E1,0x99E6,0x11D2,
	{0xBB,0x1E,0x00,0x40,0x05,0x3B,0x6D,0x66}};

	char szTmpFile[256], *ps;

	if (RTFObject!=NULL)
	{
		GetTempPath( 256, szTmpFile );
		ps = strrchr( RTFObject->szDestinationFile, '\\' );
		if ( ps==NULL ) ps = RTFObject->szDestinationFile;
		else ps++;
		strcat( szTmpFile, ps );

		/* strncpy(szTmpFile,RTFObject->szDestinationFile,strlen(RTFObject->szDestinationFile)-4);
		szTmpFile[strlen(RTFObject->szDestinationFile)-4]='\0';
		strcat(szTmpFile, "~.tmp");
		szTmpFile[strlen(RTFObject->szDestinationFile)+1]='\0'; */

		WCHAR m_sFilename[256];
		MultiByteToWideChar(CP_ACP,0,szTmpFile,-1,m_sFilename,256);
		HRESULT hr=StgCreateDocfile(m_sFilename,
			STGM_CREATE | STGM_READWRITE |STGM_DIRECT|
			STGM_SHARE_EXCLUSIVE,
			0,&pStorage);
		
		if (FAILED(hr))
		{
			return S_FALSE;
		}

		WriteClassStg(pStorage,ccslid);

		hr=pStorage->CreateStream(L"Contents",
			STGM_CREATE|STGM_DIRECT|
			STGM_WRITE|STGM_SHARE_EXCLUSIVE,
			0,0,&pStream);

		if (FAILED(hr))
		{
			pStorage->Release();
			return S_FALSE;
		}

		ULONG pcb;
		char *pBuf, szKey[30], szValue[100], *pszCur, *pszSkip, *end;
		WCHAR wszKey[30];
		FILE *input;
		UINT nByte;

		input=fopen(RTFObject->szSourceFile,"rb");
		if (input != NULL)
		{

			fseek( input, 0L, SEEK_END );
			nByte = ftell( input );
			pBuf = new CHAR[nByte+8000];
			if (pBuf==NULL)
			{
				fclose( input );
				pStream->Release();
				pStorage->Release();
				return S_FALSE;
			}

			fseek( input, 0L, SEEK_SET );
			nByte = fread( pBuf,sizeof(CHAR), nByte+1, input );
			pBuf[nByte] = 0;
		  	hr=pStream->Write(pBuf,nByte,&pcb);
			fclose( input );
			/*
			while (!feof(input))
			{
				nByte=fread(pBuf,sizeof(char),900,input);
				if (nByte>0)
		  			hr=pStream->Write(pBuf,nByte,&pcb);
			}  
			fclose(input);
			*/
			pStream->Release();
			
			hr=pStorage->CreateStream(L"edit",
				STGM_CREATE|STGM_DIRECT|
				STGM_WRITE|STGM_SHARE_EXCLUSIVE,
				0,0,&pStream);

			if (edit==1)
				strcpy(pBuf, "1");
			else
				strcpy(pBuf,"0");

			*(pBuf+1)='\0';
			nByte=strlen(pBuf);
			hr=pStream->Write(pBuf,nByte,&pcb);
			pStream->Release();

			hr=pStorage->CreateStream(L"sign",
				STGM_CREATE|STGM_DIRECT|
				STGM_WRITE|STGM_SHARE_EXCLUSIVE,
				0,0,&pStream);

			if (sign==1)
				strcpy(pBuf, "1");
			else
				strcpy(pBuf,"0");
			*(pBuf+1)='\0';
			nByte=strlen(pBuf);
			hr=pStream->Write(pBuf,nByte,&pcb);
			pStream->Release();

			hr=pStorage->CreateStream(L"cu",
				STGM_CREATE|STGM_DIRECT|
				STGM_WRITE|STGM_SHARE_EXCLUSIVE,
				0,0,&pStream);
			int len=SysStringLen(usercode);
			wcstombs(pBuf, usercode, len+1);
			pBuf[len]='\0';

			nByte=strlen(pBuf);
			hr=pStream->Write(pBuf,nByte,&pcb);
			pStream->Release();

			hr=pStorage->CreateStream(L"dp",
				STGM_CREATE|STGM_DIRECT|
				STGM_WRITE|STGM_SHARE_EXCLUSIVE,
				0,0,&pStream);
			len=SysStringLen(password);
			wcstombs(pBuf, password, len+1);
			pBuf[len]='\0';

			nByte=strlen(pBuf);
			hr=pStream->Write(pBuf,nByte,&pcb);
			pStream->Release();

			hr=pStorage->CreateStream(L"batch",
				STGM_CREATE|STGM_DIRECT|
				STGM_WRITE|STGM_SHARE_EXCLUSIVE,
				0,0,&pStream);
			sprintf(pBuf, "%d\0", batch);
			nByte=strlen(pBuf);
			hr=pStream->Write(pBuf,nByte,&pcb);
			pStream->Release();

			hr=pStorage->CreateStream(L"FilePath",
				STGM_CREATE|STGM_DIRECT|
				STGM_WRITE|STGM_SHARE_EXCLUSIVE,
				0,0,&pStream);

			if ( strncmp(RTFObject->szOrginalFile,"\\\\",2)==0 )
			{
				end = RTFObject->szOrginalFile+2;
				end = strchr( end, '\\' );
				if (end!=NULL) end = strchr(end+1,'\\');
				if (end!=NULL)
					strcpy(pBuf, end);
				else
					strcpy(pBuf, RTFObject->szOrginalFile);
			}
			else if ((RTFObject->szOrginalFile)[1]==':')
				strcpy(pBuf, RTFObject->szOrginalFile+2);
			else
				strcpy(pBuf, RTFObject->szOrginalFile);
			nByte=strlen(pBuf);
			hr=pStream->Write(pBuf,nByte,&pcb);
			pStream->Release();

			// Use simple version
			hr=pStorage->CreateStream(L"Fields",
				STGM_CREATE|STGM_DIRECT|
				STGM_WRITE|STGM_SHARE_EXCLUSIVE,
				0,0,&pStream);
			len=SysStringLen(fields);
			wcstombs(pBuf, fields, len+1);
			pBuf[len]='\0';
			nByte=strlen(pBuf);
			hr=pStream->Write(pBuf,nByte,&pcb);
			pStream->Release();
			
			// Add Fields List
			if (fields!=NULL)
			{
				len=SysStringLen(fields);
				wcstombs(pBuf, fields, len+1);
				pBuf[len]='\0';
				pszCur = pBuf;
				while ( pszCur!=NULL && pszCur[0] )
				{
					pszSkip = strchr( pszCur, '=' );
					if (pszSkip==NULL) break;
					pszSkip[0] = '\x0';
					strncpy( szKey, pszCur, sizeof(szKey)-1 );
					pszCur = pszSkip+1;
					if ( pszCur[0]==0 ) break;
					pszSkip = strchr( pszCur, '|' );
					if (pszSkip==NULL)
					{
						strncpy( szValue, pszCur, sizeof(szValue)-1);
						pszCur = NULL;
					}
					else
					{
						pszSkip[0] = '\x0';
						strncpy( szValue, pszCur, sizeof(szValue)-1 );
						pszCur = pszSkip+1;
					}
					szValue[sizeof(szValue)-1]=0;

					if ( stricmp( szKey, "Document" )==0 || stricmp( szKey, "FileList" )==0 )
					{
						mbstowcs( wszKey, szKey, strlen(szKey)+1);
						hr=pStorage->CreateStream(wszKey,
							STGM_CREATE|STGM_DIRECT|
							STGM_WRITE|STGM_SHARE_EXCLUSIVE,
							0,0,&pStream);
						nByte=strlen(szValue);
						hr=pStream->Write(szValue,nByte,&pcb);
						pStream->Release();
					}
					if (pszCur==NULL) break;
				}
			}

			pStorage->Release();

			delete pBuf;
			BOOL ss = CopyFile(szTmpFile,RTFObject->szDestinationFile,false);
		    if (ss) DeleteFile(szTmpFile);
			return (ss?S_OK:S_FALSE);
		}
		else
		{
	  	  pStream->Release();
		  pStorage->Release();
		  return S_FALSE;
		}
	}
	else
	{
		return S_FALSE;
	}
}

STDMETHODIMP CCreFnetFile::UnGlueFile()
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	LPSTORAGE pStorage=NULL;
	LPSTREAM pStream=NULL;

	WCHAR m_sFilename[256];

	if (RTFObject!=NULL)
	{
		MultiByteToWideChar(CP_ACP,0,RTFObject->szSourceFile,-1,m_sFilename,256);
		HRESULT hr=StgOpenStorage(m_sFilename,NULL,
			STGM_READ|STGM_DIRECT|STGM_SHARE_EXCLUSIVE|0,NULL,
			0,&pStorage);
		if (FAILED(hr))
			return S_FALSE;

		hr=pStorage->OpenStream(L"Contents",NULL,
			STGM_DIRECT|STGM_READ|STGM_SHARE_EXCLUSIVE,
			0,&pStream);
		if (FAILED(hr))
			return S_FALSE;

		ULONG pcb;
		char pBuf[264];
		char szTmpFile[256];

		strncpy(szTmpFile,RTFObject->szDestinationFile,strlen(RTFObject->szDestinationFile)-4);

		szTmpFile[strlen(RTFObject->szDestinationFile)-4]='\0';
		strcat(szTmpFile, "~.tmp");
		szTmpFile[strlen(RTFObject->szDestinationFile)+1]='\0';

		FILE *output;
//	UINT nByte;
		output=fopen(szTmpFile,"w");
		if (output != NULL)
		{
			hr=pStream->Read(pBuf,256,&pcb);
			while (pcb>0)
			{
				pBuf[pcb]='\0';		
				fputs(pBuf,output);
				hr=pStream->Read(pBuf,256,&pcb);
			}
			fclose(output);

			pStream->Release();
			pStorage->Release();

//	CopyFile(szTmpFile,RTFObject->szSourceFile,false);
			CopyFile(szTmpFile,RTFObject->szDestinationFile,false);
			strcpy(RTFObject->szSourceFile,RTFObject->szDestinationFile); //add test
			DeleteFile(szTmpFile);

			return S_OK;
		}
		else
		{
			pStream->Release();
			pStorage->Release();
			return S_FALSE;
		}
	}
	else
		return S_FALSE;
}

STDMETHODIMP CCreFnetFile::SaveFile()
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	CopyFile(RTFObject->szSourceFile,RTFObject->szDestinationFile,false);
	//	DeleteFile(RTFObject->szSourceFile);
	return S_OK;
}


STDMETHODIMP CCreFnetFile::CreateViewFile( BSTR SrcFileName, BSTR TarFileName, BSTR FieldValue, 
			short edit, short sign, BSTR usercode, BSTR password, short batch, BSTR* pzRes )
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char szSrcFile[256], szTarFile[256], szTemp[32];
	HRESULT ss;
	int nRes;
	
	int len=SysStringLen(SrcFileName);
	wcstombs(szSrcFile, SrcFileName, len+1);
	szSrcFile[len]='\0';
	
	len=SysStringLen(TarFileName);
	wcstombs(szTarFile, TarFileName, len+1);
	szTarFile[len]='\0';
	
	// LOGFILE( szSrcFile, "View" );
	WIN32_FIND_DATA finddata;
	HANDLE hresult;
	if ((hresult=FindFirstFile(szSrcFile, &finddata))==INVALID_HANDLE_VALUE)
	{
		// FindClose(hresult);
		LOGFILE( szSrcFile, "$$$NOFile" );
		ss = S_FALSE;
	}
	else	
	{
		if (finddata.nFileSizeLow <= 10)
		{
			LOGFILE( szSrcFile, "$$$ZeroSize" );
			// return S_FALSE;
		} 

		FindClose(hresult);
		LOGFILE( szSrcFile, "View" );

		if (GetFilename(SrcFileName,TarFileName)==S_OK)
		{
			strcpy( szSrcFile, m_szSrcFile );
			strcpy( szTarFile, m_szTarFile );

			nRes = RTFObject->CheckValidate( szSrcFile );
			if ( nRes < 0 )
			{
				ss = S_FALSE;
				LOGFILE( szSrcFile, "Access Failed!" );
			}
			else if ( nRes > 0 )
			{
				if (batch & 0x1000) // fneteditex rtf
				{
					if ( (batch&64)==0)  // 0x40, Not replace hidden fields
					{
						ss = (RefreshField(FieldValue)==S_OK); //create dest file
					}
					else
					{
						if ( CopyFile(szSrcFile, szTarFile, false))
						{
							LOGFILE( szTarFile, "Copy File To" );
							ss = S_OK;
						}
						else
						{
							LOGFILE( szTarFile, "Copy File Faild" );
							ss = S_FALSE;
						}
					}
					LOGFILE( szSrcFile, "Fields" );
					if ((edit==1)||(sign==1)) GetRTFHeader();
				}
				else //fnetedit fn2
				{
					if ( (batch&64)==0)  // 0x40, Not replace hidden fields
						ss = (RefreshField(FieldValue)==S_OK);
					else
						ss = true;

					if (ss)
					{	
						LOGFILE( szSrcFile, "Fields" );
						if ((edit==1)||(sign==1)) GetRTFHeader();

						if ( GlueFile(edit,sign,usercode,password,batch,FieldValue)==S_OK )
							ss = S_OK;
						else
							ss = S_FALSE;
					}
					else
						ss = S_FALSE;
				}
			}
			else
			{
				ss = S_FALSE;
				LOGFILE( szSrcFile, "Wrong format!" );
				LOGCONTENT( szSrcFile );
			}
		}
		else
			ss = S_FALSE;
	}
	LOGFILE( szSrcFile, "End:View" );

	if ( ss == S_OK ) strcpy(szTemp,"OK"); else strcpy(szTemp,"FAIL");
	len = strlen( szTemp );
	wchar_t* pwcbuf = new wchar_t[len+1];
	mbstowcs(pwcbuf, szTemp, len);
	pwcbuf[len]=0;
	*pzRes = SysAllocString(pwcbuf);	
	delete pwcbuf;
	return S_OK;
}


STDMETHODIMP CCreFnetFile::Save(BSTR SrcFile, BSTR TarFile, BSTR* pzRes)
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	char szSrcFile[256], szTarFile[256], szTemp[32];
	HRESULT ss;
	int nRes;
	BOOL bRTF;

	int len=SysStringLen(SrcFile);
	wcstombs(szSrcFile, SrcFile, len+1);
	szSrcFile[len]='\0';

	len=SysStringLen(TarFile);
	wcstombs(szTarFile, TarFile, len+1);
	szTarFile[len]='\0';
	
	/* For testing 
	FILE *fp = fopen( szTarFile, "r+" );
	if ( fp!=NULL )
	{
		fseek(fp, 0L, SEEK_END);
		fprintf( fp, "XXX -- %s %s\n", szSrcFile, szTarFile );
		fclose( fp );
	} */

	LOGFILE( szSrcFile, "Save" );
	// LogViewOper( szSrcFile, "SaveX" );

	WIN32_FIND_DATA finddata;
	HANDLE hresult;
	if ((hresult=FindFirstFile(szSrcFile,&finddata))==INVALID_HANDLE_VALUE)
	{
		LOGFILE( szSrcFile, "Not Exist" );
		FindClose(hresult);
		ss = S_FALSE;
	}
	else	
	{
		FindClose(hresult);

		if (GetFilename(SrcFile,TarFile)==S_OK)
		{
			strcpy( szSrcFile, m_szSrcFile );
			strcpy( szTarFile, m_szTarFile );

			nRes = RTFObject->CheckValidate( szSrcFile );
			bRTF = (nRes>0);

			if ( nRes<0 )
			{
				LOGFILE( szSrcFile, "Access Failed" );
				ss = S_FALSE;
			}
			else if ( bRTF || ( (ss=UnGlueFile())==S_OK ))
			{
				if ( bRTF )
				{
					ss = S_OK;
					if ( !CopyFile( szSrcFile, szTarFile, false ) )
					// if ( RTFObject->ExtendFunction( "15", RTF_EXTFUNC_RMVSPACE )>=0 )
						// Disable this function, need to remove to before open
						// if ( RTFObject->ExtendFunction( "", RTF_EXTFUNC_RMVSPEC1 )==-1 )
					{
						LOGFILE( szTarFile, "Copy File Failed" );
						ss = S_FALSE;
					}
				}

				nRes = RTFObject->CheckValidate( szTarFile );
				if ((ss==S_OK) && ( nRes>0 ))
				{
					LOGFILE( szSrcFile, "Suc: Save, before Head" );

					if ( SetRTFHeader()!=S_OK )
					{
						LOGFILE( szSrcFile, "Failed Set:Head" );
					}

					// Remove extra lines
					strcpy( RTFObject->szSourceFile, RTFObject->szDestinationFile );
					RTFObject->ExtendFunction( "15", RTF_EXTFUNC_RMVSPACE );
					ss = S_OK;
				}
				else
				{
					LOGFILE( szTarFile, "Access Failed or Wrong format" );
					ss = S_FALSE;
				}
			}
			else
			{
				LOGFILE( szSrcFile, "Wrong Format" );
				LOGCONTENT( szSrcFile );
				ss = S_FALSE;
			}
		}
		else
		{
			ss = S_FALSE;
		}
	}

	// LOGFILE( szSrcFile, "End:Save" );
	if (ss==S_OK) 
		LOGFILE( szSrcFile, "End:Save" );
	else
		LOGFILE( szSrcFile, "Fail:Save" );

	if ( ss == S_OK ) strcpy(szTemp,"OK"); else strcpy(szTemp,"FAIL");
	len = strlen( szTemp );
	wchar_t* pwcbuf = new wchar_t[len+1];
	mbstowcs(pwcbuf, szTemp, len);
	pwcbuf[len]=0;
	*pzRes = SysAllocString(pwcbuf);	
	delete pwcbuf;
	return S_OK;
	// return ss;
}


//////////////////////////////////////////////////////////////
// Funtion TranslateToSignFile will be used to replace hidden 
// sign field with signature picture or signature string
//
// SrcFile is source file, it must be temp file, such as: 
// c:\inetrup\wwwroot\ehr\temp\3242.fn2
//
// TarFile is target file, it must be Archive file, such as: 
// \wfs\0001\32423.fn2
//
// This function first copy SrcFile to TarFile or ungluefile to save 
// document update, then replace field, insert signature picture, copy
// target file back to temp file or gluefile to create temp s.fn2 file
//

STDMETHODIMP CCreFnetFile::TranslateToSignFile(BSTR SrcFile, BSTR TarFile, BSTR SignPicName,
			 short signatoryID, short edit, short sign, BSTR usercode, BSTR password, short batch, BSTR* pzRes )
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char szSrcFile[256], szTarFile[256], szTemp[32], szDisp[256];
	CTime xTime = CTime::GetCurrentTime();
	HRESULT ss;
	int nRes;

	int len=SysStringLen(SrcFile);
	wcstombs(szSrcFile, SrcFile, len+1);
	szSrcFile[len]='\0';

	len=SysStringLen(TarFile);
	wcstombs(szTarFile, TarFile, len+1);
	szTarFile[len]='\0';

	WIN32_FIND_DATA finddata;
	HANDLE hresult;
	bool bRTF = false;

	sprintf(szTemp,"SignNo(%x)", batch);
	LOGFILE( szSrcFile, szTemp );
	if ((hresult=FindFirstFile(szSrcFile,&finddata))==INVALID_HANDLE_VALUE)
	{
		FindClose(hresult);
		ss = S_FALSE;
		LOGFILE( szSrcFile, "Not Exist." );
	}
	else	
	{
  		FindClose(hresult);

		//Create RTFProc, source file = SrcFile, dest file = TarFile
		if (GetFilename(SrcFile,TarFile)==S_OK) 
		{
			strcpy( szSrcFile, m_szSrcFile );
			strcpy( szTarFile, m_szTarFile );

			nRes = RTFObject->CheckValidate( szSrcFile );
			bRTF = (nRes>0);

			if ( nRes<0 )
			{
				LOGFILE( szSrcFile, "Access Failed" );
				ss = S_FALSE;
			}
			else if ( bRTF || (UnGlueFile()==S_OK))
			{
				ss = S_OK;
				if ( bRTF )
				{
					CString strSrcFile = szSrcFile;	
					// Update Archive file with src file
					if(strSrcFile.CompareNoCase(szTarFile) != 0)
					{
						// copy src file to target file
						// if ( !CopyFile( szSrcFile, szTarFile, false ) )
						if ( RTFObject->ExtendFunction( "", RTF_EXTFUNC_RMVSPEC1 )==-1 )
						{
							LOGFILE( szTarFile, "Copy File Failed" );
							ss = S_FALSE;
						}
					}
				}

				nRes = RTFObject->CheckValidate( szTarFile );
				if ((ss==S_OK) && ( nRes>0 ))
				{
					// Need to consider ZeroSize
					// Function SetRTFHeader will merge the head file and uploaded file 
					// in temp dir, create the target file  
					
					if (SetRTFHeader()==S_FALSE)
					{
						LOGFILE( szSrcFile, "Set Header/Footer Fail" );
					}
					else
					{
						LOGFILE( szSrcFile, "Before Insertion" );
					}

					if (RTFObject->CheckValidate( szTarFile ) > 0)
					{
						// Check Only sign
						if ( batch & 0x2000 ) nChkOnly  = 1; else nChkOnly = 0;

						if ( batch & 0x2000 ) goto CheckExit;

						if ( ss == S_OK )
						{
							LOGFILE( szSrcFile, "Replace variables" );
							
							sprintf( szDisp, "PreText= |SignDT=%s|", xTime.Format("%Y%m%d%H%M%S") );
							/* if ( batch & 0x0400 ) */ strcat( szDisp, "FinalText=FINAL REPORT|" );

							// After filed replaced, the src file name be set same as dest file in RTFObject
							RTFObject->FieldsReplace( szDisp );

							LOGFILE( szSrcFile, "Do Insertion" );
							ss = InsertSignPic(SignPicName,signatoryID);
								
							int nn = strlen(szDisp);
							wchar_t* pwcbuf = new wchar_t[nn+1];
							if ( pwcbuf != NULL )
							{
								mbstowcs(pwcbuf, szDisp, nn);
								pwcbuf[nn]=0;
								BSTR pzBstr = SysAllocString(pwcbuf);	
								delete pwcbuf; 
								
								if( bRTF )
								{
									Sleep( 500 );
									if (GetFilename(TarFile,SrcFile)==S_OK)
									{
										// copy signed file back to src file
										CString strSrcFile = szSrcFile;					
										if(strSrcFile.CompareNoCase(szTarFile) != 0)
										{
											if ( !CopyFile( szTarFile, szSrcFile, false ) )
											{
												LOGFILE( szTarFile, "Copy File Failed" );
												ss = S_FALSE;
											}
											else
												Sleep( 500 );
											GetRTFHeader();
										}
									}
									else
										ss = S_FALSE;
								}
								else
								{
									if (GetFilename(TarFile,SrcFile)==S_OK)
									{
										GetRTFHeader();
										LOGFILE( szSrcFile, "Regluefile" );
										if (GlueFile(edit,sign,usercode,password,batch,/*(BSTR)COleVariant(szDisp, VT_BSTR)*/
													 (BSTR)pzBstr)==S_OK)
											ss = S_OK;
										else
											ss = S_FALSE;
									}
									else
										ss = S_FALSE;
								}
							}
							else
							{
								LOGFILE( szTarFile, "Memory allocation error" );
								ss = S_FALSE;
							}
						}
						else
						{
							if ( ss == S_FALSE )
							{
								LOGFILE( szTarFile, "Sig File Insertion Error" );
							}
							else
							{
								LOGFILE( szTarFile, "No Sign Fields." );
							}
						}
					}
					else
					{
						LOGFILE( szTarFile, "Failed Access or Wrong Format" );
						ss = S_FALSE;
					}
				}
				else
				{
					LOGFILE( szTarFile, "Failed Access or Wrong Format" );
					ss = S_FALSE;
				}
			}
			else
			{
				LOGFILE( szSrcFile, "Wrong Format" );
				LOGCONTENT( szSrcFile );
				ss = S_FALSE;
			}
		}
		else
			ss = S_FALSE;
	}

CheckExit:
	if (ss==S_OK) 
		LOGFILE( szSrcFile, "End:Sign" );

	if ( ss == S_OK ) strcpy(szTemp,"OK"); 
	else if ( ss == E_FAIL ) strcpy(szTemp,"NOTFOUND");
	else strcpy(szTemp, "FAIL");
	len = strlen( szTemp );
	wchar_t* pwcbuf = new wchar_t[len+1];
	mbstowcs(pwcbuf, szTemp, len);
	pwcbuf[len]=0;
	*pzRes = SysAllocString(pwcbuf);	
	delete pwcbuf;
	return S_OK;
}


// Insert Barcode in RTF File
STDMETHODIMP CCreFnetFile::InsertBarCode( BSTR BarcodeStr )
{
	char pszfieldvalue[8100]={0};
	char szTmpL[60]={0};
	int nn;

	if (RTFObject!=NULL)
	{
		int len=SysStringLen( BarcodeStr );
		nn = wcstombs(pszfieldvalue, BarcodeStr, 8100);
		if (nn>=0) pszfieldvalue[nn]='\0';
		RTFObject->InsertRTFBarCode(pszfieldvalue);
		return S_OK;
	}
	else
		return S_FALSE;
}


// Check Validatity of the document before sign,
STDMETHODIMP CCreFnetFile::ValidSignProc(BSTR SrcFile, BSTR SignPicName, short batch, BSTR* pzRes )
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char szSrcFile[256], szTarFile[256], szSignPic[256], szTemp[32], szDisp[256];
	CTime xTime = CTime::GetCurrentTime();
	HRESULT ss;

	int len=SysStringLen(SrcFile);
	wcstombs(szSrcFile, SrcFile, len+1);
	szSrcFile[len]='\0';

	len=SysStringLen(SignPicName);
	wcstombs(szSignPic, SignPicName, len+1);
	szSignPic[len]='\0';

	strcpy( szTarFile, szSrcFile );
	szTarFile[strlen(szTarFile)-2]='_';
	szTarFile[strlen(szTarFile)-1]='0';

	WIN32_FIND_DATA finddata;
	HANDLE hresult;

	LOGFILE( szSrcFile, "SignCheck" );
	if ((hresult=FindFirstFile(szSrcFile,&finddata))==INVALID_HANDLE_VALUE)
	{
		FindClose(hresult);
		ss = S_FALSE;
		LOGFILE( szSrcFile, "Not Exist." );
	}
	else	
	{
  		FindClose(hresult);

		if (RTFObject!=NULL) delete RTFObject;
		
		RTFObject = new CRTFProc(szSrcFile, szTarFile);
		// RTFObject->m_piResponse = m_piResponse;
		if ( RTFObject != NULL ) 
		{
			if ( UnGlueFile()==S_OK )
			{
				if ( RTFObject->ExtendFunction( szSignPic, RTF_EXTFUNC_BLNKCHK ) == 0 )
				{
					ss = S_OK;
				}
				else
				{
					ss = S_FALSE;
					// Modified by solomon 03/31/2007
					DeleteFile( szTarFile );
				}
			}
			else
			{
				LOGFILE( szTarFile, "UnGlue Fail" );
				ss = S_FALSE;
			}

			delete RTFObject; RTFObject = NULL;
		}
		else
			ss = S_FALSE;
	}

	if (ss==S_OK) 
		LOGFILE( szSrcFile, "End:SignCheck" );

	if ( ss == S_OK ) strcpy(szTemp,"OK"); 
	else if ( ss == E_FAIL ) strcpy(szTemp,"NOTFOUND");
	else strcpy(szTemp, "FAIL");
	len = strlen( szTemp );
	wchar_t* pwcbuf = new wchar_t[len+1];
	mbstowcs(pwcbuf, szTemp, len);
	pwcbuf[len]=0;
	*pzRes = SysAllocString(pwcbuf);	
	delete pwcbuf;
	return S_OK;
}



STDMETHODIMP CCreFnetFile::GetRTFHeader()
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	if (RTFObject!=NULL)
	{
		if (!RTFObject->GetRTFHeader())
			return S_FALSE;
		else
			return S_OK;
	}
	else
		return S_FALSE;
}


STDMETHODIMP CCreFnetFile::SetRTFHeader()
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	if (RTFObject!=NULL)
	{
		if (!RTFObject->SetRTFHeader())
			return S_FALSE;
		else
			return S_OK;
	}
	else
		return S_FALSE;
}


STDMETHODIMP CCreFnetFile::GetNewFile(BSTR SrcFile, BSTR TarFile)
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char szSrcFile[256],szTarFile[256];

	// TODO: Add your implementation code here
	int len=SysStringLen(SrcFile);
	wcstombs(szSrcFile, SrcFile, len+1);
	szSrcFile[len]='\0';
	
	len=SysStringLen(TarFile);
	wcstombs(szTarFile, TarFile, len+1);
	szTarFile[len]='\0';

	VARIANT hintmsg;
	::VariantClear(&hintmsg);
	hintmsg.vt = VT_BSTR;

	char szBuf[250];
    WCHAR wcBuf[256];

	WIN32_FIND_DATA finddata;
	HANDLE hresult;
	if ((hresult=FindFirstFile(szSrcFile,&finddata))==INVALID_HANDLE_VALUE)
	{
		FindClose(hresult);
		strcpy(szBuf,"Source file not found! ");

	    memset(wcBuf, 0, sizeof(WCHAR)*256);
	    ::MultiByteToWideChar(CP_ACP, 0, szBuf, strlen(szBuf), wcBuf, 256);
   
	    // hintmsg.bstrVal = ::SysAllocString(wcBuf);
		// m_piResponse->Write(hintmsg);
  	    // ::SysFreeString(hintmsg.bstrVal);
		return S_FALSE;
	}
	else	
	{
		FindClose(hresult);
		RTFObject = new CRTFProc(szSrcFile,szTarFile);
		if ( RTFObject == NULL )
			return S_FALSE;
		// RTFObject->m_piResponse = m_piResponse;
		return S_OK;
	}
}


// Destine File Start with '#' indicate the Convert uPCM to PCM
// Destine File can also have tail seg such as D:\temp\sound.wav[XXX]
// where XXX can be UPCM, DLL(use DLL convert) or ALGO(use Algorithm convert)
STDMETHODIMP CCreFnetFile::Vox2WaveFile(BSTR SrcFile, BSTR TarFile)
{
	// AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char szSrcFile[256],szTarFile[256];
	char szBuf[512]={0};
	char *ps;

	int len=SysStringLen(SrcFile), n=0;
	wcstombs(szSrcFile, SrcFile, len+1);
	szSrcFile[len]='\0';
	
	len=SysStringLen(TarFile);
	wcstombs(szTarFile, TarFile, len+1);
	szTarFile[len]='\0';

	if ( szTarFile[0]=='#' ) 
	{
		strcpy(szTarFile, szTarFile+1);
		n |= OPT_UPCM_CONV;
	}

	if ( (ps=strrchr( szTarFile, '[' ))!=NULL )
	{
		ps[0]=0; ++ps;
		if ( strncmp(ps, "UPCM", 4)==0 )  n |= OPT_UPCM_CONV;
		else if ( strncmp(ps, "DLL", 4)==0 )  n |= OPT_ADPCM_DLL;
		else if ( strncmp(ps, "ALGO", 4)==0 )  n |= OPT_ADPCM_ALGO;
	}

	VARIANT hintmsg;
	::VariantClear(&hintmsg);
	hintmsg.vt = VT_BSTR;

	sprintf( szBuf, "%s->%s[%d]", szSrcFile, szTarFile, n );
	LOGFILE( szBuf, "Vox2Wav" );

	int nRes = ConvertVox2Wave( szSrcFile, szTarFile, szBuf, n );
	if ( nRes<0 )
	{
		LOGFILE( szBuf, "Vox2Err" );
	}

	if (nRes > 0)
		return S_OK;
	else
		return S_FALSE;

}

#include <io.h>

STDMETHODIMP CCreFnetFile::LabelGenerate(BSTR SheetFile, BSTR TmpltFile, BSTR FieldValues)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char szSheetFile[256], szTmpltFile[256], szFieldValues[8000];

	// TODO: Add your implementation code here
	int len=SysStringLen(SheetFile);
	wcstombs(szSheetFile, SheetFile, len+1);
	szSheetFile[len]='\0';
	
	len=SysStringLen(TmpltFile);
	wcstombs(szTmpltFile, TmpltFile, len+1);
	szTmpltFile[len]='\0';

	len=SysStringLen(FieldValues);
	wcstombs(szFieldValues, FieldValues, len+1);
	szFieldValues[len]='\0';

	int nn;
	long hFnds;
	struct _finddata_t fftmp;

	hFnds = _findfirst(szSheetFile, &fftmp);
	if (hFnds==-1) 
		CopyFile( szTmpltFile, szSheetFile, false );
	else
		_findclose(hFnds);

	if (RTFObject!=NULL) delete RTFObject;
	RTFObject = new CRTFProc(szSheetFile, szSheetFile);
	nn = RTFObject->FieldsReplace( szFieldValues, 1 );
	if (nn==1)
	{
		RTFObject->InsertSignPic( szTmpltFile, 10 );
		RTFObject->FieldsReplace( szFieldValues, 1 );
	}
	delete RTFObject;
	RTFObject=NULL;

	return S_OK;
}


STDMETHODIMP CCreFnetFile::ConvertFile(BSTR SrcFileName, BSTR TarFileName, int nOpt, BSTR CtrlStr, BSTR* pzRes )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char szSrcFile[256], szTemp[256];
	HRESULT ss;

	int len=SysStringLen(SrcFileName);
	wcstombs(szSrcFile, SrcFileName, len+1);
	szSrcFile[len]='\0';
	
	// LOGFILE( szSrcFile, "View" );
	WIN32_FIND_DATA finddata;
	HANDLE hresult;
	if ((hresult=FindFirstFile(szSrcFile, &finddata))==INVALID_HANDLE_VALUE)
	{
		// FindClose(hresult);
		LOGFILE( szSrcFile, "$$$NOFile" );
		ss = S_FALSE;
	}
	else	
	{
		if (finddata.nFileSizeLow <= 10)
		{
			LOGFILE( szSrcFile, "$$$ZeroSize" );
			// return S_FALSE;
		} 

		FindClose(hresult);
		LOGFILE( szSrcFile, "Conv" );
		if (GetFilename(SrcFileName,TarFileName)==S_OK)
		{
			if (RTFObject->CheckValidate( szSrcFile )>0)
			{
				/*if ( (batch&64)==0)  // 0x40, Not replace hidden fields
					ss = (RefreshField(FieldValue)==S_OK);
				else
					ss = true;*/
				RTFObject->ClearFields();
				// Convert RTFObject->szSourceFile -> szDestinationFile
				ss = RTFObject->RtfExport( 1, szTemp );
				if ( !ss )
				{
					LOGFILE( szTemp, "Convert Error!" );
					ss = S_FALSE;
				}
				else
					ss = S_OK;
			}
			else
			{
				ss = S_FALSE;
				LOGFILE( szSrcFile, "Access Failed or wrong format!" );
				LOGCONTENT( szSrcFile );
			}
		}
		else
			ss = S_FALSE;
	}
	LOGFILE( szSrcFile, "End:Conv" );

	if ( pzRes!=NULL )
	{
		if ( ss == S_OK ) strcpy(szTemp,"OK"); else strcpy(szTemp,"FAIL");
		len = strlen( szTemp );
		wchar_t* pwcbuf = new wchar_t[len+1];
		mbstowcs(pwcbuf, szTemp, len);
		pwcbuf[len]=0;
		*pzRes = SysAllocString(pwcbuf);	
		delete pwcbuf;
	}
	return S_OK;
}


//static nTest = 0;

STDMETHODIMP CCreFnetFile::CreateHtmlFile(BSTR szSrct, BSTR szDest, BSTR szParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

/*	char szTemp[100];
	sprintf( szTemp, "... %d", nTest++ );
	LOGFILE( szTemp, "Global Testing:" );
*/
	BSTR *pzRes = NULL;
	ConvertFile( szSrct, szDest, 1, szParam, pzRes );
	Sleep( 500 );
	return S_OK;
}

STDMETHODIMP CCreFnetFile::CreateRTFFile(BSTR szSrc, BSTR szDest, int nOption, BSTR szParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char szSrcFile[256], szTemp[256];
	BSTR *pzRes = NULL;
	HRESULT ss;

	// TODO: Add your implementation code here
	int len=SysStringLen(szSrc);
	wcstombs(szSrcFile, szSrc, len+1);
	szSrcFile[len]='\0';
	
	// LOGFILE( szSrcFile, "View" );
	WIN32_FIND_DATA finddata;
	HANDLE hresult;
	if ((hresult=FindFirstFile(szSrcFile, &finddata))==INVALID_HANDLE_VALUE)
	{
		// FindClose(hresult);
		LOGFILE( szSrcFile, "$$$NOFile" );
		ss = S_FALSE;
	}
	else	
	{
		if (finddata.nFileSizeLow <= 10)
		{
			LOGFILE( szSrcFile, "$$$ZeroSize" );
			// return S_FALSE;
		} 

		FindClose(hresult);
		LOGFILE( szSrcFile, "View" );
		if (GetFilename(szSrc,szDest)==S_OK)
		{
			ss = RTFObject->RtfImport( nOption, szTemp );
			if ( !ss )
			{
				LOGFILE( szTemp, "Convert Error!" );
				ss = S_FALSE;
			}
			else
				ss = S_OK;
		}
		else
		{
			ss = S_FALSE;
			LOGFILE( szSrcFile, "Access Failed!" );
		}
	}
	LOGFILE( szSrcFile, "End:View" );

	if ( pzRes!=NULL )
	{
		if ( ss == S_OK ) strcpy(szTemp,"OK"); else strcpy(szTemp,"FAIL");
		len = strlen( szTemp );
		wchar_t* pwcbuf = new wchar_t[len+1];
		mbstowcs(pwcbuf, szTemp, len);
		pwcbuf[len]=0;
		*pzRes = SysAllocString(pwcbuf);	
		delete pwcbuf;
	}
	return S_OK;
}
