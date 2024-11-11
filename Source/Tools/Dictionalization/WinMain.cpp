// ShaderDBBuilder.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "WindowFrame.h"

#include <shellapi.h>
#include <WinUser.h>
#include <iostream>

#include "BaseDic.h"
#include "imcDirectory.h"
#include "imcFile.h"
#include "imcDirMgr.h"
#include "DicMerger.h"
#include "MakeLuaFromDic.h"
#include <set>
#include "imcIES.h"
#include "imcCommandLine.h"

extern CWindowFrame g_WndFrame;

int APIENTRY WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	LOG_LINE("Start");

	imccmdline::Reset(lpCmdLine);
	int delOldData = 0;
	delOldData = imccmdline::FindParm("-DEL_OLD_DATA") != 0 ? -2 : 0;
	if (delOldData == 0)
		delOldData = imccmdline::FindParm("-MARK_OLD_DATA") != 0 ? -1 : 0;

	if (delOldData == 0)
		std::cout << "Old Data : Remain" << std::endl;
	else if (delOldData == -1)
		std::cout << "Old Data : Not Export" << std::endl;
	else if (delOldData == -2)
		std::cout << "Old Data : Mark" << std::endl;


	//퀘스트 대사 레벨별 분리를 위해 필요 클래스 로드
	bool ret = imcIES::RegisterClassFromXML(imc::GetDataPath("xml\\", "questprogresscheck.xml").c_str());
	if (ret == false)
	{
		std::cout << "Error : Couldn't open questprogresscheck.xml" << std::endl;
	}
	ret = imcIES::RegisterClassFromXML(imc::GetDataPath("xml\\", "questprogresscheck_auto.xml").c_str());
	if (ret == false)
	{
		std::cout << "Error : Couldn't open questprogresscheck_auto.xml" << std::endl;
	}
	ret = imcIES::RegisterClassFromXML(imc::GetDataPath("xml\\", "map.xml").c_str());
	if (ret == false)
	{
		std::cout << "Error : Couldn't open map.xml" << std::endl;
	}
	ret = imcIES::RegisterClassFromXML(imc::GetDataPath("xml\\", "npcselectdialog.xml").c_str());
	if (ret == false)
	{
		std::cout << "Error : Couldn't open npcselectdialog.xml" << std::endl;
	}
	//

	// 툴 초기화
	GetBaseDic()->Init();
	CDictionalization m_dic;
	m_dic.Init();
	imcDic::LoadPossibleLanguages_Tool();

	//수집 할 폴더 목록
	std::string collectFolderPath = imc::GetPath(imc::GetBinPath(), "toolfiles\\collectFolder.xml", "");
	imcXml::CXmlDoc doc;
	bool bRet = imcXml::Load(collectFolderPath.c_str(), doc);

	if (bRet == false){
		return 0;
	}

	imcXml::CXmlNode pDicNode = doc.FirstChildElement("CollectFolderList");

	if (!pDicNode)
		return 0;

	m_dic.LoadLastDicFileTime();
	m_dic.LoadWholeDicIDFile();


	std::set<std::string> collectedfilelist;
	std::list<std::string> passeddfilelist;

	for (imcXml::CXmlNode pNodeIndex = pDicNode->FirstChildElement("CollectFolder"); pNodeIndex; pNodeIndex = pNodeIndex->NextSiblingElement("CollectFolder"))
	{
		imcXml::CXmlNode pElement = pNodeIndex->ToElement();

		std::string path = imc::GetDataPath(pElement->Attribute("Path"),"");
		std::string exportFilename = pElement->Attribute("ExportFilename");
		int ignoreFileExtension = pElement->IntAttribute("IgnoreFileExtension");

		std::cout << "Convert xml file that under [" << path << "] path to dic_xml file" << std::endl;
		m_dic.MakeDictionary(path.c_str(), true, ignoreFileExtension == 1 ? true : false, exportFilename.c_str(), &collectedfilelist, &passeddfilelist);
	}

	//LOG_LINE("g_tempnonclasscnt : %d / g_tempclasscnt : %d / etc : %d", g_tempnonclasscnt, g_temspclasscnt,g_tempetc);

	m_dic.SaveLastDicFileTime();
	m_dic.SaveWholeDicIDFile();

	boost::unordered_map<std::string, KOR_BASE_DATA>* baseData = GetBaseDic()->GetBaseData();
	boost::unordered_map<std::string, std::string>* baseDataIDKOR = GetBaseDic()->GetBaseDataIDKOR();
	std::map<std::string, std::vector<std::string>*>* baseplacedmap = GetBaseDic()->GetBasePlacedKeyMap();
	imcDic::BuildDictionary(baseData, &passeddfilelist, baseplacedmap, delOldData, baseDataIDKOR);

	

	m_dic.Exit();
	std::cout << "press any key to exit" << std::endl;
	getchar();
	return 0;
}