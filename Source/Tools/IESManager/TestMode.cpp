#include "StdAfx.h"

#include "GeMode.h"
#include "TestMode.h"
#include "TestView.h"
#include "TestWorld.h"
#include "TestObject.h"
#include "app.h"
#include "GePropXml.h"

float	fWorldCreateTime = 0.0f;
float	fWorldCreateEndTime = 0.0f;

namespace geapp
{

void CTestMode::Init(const char* worldFileName)
{
	m_pView		= new CTestView;
	m_pView->Init();
	m_pWorld	= new CTestWorld;

	float fStart = imc::GetAppTime();
	CMode::Init(worldFileName);	
	float fEnd	 = imc::GetAppTime();

	fWorldCreateEndTime	 = imc::GetAppTime();
	
	fWorldCreateTime = fWorldCreateEndTime - fStart;
}

BOOL CTestMode::Update()
{
	CMode::Update();
	return TRUE;
}

void CTestMode::Run()
{
	app::Run();
}

BOOL CTestMode::LoadZone(BOOL enableSubDir, gepath::DIRTYPE bgDirType, const char * fileName)
{
	gepath::SetBgDataPath(bgDirType);

	std::string fullPath = gepath::GetPath(bgDirType, "", fileName);

	std::string propFName	= fullPath + ".3dprop";
	std::string worldFName	= fullPath + ".3dworld";
	std::string mtlFName	= fullPath + ".3dmtl";
	std::string lightFName	= fullPath + ".3dlight";
	std::string renderFName	= fullPath + ".3drender";
	std::string effectFName	= fullPath + ".3deffect";
	std::string soundFName	= fullPath + ".3dsound";
	std::string areaFName	= fullPath + ".3darea";
	std::string watchFName	= fullPath + ".3dwatch";
	std::string scriptFName	= fullPath + ".lua";

	BOOL existWorldFile	= imcfile::IsExistFile(worldFName.c_str());
	BOOL existRenderFile= imcfile::IsExistFile(renderFName.c_str());
	BOOL exitPropFile	= imcfile::IsExistFile(propFName.c_str());

	BOOL existEffectFile= imcfile::IsExistFile(effectFName.c_str());
	BOOL existSoundFile	= imcfile::IsExistFile(soundFName.c_str());
	BOOL existWatchFile	= imcfile::IsExistFile(watchFName.c_str());

	BOOL existAreaFile	= imcfile::IsExistFile(areaFName.c_str());
	BOOL existScriptFile= imcfile::IsExistFile(scriptFName.c_str());

	GetPropertyXml()->Reset();

	if (exitPropFile) {
		GetPropertyXml()->Load(propFName.c_str());
	}

	if (existWorldFile)
	{
		if (!existRenderFile) {
			renderFName  = gepath::GetPath(bgDirType, "", "default.3drender");
		}

		m_pWorld->OnCreate(worldFName.c_str(), renderFName.c_str(), mtlFName.c_str(), lightFName.c_str());
		//m_pWorld->OnCreate("None", renderFName.c_str(), mtlFName.c_str(), lightFName.c_str());
	}
	else 
	{
		return FALSE;
	}
	return TRUE;
}

};