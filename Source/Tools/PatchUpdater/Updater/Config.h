#pragma once

namespace Config {
	void		Init();
	void		Exit();

	namespace URL {
		extern CString revisions;
		extern CString notice;
		extern CString home;
	}

	namespace String {
		extern CString connecting;
		extern CString checking;
		extern CString prepairing;
		extern CString downloading;
		extern CString updating;
		extern CString extracting;
		extern CString merging;
		extern CString arranging;
		extern CString canceling;
		extern CString complete;
		extern CString fail;
		extern CString cancelQuestion;
		extern CString exitQuestion;
		extern CString updaterConfigXMLNotExist;
		extern CString updaterCfgXMLError;
		extern CString failedFlashPlayerInit;
		extern CString duplicatedRunImpossible;
		extern CString block_msnshell;
		extern CString msnshellNotice;
		extern CString cannotOpenFlashOCX;
		extern CString failedConnectServer;
		extern CString failedDownLoadList;
		extern CString failedAnalysisDownLoadList;
		extern CString failedDownLoadFile;
		extern CString failedUpdateFile;
		extern CString failedDownLoadPrepareList;
		extern CString failedDownAnalyPrepareList;
		extern CString failedDownLoadIpfFileList;
		extern CString failedAnalyIpfFileList;	   
		extern CString failedDownLoadIpfFile;
		extern CString cannotRunLD_exe;
		extern CString changenamewarning;
		extern CString exportwarning;
	}	
}