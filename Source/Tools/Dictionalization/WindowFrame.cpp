#include "StdAfx.h"

#include "WindowFrame.h"

#include <shellapi.h>
#include <WinUser.h>
#include "boost/nowide/convert.hpp"

HWND g_hWnd;
CWindowFrame g_WndFrame;
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

static LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{	
	HDC hdc;
	PAINTSTRUCT ps;
	switch(iMessage) {
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		{
			std::string str  = "사전 및 다국어용 .xml, .lua 만들기";
			std::string str1 = "파일 또는 폴더를 드래그 앤 드랍하세요";
			std::string str2 = "만드는 도중 이 윈도우가 응답없음 상태가 됩니다. 긴장하지 마세요";
			TextOutW(hdc, 10, 10, boost::nowide::widen(str).c_str(), boost::nowide::widen(str).size());
			TextOutW(hdc, 10, 30, boost::nowide::widen(str1).c_str(), boost::nowide::widen(str1).size());
			TextOutW(hdc, 10, 50, boost::nowide::widen(str2).c_str(), boost::nowide::widen(str2).size());
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		FreeConsole();
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch(wParam) {
	case VK_ESCAPE:
		FreeConsole();
		PostQuitMessage(0);
		return 0;
		}
	}
	return g_WndFrame.WindowProc(hWnd, iMessage, wParam, lParam);
}

LRESULT CALLBACK CWindowFrame::WindowProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{	
	switch(iMessage) {
	case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;
			wchar_t szFileName[OFS_MAXPATHNAME];
			UINT fileNumber = ::DragQueryFileW( hDrop, -1, szFileName, OFS_MAXPATHNAME);
			//printf("%d", fileNumber);
			LOG_LINE("%d", fileNumber);
			for ( UINT index = 0 ; index < fileNumber ; ++index ) {
				::DragQueryFileW( hDrop, index, szFileName, OFS_MAXPATHNAME);
				LOG_LINE("%s", boost::nowide::narrow(szFileName).c_str()); // 언제나 풀패스가 들어온다. 이게 맞는지는 모르겠지만
				//m_dic.MakeDictionary(boost::nowide::narrow(szFileName).c_str());
			}
		}
		break;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

CWindowFrame::CWindowFrame()
: m_pCmdParam(NULL),	m_cmdShow(0)
, m_pClass(NULL),		m_hWnd(NULL)
, m_windowX(0),			m_windowY(0)
, m_windowCX(0),		m_windowCY(0)
{

}


CWindowFrame::CWindowFrame(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow, LPSTR WindowName)
: m_hInstance(hInstance),		m_hPrevInstance(hPrevInstance)
, m_pCmdParam(lpszCmdParam),	m_cmdShow(nCmdShow)
, m_pClass(WindowName),			m_hWnd(NULL)
, m_windowX(0),					m_windowY(0)
, m_windowCX(0),				m_windowCY(0)
{	
}

CWindowFrame::~CWindowFrame(void)
{

}

void CWindowFrame::RegWindow()
{
	m_wndClass.cbClsExtra		= 0;
	m_wndClass.cbWndExtra		= 0;
	m_wndClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	m_wndClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	m_wndClass.hIcon			= LoadIcon(NULL,IDI_APPLICATION);
	m_wndClass.hInstance		= m_hInstance;
	m_wndClass.lpfnWndProc		= (WNDPROC)WndProc;
	m_wndClass.lpszClassName	= (m_pClass);
	m_wndClass.lpszMenuName		= NULL;
	m_wndClass.style			= CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&m_wndClass);
	m_hWnd=CreateWindowEx(WS_EX_ACCEPTFILES,
						  m_pClass, m_pClass,
						  WS_OVERLAPPEDWINDOW,
						  //WS_POPUPWINDOW,
						  m_windowX, m_windowY,
						  m_windowCX, m_windowCY,
						  NULL,
						  (HMENU)NULL,
						  m_hInstance,
						  NULL);
	
	g_hWnd = m_hWnd;
}

bool CWindowFrame::CreateWnd()
{
	InitWindowSize();
	ShowWindow(m_hWnd, m_cmdShow);
	UpdateWindow(m_hWnd);
	return true;
}

void CWindowFrame::SetProperty( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow, LPSTR WindowName )
{
	m_hInstance		= hInstance;
	m_hPrevInstance = hPrevInstance;
	m_pCmdParam		= lpszCmdParam;
	m_cmdShow		= nCmdShow;
	m_pClass		= WindowName;
}

bool CWindowFrame::BeginFrame()
{
	if ( Init() == false ) {
		::MessageBox(m_hWnd, "초기화 실패", "초기화 실패", MB_OK);
	}

	while(true) 
	{
		if (PeekMessage(&m_message,NULL,0,0,PM_REMOVE)) {
			if (m_message.message==WM_QUIT)
				break;
			TranslateMessage(&m_message);
			DispatchMessage(&m_message);
		} else {
			//Input Game MainLoop Here		Ex) GameMainLoop();
			///Insert Your Code
			Sleep(1);
		}
	}
	Exit();
	return true;
}

bool CWindowFrame::InitWindowSize()
{
	int ExWidth  = 0;
	int ExHeight = 0;
	RECT ClientRt, WindowRt;
	GetClientRect(m_hWnd, &ClientRt);
	GetWindowRect(m_hWnd, &WindowRt);

	ExWidth  = ( WindowRt.right - WindowRt.left) - ( ClientRt.right - ClientRt.left);
	ExHeight = ( WindowRt.bottom - WindowRt.top) - ( ClientRt.bottom - ClientRt.top);
	MoveWindow(m_hWnd, m_windowX, m_windowY, m_windowCX + ExWidth, m_windowCY + ExHeight , true);
	return true;
}

void CWindowFrame::SetWindowSize( UINT x, UINT y, UINT cx, UINT cy )
{
	m_windowX = x;
	m_windowY = y;
	m_windowCX = cx;
	m_windowCY = cy;
}

bool CWindowFrame::Init()
{
	return m_dic.Init();
}

void CWindowFrame::Exit()
{
	m_dic.Exit();
}
