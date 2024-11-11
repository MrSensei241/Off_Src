#include "StdAfx.h"
#include "CyouLoginInfo.h"
#include "CyouServer.h"
#include "MMSystem.h"
#include <boost/lexical_cast.hpp>
#include "imcCore.h"

CyouLoginInfo::CyouLoginInfo()
{
	barrack_id = 0;
	aid = 0;
	timer_nFcmHeartbeat = 0;
	timer_nFcmNotify = 0;
	timer_nUserHeart = 0;
	timer_push_userinfo = 0;
	bCanBeDeleted = 0;
	loginCount = 0;
	accessTime = 0;
	nDeleteCheckCount = 0;
	timer_DeleteCheckCount = 0;
	
	level = 0;
	jobLevel = 0;
	characterName = "";
	familyName = "";
	cid = 0;
	money = 0;
	earningRate = 1.f;

	memset(penaltyList, 0, (int)CyouCTURetFlag::FLAG_END_INDEX);
	
	m_cyou_callback_userdata = IMC_NEW CyouCallbackInfo_UserData;
	m_cyou_callback_mopt = IMC_NEW CyouCallbackInfo_mopt;
	m_cyou_callback_fcm = IMC_NEW CyouCallbackInfo_fcm;
	m_cyou_callback_point_consume = IMC_NEW CyouCallbackInfo_point_consume;
	//m_cyou_callback_get_user_point = IMC_NEW CyouCallbackInfo_get_user_point;

	m_cyou_input_get_user_data = IMC_NEW cyou_input_param_user_data;
	m_cyou_input_fcm_heartbeat = IMC_NEW cyou_input_param_fcm_heartbeat;
	m_cyou_input_user_heartbeat = IMC_NEW cyou_input_param_user_heartbeat;
	m_cyou_input_item_buy = IMC_NEW cyou_input_param_item_buy;
	m_cyou_input_point_consume = IMC_NEW cyou_input_param_point_consume;
	m_cyou_input_get_user_point = IMC_NEW cyou_input_param_get_user_point;
}

CyouLoginInfo::CyouLoginInfo(CYOU_SDK_USER_INFO* info)
{
	CyouLoginInfo();
	SetUserInfoFromZone(info);
}


CyouLoginInfo::~CyouLoginInfo()
{
	SAFE_DELETE(m_cyou_callback_userdata);
	SAFE_DELETE(m_cyou_callback_mopt);
	SAFE_DELETE(m_cyou_callback_fcm);
	SAFE_DELETE(m_cyou_callback_point_consume);
	//SAFE_DELETE(m_cyou_callback_get_user_point);

	SAFE_DELETE(m_cyou_input_get_user_data);
	SAFE_DELETE(m_cyou_input_fcm_heartbeat);
	SAFE_DELETE(m_cyou_input_user_heartbeat);
	SAFE_DELETE(m_cyou_input_item_buy);
	SAFE_DELETE(m_cyou_input_point_consume);
	SAFE_DELETE(m_cyou_input_get_user_point);
}

void CyouLoginInfo::RunFcmHeartbeatProcess()
{		
	if (m_cyou_callback_userdata && m_cyou_callback_userdata->bFcmFlag && m_cyou_callback_userdata->isSuccessCalled)
	{
		if (timeGetTime() - timer_nFcmHeartbeat >= (DWORD)(m_cyou_callback_userdata->nFcmHeartBeatInterval * 1000))
		{
			m_cyou_input_fcm_heartbeat->pCYPTSESSION = m_cyou_input_get_user_data->pCYPTSESSION;
			m_cyou_input_fcm_heartbeat->szCnMaster = m_cyou_callback_userdata->szCnMaster;
			m_cyou_input_fcm_heartbeat->szFcmData = m_cyou_callback_userdata->szFcmData;
			m_cyou_input_fcm_heartbeat->szLoginSeq = m_cyou_callback_userdata->szLoginSeq;
			m_cyou_input_fcm_heartbeat->szMcode = m_cyou_input_get_user_data->Mcode;
			m_cyou_input_fcm_heartbeat->szUserIp = m_cyou_input_get_user_data->ip;
			m_cyou_input_fcm_heartbeat->cyou_callback_info = m_cyou_callback_fcm;
						
			CyouServer::gs_fcm_heartbeat(m_cyou_input_fcm_heartbeat);			
			timer_nFcmHeartbeat = timeGetTime();
		}
	}
}

