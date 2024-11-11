#include "stdafx.h"
#include "boost/nowide/convert.hpp"

HRESULT ConvertXMLToIES(const char * xmlFileName, const char * iesFileName)
{
	return imcIES::XMLToIES(xmlFileName, iesFileName);
}

bool ConvertIESToXML(const char * iesFileName, const char * xmlFileName, const char * pEncoding)
{
	return imcIES::IESToXML(iesFileName, xmlFileName, pEncoding);
}

enum CONVERT_TYPE
{
	XMLTOIES = 0,
	IESTOXML,
	NONETYPE,
};

CONVERT_TYPE				g_convertType		= NONETYPE;
std::string					g_fileName;
std::string					g_encoding			= "UTF-8";
BOOL						g_isFullDirConvert	= TRUE;
BOOL						g_isDeleteDir		= FALSE;
imc::CVector<std::string>	g_fileNameList;

void ParseCmdLine( int argc, char * argv[] )
{
	imccmdline::Reset(argc, argv);


	if( imccmdline::FindParm("-DELETE") ) {
		g_isDeleteDir = TRUE;
	}

	if( imccmdline::FindParm("-ALL") ) {
		g_isFullDirConvert	= TRUE;
	} else {
		g_isFullDirConvert	= FALSE;
		g_fileName			= imccmdline::ParmValue("-FILENAME", "");
	}

	if( imccmdline::FindParm("-ITOX") ) {
		g_convertType		= IESTOXML;
		g_isDeleteDir		= FALSE;
		g_encoding			= imccmdline::ParmValue("-ENCODING", "UTF-8");
	} else if( imccmdline::FindParm("-XTOI") ) {
		g_convertType		= XMLTOIES;
	} else {
		g_convertType		= NONETYPE;
	}
}

void GetDirFileList(const char * pSearchFile)
{
	WIN32_FIND_DATAW fileData; 
	BOOL fFinished = FALSE; 
	
	HANDLE hSearch = ::FindFirstFileW( boost::nowide::widen(pSearchFile).c_str(), &fileData ); 
	
	if(hSearch == INVALID_HANDLE_VALUE)
		return; 
	
	std::string tempFile = boost::nowide::narrow(fileData.cFileName);
	tempFile = tempFile.substr(0, tempFile.size()-4);
	if (g_fileNameList.Find(tempFile) == -1) {
		g_fileNameList.AddToTail(tempFile);
	}
	
	while(!fFinished) 
	{ 	
		if(!::FindNextFileW(hSearch, &fileData)) { 
			if(GetLastError() == ERROR_NO_MORE_FILES) {
				fFinished = TRUE; 
			} 
			else
				break;
		}
		else { 
			tempFile = boost::nowide::narrow(fileData.cFileName);
			tempFile = tempFile.substr(0, tempFile.size()-4);
			if (g_fileNameList.Find(tempFile) == -1) {
				g_fileNameList.AddToTail(tempFile);
			}
		} 
	} 	
	
	FindClose(hSearch);
}

bool Convert(CONVERT_TYPE type, const char * convFileName, const char * xmlPath, const char * iesPath)
{
	bool bResult = false;

	std::string xmlFullName, iesFullName;

	if(type == XMLTOIES) 
	{
		xmlFullName = xmlPath;
		xmlFullName += convFileName;
		xmlFullName += ".xml";
			
		iesFullName = iesPath;
		iesFullName += convFileName;
		iesFullName += ".ies";

		HRESULT hResult = ConvertXMLToIES(xmlFullName.c_str(), iesFullName.c_str());
		if(S_FALSE == hResult) {
			if (imc::StrIsSame(convFileName, "PC_LIST") || imc::StrIsSame(convFileName, "pc_list")) {
				printf("# 사내 개발용 XML 입니다. IPF 에 포함하지 않습니다.\n- %s\n", xmlFullName.c_str());
			} else {
				printf("# XML.IPF 에 포함이 되어야 할 파일입니다.\n- %s\n", xmlFullName.c_str());
			}
		} else if(S_OK == hResult) {
			bResult = imcIES::RegisterClassFromIES(iesFullName.c_str());
			if(!bResult) {
				printf("# 변환엔 성공하였으나 읽기 테스트에서 실패하였습니다. (문의주세요.)\n- %s\n", iesFullName.c_str());
				::DeleteFile(iesFullName.c_str());
				printf("IMC_DELETE ies file : %s\n", iesFullName.c_str());
			}
		} else if(S_TYPE_ERROR == hResult) {
			printf("- %s\n", xmlFullName.c_str());
		}
	} 
	else 
	{
		xmlFullName = iesPath;
		xmlFullName += convFileName;
		xmlFullName += "_conv.xml";
			
		iesFullName = iesPath;
		iesFullName += convFileName;
		iesFullName += ".ies";

		bResult		= ConvertIESToXML(iesFullName.c_str(), xmlFullName.c_str(), g_encoding.c_str());
		if(!bResult) {
			printf("- XML 로의 변환에 실패하였습니다. : %s\n", iesFullName.c_str());
		}
	}

	return bResult;
}

