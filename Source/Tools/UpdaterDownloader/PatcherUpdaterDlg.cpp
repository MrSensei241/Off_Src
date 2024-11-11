#include "stdafx.h"
#include "PatcherUpdater.h"
#include "PatcherUpdaterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int MERGE_BUFFER_SIZE		   = 1024*1024; //1메가
const int TIMEOUT_TIME			   = 5000;
const int TIMEOUT_REPEAT_MAX_COUNT = 5;

#define UPDATER_NAME				"tos.exe"
#define UPDATER_DIRECTORY			"updater/"
#define UPDATER_UPDATE_LIST			"updater.list.txt"

#define PREFIX_TEMP				   ".temp"

class CAboutDlg : public CDialog
{
public:
	enum { IDD = IDD_ABOUTBOX };

	CAboutDlg() : CDialog(CAboutDlg::IDD) {}

protected:
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
	}

protected:
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CPatcherUpdaterDlg::CPatcherUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatcherUpdaterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_iUpdateStep = 0;
	m_iTimeOutCount = 0;
	m_iUpdateStepTotal = 1;
	m_iConnenctTimeOutCount = 0;
}

void CPatcherUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, WMU_UPDATE_TOTAL_WORK_PROGRESS, m_TotalProg);
	DDX_Control(pDX, WMU_UPDATE_PART_WORK_PROGRESS, m_PartProg);
}

BEGIN_MESSAGE_MAP(CPatcherUpdaterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CPatcherUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	Start();

	return TRUE;
}

void CPatcherUpdaterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CPatcherUpdaterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CPatcherUpdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPatcherUpdaterDlg::Run()
{
	m_TotalProg.SetRange(0,100);
	m_PartProg.SetRange(0,100);

	CInternetSession session;

	INTERNET_PORT port;
	DWORD serviceType;
	CString svrName;
	AfxParseURL(Config::URL::revisions, serviceType, svrName, m_path, port);

	m_connection = session.GetHttpConnection(svrName, 0, port, NULL, NULL);

	m_connection->SetOption( INTERNET_OPTION_CONNECT_TIMEOUT, TIMEOUT_TIME );
	m_connection->SetOption( INTERNET_OPTION_SEND_TIMEOUT,	  TIMEOUT_TIME );
	m_connection->SetOption( INTERNET_OPTION_RECEIVE_TIMEOUT, TIMEOUT_TIME );

	if (NULL == m_connection) 
	{
		session.Close();
		return;
	}

	//받을 파일 목록 받음
	CString patcherversion;
	patcherversion.Format("%s%s", UPDATER_DIRECTORY, UPDATER_UPDATE_LIST);
	if (false == DownloadHistory(patcherversion, UPDATER_UPDATE_LIST)) 
	{
		m_connection->Close();
		session.Close();
	
		CString strComment;
		strComment.Format("%s", UPDATER_UPDATE_LIST);
		FATAL_ERROR(false, THIS_FILE, __LINE__, strComment);
	}

	//받을 파일 목록 생성 
	if (false == AnalysisHistory(UPDATER_UPDATE_LIST, m_history))
	{
		m_connection->Close();
		session.Close();

		CString strComment;
		strComment.Format("%s",UPDATER_UPDATE_LIST);
		FATAL_ERROR(false, THIS_FILE, __LINE__, strComment);
	}
	
	m_iUpdateStepTotal = (int)m_history.size();

	//업데이트 전체 진행
	UpdateProgress(session);
	
	CommitUpdater();

	m_TotalProg.SetPos(100);
	m_PartProg.SetPos(100);

	DeleteFile(g_fullPath + UPDATER_UPDATE_LIST);

	std::string _path = g_fullPath;
	char rootpath[8192] = {0,};
	memcpy(rootpath, _path.data(), _path.rfind("release/"));
	_path = rootpath;
	_path += "patch";

	DeleteAllFile(_path.data(), ".ipf");

	Execute();
	ExitProcess(0);
}