void CyouLoginInfo::ConfirmFcmHeartbeatCallBackProcess(FCM_RET& param, bool& isKick)
{
	if (m_cyou_callback_fcm->bCanCallAsynProcess)
	{
		if (!m_cyou_callback_fcm->isCalledEarning &&
			!m_cyou_callback_fcm->isCalledKick &&
			!m_cyou_callback_fcm->isCalledNotify){			
			asyn_process_cy_pt_session(m_cyou_input_get_user_data->pCYPTSESSION);
		}

		if (m_cyou_callback_fcm->isCalledEarning){
//			LOG_LINE("m_cyou_callback_fcm->isCalledEarning nRetCode = %d, dEarning = %.2f",  m_cyou_callback_fcm->nRetCode, (float)m_cyou_callback_fcm->dEarningRate);
			// 수익 조정 패킷, player class에서 이를 관리하는 변수 하나를 두고, 경험치, 드랍율 등에서 최종적으로 계산해서 획득할 수 있도록 만든다.
			param.nRetCode = m_cyou_callback_fcm->nRetCode;
			param.dEarning = m_cyou_callback_fcm->dEarningRate;
			m_cyou_callback_fcm->bCanCallAsynProcess = false;
		}
		else if (m_cyou_callback_fcm->isCalledKick){
	//		LOG_LINE("m_cyou_callback_fcm->isCalledKick");
			param.nRetCode = m_cyou_callback_fcm->nRetCode;
			m_cyou_callback_fcm->bCanCallAsynProcess = false;
			isKick = m_cyou_callback_fcm->isCalledKick;
			// 킥 패킷
		}
		else if (m_cyou_callback_fcm->isCalledNotify){
		//	LOG_LINE("m_cyou_callback_fcm->isCalledNotify");
			param.nRetCode = m_cyou_callback_fcm->nRetCode;
			m_cyou_callback_fcm->bCanCallAsynProcess = false;
			// 알림 패킷
		}
	}
}

void CyouLoginInfo::RunUserHeartbeatProcess(char* roleGuid, char* roleName, int level, int onlineTime)
{	
	if (m_cyou_callback_userdata->isSuccessCalled)
	{
		if (timeGetTime() - timer_nUserHeart >= (unsigned long)(m_cyou_callback_userdata->nUserHeartBeatInterval * 1000))
		{
			m_cyou_input_user_heartbeat->pCYPTSESSION = m_cyou_input_get_user_data->pCYPTSESSION;
			m_cyou_input_user_heartbeat->szMcode = m_cyou_input_get_user_data->Mcode;
			m_cyou_input_user_heartbeat->nMcodeLen = strlen(m_cyou_input_get_user_data->Mcode);
			m_cyou_input_user_heartbeat->szUserIp = m_cyou_input_get_user_data->ip;
			m_cyou_input_user_heartbeat->nUserIpLen = strlen(m_cyou_input_get_user_data->ip);

			m_cyou_input_user_heartbeat->szCnMaster = m_cyou_callback_userdata->szCnMaster;
			m_cyou_input_user_heartbeat->nCnMasterLen = m_cyou_callback_userdata->nCnMasterLen;
			m_cyou_input_user_heartbeat->szLoginSeq = m_cyou_callback_userdata->szLoginSeq;
			m_cyou_input_user_heartbeat->nLoginSeqLen = m_cyou_callback_userdata->nLoginSeqLen;

			m_cyou_input_user_heartbeat->szRoleGuid = roleGuid;
			m_cyou_input_user_heartbeat->nRoleGuidLen = strlen(roleGuid);
			m_cyou_input_user_heartbeat->szRoleName = roleName;
			m_cyou_input_user_heartbeat->nRoleNameLen = strlen(roleName);

			m_cyou_input_user_heartbeat->nRoleLevel = level;
			m_cyou_input_user_heartbeat->nOnlineTime = onlineTime;				
			
			CyouServer::gs_user_heartbeat(m_cyou_input_user_heartbeat);	
			timer_nUserHeart = timeGetTime();
		}
	}	
}

