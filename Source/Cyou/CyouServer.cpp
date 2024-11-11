#include "StdAfx.h"
#include "CyouServer.h"
#include <functional>
#include <algorithm>
#include "CTUFlag.h"

CyouServer::CyouServer()
{	
	cyou_sdk = new CYOU_SDK();	
}


CyouServer::~CyouServer()
{	
	if (cyou_sdk)
	{
		delete cyou_sdk;
		cyou_sdk = nullptr;
	}	
}

bool CyouServer::cy_log_callback(CY_LOG_TYPE LogType, char *szLogData, int32_t nLogDataLen)
{ 
	LogType = CY_LOG_TYPE_INFO;
	szLogData = nullptr;
	nLogDataLen = 0;
	return true; 
}

bool CyouServer::on_user_data_callback(int32_t nRetCode,
	char* szCnMaster, int32_t nCnMasterLen,
	int32_t nSecurityFlags,
	bool  bFcmFlag,
	char* szFcmData, int32_t nFcmDataLen,
	char* szLoginSeq, int32_t nLoginSeqLen,
	int32_t nUserHeartBeatInterval,
	int32_t nFcmHeartBeatInterval,
	void* parameter)
{

	CyouCallbackInfo_UserData* info = (CyouCallbackInfo_UserData*)parameter;
	info->isSuccessCalled = true;
	info->nRetCode = nRetCode;

	info->nCnMasterLen = nCnMasterLen;
	strncpy_s(info->szCnMaster, sizeof(info->szCnMaster), szCnMaster, nCnMasterLen + 1);	

	info->nSecurityFlags = nSecurityFlags;
	info->bFcmFlag = bFcmFlag;

	info->nFcmDataLen = nFcmDataLen;
	strncpy_s(info->szFcmData, sizeof(info->szFcmData), szFcmData, nFcmDataLen + 1);

	info->nLoginSeqLen = nLoginSeqLen;
	strncpy_s(info->szLoginSeq, sizeof(info->szLoginSeq), szLoginSeq, nLoginSeqLen + 1);

	info->nUserHeartBeatInterval = nUserHeartBeatInterval;
	info->nFcmHeartBeatInterval = nFcmHeartBeatInterval;

	return true;
}

bool CyouServer::on_security_token_invalid_callback(int32_t nRetCode,
	char* szSecurityToken, int32_t nSecurityTokenLen,
	char* szMessage, int32_t nMessageLen,
	void* parameter)
{
	szSecurityToken = nullptr;
	szMessage = nullptr;
	nMessageLen = 0;
	nSecurityTokenLen = 0;
	CyouCallbackInfo_UserData* info = (CyouCallbackInfo_UserData*)parameter;
	info->isFailCalled = true;
	info->nRetCode = nRetCode;	

	return true;
}

bool CyouServer::on_user_verify_motp_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szMotpMsg, int32_t nMotpMsgLen, void* parameter)
{
	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szMotpMsg = nullptr;
	nMotpMsgLen = 0;
	parameter = nullptr;
	return true;
}

bool CyouServer::on_user_verify_motp_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szMotpMsg, int32_t nMotpMsgLen, void* parameter)
{
	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szMotpMsg = nullptr;
	nMotpMsgLen = 0;
	parameter = nullptr;
	return true;
}

// asyn_gs_fcm_heartbeat -- 수익 변경
bool CyouServer::on_change_user_earning_rate_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen, double dEarningRate, void* parameter)
{

	CyouCallbackInfo_fcm* info = (CyouCallbackInfo_fcm*)parameter;

	info->nRetCode = nRetCode;
	strncpy_s(info->szCnMaster, sizeof(info->szCnMaster), szCnMaster, strlen(szCnMaster) + 1);
	info->nCnMasterLen = nCnMasterLen;
	info->dEarningRate = dEarningRate;
	info->isCalledEarning = true;

	return true;
}

