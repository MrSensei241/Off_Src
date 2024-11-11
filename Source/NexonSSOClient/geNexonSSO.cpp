#pragma  once

#include "StdAfx.h"
#include "geNexonSSO.h"

#include "NMManager.h"
#include "NMEventHandler.h"

#define	_nmman	CNMManager::GetInstance()
#define	_nmco	CNMCOClientObject::GetInstance()

#include "geClientNexonSSO.h"

#include "imcCommandLine.h"
#include "imcCore.h"

namespace geNexonSSO
{
	void SSOLog(const char * msg, ...)
	{
		char msgBuf[2048];
		va_list args;
		va_start(args, msg);
		vsprintf_s(msgBuf, 2048, msg, args);
		va_end(args);

		IMC_LOG("WARNING_SSO_LOG", ": %s", msgBuf);
	}
	
	std::string AuthResultToString(NMLoginAuthReplyCode uAuthResult)
	{
		switch (uAuthResult)
		{
		case kLoginAuth_ServiceShutdown:			return _T("인증 서비스가 일시적으로 중단되었습니다. 홈페이지 공지사항을 확인해주세요.");
		case kLoginAuth_NotAllowedLocale:			return _T("지역 정보가 맞지 않습니다.");
		case kLoginAuth_BlockedIP:					return _T("제한된 IP 주소로 로그인이 불가능합니다.");
		case kLoginAuth_TempBlockedByLoginFail:		return _T("로그인을 10회 이상 실패했습니다. 계정보호를 위해서 10분간 로그인을 중지합니다.");
		case kLoginAuth_BlockedByAdmin:				return _T("관리자에 의해 사용이 중지된 아이디입니다. 로그인이 불가능합니다.");
		case kLoginAuth_UserNotExists:				return _T("입력하신 아이디 또는 비밀번호가 정확하지 않습니다. 확인 후 다시 시도해주세요.");
		case kLoginAuth_WrongPassword:				return _T("입력하신 아이디 또는 비밀번호가 정확하지 않습니다. 확인 후 다시 시도해주세요.");
		case kLoginAuth_WrongOwner:					return _T("현재 아이디는 본인 아님이 확인되어 사용하실 수 없습니다.");
		case kLoginAuth_TempUserLoginBlocked:		return _T("회원님은 임시가입 상태입니다. 넥슨 홈페이지에서 본인인증 완료 후 로그인해주세요.");
		case kLoginAuth_DeactivatedAccount:			return _T("고객님은 장기미접속 회원으로 본인확인 후 로그인하실 수 있습니다.");
		case kLoginAuth_ForcePasswordExpired:		return _T("비밀번호 변경 캠페인 진행 중 입니다. 비밀번호 변경 후 다시 로그인해주세요.");
		case kLoginAuth_BlockedByServiceArea:		return _T("해외 로그인 차단 기능이 실행되었습니다. 넥슨닷컴에서 로그인하시면 본인확인을 통해 안전하게 로그인 할 수 있습니다.");
		case kLoginAuth_BlockedBySafeLogin:			return _T("사용이 제한된 아이디입니다.");
		case kLoginAuth_TransMember:				return _T("이메일 아이디로 전환되어 있어 이메일 아이디로 로그인 하셔야 합니다.");
		case kLoginAuth_GuestServiceClosed:			return _T("고객님께서 이용하시는 폰번호체험ID는 사용기간이 만료되었습니다. 정규 이메일ID로 전환 후 넥슨 서비스를 이용해주시기 바랍니다.");
		case kLoginAuth_PCBangLoginNotMatchOTP:		return _T("PC방 로그인 번호가 잘못되었습니다. 번호를 확인한 후 8자리 숫자를 입력해주시기 바랍니다.");
		case kLoginAuth_PCBangLoginIPBlock:			return _T("PC방 로그인 번호를 10회 이상 잘못 입력하셨습니다. 잠시 후 다시 이용해주시기 바랍니다.");
		default:									return _T("로그인에 실패했습니다. 잠시 후에 다시 시도해주세요.");
		}

		return "";
	}
	
