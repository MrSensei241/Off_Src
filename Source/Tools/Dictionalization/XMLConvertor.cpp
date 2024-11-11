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
	// ����Ʈ ������ ������ �и��� ���� �������� ���� Ŭ������ �̸� �ε�
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

	//���� �� ���� ���� �ε�
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

	//m_doc���� �������� dic_xml ����� ���� �ʰ� �ϳ��� ���Ͽ��� dic_xml�� ��ġ�Ǿ� ���� ���̵� ���� �����ϱ� ���� ����� ��
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

	std::wstring sre(L"[��-�R]");

	re.assign(sre, boost::regex_constants::icase);

	if (boost::regex_search(widestr, whaat, re))
		return true;


	if (data.find("NXP") != std::string::npos)
		return true;

	return false;
}

// imcDic::SENTENCE_ID CXMLConvertor::CalcDicID(std::string categoryName, std::string strValue)
// {
// 	return  GetBaseDic()->GetNextID(categoryName); // ������ �� ���̵�
// }

imcDic::SENTENCE_ID CXMLConvertor::CalcDicID(std::string categoryName, const std::string strValue) // ������ �� �Ǵ���. �ߺ� ��� ���� ��� 20150126
{
	bool isExist = false;
	const KOR_BASE_DATA& korData = GetBaseDic()->FindDicData(strValue, &isExist);

	imcDic::SENTENCE_ID tempid = m_pDic->IsExistSentence(strValue);

	bool isExist2 = false; // �������� ������ ���߿� ���� �ٲ���

	if (tempid != "NotExist")
	{
		isExist2 = true;
	}

	imcDic::SENTENCE_ID dic_id;
	if (isExist) {

		dic_id = korData.ID; // �̹� ���� ������ ������ �� ���̵�
	}
	else {

		if (isExist2) {
			dic_id = tempid; // ���� ����� �ִ� �������� �̹� �������� �� ���̵�
		}
		else
		{
			dic_id = GetBaseDic()->GetNextID(categoryName); // �ƴϸ� �� ���̵�
		}

	}

	return dic_id;
}

// imcDic::SENTENCE_ID CXMLConvertor::CalcDicID(std::string strValue) // �ߺ� �� �ٸ� ID�޵���.
// {
// 	bool isExistInBaseDic = false;
// 	const KOR_BASE_DATA& korData = GetBaseDic()->FindDicData(strValue, &isExistInBaseDic);
// 
// 	if (isExistInBaseDic)
// 		return 0; // �ߺ��̴�. �ٸ� ���� �ʿ�.
// 
// 	imcDic::SENTENCE_ID tempid = m_pDic->IsExistSentence(strValue);
// 
// 	if (tempid == 0)
// 		return GetBaseDic()->GetNextID();
// 	else
// 		return 0; // �ߺ��̴�. �ٸ� ���� �ʿ�.
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

		//1. ����Ʈ ���α׷������� Ž��
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
			//2. ����Ʈ ���α׷��� ���信�� Ž��
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

			//3. ���Ǿ�����Ʈ���̾�α׿��� �˻�
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

			// ���� ī�װ����� + ����;
			level += (m_questDivideLevel - 1); // �ø�
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
				mapquestlv += (m_questDivideLevel - 1); // �ø�
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

	// resultkeystring ���� �κ�.
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




	// �ѱ� ã�Ƽ� DicID�� ��ü�ϴ� �κ�.
	while (pAttr)
	{
		std::string strValue = pAttr->Value();
		std::string str_converted = strValue; // ��ȯ�� ���� string

		if (strValue.empty()) {
			pAttr = pAttr->Next();
			continue;
		}

		if (IsNeedTranslate(strValue) == false) // �ѱ� ������ �н�
		{
			pAttr = pAttr->Next();
			continue;
		}
			

		//�ѱ��� �ִ� ��� xml�����Ϳ� ����//

		// 1.�޸�� �ѱ� �÷��̸� �׳� �н��Ѵ�.
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

		//ī�װ� �̸� ���
		std::string categoryname = GetCategoryName(elem);
		
		//��Ÿ ������ ����. �ϴ� NPC �̸���
		std::string npcname = GetDialogNPCName(elem, pAttr->Name());

		std::vector<std::string> parsedString;

		// 2. ����Ʈ ���� �� Ư���� �Ľ��� �ʿ��� �÷��ΰ�?
		if (_IsNeedCompParsing(m_nowConvertFileName, pAttr->Name()))
		{
			strValue = _ReplaceAll(strValue, "/", ";");
			strValue = _ReplaceAll(strValue, "#", ";");
			strValue = _ReplaceAll(strValue, ":", ";");
			imc::FilterString(strValue.c_str(), ";", parsedString);
		}
		// 3. �Ϲ����� �ѱ� ���� �÷��ΰ�?
		else{
			strValue = _ReplaceAll(strValue, "{np}", "'");
			imc::FilterString(strValue.c_str(), "'", parsedString);
		}

		//�Ľ��� �����Ϳ��� �ѱ��� id ��ȯ. id �߰�.
		int tokitercnt = 0;
		for (unsigned int i = 0; i < parsedString.size(); i++)
		{
			if (IsNeedTranslate(parsedString[i]) == true)
			{
				char tempbuffer[32] = { 0 };
				itoa(tokitercnt, tempbuffer, 10);
				std::string resultkeystring_temp = resultkeystring + "_" + tempbuffer;

				//dicid ��� �� �߰�
				imcDic::SENTENCE_ID dic_id = CalcDicID(categoryname, parsedString[i]);
				m_pDic->AddSentence(parsedString[i], dic_id, categoryname, resultkeystring_temp, npcname);

				//wholddicid�� ������ ����
				char text[MAX_PATH] = { NULL };
				sprintf(text, "@dicID_^*$%s$*^", dic_id.c_str());

				std::string::size_type index = str_converted.find(parsedString[i]);
				str_converted.replace(index, parsedString[i].length(), text);

			}

			tokitercnt++;
		}

	
		// wholddicid.xml�� ������ �߰�.
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
			newdata.dicidstr = str_converted; // dicid�� ��ü�� ������

			eachfiledicidvector->push_back(newdata);
		}				
		


		// ���� ������
		pAttr = pAttr->Next();
	}
}

// dic_xml ����
std::string CXMLConvertor::SaveDic( const std::string& dicFilename )
{
	std::string fullPath(dicFilename);
	std::string ansiPath;
	imc::WideToMultibyte(boost::nowide::widen(fullPath).c_str(), &ansiPath);
	m_doc.SaveFile(ansiPath.c_str());

	std::cout << "make file : " << ansiPath.c_str() << std::endl;
	return ansiPath;
}