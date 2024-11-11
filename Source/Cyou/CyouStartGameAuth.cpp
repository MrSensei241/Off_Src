#include "stdafx.h"
#include "CyouStartGameAuth.h"


namespace Cyou
{
	/************************************************************************/
	/* ClientSDK 의 버전번호를 획득											*/
	/* return: char* - 버전번호 문자열										*/
	/************************************************************************/
	char* GetClientSDKVersion(){
		return cy_pt::cy_client_sdk_get_version();
	}

	/************************************************************************/
	/* Launcher가 cmd커맨드 라인을 통하여 게임 클라단이 실행시 발송하는
	파라미터 메시지를 분석함. 해당 함수는 블로킹 함수임.					*/
	/************************************************************************/
	bool GetInfo(CStringW commandLine, cy_pt::COMMANDINFOW* info){
		return cy_pt::get_info_from_cmdlineW(commandLine.GetBuffer(), commandLine.GetLength(), info);
	}

	/************************************************************************/
	/* 회원 머신 코드를 획득함. 별도로 스레드를 오픈하여 해당 함수를 호출 권장함 */
	/************************************************************************/
	int getUserMachineCode(wchar_t szMcode[MCODE_MAX_LENGTH + 1]){
		return cy_pt::get_machine_codeW(szMcode, MCODE_MAX_LENGTH + 1);
	}

	/************************************************************************/
	/* 창유 인증을 통한 클라이언트 실행 여부를 판단.						*/
	/************************************************************************/
	bool CyouClientRunAuth(CStringW lpCmdLine, cy_pt::COMMANDINFOW& info, int& nMcodeRealLen, wchar_t* szMcode){
		//char* clientSDKVersion = Cyou::GetClientSDKVersion();	// step2
		bool result = Cyou::GetInfo(lpCmdLine, &info);			// step3
		if (false == result) {
			//IMC_ERROR("ERRCODE_LOGIC_CHN_COMMANDLINE_PARSING_FAIL", "CommandLine:%s", lpCmdLine.GetString());
			return false;										// step11 (exit client)
		}
		else{													// step5 start
			memset(szMcode, 0, 1024);
			nMcodeRealLen = getUserMachineCode(szMcode);			
			//nMcodeRealLen = cy_pt::get_machine_codeW(szMcode, sizeof(szMcode)); // step6
			if (0 != nMcodeRealLen) {							// step7 start
				return true;
			}
			else {
				//IMC_ERROR("ERRCODE_LOGIC_CHN_MACHINECODE_GAIN_FAIL", "szMcode:%s", szMcode);
				return false;									// step11 (exit client)				
			}
		}
	}
};