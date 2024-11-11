#pragma once



namespace app
{

	BOOL	Init(HWND hWnd, int wndWidth, int wndHeight);
	void	Exit();
	void	Run();
	void	MouseMove(int x, int y);
	void	Play();
	void	Deselect();
	void	ProcessKeyDownMsg(WPARAM param);
	void	Resize(int x, int y);

	BOOL	IsInitialized();



};