#include "stdAfx.h"

#include <winuser.h>
#include <shellapi.h>

#include "imcd3d9.h"
#include "imcFont.h"
#include "imcUIRender.h"
#include "imcSkin.h"
#include "imcUI.h"
#include "imcUIControls.h"
#include "imcMessage.h"
#include "imcForkParticle.h"

// NOTE:: FlashTest
#include "imcFlash.h"

#include "Script.h"
#include "MsgHandlerUsr.h"

#include "Script.h"

#include "app.h"

app* g_pApp = NULL;
ui::IMessageHandler* g_pMsgHandler = NULL;

char * CWordBreaker::GetNextChar(char * pText){
	return ::CharNextExA(CP_ACP, pText, 0);
};

static imcFlash::IFlashObject* s_testFlashObj;

void app::Init(HINSTANCE hInst)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	m_hInstance	=	hInst;

	strcpy(szTitle, "UIViewer");
	strcpy(szWindowClass, "UIViewer");

	m_hWnd			=	NULL;
	m_wndWidth		=	1024;
	m_wndHeight		=	768;

	m_pMsgHandler	=	NULL;

	m_isSleepMode	=	TRUE;
	
	_MyRegisterClass();
	_InitInstance();

	if (_InitBaseLib() == FALSE)
	{
		MessageBox(m_hWnd, "Base Library Initializing is failed.", "Initialize Error", MB_ICONERROR);
	}

	if (_InitEtcLib() == FALSE)
	{
		MessageBox(m_hWnd, "Expand Library Initializing is failed.", "Initialize Error", MB_ICONERROR);
	}

	std::string scriptPath;
	scriptPath = imc::GetBinPath();
	scriptPath += "\UIviewer.lua";

	imcIES::LoadScript(scriptPath.c_str());

	clientmessage::Init(imc::GetDataPath("xml_lang\\", "clientmessage.xml").c_str());

	//imcFont::SetUseFreeFont(FALSE);

#ifdef _FINAL_RELEASE
 	CreateListFrame();

#else

	CreateTestFrame_List();
	CreateTestFrame_Button();
	CreateTestFrame_RadioButton();
	CreateTestFrame_Picture();
	CreateTestFrame_SlideBar();
	CreateTestFrame_StaticText();
	CreateTestFrame_RichText();
	CreateTestFrame_Tree();
	CreateTestFrame_TextView();
	CreateTestFrame_GroupBox();
	CreateTestFrame_Gauge();
	CreateTestFrame_Tab();
	CreateTestFrame_Slot();
	CreateTestFrame_ControlSet();
 	CreateTestFrame_FlashControl();
//	CreateTestFrame_DropList();
//	CreateTestFrame_EditBox();
 
// 	CreateTestFrame_TextFrame();
// 	CreateTestFrame_ContextMenuFrame();
// 	CreateTestFrame_MessageBoxFrame();
// 
// 	CreateTestFrame_FrameResize();
// 	CreateTestFrame_FrameMove();
// 	CreateTestFrame_ImageItem();
// 	CreateTestFrame_FrameFade(); 	

//	CreateTestFrame_ToolTip(); 	

//	CreateTestFrame_SkinBorder();

	std::string path = imc::GetDataPath("ui\\flashmovie\\", "34221.swf");

	RECT rt = {0, 0, 300, 200};
	s_testFlashObj = imcFlash::GetFlashObject(path.c_str(), &rt);
	s_testFlashObj->Play();

#endif
}

ATOM app::_MyRegisterClass()
{
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;	
	wcex.lpfnWndProc	= (WNDPROC) WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInstance;
	wcex.hIcon			= LoadIcon(m_hInstance, NULL);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, NULL);

	return RegisterClassEx(&wcex);
}

BOOL app::_InitInstance()
{
	m_hWnd = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		szWindowClass,
		szTitle,
		WS_POPUP,
		0, 0,
		m_wndWidth,
		m_wndHeight,
		NULL,
		NULL,
		m_hInstance,
		NULL);

	if (m_hWnd == NULL)
		return FALSE;

	RECT clientRect;
	GetClientRect(m_hWnd, &clientRect);

	int width		= m_wndWidth * 2 - clientRect.right;
	int height		= m_wndHeight * 2 - clientRect.bottom;

	MoveWindow(m_hWnd, 0, 0, width, height, TRUE);
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return TRUE;
}

BOOL app::_InitBaseLib()
{
	imc::InitTimer();
	imccore::Init(".\\LoggerConfig_UIViewer.xml");
	imcresmgr::Init();
	imcinput::Init(m_hWnd, m_hWnd);

	imcIES::Init();

	std::string xmlPath = imc::GetDataRootPath();
	xmlPath += "xml_client\\datatable_cheat.xml";
	imcIES::RegisterClassFromXML(xmlPath.c_str());

	char parentPath[MAX_PATH], childPath[MAX_PATH];

	std::string codeBasePath = imc::GetDataRootPath();
	imc::StrSplitParentChildPathString(codeBasePath.c_str(), parentPath, childPath);	
	imcfile::SetUseCodeBasePath(FALSE);

	if ( imcd3d9::Init( m_hWnd, FALSE, m_wndWidth, m_wndHeight, 32 ) == FALSE) 
	{
		imcd3d9::DEVICEERRORCODE errCode = imcd3d9::GetDevice()->GetErrorCode();
		return FALSE;
	}

	std::string forkParticlePath = imc::GetDataRootPath();
	forkParticlePath += "forkparticle\\";
	imcForkParticle::Init(forkParticlePath.c_str());

	return TRUE;
}