	class CPlatformEventHandler : public CNMEventHandler
	{
	public:
		//
		//	인증서버와의 접속이 끊어졌을 경우에 발생
		//
		virtual void OnAuthConnectionClosedEvent(CNMAuthConnectionClosedEvent * pEvent)
		{
			char tmpBuf[4096];
			sprintf_s(tmpBuf, "%d, %s", pEvent->uType, AuthResultToString(static_cast<NMLoginAuthReplyCode>(pEvent->uType)).c_str());
			MessageBox(0, tmpBuf, "", 0);
		}
	};

	CPlatformEventHandler g_msngrEventHandler;

	bool Init(HWND& hWnd, const char * nexonSSOServer, const char * nexonPassport, const char * serviceNation)
	{
		_nmman.Init();

		// const std::string& serviceNation = gecfg::GetServiceNation();

		NMLOCALEID		uLocaleId = kLocaleID_KR; // 일단 한국을 default
		NMREGIONCODE	uRegionCode = kRegionCode_Default;
		NMGameCode		uGameCode = kGameCode_tos_stage;


		if (strcmp(nexonSSOServer, "TOSLive") == 0)
		{
			uGameCode = kGameCode_tos;
		}
		else if (strcmp(nexonSSOServer, "TOSStage") == 0)
		{
			uGameCode = kGameCode_tos_stage;
		}
		else if (strcmp(nexonSSOServer, "TOSTestServer") == 0)
		{
			uGameCode = (NMGameCode)106523;
		}
		else{
			char errorMsgBuf[128];
			sprintf_s(errorMsgBuf, "%s", nexonSSOServer);
			MessageBox(0, "nexonSSOServer value is wrong : %s", "", 0);
			return false;
		}

		if (strcmp(serviceNation, "KOR") == 0)
		{
			uLocaleId = kLocaleID_KR;
			uRegionCode = kRegionCode_Default;
		}
		else if (strcmp(serviceNation, "JP") == 0)
		{
			uLocaleId = kLocaleID_JP;
			uRegionCode = kRegionCode_JP10;
		}
		else{
			char errorMsgBuf[128];
			sprintf_s(errorMsgBuf, "%s", serviceNation);
			MessageBox(0, "serviceNation value is wrong : %s", "", 0);
		}


		BOOL			bPatchOption = TRUE;
		BOOL			bWebLaunchingOption = TRUE;

		_nmco.SetPatchOption(bPatchOption);

		BOOL bResult = TRUE;
		if (bResult)
		{
			INT32	nMainErr = kNMModuleError_OK;
			INT32	nSubErr = 0;
			bResult = _nmco.SetLocaleAndRegion(uLocaleId, uRegionCode, &nMainErr, &nSubErr);
			SSOLog("_nmco.SetLocale: %d, %d!", nMainErr, nSubErr);
		}

		//
		//	[필수] 플랫폼 모듈 초기화
		//
		if (bResult)
		{
			bResult = _nmco.Initialize(uGameCode);
		}

		//
		//	[선택] 클라이언트 패치
		//
		//		-	NGM의 기능 중 패치 기능을 사용할 때 구현 예제 입니다.
		//		-	패치가 필요한지 여부는 게임에서 결정합니다.
		//		-	이 예제에서는 아래 bNeedPatch 값에 따라 패치 여부를 결정합니다.
		//
		BOOL bNeedPatch = FALSE;

		if (bNeedPatch)
		{
			//
			//	NGM 설치 여부 확인
			//
			//		-	NGM 을 이용해서 게임 클라이언트를 패치하기 때문에
			//			NGM 이 설치되어 있지 않으면 패치가 불가능합니다.
			//
			if (bResult)
			{
				bResult = NMCOHelpers::IsNGMInstalled();
				SSOLog("NMCOHelpers::IsNGMInstalled(): %d!", bResult);
			}

			//
			//	패치 실행
			//
			//		-	szPatchURL 및 szPatchDir 은 게임팀에서 수정하여 테스트 하시기 바랍니다.
			//
			if (bResult)
			{
				TCHAR*	szPatchURL = _T("patch url");
				TCHAR*	szPatchDir = _T("patch dir");
				TCHAR	szCmdLine[2048];

				::_tcscpy(szCmdLine, ::GetCommandLine());

				bResult = NMCOHelpers::ExecuteNGMPatcher(uGameCode, szPatchURL, szPatchDir, szCmdLine);
				SSOLog("NMCOHelpers::ExecuteNGMPatcher(): %d!", bResult);
				SSOLog("    PatchURL: %s", szPatchURL);
				SSOLog("    PatchDir: %s", szPatchDir);
				SSOLog("    CmdLine:  %s", szCmdLine);

				//
				//	게임 클라이언트 종료
				//
				//		-	실제 게임 클라이언트는 ExecuteNGMPatcher() 호출 후 종료해야 합니다.
				//			그래야 NGM이 게임 클라이언트 파일을 패치할 수 있기 때문입니다.
				//
				//		-	단, 이 예제에서는 프로그램을 종료하지 않고 계속 진행하겠습니다.
				//
			}
		}

		//
		//	[필수] 이벤트 핸들러 등록
		//
		//		-	플랫폼모듈에서 발생하는 각종 이벤트를 받아 볼 수 있도록 이벤트 핸들러를 등록합니다
		//
		if (bResult)
		{
			bResult = _nmco.RegisterCallbackMessage(hWnd, WM_NMEVENT);
			SSOLog("_nmco.RegisterCallbackMessage( 0x%08x, WM_NMEVENT ): %d!", bResult);
		}

		//
		//	[필수] 인증서버 로그인
		//
		//		-	이 함수는 동기적으로 실행되므로 함수가 리턴할 때까지 프로세스는 블록됩니다.
		//
		if (bResult)
		{
			//
			// 웹 런칭 방식에 대한 인증 플로우 입니다.
			//
			if (bWebLaunchingOption)
			{
				//
				//	-	커맨드라인으로부터 패스포트 스트링을 추출해 냅니다.
				//	-	아래 코드는 "GameClient.exe PassportString" 형식으로, 
				//	-	클라이언트 경로 뒷 부분에 한칸 띄우고 바로 패스포트 스트링이 넘어오는 경우를 가정하고 있습니다.
				//	-	일반적으로는 위 형식을 가장 많이 쓰지만 커맨드라인을 통한 패스포트 전달 형식은 게임팀에서 지정하는 것이 가능합니다.
				//
				//const char * nexonPassport = imccmdline::ParmValue("-NexonPassport", "");

				if (nexonPassport && strlen(nexonPassport) > 0)
				{
					//
					//	-	커맨드라인에서 패스포트 스트링 추출이 성공한 경우, 웹런칭 형식으로 동작합니다.
					//
					NMLoginAuthReplyCode resultAuth = _nmco.AttachAuth(nexonPassport);

					if (resultAuth == kLoginAuth_OK)
					{
						SSOLog("_nmco.AttachAuth( \"%s\" ): OK!", nexonPassport);
						bResult = TRUE;
					}
					else
					{
						SSOLog("_nmco.AttachAuth( \"%s\" ): Failed!!!", nexonPassport);
						SSOLog("    Result: 0x%08x, \"%s\"", resultAuth, AuthResultToString(resultAuth).c_str());
						bResult = FALSE;
					}
				}
				else
				{
					SSOLog("Nexon Passport is empty");
				}
			}
			//
			// 스탠드 얼론 방식에 대한 인증 플로우 입니다.
			//
			else
			{
				//
				//	-	편의상 로그인 계정 정보를 Client.ini 파일로부터 읽도록 해 두었습니다
				//
				CAtlString strNexonID = ""; // ClientUtil::Settings::GetNexonId();
				CAtlString strPassword = ""; // ClientUtil::Settings::GetPassword();

				INT32	nSubErrorCode = 0;
				NMLoginAuthReplyCode resultAuth = _nmco.LoginAuth(strNexonID, strPassword, 0, NULL, 0, &nSubErrorCode);
				if (resultAuth == kLoginAuth_OK)
				{
					SSOLog("_nmco.LoginAuth( \"%s\", \"%s\" ): OK!", strNexonID, strPassword);
					bResult = TRUE;
				}
				// !주의 - 국내에서는 사용되지 않습니다.
				// 해외일 경우 kLoginAuth_ThirdPartyError 가 발생하면
				// 해외 퍼블리셔에서 제공하는 Login API의 Error 명세에 따라 nSubErrorCode를 참고하여 핸들링 한다.
				/*else if ( resultAuth == kLoginAuth_ThirdPartyError )
				{
				ClientUtil::Log::LogTime( _T("_nmco.LoginAuth( \"%s\", \"%s\" ): Failed!!!"), strNexonID, strPassword );
				ClientUtil::Log::LogInfo( _T("    Result: %d, %d"), resultAuth, nSubErrorCode );
				bResult = FALSE;
				}*/
				else
				{
					SSOLog("_nmco.LoginAuth( \"%s\", \"%s\" ): Failed!!!", strNexonID, strPassword);
					SSOLog("    Result: %d: \"%s\"", resultAuth, AuthResultToString(resultAuth).c_str());
					bResult = FALSE;
				}
			}
		}

		if (FALSE == bResult)
		{
			MessageBox(0, "Nexon passport auth failed", "", 0);
			return false;
		}

		return true;
	}


