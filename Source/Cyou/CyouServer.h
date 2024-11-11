#pragma once
#include "CYPtSDK.h"
#include "CYTypeDef.h"
#include "CYstdint.h"
#include <atlstr.h>
#include <atlconv.h>
#include "CyouCallbackInfo.h"
#include "CyouTypeDefine.h"

using namespace cy_pt;

class CyouServer
{
public:
	CyouServer();
	virtual ~CyouServer();

	static bool cy_log_callback(CY_LOG_TYPE LogType, char *szLogData, int32_t nLogDataLen);
	static bool on_user_data_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen, int32_t nSecurityFlags,
		bool  bFcmFlag, char* szFcmData, int32_t nFcmDataLen, char* szLoginSeq, int32_t nLoginSeqLen, int32_t nUserHeartBeatInterval,
		int32_t nFcmHeartBeatInterval, void* parameter);
	static bool on_security_token_invalid_callback(int32_t nRetCode, char* szSecurityToken, int32_t nSecurityTokenLen, char* szMessage, int32_t nMessageLen,
		void* parameter);
	
	static bool on_user_verify_motp_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szMotpMsg, int32_t nMotpMsgLen, void* parameter);
	static bool on_user_verify_motp_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szMotpMsg, int32_t nMotpMsgLen, void* parameter);
	
	static bool on_user_kick_out_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char* szMessage, int32_t nMessageLen, void* parameter);
	static bool on_show_fcm_notify_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen, char* szMessage, int32_t nMessageLen,
		void* parameter);
	static bool on_change_user_earning_rate_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		double dEarningRate, void* parameter);


	static bool on_user_kick_out_with_role_status_notify_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char* szMessage, int32_t nMessageLen, void* parameter);
	
	static bool on_user_point_consume_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szBuySeq, int32_t nBuySeqLen, char *szLoginSeq, int32_t nLoginSeqLen, char *szLeftPoint, int32_t nLeftPointLen,
		void* parameter);
	static bool on_user_point_consume_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen, char *szBuySeq, int32_t nBuySeqLen, char *szLoginSeq, int32_t nLoginSeqLen,
		char *szMessage, int32_t nMessageLen, void* parameter);
	
	static bool on_get_point_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szLoginSeq, int32_t nLoginSeqLen, char *szLeftPoint, int32_t nLeftPointLen,
		void* parameter);
	static bool on_get_point_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szLoginSeq, int32_t nLoginSeqLen, char *szMessage, int32_t nMessageLen,
		void* parameter);
	
	static bool on_get_user_vip_level_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szLoginSeq, int32_t nLoginSeqLen, uint64_t nVipLevel, uint64_t nAccumulatedPoint,
		uint64_t nNextLevelNeedPoint, void* parameter);
	static bool on_get_user_vip_level_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szLoginSeq, int32_t nLoginSeqLen, char *szMessage, int32_t nMessageLen,
		void* parameter);
	
	static bool on_get_user_credit_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szLoginSeq, int32_t nLoginSeqLen, uint64_t nCredit, void* parameter);
	static bool on_get_user_credit_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szLoginSeq, int32_t nLoginSeqLen, char *szMessage, int32_t nMessageLen, void* parameter);
	
	static bool on_user_credit_consume_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szBuySeq, int32_t nBuySeqLen, char *szLoginSeq, int32_t nLoginSeqLen,
		char *szLeftCredit, int32_t nLeftCreditLen, void* parameter);
	static bool on_user_credit_consume_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
		char *szBuySeq, int32_t nBuySeqLen, char *szLoginSeq, int32_t nLoginSeqLen,
		char *szMessage, int32_t nMessageLen, void* parameter);

	
	// Cyou 패킹 관련 SDK
	bool createCyouSession(CYOU_SDK* cyou_sdk);
	bool destroyCyouSession(CYOU_SDK* cyou_sdk);
	//void clearCyouCallbackInfo(Cyou_Callback_Info* info);
	static bool gs_get_user_data(cyou_input_param_user_data* info);
	static bool gs_fcm_heartbeat(cyou_input_param_fcm_heartbeat* info);
	static bool gs_server_status_notify(cyou_input_param_server_notify* info);
	static bool gs_user_heartbeat(cyou_input_param_user_heartbeat* info);
	static bool gs_role_status_notify(cyou_input_param_role_status_notify* info, bool isLogin);	
	static bool gs_item_buy_record(cyou_input_param_item_buy* info);
	static bool gs_user_point_consume(cyou_input_param_point_consume* info);
	static bool gs_get_user_point(cyou_input_param_get_user_point* info);

	CYOU_SDK* cyou_sdk;
		
	static bool GetUseCTUServer();
};
