#pragma once

class CFindFile {
public:
	CFindFile()
	{
		m_findFile = INVALID_HANDLE_VALUE;
	}

	~CFindFile()
	{
		ASSERT(INVALID_HANDLE_VALUE==m_findFile);
	}

	void Close()
	{
		if (INVALID_HANDLE_VALUE != m_findFile)
			FindClose(m_findFile);
		m_findFile = INVALID_HANDLE_VALUE;
	}

	bool FindFirstFile(const char* path)
	{
		m_findFile = ::FindFirstFile(path, &m_findData);
		if (INVALID_HANDLE_VALUE == m_findFile) {
			switch (GetLastError()) {
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				return false;
			default:
				ReportLastErrorExit("CFindFile::FindNextFile()");
			}
			return false;
		}
		return true;
	}

	bool FindNextFile()
	{
		if (NULL == m_findFile)
			return false;

		if (FALSE == ::FindNextFile(m_findFile, &m_findData)) {
			switch (GetLastError()) {
			case ERROR_NO_MORE_FILES:
				return false;
			default:
				ReportLastErrorExit("CFindFile::FindNextFile()");
			}
			return false;
		}

		return true;
	}

	bool IsDirectory()
	{
		DWORD dwAttrib = m_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		if(dwAttrib == FILE_ATTRIBUTE_DIRECTORY)
			return true;
		return false;
	}

	bool IsDots()
	{
		bool result = false;
		if (true == IsDirectory()) {
			if (m_findData.cFileName[0] == '.') {
				if ((m_findData.cFileName[1] == '\0') || 
					(m_findData.cFileName[1] == '.' && m_findData.cFileName[2] == '\0'))
					result = true;
			}
		}
		return result;
	}

	bool IsSvnDirectory()
	{
		if (0 == _stricmp(".svn", m_findData.cFileName))
			return true;
		return false;
	}

	WIN32_FIND_DATA GetFindData()
	{
		return m_findData;
	}

private:
	WIN32_FIND_DATA m_findData;
	HANDLE m_findFile;
};

class CFileFinder {
public:
	bool Find(const char* path,const char* filter)
	{
		if (FALSE == SetCurrentDirectory(path))
			ReportLastErrorExit("CFileFinder::Find()");

		if (false == m_filefind.FindFirstFile(filter)) {
			return false;
		}

		std::vector<WIN32_FIND_DATA> directory;
		while (TRUE) {
			if (false == m_filefind.IsDots() && false == m_filefind.IsSvnDirectory()) {
				if (true == m_filefind.IsDirectory())
					directory.push_back(m_filefind.GetFindData());
				else {
					if (false == OnFindFile(m_filefind.GetFindData()))
						return false;
				}
			}

			if (false == m_filefind.FindNextFile()) {
				break;
			}
		}
		m_filefind.Close();

		char currentDir[8192];
		if (0 == GetCurrentDirectory(8192, currentDir))
			ReportLastErrorExit("CFileFinder::Find()");

		char moveDir[4096];
		WIN32_FIND_DATA findData;
		int count = (int)directory.size();
		for (int i=0; i<count; i++) {
			findData = directory[i];
			sprintf_s(moveDir, "%s\\%s", currentDir, findData.cFileName);
			Find(moveDir, filter);

			if (FALSE == SetCurrentDirectory(".."))
				ReportLastErrorExit("CFileFinder::Find()");
			
			if (false == OnFindDirectory(findData))
				return false;
		}

		return true;
	}

protected:
	virtual bool OnFindFile(WIN32_FIND_DATA& findData) = 0;
	virtual bool OnFindDirectory(WIN32_FIND_DATA& findData) = 0;

protected:
	CFindFile	m_filefind;
};
