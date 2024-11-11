#pragma once

#include <windows.h>
#include "CyouCallbackInfo.h"
#include "CYPtSDK.h"


using namespace cy_pt;

struct FCM_RET{
	int nRetCode;
	double dEarning;

	FCM_RET(){
		nRetCode = -1;
		dEarning = -1;
	}
};

struct CYOU_SDK{
	bool flag;
	CYPTSESSION* pCYPTSESSION;
	char* szConfigFilePath;
	int32_t nConfigFilePathLen;

	CYOU_SDK(){
		flag = false;
		nConfigFilePathLen = sizeof("CYConfig.ini");
		szConfigFilePath = new char[nConfigFilePathLen];
		strncpy_s(szConfigFilePath, nConfigFilePathLen, "CYConfig.ini", nConfigFilePathLen);
		//strncpy_s(szConfigFilePath, sizeof(szConfigFilePath), "CYConfig.ini", nConfigFilePathLen);
		pCYPTSESSION = new CYPTSESSION();		
	}

	~CYOU_SDK(){
		if (szConfigFilePath)
		{
			delete szConfigFilePath;
			szConfigFilePath = nullptr;
		}

		if (pCYPTSESSION)
		{
			delete pCYPTSESSION;
			pCYPTSESSION = nullptr;
		}
	}
};


struct cyou_input_param_user_data{
	cy_pt::CYPTSESSION* pCYPTSESSION;
	char ip[50];
	char Mcode[128];
	char Token[128];
	CyouCallbackInfo_UserData* cyou_callback_info;

	cyou_input_param_user_data(){
		pCYPTSESSION = nullptr;
		memset(ip, 0, sizeof(ip));
		memset(Mcode, 0, sizeof(Mcode));
		memset(Token, 0, sizeof(Token));		
		cyou_callback_info = nullptr;
	}
};

struct cyou_input_param_fcm_heartbeat{
	cy_pt::CYPTSESSION* pCYPTSESSION;
	char* szMcode;
	char* szUserIp;
	char* szCnMaster;
	char* szFcmData;
	char* szLoginSeq;
	CyouCallbackInfo_fcm* cyou_callback_info;

	cyou_input_param_fcm_heartbeat(){
		pCYPTSESSION = nullptr;
		szMcode = nullptr;
		szUserIp = nullptr;
		szCnMaster = nullptr;
		szFcmData = nullptr;
		szLoginSeq = nullptr;
		cyou_callback_info = nullptr;
	}
};

struct cyou_input_param_server_notify{
	cy_pt::CYPTSESSION* pCYPTSESSION;
	int32_t nOnlineUserNum;
	int32_t nQueueingUserNum;
	char *szPrivateData;
	int32_t nPrivateDataLen;

	cyou_input_param_server_notify(){
		pCYPTSESSION = nullptr;
		nOnlineUserNum = 0;
		nQueueingUserNum = 0;
		szPrivateData = nullptr;
		nPrivateDataLen = 0;
	}
};

struct cyou_input_param_user_heartbeat{
	CYPTSESSION* pCYPTSESSION;
	char* szMcode;
	int32_t nMcodeLen;
	char* szUserIp;
	int32_t nUserIpLen;
	char* szCnMaster;
	int32_t nCnMasterLen;
	char* szRoleGuid;
	int32_t nRoleGuidLen;
	char* szRoleName;
	int32_t nRoleNameLen;
	int32_t nRoleLevel;
	int32_t nOnlineTime;
	char* szLoginSeq;
	int32_t nLoginSeqLen;

	cyou_input_param_user_heartbeat(){
		pCYPTSESSION = nullptr;
		szMcode = nullptr;
		nMcodeLen = 0;
		szUserIp = nullptr;
		nUserIpLen = 0;
		szCnMaster = nullptr;
		nCnMasterLen = 0;
		szRoleGuid = nullptr;
		nRoleGuidLen = 0;
		szRoleName = nullptr;
		nRoleNameLen = 0;
		nRoleLevel = 0;
		nOnlineTime = 0;
		szLoginSeq = nullptr;
		nLoginSeqLen = 0;
	}
};

struct cyou_input_param_role_status_notify{
	CYPTSESSION* pCYPTSESSION;
	char* szMcode;
	int32_t nMcodeLen;
	char* szUserIp;
	int32_t nUserIpLen;
	char* szCnMaster;
	int32_t nCnMasterLen;
	char* szEventType;
	int32_t nEventTypeLen;
	char* szRoleGuid;
	int32_t nRoleGuidLen;
	char* szRoleName;
	int32_t nRoleNameLen;
	int32_t nRoleLevel;
	int32_t nOnlineTime;
	char* szLoginSeq;
	int32_t nLoginSeqLen;
	void* parameter;

