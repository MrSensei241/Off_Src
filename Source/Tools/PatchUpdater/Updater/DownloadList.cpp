#include "stdafx.h"
#include "DownloadList.h"
#include "Updater.h"

DownloadListMaker::DownloadListMaker(CCacheData& CacheDataClass): m_CacheDataClass(CacheDataClass)
{
}

bool PartialListMaker::GetList(const DWORD dwLastRevision, const CString& rFileName, UPDATE_HISTORY_LIST& vecHistory)
{
	CStdioFile file;

	if(FALSE == file.Open(rFileName, CFile::modeRead)) 
		return false;

	CString strReadString;

	while(TRUE) 
	{
		Sleep(0);

		if(false == file.ReadString(strReadString))
			break;

		if(false == InterpretRevisionString(dwLastRevision, strReadString, vecHistory))
			return false;
	}

	file.Close();

	std::map<int, PatchInfo>::iterator itr = m_CacheDataClass.m_patchinfo.begin();
	for(; itr != m_CacheDataClass.m_patchinfo.end(); ++itr)
	{
		if((*itr).second.down && ((*itr).first < m_CacheDataClass.m_dwLastPatchVersion+1))
		{
			for(int i = 0; i < (*itr).second.count; ++i)
			{
				if((*itr).second.need[i] == 0)
				{
					CString FileName;
					FileName.Format("%d_%03d%03d", (*itr).second.end, (*itr).second.count, i+1);
					UPDATE_HISTORY revHistory((*itr).second.end, 0, FileName);
					revHistory.strSavePath.Format("%s%s%s", g_patchfolder, FileName,PREFIX_IPF);
					revHistory.strFilePath.Format("%s%s%s.ipf", PARTIAL_DIRECTORY, DATA_DOWNLOAD, FileName);
					vecHistory.push_back(revHistory);
				}
			}
		}
	}

	return true;
}

bool PartialListMaker::InterpretRevisionString(const DWORD dwLastRevision, const CString& strReadString, UPDATE_HISTORY_LIST& vecHistory)
{
	if(strReadString.IsEmpty())
		return true;

	DWORD dwRevisionNum = 0;

	CString strRevisionNumber;

	std::string temp = strReadString;
	std::string filename(temp.data(), temp.find(" "));
	std::string fileCount = temp.data() + temp.find(" ") + 1;

	std::map<int, PatchInfo>::iterator itr;
	itr = m_CacheDataClass.m_patchinfo.find(atoi(filename.data()));

	if((atoi(filename.data()) > m_CacheDataClass.m_DataLowVersion ) && (itr == m_CacheDataClass.m_patchinfo.end()))
	{
		for (int i = 0; i < atoi(fileCount.data()); ++i)
		{
			CString FileName;
			FileName.Format("%s_%03d%03d", filename.data(), atoi(fileCount.data()), i+1);
 			UPDATE_HISTORY revHistory(atoi(filename.data()), 0, FileName);
 			revHistory.strSavePath.Format("%s%s%s", g_patchfolder, FileName,PREFIX_IPF);
 			revHistory.strFilePath.Format("%s%s%s.ipf", PARTIAL_DIRECTORY, DATA_DOWNLOAD, FileName);
 			vecHistory.push_back(revHistory);
		}
	}

	if(m_CacheDataClass.m_dwLastPatchVersion < atoi(filename.data()))
		m_CacheDataClass.m_dwLastPatchVersion = atoi(filename.data());

	return true;
}

bool ReleaseListMaker::GetList(const DWORD dwLastRevision, const CString& rFileName, UPDATE_HISTORY_LIST& vecHistory)
{
	CStdioFile file;

	dwLastRelease = 0;

	if(FALSE == file.Open(rFileName, CFile::modeRead)) 
		return false;

	CString strReadString;

	while(TRUE) 
	{
		Sleep(0);

		if(false == file.ReadString(strReadString))
			break;

		if(false == InterpretRevisionString(dwLastRevision, strReadString, vecHistory))
			return false;
	}

	file.Close();

	if((dwLastRelease == 0) && ((int)m_CacheDataClass.GetReleaseVersion() < m_CacheDataClass.GetFullVersion()))
	{
		CString FileName;
		FileName.Format("%d_001001.pak", m_CacheDataClass.GetFullVersion());
		UPDATE_HISTORY revHistory(m_CacheDataClass.GetFullVersion(), 0, FileName);
		revHistory.strSavePath.Format("%srelease/%s", g_gameRootPath, FileName);
		revHistory.strFilePath.Format("%s%s%s", FULL_DIRECTORY, RELEASE_DOWNLOAD, FileName);
		vecHistory.push_back(revHistory);
	}

	return true;
}

