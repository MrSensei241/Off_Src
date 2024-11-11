#pragma once

/*
창유 Client SDK 연동
step1: Launcher가 cmd 방식을 통해 client_tos.exe를 실행 (실행시에 cmd를 통해 client로 메시지 발송)
step2: client에서 cy_client_sdk_get_version 함수를 호출하여 clientSDK 버전 번호를 획득
step3: cmd 라인에서 받은 파리미터를 분석, get_info_from_cmdlineW 함수를 호출하여 security token 및 기타 정보를 획득함
Step4: 게임 클라단이 get_info_from_cmdlineW 함수 호출여부 및 실행성공여부를 판단하며 성공할 경우 Step5를 실행하고 실패할 경우 Step11를 실행함.
Step5: 게임 클라단이 get_machine_codeW함수를 호출하여 회원 클라단 머신코드를 획득함.
*/

#include "CYClientSDK.h"
#include "CYTypeDef.h"
#include "CYstdint.h"

#define MCODE_MAX_LENGTH 255

namespace Cyou
{
	/************************************************************************/
	/* ClientSDK 의 버전번호를 획득											*/
	/* return: char* - 버전번호 문자열										*/
	/************************************************************************/
	char* GetClientSDKVersion();

	/************************************************************************/
	/* Launcher가 cmd커맨드 라인을 통하여 게임 클라단이 실행시 발송하는
	파라미터 메시지를 분석함. 해당 함수는 블로킹 함수임.					*/
	/************************************************************************/
	bool GetInfo(CStringW commandLine, cy_pt::COMMANDINFOW* info);

	/************************************************************************/
	/* 회원 머신 코드를 획득함. 별도로 스레드를 오픈하여 해당 함수를 호출 권장함 */
	/************************************************************************/
	int getUserMachineCode(wchar_t szMcode[MCODE_MAX_LENGTH + 1], int nMcodeLen);

	/************************************************************************/
	/* 창유 인증을 통한 클라이언트 실행 여부를 판단.						*/
	/************************************************************************/
	bool CyouClientRunAuth(CStringW lpCmdLine, cy_pt::COMMANDINFOW& info, int& nMcodeRealLen, wchar_t* szMcode);
};
static cy_pt::COMMANDINFOW info;
static int nMcodeRealLen = 0;
static wchar_t szMcode[1024];