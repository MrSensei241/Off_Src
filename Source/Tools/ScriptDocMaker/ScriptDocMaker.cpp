#include "stdafx.h"
#include "FunctionExtracter.h"
#include "imcDateTime.h"
#include "imcSortVector.h"

#include "imcXml.h"

#pragma  comment(lib, "wsock32.lib")

std::string					g_DirName;
int							g_Rivision;
std::string					g_editor;
std::string					g_lastDate;


void ParseCmdLine( int argc, char * argv[] )
{
	imccmdline::Reset(argc, argv);
	g_Rivision = imccmdline::ParmValue("-VERSION", 1);
	g_editor = imccmdline::ParmValue("-EDITOR", "None");
	g_lastDate = imccmdline::ParmValue("-DATE", "");
	g_DirName			= imccmdline::ParmValue("-DIRNAME", "");
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

		if (strstr(findData.cFileName, ".cpp")){

			::GetCurrentDirectory(2048, tmp);
			strcat(tmp , "\\");
			strcat(tmp, findData.cFileName);
			imc::ExtactFunctions(tmp, findData.cFileName);			
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
	DECLARE_STRINGID(Comment);
	DECLARE_STRINGID(LastDate);
	DECLARE_STRINGID(LastRevision);
	DECLARE_STRINGID(LastEditor);
};

int main(int argc, char* argv[])
{
	ParseCmdLine( argc, argv );

	std::string fullpath = imc::GetBinPath();
	fullpath += "\\\\script_list.xml";

	imcIES::RegisterClassFromXML(fullpath.c_str());
	imcIES::IClassList* pClsList = imcIES::GetClassList("ScriptList");

	/*
	struct FUNCTION_INFO
	{
	imc::CStringID	Name;
	imc::CStringID	Params;
	imc::CStringID	Retuns;
	imc::CStringID	Description;
	imc::CStringID	DateTime;
	int				SvnRevision;
	};
	*/

	imc::CMap<imc::_STRING_ID, imc::FUNCTION_INFO*>	FuncList;
	imc::CStringID	tmp;

	imcIES::IClass* pCls;
	int j;
	int cnt = 0;
	if (pClsList)
		cnt = pClsList->Count();

	for (j = 0 ; j < cnt ; j ++)
	{
		pCls = pClsList->GetByIndex(j);
		
		imc::FUNCTION_INFO* pInfo = new imc::FUNCTION_INFO;
		pInfo->Name = pCls->GetName();
		pCls->Get(StrID::Params, pInfo->Params);
		if (strlen(pInfo->Params.c_str()) == 0)
			pInfo->Params= imc::CStringID("-");

		pCls->Get(StrID::Returns, pInfo->Returns);
		if (strlen(pInfo->Returns.c_str()) == 0)
			pInfo->Returns = imc::CStringID("-");
		pCls->Get(StrID::Description, pInfo->Description);
		if (false == pCls->Get(StrID::Comment, pInfo->comment)){
			pInfo->comment = "";
		}

		pCls->Get(StrID::LastDate, pInfo->LastDate);
		pCls->Get(StrID::LastRevision, pInfo->LastRevision);
		pCls->Get(StrID::LastEditor, pInfo->LastEditor);

		FuncList.Insert(pInfo->Name.ID(), pInfo);
	}



	imc::CIMCDateTime	imcDate		= imc::CIMCDateTime::GetCurrentTime();
	char dateStr[256];
	sprintf(dateStr, "%d-%d-%d", imcDate.GetYear(), imcDate.GetMonth(), imcDate.GetDay());
	printf("\n%s", dateStr);
	imc::CStringID	TodayStr = imc::CStringID(dateStr);
	imc::CStringID	EditorStr = imc::CStringID(g_editor.c_str());

	char path[4096];
	char temp[4096];
	imc::StrSplitParentChildPathString(imc::GetDataRootPath(), path, temp);

	std::string sourcepath = path;
	if (g_DirName == "")
		sourcepath += "Source";
	else
		sourcepath += g_DirName.c_str();

	ALL_CPP_PRINT(sourcepath.c_str());


	imc::CVector<imc::FUNCTION_INFO*>	ResultList;

	imc::FUNCTION_LIST* pList = imc::GetFuncInfoList();

	imc::FUNCTION_INFO* pInfo;
	imc::FUNCTION_INFO* pOldInfo;
	for (j = 0 ; j < pList->Count(); j ++)
	{
		 pInfo = pList->Element(j);

		 bool funcchanged = false;
		 int index = FuncList.Find(pInfo->Name.ID());
		 if (index == FuncList.InvalidIndex())
		 {
			 funcchanged = true;
			pOldInfo = NULL;
		 } else {
			pOldInfo = FuncList.Element(index);
			if (
				(false == imc::StrIsSame(pInfo->Params.c_str(), pOldInfo->Params.c_str()))
				|| (false == imc::StrIsSame(pInfo->Returns .c_str(), pOldInfo->Returns .c_str()))
				|| (false == imc::StrIsSame(pInfo->Description.c_str(), pOldInfo->Description.c_str()))
				){
					funcchanged = true;
			}		 
		 }

		 if (funcchanged){
			pInfo->LastDate = TodayStr;
			pInfo->LastEditor = EditorStr;
			pInfo->LastRevision = g_Rivision;
		 } else {
			pInfo->LastDate = pOldInfo->LastDate;
			pInfo->LastEditor = pOldInfo->LastEditor;
			pInfo->LastRevision = pOldInfo->LastRevision;
		 }

		 if (pOldInfo)
			 pInfo->comment = pOldInfo->comment;
	}

	imc::FUNCTION_INFO	tmpf;
	imc::CMap<imc::_STRING_ID, imc::CSortVector<imc::FUNCTION_INFO>*> MapListByFileName;
	for (j = 0 ; j < pList->Count(); j ++)
	{
		pInfo = pList->Element(j);

		imc::CSortVector<imc::FUNCTION_INFO>* list = NULL;
		int index = MapListByFileName.Find(pInfo->FileName.ID());
		if (index == MapListByFileName.InvalidIndex())
		{
			list = new imc::CSortVector<imc::FUNCTION_INFO>;
			MapListByFileName.Insert(pInfo->FileName.ID(), list);
		} else {
			list = MapListByFileName.Element(index);
		}

		memcpy(&tmpf, pInfo, sizeof(imc::FUNCTION_INFO));
		list->Insert(tmpf);
	}

	tinyxml2::XMLDocument doc;
	imcXml::InitDocument(doc);
	tinyxml2::XMLElement * root = imcXml::CreateChild(&doc, "ScriptList");

	int clsId = 1;
	int index = MapListByFileName.FirstInorder();
	while(true)
	{

		if (index == MapListByFileName.InvalidIndex())
			break;

		imc::CSortVector<imc::FUNCTION_INFO>*	pSortList = MapListByFileName.Element(index);
		index = MapListByFileName.NextInorder(index);

	for (j = 0 ; j < pSortList->Count(); j ++)
	{
		pInfo = &pSortList->Element(j);
		
		tinyxml2::XMLElement * pCls = imcXml::CreateChild(root, "Class");
		pCls->SetAttribute("ClassID", clsId);
		pCls->SetAttribute("ClassName", pInfo->Name.c_str());
		pCls->SetAttribute("Params", pInfo->Params.c_str());
		pCls->SetAttribute("Returns", pInfo->Returns.c_str());
		pCls->SetAttribute("Description", pInfo->Description.c_str());
		pCls->SetAttribute("Comment", pInfo->comment.c_str());
		pCls->SetAttribute("FileName", pInfo->FileName.c_str());
		pCls->SetAttribute("LastDate", pInfo->LastDate.c_str());
		pCls->SetAttribute("LastRevision", pInfo->LastRevision);
		pCls->SetAttribute("LastEditor", pInfo->LastEditor.c_str());
		
		clsId++;
	}
	
}
	imcXml::SaveByUTF8(doc, fullpath.c_str(), true);

	fullpath = imc::GetBinPath();
	fullpath += "\\\\script_list.html";
	FILE* pHtml = fopen(fullpath.c_str(), "w");
	if (pHtml == NULL)
	{
		IMC_ASSERT_EX(pHtml,"%s", fullpath.c_str());
		return 0;
	}



	imc::MakeFileHeader(pHtml);
	imc::MakeTableHeader(pHtml);


	index = MapListByFileName.FirstInorder();
	while(true)
	{

		if (index == MapListByFileName.InvalidIndex())
			break;

		imc::CSortVector<imc::FUNCTION_INFO>*	pSortList = MapListByFileName.Element(index);
		index = MapListByFileName.NextInorder(index);

		for (j = 0 ; j < pSortList->Count(); j ++)
		{
			pInfo = &pSortList->Element(j);
			imc::MakeTableInfo(pInfo, pHtml);
		}

	}

	imc::MakeFileTail(pHtml);



	

	return 0;
}
