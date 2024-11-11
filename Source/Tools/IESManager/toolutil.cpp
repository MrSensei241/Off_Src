#include "stdafx.h"
#include "toolutil.h"
#include "toolnet.h"
#include "GeSession.h"
#include "geCrypt.h"
#include "PaneManager.h"
#include "PaneLogin.h"
#include "PaneIESList.h"

namespace util
{
	std::string GetConfigIniPath()
	{
		std::string path = gepath::GetPath(gepath::DIRTYPE_CODEBASE, "balanacemanager.ini");
		return path;
	}

	void InitTool()
	{
		gepath::Init(imc::GetBinPath());

		geapp::XmlInit();
		geapp::TableInit();
		geCrypt::Init();

		InitServerList();

		net::Init();

	}

	void ExitTool()
	{
		net::Exit();
		geapp::TableExit();
		geCrypt::Exit();
	}

	void WriteLastLoginInfo()
	{
		imc::CStringID id = GetPaneManager().GetPaneLogin()->GetLoginID();
		imc::CStringID server = GetPaneManager().GetPaneLogin()->GetLoginServerName();

		std::string path = GetConfigIniPath();
		WritePrivateProfileString("Login", "ID", id.c_str(), path.c_str());
		WritePrivateProfileString("Login", "Server", server.c_str(), path.c_str());		

	}

	void InitServerList()
	{
		TiXmlDocument svrList;
		imcfile::IFileMem* pFileMem = imcfile::Load(gepath::GetPath(gepath::DIRTYPE_CODEBASE, "serverlist.xml").c_str(), true);
		if (pFileMem && svrList.Parse(pFileMem->GetData()) && svrList.Error()==0)
		{
			imcfile::Close(pFileMem);
			Session().LoadServerList(svrList);
		}
	}

	void UpdateAllHistory()
	{
		GetPaneManager().GetPaneIESList()->UpdateAll();
	}

	void UpdatePaneVisible()
	{
		static DWORD lastTime = timeGetTime();
		if (timeGetTime() - lastTime < 200)
			return;

		lastTime = timeGetTime();

		BOOL isCon = net::IsConnected();
		//GetPaneManager().ShowLoginPane(!isCon);
		GetPaneManager().ShowPane(IDR_LOGIN_PANE, !isCon);
		GetPaneManager().ShowPane(IDR_IES_PANE, isCon);
	}


};

