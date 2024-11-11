#pragma once

#include <windows.h>
#include "CyouCTUTypeDef.h"

struct CYOU_SDK_USER_INFO{		// global <--> zone transfer information
	char ip[50];
	char machineCode[128];
	char securityToken[128];
	char CnMaster[128];
	char LoginSeq[512];
	char FcmData[512];
	int nSecurityFlags;
	int nUserHeartBeatInterval;
	int nFcmHeartBeatInterval;
	bool bFcmFlag;
	bool bCanBeDeleted;
	unsigned long accessTime;
	unsigned short barrack_id;
	
	bool bIsAssigned;
	UINT64 aid;
	unsigned long timer;
	double earningRate;
	DWORD timer_nFcmNotify;
	bool penaltyList[(int)(CyouCTURetFlag)FLAG_END_INDEX];


	CYOU_SDK_USER_INFO(){
		memset(ip, 0, sizeof(ip));
		memset(machineCode, 0, sizeof(machineCode));
		memset(securityToken, 0, sizeof(securityToken));
		memset(CnMaster, 0, sizeof(CnMaster));
		memset(LoginSeq, 0, sizeof(LoginSeq));
		memset(FcmData, 0, sizeof(FcmData));
		nSecurityFlags = 0;
		nUserHeartBeatInterval = 0;
		nFcmHeartBeatInterval = 0;
		accessTime = 0;
		barrack_id = 0;
		aid = 0;
		bFcmFlag = false;
		bCanBeDeleted = false;
		bIsAssigned = false;
		timer = 0;
		earningRate = 1.0;
		timer_nFcmNotify = 0;
		memset(penaltyList, 0, sizeof(penaltyList));
	}

	void print(){
// 		IMC_NORMAL_INFO("CYOU_SDK_USER_INFO : ip(%s), machineCode(%s), securityToken(%s), CnMaster(%s), \
// 			LoginSeq(%s), FcmData(%s), nSecurityFlags(%d), nUserHeartBeatInterval(%d), nFcmHeartBeatInterval(%d),\
// 			bFcmFlag(%d), bCanBeDeleted(%d), accessTime(%d), barrack_id(%d), aid(%I64u), earningRate(%.2f)",
// 			ip, machineCode, securityToken, CnMaster, LoginSeq, FcmData, nSecurityFlags, nUserHeartBeatInterval, nFcmHeartBeatInterval
// 			, bFcmFlag, bCanBeDeleted, accessTime, barrack_id, aid, earningRate);
	}
};