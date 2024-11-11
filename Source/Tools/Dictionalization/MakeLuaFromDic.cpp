#include "StdAfx.h"
#include "MakeLuaFromDic.h"
#include "boost/nowide/convert.hpp"

CMakeLuaFromDic::CMakeLuaFromDic(void)
{
}


CMakeLuaFromDic::~CMakeLuaFromDic(void)
{
}

void CMakeLuaFromDic::StartMake(const std::string& filePath)
{
	std::string baseDir = imc::GetDataPath(filePath.c_str(), "");
	//imcfile::CDirectoryFiles* pBaseFiles = imcfile::GetDirectory()->GetFiles(imc::FDT_DATA, filePath.c_str(), ".dic_lua");
	imcfile::CDirectoryFiles* pBaseFiles = imcfile::GetDirectory()->GetFiles(baseDir.c_str(), ".dic_lua");
	if ( pBaseFiles == nullptr ) 
		return;

	int numFiles = pBaseFiles->GetFileNum();
	for ( unsigned int index = 0 ; index < numFiles ; ++index ) 
	{ 
		std::string fileName = pBaseFiles->GetFileNameByIndex(index);
		std::string ansi_fileName;
		imc::WideToMultibyte(boost::nowide::widen(fileName).c_str(), &ansi_fileName);

// 		for ( unsigned int countryIndex = 0 ; countryIndex < imcDic::GetNumCountry() ; ++countryIndex )  // 주석 하면 안되는 부분임. 루아 컨버터 안쓸것 같아서 일단 걍 주석. 테스트 끝나면 바뀐 언어 저장 방식으로 다시 만들자
// 		{
// 			const char* pCountry = imcDic::GetCountry();
// 			imcDic::SetCountry(pCountry);
// 			MakeFile(ansi_fileName);
// 		}
	}
}

void CMakeLuaFromDic::MakeFile( const std::string& fileName )
{
	m_textParser.Reset();
	m_texExporter.Reset();

	m_textParser.Init(fileName);
	while( m_textParser.IsEnd() == false ) 
	{
		std::string sentence = m_textParser.GetGlyph();
		sentence = ChangeToDicDataRecursive(sentence);
		m_texExporter.AddSentence(sentence);
		m_texExporter.AddReturn();
	}

	std::string saveFileName = fileName;
	saveFileName.erase(saveFileName.find("."), saveFileName.size());
	saveFileName += ".";
	saveFileName += imcDic::GetCountry();
	saveFileName += "_lua";
	m_texExporter.SaveAS(saveFileName);
	std::cout << "[SYSTEM : " << saveFileName << " 생성" << std::endl;
}

std::string CMakeLuaFromDic::ChangeToDicDataRecursive( std::string sentence )
{
	std::string::size_type startDic = sentence.find("@dicID_");
	if ( startDic == std::string::npos ) 
		return sentence;

	std::string::size_type endDic = sentence.find("~@");
	IMC_ASSERT(endDic!=std::string::npos);
	std::string dicID = sentence.substr(startDic, endDic-startDic + 2);

	imcDic::FIND_DIC_INFO dicInfo;
	dicInfo.dataFile = imcDic::GenerateDicFileName(dicID.c_str());
	dicInfo.mode	 = imcDic::DM_USE_DIC;
	imcDic::IDic* pDic = imcDic::FindDic(dicInfo);
	if ( pDic ) {
		std::string dicData = pDic->FindID(dicID);
		sentence.erase(startDic, endDic-startDic+2);
		sentence.insert(startDic, dicData);
	}

	return ChangeToDicDataRecursive(sentence);
}
