#include "stdafx.h"
#include "CacheData.h"
#include "Updater.h"
#include "UpdaterDlg.h"
#include "Config.h"
#include "XML.h"

#define chSTR2(x) #x
#define chSTR(x)  chSTR2(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)


UPDATE_HISTORY_LIST::UPDATE_HISTORY_LIST()
{
	runableIndex = 0;
}

UPDATE_HISTORY& UPDATE_HISTORY_LIST::operator[](int i)
{
	return histories[i];
}

void UPDATE_HISTORY_LIST::PrepareDownload()
{
	notNecessaryHistories.clear();
	std::vector<UPDATE_HISTORY> notNecessaryList;
	for (UINT i = 0; i < histories.size(); i++)
	{
		UPDATE_HISTORY& history = histories[i];
		const char * fileName = LPCTSTR(histories[i].strFileName);
		if (strnicmp(fileName, "bg_hi", 5) == 0 && strlen(fileName) > 5)
		{
			char tmpBuf[1024];
			sprintf(tmpBuf, "%d %s", i, LPCTSTR(histories[i].strFileName));
			AfxMessageBox(tmpBuf, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);
			LINE_MESSAGE_BOX_EX("Z");

			notNecessaryList.push_back(history);
			histories.erase(histories.begin() + i);
			i--;
		}
	}
}

UINT UPDATE_HISTORY_LIST::size() const
{
	return histories.size();
}

bool UPDATE_HISTORY_LIST::empty() const
{
	return histories.empty() && notNecessaryHistories.empty();
}

void UPDATE_HISTORY_LIST::clear()
{
	histories.clear();
	notNecessaryHistories.clear();
}

void UPDATE_HISTORY_LIST::push_back(const UPDATE_HISTORY& history)
{
	histories.push_back(history);
}

CCacheData::CCacheData(void)
{
	m_PatchStart = UINT_MAX;
	m_dwReleaseVersion = 0;
	m_dwLastPatchVersion = 0;
	m_ServerDataList.clear();
	m_NeedDataList.clear();

	m_fullDownload = false;
}

CCacheData::~CCacheData(void)
{
}

void CCacheData::SetFullVersion()
{
	CStdioFile file;
	CString strReadString;

	file.Open(g_fullPath + FULL_DATA_REVISION_FILE_NAME, CFile::modeRead);
	while(TRUE) 
	{
		if(false == file.ReadString(strReadString))
			break;

		m_ServerVersion = atoi(strReadString.GetBuffer());
		m_dwLastPatchVersion = m_ServerVersion;
	}
	file.Close();

	SetDataVersion();
	SetServerDataList();
}

void CCacheData::SetDataVersion()
{
	m_DataLowVersion = UINT_MAX;
	m_DataHighVersion = 0;

	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_datafolder + "*" + PREFIX_IPF);
	while(bResult) {
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		if(m_IpfManager.isIPF(g_datafolder + strFileName)) {
			if (m_IpfManager.m_info.End < m_DataLowVersion) {
				m_DataLowVersion = m_IpfManager.m_info.End;
			}

			if (m_IpfManager.m_info.End > m_DataHighVersion) {
				m_DataHighVersion = m_IpfManager.m_info.End;
			}
		}
	}

	m_PatchStart = m_DataLowVersion + 1;
}

void CCacheData::SetServerDataList()
{
	CStdioFile file;
	CString strReadString;

	file.Open(g_fullPath + DATA_LIST_FILE_NAME, CFile::modeRead);
	while(TRUE) 
	{
		if(false == file.ReadString(strReadString))
			break;

		m_ServerDataList.push_back(strReadString + PREFIX_IPF);
	}
	file.Close();
}


