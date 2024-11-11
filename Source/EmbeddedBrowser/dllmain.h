#pragma once

#define DLLFunction _declspec(dllexport)
extern "C"{	
	DLLFunction BOOL _cdecl CheckOpenedBrowserWnd();
	DLLFunction void _cdecl CloseBrowserWnd();
	DLLFunction void _cdecl NaviBrowser(HICON&	hIcon, const char * caption, const char * webAddress, int wndW, int wndH);
}
