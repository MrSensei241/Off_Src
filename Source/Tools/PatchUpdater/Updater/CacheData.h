#pragma once
#include "IpfManager.h"
#include "structure.h"
#include <list>

struct UPDATE_HISTORY 
{
	DWORD dwHistory;
	DWORD dwMultiNum;
	CString strFileName;
	CString strFilePath;
	CString strSavePath;

	UPDATE_HISTORY() : dwHistory(0), dwMultiNum(0) { }
	UPDATE_HISTORY(const DWORD dwHistoryNum, const DWORD dwMultiNumber = 0, const CString name = "") 
	{
		dwHistory  = dwHistoryNum;
		dwMultiNum = dwMultiNumber;
		strFileName = name;
	}
};

struct UPDATE_HISTORY_LIST
{
	UPDATE_HISTORY_LIST();
	std::vector<UPDATE_HISTORY> histories;
	std::vector<UPDATE_HISTORY> notNecessaryHistories;

	int runableIndex;

	void PrepareDownload();
	UINT size() const;
	UPDATE_HISTORY& operator[](int i);
	bool empty() const;
	void clear();
	void push_back(const UPDATE_HISTORY& history);
};

struct PatchInfo
{
	std::vector<BOOL> need;
	WORD start;
	WORD end;
	int count;
	bool down;
};

class CUpdaterDlg;
class CCacheData
{
public:
	CCacheData(void);
	~CCacheData(void);

	void WriteCacheData();
	void OptimizeAllIpfFiles();

	void SetReleaseVersion(DWORD);
	void SetReleaseVersionNS(DWORD);

	DWORD GetFullVersion() {return m_ServerVersion;}
	DWORD GetReleaseVersion() {return m_dwReleaseVersion;}
	DWORD GetLastPatchVersion() {return m_dwLastPatchVersion;}
	void SetServerVersion(DWORD version) {m_ServerVersion =  version;}
	
public:
	CUpdaterDlg* pDlg;
	std::map<CString, std::map<std::string, DATA_DESCRIPTOR>> AllIpfDataHeader;
	std::map<CString, std::vector<REMOVE_DATA_DESCRIPTOR>> RemoveDataList;
	CIpfManager m_IpfManager;

public:
	void SetDataVersion();
	void SetFullVersion();
	bool InspectDataIpf();
	void InspectPatchIpf();
	void GetDownloadDataList(UPDATE_HISTORY_LIST& list);

protected:
	void DeleteLowVersionData();
	bool IsFullDownload();
	void SetServerDataList();
	bool SetDataDirectoryInfo();
	bool SetPatchDirectoryInfo();
	bool SetRemoveDirectoryInfo();
	void SetDataDescriptor(CString key, CString filename, CString path);
	void SetRemoveDataDescriptor(CString key, CString filename, CString path);

	void DeletePatchFile(int name, int count);

public:
	std::list<CString> m_NeedDataList;
	std::list<CString> m_ServerDataList;
	std::list<int> m_PatchVersionList;
	std::map<int, PatchInfo> m_patchinfo;
	std::map<CString, std::map<CString, CACHE_DATA>> m_CacheDataMap;

	DWORD m_PatchStart;
	DWORD m_ServerVersion;
	DWORD m_DataLowVersion;
	DWORD m_DataHighVersion;
	DWORD m_dwLastPatchVersion;
	DWORD m_dwReleaseVersion;

	bool  m_fullDownload;
};
