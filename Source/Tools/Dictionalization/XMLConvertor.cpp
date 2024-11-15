#include "StdAfx.h"

#include "imcStr.h"
#include "imcDirMgr.h"
#include "BaseDic.h"

#include "imcIES.h"
//#include "imcPropertySys.h"

#include "XMLConvertor.h"
#include "boost/nowide/convert.hpp"
#include "boost/regex.hpp"
#include "boost/tokenizer.hpp"
#include "boost/bind.hpp"
#include "boost/algorithm/string/replace.hpp"

#include "GePath.h"

std::unordered_map<std::string, std::set<std::string>*> g_compColumnFileMap;

CXMLConvertor::CXMLConvertor(void)
	: m_bIsExist(false),
	m_nowConvertFileName("None"),
	m_categoryName("None")
{
	m_dialogTextLvMap.clear();
	m_dialogTextGenTypeMap.clear();
	m_beforedicxmlMap.clear();					
}


CXMLConvertor::~CXMLConvertor(void)
{
}

void CXMLConvertor::Init()
{
	// 퀘스트 레벨별 데이터 분리를 위해 여러가지 관련 클래스들 미리 로드
	std::vector<std::string> pOutGroup;
	std::string datapathdir(imc::GetDataPath("xml_mongen", "").c_str());

	auto findSize = datapathdir.find(".xml");
	if (findSize != std::string::npos){
		pOutGroup.push_back(datapathdir);
		return;
	}

	imcfile::CDirectoryFiles* pFiles = imcfile::GetDirectory()->GetFiles(datapathdir.c_str());
	if (pFiles) {
		pOutGroup = pFiles->GetFiles();
	}

	for (unsigned int index = 0; index < pOutGroup.size(); ++index) {
		std::string tempfilename = pOutGroup[index];

		if ((tempfilename.find("gentype") != std::string::npos || tempfilename.find("GenType") != std::string::npos) && tempfilename.find("dic_xml") == std::string::npos)
		{
			imcIES::RegisterClassFromXML(tempfilename.c_str());
		}
	}

	imcIES::IClassList* pClsList = imcIES::GetClassList("Map");

	for (int i = 0; i < pClsList->Count(); ++i)
	{
		imcIES::IClass* pClass = pClsList->GetByIndex(i);
		std::string mapname = pClass->GetName().c_str();
		std::string gentypename = "GenType_" + mapname;

		std::set<std::string> eachdialogset;

		imcIES::IClassList* pGenClsList = imcIES::GetClassList(gentypename.c_str());

		if (pGenClsList && pGenClsList->Count() > 0)
		{
			for (int j = 0; j < pGenClsList->Count(); ++j)
			{
				imcIES::IClass* pGenClass = pGenClsList->GetByIndex(j);
				if (pGenClass){
					std::string dialogstr = pGenClass->GetString("Dialog").c_str();

					if (dialogstr != "None")
					{
						eachdialogset.insert(dialogstr);
					}
				}
				
			}

			GENSET newset;
			newset.mapname = mapname;
			newset.genset = eachdialogset;

			m_dialogTextGenTypeMap.push_back(newset);
		}
	}

	//사전 툴 설정 파일 로드
	std::cout << "Load dictooloption.ini " << std::endl;
	TCHAR buffer[512] = { 0, };
	std::string dictooloptionPath = imc::GetBinPath("toolfiles", "") + "\\dictooloption.ini";
	GetPrivateProfileString("DICTOOL", "QuestDivideLevel", "100", buffer, 512, dictooloptionPath.c_str());
	m_questDivideLevel = atoi(buffer);
	std::cout << "Set Quest Divide Level : " << m_questDivideLevel << std::endl;



	std::cout << "Load dictionary_comp_parsing.xml " << std::endl;


	std::string dictionary_comp_parsingPath = imc::GetBinPath("toolfiles", "") + "\\dictionary_comp_parsing.xml";
	imcfile::IFileMem* pFileMem = imcfile::Load(dictionary_comp_parsingPath.c_str(), true);
	if (pFileMem == nullptr)
	{
		std::cout << "Error : Couldn't find dictionary_comp_parsing.xml" << std::endl;
		return;
	}

	imcXml::CXmlDoc doc;
		
	bool ret = doc.Parse((const char*)pFileMem->GetData());
	if (false == ret)
	{
		std::cout << "Error : Couldn't open dictionary_comp_parsing.xml" << std::endl;
		return;
	}

	imcXml::CXmlNode pDicNode = doc.FirstChildElement("CompParsing");

	if (!pDicNode)
	{
		std::cout << "Error : Couldn't open dictionary_comp_parsing.xml" << std::endl;
		return;
	}

	for (imcXml::CXmlNode pNodeIndex = pDicNode->FirstChildElement("File"); pNodeIndex; pNodeIndex = pNodeIndex->NextSiblingElement("File"))
	{
		imcXml::CXmlNode pElement = pNodeIndex->ToElement();
		const char * fileName = pElement->Attribute("Name");

		auto fileit = g_compColumnFileMap.find(fileName);

		std::set<std::string>* eachSet = nullptr;
		if (fileit == g_compColumnFileMap.end())
		{
			eachSet = new std::set < std::string >;
			g_compColumnFileMap[fileName] = eachSet;
		}
		else{
			eachSet = fileit->second;
		}

		for (imcXml::CXmlNode pSubNodeIndex = pElement->FirstChildElement("Column"); pSubNodeIndex; pSubNodeIndex = pSubNodeIndex->NextSiblingElement("Column"))
		{
			const char * columnName = pSubNodeIndex->Attribute("Name");
			eachSet->insert(columnName);
		}
	}

	imcfile::Close(pFileMem);

}



