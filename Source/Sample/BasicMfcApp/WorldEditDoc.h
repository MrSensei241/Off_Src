// WorldEditDoc.h : interface of the CWorldEditDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDEDITDOC_H__79368DBE_EB92_44D6_80F8_7A27121FFBE1__INCLUDED_)
#define AFX_WORLDEDITDOC_H__79368DBE_EB92_44D6_80F8_7A27121FFBE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldEditDoc : public CDocument {
protected: // create from serialization only
	CWorldEditDoc();
	DECLARE_DYNCREATE(CWorldEditDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldEditDoc)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorldEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWorldEditDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDEDITDOC_H__79368DBE_EB92_44D6_80F8_7A27121FFBE1__INCLUDED_)
