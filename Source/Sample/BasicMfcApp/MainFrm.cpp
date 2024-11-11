// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Mfc3dTest.h"

#include "MainFrm.h"
#include "WorldEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_SELECT, OnEditSelect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT, OnUpdateEditSelect)
	ON_COMMAND(ID_OBJECT_MOVE, OnObjectMove)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVE, OnUpdateObjectMove)
	ON_COMMAND(ID_OBJECT_ROTATE, OnObjectRotate)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_ROTATE, OnUpdateObjectRotate)
	ON_COMMAND(ID_OBJECT_SCALE, OnObjectScale)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_SCALE, OnUpdateObjectScale)
	ON_COMMAND(ID_VIEWPORT_ANGLE, OnViewportAngle)
	ON_UPDATE_COMMAND_UI(ID_VIEWPORT_ANGLE, OnUpdateViewportAngle)
	ON_COMMAND(ID_VIEWPORT_FOV, OnViewportFov)
	ON_UPDATE_COMMAND_UI(ID_VIEWPORT_FOV, OnUpdateViewportFov)
	ON_COMMAND(ID_VIEWPORT_PAN, OnViewportPan)
	ON_UPDATE_COMMAND_UI(ID_VIEWPORT_PAN, OnUpdateViewportPan)
	ON_COMMAND(ID_VIEW_AXIS, OnViewAxis)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AXIS, OnUpdateViewAxis)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_controlMode = MODE_SELECTMOVE;
	m_viewAxis = TRUE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_BASIC))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnEditSelect() 
{
	m_controlMode = MODE_SELECT;
}

void CMainFrame::OnUpdateEditSelect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_controlMode == MODE_SELECT);
}

void CMainFrame::OnObjectMove() 
{
	m_controlMode = MODE_SELECTMOVE;
}

void CMainFrame::OnUpdateObjectMove(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_controlMode == MODE_SELECTMOVE);
}

void CMainFrame::OnObjectRotate() 
{
	m_controlMode = MODE_SELECTROTATE;
}

void CMainFrame::OnUpdateObjectRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_controlMode == MODE_SELECTROTATE);
}

void CMainFrame::OnObjectScale() 
{
	m_controlMode = MODE_SELECTSCALE;
}

void CMainFrame::OnUpdateObjectScale(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_controlMode == MODE_SELECTSCALE);
}

void CMainFrame::OnViewportAngle() 
{
	m_controlMode = MODE_ROTATEVIEW;
}

void CMainFrame::OnUpdateViewportAngle(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_controlMode == MODE_ROTATEVIEW);
}

void CMainFrame::OnViewportFov() 
{
	m_controlMode = MODE_ZOOMVIEW;
}

void CMainFrame::OnUpdateViewportFov(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_controlMode == MODE_ZOOMVIEW);
}

void CMainFrame::OnViewportPan() 
{
	m_controlMode = MODE_PANVIEW;
}

void CMainFrame::OnUpdateViewportPan(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_controlMode == MODE_PANVIEW);
}

CMainFrame *	GetMainFrameWnd()
{
	return	(CMainFrame *)(((CWorldEditApp *)AfxGetApp())->m_pMainWnd);
}

void CMainFrame::OnViewAxis() 
{
	m_viewAxis = 1-m_viewAxis;
}

void CMainFrame::OnUpdateViewAxis(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_viewAxis);
}
