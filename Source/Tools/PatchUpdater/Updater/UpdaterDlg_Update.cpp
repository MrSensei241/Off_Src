#include "stdafx.h"
#include "Updater.h"
#include "UpdaterDlg.h"
#include "Downloader.h"
#include "FileFinder.h"
#include "Config.h"
#include "Revision.h"
#include "Option.h"
#include ".\updaterdlg.h"
#include "XML.h"
#include <algorithm>
#include <utility>
#include "Compress.h"

// determine number of elements in an array (not bytes)
//#define _countof(array) (sizeof(array)/sizeof(array[0]))

class CDeleter : public CFileFinder {
public:
	void Init(const char* prefix)
	{
		m_prefix = prefix;
		Find(g_fullPath, "*.*");
		if (FALSE == SetCurrentDirectory(g_fullPath))
			ReportLastErrorExit("CDeleter::Init()");
	}

	void Exit()
	{
		m_filefind.Close();
	}

private:
	bool OnFindFile(WIN32_FIND_DATA& findData)
	{
		if (NULL == StrStrI(findData.cFileName, m_prefix))
			return true;

		if (FALSE == SetFileAttributes(findData.cFileName, FILE_ATTRIBUTE_NORMAL))
			ReportLastErrorExit("CDeleter::OnFindFile()");

		if (FALSE == DeleteFile(findData.cFileName))
			ReportLastErrorExit("CDeleter::OnFindFile()");

		return true;
	}

	bool OnFindDirectory(WIN32_FIND_DATA& findData)
	{
		return true;
	}

private:
	const char* m_prefix;
};

class CFinder : public CFileFinder {
public:
	void Init(const char* prefix)
	{
		m_found = false;
		m_prefix = prefix;
		Find(g_fullPath, "*.*");
		if (FALSE == SetCurrentDirectory(g_fullPath))
			ReportLastErrorExit("CFinder::Init()");
	}

	void Exit()
	{
		m_filefind.Close();
	}

	bool IsFound()
	{
		return m_found;
	}

private:
	bool OnFindFile(WIN32_FIND_DATA& findData)
	{
		if (NULL == StrStrI(findData.cFileName, m_prefix))
			return true;
		m_found = true;
		return false;
	}

	bool OnFindDirectory(WIN32_FIND_DATA& findData)
	{
		return true;
	}

private:
	const char* m_prefix;
	bool		m_found;
};

class CRenamer : public CFileFinder {
public:
	void Init()
	{
		Find(g_fullPath, "*.*");
		if (FALSE == SetCurrentDirectory(g_fullPath))
			ReportLastErrorExit("CRenamer::Init()");
	}

	void Exit()
	{
		m_filefind.Close();
	}

private:
	bool OnFindFile(WIN32_FIND_DATA& findData)
	{
		if (NULL == StrStrI(findData.cFileName, PREFIX_BACKUP)) {
			return true;
		}

		if (FALSE == SetFileAttributes(findData.cFileName, FILE_ATTRIBUTE_NORMAL))
			ReportLastErrorExit("CRenamer::OnFindFile()");

		CString newName(findData.cFileName);
		newName.Replace(PREFIX_BACKUP, PREFIX_ARRANGE);

		if (0 != rename(findData.cFileName, newName))
			ReportLastErrorExit("CRenamer::OnFindFile()");

		return true;
	}

	bool OnFindDirectory(WIN32_FIND_DATA& findData)
	{
		return true;
	}
};

class CCounter : public CFileFinder {
public:
	void Init(const char* prefix)
	{
		m_count = 0;
		m_prefix = prefix;
		Find(g_fullPath, "*.*");
		if (FALSE == SetCurrentDirectory(g_fullPath))
			ReportLastErrorExit("CFinder::Init()");
	}

	void Exit()
	{
		m_filefind.Close();
	}

	DWORD GetCount()
	{
		return m_count;
	}

private:
	bool OnFindFile(WIN32_FIND_DATA& findData)
	{
		if (NULL == StrStrI(findData.cFileName, m_prefix))
			return true;
		m_count++;
		return true;
	}

	bool OnFindDirectory(WIN32_FIND_DATA& findData)
	{
		return true;
	}

private:
	const char* m_prefix;
	DWORD		m_count;
};

class CRepairer : public CFileFinder {
public:
	void Init(CUpdaterDlg* owner)
	{
		CCounter counter;
		counter.Init(PREFIX_BACKUP);
		counter.Exit();

		m_owner = owner;
		m_totalCount = counter.GetCount();
		m_repaired = 0;

		Find(g_fullPath, "*.*");
		if (FALSE == SetCurrentDirectory(g_fullPath))
			ReportLastErrorExit("CRepairer::Init()");
	}

