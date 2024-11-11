
// SmtpTesterDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"



// CSmtpTesterDlg 대화 상자
class CSmtpTesterDlg : public CDialogEx
{
// 생성입니다.
public:
	CSmtpTesterDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMTPTESTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CToolTipCtrl m_tooltip;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
