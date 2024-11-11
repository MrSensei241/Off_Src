// EffectEditorView.h : interface of the CEffectEditorView class
//


#pragma once

class CEffectEditorDoc;


class CEffectEditorView : public CView
{
protected: // create from serialization only
	CEffectEditorView();
	DECLARE_DYNCREATE(CEffectEditorView)

// Attributes
public:
	CEffectEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CEffectEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:

	virtual void OnInitialUpdate();
	virtual BOOL OnEraseBkgnd(CDC* pDC);

	CPoint		m_oldCursorPos;
	CPoint		m_capturePos;
	
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);


};

#ifndef _DEBUG  // debug version in EffectEditorView.cpp
inline CEffectEditorDoc* CEffectEditorView::GetDocument() const
   { return reinterpret_cast<CEffectEditorDoc*>(m_pDocument); }
#endif