	void Exit()
	{
		m_filefind.Close();
	}

private:
	bool OnFindFile(WIN32_FIND_DATA& findData)
	{
		if (NULL == StrStrI(findData.cFileName, PREFIX_BACKUP)) {
			return true;
		}

		CString originalName(findData.cFileName);
		originalName.Replace(PREFIX_BACKUP, "");
		
		if (FALSE == SetFileAttributes(originalName, FILE_ATTRIBUTE_NORMAL)) {
			if (ERROR_FILE_NOT_FOUND != GetLastError())
				ReportLastErrorExit("CRepairer::OnFindFile()");
		}

		if (FALSE == DeleteFile(originalName)) {
			if (ERROR_FILE_NOT_FOUND != GetLastError())
				ReportLastErrorExit("CRepairer::OnFindFile()");
		}

		if (FALSE == SetFileAttributes(findData.cFileName, FILE_ATTRIBUTE_NORMAL))
			ReportLastErrorExit("CRepairer::OnFindFile()");

		if (0 != rename(findData.cFileName, originalName))
			ReportLastErrorExit("CRepairer::OnFindFile()");

		m_repaired++;

		m_owner->SetPartWorkProgressGraphic(m_repaired, m_totalCount);

		return true;
	}

	bool OnFindDirectory(WIN32_FIND_DATA& findData)
	{
		return true;
	}

private:
	CUpdaterDlg*	m_owner;
	DWORD			m_totalCount;
	DWORD			m_repaired;
};

bool IsExistPrefixedFile(const char* prefix)
{
	CFinder finder;
	finder.Init(prefix);
	finder.Exit();
	return finder.IsFound();
}

//CFileException 의 에러 메시지를 얻어오는 함수 
CString CUpdaterDlg::GetFileExceptionError(CFileException* exception)
{
	TCHAR   szErrorMessage[512];
	UINT    nHelpContext;

	exception->GetErrorMessage(szErrorMessage, _countof(szErrorMessage), &nHelpContext);

	return CString(szErrorMessage);
}