std::string CXMLConvertor::StartConvert( const std::string& dataFileName )
{
	m_bIsExist = false;
	m_doc.Clear();

	//m_doc으로 여러개의 dic_xml 만들어 쓰지 않고 하나의 파일에서 dic_xml에 대치되어 들어가던 아이디를 전부 관리하기 위해 만드는 맵
	std::vector<ORISTR_N_DICID> eachfiledicidvector;

	std::string dataFileName_full(dataFileName);

	if ( imcXml::Load(dataFileName_full.c_str(), m_doc) == false ) {
		std::cout << "error XML name : " << dataFileName << " load failed" << std::endl;
		return "";
	}

	m_nowConvertFileName = dataFileName_full;

// 	std::string convertFileName = dataFileName;
// 
// 	if (dataFileName_full.find("xml\\direction") == std::string::npos)
// 		convertFileName.erase(convertFileName.size()-4, convertFileName.size());
// 
// 	convertFileName += ".dic_xml";

	imcDic::FIND_DIC_INFO dicInfo;
	dicInfo.dataFile	= dataFileName;
	dicInfo.mode		= imcDic::DM_MAKE_DIC;
	m_pDic = imcDic::FindDic(dicInfo);

	if (MakeConvertDoc(&eachfiledicidvector) == false)
		return "";

	if ( m_bIsExist ) {

		EACH_FILE_DICIDSET newset;

		std::string datapath = imc::GetDataPath("", "");

		newset.filename = dataFileName_full;
		int startindex = newset.filename.find(datapath);
		newset.filename = newset.filename.substr(startindex + datapath.length(), newset.filename.length());
		newset.datavector = eachfiledicidvector;
		m_dicxmlvector.push_back(newset);

		//return SaveDic(convertFileName);
	}
	return "";
}


bool CXMLConvertor::MakeConvertDoc(std::vector<ORISTR_N_DICID>* eachfiledicidvector)
{
	std::vector<imcXml::CXmlNode>	removeGroup;

	imcXml::CXmlNode root = m_doc.RootElement();
	
	for ( imcXml::CXmlNode it = root->FirstChildElement() ; it ; it = it->NextSiblingElement() )
	{
		imcXml::CXmlNode elem = it->ToElement();
		if (elem)
		{
			std::string treestring = it->Value(false);
			imcXml::CXmlAttribute ptempAtt = elem->FirstAttribute();
			if (ptempAtt)
			{
				treestring += ptempAtt->Name();
				treestring += ptempAtt->Value();
			}
			
			ConvertNodeRecursive(elem, eachfiledicidvector, treestring);
		}
	}
	return true;
}

