#include "stdafx.h"
#include <tlhelp32.h>
#include "Updater.h"
#include "UpdaterDlg.h"
#include "Config.h"
#include "Option.h"
#include ".\updater.h"
#include "FileFinder.h"
#include "ShockwaveFlash.h"

#define NEED_FLASH_VERSION 0x70000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CUpdaterApp::CUpdaterApp()
{
	m_mutex = NULL;
	g_mmap = NULL;
}

bool g_manualy = false;
CUpdaterApp theApp;
CString g_fullPath;
CString g_gameRootPath;
CString g_datafolder;
CString g_patchfolder;
CString g_datainfofolder;
HANDLE g_mmap;
HANDLE g_hFile;

void CUpdaterApp::EnsureFlashPlayer()
{
	FATAL_ERROR(S_OK==CoInitialize(NULL), THIS_FILE, __LINE__, Config::String::failedFlashPlayerInit);

	CLSID	clsID	= {0xd27cdb6e, 0xae6d, 0x11cf, {0x96, 0xb8, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0}};
	IID		iID		= {0xD27CDB6C, 0xAE6D, 0x11CF, {0x96, 0xB8, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0}};

	LPVOID flash;
	FATAL_ERROR(S_OK==CoCreateInstance(clsID, NULL, CLSCTX_INPROC_SERVER, iID, (void**)&flash), THIS_FILE, __LINE__, Config::String::failedFlashPlayerInit);

	if (NULL != flash) {
		CShockwaveFlash player((LPDISPATCH)flash);
		long ver = player.FlashVersion();
		player.ReleaseDispatch();
		if (NEED_FLASH_VERSION < ver)
			return;
	}

	HMODULE module = LoadLibrary(g_fullPath + "flash.ocx");
	FATAL_ERROR(NULL!=module, THIS_FILE, __LINE__, Config::String::cannotOpenFlashOCX);

	void (__stdcall *entryPoint)(void);
	(FARPROC&)entryPoint = GetProcAddress(module, _T("DllRegisterServer"));
	FATAL_ERROR(NULL!=entryPoint, THIS_FILE, __LINE__, Config::String::failedFlashPlayerInit);
	(*entryPoint)();

	FreeLibrary(module);
	CoUninitialize();
}

void CUpdaterApp::CheckBlockProcess()
{
	char tempStr[MAX_PATH] = { 0 };

	HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == snapshot)
		return;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	std::vector<CString> vCurProcList;

	if(FALSE == ::Process32First(snapshot, &pe32)) {
		::CloseHandle(snapshot);
		return;
	}

	do {
		strcpy_s(tempStr, pe32.szExeFile);
		_tcslwr_s(tempStr);
		vCurProcList.push_back(tempStr);
	} while(::Process32Next(snapshot, &pe32));

	::CloseHandle(snapshot);	

	CString strProcessName = _T("");

	for(UINT i = 0; i < vCurProcList.size(); ++i)
	{
		strProcessName = vCurProcList[i];

		//msnshell 인 경우 경고 후 종료 시킴.
		if(strProcessName.CompareNoCase(Config::String::block_msnshell) == 0)
		{
			if(IDOK == AfxMessageBox(Config::String::msnshellNotice, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST))
			{
				if(!vCurProcList.empty())
					vCurProcList.clear();

				ExitProcess(0);
			}	
		}
	}

	if(!vCurProcList.empty())
		vCurProcList.clear();
}

class CUpdaterCommandLineInfo : public CCommandLineInfo {
public:
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
	{
		if (0 == _stricmp(pszParam, "manualy")) {
			g_manualy = true;
		}

		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
	}
};

BOOL CUpdaterApp::InitInstance()
{
	char fullPath[8192];
	GetModuleFileName(NULL, fullPath, 8192);

	char drive[_MAX_DRIVE];
	char dir[8192];
	char fileName[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(fullPath, drive, dir, fileName, ext);

	g_fullPath.Format("%s%s", drive, dir);
	g_fullPath.Replace('\\', '/');

	g_gameRootPath = g_fullPath;

	SetCurrentDirectory(g_fullPath);

	g_datafolder = g_gameRootPath + "data/";
	g_patchfolder = g_gameRootPath + "patch/";
	g_datainfofolder = g_gameRootPath + "data/info/";

	CUpdaterCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	Config::Init();
	Option::Init();

	//단일 실행을 위해 뮤택스 얻기
	CString name("geUpdater$%&%%^@&^*($#");

	CString strTemp111 = Config::String::duplicatedRunImpossible;

	m_mutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);
	FATAL_ERROR(NULL==m_mutex, THIS_FILE, __LINE__, Config::String::duplicatedRunImpossible);

	m_mutex = ::CreateMutex(0, 0, name);
	FATAL_ERROR(NULL!=m_mutex, THIS_FILE, __LINE__, Config::String::duplicatedRunImpossible);

	CString MemoryMapName = "^&(%($$#^@@%$^!Project_R1!@$%^&!#*()#$%^";

#ifdef _DEBUG
	CreateDirectory(g_datainfofolder, NULL);
	g_hFile = CreateFile(g_datainfofolder + "FileInfo.txt", 
		GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else
	g_hFile = (HANDLE)0xFFFFFFFF;
#endif

	g_mmap = CreateFileMapping(g_hFile, NULL, PAGE_READWRITE, 0, 1024*1024*8, MemoryMapName);
	CloseHandle(g_hFile);

	if (g_mmap == NULL)
		FATAL_ERROR(false, THIS_FILE, __LINE__, Config::String::duplicatedRunImpossible);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		FATAL_ERROR(false, THIS_FILE, __LINE__, Config::String::duplicatedRunImpossible);

	//플래쉬 플레이어 설정 
	EnsureFlashPlayer();

	AfxEnableControlContainer();

	CheckBlockProcess();

	if (true == g_manualy)
		AfxMessageBox("Manual Update !!!", MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST);

	CUpdaterDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

int CUpdaterApp::ExitInstance()
{
	if(NULL != g_mmap) {
		CloseHandle(g_mmap);
		g_mmap = NULL;
	}

	if (NULL != m_mutex) {
		CloseHandle(m_mutex);
		m_mutex = NULL;
	}

	return CWinApp::ExitInstance();
}
