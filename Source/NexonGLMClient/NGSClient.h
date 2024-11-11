#pragma once

#ifdef NEXONGLMCLIENT_EXPORTS
#define DLLFunction _declspec(dllexport)
#else
#define DLLFunction _declspec(dllimport)
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

typedef void(__stdcall *TSendCallback)(
	unsigned char* buf,
	unsigned long bufSize);

extern "C"{
	DLLFunction unsigned int	Start_IMCDLL(IN TSendCallback sc, IN bool needPatch = true);
	DLLFunction void			Stop_IMCDLL();
	DLLFunction void			SetNGLUserIDA_IMCDLL(IN const char* userid);
	DLLFunction void			SetNGLUserIDW_IMCDLL(IN const wchar_t* userid);
	DLLFunction void			GethardwareID_IMCDLL(OUT char * hwid);
	DLLFunction void			Control_IMCDLL(PBYTE buf, DWORD bufsize);
}