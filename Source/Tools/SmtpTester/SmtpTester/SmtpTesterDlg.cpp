
// SmtpTesterDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "SmtpTester.h"
#include "SmtpTesterDlg.h"
#include "afxdialogex.h"

#include "CurlSmtp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CSmtpTesterDlg 대화 상자



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


// CSmtpTesterDlg 메시지 처리기

BOOL CSmtpTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	InitControls();
	InitTooltip();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSmtpTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
	// 보안설정 콤보박스
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
	m_tooltip.AddTool(GetDlgItem(IDC_CMB_1), "SMTP 보안 모드를 설정합니다.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_1), "SMTP 서버 URL을 입력합니다. ex)smtp.google.com");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_3), "보안 모드일 경우 SMTP접속 ID를 입력합니다.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_4), "보안 모드일 경우 SMTP접속 PW를 입력합니다.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_2), "SMTP의 사용 포트를 입력합니다. TLS모드일 경우에만 입력이 가능합니다.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT1), "보내는 사람 메일 주소를 입력합니다.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT2), "받는 사람 메일 주소를 입력합니다. 한줄씩 입력하면 여러사람 입력이 가능합니다.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT4), "전송되는 메일 제목을 입력합니다.");
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT5), "전송되는 메일의 내용을 입력합니다.");

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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_tooltip.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}
