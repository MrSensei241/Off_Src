#include "stdafx.h"
#include "imcForestTest.h"
#include "imcDirMgr.h"
#include "imcd3d9Util.h"

//----------------------------------------------------------
//	win32 main
//----------------------------------------------------------
#define MAX_LOADSTRING 100
HINSTANCE	g_hInst;
HWND		g_hWnd;
TCHAR		szTitle[MAX_LOADSTRING];
TCHAR		szWindowClass[MAX_LOADSTRING];
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//----------------------------------------------------------
//	app main
//----------------------------------------------------------
void	AppInit();
void	AppRender();
void	AppExit();

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	ZeroMemory( &msg, sizeof(msg) );

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMCFORESTTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMCFORESTTEST));
	AppInit();

	while (msg.message!=WM_QUIT) {
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else if( imcForest::IsInitialize() )
			AppRender();
	}

	AppExit();
	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMCFORESTTEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance;
	g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 0, 1024, 768, NULL, NULL, hInstance, NULL);

	if (!g_hWnd)
		return FALSE;

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------
//	app main
//----------------------------------------------------------

void AppInit()
{
	imcd3d9::IDevice* pDev = imcd3d9::GetDevice();
	IMC_ASSERT(pDev);

	imcd3d9::Init(g_hWnd, TRUE, 1024, 768, 32);

	std::string testPath = imc::GetBinPath();
	testPath += "SpeedTreeTestData\\";

	imcForest::FOREST_INIT forestInit;
	forestInit.pD3DDev = pDev->Get3DDevice();
	forestInit.pShaderPath = testPath.c_str();
	forestInit.usePrecompliedShaders = true;
	imcForest::Init(forestInit);

	std::string fileName = testPath + "Acacia_RT.srt";
	imcForest::FOREST_TREE_INSTANCE instance;
	instance.pos = imc::vector3(-15.0f, 0.0f, 0.0f);
	instance.scale = 0.75f;
	imcForest::CreateTree(fileName.c_str(), instance);
	instance.pos = imc::vector3(0.0f, 0.0f, 20.0f);
	instance.scale = 1.5f;
	imcForest::CreateTree(fileName.c_str(), instance);
	instance.pos = imc::vector3(15.0f, 0.0f, 0.0f);
	instance.scale = 1.0f;
	imcForest::CreateTree(fileName.c_str(), instance);
}

void AppExit()
{
	imcd3d9::Exit();
}

void AppRender()
{
	imcd3d9::IDevice* pDev = imcd3d9::GetDevice();
	IMC_ASSERT(pDev);

	RECT r;
	GetClientRect(g_hWnd, &r);

	D3DVIEWPORT9 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = r.right;
	vp.Height = r.bottom;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;
	pDev->Get3DDevice()->SetViewport(&vp);

	pDev->Get3DDevice()->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xc0,0xc0,0xc0), 1.0f, 0);

	if (FAILED(pDev->Get3DDevice()->BeginScene())) {
		return;
	}

	imc::vector3 cameraPos(0.0f, 17.5f, -55.0f);
	imc::vector3 lookAt(0.0f, 17.5f, 0.0f);
	imc::vector3 cameraUp(0.0f, 1.0f, 0.0f);
	float cameraNear = 1.0f;
	float cameraFar = 5000.0f;
	float fov = D3DX_PI / 4;
	float aspect = float(r.right) / float(r.bottom);
	imc::matrix44 viewTM, projTM;
	imc::MatrixLookAtLH(&viewTM, &cameraPos, &lookAt, &cameraUp);
	imc::MatrixPerspectiveFovLH(&projTM, fov, aspect, cameraNear, cameraFar);

	if (imcForest::IsInitialize()) {
		imcForest::Update(cameraPos, viewTM, projTM, cameraNear, cameraFar);
		imcForest::Render();
	}

	pDev->Get3DDevice()->EndScene();
	pDev->Get3DDevice()->Present(&r, &r, g_hWnd, NULL);
}
