
// SmtpTesterDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "SmtpTester.h"
#include "SmtpTesterDlg.h"
#include "afxdialogex.h"

#include "CurlSmtp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSmtpTesterDlg ��ȭ ����



CSmtpTesterDlg::CSmtpTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SMTPTESTER_DIALOG, pParent)
	, m_valueEditLog(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSmtpTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_ctrlEditLog);
	DDX_Text(pDX, IDC_EDIT3, m_valueEditLog);
	DDX_Control(pDX, IDC_CMB_1, m_ctrlCmbSecurity);
	DDX_Control(pDX, IDC_EDIT_3, m_ctrlEditID);
	DDX_Control(pDX, IDC_EDIT_4, m_ctrlEditPW);
	DDX_Control(pDX, IDC_EDIT_1, m_ctrlEditURL);
	DDX_Control(pDX, IDC_EDIT_2, m_ctrlEditPort);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlEditSenderMail);
	DDX_Control(pDX, IDC_EDIT2, m_ctrlEditRecieverMail);
	DDX_Control(pDX, IDC_EDIT4, m_ctrlEditMailSubject);
	DDX_Control(pDX, IDC_EDIT5, m_ctrlEditMailBody);
}

BEGIN_MESSAGE_MAP(CSmtpTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BTN_SEND, &CSmtpTesterDlg::OnBnClickedBtnSend)
	ON_CBN_SELCHANGE(IDC_CMB_1, &CSmtpTesterDlg::OnCbnSelchangeCmb1)
END_MESSAGE_MAP()


// CSmtpTesterDlg �޽��� ó����

BOOL CSmtpTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	InitControls();
	InitTooltip();
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CSmtpTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CSmtpTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CSmtpTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include <string>


class callBackLogger_tester : public imcSMTP::CurlSmtpLogger
{
private:
	CSmtpTesterDlg* m_ptr;

public:
	callBackLogger_tester(CSmtpTesterDlg* ptr)
	{
		m_ptr = ptr;
	}
	virtual void Setlog(const std::string& logStr) override
	{
		if (m_ptr)
		{
			m_ptr->SetLog(logStr.c_str());
		}
	}

};

void CSmtpTesterDlg::InitControls()
{
	// ���ȼ��� �޺��ڽ�
	m_ctrlCmbSecurity.AddString("None");
	m_ctrlCmbSecurity.AddString("SSL");
	m_ctrlCmbSecurity.AddString("TLS");
	m_ctrlCmbSecurity.SetCurSel(0);
	OnCbnSelchangeCmb1();
	// SMTP Def UrL
	m_ctrlEditURL.SetWindowTextA("mail.imcgames.net");
	

}

void CSmtpTesterDlg::InitTooltip()
{
	m_tooltip.Create(this);
	m_tooltip.AddTool(GetDlgItem(IDC_CMB_1), "SMTP ���� ��带 �����մϴ�.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_1), "SMTP ���� URL�� �Է��մϴ�. ex)smtp.google.com");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_3), "���� ����� ��� SMTP���� ID�� �Է��մϴ�.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_4), "���� ����� ��� SMTP���� PW�� �Է��մϴ�.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_2), "SMTP�� ��� ��Ʈ�� �Է��մϴ�. TLS����� ��쿡�� �Է��� �����մϴ�.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT1), "������ ��� ���� �ּҸ� �Է��մϴ�.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT2), "�޴� ��� ���� �ּҸ� �Է��մϴ�. ���پ� �Է��ϸ� ������� �Է��� �����մϴ�.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT4), "���۵Ǵ� ���� ������ �Է��մϴ�.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT5), "���۵Ǵ� ������ ������ �Է��մϴ�.");

}

void CSmtpTesterDlg::OnBnClickedBtnSend()
{
	callBackLogger_tester logger(this);
	imcSMTP::CurlSmtp smtp(&logger);
	
	CString getData;

	//security
	m_ctrlCmbSecurity.GetWindowTextA(getData);
	if (getData == "TLS")
	{
		smtp.SetSecureType(imcSMTP::TLS);
	}
	else if (getData == "SSL")
	{
		smtp.SetSecureType(imcSMTP::SSL);
	}
	else
	{
		smtp.SetSecureType(imcSMTP::NONE);
	}
	//port
	m_ctrlEditPort.GetWindowTextA(getData);
	std::string port((LPCTSTR)getData);
	smtp.SetPort(std::atoi(port.c_str()));

	//URL
	m_ctrlEditURL.GetWindowTextA(getData);
	smtp.SetSmtpUrl(std::string((LPCTSTR)getData));

	// subject
	m_ctrlEditMailSubject.GetWindowTextA(getData);
	smtp.SetSubject(std::string((LPCTSTR)getData));

	// body
	int len, i, nLineCount = m_ctrlEditMailBody.GetLineCount();
	for (i = 0; i < nLineCount; ++i)
	{
		len = m_ctrlEditMailBody.LineLength(m_ctrlEditMailBody.LineIndex(i));
		m_ctrlEditMailBody.GetLine(i, getData.GetBuffer(len), len);
		getData.ReleaseBuffer(len);
		smtp.AddMessage(std::string((LPCTSTR)getData));
	}

	//from 
	m_ctrlEditSenderMail.GetWindowTextA(getData);
	smtp.SetFromMailAddress(std::string((LPCTSTR)getData));

	//to
	nLineCount = m_ctrlEditRecieverMail.GetLineCount();
	for (i = 0; i < nLineCount; ++i)
	{
		len = m_ctrlEditRecieverMail.LineLength(m_ctrlEditRecieverMail.LineIndex(i));
		m_ctrlEditRecieverMail.GetLine(i, getData.GetBuffer(len), len);
		getData.ReleaseBuffer(len);
		smtp.AddToMailAddress(std::string((LPCTSTR)getData));
	}

	CString id, pw;
	m_ctrlEditID.GetWindowTextA(id);
	m_ctrlEditPW.GetWindowTextA(pw);
	smtp.Send(std::string((LPCTSTR)id), std::string((LPCTSTR)pw));
}


void CSmtpTesterDlg::SetLog(CString logStr)
{
	m_valueEditLog = logStr + "\r\n";
	m_ctrlEditLog.SetSel(-2, -1);
	m_ctrlEditLog.ReplaceSel(m_valueEditLog);
}


void CSmtpTesterDlg::OnCbnSelchangeCmb1()
{
	CString getData;
	m_ctrlCmbSecurity.GetLBText(m_ctrlCmbSecurity.GetCurSel(), getData);
	if (getData == "TLS")
	{
		m_ctrlEditID.EnableWindow(TRUE);
		m_ctrlEditPW.EnableWindow(TRUE);
		m_ctrlEditPort.EnableWindow(TRUE);
		m_ctrlEditPort.SetWindowTextA("587");
	}
	else if (getData == "SSL")
	{
		m_ctrlEditID.EnableWindow(TRUE);
		m_ctrlEditPW.EnableWindow(TRUE);
		m_ctrlEditPort.EnableWindow(FALSE);
		m_ctrlEditPort.SetWindowTextA("465");
	}
	else
	{
		m_ctrlEditID.EnableWindow(FALSE);
		m_ctrlEditPW.EnableWindow(FALSE);
		m_ctrlEditPort.EnableWindow(FALSE);
		m_ctrlEditPort.SetWindowTextA("25");
	}
}


BOOL CSmtpTesterDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_tooltip.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}
