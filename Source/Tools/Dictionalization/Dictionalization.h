#pragma once

#include "LuaConvertor.h"
#include "XMLConvertor.h"
#include <set>

typedef std::map<std::string, __int64> XML_STAT_LIST;

enum FILE_TYPE {
	FT_XML,
	FT_LUA,
	FT_ERROR,
};

class CDictionalization
{
public:
	CDictionalization(void);
	~CDictionalization(void);

public:
	bool	Init();
	void	Exit();
	//CLuaConvertor GetLuaConvertor(){ return m_luaConvertor; };

public:
	//collectedfilelist : 중복해서 수집하지 않기 위핸 임시 저장장소
	//passedfilelist : 파일이 tmp파일과 비교했을 떄 수정사항이 없기 때문에 패스한 파일들
	void		MakeDictionary(const char* folderOrFileName, bool disableLua = true, bool ignoreFileExtension = false, std::string categoryName = "translatedData", std::set<std::string>* collectedfilelist = nullptr, std::list<std::string>* passedfilelist = nullptr);

private:
	void		CollectFileName(const char* folderOrFileName, std::vector<std::string>* pOutGroup);
	FILE_TYPE	GetFileType(const std::string& fullFilePath);

private:
	void		ConvertXML(const std::string& filename, std::string categoryName);
	void		ConvertLUA(const std::string& filename);

private:
	void		CollectExistData(const std::string& extname);
	void		DeleteNotCreatedFile();

public:
	void		SaveLastDicFileTime();
	void		LoadLastDicFileTime();
	void		SaveWholeDicIDFile();
	void		LoadWholeDicIDFile();

private:
	CXMLConvertor	m_xmlConvertor;
	CLuaConvertor	m_luaConvertor;
	std::vector<std::string> m_noDicFile;

private:
	std::vector<std::string>	m_makedata;
	std::vector<std::string>	m_existData;

	XML_STAT_LIST m_lastfiletimemap;
};