// asyn_gs_fcm_heartbeat -- 유저 킥
bool CyouServer::on_user_kick_out_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen, char* szMessage, int32_t nMessageLen, void* parameter)
{

	CyouCallbackInfo_fcm* info = (CyouCallbackInfo_fcm*)parameter;

	info->nRetCode = nRetCode;
	strncpy_s(info->szCnMaster, sizeof(info->szCnMaster), szCnMaster, strlen(szCnMaster) + 1);
	info->nCnMasterLen = nCnMasterLen;
	strncpy_s(info->szMessage, sizeof(info->szMessage), szMessage, strlen(szMessage) + 1);
	info->nMessageLen = nMessageLen;
	info->isCalledKick = true;

	return true;
}

//asyn_gs_fcm_heartbeat -- 메시지 알림
bool CyouServer::on_show_fcm_notify_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen, char* szMessage, int32_t nMessageLen, void* parameter)
{
	szMessage = nullptr;
	nMessageLen = 0;
	CyouCallbackInfo_fcm* info = (CyouCallbackInfo_fcm*)parameter;

	info->nRetCode = nRetCode;		
	info->isCalledNotify = true;

	strncpy_s(info->szMessage, sizeof(info->szMessage), szMessage, strlen(szMessage) + 1);
	info->nMessageLen = nMessageLen;
	
	strncpy_s(info->szCnMaster, sizeof(info->szCnMaster), szCnMaster, strlen(szCnMaster) + 1);
	info->nCnMasterLen = nCnMasterLen;
	
	return true;
}

// asyn_gs_role_status_notify -- 캐릭터 로그인/아웃 시에 호출
bool CyouServer::on_user_kick_out_with_role_status_notify_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char* szMessage, int32_t nMessageLen, void* parameter)
{
	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szMessage = nullptr;
	nMessageLen = 0;
	parameter = nullptr;
	return true;
}

///////////////////////////////////// asyn_gs_user_point_consume 호출시 콜백함 ///////////////////////////////////
bool CyouServer::on_user_point_consume_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szBuySeq, int32_t nBuySeqLen, char *szLoginSeq, int32_t nLoginSeqLen,
	char *szLeftPoint, int32_t nLeftPointLen, void* parameter){

	CyouCallbackInfo_point_consume* info = (CyouCallbackInfo_point_consume*)parameter;

	info->isSuccessCalled = true;
	info->nRetCode = nRetCode;	
	strncpy_s(info->szCnMaster, sizeof(info->szCnMaster), szCnMaster, strlen(szCnMaster + 1));
	info->nCnMasterLen = nCnMasterLen;
	info->szBuySeq = szBuySeq;
	info->nBuySeqLen = nBuySeqLen;
	strncpy_s(info->szLoginSeq, sizeof(info->szLoginSeq), szLoginSeq, strlen(szLoginSeq + 1));
	info->nLoginSeqLen = nLoginSeqLen;
	info->szLeftPoint = szLeftPoint;
	info->nLeftPointLen = nLeftPointLen;
	return true;
}

bool CyouServer::on_user_point_consume_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szBuySeq, int32_t nBuySeqLen, char *szLoginSeq, int32_t nLoginSeqLen,
	char *szMessage, int32_t nMessageLen, void* parameter){

	CyouCallbackInfo_point_consume* info = (CyouCallbackInfo_point_consume*)parameter;

	info->isFailCalled = true;
	info->nRetCode = nRetCode;
	strncpy_s(info->szCnMaster, sizeof(info->szCnMaster), szCnMaster, strlen(szCnMaster + 1));
	info->nCnMasterLen = nCnMasterLen;
	info->szBuySeq = szBuySeq;
	info->nBuySeqLen = nBuySeqLen;
	strncpy_s(info->szLoginSeq, sizeof(info->szLoginSeq), szLoginSeq, strlen(szLoginSeq + 1));
	info->nLoginSeqLen = nLoginSeqLen;
	strncpy_s(info->szMessage, sizeof(info->szMessage), szMessage, strlen(szMessage + 1));	
	info->nMessageLen = nMessageLen;
	return true;
}
///////////////////////////////////// asyn_gs_user_point_consume 호출시 콜백함 ///////////////////////////////////

