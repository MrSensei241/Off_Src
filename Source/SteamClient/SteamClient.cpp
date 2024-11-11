#include "stdafx.h"
#include "SteamClient.h"

//std::vector<std::string> s_warningMsgVec;

HAuthTicket g_hAuthTicket = k_HAuthTicketInvalid;

void(*s_steamAPIDebugTextHookFunc)(int, const char *) = nullptr;

DLLFunction int _cdecl Steam_Init_DLL(const char * binPath, std::string& errorCode, void(*steamAPIDebugTextHookFunc)(int, const char *))
{
	s_steamAPIDebugTextHookFunc = steamAPIDebugTextHookFunc;
	if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
	{
		// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
		// local Steam client and also launches this game again.

		// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
		// removed steam_appid.txt from the game depot.

		return 0;
	}
	
	// Init Steam CEG
	if (!Steamworks_InitCEGLibrary())
	{
		OutputDebugString("Steamworks_InitCEGLibrary() failed\n");
		errorCode = "Steam must be running to play this game. CEGLibaryInitFailed";
		return 0;
	}

	// Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
	// You don't necessarily have to though if you write your code to check whether all the Steam
	// interfaces are NULL before using them and provide alternate paths when they are unavailable.
	//
	// This will also load the in-game steam overlay dll into your process.  That dll is normally
	// injected by steam when it launches games, but by calling this you cause it to always load,
	// even when not launched via steam.
	if (!SteamAPI_Init())
	{
		OutputDebugString("SteamAPI_Init() failed\n");
		errorCode = "Steam must be running to play this game. SteamAPInitfailed";
		return 0;
	}

	// set our debug handler
	SteamClient()->SetWarningMessageHook(s_steamAPIDebugTextHookFunc);

	// Ensure that the user has logged into Steam. This will always return true if the game is launched
	// from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
	// will return false.
	if (!SteamUser()->BLoggedOn())
	{
		OutputDebugString("Steam user is not logged in\n");
		errorCode = "Steam user must be logged in to play this game";
		return 0;
	}

	// We are going to use the controller interface, initialize it, which is a seperate step as it 
	// create a IMC_NEW thread in the game proc and we don't want to force that on games that don't
	// have native Steam controller implementations

	const char * rgchCWD = binPath;

	char rgchFullPath[1024];
#if defined(_WIN32)
	_snprintf(rgchFullPath, sizeof(rgchFullPath), "%s\\%s", rgchCWD, "controller.vdf");
#elif defined(OSX)
	// hack for now, because we do not have utility functions available for finding the resource path
	// alternatively we could disable the SteamController init on OS X
	_snprintf(rgchFullPath, sizeof(rgchFullPath), "%s/steamworksexample.app/Contents/Resources/%s", rgchCWD, "controller.vdf");
#else
	_snprintf(rgchFullPath, sizeof(rgchFullPath), "%s/%s", rgchCWD, "controller.vdf");
#endif
	if (!SteamController()->Init(rgchFullPath))
	{
		OutputDebugString("SteamController()->Init() failed\n");
		errorCode = "Steam Controller Init failed. Is controller.vdf in the current working directory?\n";
		return 0;
	}

	Steamworks_SelfCheck();
	return 1;
}

DLLFunction UINT64 _cdecl GetSteamID_DLL()
{
	ISteamUser* steamUser = SteamUser();
	if (!steamUser)
		return 0;

	return steamUser->GetSteamID().ConvertToUint64();
}

DLLFunction const char * _cdecl GetNameByID_DLL(UINT64 id)
{
	ISteamFriends* steamFriends = SteamFriends();
	if (!steamFriends)
		return "";

	return steamFriends->GetFriendPersonaName(id);
}

DLLFunction bool _cdecl ClearLoginInfo_DLL()
{
	ISteamUser* steamUser = SteamUser();
	if (!steamUser)
		return false;

	if (g_hAuthTicket != k_HAuthTicketInvalid)
	{
		SteamUser()->CancelAuthTicket(g_hAuthTicket);
		g_hAuthTicket = k_HAuthTicketInvalid;
		return true;
	}

	return false;
}

bool GetLoginInfo_NotEncrypt(BYTE * rgchToken, uint32& unTokenLen)
{
	ISteamUser* steamUser = SteamUser();
	if (!steamUser)
		return false;

	ClearLoginInfo_DLL();
	g_hAuthTicket = SteamUser()->GetAuthSessionTicket(rgchToken, 1024, &unTokenLen);
	return true;
}

bool s_receivedTicket = false;

uint32 k_unSecretData = 0x5444;
class CTosSteamClientEncrypt
{
public:
	void OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t *pEncryptedAppTicketResponse, bool bIOFailure)
	{
		s_receivedTicket = true;

		if (pEncryptedAppTicketResponse->m_eResult == k_EResultOK)
		{
			SteamUser()->GetEncryptedAppTicket(m_rgubTicket, sizeof(m_rgubTicket), &m_cubTicket);
			m_result = true;
		}
		else {

			if (s_steamAPIDebugTextHookFunc)
			{
				if (pEncryptedAppTicketResponse->m_eResult == k_EResultLimitExceeded)
				{
					s_steamAPIDebugTextHookFunc(0, "Calling RequestEncryptedAppTicket more than once per minute returns this error\n");
				}
				else if (pEncryptedAppTicketResponse->m_eResult == k_EResultDuplicateRequest)
				{
					s_steamAPIDebugTextHookFunc(0, "Calling RequestEncryptedAppTicket while there is already a pending request results in this error\n");
				}
				else if (pEncryptedAppTicketResponse->m_eResult == k_EResultNoConnection)
				{
					s_steamAPIDebugTextHookFunc(0, "Calling RequestEncryptedAppTicket while not connected to steam results in this error\n");
				}
			}			

			m_result = false;
		}
	}

	void Retrive()
	{
		SteamAPICall_t hSteamAPICall = SteamUser()->RequestEncryptedAppTicket(&k_unSecretData, sizeof(k_unSecretData));
		m_SteamCallResultEncryptedAppTicket.Set(hSteamAPICall, this, &CTosSteamClientEncrypt::OnRequestEncryptedAppTicket);
		s_receivedTicket = false;
	}

public:
	bool m_result;
	uint8 m_rgubTicket[1024];
	uint32 m_cubTicket;
	CCallResult< CTosSteamClientEncrypt, EncryptedAppTicketResponse_t > m_SteamCallResultEncryptedAppTicket;
};

CTosSteamClientEncrypt s_encryptTicketGetter;

bool GetLoginInfo_Encrypt(BYTE * rgchToken, uint32& unTokenLen)
{
	ISteamUser* steamUser = SteamUser();
	if (!steamUser)
		return false;

	s_encryptTicketGetter.Retrive();
	
	while (true)
	{
		Sleep(100);

		SteamAPI_RunCallbacks();
		if (s_receivedTicket)
			break;
	}

	if (s_encryptTicketGetter.m_result == false)
		return false;

	memcpy_s(rgchToken, unTokenLen, s_encryptTicketGetter.m_rgubTicket, s_encryptTicketGetter.m_cubTicket);
	unTokenLen = s_encryptTicketGetter.m_cubTicket;

	return true;
}

DLLFunction bool _cdecl Steam_GetLoginInfo_DLL(BYTE * rgchToken, uint32& unTokenLen, bool useEncrypt)
{
	if (useEncrypt)
	{
		return GetLoginInfo_Encrypt(rgchToken, unTokenLen);
	}
	else {
		return GetLoginInfo_NotEncrypt(rgchToken, unTokenLen);
	}
}
