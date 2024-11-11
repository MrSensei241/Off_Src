#pragma once

#include "LiveAssistEnum.h"
#ifdef NEXONCRCLIENT_EXPORTS
#define DLLFunction _declspec(dllexport)
#else
#define DLLFunction _declspec(dllimport)
#endif

extern "C"
{
	DLLFunction void _cdecl CrashReporter_Init(const wchar_t* projectId, LiveAssist_imc::CRASH_BEHAVIOR behavior = LiveAssist_imc::SENDSERVER_MINIDUMP);	// 크래시리포터 초기화 (라이브러리 안쪽에서 SetUnhandledExceptionFilter 셋팅)
	DLLFunction bool _cdecl CrashReporter_IsInit();															// 초기화되었는지의 여부 리턴

	DLLFunction void _cdecl CrashReporter_RegisterLogFile(const wchar_t* filePath);							// 수집할 클라이언트 로그 파일 등록
	DLLFunction void _cdecl CrashReporter_RegisterCallback_BeforeDump(LiveAssist_imc::CrashReporter_CallBackFunc callbackFunc);		// 덤프생성 전에 불릴 콜백 등록
	DLLFunction void _cdecl CrashReporter_RegisterCallback_AfterDump(LiveAssist_imc::CrashReporter_CallBackFunc callbackFunc);		// 덤프전송 이후에 불릴 콜백 등록
	DLLFunction void _cdecl CrashReporter_RegisterClientModule(const wchar_t* modulePath);					// FPS의 game.dll 등 PDB가 제공되어 덤프 분석이 이루어지는 클라이언트 모듈을 추가합니다.

	DLLFunction void _cdecl CrashReporter_SetReceiveServerRegion(LiveAssist_imc::CRASH_RECEIVE_SERVER_REGION region);			// 덤프 수집서버의 지역을 설정
	DLLFunction void _cdecl CrashReporter_SetClientExit(int exitType = 0);								// 클라이언트 정상 종료 명시 (이 함수가 호출된 후에 발생하는 크래시는 종료팅으로 분류되어 콜스택 리스트에서 표시(딤드) 처리됩니다.)
	DLLFunction void _cdecl CrashReporter_SetSendDump(bool isSend);										// 특정시점 이후 덤프전송을 하지않기위해 사용하는 API함수 ex) 종료시점 이후 크래시는 수집하지 않는다.
	DLLFunction void _cdecl CrashReporter_SetDumpFileName(const wchar_t* fileName);						// 최종 덤프 zip파일네임 변경 (default : dump.zip)
	DLLFunction void _cdecl CrashReporter_SetFullDumpPath(const wchar_t* fullPath);						// 풀덤프 파일경로를 지정한다. (주의 : path는 풀패스로 지정해준다) (default :  "./Dump_YYYYMMDD-hhmmss.dmp")
	DLLFunction void _cdecl CrashReporter_SetSSL(bool isHttps);											// SSL처리 (HTTPS를 사용하기위한 API) (주의 : 사용하는 포트가 변경되므로 적용시 인프라기술팀에 문의)	
	DLLFunction void _cdecl CrashReporter_SetGatherCrashScreenShot(bool isGather);

	// * Add BaseInfo Data (크래시시점 부가 정보(로그) 추가 / BaseInfo.json에 정보 추가) (부가 정보의 종류는 https://confluence.nexon.com/pages/viewpage.action?pageId=4390981 참조)
	DLLFunction void _cdecl CrashReporter_SetStrInfoW(const wchar_t* key, const wchar_t* value);		// 클라이언트 부가 정보를 설정 (stirng)
	DLLFunction void _cdecl CrashReporter_SetStrInfoA(const wchar_t* key, const char* value);				// 클라이언트 부가 정보를 설정 (string)
	DLLFunction void _cdecl CrashReporter_SetBoolInfo(const wchar_t* key, const bool &value);				// 클라이언트 부가 정보를 설정 (bool)
	DLLFunction void _cdecl CrashReporter_SetIntInfo(const wchar_t* key, const int &value);				// 클라이언트 부가 정보를 설정 (int)
	DLLFunction void _cdecl CrashReporter_SetInt64Info(const wchar_t* key, const __int64 &value);				// 클라이언트 부가 정보를 설정 (int64)
	DLLFunction void _cdecl CrashReporter_SetUintInfo(const wchar_t* key, const unsigned int &value);

	DLLFunction void _cdecl CrashReporter_SetJsonInfoW(const wchar_t* key, const wchar_t* jsonStr);		// 클라이언트 추가 정보를 설정 (JSON string)
	DLLFunction void _cdecl CrashReporter_SetJsonInfoA(const wchar_t* key, const char* jsonStr);			// 클라이언트 추가 정보를 설정 (JSON string)

	// * Get BaseInfo Data
	DLLFunction bool _cdecl CrashReporter_GetInfoW(const wchar_t* key, wchar_t* outBuffer, int outBufferSize);
	DLLFunction bool _cdecl CrashReporter_GetInfoA(const wchar_t* key, char* outBuffer, int outBufferSize);

	DLLFunction bool _cdecl CrashReporter_SendCurrentState(const wchar_t* extraInfo);

	// ScreenShot
	// * Initailize & Update
	DLLFunction bool _cdecl ScreenShot_Init(LiveAssist_imc::D3DDeviceFunc func);	//스크린샷 모듈 초기화 (func : 유효한 D3DDevice인터페이스를 받아올 수 있는 콜백함수)
	DLLFunction void _cdecl ScreenShot_Update();					//매 프레임마다 호출 (연속 스크린샷 찍기 등에 활용된다)
	DLLFunction void _cdecl ScreenShot_Clear();					//클라이언트 종료나, DeviceLost등에서 QuickCam에 의해 남은 서피스들을 정리
	DLLFunction void _cdecl ScreenShot_Destroy();					//클라이언트 종료시점에 쓰레드를 안정하게 정리한다. (ScreenShot_Clear도 같이 호출함) 
					 
	// * Setting
	DLLFunction void _cdecl ScreenShot_SetFormat(LiveAssist_imc::Screenshot_Format format);	//스크린샷 포멧을 설정
	DLLFunction void _cdecl ScreenShot_SetJpegQuality(int quality);			//Jpeg포멧의 경우 퀄리티를 설정
	DLLFunction void _cdecl ScreenShot_SetQuickCamActive(bool isActive);				//Quickcam(연속촬영) 활성화
					 
	DLLFunction void _cdecl ScreenShot_SetSavePathW(const wchar_t* dirPath);			//스크린샷 저장경로 설정
	DLLFunction void _cdecl ScreenShot_SetSavePathA(const char* dirPath);				//스크린샷 저장경로 설정
					 
	// * Take Screenshot
	DLLFunction void _cdecl ScreenShot_ShootW(const wchar_t* filePath, const char* desc = NULL);		//스크린샷찍기 (desc : 메타정보 ASCII만 가능)
	DLLFunction void _cdecl ScreenShot_ShootA(const char* filePath, const char* desc = NULL);			//스크린샷찍기 (desc : 메타정보 ASCII만 가능)
					 
	DLLFunction void _cdecl ScreenShot_Shoot_ImmediateW(const wchar_t* filePath, const char* desc = NULL);		//메인쓰레드 에서 스크린샷찍기 (desc : 메타정보 ASCII만 가능)
	DLLFunction void _cdecl ScreenShot_Shoot_ImmediateA(const char* filePath, const char* desc = NULL);			//메인쓰레드 에서 스크린샷찍기 (desc : 메타정보 ASCII만 가능)
					 
	DLLFunction void _cdecl ScreenShot_Shoot_7W(const wchar_t* fileNameFormat);	//Quickcam으로 7장 연속으로 스크린샷 찍기
	DLLFunction void _cdecl ScreenShot_Shoot_7A(const char* fileNameFormat);		//Quickcam으로 7장 연속으로 스크린샷 찍기

	DLLFunction int	_cdecl ScreenShot_GetDescriptionW(const wchar_t* filePath, char* buffer, int bufferSize);	//이미지에서 메타정보 가져오기 (반환값 : 가져온 데이터길이)
	DLLFunction int	_cdecl ScreenShot_GetDescriptionA(const char* filePath, char* buffer, int bufferSize);		//이미지에서 메타정보 가져오기 (반환값 : 가져온 데이터길이)

	// * Custom
	DLLFunction void _cdecl ScreenShot_CustomCallBack(LiveAssist_imc::CustomFunc func);		//개발팀에서 스크린샷을 변경할 수 있도록 인터페이스 지원
	//------------------------CustomFunc 안에서만 부를 수 있는 함수들
	DLLFunction unsigned char* _cdecl ScreenShot_Custom_GetBuffer(int& width, int& height);			//버퍼가져오기
	DLLFunction void _cdecl ScreenShot_Custom_DrawBuffer(const unsigned char* buffer, int x, int y, int width, int height); //간단한 이미지 붙이기 - 리사이즈 불가
	//------------------------CustomFunc------------------------

	// * Verify
	DLLFunction void _cdecl ScreenShot_SetVerifyLevel(LiveAssist_imc::Verify_Level level);		//스크린샷 검증레벨을 설정 0:없음, 1:체크썸, 2:고수준검증루틴

	// * CrashReporter 에서만 호출
	DLLFunction void _cdecl ScreenShot_ResetPathConfig();							//스크린샷 저장경로 리셋하는 기능, 크래시리포터에서만 호출한다.
}