//////////////////////////////////////// asyn_gs_get_user_point 호출시 콜백함 ///////////////////////////////////
bool CyouServer::on_get_point_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szLoginSeq, int32_t nLoginSeqLen, char *szLeftPoint, int32_t nLeftPointLen,
	void* parameter){

	CyouCallbackInfo_point_consume* info = (CyouCallbackInfo_point_consume*)parameter;
	info->isSuccessCalled = true;
	info->nRetCode = nRetCode;
	strncpy_s(info->szCnMaster, sizeof(info->szCnMaster), szCnMaster, strlen(szCnMaster + 1));
	info->nCnMasterLen = nCnMasterLen;	
	strncpy_s(info->szLoginSeq, sizeof(info->szLoginSeq), szLoginSeq, strlen(szLoginSeq + 1));
	info->nLoginSeqLen = nLoginSeqLen;
	info->szLeftPoint = szLeftPoint;
	info->nLeftPointLen = nLeftPointLen;
	return true;
}

bool CyouServer::on_get_point_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szLoginSeq, int32_t nLoginSeqLen, char *szMessage, int32_t nMessageLen,
	void* parameter){

	CyouCallbackInfo_point_consume* info = (CyouCallbackInfo_point_consume*)parameter;
	info->isFailCalled = true;
	info->nRetCode = nRetCode;
	strncpy_s(info->szCnMaster, sizeof(info->szCnMaster) ,szCnMaster, strlen(szCnMaster + 1));
	info->nCnMasterLen = nCnMasterLen;
	strncpy_s(info->szLoginSeq, sizeof(info->szLoginSeq), szLoginSeq, strlen(szLoginSeq + 1));
	info->nLoginSeqLen = nLoginSeqLen;
	strncpy_s(info->szMessage, sizeof(info->szMessage), szMessage, strlen(szMessage + 1));
	info->nMessageLen = nMessageLen;
	return true;
}
/////////////////////////////////////// asyn_gs_get_user_point 호출시 콜백함 ////////////////////////////////////

bool CyouServer::on_get_user_vip_level_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szLoginSeq, int32_t nLoginSeqLen, uint64_t nVipLevel, uint64_t nAccumulatedPoint,
	uint64_t nNextLevelNeedPoint, void* parameter)
{
	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szLoginSeq = nullptr;
	nLoginSeqLen = 0;
	nVipLevel = 0;
	nAccumulatedPoint = 0;
	nNextLevelNeedPoint = 0;
	parameter = nullptr;
	return true;
}

bool CyouServer::on_get_user_vip_level_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szLoginSeq, int32_t nLoginSeqLen, char *szMessage, int32_t nMessageLen,
	void* parameter){

	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szLoginSeq = nullptr;
	nLoginSeqLen = 0;
	szMessage = nullptr;
	nMessageLen = 0;
	parameter = nullptr;
	return true;
}

bool CyouServer::on_get_user_credit_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szLoginSeq, int32_t nLoginSeqLen, uint64_t nCredit, void* parameter){
	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szLoginSeq = nullptr;
	nLoginSeqLen = 0;
	nCredit = 0;
	parameter = nullptr;	
	return true;
}

bool CyouServer::on_get_user_credit_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szLoginSeq, int32_t nLoginSeqLen, char *szMessage, int32_t nMessageLen, void* parameter){
	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szLoginSeq = nullptr;
	nLoginSeqLen = 0;
	szMessage = nullptr;
	nMessageLen = 0;
	parameter = nullptr;
	return true;
}

bool CyouServer::on_user_credit_consume_success_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szBuySeq, int32_t nBuySeqLen, char *szLoginSeq, int32_t nLoginSeqLen,
	char *szLeftCredit, int32_t nLeftCreditLen, void* parameter)
{
	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szBuySeq = nullptr;
	nBuySeqLen = 0;
	szLoginSeq = nullptr;
	nLoginSeqLen = 0;	
	szLeftCredit = nullptr;
	nLeftCreditLen = 0;
	parameter = nullptr;

	return true;
}

bool CyouServer::on_user_credit_consume_fail_callback(int32_t nRetCode, char* szCnMaster, int32_t nCnMasterLen,
	char *szBuySeq, int32_t nBuySeqLen, char *szLoginSeq, int32_t nLoginSeqLen,
	char *szMessage, int32_t nMessageLen, void* parameter)
{
	nRetCode = 0;
	szCnMaster = nullptr;
	nCnMasterLen = 0;
	szBuySeq = nullptr;
	nBuySeqLen = 0;
	szLoginSeq = nullptr;
	nLoginSeqLen = 0;
	szMessage = nullptr;
	nMessageLen = 0;
	parameter = nullptr;
	
	return true;
}

