// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>
#include <xstring>
#include "steam_api.h"
// steam api header file
#include "steam_api.h"
#include "isteamuserstats.h"
#include "isteamremotestorage.h"
#include "isteammatchmaking.h"
#include "steam_gameserver.h"

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.


#ifdef STEAM_CEG
// Steam DRM header file
#include "cegclient.h"
#else
#define Steamworks_InitCEGLibrary() (true)
#define Steamworks_TermCEGLibrary()
#define Steamworks_TestSecret()
#define Steamworks_SelfCheck()
#endif

