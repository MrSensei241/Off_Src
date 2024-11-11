#include "stdafx.h"
#include "FunctionExtracter.h"
#include "imcDateTime.h"
#include "imcSortVector.h"


std::string					g_DirName;
int							g_Rivision;
std::string					g_editor;
std::string					g_lastDate;


void ParseCmdLine( int argc, char * argv[] )
{
	imccmdline::Reset(argc, argv);
	imc::CVector<std::string> extnames;
	for (int j = 1 ; j < argc ; j ++)
	{
		extnames.AddToTail(argv[j]);
	}

	imc::RegisterEffectCmds(extnames);
}

class CFindFile
{
public:
	CFindFile()
	{
		m_findFile = INVALID_HANDLE_VALUE;
	}

	~CFindFile()
	{
		FindClose(m_findFile);
		m_findFile = NULL;
	}

	bool FindFirstFile(const char* path)
	{
		m_findFile = ::FindFirstFile(path, &m_findData);
		if (m_findFile == INVALID_HANDLE_VALUE)
			return false;
		else
			return true;
	}

	bool FindNextFile()
	{
		if (m_findFile == NULL )
			return false;
		if (::FindNextFile(m_findFile, &m_findData) == FALSE)
			return false;
		return true;
	}

	ULONGLONG GetFileSize()
	{
		ULARGE_INTEGER nFileSize;
		nFileSize.LowPart = m_findData.nFileSizeLow;
		nFileSize.HighPart = m_findData.nFileSizeHigh;
		return nFileSize.QuadPart;
	}

	void GetFileName(char* fileName)
	{
		strcpy(fileName, m_findData.cFileName);
	}

	bool IsDirectory()
	{
		DWORD dwAttrib = m_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		if(dwAttrib == FILE_ATTRIBUTE_DIRECTORY)
			return true;
		else
			return false;
	}

	bool IsDots()
	{
		bool result = false;
		if (IsDirectory())
		{
			if (m_findData.cFileName[0] == '.')
			{
				if ((m_findData.cFileName[1] == '\0') || 
					(m_findData.cFileName[1] == '.' && m_findData.cFileName[2] == '\0'))
					result = true;
			}
		}
		return result;
	}

	bool IsSvnDirectory()
	{
		if (_stricmp(".svn", m_findData.cFileName) == 0)
			return true;
		else
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



class CFileFinder
{
public:
	void FindFile(const char* path, bool subdirectory = false)
	{
		::SetCurrentDirectory(path);

		if (subdirectory) {
			if (m_filefind.FindFirstFile("*.*") == false)
				return;
		}
		else
		{
			if (m_filefind.FindFirstFile("*.*") == false)
				return;
		}

		imc::CVector<WIN32_FIND_DATA> directory;
		while (TRUE)
		{
			if (m_filefind.IsDots() == false && m_filefind.IsSvnDirectory() == false)
			{
				if (m_filefind.IsDirectory())
					directory.AddToTail(m_filefind.GetFindData());
				else {

					if (false == subdirectory){
						OnFindFile(m_filefind.GetFindData());					
					}
				}
			}

			if (m_filefind.FindNextFile() == false)
				break;
		}

		char currentDir[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, currentDir);

		char moveDir[MAX_PATH];
		WIN32_FIND_DATA findData;
		int count = directory.Count();

		for (int i = 0; i < count; i++)
		{
			findData = directory[i];
			sprintf(moveDir, "%s\\%s", currentDir, findData.cFileName);
			FindFile(moveDir, subdirectory);
			::SetCurrentDirectory("..");
		}
	}

protected:
	virtual void OnFindFile(WIN32_FIND_DATA& findData) {

		if (strstr(findData.cFileName, ".eft")){
			::GetCurrentDirectory(2048, tmp);
			strcat(tmp , "\\");
			strcat(tmp, findData.cFileName);
			imc::TestMergeEffect(tmp, findData.cFileName);			
		}

	};

private:
	CFindFile m_filefind;
	char	tmp[2048];
};




void ALL_CPP_PRINT(const char * searchpath)
{

	CFileFinder	tmp;
	tmp.FindFile(searchpath, false);

}

namespace StrID
{
	DECLARE_STRINGID(Params);
	DECLARE_STRINGID(Returns);
	DECLARE_STRINGID(Description);
	DECLARE_STRINGID(LastDate);
	DECLARE_STRINGID(LastRevision);
	DECLARE_STRINGID(LastEditor);
};

int main(int argc, char* argv[])
{
	ParseCmdLine( argc, argv );

	imc::CVector<std::string>* list = imc::GetCmdsList();
	printf("\n 현재 사용 하는 이펙트 명령 목록은");
	for (int j = 0 ; j < list->Count() ; j ++)
	{
		printf("\n[%s]", list->Element(j).c_str());
	}

	printf("\n 입니다. 변경을 원할시에는 .exe -forkptcl -particle -test -abcd\n의 형식으로 명령을 추가해주세요.");

	std::string dataPath = imc::GetDataPath("sprite\\", "");
	ALL_CPP_PRINT(dataPath.c_str());

	printf("\n\n\nACT파일 변환 완료.\n\n");

	return 0;
}
