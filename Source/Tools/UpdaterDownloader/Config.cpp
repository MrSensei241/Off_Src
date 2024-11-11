#include "stdafx.h"
#include "Config.h"
#include "XML.h"
#include "PatcherUpdater.h"

namespace Config {
	const char*	GetValue(XML::Nodes& node,const char* keyName)
	{
		XML::LPAttr key, value;
		for (int i=0; i<(int)node.size(); i++) {
			key = node[i]->GetAttr("Key");
			ASSERT(NULL!=key);

			if (key->value == keyName) {
				value = node[i]->GetAttr("Value");
				ASSERT(NULL!=value);
				return value->value;
			}
		}

		return NULL;
	}

	namespace String {
		CString connecting;
		CString checking;
		CString prepairing;
		CString downloading;
		CString updating;
		CString extracting;
		CString merging;
		CString arranging;
		CString canceling;
		CString complete;
		CString fail;
		CString cancelQuestion;
		CString exitQuestion;
		CString updaterConfigXMLNotExist;
		CString updaterCfgXMLError;
		CString failedFlashPlayerInit;
		CString duplicatedRunImpossible;
		CString block_msnshell;
		CString msnshellNotice;
		CString cannotOpenFlashOCX;
		CString failedConnectServer;
		CString failedDownLoadList;
		CString failedAnalysisDownLoadList;
		CString failedDownLoadFile;
		CString failedUpdateFile;
		CString failedDownLoadPrepareList;
		CString failedDownAnalyPrepareList;
		CString failedDownLoadIpfFileList;
		CString failedAnalyIpfFileList;	   
		CString failedDownLoadIpfFile;
		CString cannotRunLD_exe;
		CString changenamewarning;
		CString exportwarning;
	}

	namespace URL {
		CString revisions;
		CString notice;
		CString home;
		CString port;
	}

