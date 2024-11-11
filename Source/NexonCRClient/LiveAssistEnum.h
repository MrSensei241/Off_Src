#pragma once

#include <d3d9.h>

extern "C"
{
	namespace LiveAssist_imc
	{
		enum CRASH_BEHAVIOR
		{							// 크래시가 날 경우의 동작
			DO_NOTHING = 0,			// 아무것도 하지 않음
			SAVELOCAL_MINIDUMP,		// 로컬에 미니덤프만 생성 (서버 전송 x)
			SAVELOCAL_FULLDUMP,		// 로컬에 풀덤프만 생성 (서버 전송 x)
			SENDSERVER_MINIDUMP,	// 미니덤프 생성 / 서버로 전송 후 덤프 파일 삭제 (default)
			SAVELOCAL_FULLDUMP_AND_SENDSERVER_MINIDUMP,		// 미니덤프/풀덤프 생성 후 미니덤프만 서버로 전송 (전송 후 미니덤프 파일 삭제 / 풀덤프 유지)
		};

		enum CRASH_RECEIVE_SERVER_REGION
		{
			RECEIVE_SERVER_NEXON,		// 넥슨 내부서버 (default)
			RECEIVE_SERVER_AWS_ASIA,	// AWS - ASIA (아시아)
			RECEIVE_SERVER_AWS_EU,		// AWS - EU (유럽)
			RECEIVE_SERVER_AWS_US,		// AWS - US (북미)
		};

		typedef void(*CrashReporter_CallBackFunc)();		// exceptionCode : _EXCEPTION_RECORD::ExceptionCode (winnt.h)

		enum Screenshot_Format
		{
			SCREENSHOT_BMP,
			SCREENSHOT_JPG,
			SCREENSHOT_PNG,
		};

		enum Buffer_Format
		{
			R = 0,
			G,
			B,
			A,
			PIXEL_SIZE
		};

		enum Verify_Level
		{
			VerifyNone = 0,
			VerifyBasic,
			VerifyAdvanced		// 아직 구현중
		};

		typedef IDirect3DDevice9*(__stdcall *D3DDeviceFunc)();
		typedef void(__stdcall *CustomFunc)();
	}
}