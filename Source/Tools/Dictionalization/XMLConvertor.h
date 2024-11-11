#pragma once

#include <set>
#include <map>

struct GENSET;

struct GENSET {

	std::string mapname;
	std::set<std::string> genset;

};

struct ORISTR_N_DICID {

	std::string original;
	std::string dicidstr;
};

struct EACH_FILE_DICIDSET {

	std::string filename;
	std::vector<ORISTR_N_DICID> datavector;

};

class CDic;
class CXMLConvertor
{
public:
	CXMLConvertor(void);
	~CXMLConvertor(void);

public:
	void Init();
	std::string	StartConvert(const std::string& dataFileName);
	void	SetCategoryName(std::string newCategory){ m_categoryName = newCategory; };
	std::string	GetCategoryName(){ return m_categoryName; };
	std::vector<EACH_FILE_DICIDSET>* GetDicXmlVector(){ return &m_dicxmlvector; };
	std::map<std::string, std::map<std::string, std::string>>* GetBeforeDicXmlMap(){ return &m_beforedicxmlMap; };

private:
	bool	MakeConvertDoc(std::vector<ORISTR_N_DICID>* eachfiledicidvector);
	void	ConvertNodeRecursive(imcXml::CXmlNode pNode, std::vector<ORISTR_N_DICID>* eachfiledicidvector, std::string treestring);

private:
	void	AttrGroupToChange(imcXml::CXmlNode elem, const imcXml::CXmlAttribute pAttr, std::vector<ORISTR_N_DICID>* eachfiledicidvector, std::string treestring);
	std::string		SaveDic(const std::string& dicFilename);
	imcDic::SENTENCE_ID		CalcDicID(std::string categoryName, std::string strValue);

private:
	imcDic::IDic*	m_pDic;
	imcXml::CXmlDoc m_doc;
	bool			m_bIsExist; // �ѱ��� �� �÷��̶� �ִ°�
	
	std::string m_categoryName; // ���߿� .txt������ ���� �� ���ϸ����� ���δ�.
	std::string m_nowConvertFileName; 
	std::map<std::string, std::string> m_dialogTextLvMap; // ����Ʈ �������� ������ �и��� ���� ��
	std::vector<EACH_FILE_DICIDSET> m_dicxmlvector; // map<�����̸�, map<�ѱ������, dicid��> >
	std::map<std::string, std::map<std::string, std::string>> m_beforedicxmlMap; // ���� ���.


	std::vector<GENSET> m_dialogTextGenTypeMap;
	int duplicateidcount;

	int m_questDivideLevel;
};

