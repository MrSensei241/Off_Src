#pragma once

enum EFFECT_CLASS;

class CMainFrame : public CXTPFrameWnd
	 ,CXTPOffice2007FrameHook
{
public:

	virtual				~CMainFrame();

	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL		OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL		PreTranslateMessage(MSG* pMsg);

#ifdef _DEBUG
	virtual void		AssertValid() const;
	virtual void		Dump(CDumpContext& dc) const;
#endif
	
	void				ShowMessageInStatusBar(const char* message);
	
	void				Play();


	DECLARE_MESSAGE_MAP()

	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnClose();

	afx_msg	LRESULT		OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);	
	afx_msg	BOOL		CommandProcess(UINT id);
	afx_msg	void		CommandUpdateProcess(CCmdUI* pCmdUI);


protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	CXTPStatusBar				m_wndStatusBar;
	CXTPOfficeBorder<CEdit>		m_wndProperties;
	CXTPOfficeBorder<CTreeCtrl> m_wndList;
	CXTPOfficeBorder<CStatic>	m_wndManage;

};
