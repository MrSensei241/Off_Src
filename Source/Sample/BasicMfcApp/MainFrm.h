// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__A1FCE5C6_5AE7_4987_851A_52D046C07843__INCLUDED_)
#define AFX_MAINFRM_H__A1FCE5C6_5AE7_4987_851A_52D046C07843__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CMDIFrameWnd {
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	enum	ControlMode {
		MODE_SELECT, 
		MODE_SELECTMOVE, 
		MODE_SELECTROTATE, 
		MODE_SELECTSCALE, 
		MODE_PANVIEW, 
		MODE_ZOOMVIEW, 
		MODE_ROTATEVIEW 
	};
	ControlMode	m_controlMode;
	BOOL		m_viewAxis;

// Operations
public:
	void		SetPaneText(CString t) { m_wndStatusBar.SetPaneText(0, t); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditSelect();
	afx_msg void OnUpdateEditSelect(CCmdUI* pCmdUI);
	afx_msg void OnObjectMove();
	afx_msg void OnUpdateObjectMove(CCmdUI* pCmdUI);
	afx_msg void OnObjectRotate();
	afx_msg void OnUpdateObjectRotate(CCmdUI* pCmdUI);
	afx_msg void OnObjectScale();
	afx_msg void OnUpdateObjectScale(CCmdUI* pCmdUI);
	afx_msg void OnViewportAngle();
	afx_msg void OnUpdateViewportAngle(CCmdUI* pCmdUI);
	afx_msg void OnViewportFov();
	afx_msg void OnUpdateViewportFov(CCmdUI* pCmdUI);
	afx_msg void OnViewportPan();
	afx_msg void OnUpdateViewportPan(CCmdUI* pCmdUI);
	afx_msg void OnViewAxis();
	afx_msg void OnUpdateViewAxis(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

CMainFrame *	GetMainFrameWnd();

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__A1FCE5C6_5AE7_4987_851A_52D046C07843__INCLUDED_)