// 유저가 존으로 갔을 때 호출해야 할 듯 하다.
bool CyouLoginInfo::UserRoleNotify(bool isLogin, char* roleGuid="", char* roleName="", int level=0, int onlineTime=0)
{
	cyou_input_param_role_status_notify info;
	
	info.pCYPTSESSION = m_cyou_input_get_user_data->pCYPTSESSION;
	info.szMcode = m_cyou_input_get_user_data->Mcode;
	info.nMcodeLen = strlen(info.szMcode);
	info.szUserIp = m_cyou_input_get_user_data->ip;
	info.nUserIpLen = strlen(info.szUserIp);
	info.szCnMaster = m_cyou_callback_userdata->szCnMaster;
	info.nCnMasterLen = m_cyou_callback_userdata->nCnMasterLen;
	info.szRoleGuid = roleGuid;
	info.nRoleGuidLen = strlen(info.szRoleGuid);
	info.szRoleName = roleName;
	info.nRoleNameLen = strlen(info.szRoleName);
	info.nRoleLevel = level;
	info.nOnlineTime = onlineTime;
	info.szLoginSeq = m_cyou_callback_userdata->szLoginSeq;
	info.nLoginSeqLen = m_cyou_callback_userdata->nLoginSeqLen;
	info.parameter = nullptr;		

	return CyouServer::gs_role_status_notify(&info, isLogin);
}

void CyouLoginInfo::SetUserInfoFromZone(CYOU_SDK_USER_INFO* info)
{
	accessTime = info->accessTime;
	aid = info->aid;
	barrack_id = info->barrack_id;
	m_cyou_callback_fcm->dEarningRate = info->earningRate;
	earningRate = (float)info->earningRate;
	timer_nFcmNotify = info->timer_nFcmNotify;

	// 값을 새로 세팅하는거니 이미 로그인을 했었다고 봐야한다.
	loginCount = 1;

	strncpy_s(m_cyou_input_get_user_data->ip, sizeof(m_cyou_input_get_user_data->ip), info->ip, strlen(info->ip) + 1);
	strncpy_s(m_cyou_input_get_user_data->Mcode, sizeof(m_cyou_input_get_user_data->Mcode), info->machineCode, strlen(info->machineCode) + 1);
	strncpy_s(m_cyou_input_get_user_data->Token, sizeof(m_cyou_input_get_user_data->Token), info->securityToken, strlen(info->securityToken) + 1);

	m_cyou_callback_userdata->bFcmFlag = info->bFcmFlag;	
	m_cyou_callback_userdata->isSuccessCalled = true;
	strncpy_s(m_cyou_callback_userdata->szCnMaster, sizeof(m_cyou_callback_userdata->szCnMaster), info->CnMaster, strlen(info->CnMaster) + 1);
	m_cyou_callback_userdata->nCnMasterLen = strlen(info->CnMaster);
	strncpy_s(m_cyou_callback_userdata->szFcmData, sizeof(m_cyou_callback_userdata->szFcmData), info->FcmData, strlen(info->FcmData) + 1);
	m_cyou_callback_userdata->nFcmDataLen = strlen(info->FcmData);
	strncpy_s(m_cyou_callback_userdata->szLoginSeq, sizeof(m_cyou_callback_userdata->szLoginSeq), info->LoginSeq, strlen(info->LoginSeq) + 1);
	m_cyou_callback_userdata->nLoginSeqLen = strlen(info->LoginSeq);
	m_cyou_callback_userdata->nFcmHeartBeatInterval = info->nFcmHeartBeatInterval;
	m_cyou_callback_userdata->nSecurityFlags = info->nSecurityFlags;
	m_cyou_callback_userdata->nUserHeartBeatInterval = info->nUserHeartBeatInterval;
	memcpy(penaltyList, info->penaltyList, sizeof(penaltyList));

	info->print();
}

