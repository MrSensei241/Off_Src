#pragma once

#include "Dictionalization.h"

class CWindowFrame 
{
public:
	CWindowFrame();
	CWindowFrame(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow,  LPSTR WindowName);
	~CWindowFrame(void);	
public:
	void				RegWindow();
	bool				CreateWnd();	
	void				SetProperty(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow,  LPSTR WindowName);	
	bool				BeginFrame();	
	void				SetWindowSize(UINT x, UINT y, UINT cx, UINT cy);
	const	HWND		GethWnd() const { return m_hWnd; }
	bool				IsCreateWindow() const { return (m_hWnd!=NULL); }
	LRESULT CALLBACK	WindowProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

private:
	bool				InitWindowSize();

private:
	bool				Init();
	void				Exit();

private:
	CDictionalization	m_dic;

private:		
	HWND			m_hWnd;
	MSG				m_message;
	WNDCLASS		m_wndClass;
	int				m_cmdShow;
	HINSTANCE		m_hInstance;
	HINSTANCE		m_hPrevInstance;
	LPSTR			m_pCmdParam;	
	LPSTR			m_pClass;

private:
	UINT			m_windowX;
	UINT			m_windowY;
	UINT			m_windowCX;
	UINT			m_windowCY;
};
