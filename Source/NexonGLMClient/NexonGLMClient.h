#pragma once

#ifdef NEXONGLMCLIENT_EXPORTS
#define DLLFunction _declspec(dllexport)
#else
#define DLLFunction _declspec(dllimport)
#endif

extern "C"{
	
	DLLFunction BOOL _cdecl InitializeGameLogManager_IMCDLL(LPCSTR szGameName, LPCSTR szBaseUrl, LPCSTR szSessionID, LPCSTR szServiceID, LPCSTR binPath);
	DLLFunction void _cdecl FinalizeGameLogManager_IMCDLL();
	DLLFunction BOOL _cdecl WriteStageLog_IMCDLL(INT32 nStage, LPCSTR szComment);
	DLLFunction BOOL _cdecl WriteErrorLog_IMCDLL(INT32 nErrorType, LPCSTR szComment);
	DLLFunction LPCSTR _cdecl GetSessionID_IMCDLL(LPSTR szSessionID, INT32 cchSessionID);
	

}