	// 플랫폼 모듈 내에서 nexon passport는 계속 갱신을 하고있음
	// 따라서 게임 서버에서 CheckSession을 호출하기 직전에 게임 클라에서는
	// GetNexonPassport()함수를 통해 갱신된 nexon passport를 얻어와 사용해야한다고함.
	void UpdateNexonPassport()
	{
		//	넥슨패스포트 얻기
		TCHAR szNexonPassport[NXPASSPORT_SIZE];
		SSOLog("    Nexon Passport: %s", _nmco.GetNexonPassport(szNexonPassport));

		//	[필수] 게임 서버를 통한 2차 인증
		//
		//		-	게임 서버와의 인증은 샘플로 제작되어 있지 않지만,
		//		-	게임 클라이언트는 GetNexonPassport() 함수를 통해 받아온 넥슨 패스포트를 게임 서버로 전송하여 게임 서버와 넥슨 인증 서버 간의 2차 인증을 거친다.
		
		geClientNexonSSO::SetPassport(szNexonPassport);
	}

	LRESULT OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COMMAND:
			/*
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch (wmId)
			{
			case ID_FILE_LOGOUTAUTH:
				//
				//	인증서버에서 로그아웃 합니다
				//
			{
									   BOOL bResult = _nmco.LogoutAuth();
									   SSOLog("_nmco.LogoutAuth(): %d!", bResult);
			}
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			*/
			break;
		case WM_DESTROY:
			//
			//	인증서버에서 로그아웃
			//
			_nmco.LogoutAuth();
			//
			//	이벤트 핸들러 등록 제거
			//
			_nmco.ResetCallbackMessage(hWnd);
			//
			//	매니저 클래스 종료
			//
			_nmman.Finalize();

			break;
		case WM_NMEVENT:
			//
			//	메신저 이벤트 처리
			//
			g_msngrEventHandler.HandleNMEvent(wParam, lParam);
			break;
		default:
			break;
		}
		return 0;
	}
		
};
