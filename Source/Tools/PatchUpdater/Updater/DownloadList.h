#pragma once
#include "CacheData.h"
#include <list>

class DownloadListMaker
{
public:
	explicit DownloadListMaker(CCacheData& CacheDataClass);

	virtual bool GetList(const DWORD dwLastRevision, const CString& rFileName, UPDATE_HISTORY_LIST& vecHistory) = 0;
	virtual bool InterpretRevisionString(const DWORD dwLastRevision, const CString& strReadString, UPDATE_HISTORY_LIST& vecHistory) = 0;

protected:
	CCacheData& m_CacheDataClass;
};

class PartialListMaker : public DownloadListMaker
{
public:
	explicit PartialListMaker(CCacheData& CacheDataClass) : DownloadListMaker(CacheDataClass) {}

	virtual bool GetList(const DWORD dwLastRevision, const CString& rFileName, UPDATE_HISTORY_LIST& vecHistory);
	virtual bool InterpretRevisionString(const DWORD dwLastRevision, const CString& strReadString, UPDATE_HISTORY_LIST& vecHistory);

protected:
	//DWORD dwStart
};

class ReleaseListMaker : public DownloadListMaker
{
public:
	explicit ReleaseListMaker(CCacheData& CacheDataClass) : DownloadListMaker(CacheDataClass) {}

	virtual bool GetList(const DWORD dwLastRevision, const CString& rFileName, UPDATE_HISTORY_LIST& vecHistory);
	virtual bool InterpretRevisionString(const DWORD dwLastRevision, const CString& strReadString, UPDATE_HISTORY_LIST& vecHistory);

protected:
	DWORD dwLastRelease;
	DWORD dwCount;
};

class PrepareListMaker : public DownloadListMaker
{
public:
	explicit PrepareListMaker(CCacheData& CacheDataClass) : DownloadListMaker(CacheDataClass) {}

	virtual bool GetList(const DWORD dwLastRevision, const CString& rFileName, UPDATE_HISTORY_LIST& vecHistory);
	virtual bool InterpretRevisionString(const DWORD dwLastRevision, const CString& strReadString, UPDATE_HISTORY_LIST& vecHistory);

protected:
	virtual bool ChangeFileName(const CString& rFileName);

	std::list<std::string> m_prepareList;
};
