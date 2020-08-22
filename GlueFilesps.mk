
GlueFilesps.dll: dlldata.obj GlueFiles_p.obj GlueFiles_i.obj
	link /dll /out:GlueFilesps.dll /def:GlueFilesps.def /entry:DllMain dlldata.obj GlueFiles_p.obj GlueFiles_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del GlueFilesps.dll
	@del GlueFilesps.lib
	@del GlueFilesps.exp
	@del dlldata.obj
	@del GlueFiles_p.obj
	@del GlueFiles_i.obj
