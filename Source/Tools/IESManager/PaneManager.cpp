#include "stdafx.h"

#include "IESManager.h"
#include "MainFrm.h"
#include "PaneManager.h"
#include "PaneLogin.h"
#include "PaneIESList.h"
#include "PaneIESView.h"

// CMainFrame
static CPaneManager s_paneManager;
static CMainFrame*	s_pMainFrame  = NULL;

CPaneManager& GetPaneManager()
{	
	return s_paneManager;
}

// NOTE :  PaneManager
CPaneManager::CPaneManager() 
{		
	m_paneLogin = NULL;
	m_paneIESList = NULL;
	m_pdockpanelogin = NULL;
	m_pdockpaneieslist = NULL;
	m_paneIESView = NULL;
	m_pdockIESVIEW = NULL;
}

CPaneManager::~CPaneManager()
{
	DeleteAllPanes();
}

void CPaneManager::Init(CMainFrame* pMainFrame)
{
	s_pMainFrame = pMainFrame;

	m_pDockingPaneManager = new CXTPDockingPaneManager;

	m_pDockingPaneManager->InstallDockingPanes((CWnd*)s_pMainFrame);
	m_pDockingPaneManager->SetTheme(xtpPaneThemeOffice2007);
	
	CreatePanes();

	// Note : 페인 아이콘 설정
	//int nIDIcons[] = {IDR_PANE_EFFECTLIST, IDR_PANE_PROPERTIES, IDR_PANE_PLAYER, IDR_PANE_PLAYOPTION, IDR_PANE_PLAYACTION};
	//m_pDockingPaneManager->SetIcons(IDB_BITMAP_ICONS, nIDIcons, _countof(nIDIcons), RGB(0, 255, 0));	
}

#define EFFECTEDITOR_LAYOUTNAME "iesmanager_layout_1"

void CPaneManager::Exit()
{
	// Save the current state for docking panes.
	/*CXTPDockingPaneLayout layoutNormal(m_pDockingPaneManager);
	m_pDockingPaneManager->GetLayout(&layoutNormal);
	layoutNormal.Save(_T(EFFECTEDITOR_LAYOUTNAME));*/

	// NOTE : 각 페인 삭제
	m_pDockingPaneManager->DestroyAll();

	SAFE_DELETE(m_pDockingPaneManager);

	DeleteAllPanes();
}



// NOTE : Pane 생성
void CPaneManager::CreatePanes()
{	
	// Load the previous state for docking panes.
	/*CXTPDockingPaneLayout layoutNormal(m_pDockingPaneManager);
	if (layoutNormal.Load(_T(EFFECTEDITOR_LAYOUTNAME)))
	{		
//#ifndef _DEBUG
		m_pDockingPaneManager->SetLayout(&layoutNormal);
//#endif		
	}*/

	m_paneLogin = new CPaneLogin;
	m_paneIESList = new CPaneIESList;
	m_paneIESView = new CPaneIESView;
	

	CXTPDockingPane* paneLogin = m_pDockingPaneManager->CreatePane(IDR_LOGIN_PANE, CRect(0, 0, 300, 1400), xtpPaneDockTop);
	CXTPDockingPane* paneIES = m_pDockingPaneManager->CreatePane(IDR_IES_PANE, CRect(0, 0, 300, 1400), xtpPaneDockTop);
	CXTPDockingPane* pDockPaneDead			= m_pDockingPaneManager->CreatePane(IDR_IES_VIEW, CRect(0, 0, 700, 1400), xtpPaneDockTop);
	pDockPaneDead->Close();


 	m_pDockingPaneManager->SetAlphaDockingContext(TRUE);
 	m_pDockingPaneManager->SetShowDockingContextStickers(TRUE);
 	m_pDockingPaneManager->SetShowContentsWhileDragging(TRUE);
}

void CPaneManager::DeleteAllPanes()
{

}

CXTPDockingPane* CPaneManager::GetActivePane()
{
	return m_pDockingPaneManager->GetActivePane();
}

CWnd* CPaneManager::GetActivePaneView()
{
	CXTPDockingPane* pActivePane = GetActivePane();
	if (pActivePane)
		return pActivePane->GetChild();
	return NULL;
}

void CPaneManager::UpdateSelection()
{

}

LRESULT CPaneManager::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case XTP_DPN_CLOSEPANE:		
		{
			CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
			/*switch (pPane->GetID())	
			{
			case IDR_PANE_COMPONENTLIST:			

				break;
			}*/
			break;
		}

	case XTP_DPN_SHOWWINDOW:
		{
			CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
			if (pPane->IsValid()) 
				return TRUE;

			m_paneMap.Insert(pPane->GetID(), pPane);

			switch (pPane->GetID())	
			{	

			case IDR_LOGIN_PANE:
				if (m_pdockpanelogin == NULL)
				{
					m_pdockpanelogin = pPane;

					if (!::IsWindow(m_paneLogin->m_hWnd))
					{
						m_paneLogin->Create(IDD_LOGIN_DLG, s_pMainFrame);							
					}
				}

				pPane->Attach(m_paneLogin);
				break;

			case IDR_IES_PANE:
				if (m_pdockpaneieslist == NULL)
				{
					m_pdockpaneieslist = pPane;

					if (!::IsWindow(m_paneIESList->m_hWnd))
					{
						m_paneIESList->Create(IDD_IES_DLG, s_pMainFrame);							
					}
				}

				pPane->Attach(m_paneIESList);
				break;

			case IDR_IES_VIEW:
				if (m_pdockIESVIEW == NULL)
				{
					m_pdockIESVIEW = pPane;
					m_paneIESView->SetID(pPane->GetID());				
				}
				pPane->Attach(m_paneIESView->CreatePane(s_pMainFrame));
				break;


			default:
				break;
			}
		}
	}

	return TRUE;
}

BOOL CPaneManager::CommandProcess(UINT id)
{
	return TRUE;
}

void CPaneManager::CommandUpdateProcess(UINT id, CCmdUI *pCmdUI)
{
	

};


CPaneLogin* CPaneManager::GetPaneLogin()
{
	return m_paneLogin;
}

CPaneIESList* CPaneManager::GetPaneIESList()
{
	return m_paneIESList;
}

void CPaneManager::ShowPane(UINT id, BOOL isShow)
{
	CXTPDockingPane* pane = m_paneMap.FindAndGet(id);
	if (pane != NULL)
	{
		if (isShow == TRUE)
			pane->Select();
		else		
			pane->Close();
	}
	else if (isShow == TRUE)
	{
		m_pDockingPaneManager->ShowPane(m_paneMap.Key(id), TRUE);
	}
}
/*
void CPaneManager::ShowLoginPane(BOOL isShow)
{
	if (m_pdockpanelogin != NULL)
	{
		if (isShow == TRUE)
			m_pdockpanelogin->Select();
		else		
			m_pdockpanelogin->Close();
	}
	else if (isShow == TRUE)
	{
		m_pDockingPaneManager->ShowPane(IDR_LOGIN_PANE, TRUE);
	}
}

void CPaneManager::ShowIESPane(BOOL isShow)
{
	if (m_pdockIESVIEW != NULL)
	{
		if (isShow == TRUE)
			m_pdockIESVIEW->Select();
		else		
			m_pdockIESVIEW->Close();
	}
	else if (isShow == TRUE)
	{
		m_pDockingPaneManager->ShowPane(IDR_IES_VIEW, TRUE);
	}
}
*/