void CUpdaterDlg::Run()
{
	SetUpdaterVersion(UPDATER_VERSION);
	m_CacheDataClass.pDlg = this;

	DeleteAllFile(g_datainfofolder);
	CreateDirectory(g_datafolder, NULL);
	CreateDirectory(g_patchfolder, NULL);

#ifdef _DEBUG
	CreateDirectory(g_datainfofolder, NULL);
#endif

	SetWorkingExplain(Config::String::connecting);
	SetTotalProgress(0);
	SetPartWorkProgressGraphic(0, 0);
	m_iUpdateStep = 0;
	m_iUpdateStepTotal = 0;

	Sleep(100);

	m_flashCtrl.SetSeamlessTabbing(FALSE);

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
		ReportLastErrorExit("CUpdaterDlg::Run()");
		
		SetWorkingExplain(Config::String::connecting, Config::String::fail);

		FATAL_ERROR(false, THIS_FILE, __LINE__, Config::String::failedConnectServer);
		
		return;
	}

	SetWorkingExplain(Config::String::connecting, Config::String::complete);
	SetWorkingExplain(Config::String::prepairing);

	//// 업데이터의 버전을 비교하여 최신버전이 아니면 업데이트를 한다.
	CString strFullName;
	strFullName.Format("%s%s", UPDATER_DIRECTORY, UPDATER_VERSION_FILE_NAME);
	DownloadRevisionFile(session, strFullName, UPDATER_VERSION_FILE_NAME);

	if(false == InspectUpdaterVersion(session))
	{
		strFullName.Format("%s%s", UPDATER_DIRECTORY, UPDATER_DOWNLOADER_EXE);
		DownloadRevisionFile(session, strFullName, UPDATER_DOWNLOADER_EXE);

		UpdaterExecute();
		ExitProcess(0);
	}

	m_bExchangeFileName = true;
	DeleteFile(g_fullPath + UPDATER_DOWNLOADER_EXE);
	SetVersion();

	DeleteAllFile(g_gameRootPath + "release/", ".pak");
	m_bExchangeFileName = false;
	
	//////
	std::string pullversion;
	CString DataFullVersion, ReleaseFullVersion, DataFullList, revisionfullname;

	m_bNowUpdate = true;
	//받을 파일 목록 받음
	DataFullVersion.Format("%s%s", FULL_DIRECTORY, DATA_REVISION_FILE_NAME);
	DownloadRevisionFile(session, DataFullVersion, FULL_DATA_REVISION_FILE_NAME);

	ReleaseFullVersion.Format("%s%s", FULL_DIRECTORY, RELEASE_REVISION_FILE_NAME);
	DownloadRevisionFile(session, ReleaseFullVersion, FULL_RELEASE_REVISION_FILE_NAME);

	DataFullList.Format("%s%s", FULL_DIRECTORY, DATA_LIST_FILE_NAME);
	DownloadRevisionFile(session, DataFullList, DATA_LIST_FILE_NAME);

	m_CacheDataClass.SetFullVersion();

	m_bExchangeFileName = true;
	SetWorkingExplain(Config::String::arranging);
	DeleteFile(g_fullPath + DATA_LIST_FILE_NAME);
	DeleteFile(g_fullPath + FULL_DATA_REVISION_FILE_NAME);
	DeleteFile(g_fullPath + FULL_RELEASE_REVISION_FILE_NAME);
	m_bExchangeFileName = false;

	SetWorkingExplain(Config::String::prepairing);

	revisionfullname.Format("%s%s", PARTIAL_DIRECTORY, PRE_REVISION_FILE_NAME);
	DownloadRevisionFile(session, revisionfullname, PRE_REVISION_FILE_NAME);

	revisionfullname.Format("%s%s", PARTIAL_DIRECTORY, DATA_REVISION_FILE_NAME);
	DownloadRevisionFile(session, revisionfullname, DATA_REVISION_FILE_NAME);

	revisionfullname.Format("%s%s", PARTIAL_DIRECTORY, RELEASE_REVISION_FILE_NAME);
	DownloadRevisionFile(session, revisionfullname, RELEASE_REVISION_FILE_NAME);

	m_iUpdateStepTotal = 0;

	m_CacheDataClass.InspectPatchIpf();
 	m_CacheDataClass.InspectDataIpf();

	SetPartWorkProgressGraphic(100, 100);
	Sleep(100);

	SetPartWorkProgressGraphic(0, 0);
	SetWorkingExplain(Config::String::prepairing);
	Sleep(100);

	//업데이트 전체 진행
	TotalUpdateProgress(session);

	SetPartWorkProgress(100);

	m_bExchangeFileName = true;
	SetWorkingExplain(Config::String::arranging);
	m_CacheDataClass.WriteCacheData();

	DeleteFile(g_fullPath + PRE_REVISION_FILE_NAME);
	DeleteFile(g_fullPath + DATA_REVISION_FILE_NAME);
	DeleteFile(g_fullPath + RELEASE_REVISION_FILE_NAME);

	m_bExchangeFileName = false;

	SetTotalProgress(100);
	SetPartWorkProgress(100);

	SetWorkingExplain(Config::String::complete);

	m_bNowUpdate = false;

	if (Option::directlyRun) {
		Execute();
		ExitProcess(0);
	}

	LINE_MESSAGE_BOX; 

	LINE_MESSAGE_BOX_EX("%d", m_bRequestedRepair);
	while(1) {
		if(m_bRequestedOptimize) {
			OptimizeIpfFiles();
		}

		if(m_bRequestedRepair) {
			SetTotalProgress(0);
			SetPartWorkProgress(0);

			m_bRepair = true;
			m_releasedownlist.clear();
			m_iUpdateStepTotal = 0;

			m_CacheDataClass.SetDataVersion();
			m_CacheDataClass.InspectPatchIpf();
			m_CacheDataClass.InspectDataIpf();
			m_CacheDataClass.SetReleaseVersionNS(0);

			revisionfullname.Format("%s%s", PARTIAL_DIRECTORY, RELEASE_REVISION_FILE_NAME);
			DownloadRevisionFile(session, revisionfullname, RELEASE_REVISION_FILE_NAME);

			revisionfullname.Format("%s%s", PARTIAL_DIRECTORY, DATA_REVISION_FILE_NAME);
			DownloadRevisionFile(session, revisionfullname, DATA_REVISION_FILE_NAME);

			TotalUpdateProgress(session, false);

			m_bExchangeFileName = true;
			SetWorkingExplain(Config::String::arranging);
			DeleteFile(g_fullPath + DATA_REVISION_FILE_NAME);
			DeleteFile(g_fullPath + RELEASE_REVISION_FILE_NAME);

			m_CacheDataClass.WriteCacheData();
			m_bExchangeFileName = false;

			m_bRepair = false;
			SetTotalProgress(100);
			SetPartWorkProgress(100);
			SetWorkingExplain(Config::String::complete);
		}
		Sleep(1);
	}

	m_connection->Close();
	session.Close();
}

void CUpdaterDlg::SetVersion()
{
	CStdioFile file;
	CString strReadString;
	DWORD version = 0;

	if(file.Open(g_gameRootPath + RELEASE_REVISION_FILE_NAME, CFile::modeRead))
	{
		file.ReadString(strReadString);
		file.Close();
		version = atoi(strReadString.GetBuffer());
	}

	m_CacheDataClass.SetReleaseVersion(version);
}

bool CompareRevision(UPDATE_HISTORY& r1,UPDATE_HISTORY& r2)
{
	if(r1.dwHistory < r2.dwHistory)
		return true;

	return false;
}

