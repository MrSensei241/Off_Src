#pragma once


// CAboutDlg dialog used for App About

class CPaneLogin : public CDialog
{
public:
	CPaneLogin();

	// Dialog Data
	enum { IDD = IDD_LOGIN_DLG };
public:
	void	UpdateServerList();
	int		GetServerIndex();
	virtual BOOL			OnInitDialog();
	imc::CStringID	GetLoginID();
	imc::CStringID	GetLoginServerName();


protected:
	CEdit	m_editID;
	CEdit	m_editPasswd;
	CComboBox	m_serverList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

