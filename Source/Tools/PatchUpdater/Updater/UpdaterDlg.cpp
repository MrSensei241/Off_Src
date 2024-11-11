UpdaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Updater.h"
#include "UpdaterDlg.h"
#include "Option.h"
#include "Config.h"
#include "Revision.h"

//#include "xtrap.h"
#include "ShellApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WMU_UPDATE_PART_WORK_PROGRESS		(WM_USER + 1001)
#define WMU_UPDATE_PART_WORK_PROGRESS_BYTE	(WM_USER + 1002)
#define WMU_UPDATE_PART_WORK_PROGRESS_COUNT	(WM_USER + 1003)
#define	WMU_UPDATE_TOTAL_WORK_PROGRESS		(WM_USER + 1004)
#define WMU_UPDATE_WORK_EXPLAIN				(WM_USER + 1005)
#define WMU_UPDATE_BUTTON_TEXT				(WM_USER + 1006)
#define WMU_UPDATE_UPDATER_VERSION			(WM_USER + 1007)
#define WMU_UPDATE_CURRENT_REVISION			(WM_USER + 1008)

LRESULT CUpdaterDlg::OnUpdateButtonText(WPARAM wParam, LPARAM lParam)
{
	const char* text = (const char*)wParam;
	m_flashCtrl.SetVariable("Btn_Up_Text", text);
	m_flashCtrl.SetVariable("Btn_Over_Text", text);
	m_flashCtrl.SetVariable("Btn_Down_Text", text);
	return TRUE;
}

const char* CUpdaterDlg::GetButtonText()
{
	static CString temp;
	temp = m_flashCtrl.GetVariable("Btn_Up_Text");
	return temp;
}

void CUpdaterDlg::SetWorkExplain(const char* explain)
{
	static CString beforeExplain;
	if (beforeExplain != explain) {
		beforeExplain = explain;
		PostMessage(WMU_UPDATE_WORK_EXPLAIN, (WPARAM)((LPCTSTR)beforeExplain));
	}
}

void CUpdaterDlg::SetTotalProgress(int per)
{
	PostMessage(WMU_UPDATE_TOTAL_WORK_PROGRESS, per);
}

void CUpdaterDlg::SetPartWorkProgress(int per)
{
	PostMessage(WMU_UPDATE_PART_WORK_PROGRESS, per);
}

void CUpdaterDlg::SetPartWorkProgress(const UINT iCurrent, const UINT iTotal)
{
	PostMessage(WMU_UPDATE_PART_WORK_PROGRESS_BYTE, iCurrent, iTotal);
}

void CUpdaterDlg::SetPartWorkProgress(const char* str)
{
	PostMessage(WMU_UPDATE_PART_WORK_PROGRESS_COUNT, (WPARAM)((LPCTSTR)str));
}

LRESULT CUpdaterDlg::OnUpdateWorkExplain(WPARAM wParam, LPARAM lParam)
{
	CString temp;
	temp.Format("<invoke name=\"LauncherState\"><arguments><string>%s</string></arguments></invoke>", (const char*)wParam);
	m_flashCtrl.CallFunction(temp);
	return TRUE;
}

LRESULT CUpdaterDlg::OnUpdatePartWorkProgress(WPARAM wParam, LPARAM lParam)
{
	CString temp;
	temp.Format("<invoke name=\"ProgressGauge\"><arguments><number>%d</number></arguments></invoke>", (int)wParam);
	m_flashCtrl.CallFunction(temp);
	return TRUE;
}

LRESULT CUpdaterDlg::OnUpdatePartWorkProgressBYTE(WPARAM wParam, LPARAM lParam)
{
	int iCur = (int)wParam;
	int iMax = (int)lParam;

	CString temp;
	temp.Format("<invoke name=\"progressFileSize\"><arguments><string>%.3fKB/%.3fKB</string></arguments></invoke>", iCur/1000.f, iMax/1000.f);
	m_flashCtrl.CallFunction(temp);

	return TRUE;
}

