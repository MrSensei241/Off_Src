#include "StdAfx.h"

#include "imcStr.h"
#include "imcDirMgr.h"
#include "imcDirectory.h"
#include "Dictionalization.h"
#include "boost/nowide/convert.hpp"
#include <set>
#include <list>
#include <sys/stat.h>


CDictionalization::CDictionalization(void)
{
}


CDictionalization::~CDictionalization(void)
{
}

bool CDictionalization::Init()
{
	imc::InitPath();
	imcDic::EnableDic(false);
	imcDic::Init(true, false);
	imcDic::SetCountry("kr");

	std::string no_dic = imc::GetPath(imc::GetBinPath(), "toolfiles\\no_dictionary.ini", ""); //읽지 말아야 할 파일 리스트가 저장되어 있음
	std::string filename;
	imc::WideToMultibyte(boost::nowide::widen(no_dic), &filename);

	std::ifstream fileInput(filename.c_str(), std::ios::in);
	if (fileInput.is_open())
	{
		while (fileInput.eof() == false)
		{
			std::string fileName;
			fileInput >> fileName;
			m_noDicFile.emplace_back(fileName);
		}
	}

	//CollectExistData(".dic_xml"); // dic_xml 안쓰면서 취소

	m_xmlConvertor.Init();
	m_lastfiletimemap.clear();
	return true;
}

void CDictionalization::CollectExistData(const std::string& extname)
{
	std::string baseDir = imc::GetDataPath("", ""); // Data 하위만 검색한다. 고로 모든 dic_xml은 Data 하위에 있어야 함.
	imcfile::CDirectoryFiles* pFiles = imcfile::GetDirectory()->GetFiles(baseDir.c_str(), extname.c_str());
	for (unsigned int index = 0; index < pFiles->GetFileNum(); ++index) {
		std::string fileName = pFiles->GetFileNameByIndex(index);
		std::string ansiFilename;
		imc::WideToMultibyte(boost::nowide::widen(fileName).c_str(), &ansiFilename);
		m_existData.emplace_back(ansiFilename);
	}
}

void CDictionalization::Exit()
{
	//	DeleteNotCreatedFile();
	imcDic::Exit();
}

void _InsertStat(const char * name, __int64 time, XML_STAT_LIST& statList)
{
	statList[name] = time;
}

bool _IsOldStat(const char * name, __int64 time, XML_STAT_LIST& statList)
{
	auto index = statList.find(name);
	if (index == statList.end())
	{
		return true;
	}

	__int64 lateTime = index->second;
	return lateTime != time;
}

void CDictionalization::MakeDictionary(const char* folderOrFilename, bool disableLua, bool ignoreFileExtension, std::string categoryName, std::set<std::string>* collectedfilelist, std::list<std::string>* passedfilelist)
{
	imcDic::Reset();
	std::string ansiFolder(folderOrFilename);

	std::vector<std::string> collectfiles;
	CollectFileName(ansiFolder.c_str(), &collectfiles);

	for (unsigned int index = 0; index < collectfiles.size(); ++index) {
		const std::string& tempfilename = collectfiles[index];

		if (tempfilename.find("dic_xml") != std::string::npos)
			continue;



		//검사할 필요가 있는 파일인지 검사
		struct _stat buf;
		buf.st_mtime = 0;

		if (0 != _stat(tempfilename.c_str(), &buf))
			continue;

		auto isAlreadyCollectd = collectedfilelist->find(tempfilename);// 파일당 한번씩만 돌도록

		if (_IsOldStat(tempfilename.c_str(), buf.st_mtime, m_lastfiletimemap) == false && isAlreadyCollectd == collectedfilelist->end()) //변한게 없다. 즉 패스.
		{
			std::string datapath = imc::GetDataPath("", "");
			int startindex = tempfilename.find(datapath);
			std::string tempfilenamenodatapath = tempfilename.substr(startindex + datapath.length(), tempfilename.length());
			passedfilelist->push_back(tempfilenamenodatapath); // 넘어간 파일이라는걸 표시해주고

			collectedfilelist->insert(tempfilename); // 중복해서 돌지 않도록.

			continue;
		}

		_InsertStat(tempfilename.c_str(), buf.st_mtime, m_lastfiletimemap);
		///////////////////////////



		

		if (isAlreadyCollectd == collectedfilelist->end()){

			if (ignoreFileExtension == true){

				ConvertXML(tempfilename, categoryName);
				collectedfilelist->insert(tempfilename);
				
			}
			else{
				
				switch (GetFileType(tempfilename)) {

				case FT_XML:
					ConvertXML(tempfilename, categoryName);
					collectedfilelist->insert(tempfilename);

					break;
				case FT_LUA:
					if (disableLua == false) // 루아는 이제 할 일 없다. 없어야 함. 140512 ayase
						//ConvertLUA(tempfilename);
						break;
				case FT_ERROR:
					continue;
				}
				
			}

		}
		
	}
}

