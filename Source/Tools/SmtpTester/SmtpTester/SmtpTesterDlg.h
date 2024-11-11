
// SmtpTesterDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"



// CSmtpTesterDlg ��ȭ ����
class CSmtpTesterDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CSmtpTesterDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMTPTESTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

public:
	CToolTipCtrl m_tooltip;

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void InitControls();
	void InitTooltip();
public:
	
	afx_msg void OnBnClickedBtnSend();
	
	CEdit m_ctrlEditLog;
	void SetLog(CString logStr);
	CString m_valueEditLog;
	CComboBox m_ctrlCmbSecurity;
	CEdit m_ctrlEditID;
	CEdit m_ctrlEditPW;
	CEdit m_ctrlEditURL;
	CEdit m_ctrlEditPort;
	CEdit m_ctrlEditSenderMail;
	CEdit m_ctrlEditRecieverMail;
	CEdit m_ctrlEditMailSubject;
	CEdit m_ctrlEditMailBody;
	afx_msg void OnCbnSelchangeCmb1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