BOOL CreateIESDirectory(const char * iesPath)
{
	if (g_isDeleteDir) {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		char param[1024];
		sprintf_s(param, "/C del %s*.ies", iesPath);

		if( CreateProcess("C:/WINDOWS/system32/cmd.exe", param, NULL, NULL, FALSE, 0, NULL, NULL,&si,&pi))
		{
			WaitForSingleObject( pi.hProcess, INFINITE );
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
		}
	}

	return ::CreateDirectoryW(boost::nowide::widen(iesPath), NULL);
}



int main(int argc, char* argv[])
{
	ParseCmdLine( argc, argv );
	if(g_convertType == NONETYPE) 
	{
		printf(" * need option. ( -XTOI or -ITOX )\n");
		return 0;
	}
	
	imcfile::Init();
	imc::InitTimer(FALSE);
	imcfile::SetUsePak(false);

	std::string ConverterPath = imc::GetPath(imc::GetBinPath(), "LoggerConfig\\", "LoggerConfig_XMLConverter.xml");
	imccore::Init(ConverterPath.c_str());

	std::string xmlPath[5];
	xmlPath[0] = imc::GetDataPath("xml\\", NULL);
	xmlPath[1] = imc::GetDataPath("xml_server\\", NULL);
	xmlPath[2] = imc::GetDataPath("xml_server\\mongen\\", NULL);
	xmlPath[3] = imc::GetDataPath("xml_server\\position\\", NULL);
	xmlPath[4] = imc::GetDataPath("xml_client\\", NULL);
	std::string iesPath = imc::GetDataPath("ies\\", NULL);

	CreateIESDirectory(iesPath.c_str());

	if(g_isFullDirConvert) 
	{
		std::string searchFile;

		for( int i = 0; i < 5; ++i )
		{
			if(g_convertType == XMLTOIES) {
				searchFile = xmlPath[i];
				searchFile += "*.xml";
			} else {
				searchFile = iesPath;
				searchFile += "*.ies";
			}

			GetDirFileList(searchFile.c_str());

			int numFile = g_fileNameList.Count();
			for( int j = 0; j < numFile; ++j) {
				Convert(g_convertType, g_fileNameList[j].c_str(), xmlPath[i].c_str(), iesPath.c_str());
			}
			g_fileNameList.RemoveAll();
		}
	}
	else
	{
		if(g_fileName.empty()) 
		{
			printf(" * need option. ( -FILENAME *.xml or *.ies )\n");
		} 
		else 
		{
			std::string tempString = g_fileName.substr(g_fileName.size()-4);
			if( imc::StrIsSame(".xml", tempString.c_str())
			 || imc::StrIsSame(".ies", tempString.c_str())
			 || imc::StrIsSame(".XML", tempString.c_str())
			 || imc::StrIsSame(".IES", tempString.c_str()) )
			{
				g_fileName = g_fileName.substr(0, g_fileName.size()-4);
			}
			Convert(g_convertType, g_fileName.c_str(), xmlPath[0].c_str(), iesPath.c_str());
		}
	}
		
	imc::ExitTimer();
	imcIES::Reset();
	imcfile::Exit();

	printf("End.\n");
	char temp[1024] = {NULL};
	gets(temp);

	return 0;
}
