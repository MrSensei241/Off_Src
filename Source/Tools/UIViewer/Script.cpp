#include "StdAfx.h"

#include "imcd3d9.h"
#include "imcFont.h"
#include "imcUIRender.h"
#include "imcSkin.h"
#include "imcUI.h"
#include "imcUIControls.h"

#include "Script.h"
#include "MsgHandlerUsr.h"

#include "app.h"

std::string	g_basePath;
imc::CVector<std::string>	g_frameList;

void Open(const char* argStr, int argNum)
{
	// listbox 얻기
	ui::IObject* pFrameObj = ui::GetFrame("FrameList");
	
	std::string frameName;
	if (pFrameObj != NULL)
	{
		ui::IObject* pListObj = pFrameObj->GetChild("List");

		frameName = ui::Get_IListBox(pListObj)->GetSelItemText();
	}

	// 프레임 로딩
	std::string uiPath = g_basePath;
	uiPath += frameName.c_str();

	ui::IObject* pFrame = ui::GetFrame(frameName.c_str());
	
	if (pFrame != NULL)
	{
		ui::DestroyFrame(pFrame->GetName());
	}

	pFrame = ui::CreateFrameFromXML(uiPath.c_str());

	if (pFrame != NULL)	
	{
		pFrame->ShowWindow(TRUE);

		std::string	scpFileName = uiPath;
		scpFileName += "\\";
		scpFileName += frameName.c_str();
		scpFileName += ".scp";

		// 스크립트 처리 추가
		if (imcfile::IsExistFile(scpFileName.c_str()))
		{
			imcIES::LoadScript(scpFileName.c_str());
		}

		char szNameText[64];
		strcpy(szNameText, frameName.c_str());
		imc::StrToUpper(szNameText);
		std::string initFuncName = szNameText;
		initFuncName += "_ON_INIT";

		//@breif : 프레임이 초기화 될때의 스크립트 실행	
		imcIES::IThread* pThread = imcIES::CreateThread(imc::CStringID(initFuncName.c_str()), true);
		if (pThread != NULL)
		{
			pThread->PushTypedUserData(NULL, "geui::CAddOn");
			pThread->PushTypedUserData(pFrame, "ui::CFrame");
			pThread->Resume();
		}
	}
	else
	{
		std::string str = frameName.c_str();
		str =" < " + str + " > 프레임을 로딩할수 없습니다";
		ui::MsgBox(str.c_str());
	}
}

void SetBasePath(const char* path)
{
	g_basePath = path;
}

const char* GetBasePath()
{
	return g_basePath.c_str();
}

BOOL CheckChildDirectory()
{
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;
	std::string path = g_basePath;
	path += "*.";

	g_frameList.RemoveAll();
	
	HANDLE hSrch = FindFirstFile(path.c_str(), &wfd);

	if (hSrch == INVALID_HANDLE_VALUE) 
		return FALSE;

	while(bResult)
	{
		DWORD attr = wfd.dwFileAttributes;
		BOOL isDirectory = attr & FILE_ATTRIBUTE_DIRECTORY;
		BOOL isHidden	 = attr & FILE_ATTRIBUTE_HIDDEN;
		if  ( isDirectory && !isHidden )
		{
			std::string str	=	wfd.cFileName;
			if (str != "." && str != ".." )
			{
				g_frameList.AddToTail(str);
			}
		}
		bResult= FindNextFile(hSrch, &wfd);

		if (bResult == FALSE)
			break;
	}

	return TRUE;
}

int GetFrameXMLCount()
{
	return g_frameList.Count();
}

const char* GetFrameXMLName(int index)
{
	if (index >= GetFrameXMLCount())
		return NULL;

	return g_frameList[index].c_str();
}


#include "script_bind.h"

TOLUA_API int tolua_script_open (lua_State* tolua_S);

namespace script
{

void Init(const char* rootPath)
{
	imcIES::GetMainThread()->RegisterFunctionSet(tolua_script_open);
 
 	g_basePath = rootPath;
 	g_basePath += "addon\\";

	CheckChildDirectory();
}

int GetFrameCount()
{
	return g_frameList.Count();
}

const char* GetFrameName(int index)
{
	if (index >= GetFrameCount())
		return NULL;

	return g_frameList[index].c_str();
}

}	

