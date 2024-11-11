#include "stdafx.h"

#include "imcd3d9.h"

#include "imcfont.h"
#include "imcUiRender.h"

#include "imc3d.h"
#include "imcEffect.h"
#include "imcEffectUtil.h"

#include "App.h"
#include "AppFrame.h"
#include "AppProgressFrame.h"

namespace app
{

CProgressFrame::CProgressFrame() :	
	m_lineHeight(20),	
	m_pDC(NULL),
	m_cursorOnIndex(-1),
	m_isPlaying(FALSE),
	m_selectedUnitType(EU_NONE)
{
	SetRect(&m_rect, 0, 0, 0, 0);
}	

CProgressFrame::~CProgressFrame()
{
	Exit();
}

void CProgressFrame::Init(const char* name, const RECT& surfaceRect)
{
	m_rect		= surfaceRect;

	int width	= m_rect.right - m_rect.left;
	int height	= m_rect.bottom - m_rect.top;

	m_pDC = imcUiRender::CreateDC(width, height);
}

void CProgressFrame::Exit()
{
	Clear();

	imcUiRender::DestroyDC(m_pDC);
	m_pDC = NULL;
}

void CProgressFrame::Update(float elapsedTime)
{
	if (IsVisible() == TRUE)
	{
		if (app::GetCurEffectInst() != NULL)
		{
			imcEffect::IEffectInstance* pEffectInstance = app::GetCurEffectInst();

			int layerInstCount = pEffectInstance->GetLayerInstCount();			

			for (int i = 0; i < layerInstCount; ++i)
			{
				imcEffect::ILayerInstance* pLayerInstance = pEffectInstance->GetLayerInst(i);
				m_unitBarList[i].progressRatio = pLayerInstance->GetProgressRatio();
			}

			if (pEffectInstance->IsEnd())
				m_isPlaying = FALSE;
			else
				m_isPlaying = TRUE;
			
		}
		else
		{
			m_isPlaying = FALSE;
		}
	}
}

void CProgressFrame::MouseMove(int x, int y)
{
	if (IsVisible() == TRUE)
	{
		m_cursorOnIndex = -1;

		if (x > m_rect.left && x < m_rect.right && y > m_rect.top && y < m_rect.bottom)
		{
			x -= m_rect.left;
			y -= m_rect.top;

			for (int i = 0; i < m_unitBarList.Count(); ++i)
			{
				const RECT& rect = m_unitBarList[i].rect;
				if (x > rect.left && x < rect.right && y > rect.top && y < rect.bottom)
				{
					m_cursorOnIndex = i;
					break;
				}
			}
		}
	}	
}

void CProgressFrame::LButtonDown(int x, int y)
{
	if (m_cursorOnIndex >= 0)	
	{
		switch (m_selectedUnitType)
		{
		case EU_EFFECT:
			{
				imcEffect::IEffect* pEffect = imcEffect::util::GetEffect(m_selectedUnitName.c_str());
				if (pEffect != NULL)
				{
					IMC_ASSERT(m_cursorOnIndex >= pEffect->GetLayerCount());
					imcEffect::ILayer* pLayer = pEffect->GetLayer(m_cursorOnIndex);
					pLayer->SetVisible(!pLayer->IsVisible());

					m_unitBarList[m_cursorOnIndex].isVisible = pLayer->IsVisible();
				}
			}
			break;
		}
	}
}

void CProgressFrame::Render()
{
	if (IsVisible() == TRUE && m_pDC != NULL)
	{
		int width			= m_rect.right - m_rect.left;
		int height			= m_rect.bottom - m_rect.top;

		DWORD bgColor		= 0x44000000;		
		DWORD disableColor	= 0xFF666666;

		DWORD cursorOnBgColor	= 0x44448844;
		DWORD cursorOnColor		= 0xFF88FF88;

		int	sideMargin			= 5;
		int verticalMargin		= 1;

		imcFont::SetFontState(imcFont::FS_SIZE, 16);

		if (m_pDC->SetRenderTarget() == TRUE)
		{
			m_pDC->Clear();

			RECT clearRect = {0, 0, width, height};
			m_pDC->FillColorRect(clearRect, bgColor);

			int x = 0;
			int y = 0;

			for (int i = 0; i < m_unitBarList.Count(); ++i)
			{				
				const UnitProgressBar& unitBar = m_unitBarList[i];
				x = unitBar.rect.left;
				y = unitBar.rect.top;

				DWORD nameColor	 = 0xFFDDCC36;
				DWORD gaugeColor = 0xFFDDCC36;

				if (m_cursorOnIndex == i)
				{
					nameColor	= cursorOnColor;
					gaugeColor	= cursorOnColor;

					RECT bgRect = {x + sideMargin, y + 1, x + width - sideMargin, y + m_lineHeight - 1};
					m_pDC->FillColorRect(bgRect, cursorOnBgColor);
				}

				if (unitBar.isVisible == FALSE)
				{
					nameColor	= disableColor;
					gaugeColor	= disableColor;
				}

				imcFont::SetFontState(imcFont::FS_COLOR, nameColor);

				x += sideMargin + 1;	
				imcFont::DrawText(x, y, unitBar.name.c_str());

				x += 150;
				int gaugeValue  = (width - x - sideMargin - 1) * unitBar.progressRatio;
				RECT gaugeRect = {x, y + 5, x + gaugeValue, y + m_lineHeight - 5};
				m_pDC->FillColorRect(gaugeRect, gaugeColor);
			}

			m_pDC->ReleaseRenderTarget();
		}

		m_pDC->Render(m_rect.left, m_rect.top, width , height, 0.8f);
	}
}

void CProgressFrame::Clear()
{
	m_unitBarList.RemoveAll();
}

void CProgressFrame::CancelProgress()
{
	m_isPlaying = FALSE;
}

void CProgressFrame::UpdateSelection()
{
	if (m_isPlaying == FALSE)
	{
		Clear();

		m_selectedUnitType = app::GetSelectedUnitType();

		if (m_selectedUnitType == EU_NONE)
			return;
		
		m_selectedUnitName = app::GetSelectedUnitName();

		switch (m_selectedUnitType)
		{
		case EU_EFFECT:
			{
				imcEffect::IEffect* pEffect = app::GetSelectedEffect();
				IMC_ASSERT(pEffect != NULL);

				m_totalUnitCount = pEffect->GetLayerCount() + pEffect->GetSoundCount();

				for (int i = 0; i < pEffect->GetLayerCount(); ++i)
				{
					imcEffect::ILayer* pLayer = pEffect->GetLayer(i);
					IMC_ASSERT(pLayer);

					if (pLayer->IsChild() == FALSE) {
						AddUnit(EL_LAYER, pLayer->GetName(), pLayer->IsVisible());
					}
				}
			}
			break;

		case EU_EMITTER:
			{
				imcEffect::IEmitter* pEmitter = app::GetSelectedEmitter();
				IMC_ASSERT(pEmitter != NULL);

				m_totalUnitCount = pEmitter->GetEventCount();

				for (int i = 0; i < pEmitter->GetEventCount(); ++i)
				{
					imcEffect::IEmitterEvent* pEmitterEvent = pEmitter->GetEvent(i);
					IMC_ASSERT(pEmitterEvent);

					AddUnit(EL_EMITTERNODE, pEmitterEvent->nodeName.c_str(), pEmitterEvent->isEnable);
				}
			}
			break;

		case EU_TRAIL:
			{
			}
			break;
		}

		int height = m_unitBarList.Count() * m_lineHeight;
		m_rect.top = m_rect.bottom - height;

		m_pDC->Resize(m_rect.left, m_rect.top, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top);
	}
}

void CProgressFrame::AddUnit(EFFECT_CLASS classType, const char* name, BOOL isVisible)
{
	UnitProgressBar unitBar;					
	unitBar.name		= name;
	unitBar.classType	= classType;
	unitBar.isVisible	= isVisible;

	int width		= m_rect.right - m_rect.left;
	int height		= m_totalUnitCount * m_lineHeight;	

	int index		= m_unitBarList.Count();
	int offsetY		= index * m_lineHeight;

	SetRect(&unitBar.rect, 0, offsetY, width, offsetY + m_lineHeight); 

	m_unitBarList.AddToTail(unitBar);
}

void CProgressFrame::ResizeFrame(int width, int height)
{
	m_rect.right	= m_rect.left + width;
	m_rect.bottom	= height;	

	int offsetHeight = max(1, m_unitBarList.Count()) * m_lineHeight;
	m_rect.top = m_rect.bottom - offsetHeight;

	m_pDC->Resize(m_rect.left, m_rect.top, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top);
}

} 