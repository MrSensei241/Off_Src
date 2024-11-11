#include "stdafx.h"
#include "app.h"
#include "toolutil.h"
#include "toolnet.h"

namespace app
{

HWND				s_hWnd;
int					s_windowWidth;
int					s_windowHeight;
BOOL				s_isInitApp = FALSE;

HWND				s_sampleWindowHandle = NULL;

// NOTE : app 상태 관리
BOOL IsInitialized()
{
	return s_isInitApp;
}

int	GetClientWidth()
{
	return s_windowWidth;
}

int	GetClientHeight()
{
	return s_windowHeight;
}

void	Resize(int x, int y)
{

}

void	Play()
{

}

void	Deselect()
{

}

void	ProcessKeyDownMsg(WPARAM param)
{
	
}


BOOL Init(HWND hWnd, int wndWidth, int wndHeight)
{
	s_hWnd			= hWnd;
	s_windowWidth	= wndWidth;
	s_windowHeight	= wndHeight;

	util::InitTool();
	s_isInitApp = TRUE;
	return TRUE;
}


void Exit()
{
	util::ExitTool();
}

DWORD GetElapsedTime()
{
	static DWORD lastUpdated = timeGetTime();
	static DWORD elapsed = 0;
	elapsed += (timeGetTime() - lastUpdated);
	lastUpdated = timeGetTime();
	return elapsed;
}

void Run()
{
	if (GetElapsedTime() < 200)
		return;

	util::UpdatePaneVisible();
	net::Recv();
}

void						MouseMove(int x, int y)
{

}


}