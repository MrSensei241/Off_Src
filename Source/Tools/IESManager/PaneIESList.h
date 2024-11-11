#pragma once


// CAboutDlg dialog used for App About
struct SORT_PARAMS
{
	HWND handle;
	int nCol;
	BOOL isAcend;
};

class CPaneIESList : public CDialog
{
public:
	CPaneIESList();

	// Dialog Data
	enum { IDD = IDD_IES_DLG };
public:
	virtual BOOL			OnInitDialog();

	void		UpdateAll();
	void		UpdateAllHistory();
	void		UpdateCategory();
	void		UpdateProperty();
	void		UpdateCurrentLog();

protected:
	void		InitListHead();
	void		InitCheckBoxes();

	void		UpdateSelectedCls(imc::CStringID idSpace, int classID);
	void		UpdateAllProperty();
	void		UpdateCategoryProperty(imc::CStringID curCategory);
	void		GetSelectedRevision(INT_LIST* list);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTreeCtrl	m_tree;
	CListCtrl	m_allHistory;
	CListCtrl	m_curCls;
	CListCtrl	m_curLog;
	CComboBox	m_category;
	CStatic		m_readName;
	
	CStatic		m_editingProp;
	CEdit		m_changeProp;
	CEdit		m_editLogInput;
	CButton		m_db;
	CButton		m_rollback;
	CButton		m_noapply;

	imcIES::IClass*	m_class;
	imc::CStringID	m_idSpace;
	imc::CStringID	m_selectedPropName;

	BOOL		m_useSort;
	BOOL		m_isAcsend;

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCommit();
	afx_msg void OnDelete();
	afx_msg void OnSaveXML();
	afx_msg void OnNMClickIdspaces(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedIdspaces(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangePropcategory();
	afx_msg void OnHdnItemchangingProperties(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedProperties(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawAllhistory(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickAllhistory(NMHDR *pNMHDR, LRESULT *pResult);

	static int CALLBACK SortFunc(LPARAM lParam1 , LPARAM lParam2 , LPARAM lParamSort);
};

