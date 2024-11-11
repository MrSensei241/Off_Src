#include "StdAfx.h"
#include "CyouCallbackInfo.h"

// CyouCallbackInfo
CyouCallbackInfo::CyouCallbackInfo()
{	
	nRetCode = -1;
	memset(szCnMaster, 0, sizeof(szCnMaster));
	nCnMasterLen = 0;
	memset(szLoginSeq, 0, sizeof(szLoginSeq));
	nLoginSeqLen = 0;
	memset(szMessage, 0, sizeof(szMessage));	
	nMessageLen = 0;
	aid = 0;
	bCanCallAsynProcess = false;
}

CyouCallbackInfo::~CyouCallbackInfo(){}

void CyouCallbackInfo::clear()
{		
	nRetCode = -1;
	memset(szCnMaster, 0, sizeof(szCnMaster));
	nCnMasterLen = 0;
	memset(szLoginSeq, 0, sizeof(szLoginSeq));
	nLoginSeqLen = 0;
	memset(szMessage, 0, sizeof(szMessage));
	nMessageLen = 0;
}

void CyouCallbackInfo::readyToCall()
{
	bCanCallAsynProcess = false;
}

// CyouCallbackInfo_UserData
CyouCallbackInfo_UserData::CyouCallbackInfo_UserData() : CyouCallbackInfo()
{
	isSuccessCalled = false;
	isFailCalled = false;
	nSecurityFlags = 0;
	bFcmFlag = false;
	memset(szFcmData, 0, sizeof(szFcmData));	
	nFcmDataLen = 0;
	nUserHeartBeatInterval = 0;
	nFcmHeartBeatInterval = 0;
	memset(szSecurityToken, 0, sizeof(szSecurityToken));	
	nSecurityTokenLen = 0;		
	timer_copy_to_session_interval = 60 * 1000;
}

CyouCallbackInfo_UserData::~CyouCallbackInfo_UserData(){}

void CyouCallbackInfo_UserData::clear()
{
	CyouCallbackInfo::clear();
	isSuccessCalled = false;
	isFailCalled = false;
	nSecurityFlags = 0;
	bFcmFlag = false;;
	memset(szFcmData, 0, sizeof(szFcmData));
	nFcmDataLen = 0;
	nUserHeartBeatInterval = 0;
	nFcmHeartBeatInterval = 0;

	memset(szSecurityToken, 0, sizeof(szSecurityToken));
	nSecurityTokenLen = 0;

	timer_copy_to_session_interval = 0;
}

void CyouCallbackInfo_UserData::readyToCall()
{
	CyouCallbackInfo::readyToCall();
	isFailCalled = false;
	isSuccessCalled = false;
}

// CyouCallbackInfo_mopt
CyouCallbackInfo_mopt::CyouCallbackInfo_mopt() : CyouCallbackInfo()
{

	szMotpMsg = nullptr;
	nMotpMsgLen = 0;
}

CyouCallbackInfo_mopt::~CyouCallbackInfo_mopt(){}

void CyouCallbackInfo_mopt::clear()
{
	CyouCallbackInfo::clear();
	szMotpMsg = nullptr;
	nMotpMsgLen = 0;
}

// CyouCallbackInfo_notify
CyouCallbackInfo_fcm::CyouCallbackInfo_fcm() : CyouCallbackInfo()
{
	isCalledEarning = false;
	isCalledKick = false;
	isCalledNotify = false;
	dEarningRate = 1;
}

CyouCallbackInfo_fcm::~CyouCallbackInfo_fcm()
{

}

void CyouCallbackInfo_fcm::clear()
{
	CyouCallbackInfo::clear();
	isCalledEarning = false;
	isCalledKick = false;
	isCalledNotify = false;
	dEarningRate = 0;	
}

void CyouCallbackInfo_fcm::readyToCall()
{
	CyouCallbackInfo::readyToCall();
	isCalledEarning = false;
	isCalledKick = false;
	isCalledNotify = false;
}


// CyouCallbackInfo_point_consume
CyouCallbackInfo_point_consume::CyouCallbackInfo_point_consume() : CyouCallbackInfo()
{	
	CyouCallbackInfo::readyToCall();
	isSuccessCalled = false;
	isFailCalled = false;
	szBuySeq = nullptr;
	nBuySeqLen = 0;
	szLeftPoint = nullptr;
	nLeftPointLen = 0;
}

CyouCallbackInfo_point_consume::~CyouCallbackInfo_point_consume()
{

}

void CyouCallbackInfo_point_consume::clear()
{
	CyouCallbackInfo::clear();
	isSuccessCalled = false;
	isFailCalled = false;
	szBuySeq = nullptr;
	nBuySeqLen = 0;
	szLeftPoint = nullptr;
	nLeftPointLen = 0;
}

void CyouCallbackInfo_point_consume::readyToCall()
{
	CyouCallbackInfo::readyToCall();
	isFailCalled = false;
	isSuccessCalled = false;
}

CyouCallbackInfo_get_user_point::CyouCallbackInfo_get_user_point() : CyouCallbackInfo()
{
	isSuccessCalled = false;
	isFailCalled = false;	
	szleftPoint = nullptr;
	nLeftPointLen = 0;
}

CyouCallbackInfo_get_user_point::~CyouCallbackInfo_get_user_point()
{

}

void CyouCallbackInfo_get_user_point::clear()
{
	CyouCallbackInfo::clear();
	isSuccessCalled = false;
	isFailCalled = false;
	szleftPoint = nullptr;
	nLeftPointLen = 0;
}

void CyouCallbackInfo_get_user_point::readyToCall()
{
	CyouCallbackInfo::readyToCall();
	isFailCalled = false;
	isSuccessCalled = false;
}
