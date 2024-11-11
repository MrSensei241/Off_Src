// NexonGLMClient.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//



#include "stdafx.h"

#include "NexonGLMClient.h"
#include "ClientLogWriter.h"

INIT_LOGWRITER_GLOVAL_VAL

DLLFunction BOOL _cdecl InitializeGameLogManager_IMCDLL(LPCSTR szGameName, LPCSTR szBaseUrl, LPCSTR szSessionID, LPCSTR binPath)
{
	if (LoadClientLogWriter(binPath))
	{
		return InitializeClientLogManager(szGameName, szSessionID, "430007405", 4, "", false); //�츮 ������ ���� �н� �ܿ��� W() �Ⱦ���. ���� A()
	}
	else
	{
		return false;
	}
}

DLLFunction void _cdecl FinalizeGameLogManager_IMCDLL(){

	//FinalizeGameLogManager();
	FinalizeClientLogManager();
	UnLoadClientLogWriter();
}

DLLFunction BOOL _cdecl WriteStageLog_IMCDLL(INT32 nStage, LPCSTR szComment){
	return WriteStageLog(nStage, szComment);
}

DLLFunction BOOL _cdecl WriteErrorLog_IMCDLL(INT32 nErrorType, LPCSTR szComment){
	return WriteErrorLog(nErrorType, szComment);
}

DLLFunction LPCSTR _cdecl GetSessionID_IMCDLL(LPSTR szSessionID, INT32 cchSessionID){
	return GetSessionID(szSessionID, cchSessionID);
}