//업데이트 전체 진행 함수 
void CUpdaterDlg::TotalUpdateProgress(CInternetSession& session, bool pre)
{
	m_iUpdateStep = 0;
	m_iUpdateStepTotal = 0;

	m_datadownlist.clear();
	m_preparedownlist.clear();
	m_releasedownlist.clear();

	PrepareListMaker premaker(m_CacheDataClass);
	PartialListMaker datamaker(m_CacheDataClass);
	ReleaseListMaker relmaker(m_CacheDataClass);

	m_CacheDataClass.GetDownloadDataList(m_reparedownlistD);

	m_iUpdateStepTotal += (int)m_reparedownlistD.size() * 2;

	MakeDownloadList(m_CacheDataClass.GetFullVersion(), session, DATA_REVISION_FILE_NAME, datamaker, m_datadownlist);
	MakeDownloadList(m_CacheDataClass.GetReleaseVersion(), session, RELEASE_REVISION_FILE_NAME, relmaker, m_releasedownlist);
	if(pre) MakeDownloadList(m_CacheDataClass.GetFullVersion(), session, PRE_REVISION_FILE_NAME, premaker, m_preparedownlist);

	if(!m_datadownlist.empty()) m_iUpdateStepTotal += (int)m_datadownlist.size() * 2;
	if(!m_preparedownlist.empty()) m_iUpdateStepTotal += (int)m_preparedownlist.size();
	if(!m_releasedownlist.empty()) m_iUpdateStepTotal += (int)m_releasedownlist.size() * 2;
	m_iUpdateStepTotal += 1;

	Repair(session);

	NormalUpdateProgress(session);

	DeleteAllFile(g_datafolder, PREFIX_TEMP);

	ReleaseUpdateProgress(session);

	PrepareUpdateProgress(session);

}

//일반 업데이트 진행 함수 
void CUpdaterDlg::NormalUpdateProgress(CInternetSession& session)
{
	int iHistoryCount = (int)m_datadownlist.size();

	if(iHistoryCount <= 0)
		return;

	for(int iNum = 0; iNum < iHistoryCount;)
	{
		//ftp에서 파일 받는 부분
		DOWN_LOAD_TYPE eTypeDownLoad = DownloadRevisionsProcess(m_datadownlist, iNum);

		if(CUpdaterDlg::E_DN_TYPE_FAIL == eTypeDownLoad)
		{
			m_connection->Close();
			session.Close();

			SetWorkingExplain(Config::String::downloading, Config::String::fail);

			CString strComment;
			strComment.Format("%s %s", m_datadownlist[iNum].strFileName, Config::String::failedDownLoadFile);
			FATAL_ERROR(false, THIS_FILE, __LINE__, strComment);

			return;
		}
		else if(CUpdaterDlg::E_DN_TYPE_REPEAT == eTypeDownLoad)
		{
			continue;
		}

		SetTotalProgress((100*(++m_iUpdateStep)) / m_iUpdateStepTotal);
		Sleep(100);

		++iNum;
	}
}


//릴리즈 폴더 업데이트 진행 함수 
void CUpdaterDlg::ReleaseUpdateProgress(CInternetSession& session)
{
	int iHistoryCount = (int)m_releasedownlist.size();

	if(iHistoryCount <= 0)
		return;

	for(int iNum = 0; iNum < iHistoryCount;)
	{
		//ftp에서 파일 받는 부분
		DOWN_LOAD_TYPE eTypeDownLoad = DownloadRevisionsProcess(m_releasedownlist, iNum);

		if(CUpdaterDlg::E_DN_TYPE_FAIL == eTypeDownLoad)
		{
			m_connection->Close();
			session.Close();

			SetWorkingExplain(Config::String::downloading, Config::String::fail);

			CString strComment;
			strComment.Format("%s %s", m_releasedownlist[iNum].strFileName, Config::String::failedDownLoadFile);
			FATAL_ERROR(false, THIS_FILE, __LINE__, strComment);

			return;
		}
		else if(CUpdaterDlg::E_DN_TYPE_REPEAT == eTypeDownLoad)
		{
			continue;
		}

		m_CacheDataClass.SetReleaseVersion(m_releasedownlist[iNum].dwHistory);
		while(m_bPause) {Sleep(1);}

		ExportPakFile(m_releasedownlist[iNum].strSavePath);
		DeleteFile(m_releasedownlist[iNum].strSavePath);

		SetTotalProgress((100*(++m_iUpdateStep)) / m_iUpdateStepTotal);
		Sleep(100);

		++iNum;
	}
}

