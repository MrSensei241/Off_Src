#pragma once

struct KOR_BASE_DATA {
	imcDic::SENTENCE_ID		ID;
	bool		isUse;
	std::string koreanString;  // 한국어 원문
	std::string category;  // 카테고리명
};

class CBaseDic
{
public:
	CBaseDic(void);
	~CBaseDic(void);

public:
	void				Init();
	void				LoadBaseData(imcXml::CXmlDoc& doc);

	const KOR_BASE_DATA&	FindDicData(const std::string kor_data, bool* pOutExist) const;
	imcDic::SENTENCE_ID		GetNextID(std::string categoryName);
	//imcDic::SENTENCE_ID		GetLastID(std::string categoryName) const;
	void	ParsingClMsgData(const imcXml::CXmlDoc& doc);
	std::string	GetRecentDIcIDByPlacedKey(std::string placedkey);
	std::unordered_map<std::string, KOR_BASE_DATA>* GetBaseData(){ return &m_baseData; };
	std::map<std::string, std::string>* GetBasePlacedKeyMap(){ return &m_basePlacedKeyMap; };

private:
	std::unordered_map<std::string, KOR_BASE_DATA>	m_baseData;
	//imcDic::SENTENCE_ID m_lastID;
	std::map<std::string, int>	m_lastIDMap;

	std::map<std::string, std::string>	m_basePlacedKeyMap; // <placedKey,ID>
};

CBaseDic* GetBaseDic();