bool CCacheData::InspectDataIpf()
{
	pDlg->SetPartWorkProgressGraphic(100, 100);
	m_NeedDataList = m_ServerDataList;

	if(m_fullDownload)
	{
		pDlg->DeleteAllFile(g_datafolder, PREFIX_IPF);
		m_DataLowVersion = m_DataHighVersion = m_ServerVersion;
		return false;
	}

	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_datafolder + "*" + PREFIX_IPF);
	while(bResult)
	{
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		std::list<CString>::iterator it;
		it = std::find(m_ServerDataList.begin(), m_ServerDataList.end(), strFileName);

		if((it != m_ServerDataList.end()) && (m_IpfManager.isIPF(g_datafolder + strFileName))) {
			m_NeedDataList.remove(strFileName);
		} else {
			DeleteFile(g_datafolder + strFileName);
		}
	}

	return m_NeedDataList.empty();
}

void CCacheData::GetDownloadDataList(UPDATE_HISTORY_LIST& list)
{
	list.clear();

	std::list<CString>::iterator it = m_NeedDataList.begin();
	for(; it != m_NeedDataList.end(); ++it)
	{
		std::string filename((*it).GetBuffer(), 0, (*it).Find("."));
		UPDATE_HISTORY revHistory(m_ServerVersion, 0, filename.data());
		revHistory.strFilePath.Format("%s%s%s.ipf", FULL_DIRECTORY, DATA_DOWNLOAD, filename.data());
		revHistory.strSavePath.Format("%s%s%s", g_datafolder, filename.data(), PREFIX_TEMP);
		list.push_back(revHistory);

		if (m_ServerVersion < m_DataLowVersion) {
			m_DataLowVersion = m_ServerVersion;
		}

		if (m_ServerVersion > m_DataHighVersion) {
			m_DataHighVersion = m_ServerVersion;
		}
	}

	list.PrepareDownload();
}

void CCacheData::InspectPatchIpf()
{
	pDlg->SetPartWorkProgressGraphic(100, 100);
	m_patchinfo.clear();
	m_PatchVersionList.clear();

	m_fullDownload = false;

	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_patchfolder + "*" + PREFIX_IPF);

	while(bResult)
	{
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		if(m_IpfManager.isIPF(g_patchfolder + strFileName))
		{
			std::string filename = strFileName.GetBuffer();

			if(filename.find("_") == -1) {
				DeleteFile(g_patchfolder + strFileName);
				continue;
			}
			std::string version(filename, 0, filename.find("_"));
	
			if(filename.length() < version.length() + 7) {
				DeleteFile(g_patchfolder + strFileName);
				continue;
			}
			std::string count(filename, version.length() + 1, 3);
			std::string num(filename, version.length() + 4, 3);

			std::map<int, PatchInfo>::iterator _itr;
			_itr = m_patchinfo.find(atoi(version.data()));
			if(_itr == m_patchinfo.end())
			{
				PatchInfo info;
				info.count = atoi(count.data());
				info.start = m_IpfManager.m_info.Start;
				info.end = m_IpfManager.m_info.End;
				info.need.resize(info.count, FALSE);
				info.need[atoi(num.data()) - 1] = TRUE;
				
				if (info.count == 1) info.down = false;
				else info.down = true;

				m_patchinfo.insert(std::pair<int, PatchInfo>(atoi(version.data()), info));
				m_PatchVersionList.push_back(atoi(version.data()));
				
				if (m_PatchStart > info.start) m_PatchStart = info.start;
			} 
			else
			{
				bool down = false;
				(*_itr).second.need[atoi(num.data()) - 1] = TRUE;

				for (int i = 0; i < (*_itr).second.count; ++i) {
					if((*_itr).second.need[i] == 0) {
						down = true;
						break;
					}
				}

				(*_itr).second.down = down;
			}
		}
	}

	m_PatchVersionList.sort();
	IsFullDownload();
}

void CCacheData::DeleteLowVersionData()
{
	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_datafolder + "*" + PREFIX_IPF);
	while(bResult) {
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		if(m_IpfManager.isIPF(g_datafolder + strFileName)) {
			if (m_IpfManager.m_info.End < m_ServerVersion) {
				DeleteFile(g_datafolder + strFileName);
			}
		}
	}

	m_DataLowVersion = m_ServerVersion;
	m_PatchStart = m_DataLowVersion + 1;
}