LRESULT CUpdaterDlg::OnUpdatePartWorkProgressCount(WPARAM wParam, LPARAM lParam)
{
	CString temp;
	temp.Format("<invoke name=\"progressFileSize\"><arguments><string>%s</string></arguments></invoke>", (const char*)wParam);
	m_flashCtrl.CallFunction(temp);

	return TRUE;
}

LRESULT CUpdaterDlg::OnUpdateTotalWorkProgress(WPARAM wParam, LPARAM lParam)
{
	CString temp;
	temp.Format("<invoke name=\"TotalGauge\"><arguments><number>%d</number></arguments></invoke>", (int)wParam);
	m_flashCtrl.CallFunction(temp);

	if((int)wParam == 100) {
		m_flashCtrl.CallFunction("<invoke name=\"StartBtnEnable\"><arguments><number>1</number></arguments></invoke>");
		m_flashCtrl.TStopPlay("disable");
	}

	if((int)wParam < 5) {
		m_flashCtrl.CallFunction("<invoke name=\"StartBtnEnable\"><arguments><number>0</number></arguments></invoke>");
		m_flashCtrl.TStopPlay("enable");
	}

	return TRUE;
}

CUpdaterDlg::CUpdaterDlg(CWnd* pParent)
	: CDialog(CUpdaterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_dragging = FALSE;
	m_iUpdateStep = 0;
	m_iUpdateStepTotal = 1;
	m_bExportPak = 0;
	m_bOptimizeStop = 0;
}

void CUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH, m_flashCtrl);
}

BEGIN_MESSAGE_MAP(CUpdaterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WMU_UPDATE_PART_WORK_PROGRESS, OnUpdatePartWorkProgress)
	ON_MESSAGE(WMU_UPDATE_PART_WORK_PROGRESS_BYTE, OnUpdatePartWorkProgressBYTE)
	ON_MESSAGE(WMU_UPDATE_PART_WORK_PROGRESS_COUNT, OnUpdatePartWorkProgressCount)
	ON_MESSAGE(WMU_UPDATE_TOTAL_WORK_PROGRESS, OnUpdateTotalWorkProgress)
	ON_MESSAGE(WMU_UPDATE_WORK_EXPLAIN, OnUpdateWorkExplain)
	ON_MESSAGE(WMU_UPDATE_BUTTON_TEXT, OnUpdateButtonText)
	ON_MESSAGE(WMU_UPDATE_UPDATER_VERSION, OnUpdateUpdaterVersion)
	ON_MESSAGE(WMU_UPDATE_CURRENT_REVISION, OnUpdateCurrentRevision)
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	SetWindowText("Project R1");

	m_flashCtrl.LoadMovie(0, g_fullPath + "updater.skin.swf");
	
	CString width = m_flashCtrl.TGetProperty("bgtop", 8);
	CString height = m_flashCtrl.TGetProperty("/", 9);
	
	MoveWindow(0, 0, atoi(width), atoi(height));
	m_flashCtrl.MoveWindow(0, 0, atoi(width), atoi(height));

	CString x = m_flashCtrl.TGetProperty("notice", 0);
	CString y = m_flashCtrl.TGetProperty("notice", 1);
	width = m_flashCtrl.TGetProperty("notice", 8);
	height = m_flashCtrl.TGetProperty("notice", 9);

	//m_flashCtrl.CallFunction("<invoke name=\"BtnText\"><arguments><string>실행</string></arguments></invoke>");
	m_flashCtrl.CallFunction("<invoke name=\"TotalGauge\"><arguments><number>0</number></arguments></invoke>");
	m_flashCtrl.CallFunction("<invoke name=\"ProgressGauge\"><arguments><number>0</number></arguments></invoke>");

	if (Option::windowMode) {
		m_flashCtrl.CallFunction("<invoke name=\"WindowMode\"><arguments><number>1</number></arguments></invoke>");
	}

	if (Option::directlyRun) {
		m_flashCtrl.CallFunction("<invoke name=\"AutoRunValue\"><arguments><number>1</number></arguments></invoke>");
	}

	m_flashCtrl.SetVariable("status", "");

	m_flashCtrl.SetVariable("HomeURL_UP", Config::URL::home);
	m_flashCtrl.SetVariable("HomeURL_Over", Config::URL::home);
	m_flashCtrl.SetVariable("HomeURL_Down", Config::URL::home);

	m_noticeDlg.Create(this);
	m_noticeDlg.SetWindowPos(&CWnd::wndTop, atoi(x), atoi(y), atoi(width), atoi(height), SWP_SHOWWINDOW);
	m_noticeDlg.Update();

	m_flashCtrl.TStopPlay("enable");
	m_flashCtrl.TStopPlay("disable");

	m_flashCtrl.TGotoFrame("disable", 0);
	m_flashCtrl.TPlay("disable");

	m_noticeDlg.ShowWindow(TRUE);
	m_flashCtrl.ShowWindow(TRUE);

	if (false == Start()) {
		return FALSE;
	}

	m_iWnuUpPartProg	 = WMU_UPDATE_PART_WORK_PROGRESS;
	m_iWnuUpPartProgByte = WMU_UPDATE_PART_WORK_PROGRESS_BYTE;

	m_bNowMerging = false;
	m_bNowUpdate  = true;	
	m_bOptimize	  = false;
	m_bExchangeFileName  = false;
	m_bRequestedOptimize = false;
	m_bRequestedRepair = false;
	m_bRepair = false;
	m_bPause = false;

	m_iTimeOutCount			= 0;
	m_iConnenctTimeOutCount = 0;

	return TRUE;
}

void CUpdaterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CUpdaterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BEGIN_EVENTSINK_MAP(CUpdaterDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CUpdaterDlg)
	ON_EVENT(CUpdaterDlg, IDC_SHOCKWAVEFLASH, 150, OnFSCommandShockwaveflash, VTS_BSTR VTS_BSTR)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


BOOL CUpdaterDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CDialog::PreCreateWindow(cs);
}

void CUpdaterDlg::PreSubclassWindow() 
{
	CDialog::PreSubclassWindow();
}

void CUpdaterDlg::Execute()
{
	char path[1024] = { 0, };
	sprintf_s(path, "%s%s", g_gameRootPath, "release");

	OSVERSIONINFO osInfo = { 0 };
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	SHELLEXECUTEINFO exInfo = { 0 };
	exInfo.cbSize			= sizeof(SHELLEXECUTEINFO);

	if (osInfo.dwMajorVersion == 6) {
		exInfo.lpVerb = "runas";
	}

	CString param;
	param.Format("-SERVICE -WINDOWMODE %d", Option::windowMode);

	char filePath[1024] = { 0 };
	sprintf_s(filePath, "%s%s", g_gameRootPath, "release/Client_R1.exe");

	exInfo.lpFile = filePath;
	exInfo.lpDirectory = path;
	exInfo.lpParameters = param.GetBuffer();
	exInfo.nShow = SW_NORMAL;

	FATAL_ERROR(TRUE == ShellExecuteEx(&exInfo), THIS_FILE, __LINE__, Config::String::cannotRunLD_exe);

	char* indata = (char *)MapViewOfFile (g_mmap, FILE_MAP_READ, 0, 0, 0);
	
	while((*(DWORD*)indata) == 0)
	{
		static DWORD _time = timeGetTime();
		Sleep(1);

		if((timeGetTime() - _time) > 10000)
			break;
	}
	
	UnmapViewOfFile(indata);
	CloseHandle(g_mmap);
	g_mmap = NULL;
}