CYOU_SDK_USER_INFO CyouLoginInfo::GetUserInfo()
{
	CYOU_SDK_USER_INFO info;
	info.accessTime = accessTime;
	info.aid = aid;
	info.barrack_id = barrack_id;
	//info.earningRate = m_cyou_callback_fcm->dEarningRate;
	info.earningRate = earningRate;
	info.timer_nFcmNotify = timer_nFcmNotify;

	strncpy_s(info.ip, sizeof(info.ip), m_cyou_input_get_user_data->ip, strlen(m_cyou_input_get_user_data->ip) + 1);
	strncpy_s(info.machineCode, sizeof(info.machineCode), m_cyou_input_get_user_data->Mcode, strlen(m_cyou_input_get_user_data->Mcode) + 1);
	strncpy_s(info.securityToken, sizeof(info.securityToken), m_cyou_input_get_user_data->Token, strlen(m_cyou_input_get_user_data->Token) + 1);

	info.bFcmFlag = m_cyou_callback_userdata->bFcmFlag;
	strncpy_s(info.CnMaster, sizeof(info.CnMaster), m_cyou_callback_userdata->szCnMaster, strlen(m_cyou_callback_userdata->szCnMaster) + 1);
	strncpy_s(info.FcmData, sizeof(info.FcmData), m_cyou_callback_userdata->szFcmData, strlen(m_cyou_callback_userdata->szFcmData) + 1);
	strncpy_s(info.LoginSeq, sizeof(info.LoginSeq), m_cyou_callback_userdata->szLoginSeq, strlen(m_cyou_callback_userdata->szLoginSeq) + 1);
	info.nFcmHeartBeatInterval = m_cyou_callback_userdata->nFcmHeartBeatInterval;
	info.nSecurityFlags = m_cyou_callback_userdata->nSecurityFlags;
	info.nUserHeartBeatInterval = m_cyou_callback_userdata->nUserHeartBeatInterval;
	memcpy(info.penaltyList, penaltyList, sizeof(penaltyList));

	return info;
}

void CyouLoginInfo::Test()
{
	std::list<CTU::PlayerParam> lstPlayerParam; //파라미터 리스트
	CTU::PlayerParam paramUsername("CharName", "hihihi18");
	lstPlayerParam.push_back(paramUsername);
	CTU::PlayerParam paramIP("IpAddr", "10.01.10.20");  //회원IP
	lstPlayerParam.push_back(paramIP);
	CTU::PlayerParam paramLevel("Level", 33); //회원 레밸
	lstPlayerParam.push_back(paramLevel);
	CTU::PlayerParam paramMoney("Money", 2134656);
	lstPlayerParam.push_back(paramMoney);

	CTU::GetCTUServerEntity()->OnPlayerLogin("12234", "cy0002", 6, lstPlayerParam);
	CTU::GetCTUServerEntity()->OnTick();
}

void CyouLoginInfo::SetSessionPtr(cy_pt::CYPTSESSION* pCYPTSESSION)
{
	m_cyou_input_get_user_data->pCYPTSESSION = pCYPTSESSION;
	m_cyou_input_user_heartbeat->pCYPTSESSION = pCYPTSESSION;
	m_cyou_input_fcm_heartbeat->pCYPTSESSION = pCYPTSESSION;
	m_cyou_input_item_buy->pCYPTSESSION = pCYPTSESSION;
	m_cyou_input_point_consume->pCYPTSESSION = pCYPTSESSION;
	m_cyou_input_get_user_point->pCYPTSESSION = pCYPTSESSION;
}

void CyouLoginInfo::RecordItemBuy(char* szRoleGuid, char* szRoleName, int nRoleLevel, char* szOperation, char* szBuyItems)
{
	if (m_cyou_input_get_user_data){
		m_cyou_input_item_buy->pCYPTSESSION = m_cyou_input_get_user_data->pCYPTSESSION;
		m_cyou_input_item_buy->szMcode = m_cyou_input_get_user_data->Mcode;
		m_cyou_input_item_buy->nMcodeLen = strlen(m_cyou_input_item_buy->szMcode);
		m_cyou_input_item_buy->szUserIp = m_cyou_input_get_user_data->ip;
		m_cyou_input_item_buy->nUserIpLen = strlen(m_cyou_input_item_buy->szUserIp);
		m_cyou_input_item_buy->szCnMaster = m_cyou_callback_userdata->szCnMaster;
		m_cyou_input_item_buy->nCnMasterLen = strlen(m_cyou_input_item_buy->szCnMaster);
		m_cyou_input_item_buy->szRoleGuid = szRoleGuid;
		m_cyou_input_item_buy->nRoleGuidLen = strlen(szRoleGuid);
		m_cyou_input_item_buy->szRoleName = szRoleName;
		m_cyou_input_item_buy->nRoleNameLen = strlen(szRoleName);
		m_cyou_input_item_buy->nRoleLevel = nRoleLevel;
		m_cyou_input_item_buy->szOperation = szOperation;
		m_cyou_input_item_buy->nOperationLen = strlen(szOperation);
		m_cyou_input_item_buy->szBuyItems = szBuyItems;
		m_cyou_input_item_buy->nBuyItemsLen = strlen(szBuyItems);
		m_cyou_input_item_buy->szLoginSeq = m_cyou_callback_userdata->szLoginSeq;
		m_cyou_input_item_buy->nLoginSeqLen = strlen(m_cyou_input_item_buy->szLoginSeq);

		int TryCount = 3;
		for (int i = 0; i < TryCount; i++){
			bool ret = CyouServer::gs_item_buy_record(m_cyou_input_item_buy);
			if (ret) break;
		}
	}
}

