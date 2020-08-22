/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Aug 22 12:52:29 2020
 */
/* Compiler settings for C:\Users\Qing\Documents\Qing\Source\vc60\FnetProducts\Gluefile\GlueFiles.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __GlueFiles_h__
#define __GlueFiles_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICreFnetFile_FWD_DEFINED__
#define __ICreFnetFile_FWD_DEFINED__
typedef interface ICreFnetFile ICreFnetFile;
#endif 	/* __ICreFnetFile_FWD_DEFINED__ */


#ifndef __CreFnetFile_FWD_DEFINED__
#define __CreFnetFile_FWD_DEFINED__

#ifdef __cplusplus
typedef class CreFnetFile CreFnetFile;
#else
typedef struct CreFnetFile CreFnetFile;
#endif /* __cplusplus */

#endif 	/* __CreFnetFile_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ICreFnetFile_INTERFACE_DEFINED__
#define __ICreFnetFile_INTERFACE_DEFINED__

/* interface ICreFnetFile */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICreFnetFile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7408BACE-D724-11D3-A70A-000021E20B2D")
    ICreFnetFile : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnStartPage( 
            /* [in] */ IUnknown __RPC_FAR *piUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFilename( 
            /* [in] */ BSTR SourceFileName,
            /* [in] */ BSTR DestinationFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RefreshField( 
            /* [in] */ BSTR FieldValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InsertSignPic( 
            /* [in] */ BSTR PicFileName,
            /* [in] */ short signatoryID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GlueFile( 
            /* [in] */ short edit,
            /* [in] */ short sign,
            /* [in] */ BSTR usercode,
            /* [in] */ BSTR password,
            /* [in] */ short batch,
            /* [in] */ BSTR fields) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnGlueFile( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveFile( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateViewFile( 
            /* [in] */ BSTR SrcFileName,
            /* [in] */ BSTR TarFileName,
            /* [in] */ BSTR FieldValue,
            /* [in] */ short edit,
            /* [in] */ short sign,
            /* [in] */ BSTR usercode,
            /* [in] */ BSTR password,
            /* [in] */ short batch,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR TarFile,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TranslateToSignFile( 
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR TarFileName,
            /* [in] */ BSTR SignPicName,
            /* [in] */ short signatoryID,
            /* [in] */ short edit,
            /* [in] */ short sign,
            /* [in] */ BSTR usercode,
            /* [in] */ BSTR password,
            /* [in] */ short batch,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRTFHeader( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRTFHeader( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNewFile( 
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR TarFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Vox2WaveFile( 
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR TarFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LabelGenerate( 
            /* [in] */ BSTR SheetFile,
            /* [in] */ BSTR TmpltFile,
            /* [in] */ BSTR FiledValues) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConvertFile( 
            /* [in] */ BSTR SrcFileName,
            /* [in] */ BSTR TarFileName,
            /* [in] */ int nOpt,
            /* [in] */ BSTR CtrlStr,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateHtmlFile( 
            BSTR szSrct,
            BSTR szDest,
            BSTR szParam) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateRTFFile( 
            BSTR szSrc,
            BSTR szDest,
            int nOption,
            BSTR szParam) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ValidSignProc( 
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR SignPicName,
            /* [in] */ short batch,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InsertBarCode( 
            /* [in] */ BSTR BarcodeStr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICreFnetFileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICreFnetFile __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICreFnetFile __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICreFnetFile __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnStartPage )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *piUnk);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnEndPage )( 
            ICreFnetFile __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFilename )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SourceFileName,
            /* [in] */ BSTR DestinationFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RefreshField )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR FieldValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertSignPic )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR PicFileName,
            /* [in] */ short signatoryID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GlueFile )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ short edit,
            /* [in] */ short sign,
            /* [in] */ BSTR usercode,
            /* [in] */ BSTR password,
            /* [in] */ short batch,
            /* [in] */ BSTR fields);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnGlueFile )( 
            ICreFnetFile __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveFile )( 
            ICreFnetFile __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateViewFile )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SrcFileName,
            /* [in] */ BSTR TarFileName,
            /* [in] */ BSTR FieldValue,
            /* [in] */ short edit,
            /* [in] */ short sign,
            /* [in] */ BSTR usercode,
            /* [in] */ BSTR password,
            /* [in] */ short batch,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR TarFile,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TranslateToSignFile )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR TarFileName,
            /* [in] */ BSTR SignPicName,
            /* [in] */ short signatoryID,
            /* [in] */ short edit,
            /* [in] */ short sign,
            /* [in] */ BSTR usercode,
            /* [in] */ BSTR password,
            /* [in] */ short batch,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRTFHeader )( 
            ICreFnetFile __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetRTFHeader )( 
            ICreFnetFile __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNewFile )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR TarFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Vox2WaveFile )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR TarFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LabelGenerate )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SheetFile,
            /* [in] */ BSTR TmpltFile,
            /* [in] */ BSTR FiledValues);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertFile )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SrcFileName,
            /* [in] */ BSTR TarFileName,
            /* [in] */ int nOpt,
            /* [in] */ BSTR CtrlStr,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateHtmlFile )( 
            ICreFnetFile __RPC_FAR * This,
            BSTR szSrct,
            BSTR szDest,
            BSTR szParam);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateRTFFile )( 
            ICreFnetFile __RPC_FAR * This,
            BSTR szSrc,
            BSTR szDest,
            int nOption,
            BSTR szParam);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ValidSignProc )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR SrcFile,
            /* [in] */ BSTR SignPicName,
            /* [in] */ short batch,
            /* [retval][out] */ BSTR __RPC_FAR *pzRes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertBarCode )( 
            ICreFnetFile __RPC_FAR * This,
            /* [in] */ BSTR BarcodeStr);
        
        END_INTERFACE
    } ICreFnetFileVtbl;

    interface ICreFnetFile
    {
        CONST_VTBL struct ICreFnetFileVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICreFnetFile_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICreFnetFile_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICreFnetFile_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICreFnetFile_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICreFnetFile_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICreFnetFile_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICreFnetFile_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICreFnetFile_OnStartPage(This,piUnk)	\
    (This)->lpVtbl -> OnStartPage(This,piUnk)

#define ICreFnetFile_OnEndPage(This)	\
    (This)->lpVtbl -> OnEndPage(This)

#define ICreFnetFile_GetFilename(This,SourceFileName,DestinationFileName)	\
    (This)->lpVtbl -> GetFilename(This,SourceFileName,DestinationFileName)

#define ICreFnetFile_RefreshField(This,FieldValue)	\
    (This)->lpVtbl -> RefreshField(This,FieldValue)

#define ICreFnetFile_InsertSignPic(This,PicFileName,signatoryID)	\
    (This)->lpVtbl -> InsertSignPic(This,PicFileName,signatoryID)

#define ICreFnetFile_GlueFile(This,edit,sign,usercode,password,batch,fields)	\
    (This)->lpVtbl -> GlueFile(This,edit,sign,usercode,password,batch,fields)

#define ICreFnetFile_UnGlueFile(This)	\
    (This)->lpVtbl -> UnGlueFile(This)

#define ICreFnetFile_SaveFile(This)	\
    (This)->lpVtbl -> SaveFile(This)

#define ICreFnetFile_CreateViewFile(This,SrcFileName,TarFileName,FieldValue,edit,sign,usercode,password,batch,pzRes)	\
    (This)->lpVtbl -> CreateViewFile(This,SrcFileName,TarFileName,FieldValue,edit,sign,usercode,password,batch,pzRes)

#define ICreFnetFile_Save(This,SrcFile,TarFile,pzRes)	\
    (This)->lpVtbl -> Save(This,SrcFile,TarFile,pzRes)

#define ICreFnetFile_TranslateToSignFile(This,SrcFile,TarFileName,SignPicName,signatoryID,edit,sign,usercode,password,batch,pzRes)	\
    (This)->lpVtbl -> TranslateToSignFile(This,SrcFile,TarFileName,SignPicName,signatoryID,edit,sign,usercode,password,batch,pzRes)

#define ICreFnetFile_GetRTFHeader(This)	\
    (This)->lpVtbl -> GetRTFHeader(This)

#define ICreFnetFile_SetRTFHeader(This)	\
    (This)->lpVtbl -> SetRTFHeader(This)

#define ICreFnetFile_GetNewFile(This,SrcFile,TarFile)	\
    (This)->lpVtbl -> GetNewFile(This,SrcFile,TarFile)

#define ICreFnetFile_Vox2WaveFile(This,SrcFile,TarFile)	\
    (This)->lpVtbl -> Vox2WaveFile(This,SrcFile,TarFile)

#define ICreFnetFile_LabelGenerate(This,SheetFile,TmpltFile,FiledValues)	\
    (This)->lpVtbl -> LabelGenerate(This,SheetFile,TmpltFile,FiledValues)

#define ICreFnetFile_ConvertFile(This,SrcFileName,TarFileName,nOpt,CtrlStr,pzRes)	\
    (This)->lpVtbl -> ConvertFile(This,SrcFileName,TarFileName,nOpt,CtrlStr,pzRes)

#define ICreFnetFile_CreateHtmlFile(This,szSrct,szDest,szParam)	\
    (This)->lpVtbl -> CreateHtmlFile(This,szSrct,szDest,szParam)

#define ICreFnetFile_CreateRTFFile(This,szSrc,szDest,nOption,szParam)	\
    (This)->lpVtbl -> CreateRTFFile(This,szSrc,szDest,nOption,szParam)

#define ICreFnetFile_ValidSignProc(This,SrcFile,SignPicName,batch,pzRes)	\
    (This)->lpVtbl -> ValidSignProc(This,SrcFile,SignPicName,batch,pzRes)

#define ICreFnetFile_InsertBarCode(This,BarcodeStr)	\
    (This)->lpVtbl -> InsertBarCode(This,BarcodeStr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICreFnetFile_OnStartPage_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *piUnk);


void __RPC_STUB ICreFnetFile_OnStartPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICreFnetFile_OnEndPage_Proxy( 
    ICreFnetFile __RPC_FAR * This);


void __RPC_STUB ICreFnetFile_OnEndPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_GetFilename_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SourceFileName,
    /* [in] */ BSTR DestinationFileName);


