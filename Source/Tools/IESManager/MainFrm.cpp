#include "stdafx.h"



#include "app.h"

#include "PaneView.h"

#include "IESManager.h"
#include "MainFrm.h"

#include "PaneManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static UINT uHideCmds[] =
{
	ID_FILE_PRINT,
	ID_FILE_PRINT_PREVIEW,
};

CMainFrame::CMainFrame()	
{
}

CMainFrame::~CMainFrame()	{}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Initialize the command bars
	if (!InitCommandBars())			
		return -1;

	// Get a pointer to the command bars object.	
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (pCommandBars == NULL)
	{
		TRACE0("Failed to create command bars object.\n");
		return -1;      // fail to create
	}

	pCommandBars->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME);
	pCommandBars->GetToolTipContext()->SetStyle(xtpToolTipOffice);

	// Load the previous state for toolbars and menus.
	LoadCommandBars(_T("CommandBars"));

	// Add the menu bar
	CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(_T("Menu Bar"), IDR_MAINFRAME);
	if (pMenuBar == NULL)
	{
		TRACE0("Failed to create menu bar.\n");
		return -1;      // fail to create
	}

	// Create ToolBar
// 	CXTPRibbonBar* pToolBar = (CXTPRibbonBar*)pCommandBars->Add(_T("Standard"), xtpBarTop);
// 	if (!pToolBar || !pToolBar->LoadToolBar(IDR_MAINFRAME))
// 	{
// 		TRACE0("Failed to create toolbar\n");
// 		return -1;
// 	}

	// Note : 테마 설정
	XTPPaintManager()->SetTheme(xtpThemeOffice2007);
	EnableOffice2007Frame(GetCommandBars());
	XTThemeManager()->SetTheme(xtThemeOffice2007);
	GetCommandBars()->RedrawCommandBars();

	RedrawWindow(0, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);

	
	return 0;
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// Use GetActivePane() to search the pane manager's list for the pane
	// with a view that has input focus.  If one is found, give the view an
	// opportunity to handle the OnCmdMsg first before we pass it to the base
	// class.

	CWnd* pView = GetPaneManager().GetActivePaneView();
	if (pView != NULL && pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE; // handled.
	}

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if ( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;	

	cs.lpszClass = _T("XTPMainFrame");
	CXTPDrawHelpers::RegisterWndClass(AfxGetInstanceHandle(), cs.lpszClass, 
		CS_DBLCLKS, AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;

	cs.x	= 50;
	cs.y	= 50;
	cs.cx	= 800;
	cs.cy	= 840;

	return TRUE;
}


// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const			{CFrameWnd::AssertValid();}
void CMainFrame::Dump(CDumpContext& dc) const	{CFrameWnd::Dump(dc);}
#endif //_DEBUG

// CMainFrame message handlers
void CMainFrame::OnClose()
{
	// Save the current state for toolbars and menus.
	SaveCommandBars(_T("CommandBars"));

	GetPaneManager().Exit();

	CFrameWnd::OnClose();
}

void CMainFrame::ShowMessageInStatusBar(const char* message)
{
	if (message != NULL)
	{
		m_wndStatusBar.SetPaneText(0, message);
		m_wndStatusBar.UpdateData(FALSE);
		m_wndStatusBar.Invalidate();
	}
}

/*
	NOTE : 공용 처리
*/

void CMainFrame::Play()
{
	app::Play();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// NOTE : 키 입력 선처리
	UINT	msg = pMsg->message;
	WPARAM	wParam = pMsg->wParam;

	// NOTE : 스페이스키가 눌린 경우 이펙트 실행
	if (msg == WM_KEYDOWN)
	{
		if (wParam == VK_SPACE)
			Play();

		app::ProcessKeyDownMsg(wParam);
		return TRUE;
	}	

	return __super::PreTranslateMessage(pMsg);
}

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)

	ON_WM_CREATE()
	ON_WM_CLOSE()

	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, CMainFrame::OnDockingPaneNotify)
	ON_COMMAND_EX_RANGE(ID_VIEW_START, ID_VIEW_END, &CMainFrame::CommandProcess)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_START, ID_VIEW_END, &CMainFrame::CommandUpdateProcess)

END_MESSAGE_MAP()

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	return GetPaneManager().OnDockingPaneNotify(wParam, lParam);
}

BOOL CMainFrame::CommandProcess(UINT id)
{
	return GetPaneManager().CommandProcess(id);
}

void CMainFrame::CommandUpdateProcess(CCmdUI* pCmdUI)
{
	GetPaneManager().CommandUpdateProcess(pCmdUI->m_nID, pCmdUI);
}



