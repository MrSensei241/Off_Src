#pragma once

namespace imcUiRender
{
	class IDC;
}

namespace app
{

struct UnitProgressBar
{
	UnitProgressBar() :
		classType(EL_LAYER),
		isVisible(TRUE),
		name("None"),
		progressRatio(0.0f)
	{
		SetRect(&rect, 0, 0, 0, 0);
	};

	BOOL			isVisible;
	RECT			rect;
	std::string		name;
	float			progressRatio;
	EFFECT_CLASS	classType;
};

class CProgressFrame : public CAppFrame
{
public:

	CProgressFrame();
	virtual			~CProgressFrame();

	void			Init(const char* name, const RECT& surfaceRect);
	void			Exit();

	void			Update(float elapsedTime);
	void			Render();

	void			UpdateSelection();
	void			CancelProgress();

	void			ResizeFrame(int width, int height);
	void			MouseMove(int x, int y);
	void			LButtonDown(int x, int y);

protected:

	void			Clear();
	void			AddUnit(EFFECT_CLASS classType, const char* name, BOOL isVisible);

private:

	int								 m_lineHeight;
	
	RECT							 m_rect;
	imc::CVector< UnitProgressBar >	 m_unitBarList;
	imcUiRender::IDC*				 m_pDC;

	BOOL							 m_isPlaying;
	int								 m_cursorOnIndex;
	int								 m_totalUnitCount;

	EFFECT_PLAYUNIT					m_selectedUnitType;
	std::string						m_selectedUnitName;
};


};
