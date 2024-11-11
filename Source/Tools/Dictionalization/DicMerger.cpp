#include "StdAfx.h"

#include "imcStr.h"
#include "imcDirMgr.h"
#include "imcDirectory.h"
#include "DicMerger.h"
#include <omp.h>
#include "boost/nowide/convert.hpp"

CDicMerger::CDicMerger(void)
{
}


CDicMerger::~CDicMerger(void)
{
}

void CDicMerger::StartMerge()
{
	//imcfile::CDirectoryFiles* pTargetForlder = imcfile::GetDirectory()->GetFiles(imc::FDT_BIN, "language");

	std::string dirName = imc::GetDataPath("","");
	dirName += "language";
	//imcfile::CDirectoryFiles* pTargetForlder = imcfile::GetDirectory()->GetFiles(dirName.c_str());
	std::string baseFullFile = dirName + "\\translatedData.xml";
	//imcXml::CXmlDoc doc;
	imcXml::CXmlDoc doc;
	
	if ( imcXml::Load(baseFullFile.c_str(), doc) ) 
	{
		std::vector<MERGE_DATA> mergeBase;
		//LoadBaseData(doc, &mergeBase);	

		//for ( unsigned int index = 0 ; index < pTargetForlder->GetFileNum() ; ++index ) 
//			MergeDicData(pTargetForlder->GetFileNameByIndex(index), doc, mergeBase);

		MergeDicData(baseFullFile, doc, mergeBase);
	} else {
		std::wcout << "error XML name : " << baseFullFile.c_str() << " load failed" << std::endl;
	}
}

void CDicMerger::LoadBaseData(imcXml::CXmlDoc& doc, std::vector<MERGE_DATA>* pOutMerge )
{
// 	imcXml::CXmlNode pRoot = doc.RootElement();
// 	int count = 0;
// 	for ( imcXml::CXmlNode pDicNode = pRoot->FirstChildElement() ; pDicNode ; pDicNode = pDicNode->NextSiblingElement() )
// 	{
// 		imcXml::CXmlNode pElem = pDicNode->ToElement();
// 		IMC_ASSERT(pElem);
// 		const char * buf = pElem->Value();
// 
// 		if (imc::StrIsSame(pElem->Value(), "dic_data")) {
// 			count++;
// 
// 			MERGE_DATA data;
// 			imcXml::GetXmlElemAttr(pElem, "ID", &data.ID);
// 			imcXml::GetXmlElemAttr(pElem, "IsUse", &data.isUse);
// 
// 
// 			std::vector<std::string> *pLanguages = imcDic::GetPossibleLanguages();
// 			for (unsigned int i = 0; i < pLanguages->size(); i++)
// 			{
// 				std::string iterstr = pLanguages->at(i);
// 				std::string tempstr = pElem->Attribute(iterstr.c_str());
// 
// 				if (tempstr != "")
// 				{
// 					data.translateDatas.push_back(tempstr);
// 				}
// 				else{
// 					data.translateDatas.push_back("");
// 				}
// 			}
// 
// 			//data.kor		= pElem->Attribute("KOR");
// 			//data.translate	= pElem->Attribute("ENG"); // 여러 언어의 번역본이 한 파일에 있을 것이다. 그래도 괜찮은가?
// 			data.fileName	= pElem->Attribute("Filename");
// 			pOutMerge->emplace_back(data);
// 		}
// 	}
}

void CDicMerger::MergeDicData( const std::string& targetFullPath, imcXml::CXmlDoc& doc, const std::vector<MERGE_DATA>& mergeBase )
{
	
// 	if ( targetFullPath.find("translatedData.xml") != std::string::npos ) 
// 		return;

	std::string fullPath = targetFullPath;
	bool isOverWrite = false;
	HANDLE hHandle = CreateFileW(boost::nowide::widen(fullPath).c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if ( hHandle == INVALID_HANDLE_VALUE ) {
		isOverWrite = true;
	} else {
		SAFE_CLOSE_HANDLE(hHandle);
		imcXml::CXmlDoc targetDoc;
		if ( imcXml::Load(fullPath.c_str(), targetDoc) == false ) {
			isOverWrite = true;
		} else {
			std::vector<MERGE_DATA>	 targetMerge, newGroup;
			LoadBaseData(targetDoc, &targetMerge);

			for ( unsigned int index = 0 ; index < mergeBase.size() ; ++index ) 
			{
				const MERGE_DATA& baseData = mergeBase[index];

				//같은 단어를 찾자.
				auto findIter = std::find_if(targetMerge.begin(), targetMerge.end(), [&](const MERGE_DATA& data) -> bool {
					return (data.translateDatas[0].compare(baseData.translateDatas[0]) == 0);
				});

				if ( findIter == targetMerge.end() ) 
				{	
					newGroup.emplace_back(baseData);
				} else {
					MERGE_DATA& findMerge = (*findIter);
					MERGE_DATA newData = (*findIter);
					newData.ID = baseData.ID;
					newData.isUse = baseData.isUse;
					newData.fileName = baseData.fileName;
					newGroup.emplace_back(newData);
				}
			}
			SaveMergeData(fullPath, newGroup);
		}
	}

	if ( isOverWrite ) {
		std::string ansi_path;
		imc::WideToMultibyte(boost::nowide::widen(fullPath), &ansi_path);
		doc.SaveFile(ansi_path.c_str());
		std::cout << "make multilanguafe folder file : " << ansi_path.c_str() << std::endl;
	}
}

void CDicMerger::SaveMergeData( const std::string& path, std::vector<MERGE_DATA>& mergeData ) // 안쓰는 함수
{
	std::sort(mergeData.begin(), mergeData.end(), [](const MERGE_DATA& rhs, const MERGE_DATA& lhs) -> bool {
		return rhs.translateDatas[0] > lhs.translateDatas[0];
	});

	imcXml::CXmlDoc dicDoc;
	//tinyxml2::XMLDeclaration* decl = dicDoc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");

	dicDoc.Clear();
	//dicDoc.SetTabSize(8);
	//dicDoc.InsertEndChild(decl);
	dicDoc.SetDeclaration();

	std::sort(mergeData.begin(), mergeData.end(), [](const MERGE_DATA& left, const MERGE_DATA& right) -> bool {
		return left.ID < right.ID;
	});

	//GetBaseDic()->GetLastID();

	imcXml::CXmlNode dic = dicDoc.NewElement("dictionary");
	//dic->SetAttribute("lastID", m_lastid);


	for ( unsigned int index = 0 ; index < mergeData.size() ; ++index ) {

		imcXml::CXmlNode element = imcXml::CreateChild(dic,"dic_data");
		const MERGE_DATA& dicData = mergeData[index];

		element->SetAttribute("ID", dicData.ID.c_str());
		element->SetAttribute("IsUse", dicData.isUse == true ? 1 : 0);


		std::vector<imcDic::POSSIBLE_COUNTRY> *pLanguages = imcDic::GetPossibleLanguages();
		for (unsigned int i = 0; i < pLanguages->size(); i++)
		{
			std::string iterstr = pLanguages->at(i).code;

			element->SetAttribute(iterstr.c_str(), dicData.translateDatas[i].c_str());
		}

		element->SetAttribute("Filename", dicData.fileName.c_str());
	}
	dicDoc.InsertEndChild(dic);

	std::string ansiPath;
	imc::WideToMultibyte(boost::nowide::widen(path), &ansiPath);
	dicDoc.SaveFile(ansiPath.c_str());
	std::cout << "Merge make : " << ansiPath.c_str() << std::endl;
}