	void Init()
	{
		String::updaterConfigXMLNotExist = "updater.config.xml 파일을 열 수 없습니다. 파일이 손상되었거나 지워졌을 수 있으니 재설치 하시기 바랍니다.";

		CStdioFile file;
		FATAL_ERROR(TRUE==file.Open(g_fullPath + "updater.config.xml", CFile::modeRead), THIS_FILE, __LINE__, Config::String::updaterConfigXMLNotExist);
		
		CString xml, temp;
		while (TRUE) {
			if (FALSE == file.ReadString(temp))
				break;
			xml += temp;
		}

		file.Close();

		XML::PARSEINFO info;
		
		XML::Node root;
		root.Load(xml, &info);
		FATAL_ERROR(false==info.erorr_occur, THIS_FILE, __LINE__, Config::String::updaterConfigXMLNotExist);

		XML::Nodes url		= root.GetChilds("URL");
		URL::revisions		= GetValue(url, "Revisions");
		URL::notice			= GetValue(url, "Notice");
		URL::home			= GetValue(url, "Home");

// 		XML::Nodes string				   = root.GetChilds("String");
// 		String::connecting				   = GetValue(string, "Connecting");
// 		String::checking				   = GetValue(string, "Checking");
// 		String::prepairing				   = GetValue(string, "Prepairing");
// 		String::downloading				   = GetValue(string, "Downloading");
// 		String::updating				   = GetValue(string, "Updating");
// 		String::extracting				   = GetValue(string, "Extracting");
// 		String::merging					   = GetValue(string, "Merging");
// 		String::canceling				   = GetValue(string, "Canceling");
// 		String::complete				   = GetValue(string, "Complete");
// 		String::fail					   = GetValue(string, "Fail");
// 		String::cancelQuestion			   = GetValue(string, "CancelQuestion");
// 		String::exitQuestion			   = GetValue(string, "ExitQuestion");
// 		String::updaterConfigXMLNotExist   = GetValue(string, "UpdaterConfigXMLNotExist");
// 		String::updaterCfgXMLError		   = GetValue(string, "UpdaterConfigXMLError");
// 		String::failedFlashPlayerInit	   = GetValue(string, "FailedFlashPlayerInit");
// 		String::duplicatedRunImpossible	   = GetValue(string, "DuplicatedRunImpossible");
// 		String::block_msnshell  		   = GetValue(string, "Block_msnshell");
// 		String::msnshellNotice			   = GetValue(string, "Notice_msnshell");
// 		String::cannotOpenFlashOCX		   = GetValue(string, "CannotOpenFlashOCX");
// 		String::failedConnectServer		   = GetValue(string, "FailedConnectServer");
// 		String::failedDownLoadList		   = GetValue(string, "FailedDownLoadList");
// 		String::failedAnalysisDownLoadList = GetValue(string, "FailedAnalysisDownLoadList");
// 		String::failedDownLoadFile		   = GetValue(string, "FailedDownLoadFile");
// 		String::failedUpdateFile		   = GetValue(string, "FailedUpdateFile");
// 		String::failedDownLoadPrepareList  = GetValue(string, "FailedDownLoadPrepareList");
// 		String::failedDownAnalyPrepareList = GetValue(string, "FailedAnalyDnLoadPrepareList");
// 		String::failedDownLoadIpfFileList  = GetValue(string, "FailedDownLoadIpfFileList");
// 		String::failedAnalyIpfFileList	   = GetValue(string, "FailedAnalyIpfFileList");
// 		String::failedDownLoadIpfFile	   = GetValue(string, "FailedDownLoadIpfFile");
// 		String::cannotRunLD_exe			   = GetValue(string, "CannotRunLD_exe");
// 		String::changenamewarning		   = GetValue(string, "Arrangingwarning");
// 		String::exportwarning			   = GetValue(string, "Exportwarning");
// 		String::arranging				   = GetValue(string, "Arranging");
						
		root.Close();

		if(String::updaterCfgXMLError.GetLength() <= 0)
			String::updaterCfgXMLError = "잘못된 updater.config.xml 파일 입니다. 파일이 손상되었거나 지워졌을 수 있으니 재설치 하시기 바랍니다.";

		FATAL_ERROR(0<URL::revisions.GetLength(),						THIS_FILE, __LINE__, String::updaterCfgXMLError);
		FATAL_ERROR(0<URL::notice.GetLength(),							THIS_FILE, __LINE__, String::updaterCfgXMLError);
		FATAL_ERROR(0<URL::home.GetLength(),							THIS_FILE, __LINE__, String::updaterCfgXMLError);

// 		FATAL_ERROR(0<String::connecting.GetLength(),		 			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::checking.GetLength(),	 					THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::prepairing.GetLength(),	 				THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::downloading.GetLength(),		 			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::updating.GetLength(),	 					THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::extracting.GetLength(),	 				THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::merging.GetLength(),	 					THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::canceling.GetLength(),	 				THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::complete.GetLength(),	 					THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::fail.GetLength(),	 						THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::cancelQuestion.GetLength(),	 			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::exitQuestion.GetLength(),		 			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::updaterConfigXMLNotExist.GetLength(),		THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::updaterCfgXMLError.GetLength(),			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedFlashPlayerInit.GetLength(),		THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::duplicatedRunImpossible.GetLength(),		THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::block_msnshell.GetLength(),	 			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::msnshellNotice.GetLength(),	 			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::cannotOpenFlashOCX.GetLength(),			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedConnectServer.GetLength(),			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedDownLoadList.GetLength(),			THIS_FILE, __LINE__, String::updaterCfgXMLError);
//  		FATAL_ERROR(0<String::failedAnalysisDownLoadList.GetLength(),	THIS_FILE, __LINE__, String::updaterCfgXMLError);
//  		FATAL_ERROR(0<String::failedDownLoadFile.GetLength(),			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedUpdateFile.GetLength(),				THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedDownLoadPrepareList.GetLength(),	THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedDownAnalyPrepareList.GetLength(),	THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedDownLoadIpfFileList.GetLength(),	THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedAnalyIpfFileList.GetLength(),		THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::failedDownLoadIpfFile.GetLength(),		THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::cannotRunLD_exe.GetLength(),				THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::changenamewarning.GetLength(),			THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::exportwarning.GetLength(),				THIS_FILE, __LINE__, String::updaterCfgXMLError);
// 		FATAL_ERROR(0<String::arranging.GetLength(),					THIS_FILE, __LINE__, String::updaterCfgXMLError);
	}

	void Exit()
	{
	}
}