bool ReleaseListMaker::InterpretRevisionString(const DWORD dwLastRevision, const CString& strReadString, UPDATE_HISTORY_LIST& vecHistory)
{
	if(strReadString.IsEmpty())
		return true;

	DWORD dwRevisionNum = 0;

	CString strRevisionNumber;

	std::string temp = strReadString;
	std::string filename(temp.data(), temp.find(" "));
	std::string fileCount = temp.data() + temp.find(" ") + 1;

	dwLastRelease = atoi(filename.data());
	dwCount = atoi(fileCount.data());

	if((m_CacheDataClass.GetReleaseVersion() < dwLastRelease))
	{
		for (unsigned int i = 0; i < dwCount; ++i)
		{
			CString FileName;
			FileName.Format("%s_%03d%03d.pak", filename.data(), dwCount, i+1);
			UPDATE_HISTORY revHistory(dwLastRelease, 0, FileName);
			revHistory.strSavePath.Format("%srelease/%s", g_gameRootPath, FileName);
			revHistory.strFilePath.Format("%s%s%s", PARTIAL_DIRECTORY, RELEASE_DOWNLOAD, FileName);
			vecHistory.push_back(revHistory);
		}
	}
	
	return true;
}

bool PrepareListMaker::GetList(const DWORD dwLastRevision, const CString& rFileName, UPDATE_HISTORY_LIST& vecHistory)
{
	CStdioFile file;

	if(FALSE == file.Open(rFileName, CFile::modeRead)) 
		return false;

	CString strReadString;

	while(TRUE) 
	{
		Sleep(0);

		if(false == file.ReadString(strReadString))
			break;

		if(true == InterpretRevisionString(dwLastRevision, strReadString, vecHistory))
			break;
	}

	file.Close();

	ChangeFileName(rFileName);
	
	return true;
}

bool PrepareListMaker::ChangeFileName(const CString& rFileName)
{
	m_prepareList.clear();

	CFileFind cFileFinder;
	CString strFileName;
	BOOL bResult = cFileFinder.FindFile(g_patchfolder + "*" + PREFIX_ARRANGE);
	while(bResult)
	{
		bResult = cFileFinder.FindNextFile();
		strFileName = cFileFinder.GetFileName();

		m_prepareList.push_back(strFileName.GetBuffer());
	}

	CStdioFile file;

	if(FALSE == file.Open(rFileName, CFile::modeRead)) 
		return false;

	CString strReadString;

	while(TRUE) 
	{
		Sleep(0);

		if(false == file.ReadString(strReadString))
			break;

		std::string temp = strReadString;
		std::string filename(temp.data(), temp.find(" "));
		std::string fileCount = temp.data() + temp.find(" ") + 1;
		DWORD dwCount = atoi(fileCount.data());

		for (unsigned int i = 0; i < dwCount; ++i)
		{
			CString FileName;
			FileName.Format("%s_%03d%03d%s%s", filename.data(), dwCount, i+1, PREFIX_IPF, PREFIX_ARRANGE);

			std::list<std::string>::iterator it;
			it = std::find(m_prepareList.begin(), m_prepareList.end(), FileName.GetBuffer());

			if (it != m_prepareList.end())
			{
				m_prepareList.erase(it);
			}
		}
	}

	file.Close();

	std::list<std::string>::iterator it = m_prepareList.begin();
	for(; it != m_prepareList.end(); ++it)
	{
		std::string temp((*it), 0, (*it).find(PREFIX_ARRANGE));

		rename(g_patchfolder + (*it).data(), g_patchfolder + temp.data());
		DeleteFile(g_patchfolder + (*it).data());
	}

	return true;
}

bool PrepareListMaker::InterpretRevisionString(const DWORD dwLastRevision, const CString& strReadString, UPDATE_HISTORY_LIST& vecHistory)
{
	if(strReadString.IsEmpty())
		return false;

	std::string temp = strReadString;
	std::string filename(temp.data(), temp.find(" "));
	std::string fileCount = temp.data() + temp.find(" ") + 1;
	DWORD dwCount = atoi(fileCount.data());

	for (unsigned int i = 0; i < dwCount; ++i)
	{
		CString FileName;
		FileName.Format("%s_%03d%03d", filename.data(), dwCount, i+1);

		UPDATE_HISTORY revHistory(atoi(filename.data()), 0, FileName);
		revHistory.strSavePath.Format("%s%s%s%s", g_patchfolder, FileName, PREFIX_IPF, PREFIX_ARRANGE);
		revHistory.strFilePath.Format("%s%s%s.ipf", PARTIAL_DIRECTORY, DATA_DOWNLOAD, FileName);

		if(false == m_CacheDataClass.m_IpfManager.isIPF(revHistory.strSavePath))
		{
			vecHistory.push_back(revHistory);
			return true;
		}
	}

	return false;
}