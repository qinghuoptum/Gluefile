// CreFnetFile.h : Declaration of the CCreFnetFile

#ifndef __CREFNETFILE_H_
#define __CREFNETFILE_H_

#include "resource.h"       // main symbols
#include <asptlb.h>         // Active Server Pages Definitions
#include "RTFProc.h"

/////////////////////////////////////////////////////////////////////////////
// CCreFnetFile
class ATL_NO_VTABLE CCreFnetFile : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCreFnetFile, &CLSID_CreFnetFile>,
	public IDispatchImpl<ICreFnetFile, &IID_ICreFnetFile, &LIBID_GLUEFILESLib>
{
public:
	CCreFnetFile()
	{ 
		m_bOnStartPageCalled = FALSE;
		RTFObject = NULL;
		nChkOnly = 0;
		m_szTempFile[0]	= 0;
	}

	~CCreFnetFile()
	{
		if (m_szTempFile[0]) DeleteFile( m_szTempFile );
	}

public:

DECLARE_REGISTRY_RESOURCEID(IDR_CREFNETFILE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCreFnetFile)
	COM_INTERFACE_ENTRY(ICreFnetFile)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ICreFnetFile
public:
	STDMETHOD(CreateRTFFile)(BSTR szSrc, BSTR szDest, int nOption, BSTR szParam);
	STDMETHOD(CreateHtmlFile)(BSTR szSrct, BSTR szDest, BSTR szParam);
	STDMETHOD(ConvertFile)(/*[in]*/ BSTR SrcFileName, /*[in]*/ BSTR TarFileName, /*[in]*/ int nOpt, /*[in]*/ BSTR CtrlStr, /*[out]*/ BSTR* pzRes );
	STDMETHOD(LabelGenerate)(/*[in]*/ BSTR SheetFile, /*[in]*/ BSTR TmpltFile, /*[in]*/ BSTR FiledValues);
	STDMETHOD(Vox2WaveFile)(/*[in]*/ BSTR SrcFile, /*[in]*/ BSTR TarFile);
	STDMETHOD(GetNewFile)(/*[in]*/ BSTR SrcFile, /*[in]*/ BSTR TarFile);
	STDMETHOD(SetRTFHeader)();
	STDMETHOD(GetRTFHeader)();
	STDMETHOD(TranslateToSignFile)(/*[in]*/ BSTR SrcFile,/*[in]*/ BSTR TarFile,/*[in]*/ BSTR SignPicName,
		/*[in]*/ short signatoryID,/*[in]*/ short edit,/*[in]*/ short sign,/*[in]*/ BSTR usercode, /*[in]*/ BSTR password, /*[in]*/ short batch, /*[out]*/ BSTR* pzRes );
	STDMETHOD(Save)(/*[in]*/ BSTR SrcFile,/*[in]*/ BSTR TarFile, /*[out]*/ BSTR* pzRes );
	STDMETHOD(CreateViewFile)(/*[in]*/ BSTR SrcFileName,/*[in]*/ BSTR TarFileName,/*[in]*/ BSTR FieldValue,
		/*[in]*/ short edit,/*[in]*/ short sign,/*[in]*/ BSTR usercode, /*[in]*/ BSTR password, /*[in]*/ short batch, /*[out]*/ BSTR* pzRes );
	STDMETHOD(SaveFile)();
	STDMETHOD(UnGlueFile)();
	STDMETHOD(GlueFile)(/*[in]*/ short edit,/*[in]*/ short sign,/*[in]*/BSTR usercode,/*[in]*/BSTR password,/*[in]*/ short batch,/*[in]*/ BSTR fields);
	STDMETHOD(InsertSignPic)(/*[in]*/ BSTR PicFileName,/*[in]*/ short signatoryID);
	STDMETHOD(RefreshField)(/*[in]*/ BSTR FieldValue);
	STDMETHOD(GetFilename)(/*[in]*/ BSTR SourceFileName,/*[in]*/  BSTR DestinationFileName);
	//Active Server Pages Methods
	STDMETHOD(OnStartPage)(IUnknown* IUnk);
	STDMETHOD(OnEndPage)();
	STDMETHOD(ValidSignProc)(/*[in]*/BSTR SrcFile, /*[in]*/BSTR SignPicName, /*[in]*/short batch, 
		/*[out]*/BSTR* pzRes );
	STDMETHOD(InsertBarCode)( /*[in]*/BSTR BarcodeStr );

private:
	CComPtr<IRequest> m_piRequest;					//Request Object
	CComPtr<IResponse> m_piResponse;				//Response Object
	CComPtr<ISessionObject> m_piSession;			//Session Object
	CComPtr<IServer> m_piServer;					//Server Object
	CComPtr<IApplicationObject> m_piApplication;	//Application Object
	BOOL m_bOnStartPageCalled;						//OnStartPage successful?
	CRTFProc * RTFObject;

	int nChkOnly; 
	char m_szTempFile[256];
	char m_szSrcFile[256], m_szTarFile[256];

};

#endif //__CREFNETFILE_H_