BOOL app::_InitEtcLib()
{
	g_pApp = this;

	imclang::Init();

	std::string rootPath	= imc::GetDataRootPath();
	std::string binPath		= imc::GetBinPath();
	std::string fontPath	= rootPath + "font\\";

	script::Init(binPath.c_str());	

	if ( imcUiRender::Init() == FALSE)
		return FALSE;

	if ( imcFont::Init( fontPath.c_str() ) == FALSE)
		return FALSE;

	std::string skinPath			= rootPath + "ui\\";
	std::string defaultSkinFilePath	= skinPath + "uixml\\baseskinset.xml";	
	if ( imcSkin::Init( skinPath.c_str()) == FALSE)
		return FALSE;

	std::string uiDataPath			= rootPath + "ui\\";
	if ( ui::Init(m_hWnd, m_hInstance, m_wndWidth, m_wndHeight, uiDataPath.c_str()) == FALSE)
		return FALSE;

	imcFlash::Init();

	// 메세지 핸들러 인스턴스 설정
	g_pMsgHandler = new CMsgHandlerUsr;
	ui::SetMsgHandler(g_pMsgHandler);

	return TRUE;
}

void app::Exit()
{
	SAFE_DELETE(g_pMsgHandler);

 	ui::Exit();
	imcSkin::Exit();

	imcUiRender::Exit();
	imcFont::Exit();

	imcinput::Exit();
	imclang::Exit();
	imcIES::Reset();

	imcresmgr::DestroyResAll();
	imcresmgr::Exit();
	imcd3d9::Exit();
	imccore::Exit();
	imc::ExitTimer();

	imcFlash::Exit();

	::DestroyWindow(m_hWnd);
}