void CXMLConvertor::ConvertNodeRecursive(imcXml::CXmlNode pNode, std::vector<ORISTR_N_DICID>* eachfiledicidvector, std::string treestring)
{
	if ( pNode == nullptr ) 
		return;

	if ( pNode->ToElement() ) {
		
 		imcXml::CXmlAttribute pAttr = pNode->ToElement()->FirstAttribute();
		AttrGroupToChange(pNode->ToElement(), pAttr, eachfiledicidvector, treestring);
	}

	for ( imcXml::CXmlNode it = pNode->FirstChildElement() ; it ; it = it->NextSiblingElement() )
	{
		imcXml::CXmlNode elem = it->ToElement();
		if (elem)
		{
			std::string temptreesting = treestring + elem->Value(false);
			imcXml::CXmlAttribute ptempAtt = elem->FirstAttribute();
			if (ptempAtt)
			{
				temptreesting += ptempAtt->Name();
				temptreesting += ptempAtt->Value();
			}
			ConvertNodeRecursive(elem, eachfiledicidvector, temptreesting);
		}
	}

}

static bool IsNeedTranslate(const std::string& data)
{
	std::wstring widestr = boost::nowide::widen(data);
	boost::wregex re;
	boost::wsmatch whaat;

	std::wstring sre(L"[가-힣]");

	re.assign(sre, boost::regex_constants::icase);

	if (boost::regex_search(widestr, whaat, re))
		return true;


	if (data.find("NXP") != std::string::npos)
		return true;

	return false;
}

// imcDic::SENTENCE_ID CXMLConvertor::CalcDicID(std::string categoryName, std::string strValue)
// {
// 	return  GetBaseDic()->GetNextID(categoryName); // 무조건 새 아이디를
// }

imcDic::SENTENCE_ID CXMLConvertor::CalcDicID(std::string categoryName, const std::string strValue) // 기존의 잘 되던것. 중복 허용 안할 경우 20150126
{
	bool isExist = false;
	const KOR_BASE_DATA& korData = GetBaseDic()->FindDicData(strValue, &isExist);

	imcDic::SENTENCE_ID tempid = m_pDic->IsExistSentence(strValue);

	bool isExist2 = false; // 거지같은 구조다 나중에 필히 바꾸자

	if (tempid != "NotExist")
	{
		isExist2 = true;
	}

	imcDic::SENTENCE_ID dic_id;
	if (isExist) {

		dic_id = korData.ID; // 이미 기존 사전에 있으면 그 아이디를
	}
	else {

		if (isExist2) {
			dic_id = tempid; // 새로 만들고 있는 사전에서 이미 나왔으면 그 아이디를
		}
		else
		{
			dic_id = GetBaseDic()->GetNextID(categoryName); // 아니면 새 아이디를
		}

	}

	return dic_id;
}

// imcDic::SENTENCE_ID CXMLConvertor::CalcDicID(std::string strValue) // 중복 시 다른 ID받도록.
// {
// 	bool isExistInBaseDic = false;
// 	const KOR_BASE_DATA& korData = GetBaseDic()->FindDicData(strValue, &isExistInBaseDic);
// 
// 	if (isExistInBaseDic)
// 		return 0; // 중복이다. 다른 문장 필요.
// 
// 	imcDic::SENTENCE_ID tempid = m_pDic->IsExistSentence(strValue);
// 
// 	if (tempid == 0)
// 		return GetBaseDic()->GetNextID();
// 	else
// 		return 0; // 중복이다. 다른 문장 필요.
// }

std::string _ReplaceAll(std::string str, const std::string& from, const std::string& to){
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos){
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return str;
}