void CDictionalization::CollectFileName(const char* folderOrFilename, std::vector<std::string>* pOutGroup)
{
	std::string datapathdir(folderOrFilename);

	auto findSize = datapathdir.find(".xml");
	if (findSize != std::string::npos){
		pOutGroup->push_back(datapathdir);
		return;
	}


	imcfile::CDirectoryFiles* pFiles = imcfile::GetDirectory()->GetFiles(datapathdir.c_str());
	if (pFiles) {
		*pOutGroup = pFiles->GetFiles();
	}
}

FILE_TYPE CDictionalization::GetFileType(const std::string& fullFilePath)
{
	std::string::size_type findSize = std::string::npos;

	findSize = fullFilePath.find(".dic_xml");
	if (findSize != std::string::npos)
		return FT_ERROR;

	findSize = fullFilePath.find(".dic_lua");
	if (findSize != std::string::npos)
		return FT_ERROR;


	findSize = fullFilePath.find(".xml");
	if (findSize != std::string::npos)
		return FT_XML;

	findSize = fullFilePath.find(".lua");
	if (findSize != std::string::npos)
		return FT_LUA;

	return FT_ERROR;
}

void CDictionalization::ConvertXML(const std::string& fullFileName, std::string categoryName)
{
	for (unsigned int index = 0; index < m_noDicFile.size(); ++index) {
		if (fullFileName.find(m_noDicFile[index]) != std::wstring::npos)
			return;
	}

	std::string datafileName(fullFileName);
	m_xmlConvertor.SetCategoryName(categoryName);
	std::string makeFileName = m_xmlConvertor.StartConvert(datafileName);
	if (makeFileName.empty() == false)
		m_makedata.emplace_back(makeFileName);
}

void CDictionalization::ConvertLUA(const std::string& fullFileName)
{
	for (unsigned int index = 0; index < m_noDicFile.size(); ++index) {
		if (fullFileName.find(m_noDicFile[index]) != std::wstring::npos)
			return;
	}

	std::string fullfileName;
	imc::WideToMultibyte(boost::nowide::widen(fullFileName).c_str(), &fullfileName);
	std::string makeFileName = m_luaConvertor.StartConvert(fullfileName);



	// 	if ( makeFileName.empty() == false  ) 
	// 		m_makedata.emplace_back(makeFileName);
}

void CDictionalization::DeleteNotCreatedFile()
{
	for (unsigned int index = 0; index < m_existData.size(); ++index) {
		const std::string& fileName = m_existData[index];
		auto findIter = std::find(m_makedata.begin(), m_makedata.end(), fileName);
		if (findIter == m_makedata.end()) {
			//못찾았넹?
			::remove(fileName.c_str());
			std::cout << "remove file : " << fileName.c_str() << std::endl;
		}
	}

}

void CDictionalization::LoadWholeDicIDFile()
{
	imcXml::CXmlDoc doc;
	std::string wholeDicIDPath = imc::GetDataPath("language\\", "wholeDicID.xml");
	bool bRet = imcXml::Load(wholeDicIDPath.c_str(), doc);

	if (bRet == false){
		return;
	}

	imcXml::CXmlNode pDicNode = doc.FirstChildElement("filelist");

	if (!pDicNode)
		return;

	std::map<std::string, std::map<std::string, std::string>>* beforedicxmlmap = m_xmlConvertor.GetBeforeDicXmlMap();

	for (imcXml::CXmlNode pNodeIndex = pDicNode->FirstChildElement("file"); pNodeIndex; pNodeIndex = pNodeIndex->NextSiblingElement("file"))
	{
		imcXml::CXmlNode pElement = pNodeIndex->ToElement();

		std::string eachfilename = pElement->Attribute("name");
		std::map<std::string, std::string> datamap;

		for (imcXml::CXmlNode pSubNodeIndex = pNodeIndex->FirstChildElement("data"); pSubNodeIndex; pSubNodeIndex = pSubNodeIndex->NextSiblingElement("data"))
		{
			imcXml::CXmlNode psubElement = pSubNodeIndex->ToElement();
			std::string original = psubElement->Attribute("original");
			std::string dicid = psubElement->Attribute("dicid");

			datamap.insert(std::make_pair(original, dicid));
		}

		beforedicxmlmap->insert(std::make_pair(eachfilename, datamap));
	}

	std::cout << "beforedicxmlmap->size() : " << beforedicxmlmap->size() << std::endl;
}

