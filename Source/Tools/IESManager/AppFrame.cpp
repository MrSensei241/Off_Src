#include "stdafx.h"

#include "imcd3d9.h"

#include "imcfont.h"
#include "imcUiRender.h"

#include "App.h"
#include "AppFrame.h"
#include "AppProgressFrame.h"


namespace app
{


// NOTE : UI 프레임
CProgressFrame*	s_progressFrame	= NULL;

void InitFrames()
{
	RECT progressFrameRect = {0, GetClientHeight() - 20, GetClientWidth(), GetClientHeight()};

	s_progressFrame = new CProgressFrame;
	s_progressFrame->Init("LayerProgressFrame", progressFrameRect);
}

void ExitFrames()
{
	SAFE_DELETE(s_progressFrame);
}

void UpdateFrames(float elapsedTime)
{
	s_progressFrame->Update(elapsedTime);
}

void RenderFrames()
{
	s_progressFrame->Render();
}

void ResizeFrames(int width, int height)
{
	s_progressFrame->ResizeFrame(width, height);
}

void UpdateSelection(BOOL isPlayingObject)
{
	if (isPlayingObject == TRUE)
		s_progressFrame->CancelProgress();

	s_progressFrame->UpdateSelection();
}

void MouseMoveFrames(int x, int y)
{
	s_progressFrame->MouseMove(x, y);
}

void LButtonDownFrames(int x, int y)
{
	s_progressFrame->LButtonDown(x, y);	
}

CAppFrame::CAppFrame() :
		m_name("None"),
		m_isVisible(TRUE)
{
	
}

} 