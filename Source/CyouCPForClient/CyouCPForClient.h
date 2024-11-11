#pragma once

// CyouCPForClient.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//

#include "stdafx.h"
#include "CPC.h"

#define DLLFunction __declspec(dllexport)

extern "C" DLLFunction int CProtect_OnInit_Wrap(const char *pUserMark, tdMSGCALLBACK pfnMsgCallBack);
extern "C" DLLFunction int CProtect_OnTick_Wrap();