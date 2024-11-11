#include "stdafx.h"
#include "CyouNotify.h"


CyouNotify::CyouNotify()
{	
	version = L"1";
	gameType = L"48";
	start_game_success_flag = false;
	server_select_index = -1;
	is_first_enter_flag = false;
}

CyouNotify::~CyouNotify()
{
}

/*
CYClientStatusNotifyType paramType

INSTALL_CLIENT_START		= 1, //gameclient-installer start
INSTALL_CLIENT_FINISH		= 2, //gameclient-installer finish
LAUNCH_START				= 3, //launcher start
RUN_LAUNCH_SUCCESS			= 4, //launcher start success
SELECT_ZONE_SUCCESS			= 5, //launcher select zone
LAUNCH_START_UPDATE_CLIENT  = 6, //laucnher start update client
CLIENT_UPDATE_SCUCCESS		= 7, //laucnher update client success
LAUNCH_START_GAME			= 8, //launcher start game
START_GAME_SUCCESS			= 9, //gameclient start success （must）
CLICK_LOGIN					=10, //launcher click login
SELECT_WORLD         	    =11, //select world
SELECT_WORLD_SUCCESS		=12, //select world success	（must）
CREATE_ROLE_SUCCESS			=13, //gamclient role success （must）
LOGIN_SCENE_SUCCESS			=14, //role-online
CLIENT_UNSTALL_FINISH		=15, //game-unstaller finish
MANUAL_UPDATE_FINISH		=16, //launcher update client success
CLIENT_FIRST_CRASH			=17, //game-client first crash
CLIENT_CRASH				=18, //game-client not first crash （must）
GAME_GUARD_UNUSUAL			=19, //
FIRST_START_LAUNCH			=20, //launcher start
FIRST_START_LAUNCH_SUCCESS	=21, //launcher start success
FIRST_LAUNCH_CRASH			=22, //first launcher crash
LAUNCH_CRASH				=23, //launcher crash
MANUAL_UPDATE_START			=24, //launcher update start
MANUAL_UPDATE_ERROR			=25  //launcher update error

*/
void CyouNotify::send_game_client_status_notify_async(CYClientStatusNotifyType paramType)
{	
	if (this->check_condition(paramType) == false) return;

	asyn_client_status_notifyW(getGameType(), getVersion(), paramType);
}

void CyouNotify::Log(const char* msg, char* filename, int line)
{
	char temp[1024];
	sprintf_s(temp, "%s (%s:%d)", msg, filename, line);
	std::ofstream logFile("log_launcher.txt", std::ios::app);
	logFile << temp << std::endl;
	logFile.close();
}

std::string CyouNotify::replace(std::string &message, std::string& pattern, std::string& replace)
{
	std::string result = message;
	std::string::size_type pos = 0;
	std::string::size_type offset = 0;
	pos = result.find(pattern, offset);
	if (pos != std::string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
	}
	return result;
}

// wchar_t* CyouNotify::cStringToWChar(CString s)
// {
// 	char* str = s.GetBuffer();
// 	wchar_t* pStr;
// 	//멀티 바이트 크기 계산 길이 반환
// 	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
// 	//wchar_t 메모리 할당
// 	pStr = new WCHAR[strSize];
// 	//형 변환
// 	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
// 	return pStr;
// }

wchar_t* CyouNotify::getGameType(){
	return gameType;
}

wchar_t* CyouNotify::getVersion()
{
	return version;
}

void CyouNotify::test_notify(CYClientStatusNotifyType paramType)
{	
	if (this->check_condition(paramType) == false) return;

	if (paramType == (CYClientStatusNotifyType)START_GAME_SUCCESS){
		//LOG_LINE("START_GAME_SUCCESS");
	}
	else if (paramType == (CYClientStatusNotifyType)SELECT_WORLD){
		//LOG_LINE("SELECT_WORLD");
	}
	else if (paramType == (CYClientStatusNotifyType)SELECT_WORLD_SUCCESS){
		//LOG_LINE("SELECT_WORLD_SUCCESS");
	}
	else if (paramType == (CYClientStatusNotifyType)LOGIN_SCENE_SUCCESS){
		//LOG_LINE("LOGIN_SCENE_SUCCESS");
	}
	else if (paramType == (CYClientStatusNotifyType)CREATE_ROLE_SUCCESS){
		//LOG_LINE("CREATE_ROLE_SUCCESS");
	}
	//LOG_LINE("test_notify : %d", (int)paramType);
}

bool CyouNotify::is_diff_server_select_index(int index)
{
	if (this->server_select_index != index){
		this->server_select_index = index;
		return true;
	}
	else{
		return false;
	}
}

bool CyouNotify::is_first_enter_zone()
{
	if (is_first_enter_flag == false){
		return true;
	}
	else{
		return false;
	}
}

bool CyouNotify::check_condition(CYClientStatusNotifyType paramType)
{
	if (paramType == (CYClientStatusNotifyType)START_GAME_SUCCESS){
		if (start_game_success_flag){
			return false;
		}
		else{
			start_game_success_flag = true;
		}
	}
	else if (paramType == (CYClientStatusNotifyType)LOGIN_SCENE_SUCCESS){
		if (this->is_first_enter_zone() == false) return false;
		else{
			this->set_first_enter_flag(true);
		}
	}	
	return true;
}

void CyouNotify::set_first_enter_flag(bool b)
{
	this->is_first_enter_flag = b;
}