int _GetMinQuestLvByQuestClassName(imc::CStringID className){

	int level = 99999;

	if (className == imc::CStringID("None"))
		return level;

	imcIES::IClass* pClass = imcIES::GetClass("QuestProgressCheck", className);

	if (pClass == NULL){
		return level;
	}
		
	
	int checkquestcount = (int)pClass->GetFloat("Check_QuestCount");

	for (int i = 1; i <= checkquestcount; i++)
	{
		char tempbuf[100] = { 0, };
		sprintf(tempbuf, "QuestName%d", i);

		imc::CStringID checkquestname = pClass->GetString(tempbuf);
		
		if (checkquestname != imc::CStringID("") && pClass->GetName() != checkquestname)
		{
		
			int subminlv = _GetMinQuestLvByQuestClassName(checkquestname);

			if (subminlv < level)
				level = subminlv;
		}
		
	}


	int questlevel = (int)pClass->GetFloat("Level"); //EP_Level
	if (questlevel > 0 && questlevel < 9000 && questlevel < level)
		level = questlevel;

	questlevel = (int)pClass->GetFloat("Lvup");
	if (questlevel > 0 && questlevel < 9000 && questlevel < level)
		level = questlevel;

	questlevel = (int)pClass->GetFloat("Lvdown");
	if (questlevel > 0 && questlevel < 9000 && questlevel < level)
		level = questlevel;

	questlevel = (int)pClass->GetFloat("Succ_Lv");
	if (questlevel > 0 && questlevel < 9000 && questlevel < level)
		level = questlevel;

	return level;
}

int _GetMinQuestJobStepByQuestClassName(imc::CStringID className){

	int level = 99999;

	if (className == imc::CStringID("None"))
		return level;

	imcIES::IClass* pClass = imcIES::GetClass("QuestProgressCheck", className);

	if (pClass == NULL)
		return level;

	int checkquestcount = (int)pClass->GetFloat("Check_QuestCount");

	for (int i = 1; i <= checkquestcount; i++)
	{
		char tempbuf[100] = { 0, };
		sprintf(tempbuf, "QuestName%d", i);

		imc::CStringID checkquestname = pClass->GetString(tempbuf);
		
		if (checkquestname != imc::CStringID("") && pClass->GetName() != checkquestname)
		{
			int subminlv = _GetMinQuestJobStepByQuestClassName(checkquestname);

			if (subminlv < level)
				level = subminlv;
		}

		
	}

	int questlevel = (int)pClass->GetFloat("JobStep"); //EP_Level
	if (questlevel > 0 && questlevel < 9000 && questlevel < level)
		level = questlevel;

	return level;
}

bool _IsNeedCompParsing(const std::string &nowConvertFileName, const std::string &nowAttributeName)
{
	std::size_t fieNameStartPos = nowConvertFileName.rfind("\\");
	fieNameStartPos++;
	std::string filename_nopath = nowConvertFileName.substr(fieNameStartPos, nowConvertFileName.size() - fieNameStartPos);

	auto finder = g_compColumnFileMap.find(filename_nopath);

	if (finder != g_compColumnFileMap.end())
	{
		auto subfinder = finder->second->find(nowAttributeName);
		if (subfinder != finder->second->end())
		{
			return true;
		}
	}

	return false;
}

std::string	CXMLConvertor::GetDialogNPCName(imcXml::CXmlNode elem, const std::string &nowAttributeName)
{
	if (m_nowConvertFileName.find("dialogtext.xml") == std::string::npos)
		return "";

	if (nowAttributeName != "Text")
		return "";

	imcXml::CXmlNode pCurElement = elem;
	for (imcXml::CXmlAttribute attr = pCurElement->FirstAttribute(); attr; attr = attr->Next())
	{
		if (strcmp(attr->Name(), "Caption") == 0){
			return attr->Value();
		}
	}

	return "";

}


