/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sun Nov 05 19:53:03 2017
 */
/* Compiler settings for C:\Users\Qing\Documents\Qing\Source\vc60\FnetProducts\Gluefile\GlueFiles.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_ICreFnetFile = {0x7408BACE,0xD724,0x11D3,{0xA7,0x0A,0x00,0x00,0x21,0xE2,0x0B,0x2D}};


const IID LIBID_GLUEFILESLib = {0x7408BAC1,0xD724,0x11D3,{0xA7,0x0A,0x00,0x00,0x21,0xE2,0x0B,0x2D}};


const CLSID CLSID_CreFnetFile = {0xF2FEDE32,0x9910,0x11D2,{0xBB,0x1E,0x00,0x40,0x05,0x3B,0x6D,0x66}};


#ifdef __cplusplus
}
#endif