/************************************************************************/
/* 글로벌서버 Init() 에서 호출한다. (GlobalServer/GlobalServer.cpp)		*/
/************************************************************************/
bool CyouServer::createCyouSession(CYOU_SDK* cyou_sdk){	
	bool ret = asyn_create_cy_pt_session(
		cyou_sdk->pCYPTSESSION,
		cy_log_callback,
		on_user_data_callback,
		on_security_token_invalid_callback,
		on_change_user_earning_rate_callback,
		on_user_verify_motp_success_callback,
		on_user_verify_motp_fail_callback,
		on_user_kick_out_callback,
		on_show_fcm_notify_callback,
		on_user_kick_out_with_role_status_notify_callback,
		on_user_point_consume_success_callback,
		on_user_point_consume_fail_callback,
		on_get_point_success_callback,
		on_get_point_fail_callback,
		on_get_user_vip_level_success_callback,
		on_get_user_vip_level_fail_callback,
		on_get_user_credit_success_callback,
		on_get_user_credit_fail_callback,
		on_user_credit_consume_success_callback,
		on_user_credit_consume_fail_callback,
		cyou_sdk->szConfigFilePath,
		cyou_sdk->nConfigFilePathLen
		);
	return ret;
}

/************************************************************************/
/* 글로벌서버 Exit() 에서 호출한다. (GlobalServer/GlobalServer.cpp)		*/
/************************************************************************/
bool CyouServer::destroyCyouSession(CYOU_SDK* cyou_sdk){	
	return asyn_distroy_cy_pt_session((cyou_sdk->pCYPTSESSION));
}

// 콜백을 받아야 한다면 호출하는 쪽에서 초기화를 해주자. (isSuccessCalled or bCanCallAsynProcess ...)
bool CyouServer::gs_get_user_data(cyou_input_param_user_data* info)
{
	//LOG_LINE("gs_get_user_data : %p, %s, %s, %s, %p", info->pCYPTSESSION, info->ip, info->Mcode, info->Token, info->cyou_callback_info);
	info->cyou_callback_info->readyToCall();
	bool ret = cy_pt::asyn_gs_get_user_data(info->pCYPTSESSION,
		info->ip, strlen(info->ip),
		info->Mcode, strlen(info->Mcode),
		info->Token, strlen(info->Token),
		info->cyou_callback_info);
	if(ret) info->cyou_callback_info->bCanCallAsynProcess = true;
	//LOG_LINE("asyn_gs_get_user_data() == %s", ret ? "True" : "False");
	cy_pt::asyn_process_cy_pt_session(info->pCYPTSESSION);
	return ret;
}

bool CyouServer::gs_fcm_heartbeat(cyou_input_param_fcm_heartbeat* info)
{
	info->cyou_callback_info->readyToCall();
	bool ret = cy_pt::asyn_gs_fcm_heartbeat(info->pCYPTSESSION,
		info->szUserIp, strlen(info->szUserIp),
		info->szMcode, strlen(info->szMcode),
		info->szCnMaster, strlen(info->szCnMaster),
		info->szFcmData, strlen(info->szFcmData),
		info->szLoginSeq, strlen(info->szLoginSeq),
		info->cyou_callback_info
		);
	// 성공적으로 호출되었으니 asyn_process 를 콜해도 된다.
	if (ret) info->cyou_callback_info->bCanCallAsynProcess = true;
	cy_pt::asyn_process_cy_pt_session(info->pCYPTSESSION);
	return ret;
}

bool CyouServer::gs_server_status_notify(cyou_input_param_server_notify* info)
{
	bool ret = cy_pt::asyn_gs_server_status_notify(info->pCYPTSESSION,
		info->nOnlineUserNum,
		info->nQueueingUserNum,
		info->szPrivateData,
		info->nPrivateDataLen		
		);	
	cy_pt::asyn_process_cy_pt_session(info->pCYPTSESSION);
	return ret;
}

