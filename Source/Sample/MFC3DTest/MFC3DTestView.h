// MFC3DTestView.h : interface of the CMFC3DTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFC3DTESTVIEW_H__0BA759C6_0B82_4487_98A3_AFE8C3EEC072__INCLUDED_)
#define AFX_MFC3DTESTVIEW_H__0BA759C6_0B82_4487_98A3_AFE8C3EEC072__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModelCamera.h"

class CMFC3DTestView : public CView {
protected: // create from serialization only
								CMFC3DTestView();
								DECLARE_DYNCREATE(CMFC3DTestView)

	// Attributes
public:
	CMFC3DTestDoc *				GetDocument();

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFC3DTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

	// Implementation
public:
	BOOL						PreTranslateMessage(MSG * pMsg);
	void						SetVMode();

	virtual						~CMFC3DTestView();
#ifdef _DEBUG
	virtual void				AssertValid() const;
	virtual void				Dump(CDumpContext & dc) const;
#endif

protected:
	//	BOOL		m_bWindowCreated;

	BOOL						m_bLClick;
	BOOL						m_bMClick;
	BOOL						m_bActive;
	CAMERA_MODE					m_nVMode;

	imc3d::IRenderer *			m_renderer;
	imc3d::CAxisGrid			m_axisGrid;

	CModelCamera				m_modelCamera;

	BOOL						m_checkFPS;
	FLOAT						m_fTrsPerSec;	

	BOOL						m_bD3DInit;

public:

	//imc3d
	BOOL						OneTimeSceneInit();
	BOOL						FinalCleanUp();
	BOOL						DeleteD3D(imc3d::IDevice * pDev);
	BOOL						InitD3D(imc3d::IDevice * pDev);
	void						Render();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMFC3DTestView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void	OnMouseMove( UINT nFlags, CPoint point );
	//}}AFX_MSG
								DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MFC3DTestView.cpp
inline CMFC3DTestDoc * CMFC3DTestView::GetDocument()
{
	return (CMFC3DTestDoc *) m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC3DTESTVIEW_H__0BA759C6_0B82_4487_98A3_AFE8C3EEC072__INCLUDED_)
