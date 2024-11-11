#pragma once

#include "CYClientSDK.h"
#include "CYTypeDef.h"
#include "CYstdint.h"
#include <process.h>
#include <fstream>
#include <iostream>
#include <atlstr.h>  

using namespace cy_pt;

class CyouNotify
{
private:
	//static wchar_t* cStringToWChar(CString str);
	wchar_t* version;
	wchar_t* gameType;
	bool start_game_success_flag;
	int server_select_index;	
	bool check_condition(CYClientStatusNotifyType paramType);
	bool is_first_enter_flag;

public:
	CyouNotify();
	~CyouNotify();

	void send_game_client_status_notify_async(CYClientStatusNotifyType paramType);
	static void Log(const char* msg, char* filename, int line);
	static std::string replace(std::string &message, std::string& pattern, std::string& replace);
	wchar_t* getVersion();
	wchar_t* getGameType();
	void test_notify(CYClientStatusNotifyType paramType);
	bool is_diff_server_select_index(int index);	
	bool is_first_enter_zone();	
	void set_first_enter_flag(bool b);
};

static CyouNotify cyouNotify;