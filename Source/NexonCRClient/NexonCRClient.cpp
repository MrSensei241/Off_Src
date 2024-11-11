// NexonCRClient.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "NexonCRClient.h"
#include "CrashReporter.h"
#include "screenshotEx.h"

extern "C"
{
	DLLFunction void _cdecl CrashReporter_Init(const wchar_t* projectId, LiveAssist_imc::CRASH_BEHAVIOR behavior/* = LiveAssist_imc::SENDSERVER_MINIDUMP*/)
	{
		LiveAssist::CrashReporter_Init(projectId, (LiveAssist::CRASH_BEHAVIOR) behavior);
	}

	DLLFunction bool _cdecl CrashReporter_IsInit()
	{
		return LiveAssist::CrashReporter_IsInit();
	}

	DLLFunction void _cdecl CrashReporter_RegisterLogFile(const wchar_t* filePath)
	{
		LiveAssist::CrashReporter_RegisterLogFile(filePath);
	}

	DLLFunction void _cdecl CrashReporter_RegisterCallback_BeforeDump(LiveAssist_imc::CrashReporter_CallBackFunc callbackFunc)
	{
		LiveAssist::CrashReporter_RegisterCallback_BeforeDump((LiveAssist::CrashReporter_CallBackFunc) callbackFunc);
	}

	DLLFunction void _cdecl CrashReporter_RegisterCallback_AfterDump(LiveAssist_imc::CrashReporter_CallBackFunc callbackFunc)
	{
		LiveAssist::CrashReporter_RegisterCallback_AfterDump((LiveAssist::CrashReporter_CallBackFunc) callbackFunc);
	}

	DLLFunction void _cdecl CrashReporter_RegisterClientModule(const wchar_t* modulePath)
	{
		LiveAssist::CrashReporter_RegisterClientModule(modulePath);
	}

	DLLFunction void _cdecl CrashReporter_SetReceiveServerRegion(LiveAssist_imc::CRASH_RECEIVE_SERVER_REGION region)
	{
		LiveAssist::CrashReporter_SetReceiveServerRegion((LiveAssist::CRASH_RECEIVE_SERVER_REGION)region);
	}

	DLLFunction void _cdecl CrashReporter_SetClientExit(int exitType/* = 0*/)
	{
		LiveAssist::CrashReporter_SetClientExit(exitType);
	}

	DLLFunction void _cdecl CrashReporter_SetSendDump(bool isSend)
	{
		LiveAssist::CrashReporter_SetSendDump(isSend);
	}

	DLLFunction void _cdecl CrashReporter_SetDumpFileName(const wchar_t* fileName)
	{
		LiveAssist::CrashReporter_SetDumpFileName(fileName);
	}

	DLLFunction void _cdecl CrashReporter_SetFullDumpPath(const wchar_t* fullPath)
	{
		LiveAssist::CrashReporter_SetFullDumpPath(fullPath);
	}

	DLLFunction void _cdecl CrashReporter_SetSSL(bool isHttps)
	{
		LiveAssist::CrashReporter_SetSSL(isHttps);
	}

	DLLFunction void _cdecl CrashReporter_SetGatherCrashScreenShot(bool isGather)
	{
		LiveAssist::CrashReporter_SetGatherCrashScreenShot(isGather);
	}

	DLLFunction void _cdecl CrashReporter_SetStrInfoW(const wchar_t* key, const wchar_t* value)
	{
		LiveAssist::CrashReporter_SetStrInfoW(key, value);
	}

	DLLFunction void _cdecl CrashReporter_SetStrInfoA(const wchar_t* key, const char* value)
	{
		LiveAssist::CrashReporter_SetStrInfoA(key, value);
	}

	DLLFunction void _cdecl CrashReporter_SetBoolInfo(const wchar_t* key, const bool &value)
	{
		LiveAssist::CrashReporter_SetBoolInfo(key, value);
	}

	DLLFunction void _cdecl CrashReporter_SetIntInfo(const wchar_t* key, const int &value)
	{
		LiveAssist::CrashReporter_SetIntInfo(key, value);
	}

	DLLFunction void _cdecl CrashReporter_SetInt64Info(const wchar_t* key, const __int64 &value)
	{
		LiveAssist::CrashReporter_SetInt64Info(key, value);
	}

	DLLFunction void _cdecl CrashReporter_SetUintInfo(const wchar_t* key, const unsigned int &value)
	{
		LiveAssist::CrashReporter_SetUintInfo(key, value);
	}

	DLLFunction void _cdecl CrashReporter_SetJsonInfoW(const wchar_t* key, const wchar_t* jsonStr)
	{
		LiveAssist::CrashReporter_SetJsonInfoW(key, jsonStr);
	}

	DLLFunction void _cdecl CrashReporter_SetJsonInfoA(const wchar_t* key, const char* jsonStr)
	{
		LiveAssist::CrashReporter_SetJsonInfoA(key, jsonStr);
	}

	DLLFunction bool _cdecl CrashReporter_GetInfoW(const wchar_t* key, wchar_t* outBuffer, int outBufferSize)
	{
		return LiveAssist::CrashReporter_GetInfoW(key, outBuffer, outBufferSize);
	}

	DLLFunction bool _cdecl CrashReporter_GetInfoA(const wchar_t* key, char* outBuffer, int outBufferSize)
	{
		return LiveAssist::CrashReporter_GetInfoA(key, outBuffer, outBufferSize);
	}

	DLLFunction bool _cdecl CrashReporter_SendCurrentState(const wchar_t* extraInfo)
	{
		return LiveAssist::CrashReporter_SendCurrentState(extraInfo);
	}

	DLLFunction bool _cdecl ScreenShot_Init(LiveAssist_imc::D3DDeviceFunc func)
	{
		return LiveAssist::ScreenShot_Init(func);
	}

	DLLFunction void _cdecl ScreenShot_Update()
	{
		LiveAssist::ScreenShot_Update();
	}

	DLLFunction void _cdecl ScreenShot_Clear()
	{
		LiveAssist::ScreenShot_Clear();
	}

	DLLFunction void _cdecl ScreenShot_Destroy()
	{
		LiveAssist::ScreenShot_Destroy();
	}

	DLLFunction void _cdecl ScreenShot_SetFormat(LiveAssist_imc::Screenshot_Format format)
	{
		LiveAssist::ScreenShot_SetFormat(static_cast<LiveAssist::Screenshot_Format>(format));
	}

	DLLFunction void _cdecl ScreenShot_SetJpegQuality(int quality)
	{
		LiveAssist::ScreenShot_SetJpegQuality(quality);
	}

	DLLFunction void _cdecl ScreenShot_SetQuickCamActive(bool isActive)
	{
		LiveAssist::ScreenShot_SetQuickCamActive(isActive);
	}

	DLLFunction void _cdecl ScreenShot_SetSavePathW(const wchar_t* dirPath)
	{
		LiveAssist::ScreenShot_SetSavePathW(dirPath);
	}

	DLLFunction void _cdecl ScreenShot_SetSavePathA(const char* dirPath)
	{
		LiveAssist::ScreenShot_SetSavePathA(dirPath);
	}

	DLLFunction void _cdecl ScreenShot_ShootW(const wchar_t* filePath, const char* desc)
	{
		LiveAssist::ScreenShot_ShootW(filePath, desc);
	}

	DLLFunction void _cdecl ScreenShot_ShootA(const char* filePath, const char* desc)
	{
		LiveAssist::ScreenShot_ShootA(filePath, desc);
	}

	DLLFunction void _cdecl ScreenShot_Shoot_ImmediateW(const wchar_t* filePath, const char* desc)
	{
		LiveAssist::ScreenShot_Shoot_ImmediateW(filePath, desc);
	}

	DLLFunction void _cdecl ScreenShot_Shoot_ImmediateA(const char* filePath, const char* desc)
	{
		LiveAssist::ScreenShot_Shoot_ImmediateA(filePath, desc);
	}

	DLLFunction void _cdecl ScreenShot_Shoot_7W(const wchar_t* fileNameFormat)
	{
		LiveAssist::ScreenShot_Shoot_7W(fileNameFormat);
	}

	DLLFunction void _cdecl ScreenShot_Shoot_7A(const char* fileNameFormat)
	{
		LiveAssist::ScreenShot_Shoot_7A(fileNameFormat);
	}

	DLLFunction int	_cdecl ScreenShot_GetDescriptionW(const wchar_t* filePath, char* buffer, int bufferSize)
	{
		return LiveAssist::ScreenShot_GetDescriptionW(filePath, buffer, bufferSize);
	}

	DLLFunction int	_cdecl ScreenShot_GetDescriptionA(const char* filePath, char* buffer, int bufferSize)
	{
		return LiveAssist::ScreenShot_GetDescriptionA(filePath, buffer, bufferSize);
	}

	DLLFunction void _cdecl ScreenShot_CustomCallBack(LiveAssist_imc::CustomFunc func)
	{
		LiveAssist::ScreenShot_CustomCallBack(func);
	}

	DLLFunction unsigned char* _cdecl ScreenShot_Custom_GetBuffer(int& width, int& height)
	{
		return LiveAssist::ScreenShot_Custom_GetBuffer(width, height);
	}

	DLLFunction void _cdecl ScreenShot_Custom_DrawBuffer(const unsigned char* buffer, int x, int y, int width, int height)
	{
		LiveAssist::ScreenShot_Custom_DrawBuffer(buffer, x, y, width, height);
	}

	DLLFunction void _cdecl ScreenShot_SetVerifyLevel(LiveAssist_imc::Verify_Level level)
	{
		LiveAssist::ScreenShot_SetVerifyLevel(static_cast<LiveAssist::Verify_Level>(level));
	}

	DLLFunction void _cdecl ScreenShot_ResetPathConfig()
	{
		LiveAssist::ScreenShot_ResetPathConfig();
	}
}