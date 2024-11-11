// MFC3DTestDoc.h : interface of the CMFC3DTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFC3DTESTDOC_H__6E11092C_EB77_4136_BF0B_7DBAC71CFE9D__INCLUDED_)
#define AFX_MFC3DTESTDOC_H__6E11092C_EB77_4136_BF0B_7DBAC71CFE9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMFC3DTestDoc : public CDocument {
protected: // create from serialization only
					CMFC3DTestDoc();
					DECLARE_DYNCREATE(CMFC3DTestDoc)

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFC3DTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual			~CMFC3DTestDoc();
#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext & dc) const;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CMFC3DTestDoc)
	//}}AFX_MSG
					DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC3DTESTDOC_H__6E11092C_EB77_4136_BF0B_7DBAC71CFE9D__INCLUDED_)
