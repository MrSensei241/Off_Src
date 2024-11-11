#include "StdAfx.h"
#include "BaseDic.h"
#include "boost/tokenizer.hpp"
#include "imcDateTime.h"

CBaseDic::CBaseDic(void)
{
	m_lastIDMap.clear();
	m_basePlacedKeyMap.clear();
}


CBaseDic::~CBaseDic(void)
{
	
}

void CBaseDic::Init()
{

	std::vector<imcDic::POSSIBLE_COUNTRY> *pPossibleLan = imcDic::GetPossibleLanguages();
	pPossibleLan->clear();

 	std::string dirName = imc::GetDataPath("",""); // 기존 원문 데이터는 결국 읽어와야 할듯
 	dirName += "language";
 
 	imcXml::CXmlDoc doc;
 	std::string baseFullFile = dirName + "\\DicIDTable.xml";
 	bool bRet = imcXml::Load(baseFullFile.c_str(), doc);	
 	if (bRet)
 	{		
 		LoadBaseData(doc);	
 	} else {
 		std::wcout << "error XML name : " << baseFullFile.c_str() << " load failed" << std::endl;
 	}

}

std::string CBaseDic::GetRecentDIcIDByPlacedKey(std::string placedkey)
{
	auto finder = m_basePlacedKeyMap.find(placedkey);

	if (finder == m_basePlacedKeyMap.end())
		return finder->second;
			
	return "";

}

void SplitWord(const std::string &pattern, const std::string &subject, std::vector<std::string>* pOutGroup)
{
	std::string::size_type pattern_length = pattern.length();
	std::string::size_type beginpos = 0;
	std::string::size_type endpos = subject.find(pattern);
	while (endpos != std::string::npos)
	{
		pOutGroup->emplace_back(subject.substr(beginpos, endpos - beginpos));
		beginpos = endpos + pattern_length;
		endpos = subject.find(pattern, beginpos);
	}
	pOutGroup->emplace_back(subject.substr(beginpos));
}

void CBaseDic::LoadBaseData(imcXml::CXmlDoc& doc) //doc : DicIDTable.xml
{

	imcXml::CXmlNode pDicNode = doc.FirstChildElement("DicIDTable");

	if (!pDicNode)
		return;

	for (imcXml::CXmlNode pNodeIndex = pDicNode->FirstChildElement("dic_data"); pNodeIndex; pNodeIndex = pNodeIndex->NextSiblingElement("dic_data"))
	{
		imcXml::CXmlNode pElement = pNodeIndex->ToElement();


		KOR_BASE_DATA data;
	
		data.ID = pElement->Attribute("ID");
		data.isUse = pElement->IntAttribute("IsUse") == 1 ? true : false;
		data.koreanString = pElement->Attribute("kr");
		data.category = pElement->Attribute("Category");

		m_baseData.insert(std::make_pair(data.koreanString, data));



		const char* pFileNameWithKey = pElement->Attribute("FilenameWithKey");
		std::string fileNameWithKeys;
		if (pFileNameWithKey)
			fileNameWithKeys = pFileNameWithKey;

		if (fileNameWithKeys != "")
		{
			std::vector<std::string> pOutGroup;
			SplitWord("%%%", pFileNameWithKey, &pOutGroup);
			for (auto it = pOutGroup.begin(); it != pOutGroup.end(); it++)
			{
				std::string placedkey = *it;
				m_basePlacedKeyMap[placedkey] = data.ID; // 가장 최근 키만 저장하기 위해 일부러 덮어쓴다

				//LOG_LINE("%s %s", placedkey.c_str(), data.ID.c_str());

			}
		}
		
		

		//data.id에서 숫자만 파싱한거랑, map안의 m_lastID중 맥스를 넣어야함.

		auto iter = m_lastIDMap.find(data.category);
		int lastidofcategory = 0;

		if (iter != m_lastIDMap.end()){
			lastidofcategory = iter->second;

			std::string idtempstr = data.ID;

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep("_");
			tokenizer tokens(idtempstr, sep);
			std::string substr;
			for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
			{
				substr = *tok_iter;
			}

			int oldid = imc::StrToINT(substr.c_str());

			iter->second = max(lastidofcategory, oldid);

		}
		else{
			m_lastIDMap.insert(std::make_pair(data.category,1));
		}

	}

	LOG_LINE("Base Data size : %d", m_baseData.size());
	LOG_LINE("Base Placed Key size %d", m_basePlacedKeyMap.size());
 }


 const KOR_BASE_DATA& CBaseDic::FindDicData( const std::string kor_data, bool* pOutExist ) const
{
	auto findIter = m_baseData.find(kor_data);
	if ( findIter != m_baseData.end() ) {
		*pOutExist = true;
		return findIter->second;
	}

	*pOutExist = false;
	static const KOR_BASE_DATA data;
	return data;
}