void __RPC_STUB ICreFnetFile_GetFilename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_RefreshField_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR FieldValue);


void __RPC_STUB ICreFnetFile_RefreshField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_InsertSignPic_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR PicFileName,
    /* [in] */ short signatoryID);


void __RPC_STUB ICreFnetFile_InsertSignPic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_GlueFile_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ short edit,
    /* [in] */ short sign,
    /* [in] */ BSTR usercode,
    /* [in] */ BSTR password,
    /* [in] */ short batch,
    /* [in] */ BSTR fields);


void __RPC_STUB ICreFnetFile_GlueFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_UnGlueFile_Proxy( 
    ICreFnetFile __RPC_FAR * This);


void __RPC_STUB ICreFnetFile_UnGlueFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_SaveFile_Proxy( 
    ICreFnetFile __RPC_FAR * This);


void __RPC_STUB ICreFnetFile_SaveFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_CreateViewFile_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SrcFileName,
    /* [in] */ BSTR TarFileName,
    /* [in] */ BSTR FieldValue,
    /* [in] */ short edit,
    /* [in] */ short sign,
    /* [in] */ BSTR usercode,
    /* [in] */ BSTR password,
    /* [in] */ short batch,
    /* [retval][out] */ BSTR __RPC_FAR *pzRes);