std::string	CXMLConvertor::GetCategoryName(imcXml::CXmlNode elem)
{

	std::string retCategoryname = m_categoryName;

	if (m_nowConvertFileName.find("dialogtext.xml") == std::string::npos &&
		m_nowConvertFileName.find("questprogresscheck.xml") == std::string::npos &&
		m_nowConvertFileName.find("questprogresscheck_auto.xml") == std::string::npos &&
		m_nowConvertFileName.find("questprogressnpc.xml") == std::string::npos)
	{
		return retCategoryname;
	}
		


	std::string dialogtextclassname = "";

	imcXml::CXmlNode pCurElement = elem;

	for (imcXml::CXmlAttribute attr = pCurElement->FirstAttribute(); attr; attr = attr->Next())
	{
		if (strcmp(attr->Name(), "ClassName") == 0){
			dialogtextclassname = attr->Value();
			break;
		}
	}

	int level = 99999;
	int unusedchecklevel = 90000;
	int jobstep = 99999;
	std::string newmapname = "";

	auto findlvmap = m_dialogTextLvMap.find(dialogtextclassname);
	if (findlvmap != m_dialogTextLvMap.end())
	{
		return findlvmap->second;
	}
	else{

		//1. 퀘스트 프로그레스에서 탐색
		imcIES::IClassList* pClsList = imcIES::GetClassList("QuestProgressCheck");

		for (int i = 0; i < pClsList->Count(); ++i)
		{
			imcIES::IClass* pClass = pClsList->GetByIndex(i);

			for (int j = 0; j < pClass->GetPropertyCount(); ++j)
			{
				const imcIES::PROPERTY_ENTRY* pEntryCls = pClass->GetPropertyEntryByIndex(j);
				if (pEntryCls->type == imcIES::PROPERTY_ENTRY::STRING){
					imc::CStringID tempstrid = pEntryCls->sValue;

					if (strstr(tempstrid.c_str(), dialogtextclassname.c_str()))
					{

						int questlevel = _GetMinQuestLvByQuestClassName(pClass->GetName());

						if (questlevel > 0 && questlevel < 9000 && questlevel < level)
							level = questlevel;

						if (questlevel >= 900){
							level = unusedchecklevel;
						}

						int questjobstep = _GetMinQuestJobStepByQuestClassName(pClass->GetName());
						if (questjobstep > 0 && questjobstep < jobstep)
							jobstep = questjobstep;

						std::string startmapname = pClass->GetString("StartMap").c_str();
						if (startmapname != "None")
							newmapname = startmapname;

					}
				}

			}
		}

		if (jobstep == 99999 && level == 99999){
			//2. 퀘스트 프로그레스 오토에서 탐색
			imcIES::IClassList* pAutoClsList = imcIES::GetClassList("QuestProgressCheck_Auto");

			for (int i = 0; i < pAutoClsList->Count(); ++i)
			{
				imcIES::IClass* pClass = pAutoClsList->GetByIndex(i);

				for (int j = 0; j < pClass->GetPropertyCount(); ++j)
				{
					const imcIES::PROPERTY_ENTRY* pEntryCls = pClass->GetPropertyEntryByIndex(j);
					if (pEntryCls->type == imcIES::PROPERTY_ENTRY::STRING){
						imc::CStringID tempstrid = pEntryCls->sValue;


						if (strstr(tempstrid.c_str(), dialogtextclassname.c_str()))

						{
							imcIES::IClass* QuestProgressCheckClass = imcIES::GetClass("QuestProgressCheck", pClass->GetName());


							if (QuestProgressCheckClass){

								int questlevel = _GetMinQuestLvByQuestClassName(pClass->GetName());
								if (questlevel > 0 && questlevel < 9000 && questlevel < level)
									level = questlevel;

								if (questlevel >= 900){
									level = unusedchecklevel;
								}

								int questjobstep = _GetMinQuestJobStepByQuestClassName(pClass->GetName());
								if (questjobstep > 0 && questjobstep < jobstep)
									jobstep = questjobstep;

								std::string startmapname = QuestProgressCheckClass->GetString("StartMap").c_str();
								if (startmapname != "None"){

									newmapname = startmapname;
								}

							}


						}
					}

				}
			}
		}


		if (jobstep == 99999 && level == 99999 && newmapname == ""){

			//3. 엔피씨셀렉트다이얼로그에서 검색
			imcIES::IClassList* pNpcDialogClsList = imcIES::GetClassList("NPCSelectDialog");

			for (int i = 0; i < pNpcDialogClsList->Count(); ++i)
			{
				imcIES::IClass* pClass = pNpcDialogClsList->GetByIndex(i);

				for (int j = 0; j < pClass->GetPropertyCount(); ++j)
				{
					const imcIES::PROPERTY_ENTRY* pEntryCls = pClass->GetPropertyEntryByIndex(j);
					if (pEntryCls->type == imcIES::PROPERTY_ENTRY::STRING){
						imc::CStringID tempstrid = pEntryCls->sValue;

						if (strstr(tempstrid.c_str(), dialogtextclassname.c_str()))
						{

							for (auto it = m_dialogTextGenTypeMap.begin(); it != m_dialogTextGenTypeMap.end(); it++)
							{
								std::string dialogname = pClass->GetName().c_str();
								auto diait = it->genset.find(dialogname);

								if (diait != it->genset.end()){
									newmapname = it->mapname;
									break;
								}

							}
						}
					}

					if (newmapname != "")
						break;

				}

				if (newmapname != "")
					break;
			}

		}


		if (jobstep < 99999){

			char strbuff[20];
			//sprintf(strbuff, "_JOBSTEP_%04d", jobstep);
			sprintf(strbuff, "_JOBSTEP");
			retCategoryname = retCategoryname + strbuff;

			m_dialogTextLvMap[dialogtextclassname] = retCategoryname;
		}
		else if (level == unusedchecklevel){
			retCategoryname = retCategoryname + "_UNUSED";
			m_dialogTextLvMap[dialogtextclassname] = retCategoryname;
		}
		else if (level < 99999)
		{

			// 기존 카테고리네임 + 레벨;
			level += (m_questDivideLevel - 1); // 올림
			level /= m_questDivideLevel;
			level *= m_questDivideLevel;
			char strbuff[20];
			sprintf(strbuff, "_LV_%04d", level);
			retCategoryname = retCategoryname + strbuff;

			m_dialogTextLvMap[dialogtextclassname] = retCategoryname;
		}
		else if (newmapname != ""){

			imcIES::IClass* MapCalss = imcIES::GetClass("Map", newmapname.c_str());
			int mapquestlv = 0;
			mapquestlv = (int)MapCalss->GetFloat("QuestLevel");

			if (mapquestlv > 0)
			{
				mapquestlv += (m_questDivideLevel - 1); // 올림
				mapquestlv /= m_questDivideLevel;
				mapquestlv *= m_questDivideLevel;
				char strbuff[20];
				sprintf(strbuff, "_LV_%04d", mapquestlv);
				retCategoryname = retCategoryname + strbuff;

				m_dialogTextLvMap[dialogtextclassname] = retCategoryname;
			}
		}
	}


	return retCategoryname; 

};


