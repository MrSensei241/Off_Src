// CyouCPForClient.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
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