void CPatcherUpdaterDlg::DeleteAllFile(const char* szDir, const char* key)
{
	CString strName; 
	strName.Format( "%s\\*%s", szDir, key ); 

	CFileFind ff; 
	BOOL bFind = ff.FindFile( strName ); 

	while( bFind ) 
	{ 
		bFind = ff.FindNextFile(); 

		if( ff.IsDots() == TRUE || ff.IsDirectory() == TRUE ) continue; 

		DeleteFile( ff.GetFilePath() ); 
	} 

	ff.Close(); 
}


bool CPatcherUpdaterDlg::DownloadHistory(const CString& rFtpFileName, const CString& rLocalFileName)
{
	m_PartProg.SetPos(0);

	CDownloader downloader;

 	if(CDownloader::STATE::SUCCESS != downloader.Init(m_connection, m_path + rFtpFileName, g_fullPath + rLocalFileName, false)) 
	{
		downloader.Exit();
		return false;
	}

	bool bDownLoaded = false;
	DOWN_LOAD_TYPE DownLoadProcessType = FileDownloadProcess(downloader, bDownLoaded);

	if(E_DN_TYPE_OK == DownLoadProcessType)
		return true;

	return false;
}

CPatcherUpdaterDlg::DOWN_LOAD_TYPE CPatcherUpdaterDlg::FileDownloadProcess(CDownloader& rDownLoader, bool& rbDownLoaded)
{
	while (TRUE) 
	{
		Sleep(2);

		if(true == m_stop) 
		{
			rDownLoader.Exit();
			return E_DN_TYPE_FAIL;
		}

		CDownloader::STATE state = rDownLoader.Do();

		m_PartProg.SetPos(((UINT)rDownLoader.GetReceivedByte()/(UINT)rDownLoader.GetTotalByte())*100);

		if(CDownloader::DOING == state) 
		{
			rbDownLoaded = true;
			continue;
		}
		rDownLoader.Exit();

		switch (state) 
		{
		case CDownloader::FAIL:
			return E_DN_TYPE_FAIL;

		case CDownloader::REPEAT:
			if(m_iTimeOutCount < TIMEOUT_REPEAT_MAX_COUNT)
			{
				++m_iTimeOutCount;

				return E_DN_TYPE_REPEAT;
			}
			return E_DN_TYPE_FAIL;

		case CDownloader::SUCCESS:
			m_iTimeOutCount = 0;
			Sleep(100);
			break;
		}

		break;
	}

	return E_DN_TYPE_OK;
}

bool CPatcherUpdaterDlg::AnalysisHistory(const CString& rRevisionFileName, std::vector<UPDATE_HISTORY>& vecHistory)
{
	CStdioFile file;

	if(FALSE == file.Open(g_fullPath + rRevisionFileName, CFile::modeRead)) 
		return false;

	CString strReadString;

	while(TRUE) 
	{
		Sleep(0);

		if(true == m_stop)
			return false;

		if(false == file.ReadString(strReadString))
			break;

		UPDATE_HISTORY revHistory(false, m_version, 0, strReadString);
		revHistory.strSavePath.Format("%s%s%s", g_fullPath, strReadString, PREFIX_TEMP);
		revHistory.strFilePath.Format("%spatcher/%s", UPDATER_DIRECTORY, strReadString);
		vecHistory.push_back(revHistory);
	}

	file.Close();

	return true;
}

//릴리즈 폴더 업데이트 진행 함수 
void CPatcherUpdaterDlg::UpdateProgress(CInternetSession& session)
{
	int iHistoryCount = (int)m_history.size();

	if(iHistoryCount <= 0)
		return;

	for(int iNum = 0; iNum < iHistoryCount;)
	{
		DOWN_LOAD_TYPE eTypeDownLoad = DownloadProcess(m_history, iNum);

		if(E_DN_TYPE_FAIL == eTypeDownLoad)
		{
			m_connection->Close();
			session.Close();

			return;
		}
		else if(E_DN_TYPE_REPEAT == eTypeDownLoad)
		{
			continue;
		}

		++iNum;
	}
}