void CUpdaterDlg::OnFSCommandShockwaveflash(LPCTSTR command, LPCTSTR args) 
{
	CString type = command;
	if (type == "beginDrag") {
		m_dragging = TRUE;
	} else if (type == "endDrag") {
		m_dragging = FALSE;
	} else if (type == "StartBtn") {
		Execute();
		ExitProcess(0);
	} else if (type == "home") {
		ShellExecute(NULL, "open", Config::URL::home, NULL, "", SW_NORMAL);
	} else if (type == "Repair") {
		m_bRequestedOptimize = true;
	} else if (type == "checkup") {
		m_bRequestedRepair = true;
	} else if (type == "startdirect") {
		Option::directlyRun = !Option::directlyRun;
		if (Option::directlyRun) {
			m_flashCtrl.CallFunction("<invoke name=\"AutoRunValue\"><arguments><number>1</number></arguments></invoke>");
		} else {
			m_flashCtrl.CallFunction("<invoke name=\"AutoRunValue\"><arguments><number>0</number></arguments></invoke>");
		}
	} else if (type == "windowmode") {
		Option::windowMode = !Option::windowMode;
		if (Option::windowMode) {
			m_flashCtrl.CallFunction("<invoke name=\"WindowMode\"><arguments><number>1</number></arguments></invoke>");
		} else {
			m_flashCtrl.CallFunction("<invoke name=\"WindowMode\"><arguments><number>0</number></arguments></invoke>");
		}
	} else if (type == "close") {
		m_bPause = true;

		if(m_bExportPak) {
			AfxMessageBox(Config::String::exportwarning, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);
			m_bPause = false;
			return;
		}

 		if(m_bExchangeFileName) {
			AfxMessageBox(Config::String::changenamewarning, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);
			m_bPause = false;
 			return;
 		}

		if(m_bNowUpdate || m_bRequestedRepair || m_bRepair) {
			if (IDNO == AfxMessageBox(Config::String::exitQuestion, MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST)) {
				m_bPause = false;
				return;
			}
		}
 
 		if(m_bOptimize) {
 			if (IDNO == AfxMessageBox(Config::String::cancelQuestion, MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND | MB_TOPMOST)) {
				m_bPause = false;
 				return;
 			}
			else {
				m_bPause = false;
				m_bOptimizeStop = true;
				return;
			}
 		}

		CloseHandle(g_mmap);
		g_mmap = NULL;
		ExitProcess(0);
	}
}

BOOL CUpdaterDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_MOUSEMOVE && GetAsyncKeyState(VK_LBUTTON)) {
		CPoint point = m_flashCtrl.GetMousePoint();
		TRACE("dragging(%d,%d)\n", point.x, point.y);
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	}

	if(GetAsyncKeyState(VK_RBUTTON))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CUpdaterDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
}

void CUpdaterDlg::OnDestroy()
{
	__super::OnDestroy();

	m_datadownlist.clear();
	m_releasedownlist.clear();
	m_preparedownlist.clear();
	m_reparedownlistD.clear();
}

LRESULT CUpdaterDlg::OnUpdateUpdaterVersion(WPARAM wParam, LPARAM lParam)
{
	m_flashCtrl.SetVariable("updater_version", (const char*)wParam);
	return TRUE;
}

LRESULT CUpdaterDlg::OnUpdateCurrentRevision(WPARAM wParam, LPARAM lParam)
{
	m_flashCtrl.SetVariable("current_revision", (const char*)wParam);
	return TRUE;
}

void CUpdaterDlg::SetUpdaterVersion(const float version)
{
	static CString str;
	str.Format("ver %.1f", version);
	PostMessage(WMU_UPDATE_UPDATER_VERSION, (WPARAM)(LPCTSTR)str);
}

void CUpdaterDlg::SetUpdateCurrentRevision(const int version)
{
	static CString str;
	str.Format("rev %d", version);
	PostMessage(WMU_UPDATE_CURRENT_REVISION, (WPARAM)(LPCTSTR)str);
}





