bool CCacheData::IsFullDownload()
{
	bool bServerVersionFind = false;
	if(m_patchinfo.empty())
	{
		DeleteLowVersionData();
		bServerVersionFind = true;
	} else {
		std::list<int>::reverse_iterator rkeyList = m_PatchVersionList.rbegin();
	
		int _version = (*rkeyList);
		if (m_ServerVersion > _version) {
			m_fullDownload = true;
		}
	}


	return m_fullDownload;
}

void CCacheData::DeletePatchFile(int name, int count)
{
	for (int i = 0; i < count; ++i)
	{
		CString FileName;
		FileName.Format("%d_%03d%03d.ipf", name, count, i+1);
		DeleteFile(g_patchfolder + FileName);
	}
}

void CCacheData::WriteCacheData()
{
	m_CacheDataMap.clear();
	pDlg->SetPartWorkProgress(" ");

	SetDataDirectoryInfo();
	SetPatchDirectoryInfo();
	SetRemoveDirectoryInfo();

	char* indata = (char *)MapViewOfFile (g_mmap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	ZeroMemory(indata, 1024*1024*8);
	char* __indata = indata;
	int progressvalue = 0;
	DWORD offset = 0;
	int size, count;
 	pDlg->SetPartWorkProgressGraphic(0, m_CacheDataMap.size());

	size = 0;
	memcpy(__indata, &size, sizeof(DWORD));
	__indata = __indata+sizeof(DWORD);

	size = m_CacheDataMap.size();
	memcpy(__indata, &size, sizeof(DWORD));		// 전체 ipf파일 수 
	__indata = __indata+sizeof(DWORD);

	offset += sizeof(DWORD) + sizeof(DWORD);

	std::map<CString, std::map<CString, CACHE_DATA>>::iterator itr = m_CacheDataMap.begin();
 	for(; itr != m_CacheDataMap.end(); ++itr)
 	{
		char* sizeset;

		size = (*itr).first.GetLength();
		memcpy(__indata, &size, sizeof(int));	// ipf파일 이름 길이
		__indata = __indata+sizeof(int);
		memcpy(__indata, (*itr).first, size);	// ipf파일 이름
		__indata = __indata+size;
		count = (*itr).second.size();
		memcpy(__indata, &count, sizeof(int));	// 해당 ipf에 속해 있는 파일 수
		__indata = __indata+sizeof(int);

		sizeset = __indata;
		__indata = __indata + sizeof(DWORD);	// offset위치를 비워둔다.
		offset += sizeof(int) + size + sizeof(DWORD) + sizeof(int);

		std::map<CString, CACHE_DATA>::iterator dataitr = (*itr).second.begin();

		for (; dataitr != (*itr).second.end(); ++dataitr)
		{
			size = dataitr->second.strDataName.GetLength();
			memcpy(__indata, &size, sizeof(int));
			__indata = __indata+sizeof(int);
 			memcpy(__indata, dataitr->second.strDataName, size);
 			__indata = __indata+size;
			offset += sizeof(int) + size;

			size = dataitr->second.strFilePath.GetLength();
			memcpy(__indata, &size, sizeof(int));
			__indata = __indata+sizeof(int);
 			memcpy(__indata, dataitr->second.strFilePath, size);
 			__indata = __indata+size;
			offset += sizeof(int) + size;
		}

		memcpy(sizeset, &offset, sizeof(DWORD));
 	}

	UnmapViewOfFile(indata);
}

bool CCacheData::SetDataDirectoryInfo()
{
	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_datafolder + "*" + PREFIX_IPF);

	while(bResult)
	{
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		if(m_IpfManager.OpenIPF(g_datafolder + strFileName) == 0)
		{
			char (*pArray)[260];
			size_t arraySize;

			m_IpfManager.GetInternalPath("", &pArray, &arraySize);

			for(unsigned int i = 0 ; i < arraySize; ++i)
			{
				SetDataDescriptor((char*)(pArray+i), strFileName, "data/" + strFileName);
			}

			delete[] pArray;
		} 
		else return false;
	}

	return true;
}