//미리 받을 업데이트 진행 함수 
void CUpdaterDlg::PrepareUpdateProgress(CInternetSession& session)
{
	//m_iUpdateStep = 0;
	int iHistoryCount = (int)m_preparedownlist.size();

	if(iHistoryCount <= 0)
		return;

	while(true)
	{
		DOWN_LOAD_TYPE eTypeDownLoad = DownloadRevisionsProcess(m_preparedownlist, 0);

		if(CUpdaterDlg::E_DN_TYPE_FAIL == eTypeDownLoad)
		{
			m_connection->Close();
			session.Close();

			SetWorkingExplain(Config::String::downloading, Config::String::fail);

			CString strComment;
			strComment.Format("%s %s", m_preparedownlist[0].strFileName, Config::String::failedDownLoadFile);
			FATAL_ERROR(false, THIS_FILE, __LINE__, strComment);

			return;
		}
		else if(CUpdaterDlg::E_DN_TYPE_REPEAT != eTypeDownLoad) return;
	}
}

void CUpdaterDlg::RepairUpdateProgress(CInternetSession& session)
{
	int iHistoryCount = (int)m_reparedownlistD.size();

	if(iHistoryCount <= 0)
		return;

	for(int iNum = 0; iNum < iHistoryCount;)
	{
		//ftp에서 파일 받는 부분
		DOWN_LOAD_TYPE eTypeDownLoad = DownloadRevisionsProcess(m_reparedownlistD, iNum);

		if(CUpdaterDlg::E_DN_TYPE_FAIL == eTypeDownLoad)
		{
			m_connection->Close();
			session.Close();

			SetWorkingExplain(Config::String::downloading, Config::String::fail);

			CString strComment;
			strComment.Format("%s %s", m_reparedownlistD[iNum].strFileName, Config::String::failedDownLoadFile);
			FATAL_ERROR(false, THIS_FILE, __LINE__, strComment);

			return;
		}
		else if(CUpdaterDlg::E_DN_TYPE_REPEAT == eTypeDownLoad)
		{
			continue;
		}

		ExchangeIpfFileName(g_datafolder, m_reparedownlistD[iNum].strFileName.GetBuffer());

		SetTotalProgress((100*(++m_iUpdateStep)) / m_iUpdateStepTotal);
		Sleep(100);

		++iNum;
	}
}


// ftp 접속 시도 함수
CUpdaterDlg::DOWN_LOAD_TYPE CUpdaterDlg::DownloadConnectProcess(CDownloader& rDownLoader, const int iDownLoadType)
{
	switch(iDownLoadType)
	{
	case CDownloader::FAIL:
		rDownLoader.Exit();
		return E_DN_TYPE_FAIL;

	case CDownloader::REPEAT:
		if(m_iConnenctTimeOutCount < TIMEOUT_REPEAT_MAX_COUNT) // 다운로드 ftp 접속을 제한 수 이상 시도 하지 않음
		{
			++m_iConnenctTimeOutCount;
			return E_DN_TYPE_REPEAT;
		}

		rDownLoader.Exit();
		return E_DN_TYPE_FAIL;

	case CDownloader::SUCCESS:
		m_iConnenctTimeOutCount = 0;
		break;
	}

	return E_DN_TYPE_OK;
}

//DownloadRevision을 하나 수행하는 함수
CUpdaterDlg::DOWN_LOAD_TYPE CUpdaterDlg::DownloadRevisionsProcess(UPDATE_HISTORY_LIST& vecHistory, int iStep)
{
	if(true == g_manualy)
		return E_DN_TYPE_EXIST;

	int count = (int)vecHistory.size();

	if(iStep < 0 || count <= iStep)
		return E_DN_TYPE_FAIL;

	if (true == m_stop)
		return E_DN_TYPE_FAIL;

	CString fileName = vecHistory[iStep].strFileName;

	SetWorkingExplain(Config::String::downloading, fileName);

	CDownloader downloader;
	CDownloader::STATE downLoadType = downloader.Init(m_connection, m_path + vecHistory[iStep].strFilePath, vecHistory[iStep].strSavePath, true);

	CUpdaterDlg::DOWN_LOAD_TYPE ConnectType = DownloadConnectProcess(downloader, downLoadType);

	if(E_DN_TYPE_OK != ConnectType)
		return ConnectType;

	SetPartWorkProgressGraphic(0, 0);
	Sleep(100);

	bool bDownLoaded = false;
	CUpdaterDlg::DOWN_LOAD_TYPE DownLoadProcessType = FileDownloadProcess(downloader, bDownLoaded);

	if(E_DN_TYPE_OK != DownLoadProcessType)
		return DownLoadProcessType;

	SetTotalProgress((100*(++m_iUpdateStep)) / m_iUpdateStepTotal);

	if(bDownLoaded)
		return E_DN_TYPE_OK;
	else 
		return E_DN_TYPE_EXIST;

	return E_DN_TYPE_OK;
}