	cyou_input_param_role_status_notify(){
		pCYPTSESSION = nullptr;
		szMcode = nullptr;
		nMcodeLen = 0;
		szUserIp = nullptr;
		nUserIpLen = 0;
		szCnMaster = nullptr;
		nCnMasterLen = 0;
		szEventType = nullptr;
		nEventTypeLen = 0;
		szRoleGuid = nullptr;
		nRoleGuidLen = 0;
		szRoleName = nullptr;
		nRoleNameLen = 0;
		nRoleLevel = 0;
		nOnlineTime = 0;
		szLoginSeq = nullptr;
		nLoginSeqLen = 0;
		parameter = nullptr;
	}
};

// 회원이 소비 성공 후 호출함
struct cyou_input_param_item_buy
{
	CYPTSESSION* pCYPTSESSION;
	char* szMcode;
	int32_t nMcodeLen;
	char* szUserIp;
	int32_t nUserIpLen;
	char* szCnMaster;
	int32_t nCnMasterLen;
	char* szRoleGuid;
	int32_t nRoleGuidLen;
	char* szRoleName;
	int32_t nRoleNameLen;
	int32_t nRoleLevel;
	char* szOperation;
	int32_t nOperationLen;
	char* szBuyItems;
	int32_t nBuyItemsLen;
	char* szLoginSeq;
	int32_t nLoginSeqLen;

	cyou_input_param_item_buy()
	{
		pCYPTSESSION = nullptr;
		szMcode = nullptr;
		nMcodeLen = 0;
		szUserIp = nullptr;
		nUserIpLen = 0;
		szCnMaster = nullptr;
		nCnMasterLen = 0;
		szRoleGuid = nullptr;
		nRoleGuidLen = 0;
		szRoleName = nullptr;
		nRoleNameLen = 0;
		nRoleLevel = 0;
		szOperation = 0;
		nOperationLen = 0;
		szBuyItems = nullptr;
		nBuyItemsLen = 0;
		szLoginSeq = nullptr;
		nLoginSeqLen = 0;
	}
};

struct cyou_input_param_point_consume
{
	CYPTSESSION* pCYPTSESSION;
	char* szUserIp; int32_t nUserIpLen;
	char* szMcode; int32_t nMcodeLen;
	char* szCnMaster; int32_t nCnMasterLen;
	char* szRoleGuid; int32_t nRoleGuidLen;
	char* szRoleName; int32_t nRoleNameLen;
	int32_t nRoleLevel;
	char* szBuySeq; int32_t nBuySeqLen;
	char* szCostPoint; int32_t nCostPointLen;
	char* szBuyItems; int32_t nBuyItemsLen;
	char* szLoginSeq; int32_t nLoginSeqLen;
	CyouCallbackInfo_point_consume* cyou_callback_info;
	
	cyou_input_param_point_consume()
	{
		pCYPTSESSION = nullptr;
		szUserIp = nullptr;
		nUserIpLen = 0;
		szMcode = nullptr;
		nMcodeLen = 0;
		szCnMaster = nullptr;
		nCnMasterLen = 0;
		szRoleGuid = nullptr;
		nRoleGuidLen = 0;
		szRoleName = nullptr;
		nRoleNameLen = 0;
		nRoleLevel = 0;
		szBuySeq = nullptr;
		nBuySeqLen = 0;
		szCostPoint = nullptr;
		nCostPointLen = 0;
		szBuyItems = nullptr;
		nBuyItemsLen = 0;
		szLoginSeq = nullptr;
		nLoginSeqLen = 0;
		cyou_callback_info = nullptr;
	}
};

struct cyou_input_param_get_user_point
{
	CYPTSESSION* pCYPTSESSION;
	char* szUserIp;
	int32_t nUserIpLen;
	char* szMcode;
	int32_t nMcodeLen;
	char* szCnMaster;
	int32_t nCnMasterLen;
	char* szLoginSeq;
	int32_t nLoginSeqLen;
	CyouCallbackInfo_point_consume* cyou_callback_info;

	cyou_input_param_get_user_point()
	{
		pCYPTSESSION = nullptr;
		szUserIp = nullptr;
		nUserIpLen = 0;
		szMcode = nullptr;
		nMcodeLen = 0;
		szCnMaster = nullptr;
		nCnMasterLen = 0;
		szLoginSeq = nullptr;
		nLoginSeqLen = 0;
		cyou_callback_info = nullptr;
	}
};