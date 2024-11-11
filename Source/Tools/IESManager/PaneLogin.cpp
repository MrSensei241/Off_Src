#include "StdAfx.h"
#include "PaneLogin.h"
#include "GeSession.h"
#include "toolnet.h"
#include "toolutil.h"

CPaneLogin::CPaneLogin() : CDialog(CPaneLogin::IDD)
{
}

BOOL CPaneLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateServerList();

	std::string path = util::GetConfigIniPath();
	char id[256];
	GetPrivateProfileString("Login", "ID", "", id, 256, path.c_str());
	char server[256];
	GetPrivateProfileString("Login", "Server", "", server, 256, path.c_str());
	int str = m_serverList.FindString(0, server);
	m_serverList.SetCurSel(str);
	m_editID.SetWindowText(id);
	
	return TRUE;
}


int	 CPaneLogin::GetServerIndex()
{
	return m_serverList.GetCurSel();
}

void CPaneLogin::UpdateServerList()
{
	m_serverList.Clear();
	int cnt = Session().GetGlobalServerCount();
	for (int i = 0 ; i < cnt ; i ++)
	{
		GLOBAL_SERVER_INFO * info = Session().GetGlobalServerInfo(i);
		m_serverList.AddString(info->serverName);
	}
		
	m_serverList.SetCurSel(0);
	m_serverList.UpdateData(TRUE);
	
}

void CPaneLogin::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LOGIN_ID, m_editID);
	DDX_Control(pDX, IDC_LOGIN_PASS, m_editPasswd);
	DDX_Control(pDX, IDC_SERVERLIST, m_serverList);
	
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPaneLogin, CDialog)


	ON_BN_CLICKED(IDOK, &CPaneLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPaneLogin::OnBnClickedCancel)
END_MESSAGE_MAP()

imc::CStringID	CPaneLogin::GetLoginID()
{
	CString id;
	m_editID.GetWindowText(id);
	const char * uid = LPCTSTR(id);
	return uid;
}

imc::CStringID	CPaneLogin::GetLoginServerName()
{
	int index = m_serverList.GetCurSel();
	if (index >= Session().GetGlobalServerCount())
		return StrID::None;

	GLOBAL_SERVER_INFO* info = Session().GetGlobalServerInfo(index);
	if (info == NULL)
		return StrID::None;

	return info->serverName;
}

void CPaneLogin::OnBnClickedOk()
{
	CString id;
	m_editID.GetWindowText(id);
	const char * uid = LPCTSTR(id);

	CString pwd;
	m_editPasswd.GetWindowText(pwd);
	const char * upwd = LPCTSTR(pwd);
	
	

	int index = m_serverList.GetCurSel();
	if (index >= Session().GetGlobalServerCount())
		return;

	GLOBAL_SERVER_INFO* info = Session().GetGlobalServerInfo(index);
	if (info == NULL)
		return;

	BOOL connected = net::ReqLogin(uid, upwd, info->ip, info->port);
	util::WriteLastLoginInfo();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//OnOK();
}

void CPaneLogin::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//OnCancel();
}