// 파일 다운로드 진행 함수 
CUpdaterDlg::DOWN_LOAD_TYPE CUpdaterDlg::FileDownloadProcess(CDownloader& rDownLoader, bool& rbDownLoaded)
{
	while (TRUE) 
	{
		Sleep(2);

		while(m_bPause) {Sleep(1);}

		if(true == m_stop) 
		{
			rDownLoader.Exit();
			return E_DN_TYPE_FAIL;
		}

		CDownloader::STATE state = rDownLoader.Do();

		SetPartWorkProgressGraphic((UINT)rDownLoader.GetReceivedByte(), (UINT)rDownLoader.GetTotalByte());
		SetPartWorkProgress((int)rDownLoader.GetReceivedByte(), (int)rDownLoader.GetTotalByte());

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
			if(m_iTimeOutCount < TIMEOUT_REPEAT_MAX_COUNT) // Connection TimeOut 이 제한 수 이상 발생하면 Connection 재시도 하지 않음
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

//문자열 token 함수 
void CUpdaterDlg::TokenizeString(std::vector<CString>& rvecString, const CString& rStrOrg, const CString& rToken)
{
	if(!rvecString.empty())
		rvecString.clear();

	CString strTok   = rStrOrg;
	CString resToken = rStrOrg;

	int curPos = 0;

	resToken = strTok.Tokenize(rToken, curPos);

	while(resToken != "")
	{
		rvecString.push_back(resToken);
		resToken = strTok.Tokenize(rToken, curPos);
	};
}

void CUpdaterDlg::DownloadRevisionFile(CInternetSession& session, const CString& rFtpFileName, const CString& rLocalFileName)
{
	if (false == DownloadHistory(rFtpFileName, rLocalFileName)) 
	{
		m_connection->Close();
		session.Close();

		SetWorkingExplain(Config::String::prepairing, Config::String::fail);

		CString strComment;
		strComment.Format("%s\n\n%s", Config::String::failedDownLoadList, rFtpFileName);
		FATAL_ERROR(false, THIS_FILE, __LINE__, strComment);

		return;
	}
}

void CUpdaterDlg::MakeDownloadList(DWORD lastversion, CInternetSession& session, const CString& rFtpFileName, DownloadListMaker& rMaker, UPDATE_HISTORY_LIST& vecHistory)
{
	if (false == rMaker.GetList(lastversion, rFtpFileName, vecHistory)) 
	{
		m_connection->Close();
		session.Close();

		SetWorkingExplain(Config::String::prepairing, Config::String::fail);

		CString strComment;
		strComment.Format("%s\n\n%s",Config::String::failedAnalysisDownLoadList, rFtpFileName);
		FATAL_ERROR(false, THIS_FILE, __LINE__, strComment);

		return;
	}

	vecHistory.PrepareDownload();

}
bool CUpdaterDlg::DownloadHistory(const CString& rFtpFileName, const CString& rLocalFileName)
{
	if (true == g_manualy)
		return true;

	SetPartWorkProgressGraphic(0, 0);

	CDownloader downloader;

	if(CDownloader::SUCCESS != downloader.Init(m_connection, m_path + rFtpFileName, g_fullPath + rLocalFileName, false)) 
	{
		downloader.Exit();
		return false;
	}
	
	bool bDownLoaded = false;
	CUpdaterDlg::DOWN_LOAD_TYPE DownLoadProcessType = FileDownloadProcess(downloader, bDownLoaded);

	if(E_DN_TYPE_OK == DownLoadProcessType)
		return true;

	return false;
}

void CUpdaterDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_RETURN == nChar)
		return;
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CUpdaterDlg::OnOK()
{
	return;
	__super::OnOK();
}

void CUpdaterDlg::OnCancel()
{
	__super::OnCancel();
}

void CUpdaterDlg::OnClose()
{
	// alt+4f 강제 종료를 방지.
//	__super::OnClose(); 
}

//진행중인 업데이트 설명 설정 함수 
void CUpdaterDlg::SetWorkingExplain(const CString strExplain, const CString strSecond)
{
	CString explain;

	if(strSecond.IsEmpty())
		explain.Format("%s", strExplain);
	else 
		explain.Format("%s '%s'", strExplain, strSecond);

	SetWorkExplain(explain);
};

// 업데이트 진행 상황 그래픽으로 보여줌
void CUpdaterDlg::SetPartWorkProgressGraphic(const UINT uCurrent, const UINT uMaximum)
{
	if(0 == uCurrent || 0 == uMaximum)
	{
		SetPartWorkProgress(0);
		return;
	}

	ULONGLONG ulCurrent = uCurrent;

	int per = (int)((100 * ulCurrent) / uMaximum);

	SetPartWorkProgress(per);
}

//전체 패스에서 파일 이름 얻어오는 함수 
CString CUpdaterDlg::GetSpecificStringFromPath(const char* pFileName)
{
	if(NULL == pFileName)
		return CString("");

	char drive[_MAX_DRIVE];
	char dir[8192];
	char fileName[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath_s(pFileName, drive, dir, fileName, ext);

	return CString(fileName) + CString(ext);
}

// 새로 만들어진 ipf 의 파일명을 임시 파일명에서 정규 파일명으로 변경하고 기존의 ipf 파일을 삭제하는 함수 
void CUpdaterDlg::ExchangeIpfFileName(const CString& filePath, const CString& fileName)
{
	if(filePath.IsEmpty())
		return;

	if(fileName.IsEmpty())
		return;

	m_bExchangeFileName = true;
	SetWorkingExplain(Config::String::arranging);

	CString coreFileName = fileName;
	coreFileName.Replace(PREFIX_IPF, "");

	CString oldFileNamePrev = filePath + fileName;
	CString oldFileNameNext = filePath + fileName + "_";
	rename(oldFileNamePrev, oldFileNameNext);

	CString newFileNamePrev = filePath + coreFileName + PREFIX_TEMP;
	CString newFileNameNext = filePath + coreFileName + PREFIX_IPF;
	rename(newFileNamePrev, newFileNameNext);
	
	DeleteFile(oldFileNameNext);

	m_bExchangeFileName = false;
}

void CUpdaterDlg::OptimizeIpfFiles()
{
	CFileFind ff; 
	BOOL bFind = ff.FindFile(g_patchfolder + "*.ipf");
	if(bFind == FALSE)
	{
		AfxMessageBox(Config::String::complete, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);
		m_bRequestedOptimize = false;
		return;
	}

	m_bOptimize = true;

	SetWorkingExplain(Config::String::merging);
	SetTotalProgress(0);
	SetPartWorkProgress(0);

	m_CacheDataClass.OptimizeAllIpfFiles();

	SetWorkingExplain(Config::String::canceling);

	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_datafolder + "*.temp");
	while(bResult)
	{
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();
		strFileName.Replace(".temp", ".ipf");

		ExchangeIpfFileName(g_datafolder, strFileName);
	}

	m_bExchangeFileName = true;
	DeleteAllFile(g_datainfofolder);

	if (!m_bOptimizeStop) {
		DeleteAllFile(g_patchfolder, ".ipf");
	}

	CreateDirectory(g_patchfolder, NULL);
#ifdef _DEBUG
	CreateDirectory(g_datainfofolder, NULL);
#endif

	m_CacheDataClass.WriteCacheData();

	m_bExchangeFileName	 = false;
	m_bOptimize		     = false;
	m_bRequestedOptimize = false;

	SetTotalProgress(100);
	SetPartWorkProgress(100);

	SetWorkingExplain(Config::String::complete);
	m_bOptimizeStop = false;
}

bool CUpdaterDlg::Repair(CInternetSession& session)
{
	RepairUpdateProgress(session);

	m_bRequestedRepair = false;

	m_CacheDataClass.SetDataVersion();
	m_CacheDataClass.InspectPatchIpf();
	return m_CacheDataClass.InspectDataIpf();
}

void CUpdaterDlg::DeleteAllFile(const char* szDir) 
{
	CString strName; 
	strName.Format( "%s\\*.*", szDir ); 

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

void CUpdaterDlg::DeleteAllFile(const char* szDir, const char* key)
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

bool CUpdaterDlg::InspectUpdaterVersion(CInternetSession& session)
{
	DWORD version;
	CStdioFile file;
	CString strReadString;

	file.Open(g_fullPath + UPDATER_VERSION_FILE_NAME, CFile::modeRead);
	while(TRUE) 
	{
		if(false == file.ReadString(strReadString))
			break;

		version = atoi(strReadString.GetBuffer());
		break;
	}
	file.Close();

	if(UPDATER_VERSION < version)
		return false;

	DeleteFile(g_fullPath + UPDATER_DOWNLOADER_EXE);
	DeleteFile(g_fullPath + UPDATER_VERSION_FILE_NAME);
	return true;
}

void CUpdaterDlg::UpdaterExecute()
{
	OSVERSIONINFO osInfo = { 0 };
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	SHELLEXECUTEINFO exInfo = { 0 };
	exInfo.cbSize			= sizeof(SHELLEXECUTEINFO);

	if (osInfo.dwMajorVersion == 6) {
		exInfo.lpVerb = "runas";
	}

	char filePath[1024] = { 0 };
	sprintf_s(filePath, "%s%s", g_fullPath, UPDATER_DOWNLOADER_EXE);

	exInfo.lpFile = filePath;
	exInfo.lpDirectory = g_fullPath;
	exInfo.lpParameters = NULL;
	exInfo.nShow = SW_NORMAL;

	FATAL_ERROR(TRUE == ShellExecuteEx(&exInfo), THIS_FILE, __LINE__, UPDATER_DOWNLOADER_EXE);
}

inline char* SmartStrcpy(char* pDest, const char* pSrc)
{
	while (1)
	{
		*pDest = *pSrc;

		if (*pDest)
		{
			++pDest;
			++pSrc;
		}
		else
			return pDest;	// return end of 'Dest' (null)
	}
}//

inline void MakeExtractedFileFullPath(char* pOut, const char* pExtractedFileOutputDir, const char* pFilenameInIpf, size_t FilenameInIpfLength)
{
	char* pDestEnd = SmartStrcpy(pOut, pExtractedFileOutputDir);
	if (pOut < pDestEnd)
	{
		--pDestEnd;
		if (*pDestEnd != '/')
		{
			++pDestEnd;
			*pDestEnd = '/';
		}

		++pDestEnd;
	}

	while (FilenameInIpfLength)
	{
		*pDestEnd = *pFilenameInIpf;
		++pDestEnd;
		++pFilenameInIpf;
		--FilenameInIpfLength;
	}

	*pDestEnd = '\0';
}

inline void GetDirectoryFromFullPath(char* pOut, const char* pFullPath)
{
	char* pDestEnd = SmartStrcpy(pOut, pFullPath);

	do --pDestEnd;
	while (pDestEnd != pOut		&&		*pDestEnd != '/');

	*pDestEnd = '\0';
}

void CreateDirectory_(const char* pDir)
{
	BOOL bSuccessed = CreateDirectory(pDir, NULL);
	if (!bSuccessed && GetLastError()==ERROR_PATH_NOT_FOUND)
	{
		char UpperDirectory[MAX_PATH];
		GetDirectoryFromFullPath(UpperDirectory, pDir);
		CreateDirectory_(UpperDirectory);
		CreateDirectory(pDir, NULL);
	}
}

void CUpdaterDlg::ExportPakFile(const char* filename)
{
	m_bExportPak = true;
	SetWorkingExplain(Config::String::extracting);

	FILE* m_pIpfFile;
	fopen_s(&m_pIpfFile, filename, "rb+");	// AddFile...() 등 쓰기동작이 필요한 함수들도 있기때문에 rb+ 사용
	if (m_pIpfFile==NULL)
		return ;

	fseek(m_pIpfFile, 0, SEEK_END);
	DWORD filesize = ftell(m_pIpfFile);
	fseek(m_pIpfFile, 0, SEEK_SET);

	while (filesize > 0)
	{
		BYTE*					pCompressedDataBuffer;
		BYTE*					pUncompressedDataBuffer;
		PATCH_DATA_DESCRIPTOR	TempDataDescriptor;
		char					TempFilenameBuffer[MAX_PATH] = {0,};

		fread(&TempDataDescriptor.header, sizeof(PATCH_FILE_HEADER), 1, m_pIpfFile);
		fread(TempFilenameBuffer, TempDataDescriptor.header.FilenameLength, 1, m_pIpfFile);

		TempDataDescriptor.dataname = TempFilenameBuffer;
		TempDataDescriptor.Offset = ftell(m_pIpfFile);

		pCompressedDataBuffer = new BYTE[TempDataDescriptor.header.CompressedSize];
		pUncompressedDataBuffer = new BYTE[TempDataDescriptor.header.UnCompressedSize];

		fread(pCompressedDataBuffer, TempDataDescriptor.header.CompressedSize, 1, m_pIpfFile);

		bool DecompressResult = Decompress(pCompressedDataBuffer, TempDataDescriptor.header.CompressedSize
			, pUncompressedDataBuffer, TempDataDescriptor.header.UnCompressedSize);

		char ExtractedFileFullPath[MAX_PATH];
		char FileContainDir[MAX_PATH];

 		MakeExtractedFileFullPath(ExtractedFileFullPath, g_gameRootPath + "release/", TempFilenameBuffer, TempDataDescriptor.header.FilenameLength);

		GetDirectoryFromFullPath(FileContainDir, ExtractedFileFullPath);
		if (*FileContainDir)
			CreateDirectory_(FileContainDir);

		FILE* pExtractedFile;
		fopen_s(&pExtractedFile, ExtractedFileFullPath, "wb");
		fwrite(pUncompressedDataBuffer, TempDataDescriptor.header.UnCompressedSize, 1, pExtractedFile);
		fclose(pExtractedFile);

		filesize = filesize - sizeof(PATCH_FILE_HEADER) - TempDataDescriptor.header.CompressedSize
			- TempDataDescriptor.header.FilenameLength;
	}

	fclose(m_pIpfFile);

	SetWorkingExplain(Config::String::extracting, Config::String::complete);
	m_bExportPak = false;
}