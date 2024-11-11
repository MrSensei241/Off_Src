// CyouCPForClient.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "CyouCPForClient.h"

extern "C" DLLFunction int CProtect_OnInit_Wrap(const char *pUserMark, tdMSGCALLBACK pfnMsgCallBack)
{		
	return CProtect_OnInit(pUserMark, pfnMsgCallBack);
}

extern "C" DLLFunction int CProtect_OnTick_Wrap()
{	
	return CProtect_OnTick();
}