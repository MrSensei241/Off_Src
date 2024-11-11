
// PatcherUpdater.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "PatcherUpdater.h"
#include "PatcherUpdaterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString g_fullPath;

BEGIN_MESSAGE_MAP(CPatcherUpdaterApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CPatcherUpdaterApp::CPatcherUpdaterApp()
{
}

CPatcherUpdaterApp theApp;

BOOL CPatcherUpdaterApp::InitInstance()
{
	// 실행파일의 경로얻기
	char fullPath[8192];
	GetModuleFileName(NULL, fullPath, 8192);

	Config::Init();

	char drive[_MAX_DRIVE];
	char dir[8192];
	char fileName[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(fullPath, drive, dir, fileName, ext);

	g_fullPath.Format("%s%s", drive, dir);
	g_fullPath.Replace('\\', '/');

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	CPatcherUpdaterDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}