void __RPC_STUB ICreFnetFile_CreateViewFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_Save_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SrcFile,
    /* [in] */ BSTR TarFile,
    /* [retval][out] */ BSTR __RPC_FAR *pzRes);


void __RPC_STUB ICreFnetFile_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_TranslateToSignFile_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SrcFile,
    /* [in] */ BSTR TarFileName,
    /* [in] */ BSTR SignPicName,
    /* [in] */ short signatoryID,
    /* [in] */ short edit,
    /* [in] */ short sign,
    /* [in] */ BSTR usercode,
    /* [in] */ BSTR password,
    /* [in] */ short batch,
    /* [retval][out] */ BSTR __RPC_FAR *pzRes);


void __RPC_STUB ICreFnetFile_TranslateToSignFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_GetRTFHeader_Proxy( 
    ICreFnetFile __RPC_FAR * This);


void __RPC_STUB ICreFnetFile_GetRTFHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_SetRTFHeader_Proxy( 
    ICreFnetFile __RPC_FAR * This);


void __RPC_STUB ICreFnetFile_SetRTFHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_GetNewFile_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SrcFile,
    /* [in] */ BSTR TarFile);


void __RPC_STUB ICreFnetFile_GetNewFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_Vox2WaveFile_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SrcFile,
    /* [in] */ BSTR TarFile);


void __RPC_STUB ICreFnetFile_Vox2WaveFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_LabelGenerate_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SheetFile,
    /* [in] */ BSTR TmpltFile,
    /* [in] */ BSTR FiledValues);


void __RPC_STUB ICreFnetFile_LabelGenerate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_ConvertFile_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SrcFileName,
    /* [in] */ BSTR TarFileName,
    /* [in] */ int nOpt,
    /* [in] */ BSTR CtrlStr,
    /* [retval][out] */ BSTR __RPC_FAR *pzRes);


void __RPC_STUB ICreFnetFile_ConvertFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_CreateHtmlFile_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    BSTR szSrct,
    BSTR szDest,
    BSTR szParam);


void __RPC_STUB ICreFnetFile_CreateHtmlFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_CreateRTFFile_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    BSTR szSrc,
    BSTR szDest,
    int nOption,
    BSTR szParam);


void __RPC_STUB ICreFnetFile_CreateRTFFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_ValidSignProc_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR SrcFile,
    /* [in] */ BSTR SignPicName,
    /* [in] */ short batch,
    /* [retval][out] */ BSTR __RPC_FAR *pzRes);


void __RPC_STUB ICreFnetFile_ValidSignProc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICreFnetFile_InsertBarCode_Proxy( 
    ICreFnetFile __RPC_FAR * This,
    /* [in] */ BSTR BarcodeStr);


void __RPC_STUB ICreFnetFile_InsertBarCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICreFnetFile_INTERFACE_DEFINED__ */



#ifndef __GLUEFILESLib_LIBRARY_DEFINED__
#define __GLUEFILESLib_LIBRARY_DEFINED__

/* library GLUEFILESLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_GLUEFILESLib;

EXTERN_C const CLSID CLSID_CreFnetFile;

#ifdef __cplusplus

class DECLSPEC_UUID("F2FEDE32-9910-11D2-BB1E-0040053B6D66")
CreFnetFile;
#endif
#endif /* __GLUEFILESLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