void app::Run()
{
	MSG msg;
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	while (TRUE)
	{	
		if (m_isSleepMode == TRUE)
			Sleep(20);

		imcinput::Update();

		ui::MOUSE_INFO mouseInfo;
		mouseInfo.m_x			= imcinput::GetMouse()->GetX();
		mouseInfo.m_y			= imcinput::GetMouse()->GetY();
		mouseInfo.m_wheel			= imcinput::GetMouse()->GetWheel();
		mouseInfo.m_buttonState[0]	= (ui::BUTTON_STATE) imcinput::GetMouse()->GetLButtonState();
		mouseInfo.m_buttonState[1]	= (ui::BUTTON_STATE) imcinput::GetMouse()->GetRButtonState();
	
		ui::SetMouseMessage(mouseInfo);
		ui::ProcessMouseInput(0.0f);

 		Update();
  		Render();
			
		HCURSOR hcursor = GetCursor();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		SetCursor(hcursor);
	}

	_CrtCheckMemory();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::string	str;
	static int count = 0;

	switch (message) 
	{
		case WM_CHAR:			ui::CharMessage(wParam);	break;
		case WM_KEYUP:			ui::KeyupMessage(wParam);	break;
		case WM_SYSKEYDOWN:		ui::KeydownMessage(wParam);	break;
		case WM_SYSKEYUP:		ui::KeyupMessage(wParam);	break;

		case WM_DESTROY:		PostQuitMessage(0);			break;
		case WM_SYSCOMMAND:		return DefWindowProc(hWnd, message, wParam, lParam);
		case WM_DROPFILES:
			{
				HDROP hDrop = (HDROP)wParam;
				TCHAR szFileName[OFS_MAXPATHNAME];
				UINT fileNumber = ::DragQueryFile( hDrop, -1, szFileName, OFS_MAXPATHNAME);
				for ( UINT index = 0 ; index < fileNumber ; ++index ) 
				{
					::DragQueryFile( hDrop, index, szFileName, OFS_MAXPATHNAME);

					char dir[256] = {NULL};
					imc::StrSplitPathString(szFileName, dir, NULL);
					int length = strlen(dir);
					dir[length - 1] = NULL;

					ui::IObject* pFrame = ui::CreateFrameFromXML(dir);
					if (pFrame != NULL)
						pFrame->ShowWindow(TRUE);
				}
			}
			break;
		case WM_COMMAND:
			{
				int wmId, wmEvent;
				wmId = LOWORD(wParam);
				wmEvent = HIWORD(wParam);
			}
			break;

		case WM_KEYDOWN:
			if (wParam == VK_F1)
			{
				ui::IObject* pFrame = ui::GetFrame("FrameList");
				if (pFrame != NULL)
				{
					pFrame->ShowWindowToggle();
				}
			}

			if (wParam == VK_F2)
			{
				ui::IObject* pFrame = ui::GetTopMostFrame();
				ui::IObject* pBaseListFrame = ui::GetFrame("FrameList");
				if (pFrame != NULL && pFrame != pBaseListFrame)
				{					
					ui::CloseFrame(pFrame->GetName());
				}				
			}			
			
			if (wParam == VK_F3)
			{
				ui::IObject* pFrame = ui::GetTopMostFrame();
				ui::IObject* pBaseListFrame = ui::GetFrame("FrameList");
				if (pFrame != NULL && pFrame != pBaseListFrame)
				{					
					ui::DestroyFrame(pFrame->GetName());
				}		
			}		

			if (wParam == VK_F5)
			{
				g_pApp->ToggleSleepMode();
			}

			if (wParam == VK_F6)
			{
				imcIES::GetMainThread()->RunString("UIVIEWER_IconSet()");
				
			}


			ui::KeydownMessage(wParam);
			break;
			
	default:	return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void app::Update()
{
	
	// Note : LostDevice 테스트

	HRESULT hr = imcd3d9::GetDevice()->Get3DDevice()->TestCooperativeLevel();


	if (hr == D3DERR_DEVICELOST)
	{
		Sleep(500);
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		imcd3d9::GetDevice()->RestoreDevice();

		ui::InvalidateAll();
	}

}

void app::Render()
{
	HRESULT hr = imcd3d9::GetDevice()->Get3DDevice()->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
		return;

	static float elapsedTime	= imc::GetAppTime();
	static float curTime		= elapsedTime;
	static float prevTime		= elapsedTime;

	curTime		= imc::GetAppTime();
	elapsedTime = curTime - prevTime;
	prevTime	= curTime;

	imcd3d9::IDevice* pDevice = imcd3d9::GetDevice();
	IMC_ASSERT(pDevice);

	if (pDevice->BeginScene() == TRUE)
	{
		pDevice->Clear(0x00000000);

		ui::Process( elapsedTime );
		ui::Render( elapsedTime );

		s_testFlashObj->Show();
		s_testFlashObj->Update();

		//imcUiRender::Render(0, 0, 1024, 768, s_testFlashObj->GetRenderTexture());


		RenderHelpInfo();

		pDevice->EndScene();
		pDevice->Flip();
	}
}

void	app::RenderHelpInfo()
{
	imcd3d9::IDevice* pDevice = imcd3d9::GetDevice();
	IMC_ASSERT(pDevice);

	RECT windowRect;
	GetClientRect(m_hWnd, &windowRect);

	int width		= windowRect.right - windowRect.left;
	int bottom	= windowRect.bottom - windowRect.top;

	int y = 5;

	// NOTE : FPS
	static char fpsText[25] = {0};
	float fps = imc::GetFps(0.5f);
	imc::StrFormat(fpsText, "FPS : %.2f", fps);
	pDevice->DrawText(width - 200, y, fpsText, 0xffffffff);
	y += 15;

	// NOTE : 프레임 수
	static char frameCountText[128] = {0};		
	imc::StrFormat(frameCountText, "프레임수(보임/전체)[ %d / %d ]", ui::GetVisibleFrameCount(), ui::GetFrameCount());
	pDevice->DrawText(width - 200, y, frameCountText, 0xffffffff);
	y += 30;

	static char helpText[128] = {0};

	// NOTE : 애드온 프레임 리스트	
	imc::StrFormat(helpText, "[F1] 애드온 프레임 리스트");
	pDevice->DrawText(width - 200, y, helpText, 0xffffffff);
	y += 15;

	// NOTE : 최상위 프레임 닫기
	imc::StrFormat(helpText, "[F2] 최상위 프레임 닫기");
	pDevice->DrawText(width - 200, y, helpText, 0xffffffff);
	y += 15;

	// NOTE : 최상위 프레임 삭제
	imc::StrFormat(helpText, "[F3] 최상위 프레임 삭제");
	pDevice->DrawText(width - 200, y, helpText, 0xffffffff);
	y += 15;

	// NOTE : 슬립 모드 사용
	imc::StrFormat(helpText, "[F5] 슬립 사용 : %s ", m_isSleepMode? "Yes" : "No");
	pDevice->DrawText(width - 200, y, helpText, 0xffffffff);
	y += 15;
}

void app::CreateListFrame()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "FrameList", 0, 0, 300, 600, NULL);

	if (pFrame != NULL)
	{
		ui::Get_IFrame(pFrame)->SetTitleName("Frame List");
		ui::Get_IFrame(pFrame)->SetLayerLevel(0);

		ui::IObject* pListBOx = ui::CreateObject(ui::CLSID_LISTBOX, "List", 15, 5, 270, 580, pFrame);
		pListBOx->EnableScriptArgument(TRUE);
		pListBOx->SetLBtnDblClickScp("Open()");

		ui::Get_IListBox(pListBOx)->SetLineMargin(6);
		ui::Get_IListBox(pListBOx)->AddColumn("Frame Name");

		int frameCount = script::GetFrameCount();
		for (int i = 0; i < frameCount; ++i)
		{
			const char* frameName = script::GetFrameName(i);	

			ui::Get_IListBox(pListBOx)->AddItem(frameName);
		}
	}
}