bool CyouLoginInfo::RequestPointConsume(char* szRoleGuid, char* szRoleName, int nRoleLevel, char* szBuySeq, char* szCostPoint, char* szBuyItems)
{
	if (m_cyou_input_get_user_data){
		m_cyou_input_point_consume->pCYPTSESSION = m_cyou_input_get_user_data->pCYPTSESSION;
		m_cyou_input_point_consume->szUserIp = m_cyou_input_get_user_data->ip;
		m_cyou_input_point_consume->nUserIpLen = strlen(m_cyou_input_point_consume->szUserIp);
		m_cyou_input_point_consume->szMcode = m_cyou_input_get_user_data->Mcode;
		m_cyou_input_point_consume->nMcodeLen = strlen(m_cyou_input_point_consume->szMcode);
		m_cyou_input_point_consume->szCnMaster = m_cyou_callback_userdata->szCnMaster;
		m_cyou_input_point_consume->nCnMasterLen = strlen(m_cyou_input_point_consume->szCnMaster);
		m_cyou_input_point_consume->szRoleGuid = szRoleGuid;
		m_cyou_input_point_consume->nRoleGuidLen = strlen(m_cyou_input_point_consume->szRoleGuid);
		m_cyou_input_point_consume->szRoleName = szRoleName;
		m_cyou_input_point_consume->nRoleNameLen = strlen(m_cyou_input_point_consume->szRoleName);
		m_cyou_input_point_consume->nRoleLevel = nRoleLevel;
		m_cyou_input_point_consume->szBuySeq = szBuySeq;
		m_cyou_input_point_consume->nBuySeqLen = strlen(m_cyou_input_point_consume->szBuySeq);
		m_cyou_input_point_consume->szCostPoint = szCostPoint;
		m_cyou_input_point_consume->nCostPointLen = strlen(m_cyou_input_point_consume->szCostPoint);
		m_cyou_input_point_consume->szBuyItems = szBuyItems;
		m_cyou_input_point_consume->nBuyItemsLen = strlen(m_cyou_input_point_consume->szBuyItems);
		m_cyou_input_point_consume->szLoginSeq = m_cyou_callback_userdata->szLoginSeq;
		m_cyou_input_point_consume->nLoginSeqLen = strlen(m_cyou_input_point_consume->szLoginSeq);
		m_cyou_input_point_consume->cyou_callback_info = m_cyou_callback_point_consume;
		bool ret = CyouServer::gs_user_point_consume(m_cyou_input_point_consume);
		return ret;
	}
	return false;
}

bool CyouLoginInfo::RequestLookupUserPoint()
{
	if (m_cyou_input_get_user_data){
		m_cyou_input_get_user_point->pCYPTSESSION;
		m_cyou_input_get_user_point->szUserIp = m_cyou_input_get_user_data->ip;
		m_cyou_input_get_user_point->nUserIpLen = strlen(m_cyou_input_get_user_point->szUserIp);
		m_cyou_input_get_user_point->szMcode = m_cyou_input_get_user_data->Mcode;
		m_cyou_input_get_user_point->nMcodeLen = strlen(m_cyou_input_get_user_point->szMcode);
		m_cyou_input_get_user_point->szCnMaster = m_cyou_callback_userdata->szCnMaster;
		m_cyou_input_get_user_point->nCnMasterLen = strlen(m_cyou_input_get_user_point->szCnMaster);
		m_cyou_input_get_user_point->szLoginSeq = m_cyou_callback_userdata->szLoginSeq;
		m_cyou_input_get_user_point->nLoginSeqLen = strlen(m_cyou_input_get_user_point->szLoginSeq);
		m_cyou_input_get_user_point->cyou_callback_info = m_cyou_callback_point_consume;
		bool ret = CyouServer::gs_get_user_point(m_cyou_input_get_user_point);
		return ret;
	}
	return false;
}

