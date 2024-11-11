// PaneView.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PaneView.h"

// #include "IESManager.h"
// #include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPaneView::CPaneView()
{	
	m_isToolBarCreate	= FALSE;
	m_isTimerUpdate		= FALSE;
	m_iUpdateTime		= 1000;

	m_isUseMainControl	= FALSE;
}

CPaneView::~CPaneView()
{

}

BEGIN_MESSAGE_MAP(CPaneView, CWnd)
	//{{AFX_MSG_MAP(CPaneView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()	
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// NOTE : CPaneView message handlers

int CPaneView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)	return -1;

	OnCreateView();	

	if ( m_isToolBarCreate ) {
		VERIFY(m_wndToolBar.CreateToolBar(WS_VISIBLE | WS_CHILD | 
			CBRS_TOOLTIPS | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, this));
		
	}
	if ( m_isTimerUpdate ) m_updateTimer = SetTimer(m_id, m_iUpdateTime, NULL);
	
	return 0;
}

void CPaneView::OnDestroy()
{
	CWnd::OnDestroy();

	OnDestroyView();
}

void CPaneView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CSize sz(0);

	if (m_isToolBarCreate && m_wndToolBar.GetSafeHwnd())
	{
		sz = m_wndToolBar.CalcDockingLayout(cx, /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);

		m_wndToolBar.MoveWindow(0, 0, cx, sz.cy);
		m_wndToolBar.Invalidate(FALSE);
	}
	
	if (m_Control.pWnd && m_isUseMainControl == TRUE)
	{	
		CWnd* pWnd = m_Control.pWnd;		
		if (!pWnd->GetSafeHwnd()) return;

		RECT rt;
		GetControlRect(rt,m_Control,cx,cy);
		pWnd->MoveWindow(&rt);
	}
}

CWnd* CPaneView::CreatePane(CWnd* pParentWnd)
{
	if (GetSafeHwnd() == 0)
	{
		VERIFY(Create(_T("STATIC"), NULL, WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), pParentWnd, 0));
	}
	return this;
}

void CPaneView::UpdatePane()
{
	OnUpdateView();
}

void CPaneView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_id && m_isTimerUpdate)
	{
		UpdatePane();
	}
}

void CPaneView::GetControlRect(RECT &rect, ControlProperty& prop, int cx, int cy)
{
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	RECT rtWindow;
	GetWindowRect(&rtWindow);

	switch (prop.type)	{		
		case LAYOUT_FIXED:	
			x1 = prop.vSize.x;
			y1 = prop.vSize.y;
			x2 = prop.vSize.z;
			y2 = prop.vSize.w;
			break;
		case LAYOUT_RATE:
			x1 = prop.vSize.x*cx;
			y1 = prop.vSize.y*cy;
			x2 = prop.vSize.z*cx;
			y2 = prop.vSize.w*cy;
			break;
		case LAYOUT_MARGIN:
			x1 = prop.vSize.x;
			y1 = prop.vSize.y;
			x2 = cx - prop.vSize.z;
			y2 = cy - prop.vSize.w;
			break;
	}

	if (x2 < x1) x2 = x1;
	if (y2 < y1) y2 = y1;

	SetRect(&rect,x1,y1,x2,y2);
}

void CPaneView::SetID(int id)	{m_id = id;}
int	 CPaneView::GetID()			{return m_id;}

LRESULT CPaneView::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	return 0;
}