bool CCacheData::SetPatchDirectoryInfo()
{
	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_patchfolder + "*" + PREFIX_IPF);

	while(bResult)
	{
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		if(m_IpfManager.OpenIPF(g_patchfolder + strFileName) == 0)
		{
			std::string version(strFileName.GetBuffer(), 0, strFileName.Find("_"));
			if (m_dwLastPatchVersion < atoi(version.data())) {
				DeleteFile(g_patchfolder + strFileName);
			}
			else {
				char (*pArray)[260];
				size_t arraySize;

				m_IpfManager.GetInternalPath("", &pArray, &arraySize);

				for(unsigned int i = 0 ; i < arraySize; ++i) {
					SetDataDescriptor((char*)(pArray+i), strFileName, "patch/" + strFileName);
				}

				delete[] pArray;
			}
		}
		else return false;
	}

	return true;
}

bool CCacheData::SetRemoveDirectoryInfo()
{
	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_patchfolder + "*" + PREFIX_IPF);

	while(bResult)
	{
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		if(m_IpfManager.OpenIPF(g_patchfolder + strFileName) == 0)
		{
			char (*pArray)[260];
			size_t arraySize;

			m_IpfManager.GetRemoveFile("", &pArray, &arraySize);

			for(unsigned int i = 0 ; i < arraySize; ++i) {
				SetRemoveDataDescriptor((char*)(pArray+i), strFileName, "patch/" + strFileName);
			}

			delete[] pArray;
		}
		else return false;
	}

	return true;
}

void CCacheData::SetDataDescriptor(CString key, CString filename, CString path)
{
	char (*pArray)[260];
	size_t arraySize;

	m_IpfManager.GetInternalPath(key.GetBuffer(), &pArray, &arraySize);

	for(unsigned int i = 0 ; i < arraySize; ++i)
	{
		DATA_DESCRIPTOR descriptor;
		CACHE_DATA cdata(key, (char*)(pArray+i), path, m_IpfManager.m_info.End);

		std::map<CString, std::map<CString, CACHE_DATA>>::iterator itr = m_CacheDataMap.find(key);
		if(itr != m_CacheDataMap.end())
		{
			std::map<CString, CACHE_DATA>::iterator dataitr = (*itr).second.find((char*)(pArray+i));
			if(dataitr != (*itr).second.end())
			{
				if((*dataitr).second.strVersion < m_IpfManager.m_info.End) {
					(*dataitr).second.strFilePath = path;
					(*dataitr).second.strVersion = m_IpfManager.m_info.End;
				}
			}
			else
			{
				//if((*dataitr).second.strVersion < m_IpfManager.m_info.End) {
					(*itr).second.insert(std::pair<CString, CACHE_DATA>(cdata.strDataName, cdata));
				//}
			}
		}
		else
		{
			std::map<CString, CACHE_DATA> newmap;
			newmap.insert(std::pair<CString, CACHE_DATA>(cdata.strDataName, cdata));
			m_CacheDataMap.insert(std::pair<CString, std::map<CString, CACHE_DATA>>(key, newmap));
		}
	}

	delete[] pArray;
}

void CCacheData::SetRemoveDataDescriptor(CString key, CString filename, CString path)
{
	char (*pArray)[260];
	size_t arraySize;

	m_IpfManager.GetRemoveFile(key.GetBuffer(), &pArray, &arraySize);

	for(unsigned int i = 0 ; i < arraySize; ++i)
	{
		std::string filename = (char*)(pArray+i);

		std::map<CString, std::map<CString, CACHE_DATA>>::iterator itr = m_CacheDataMap.find(key);
		if(itr != m_CacheDataMap.end())
		{
			std::map<CString, CACHE_DATA>::iterator dataitr = (*itr).second.find((char*)(pArray+i));
			
			if(dataitr != (*itr).second.end()) {
				if((*dataitr).second.strVersion < m_IpfManager.m_info.End) {
					(*itr).second.erase(filename.c_str());
				}
			} else {
				(*itr).second.erase(filename.c_str());
			}
		}
	}

	delete[] pArray;
}