float CyouLoginInfo::GetUserPoint()
{
	if (m_cyou_callback_point_consume->szLeftPoint)
	{
		float rtnValue = 0.f;
		try
		{
			rtnValue = boost::lexical_cast<float>(m_cyou_callback_point_consume->szLeftPoint);
		}
		catch (boost::bad_lexical_cast const& e)
		{
//			IMC_ERROR("ERRCODE_LOGIC_BAD_LEXICAL_CAST", "CyouPoint lexical Cast Error : AID[%I64u] / LeftPoint[%s] / error[%s]", aid, m_cyou_callback_point_consume->szLeftPoint, e.what());			
			e.what();
			rtnValue = 0.f;
		}
		return rtnValue;
	}
	else
	{
		return 0.f;
	}
}

bool CyouLoginInfo::OnPlayerLogin()
{
	if (this->loginCount == 0)
	{
		this->loginCount = 1;

		std::list<CTU::PlayerParam> lstPlayerParam; //파라미터 리스트
		CTU::PlayerParam paramUsername("CharName", this->characterName.c_str());
		lstPlayerParam.push_back(paramUsername);
		CTU::PlayerParam paramIP("IpAddr", m_cyou_input_get_user_data->ip);  //회원IP
		lstPlayerParam.push_back(paramIP);
		CTU::PlayerParam paramLevel("Level", this->level); //회원 레밸
		lstPlayerParam.push_back(paramLevel);
		CTU::PlayerParam paramMoney("Money", (int)this->money);
		lstPlayerParam.push_back(paramMoney);
		std::string cid = "0";

		//LOG_LINE("OnPlayerLogin info : %s, %s, %d, %d", this->characterName.c_str(), m_cyou_input_get_user_data->ip, this->level, (int)this->money);
		try
		{
			cid = boost::lexical_cast<std::string>(this->cid);
		}
		catch (boost::bad_lexical_cast const& e)
		{
			//IMC_ERROR("ERRCODE_LOGIC_BAD_LEXICAL_CAST", "bad_lexical_cast exepction : CID : [%I64u]  / error : [%s]", this->cid, e.what());
			e.what();
			cid = "0";
		}
		
		int tryCount = 3;
		this->accessTime = timeGetTime();
		for (int i = 0; i < tryCount; i++)
		{
			bool ret = UserRoleNotify(true, (char*)cid.c_str(), (char*)this->characterName.c_str(), this->level, (int)((timeGetTime() - accessTime) / 1000));
			if (ret) break;
		}
		
		if (CyouServer::GetUseCTUServer())
		{
			CTU::GetCTUServerEntity()->OnPlayerLogin(cid.c_str(), m_cyou_callback_userdata->szCnMaster, m_cyou_callback_userdata->nCnMasterLen, lstPlayerParam);
			CTU::GetCTUServerEntity()->OnTick();
//			LOG_LINE("OnPlyaerLogin() Ret = %d", ret);						
		}
		this->timer_nUserHeart = 0;
		
		return true;
	}
	return false;
}

void CyouLoginInfo::OnPlayerInfoChanged()
{
	// PACKET_ZG_CYOU_CHANGE_USER_INFO_NOTIFY 패킷이 오면 렙업했다는 소리다.
	std::list<CTU::PlayerParam> lstPlayerParam; //파라미터 리스트
	CTU::PlayerParam paramUsername("CharName", this->characterName.c_str());
	lstPlayerParam.push_back(paramUsername);
	CTU::PlayerParam paramIP("IpAddr", m_cyou_input_get_user_data->ip);  //회원IP
	lstPlayerParam.push_back(paramIP);
	CTU::PlayerParam paramLevel("Level", this->level); //회원 레밸
	lstPlayerParam.push_back(paramLevel);
	CTU::PlayerParam paramMoney("Money", (int)this->money);
	lstPlayerParam.push_back(paramMoney);
	std::string cid = "0";

	try
	{
		cid = boost::lexical_cast<std::string>(this->cid);
	}
	catch (boost::bad_lexical_cast const& e)
	{
		//IMC_ERROR("ERRCODE_LOGIC_BAD_LEXICAL_CAST", "bad_lexical_cast exepction : AID : [%I64u]  / error : [%s]", this->aid, e.what());
		e.what();
		cid = "0";
	}
	
	if (CyouServer::GetUseCTUServer())
	{
		CTU::GetCTUServerEntity()->OnPlayerInfoChanged(cid.c_str(), lstPlayerParam);
		CTU::GetCTUServerEntity()->OnTick();
		//LOG_LINE("CTU::GetCTUServerEntity()->OnPlayerInfoChanged(%s)", cid.c_str());
	}
}
