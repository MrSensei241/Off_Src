#pragma once

#include "windows.h"
#include <string>
#include <sstream>
using namespace std;

/* 예제 입니다. (그대로 사용해도 됩니다)
   추가적인 게임 실행 단계 혹은 원하는 게임 실행 단계가
   있으면 INT32값으로 각 팀에서 정해서 사용하시면 됩니다.*/
enum GAME_START_STEP
{
	None = 0,
	WEB,
	LUNCHER,
	PATCHER,
	GAME
};

typedef void(*fnFinalize)();
typedef void(*fnSetNexonSN)(INT64 nNexonSN);
typedef void(*fnSetUserSN)(INT64 nUserSN);
typedef void(*fnSetGameStartStep)(INT32 nCurStep);

static const char* FinalizeGameLogManagerProcName = "FinalizeGameLogManager";
static const char* SetNexonSNFnName = "SetNexonSN";
static const char* SetUserSNFnName = "SetUserSN";
static const char* SetGameStartStepFnName = "SetGameStartStep";

// Debug용 함수
typedef void(*fnSetSendFlag)(BOOL bSendUXLog);
// typedef INT64(*fnGetNexonSN)();
static const char* SetSendFlagFnName = "SetSendFlag";
// static const char* GetNexonSNFnName = "GetNexonSN";

typedef BOOL(*fnInitialize)(LPCSTR szGameName, LPCSTR szSessionID, LPCSTR szServiceID, INT32 nCurStep, LPCSTR szAccountNo, bool writeFileLog);
typedef BOOL(*fnWriteStageLog)(INT32 nStage, LPCSTR szComment);
typedef BOOL(*fnWriteErrorLog)(INT32 nErrorType, LPCSTR szComment);
typedef LPCSTR(*fnGetSessionID)(LPSTR szSessionID, INT32 cchSessionID);
typedef void(*fnSetUserID)(LPCSTR szUserID);
typedef BOOL(*fnWriteLiveAPILog)(LPCSTR strLog);
typedef BOOL(*fnWriteLog)(LPCSTR strLog);
typedef BOOL(*fnWriteLogOncePerKey)(LPCSTR strKey, LPCSTR strLog);
typedef void(*fnSetCharacterID)(LPCSTR strCharacterID);
typedef void(*fnSetAccountNO)(LPCSTR szAccountNO);
typedef BOOL(*fnWriteABTest)(LPCSTR szTestKey, LPCSTR szTestGroup);

static const char* InitializeGameLogManagerProcName = "InitializeGameLogManagerA";
static const char* WriteStageLogFnName = "WriteStageLogA";
static const char* WriteErrorLogFnName = "WriteErrorLogA";
static const char* GetSessionIDFnName = "GetSessionIDA";
static const char* SetUserIDFnName = "SetUserIDA";
static const char* WriteLiveAPILogFnName = "WriteLiveAPILogA";
static const char* WriteLogFnName = "WriteLogA";
static const char* WriteLogOncePerKeyFnName = "WriteLogOncePerKeyA";
static const char* SetCharacterIdFnName = "SetCharacterIDA";
static const char* SetAccountNOFnName = "SetAccountNOA";
static const char* WriteABTestFnName = "WriteABTestA";



// 로그 함수 이름 (초기화)
extern fnInitialize __InitializeClientLogManager;
extern fnFinalize __FinalizeClientLogManager;
extern fnWriteStageLog __WriteStageLog;
extern fnWriteErrorLog __WriteErrorLog;
extern fnGetSessionID __GetSessionID;
extern fnSetUserID __SetUserID;
extern fnWriteLiveAPILog __WriteUXLog;
extern fnWriteLog __WriteClientLog;
extern fnWriteLogOncePerKey __WriteLogOncePerKey;
extern fnSetNexonSN __SetNexonSN;
extern fnSetUserSN __SetUserSN;
extern fnSetCharacterID __SetCharacterID;
extern fnSetSendFlag __SetSendLogFlag;
extern fnSetAccountNO __SetAccountNO;
extern fnWriteABTest __WriteABTest;
extern fnSetGameStartStep __SetGameStartStep;
// extern fnGetNexonSN GetNexonSN;

extern HMODULE LogWriterDLL;


/* DLL 로딩 실패했을 경우 처리하기 위해 아래와 같이 함수를 랩핑하도록 함. */
/* 각 프로젝트에서 사용하는 함수와 동일한 이름의 함수가 있으면 임의로 함수 이름을 변경해서 사용해도 무방합니다.*/

