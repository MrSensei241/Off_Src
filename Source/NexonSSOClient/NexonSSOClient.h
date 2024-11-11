#pragma once

#ifdef NEXONSSOCLIENT_EXPORTS
#define DLLFunction _declspec(dllexport)
#else
#define DLLFunction _declspec(dllimport)
#endif

extern "C"{

	DLLFunction bool _cdecl nexonSSOInit(HWND& hWnd, const char* nexonssoserver, const char* paramPassport, const char* serviceNation);
	DLLFunction void _cdecl SetPassport(const char* passPort);
	DLLFunction const char * _cdecl GetPassport();
	DLLFunction void _cdecl SetLogSID(const char* sid);
	DLLFunction const char * _cdecl GetLogSID();
	DLLFunction void _cdecl SetGameCode(const char * gamecode);
	DLLFunction const char * _cdecl GetGameCode();
	DLLFunction LRESULT _cdecl OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	DLLFunction void _cdecl UpdateNexonPassport();

}


