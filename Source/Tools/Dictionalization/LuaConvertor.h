#pragma once


#include "imcDic.h"
#include "GlyphExporter.h"
#include "GlyphParser.h"

struct AUTO_ADDED_DATA {

	std::string className;
	std::string data;

};

class CLuaConvertor
{
public:
	CLuaConvertor(void);
	~CLuaConvertor(void);

public:
	std::string	StartConvert(const std::string& fullfileName); 
	std::string ConvertSimpleHangul(std::string original_string, bool* m_bIsNeedBuild);
	void		SaveCliMsg();
	void SaveClientMessage(imcXml::CXmlDoc& s_messageDoc);
	void AddClientMessage(imcXml::CXmlDoc& s_messageDoc, const char * category, const char * className, const char * data);
	int GetMaxAttribute(imcXml::CXmlNode elem, const char * attrName);

private:
	bool		IsDialogSentnce(const std::string& sentence);
	std::string ConvertSentence(std::string sentence, std::string::size_type findOffSet);
	std::string ConvertDicID(std::string sentence, std::string::size_type start, std::string::size_type& end, char findChar);


private:
	CTextExporter	m_exporter;
	CTextLineParser	m_parser;
	imcDic::IDic*	m_pDic;
	bool			m_bIsNeedBuild;
	std::vector<AUTO_ADDED_DATA> newdatas;
};

