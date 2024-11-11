// EffectEditorDoc.h : interface of the CEffectEditorDoc class
//


#pragma once


class CEffectEditorDoc : public CDocument
{
protected: // create from serialization only
	CEffectEditorDoc();
	DECLARE_DYNCREATE(CEffectEditorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CEffectEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	
	CString		m_fileFullPath;

	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	
	/*
	afx_msg BOOL OnSaveXml();	
	afx_msg BOOL OnSaveBin();
	afx_msg BOOL OnOpenBin();
	
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileExporttobin();
	afx_msg void OnFileImporttobin();
//	afx_msg void OnOpenDocument();
*/
	afx_msg void OnUpdateEffectReload(CCmdUI *pCmdUI);
};
