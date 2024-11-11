#pragma once


class CPaneLogin;
class CPaneIESList;
class CMainFrame;
class CPaneIESView;

enum TREE_TYPE
{
	TREE_IDSPACE,
	TREE_CLASSNAME,
};

class CPaneManager
{
public:

	CPaneManager();
	virtual				~CPaneManager();

	void				Init(CMainFrame* pMainFrame);
	void				Exit();

	void				UpdateSelection();
	LRESULT				OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	BOOL				CommandProcess(UINT id);
	CWnd*				GetActivePaneView();
	CXTPDockingPane*	GetActivePane();
	void				CommandUpdateProcess(UINT id, CCmdUI *pCmdUI);
	
	CPaneLogin*			GetPaneLogin();
	CPaneIESList*		GetPaneIESList();

	void				ShowPane(UINT id, BOOL isShow);
	/*void				ShowLoginPane(BOOL isShow);
	void				ShowIESPane(BOOL isShow);
	void				ShowIESViewPane(BOOL isShow);*/

protected:

	void				CreatePanes();
	void				DeleteAllPanes();
	
private:

	CXTPDockingPaneManager*	m_pDockingPaneManager;

	CXTPDockingPane*	m_pdockpanelogin;
	CPaneLogin*			m_paneLogin;
	CXTPDockingPane*	m_pdockpaneieslist;
	CPaneIESList*		m_paneIESList;
	CPaneIESView*		m_paneIESView;
	CXTPDockingPane*	m_pdockIESVIEW;

	imc::CMap<int, CXTPDockingPane*>	m_paneMap;

			

};

CPaneManager& GetPaneManager();