void CCacheData::SetReleaseVersion(DWORD version)
{
	m_dwReleaseVersion = version;

	CFile f;
	CString str;
	CFileException e;
	if(!f.Open(g_gameRootPath + RELEASE_REVISION_FILE_NAME, CFile::modeCreate | CFile::modeWrite, &e))
	{
		TRACE(_T("File could not be opened %d\n"), e.m_cause);
	}

	str.Format("%d", version);
	f.Write(str.GetBuffer(), str.GetLength());
	f.Close();

	pDlg->SetUpdateCurrentRevision(m_dwReleaseVersion);
}

void CCacheData::SetReleaseVersionNS(DWORD version)
{
	m_dwReleaseVersion = version;
}

void CCacheData::OptimizeAllIpfFiles()
{
	CFileFind cFileFinder;
	CString strFileName;
	static BYTE buf[1024*1024];
	int totalCount = 0;
	pDlg->SetTotalProgress(0);

	std::map<CString, std::map<CString, CACHE_DATA>>::iterator CacheIter;
	for(CacheIter = m_CacheDataMap.begin(); CacheIter != m_CacheDataMap.end(); ++CacheIter)
	{
		std::map<CString, CACHE_DATA> cachedata = (*CacheIter).second;
		CIpfManager _newIPF, _oldIPF;
		strFileName = (*CacheIter).first;
		_oldIPF.OpenIPF(g_datafolder + strFileName);
		if (_oldIPF.m_info.End < m_dwLastPatchVersion)
		{
			strFileName.Replace(".ipf", ".temp");
			_newIPF.CreateIPF(g_datafolder + strFileName);
			{
				std::map<CString, CACHE_DATA>::iterator dataitr = cachedata.begin();
				int partCount = 0;
				pDlg->SetPartWorkProgressGraphic(partCount, cachedata.size());

				for(; dataitr != cachedata.end(); ++dataitr)
				{
					while(pDlg->IsPause()) {Sleep(1);}

					if(pDlg->IsOptimizeStop())
						break;

					std::map<CString, std::map<std::string, DATA_DESCRIPTOR>>::iterator Allheader;
					Allheader = AllIpfDataHeader.find((*dataitr).second.strFilePath);

					if(Allheader == AllIpfDataHeader.end())
					{
						m_IpfManager.OpenIPF(g_gameRootPath + (*dataitr).second.strFilePath);
						AllIpfDataHeader.insert(std::pair<CString, std::map<std::string, DATA_DESCRIPTOR>>((*dataitr).second.strFilePath, m_IpfManager.m_CentralDirectoryMap));
						Allheader = AllIpfDataHeader.find((*dataitr).second.strFilePath);
					}

					_newIPF.WriteIpfData((*dataitr).second, (*Allheader).second);
					pDlg->SetPartWorkProgressGraphic(++partCount, cachedata.size());
					(*dataitr).second.strFilePath.Format("data/%s", (*CacheIter).first);
				}

				_newIPF.WriteIpfData((WORD)m_dwLastPatchVersion);
				_newIPF.Close();

				(*CacheIter).second = cachedata;
			}
		}

		pDlg->SetTotalProgress((100*(++totalCount)) / (m_CacheDataMap.size() + 1));

		while(pDlg->IsPause()) {Sleep(1);}

		if(pDlg->IsOptimizeStop())
		{
			DeleteFile(g_datafolder + strFileName);
			break;
		}
	}

	if(pDlg->IsOptimizeStop() == false) {
		m_DataLowVersion = m_DataHighVersion = m_dwLastPatchVersion;
	}

	AllIpfDataHeader.clear();
}