#include "Stdafx.h"
#include "geClientNexonSSO.h"

namespace geClientNexonSSO
{
	std::string s_passPort;
	std::string s_logSID;
	std::string s_gamecode;
	

	void SetPassport(const char * passPort)
	{
		s_passPort = passPort;
	}

	const char * GetPassport()
	{
		return s_passPort.c_str();
	}

	void SetLogSID(const char * sid)
	{
		s_logSID = sid;
	}

	const char* GetLogSID()
	{
		return s_logSID.c_str();
	}

	void SetGameCode(const char * gamecode)
	{
		s_gamecode = gamecode;
	}

	const char* GetGameCode()
	{
		return s_gamecode.c_str();
	}

};