void CDictionalization::SaveWholeDicIDFile()
{
	imcXml::CXmlDoc dicDoc;
	dicDoc.Clear();
	imcXml::InitDocument(dicDoc);

	imcXml::CXmlNode filelistElement = imcXml::CreateChild(dicDoc, "filelist");

	std::vector<EACH_FILE_DICIDSET>* dicxmlvector = m_xmlConvertor.GetDicXmlVector();

	if (dicxmlvector->size() == 0)
		return;

	//dicxmlvector에 없는 얘들은 기존 데이터에서 그대로 추가
	std::map<std::string, std::map<std::string, std::string>>* beforedicxmlmap = m_xmlConvertor.GetBeforeDicXmlMap();
	for (auto oldmapiter = beforedicxmlmap->begin(); oldmapiter != beforedicxmlmap->end(); ++oldmapiter)
	{

		std::string oldfilename = oldmapiter->first;
		bool isexist = false;

		for (auto vectoriter = dicxmlvector->begin(); vectoriter != dicxmlvector->end(); ++vectoriter){

			if (oldfilename == vectoriter->filename){
				isexist = true;
				break;
			}

		}

		if (isexist == true){
			continue;
		}

		//수집한 벡터에 없는 얘들만 추가
		auto submap = oldmapiter->second;

		EACH_FILE_DICIDSET newset;

		newset.filename = oldfilename;
		newset.datavector.clear();

		for (auto oldmapsubmapiter = submap.begin(); oldmapsubmapiter != submap.end(); oldmapsubmapiter++){

			ORISTR_N_DICID newsubset;
			newsubset.original = oldmapsubmapiter->first;
			newsubset.dicidstr = oldmapsubmapiter->second;

			newset.datavector.push_back(newsubset);
		}

		dicxmlvector->push_back(newset);

	}

	std::sort(dicxmlvector->begin(), dicxmlvector->end(), [](const EACH_FILE_DICIDSET& left, const EACH_FILE_DICIDSET& right) -> bool {
		return left.filename < right.filename;
	});


	for (auto iter = dicxmlvector->begin(); iter != dicxmlvector->end(); ++iter)
	{
		std::string filename = iter->filename;

		imcXml::CXmlNode fileElement = filelistElement->NewElement("file");
		fileElement->SetAttribute("name", filename.c_str());

		std::vector<ORISTR_N_DICID> dicids = iter->datavector;
		for (auto subiter = dicids.begin(); subiter != dicids.end(); ++subiter)
		{
			std::string original = subiter->original;
			std::string dicidstr = subiter->dicidstr;

			imcXml::CXmlNode eachdataElement = fileElement->NewElement("data");
			eachdataElement->SetAttribute("original", original.c_str());
			eachdataElement->SetAttribute("dicid", dicidstr.c_str());

			fileElement->InsertEndChild(eachdataElement);
		}

		filelistElement->InsertEndChild(fileElement);
	}

	std::string wholeDicIDPath = imc::GetDataPath("language\\", "wholeDicID.xml");
	dicDoc.SaveFile(wholeDicIDPath.c_str());

}

void CDictionalization::SaveLastDicFileTime(){

	::CreateDirectoryW(boost::nowide::widen(imc::GetDataPath("language", "").c_str()).c_str(), NULL);

	std::string fileName = imc::GetPath(imc::GetBinPath(), "toolfiles\\lastDicIDFileTime.tmp", "");

	FILE* fp = _wfopen(boost::nowide::widen(fileName).c_str(), L"wb");
	int cnt = m_lastfiletimemap.size();
	fwrite(&cnt, sizeof(int), 1, fp);
	for (auto i = m_lastfiletimemap.begin(); i != m_lastfiletimemap.end(); i++)
	{
		const std::string& name = i->first;
		__int64 time = i->second;
		imcfile::WriteString(name.c_str(), fp);
		fwrite(&time, sizeof(__int64), 1, fp);
	}

	fclose(fp);
}

void CDictionalization::LoadLastDicFileTime(){

	::CreateDirectoryW(boost::nowide::widen(imc::GetDataPath("language", "").c_str()).c_str(), NULL);

	imcfile::IFileMem*	pFileMem = imcfile::Load(imc::GetPath(imc::GetBinPath(), "toolfiles\\lastDicIDFileTime.tmp", "").c_str());

	if (pFileMem == NULL)
		return;

	char * buf = pFileMem->GetData();;
	int cnt = imcfile::Read<int>(&buf);
	for (int i = 0; i < cnt; i++)
	{
		const char * fName = imcfile::ReadString(&buf);
		__int64 fTime = *((__int64*)buf);
		buf += sizeof(__int64);
		m_lastfiletimemap[fName] = fTime;
	}

	imcfile::Close(pFileMem);
}