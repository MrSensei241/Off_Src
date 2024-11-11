#pragma once
#include "CyouServer.h"
#include "CTUServerEntity.h"
#include "CyouCTUTypeDef.h"
#include "TransferTypeDef.h"

class CyouLoginInfo
{
public:
	CTU::CTUServerRetFlag serverRetFlag;
	bool penaltyList[(int)(CyouCTURetFlag)FLAG_END_INDEX];
	float earningRate;

	WORD barrack_id;		// 어느 배럭서버로부터 왔는지 저장한다.
	UINT64 aid;
	DWORD timer_nFcmHeartbeat;	// FcmHeartbeat request interval
	DWORD timer_nFcmNotify;		// interval of notifying FcmHeartbeat to user
	DWORD timer_nUserHeart;		// UserHeartbeat request interval;
	DWORD timer_push_userinfo;
	/** 로그인된 유저는 지우면 안되기 때문에 */
	bool bCanBeDeleted;
	
	/** 로그인을 한번만 처리하기 위한 변수 */
	int loginCount;

	unsigned long accessTime;	// 유저의 로그인 시간을 기록한다.
	/** 배럭에서 존으로 갈때도 로그아웃 패킷이 온다. 그래서 진정한 로그아웃인지 모르기 때문에 user_session이 존재하는지 체크하고 지우기 위해 횟수 체크 */
	int nDeleteCheckCount;
	DWORD timer_DeleteCheckCount;	// 확인을 하는 주기(50초 동안 세션이 존재하지 않으면 삭제함)

	// Logout 시에 필요한 정보라서 가지고 있음
	int level;
	int jobLevel;
	std::string characterName;
	std::string familyName;
	UINT64 cid;
	DWORD money;

	CyouCallbackInfo_UserData* m_cyou_callback_userdata;	// asyn_gs_get_user_data 에 전달
	CyouCallbackInfo_mopt* m_cyou_callback_mopt;			// asyn_gs_verify_motp 에 전달
	CyouCallbackInfo_fcm* m_cyou_callback_fcm;				// asyn_gs_fcm_heartbeat 에 전달
	CyouCallbackInfo_point_consume* m_cyou_callback_point_consume;	// asyn_gs_user_point_consume, asyn_gs_get_user_point 에 전달
	//CyouCallbackInfo_get_user_point* m_cyou_callback_get_user_point;

	cyou_input_param_user_data* m_cyou_input_get_user_data;
	cyou_input_param_fcm_heartbeat* m_cyou_input_fcm_heartbeat;
	cyou_input_param_user_heartbeat* m_cyou_input_user_heartbeat;
	cyou_input_param_item_buy* m_cyou_input_item_buy;
	cyou_input_param_point_consume* m_cyou_input_point_consume;
	cyou_input_param_get_user_point* m_cyou_input_get_user_point;

	CyouLoginInfo();
	CyouLoginInfo(CYOU_SDK_USER_INFO* info);
	~CyouLoginInfo();

	/** 유저 과몰입 하트비트 실행 후 콜백 기다림(비동기라서 실행만함) */
	void RunFcmHeartbeatProcess();
	/** 과몰입 콜백을 지속적으로 확인하면서 응답오면 처리한다. */
	void ConfirmFcmHeartbeatCallBackProcess(FCM_RET& param, bool& isKick);
	/** 유저 정보를 전송한다. */
	void RunUserHeartbeatProcess(char* roleGuid, char* roleName, int level, int onlineTime);
	/** 유저의 로그인/아웃 시에 정보를 전송한다. */
	bool UserRoleNotify(bool isLogin, char* roleGuid, char* roleName, int level, int onlineTime);
	/** 글로벌 서버가 다운 후 기동시 존으로부터 받은 cyou login info 를 세팅한다. */
	void SetUserInfoFromZone(CYOU_SDK_USER_INFO* info);
	/** sdk 호출을 위한 sesseion pointer를 세팅한다. */
	void SetSessionPtr(cy_pt::CYPTSESSION* pCYPTSESSION);

	/** 회원의 소비가 성공한 경우, 이를 호출해 알린다  */
	void RecordItemBuy(char* szRoleGuid, char* szRoleName, int nRoleLevel, char* szOperation, char* szBuyItems);
	/** 회원의 포인트 소모를 진행할 경우 호출함 */
	bool RequestPointConsume(char* szRoleGuid, char* szRoleName, int nRoleLevel, char* szBuySeq, char* szCostPoint, char* szBuyItems);
	/** 창유 서버에 회원의 포인트를 조회한다. */
	bool RequestLookupUserPoint();
	/** 회원의 포인트를 가져온다. */
	float GetUserPoint();

	// CTU SERVER 관련 //
	bool OnPlayerLogin();
	// 창유 포인트 변경 알림도 처리해야할 듯 하다.
	void OnPlayerInfoChanged();

	/** 글로벌 다운 시 복원을 위한 정보들을 가져온다.(한번이라도 글로벌에서 받은 정보가 아니라면 nullptr 을 리턴, 글로벌에서 받은 정보이어야만 유의미하다. */
	CYOU_SDK_USER_INFO GetUserInfo();

	static void Test();
};