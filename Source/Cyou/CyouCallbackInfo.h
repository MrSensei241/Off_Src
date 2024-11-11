#pragma once

class CyouCallbackInfo
{
public:	
	int32_t nRetCode;

	char szCnMaster[128];
	int32_t nCnMasterLen;

	char szLoginSeq[128];
	int32_t nLoginSeqLen;

	char szMessage[256];
	int32_t nMessageLen;

	UINT64 aid;

	// setting true when calling method to callback, setting false after callback, restrict call asyn_process until callback	
	bool bCanCallAsynProcess;

public:
	CyouCallbackInfo();
	~CyouCallbackInfo();
	virtual void clear();
	virtual void readyToCall();
};

class CyouCallbackInfo_UserData : public CyouCallbackInfo{
public:
	unsigned long timer_copy_to_session_interval;
	bool isSuccessCalled;
	bool isFailCalled;

	int32_t nSecurityFlags;
	bool  bFcmFlag;

	char szFcmData[128];
	int32_t nFcmDataLen; 	

	int32_t nUserHeartBeatInterval;
	int32_t nFcmHeartBeatInterval;

	char szSecurityToken[128];
	int32_t nSecurityTokenLen;	
	
	CyouCallbackInfo_UserData();
	~CyouCallbackInfo_UserData();
	void clear() override;
	void readyToCall() override;
};

class CyouCallbackInfo_mopt : public CyouCallbackInfo{
public:
	char *szMotpMsg;
	int32_t nMotpMsgLen;

	CyouCallbackInfo_mopt();
	~CyouCallbackInfo_mopt();
	void clear() override;
};

class CyouCallbackInfo_fcm : public CyouCallbackInfo{
public:
	bool isCalledEarning;
	bool isCalledKick;
	bool isCalledNotify;
	double dEarningRate;
	CyouCallbackInfo_fcm();
	~CyouCallbackInfo_fcm();
	void clear() override;
	void readyToCall() override;
};

class CyouCallbackInfo_point_consume : public CyouCallbackInfo{
public:
	bool isSuccessCalled;
	bool isFailCalled;
	char *szBuySeq;
	int32_t nBuySeqLen;
	char *szLeftPoint;
	int32_t nLeftPointLen;

	CyouCallbackInfo_point_consume();
	~CyouCallbackInfo_point_consume();
	void clear() override;
	void readyToCall() override;
};

class CyouCallbackInfo_get_user_point : public CyouCallbackInfo
{
public:
	bool isSuccessCalled;
	bool isFailCalled;
	char* szleftPoint;
	int32_t nLeftPointLen;

	CyouCallbackInfo_get_user_point();
	~CyouCallbackInfo_get_user_point();
	void clear() override;
	void readyToCall() override;
};