void CXMLConvertor::AttrGroupToChange(imcXml::CXmlNode elem, imcXml::CXmlAttribute pAttr, std::vector<ORISTR_N_DICID>* eachfiledicidvector, std::string treestring)
{
 	std::string wholestr = "";
 	std::string classidnamestr = "";
 	bool ishavehangul = false;
	bool alreadyFoundSomeKorean = false;
	imcXml::CXmlAttribute pAttrCopy = pAttr;

	// resultkeystring 생성 부분.
	while (pAttrCopy)
	{
		std::string strValue = pAttrCopy->Value();
		if (strValue.empty()) {
			pAttrCopy = pAttrCopy->Next();
			continue;
		}

		std::string strname = pAttrCopy->Name();

		if (strname == "ClassID" || strname == "ClassName")
		{
			if (IsNeedTranslate(strValue) == false)
			{
				classidnamestr += strValue;
				//g_tempclasscnt++;
			}
			
		}

		if (IsNeedTranslate(strValue))
		{
			ishavehangul = true;
			alreadyFoundSomeKorean = true;
		}

		if (IsNeedTranslate(strValue) == false && alreadyFoundSomeKorean == false)
		{
			wholestr += strValue;
		}

		pAttrCopy = pAttrCopy->Next();
	}

	std::string resultkeystring = "";

	if (ishavehangul)
	{
		if (classidnamestr == "" )
		{
			if (wholestr != "")
			{
				resultkeystring = wholestr;
			}
		}
		else{
			resultkeystring = classidnamestr;
		}
	}
	resultkeystring = treestring + resultkeystring;




	// 한글 찾아서 DicID로 교체하는 부분.
	while (pAttr)
	{
		std::string strValue = pAttr->Value();
		std::string str_converted = strValue; // 변환이 끝난 string

		if (strValue.empty()) {
			pAttr = pAttr->Next();
			continue;
		}

		if (IsNeedTranslate(strValue) == false) // 한글 없으면 패스
		{
			pAttr = pAttr->Next();
			continue;
		}
			

		//한글이 있는 모든 xml데이터에 대해//

		// 1.메모용 한글 컬럼이면 그냥 패스한다.
		if (strstr(pAttr->Name(), "CMM_") != nullptr)
		{
			pAttr = pAttr->Next();
			continue;
		}
			
		
		m_bIsExist = true;

		if (resultkeystring != "")
		{
			resultkeystring = resultkeystring + "_" + pAttr->Name();
		}

		//카테고리 이름 계산
		std::string categoryname = GetCategoryName(elem);
		
		//메타 데이터 추출. 일단 NPC 이름만
		std::string npcname = GetDialogNPCName(elem, pAttr->Name());

		std::vector<std::string> parsedString;

		// 2. 퀘스트 관련 등 특수한 파싱이 필요한 컬럼인가?
		if (_IsNeedCompParsing(m_nowConvertFileName, pAttr->Name()))
		{
			strValue = _ReplaceAll(strValue, "/", ";");
			strValue = _ReplaceAll(strValue, "#", ";");
			strValue = _ReplaceAll(strValue, ":", ";");
			imc::FilterString(strValue.c_str(), ";", parsedString);
		}
		// 3. 일반적인 한글 포함 컬럼인가?
		else{
			strValue = _ReplaceAll(strValue, "{np}", "'");
			imc::FilterString(strValue.c_str(), "'", parsedString);
		}

		//파싱한 데이터에서 한글을 id 변환. id 추가.
		int tokitercnt = 0;
		for (unsigned int i = 0; i < parsedString.size(); i++)
		{
			if (IsNeedTranslate(parsedString[i]) == true)
			{
				char tempbuffer[32] = { 0 };
				itoa(tokitercnt, tempbuffer, 10);
				std::string resultkeystring_temp = resultkeystring + "_" + tempbuffer;

				//dicid 계산 후 추가
				imcDic::SENTENCE_ID dic_id = CalcDicID(categoryname, parsedString[i]);
				m_pDic->AddSentence(parsedString[i], dic_id, categoryname, resultkeystring_temp, npcname);

				//wholddicid용 데이터 생성
				char text[MAX_PATH] = { NULL };
				sprintf(text, "@dicID_^*$%s$*^", dic_id.c_str());

				std::string::size_type index = str_converted.find(parsedString[i]);
				str_converted.replace(index, parsedString[i].length(), text);

			}

			tokitercnt++;
		}

	
		// wholddicid.xml에 데이터 추가.
		bool alreadyhas = false;

		for (auto it = eachfiledicidvector->begin(); it != eachfiledicidvector->end(); it++)
		{
			if (it->original == pAttr->Value())
				alreadyhas = true;
		}

		if (alreadyhas == false)
		{
			ORISTR_N_DICID newdata; 
			newdata.original = pAttr->Value();
			newdata.dicidstr = str_converted; // dicid로 대체된 데이터

			eachfiledicidvector->push_back(newdata);
		}				
		


		// 다음 행으로
		pAttr = pAttr->Next();
	}
}

// dic_xml 생성
std::string CXMLConvertor::SaveDic( const std::string& dicFilename )
{
	std::string fullPath(dicFilename);
	std::string ansiPath;
	imc::WideToMultibyte(boost::nowide::widen(fullPath).c_str(), &ansiPath);
	m_doc.SaveFile(ansiPath.c_str());

	std::cout << "make file : " << ansiPath.c_str() << std::endl;
	return ansiPath;
}