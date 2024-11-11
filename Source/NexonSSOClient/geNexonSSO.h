#pragma  once

//#ifdef USE_NEXON_SSO

#define	WM_NMEVENT	( WM_USER + 17 )

namespace geNexonSSO
{
	bool Init(HWND& hWnd, const char * nexonSSOServer, const char * nexonPassport, const char * serviceNation);
	void UpdateNexonPassport();
	LRESULT OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//#endif