// InitializeClientLogmanager
static BOOL InitializeClientLogManager(LPCSTR szGameName, LPCSTR szSessionID, LPCSTR szServiceID, INT32 nCurStep, LPCSTR szAccountNo, bool writeFileLog)
{
	if (LogWriterDLL != NULL && __InitializeClientLogManager != NULL)
	{
		return __InitializeClientLogManager(szGameName, szSessionID, szServiceID, nCurStep, szAccountNo, writeFileLog);
	}
	return FALSE;
}

// FinalizeClientLogManager
static void FinalizeClientLogManager()
{
	if (LogWriterDLL != NULL && __FinalizeClientLogManager != NULL)
	{
		__FinalizeClientLogManager();
	}
}

// SetNexonSN
static void SetNexonSN(INT64 nNexonSN)
{
	if (LogWriterDLL != NULL && __SetNexonSN != NULL)
	{
		__SetNexonSN(nNexonSN);
	}
}
// SetUserSN
static void SetUserSN(INT64 nUserSN)
{
	if (LogWriterDLL != NULL && __SetUserSN != NULL)
	{
		__SetUserSN(nUserSN);
	}
}

// WriteStageLog
static BOOL WriteStageLog(INT32 nStage, LPCSTR szComment)
{
	if (LogWriterDLL != NULL && __WriteStageLog != NULL)
	{
		return __WriteStageLog(nStage, szComment);
	}
	return FALSE;
}
// WriteErrorLog
static BOOL WriteErrorLog(INT32 nErrorType, LPCSTR szComment)
{
	if (LogWriterDLL != NULL && __WriteStageLog != NULL)
	{
		return __WriteErrorLog(nErrorType, szComment);
	}
	return FALSE;
}

// GetSessionID
static LPCSTR GetSessionID(LPSTR szSessionID, INT32 cchSessionID)
{
	if (LogWriterDLL != NULL && __GetSessionID != NULL)
	{
		return __GetSessionID(szSessionID, cchSessionID);
	}
	return NULL;
}

// SetUserID
static void SetUserID(LPCSTR szUserID)
{
	if (LogWriterDLL != NULL && __SetUserID != NULL)
	{
		__SetUserID(szUserID);
	}
}

// WriteUXLog
static BOOL WriteUXLog(LPCSTR strLog)
{
	if (LogWriterDLL != NULL && __WriteUXLog != NULL)
	{
		return __WriteUXLog(strLog);
	}
	return FALSE;
}

// WriteClientLog
static BOOL WriteClientLog(LPCSTR strLog)
{
	if (LogWriterDLL != NULL && __WriteClientLog)
	{
		return __WriteClientLog(strLog);
	}
	return FALSE;
}

// WriteLogOncePerKey
static BOOL WriteLogOncePerKey(LPCSTR strKey, LPCSTR strLog)
{
	if (LogWriterDLL != NULL && __WriteLogOncePerKey != NULL)
	{
		return __WriteLogOncePerKey(strKey, strLog);
	}
	return FALSE;
}

// SetCharacterID
static void SetCharacterID(LPCSTR strCharacterID)
{
	if (LogWriterDLL != NULL && __SetCharacterID != NULL)
	{
		__SetCharacterID(strCharacterID);
	}
}

// SetAccountNO
static void SetAccountNO(LPCSTR szAccountNO)
{
	if (LogWriterDLL != NULL && __SetAccountNO != NULL)
	{
		__SetAccountNO(szAccountNO);
	}
}

// WriteABTest
static BOOL WriteABTest(LPCSTR szTestKey, LPCSTR szTestGroup)
{
	if (LogWriterDLL != NULL && __WriteABTest != NULL)
	{
		return __WriteABTest(szTestKey, szTestGroup);
	}
	return FALSE;
}

