#pragma once

namespace app
{

class CAppFrame
{
public:

	CAppFrame();
	virtual				~CAppFrame(){}

	virtual void		Init(const char* name, const RECT& surfaceRect) {}
	virtual void		Exit() {}

	void				SetVisible(BOOL isVisible);
	void				ToggleVisible();
	BOOL				IsVisible();	

	virtual void		ResizeFrame(int width, int height) {}

	virtual	void		Update(float elapsedTime) {}
	virtual void		Render() {}

protected:

private:
	
	std::string			m_name;
	BOOL				m_isVisible;
};

inline void	CAppFrame::SetVisible(BOOL isVisible)
{
	m_isVisible = isVisible;
}

inline BOOL	CAppFrame::IsVisible()
{
	return m_isVisible;
}

inline void CAppFrame::ToggleVisible()
{
	m_isVisible = !m_isVisible;
}

void InitFrames();
void ExitFrames();
void UpdateFrames(float elapsedTime);
void RenderFrames();

void ResizeFrames(int width, int height);

void MouseMoveFrames(int x, int y);
void LButtonDownFrames(int x, int y);

void UpdateSelection(BOOL isPlayingObject);

};
