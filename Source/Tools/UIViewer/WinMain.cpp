#include "stdafx.h"

#include "imcd3d9.h"
#include "imcFont.h"
#include "imcUIRender.h"
#include "imcSkin.h"
#include "imcUI.h"
#include "imcUIControls.h"

#include "Script.h"
#include "MsgHandlerUsr.h"

#include "app.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	app	g_App;
	g_App.Init(hInstance);
	g_App.Run();
	g_App.Exit();

	return 0;
}

