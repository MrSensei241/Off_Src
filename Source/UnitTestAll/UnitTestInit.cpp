#include "StdAfx.h"
#include "UnitTestInit.h"

#include "imcd3d9.h"
#include "imc3d.h"
#include "imc3dRenderer.h"
#include "imcscene.h"
#include "imcSceneMgr.h"

#include "imcRes.h"
#include "imcDirMgr.h"
#include "imcTimer.h"

HWND g_hWnd = NULL;

CUnitTestInit::CUnitTestInit(void)
{
}

CUnitTestInit::~CUnitTestInit(void)
{
}

void CUnitTestInit::InitTest()
{
	InitIES();
	InitWin32();
}

void CUnitTestInit::ExitTest()
{
	ExitIES();
	ExitWin32();
}

const char*	testScript = 
"function test() 					"	\
"	return 3;					"	\
"end";

const char*	testScript2 = 
"function test2() 					"	\
"end";

void CUnitTestInit::InitIES()
{
	std::string strTestPath = std::string(imc::GetBinPath());
	imcIES::RegisterClassFromXML((strTestPath+"test\\datatable_pc.xml").c_str());	//xml 로딩
	imcIES::LoadScriptString(testScript);
	imcIES::LoadScriptString(testScript2);
	imcIES::LoadScript((strTestPath+"test\\/get_status_pc.scp").c_str());
	imcIES::LoadScript((strTestPath+"test\\/calc_battle_pc.scp").c_str());
	imcIES::LoadScript((strTestPath+"test\\/TestScript.scp").c_str());				//Test를 위한 Script
}

HRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}   
	return static_cast<HRESULT>(DefWindowProc(hWnd,uMsg,wParam,lParam));
}

void CUnitTestInit::InitWin32()
{
	WNDCLASS wc ={ CS_VREDRAW | CS_HREDRAW , WndProc, 0, 0, NULL, LoadIcon(0, IDI_APPLICATION), 
		LoadCursor(0, IDC_ARROW), 0, 0, "InputTest"};

	RegisterClass(&wc);
	HWND hWnd = CreateWindow("InputTest", "InputTest", WS_POPUP | WS_VISIBLE, 0 ,0, 640, 480, 0, 0, NULL, 0);
	g_hWnd = hWnd;
}

void CUnitTestInit::ExitIES()
{
	imc::CSortVector<std::string> names, types;
	imcIES::GetMainThread()->DumpTable("io", names, types);
	imcIES::Reset();
}

void CUnitTestInit::ExitWin32()
{
	SendMessage(g_hWnd, WM_CLOSE, 0, 0);
}