imcDic::SENTENCE_ID CBaseDic::GetNextID(std::string categoryName)
{
	auto iter = m_lastIDMap.find(categoryName);
	int lastidofcategory = 0;

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	
	char date[100] = { 0 };
	sprintf(date, "%d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);

	std::string currentDateStr = date;
	std::string idtempstr;

	if (iter != m_lastIDMap.end()){

		iter->second++;
		char tempchar[10] = { 0 };
		sprintf(tempchar, "%06d", iter->second);
		idtempstr = categoryName + "_" + currentDateStr + "_" + tempchar;
	
	}
	else{
		m_lastIDMap.insert(std::make_pair(categoryName, 1));
		idtempstr = categoryName + "_" + currentDateStr + "_" + "000001";
	}

	return idtempstr;
}

CBaseDic* GetBaseDic()
{
	static CBaseDic g_baseDic;
	return &g_baseDic;
}

// void CBaseDic::LoadBaseData(imcXml::CXmlDoc& doc ) //잘 되던 옛날거
// {
// 
// 	_SetPossibleDic(doc); // 이거 없어도 되고.
// 
// 	imcXml::CXmlNode pRoot = doc.RootElement();
// 
// 	for ( imcXml::CXmlNode pDicNode = pRoot->FirstChildElement() ; pDicNode ; pDicNode = pDicNode->NextSiblingElement() )
// 	{
// 		imcXml::CXmlNode pElem = pDicNode->ToElement();
// 		IMC_ASSERT(pElem);
// 		const char * buf = pElem->Value();
// 
// 		imcXml::CXmlNode pKeyElem = pDicNode->ToElement();
// 
// 		if (imc::StrIsSame(pKeyElem->Value(), "dic_data")) {
// 
// 			imcXml::CXmlNode pElement = pDicNode->ToElement();
// 
// 			KOR_BASE_DATA data;
// 			imcXml::GetXmlElemAttr(pElement, "ID", &data.ID);
// 			imcXml::GetXmlElemAttr(pElement, "IsUse", &data.isUse);
// 
// 
// 			std::vector<std::string> *pLanguages = imcDic::GetPossibleLanguages();
// 
// 			if (pLanguages->size() > 0) // 기존 사전 데이터가 있다. 그렇다면 기존에 있는 언어 수만큼 세팅
// 			{
// 				for (unsigned int i = 0; i < pLanguages->size(); i++)
// 				{
// 					std::string iterstr = pLanguages->at(i);
// 					std::string tempstr = pElement->Attribute(iterstr.c_str());
// 
// 					if (tempstr != "")
// 					{
// 						data.translateDatas.push_back(tempstr);
// 					}
// 					else{
// 						data.translateDatas.push_back("");
// 					}
// 				}
// 			}else{
// 				LOG_LINE("없으면 이 함수에 들어올리가 있나");
// 			}
// 
// 			data.fileName	= pElement->Attribute("Filename");			
// 			m_baseData.insert(std::make_pair(data.translateDatas[0], data));
// 			m_lastID = imc::Max(m_lastID, data.ID);
// 			
// 		}
// 	}
// }


// imcDic::SENTENCE_ID CBaseDic::GetLastID(std::string categoryName) const
// {
// 	//return m_lastID;
// 
// 	auto iter = m_lastIDMap.find(categoryName);
// 	int lastidofcategory = 0;
// 
// 	std::string idtempstr = "NotExist";
// 
// 	if (iter != m_lastIDMap.end()){
// 
// 		idtempstr = categoryName + "_" + currentDateStr + "_" + imc::ToString(iter->second);
// 
// 	}
// 
// 	return idtempstr;
// 
// }



// void _SetPossibleDic(const imcXml::CXmlDoc& doc) // translatedData.xml의 카테고리 종류를 읽어서 현재 XML에 저장된 국가 이름들을 사용 가능한 언어 리스트로 세팅하는 부분. txt로 바꾸면 사라져야 함.
// {
// 
// 	std::vector<imcDic::POSSIBLE_COUNTRY> *pPossibleLan = imcDic::GetPossibleLanguages();
// 
// 	pPossibleLan->clear();
// 
// 	imcXml::CXmlNode pDicNode = doc.FirstChildElement("dictionary");
// 
// 	//if (pDicNode == false)
// 		//return;
// 
// 	imcXml::CXmlNode pNodeIndex = pDicNode->FirstChildElement("dic_data");
// 
// 	imcXml::CXmlNode pElement = pNodeIndex->ToElement();
// 
// 	for (imcXml::CXmlAttribute att = pElement->FirstAttribute(); att; att = att->Next())
// 	{
// 		std::string tempCountry = att->Name();
// 
// 		if (tempCountry != "ID" && tempCountry != "IsUse" && tempCountry != "Filename")
// 			pPossibleLan->push_back(tempCountry);
// 	}
// }