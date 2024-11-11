// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.

#include "stdafx.h"
#include "windows.h"
#include "dllmain.h"
#include "EmbeddedBrowser.h"


class Embedded_Browser;

LRESULT CALLBACK BrowserWndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI BrowserThreadProc(LPVOID lpParam);
DWORD WINAPI ThreadProc(LPVOID lpParam);
void NaviWeb(const char * webAddress);

static HINSTANCE	g_hModule_hInst;
static HWND	g_HWND;
static HANDLE  g_hThread;
static DWORD   g_dwThread;
static std::string g_webAddress;

Embedded_Browser* g_pEmbedded_Browser;

typedef struct NAVIDATA
{
	HICON		hIcon;
	std::string caption;
	SIZE		szRect;
} NAVI_DATA, *PNAVI_DATA;


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
		//--PROCESS--
	case DLL_PROCESS_ATTACH:
	{
							   g_HWND = NULL;
							   g_hModule_hInst = hModule;
	}
		break;
	case DLL_PROCESS_DETACH:
	{
	}
		break;

		//--THREAD--
	case DLL_THREAD_ATTACH:
	{
	}
		break;
	case DLL_THREAD_DETACH:
	{
	}
		break;
	}

	return TRUE;
}

void MultibyteToWide(const std::string& multi, std::wstring* pOut)
{
	int len = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, multi.c_str(), -1, NULL, 0);
	if (len != 0)
	{
		pOut->resize(len);
		wchar_t *ptr = (wchar_t*)pOut->c_str();
		MultiByteToWideChar(CP_ACP, 0, multi.c_str(), -1, ptr, len);
		pOut->shrink_to_fit();
		pOut->pop_back();
	}
}

// ----- 임베디드 브라우저 창 -----

ATOM RegisterClass_BrowserWnd(PNAVI_DATA pData)
{
	WNDCLASSEX wcex;
	wcex.hInstance = g_hModule_hInst;
	wcex.lpszClassName = "EmbeddedBrowser";
	wcex.lpfnWndProc = BrowserWndProc;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hIcon = pData->hIcon;
	wcex.hIconSm = pData->hIcon;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszMenuName = NULL;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	
	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK BrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE:
	{
		RECT rcClient;
		GetClientRect(hWnd, &rcClient);
		g_pEmbedded_Browser = new Embedded_Browser(rcClient);
		HRESULT hr = g_pEmbedded_Browser->Init(hWnd);
		if (SUCCEEDED(hr))
		{
			HWND hwnd_Explorer = 0;
			g_pEmbedded_Browser->GetWindow(&hwnd_Explorer);
			SetParent(hwnd_Explorer, hWnd);

			NaviVariant m_navVariants;
			::VariantInit(&m_navVariants.vtUrl);
			::VariantInit(&m_navVariants.vtTaget);
			::VariantInit(&m_navVariants.vtPostdata);
			::VariantInit(&m_navVariants.vtHeader);
			::VariantInit(&m_navVariants.vtFlags);

			std::wstring wstrTemp;
			MultibyteToWide(g_webAddress.c_str(), &wstrTemp);
			m_navVariants.vtUrl.vt = VT_BSTR;
			m_navVariants.vtUrl.bstrVal = ::SysAllocString(wstrTemp.c_str());;
			m_navVariants.vtFlags.vt = VT_I4;
			m_navVariants.vtFlags.lVal = navNoHistory | navNoReadFromCache | navNoWriteToCache | navTrustedForActiveX;

			g_pEmbedded_Browser->Navigate(&m_navVariants);
		}
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

DWORD WINAPI BrowserThreadProc(LPVOID lpParam)
{
	MSG messages;
	PNAVI_DATA pData = (PNAVI_DATA)lpParam;

	RegisterClass_BrowserWnd(pData);
	g_HWND = CreateWindowEx(		
		0, "EmbeddedBrowser", pData->caption.c_str(), 
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_VISIBLE, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		pData->szRect.cx, pData->szRect.cy,
		HWND_DESKTOP, nullptr, 
		g_hModule_hInst, nullptr
		);
	
	if (g_HWND == NULL)
		return 0;
	
	ShowWindow(g_HWND, SW_SHOWNORMAL);
	HeapFree(GetProcessHeap(), 0, pData);
 	while (GetMessage(&messages, NULL, 0, 0))
 	{
 		TranslateMessage(&messages);
 		DispatchMessage(&messages);
	}

	if (g_pEmbedded_Browser != NULL)
	{
		delete g_pEmbedded_Browser;
		g_pEmbedded_Browser = NULL;
	}

	g_hThread = NULL;
 	return 1;
 }

DLLFunction void _cdecl CloseBrowserWnd()
{
	if (IsWindow(g_HWND) == TRUE)
	{
		::SendMessage(g_HWND, WM_DESTROY, NULL, NULL);
		g_HWND = NULL;
	}
}

DLLFunction BOOL _cdecl CheckOpenedBrowserWnd()
{
	if (g_HWND != NULL)
	{
		return IsWindow(g_HWND);
	}
	return FALSE;
}

DLLFunction void _cdecl NaviBrowser(HICON& hIcon, const char * caption, const char * webAddress, int wndW, int wndH)
 {
	 //쓰레드에 넘겨 줄 데이터의 값을 설정한다. 
	 NAVI_DATA *pData = NULL;
	 pData = (PNAVI_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NAVIDATA));

	 if (pData == NULL || g_hThread != NULL)
	 {
		 // 메모리 할당에 실패하면 종료한다. 
		 return;
	 }

	 pData->hIcon = hIcon;
	 pData->caption = caption;
	 pData->szRect.cx = wndW;
	 pData->szRect.cy = wndH;
	 g_webAddress = webAddress;

	 // 쓰레드를 생성한다. 
	 g_hThread = CreateThread(0, NULL, BrowserThreadProc, pData, NULL, &g_dwThread);
	 return;
 }