// Load DLL
static BOOL LoadClientLogWriter(const char* modulpath)
{
	LogWriterDLL = LoadLibraryA(modulpath);
	if (LogWriterDLL == NULL)
	{
		return FALSE;
	}

	__InitializeClientLogManager = (fnInitialize)::GetProcAddress(LogWriterDLL, InitializeGameLogManagerProcName);
	__FinalizeClientLogManager = (fnFinalize)::GetProcAddress(LogWriterDLL, FinalizeGameLogManagerProcName);
	__WriteStageLog = (fnWriteStageLog)::GetProcAddress(LogWriterDLL, WriteStageLogFnName);
	__WriteErrorLog = (fnWriteErrorLog)::GetProcAddress(LogWriterDLL, WriteErrorLogFnName);
	__GetSessionID = (fnGetSessionID)::GetProcAddress(LogWriterDLL, GetSessionIDFnName);
	__SetUserID = (fnSetUserID)::GetProcAddress(LogWriterDLL, SetUserIDFnName);
	__WriteUXLog = (fnWriteLiveAPILog)::GetProcAddress(LogWriterDLL, WriteLiveAPILogFnName);
	__WriteClientLog = (fnWriteLog)::GetProcAddress(LogWriterDLL, WriteLogFnName);
	__WriteLogOncePerKey = (fnWriteLogOncePerKey)::GetProcAddress(LogWriterDLL, WriteLogOncePerKeyFnName);
	__SetNexonSN = (fnSetNexonSN)::GetProcAddress(LogWriterDLL, SetNexonSNFnName);
	__SetUserSN = (fnSetUserSN)::GetProcAddress(LogWriterDLL, SetUserSNFnName);
	__SetCharacterID = (fnSetCharacterID)::GetProcAddress(LogWriterDLL, SetCharacterIdFnName);
	__SetSendLogFlag = (fnSetSendFlag)::GetProcAddress(LogWriterDLL, SetSendFlagFnName);
	__SetAccountNO = (fnSetAccountNO)::GetProcAddress(LogWriterDLL, SetAccountNOFnName);
	__WriteABTest = (fnWriteABTest)::GetProcAddress(LogWriterDLL, WriteABTestFnName);
	__SetGameStartStep = (fnSetGameStartStep)::GetProcAddress(LogWriterDLL, SetGameStartStepFnName);
	//	GetNexonSN = (fnGetNexonSN)::GetProcAddress(LogWriterDLL, GetNexonSNFnName);

	if (__InitializeClientLogManager == NULL
		|| __FinalizeClientLogManager == NULL
		|| __WriteStageLog == NULL
		|| __WriteErrorLog == NULL
		|| __GetSessionID == NULL
		|| __SetUserID == NULL
		|| __WriteUXLog == NULL
		|| __WriteClientLog == NULL
		|| __WriteLogOncePerKey == NULL
		|| __SetNexonSN == NULL
		|| __SetUserSN == NULL
		|| __SetCharacterID == NULL
		|| __SetSendLogFlag == NULL
		|| __SetAccountNO == NULL
		|| __WriteABTest == NULL
		|| __SetGameStartStep == NULL
		//		|| GetNexonSN == NULL
		)
	{
		FreeLibrary(LogWriterDLL);
		LogWriterDLL = NULL;
		return FALSE;
	}
	return TRUE;
}

static void UnLoadClientLogWriter()
{
	FinalizeClientLogManager();

	if (LogWriterDLL != NULL)
	{
		FreeLibrary(LogWriterDLL);
	}
	// 혹시 모를 호출에 대비해서 모두 NULL로 Setting!!
	__InitializeClientLogManager = NULL;
	__FinalizeClientLogManager = NULL;
	__WriteStageLog = NULL;
	__WriteErrorLog = NULL;
	__GetSessionID = NULL;
	__SetUserID = NULL;
	__WriteUXLog = NULL;
	__WriteClientLog = NULL;
	__WriteLogOncePerKey = NULL;
	__SetNexonSN = NULL;
	__SetUserSN = NULL;
	__SetCharacterID = NULL;
	__SetSendLogFlag = NULL;
	__SetAccountNO = NULL;
	__WriteABTest = NULL;
	__SetGameStartStep = NULL;
}

#define INIT_LOGWRITER_GLOVAL_VAL \
			HMODULE LogWriterDLL = NULL; \
			fnInitialize __InitializeClientLogManager = NULL; \
			fnFinalize __FinalizeClientLogManager = NULL; \
			fnWriteStageLog __WriteStageLog = NULL; \
			fnWriteErrorLog __WriteErrorLog = NULL; \
			fnGetSessionID __GetSessionID = NULL; \
			fnSetUserID __SetUserID = NULL; \
			fnWriteLiveAPILog __WriteUXLog = NULL; \
			fnWriteLog __WriteClientLog = NULL; \
			fnWriteLogOncePerKey __WriteLogOncePerKey = NULL; \
			fnSetNexonSN __SetNexonSN = NULL; \
			fnSetUserSN __SetUserSN = NULL; \
			fnSetCharacterID __SetCharacterID = NULL; \
			fnSetSendFlag __SetSendLogFlag = NULL; \
			fnSetAccountNO __SetAccountNO = NULL; \
			fnWriteABTest __WriteABTest = NULL; \
			fnSetGameStartStep __SetGameStartStep = NULL;
//			fnGetNexonSN GetNexonSN = NULL;