bool CyouServer::gs_user_heartbeat(cyou_input_param_user_heartbeat* info)
{
	bool ret = cy_pt::asyn_gs_user_heartbeat(info->pCYPTSESSION,
		info->szUserIp, info->nUserIpLen,
		info->szMcode, info->nMcodeLen,
		info->szCnMaster, info->nCnMasterLen,
		info->szRoleGuid, info->nRoleGuidLen,
		info->szRoleName, info->nRoleNameLen,
		info->nRoleLevel, 
		info->nOnlineTime,
		info->szLoginSeq, info->nLoginSeqLen
		);
	cy_pt::asyn_process_cy_pt_session(info->pCYPTSESSION);
	return ret;
}

// call when character online or offline
bool CyouServer::gs_role_status_notify(cyou_input_param_role_status_notify* info, bool isLogin)
{
	char* szEventType = nullptr;
	if (isLogin){
		szEventType = "online";
	}
	else{
		szEventType = "offline";
	}
	info->szEventType = szEventType;
	info->nEventTypeLen = strlen(szEventType);
	bool ret = cy_pt::asyn_gs_role_status_notify(info->pCYPTSESSION,
		info->szUserIp, info->nUserIpLen,
		info->szMcode, info->nMcodeLen,
		info->szCnMaster, info->nCnMasterLen,
		info->szEventType, info->nEventTypeLen,
		info->szRoleGuid, info->nRoleGuidLen,
		info->szRoleName, info->nRoleNameLen,
		info->nRoleLevel, info->nOnlineTime,
		info->szLoginSeq, info->nLoginSeqLen,
		info->parameter
		);
	cy_pt::asyn_process_cy_pt_session(info->pCYPTSESSION);
	return ret;
}

bool CyouServer::gs_item_buy_record(cyou_input_param_item_buy* info)
{
	bool ret = cy_pt::asyn_gs_item_buy_record(
		info->pCYPTSESSION,
		info->szUserIp, info->nUserIpLen,
		info->szMcode, info->nMcodeLen,
		info->szCnMaster, info->nCnMasterLen,
		info->szRoleGuid, info->nRoleGuidLen,
		info->szRoleName, info->nRoleNameLen,
		info->nRoleLevel,
		info->szOperation, info->nOperationLen,
		info->szBuyItems, info->nBuyItemsLen,
		info->szLoginSeq, info->nLoginSeqLen
		);
	cy_pt::asyn_process_cy_pt_session(info->pCYPTSESSION);
	return ret;
}

bool CyouServer::gs_user_point_consume(cyou_input_param_point_consume* info)
{
	info->cyou_callback_info->readyToCall();
	bool ret = cy_pt::asyn_gs_user_point_consume(
		info->pCYPTSESSION,
		info->szUserIp, info->nUserIpLen,
		info->szMcode, info->nMcodeLen,
		info->szCnMaster, info->nCnMasterLen,
		info->szRoleGuid, info->nRoleGuidLen,
		info->szRoleName, info->nRoleNameLen,
		info->nRoleLevel,
		info->szBuySeq, info->nBuySeqLen,
		info->szCostPoint, info->nCostPointLen,
		info->szBuyItems, info->nBuyItemsLen,
		info->szLoginSeq, info->nLoginSeqLen,
		info->cyou_callback_info
		);
	// 정상 호출이면 asyn_process 을 콜할 수 있도록 설정한다.
	if (ret) info->cyou_callback_info->bCanCallAsynProcess = true;
	cy_pt::asyn_process_cy_pt_session(info->pCYPTSESSION);
	return ret;
}

bool CyouServer::gs_get_user_point(cyou_input_param_get_user_point* info)
{
	info->cyou_callback_info->readyToCall();
	bool ret = cy_pt::asyn_gs_get_user_point(
		info->pCYPTSESSION,
		info->szUserIp, info->nUserIpLen,
		info->szMcode, info->nMcodeLen,
		info->szCnMaster, info->nCnMasterLen,
		info->szLoginSeq, info->nLoginSeqLen,
		info->cyou_callback_info
		);
	// 정상 호출이면 asyn_process 을 콜할 수 있도록 설정한다.
	if (ret) info->cyou_callback_info->bCanCallAsynProcess = true;
	cy_pt::asyn_process_cy_pt_session(info->pCYPTSESSION);
	return ret;
}

bool CyouServer::GetUseCTUServer()
{
	return useCTUServer;
}