//DownloadRevision을 하나 수행하는 함수
CPatcherUpdaterDlg::DOWN_LOAD_TYPE CPatcherUpdaterDlg::DownloadProcess(std::vector<UPDATE_HISTORY>& vecHistory, int iStep)
{
	int count = (int)vecHistory.size();

	if(iStep < 0 || count <= iStep)
		return E_DN_TYPE_FAIL;

	if (true == m_stop)
		return E_DN_TYPE_FAIL;

	CString fileName = vecHistory[iStep].strFileName;

	CDownloader downloader;
	CDownloader::STATE downLoadType = downloader.Init(m_connection, m_path + vecHistory[iStep].strFilePath, vecHistory[iStep].strSavePath, true);

	DOWN_LOAD_TYPE ConnectType = DownloadConnectProcess(downloader, downLoadType);

	if(E_DN_TYPE_OK != ConnectType)
		return ConnectType;

	m_PartProg.SetPos(0);
	Sleep(100);

	bool bDownLoaded = false;
	DOWN_LOAD_TYPE DownLoadProcessType = FileDownloadProcess(downloader, bDownLoaded);

	if(E_DN_TYPE_OK != DownLoadProcessType)
		return DownLoadProcessType;

	m_TotalProg.SetPos((100*(++m_iUpdateStep)) / m_iUpdateStepTotal);

	if(bDownLoaded)
		return E_DN_TYPE_OK;
	else 
		return E_DN_TYPE_EXIST;

	return E_DN_TYPE_OK;
}

CPatcherUpdaterDlg::DOWN_LOAD_TYPE CPatcherUpdaterDlg::DownloadConnectProcess(CDownloader& rDownLoader, const int iDownLoadType)
{
	switch(iDownLoadType)
	{
	case CDownloader::STATE::FAIL:
		rDownLoader.Exit();
		return E_DN_TYPE_FAIL;

	case CDownloader::STATE::REPEAT:
		if(m_iConnenctTimeOutCount < TIMEOUT_REPEAT_MAX_COUNT)
		{
			++m_iConnenctTimeOutCount;
			return E_DN_TYPE_REPEAT;
		}

		rDownLoader.Exit();
		return E_DN_TYPE_FAIL;

	case CDownloader::STATE::SUCCESS:
		m_iConnenctTimeOutCount = 0;
		break;
	}

	return E_DN_TYPE_OK;
}

void CPatcherUpdaterDlg::CommitUpdater()
{
	CFileFind cFileFinder;
	std::string strFileName;

	BOOL bResult = cFileFinder.FindFile(g_fullPath + "*" + PREFIX_TEMP);
 	while(bResult)
	{
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		std::string originName(strFileName.data(), strFileName.find(PREFIX_TEMP));

 		DeleteFile(g_fullPath + originName.data());
 		rename(g_fullPath + strFileName.data(), g_fullPath + originName.data());
	}
}

void CPatcherUpdaterDlg::Execute()
{
	OSVERSIONINFO osInfo = { 0 };
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	SHELLEXECUTEINFO exInfo = { 0 };
	exInfo.cbSize			= sizeof(SHELLEXECUTEINFO);

	if (osInfo.dwMajorVersion == 6) {
		exInfo.lpVerb = "runas";
	}

	char filePath[1024] = { 0 };
	sprintf_s(filePath, "%s%s", g_fullPath, UPDATER_NAME);

	exInfo.lpFile = filePath;
	exInfo.lpDirectory = g_fullPath;
	exInfo.lpParameters = NULL;
	exInfo.nShow = SW_NORMAL;

	FATAL_ERROR(TRUE == ShellExecuteEx(&exInfo), THIS_FILE, __LINE__, UPDATER_NAME);
}
void CPatcherUpdaterDlg::OnClose()
{
	//__super::OnClose();
}
