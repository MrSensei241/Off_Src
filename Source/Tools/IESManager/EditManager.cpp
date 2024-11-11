#include "stdafx.h"

#include "imcFont.h"
#include "imcLanguage.h"
#include "imcInput.h"
#include "imcSound.h"
#include "imcSoundUtil.h"
#include "imcUiRender.h"

#include "imc3d.h"
#include "imc3dRenderer.h"

#include "imcEffect.h"
#include "imcEffectUtil.h"

#include "imcScene.h"
#include "imcSceneMgr.h"


#include "Dlg_EditFrame.h"
#include "Dlg_CreateParticle.h"
#include "Dlg_CreateLayer.h"
#include "PaneView.h"
#include "PaneTextureList.h"
#include "PaneEffectList.h"
#include "PaneProperty.h"
#include "EffectEditor.h"
#include "MainFrm.h"

#include "Editmanager.h"

namespace app
{

int							g_windowWidth		=	0;
int							g_windowHeight		=	0;
const int					g_framePerMakePtcl	=	1;

BOOL						g_isInit			=	FALSE;
BOOL						g_isShowUI			=	TRUE;	
BOOL						g_isCreateContinue	=	FALSE;
BOOL						g_isPause			=	FALSE;

COLORREF					g_clrBG				=	RGB(0,0,0);
ActiveMode					g_activeMode		=	AM_NONE;
BOOL						g_bShowGrid			=	TRUE;

imc::vector3				g_vPos(0, 0, 0);
imc::vector3				g_targetPos(0, 0, 1000);
float						g_rotationSpd		=	1.0f;
float						g_rotationRadius	=	100.0f;
float						g_rotationCurAngle	=	0.0f;
float						g_linearSpd		=	1.0f;
float						g_linearDist		=	500.0f;

imcEffect::ILayer*			g_pCopyLayer		=	NULL;
int							g_effectIndex		=	-1;
int							g_layerIndex		=	-1;

imcscene::ISceneNode*		g_pGridNode			=	NULL;
imcscene::ICameraNode*		g_pCameraNode		=	NULL;
imcscene::ISceneNode*		g_pCurEffectNode	=	NULL;

imcRenderer::IRenderer*		g_pRenderer			=	NULL;
imcd3d9::IDevice*			g_pDev				=	NULL;
imcscene::ISceneMgr*		g_SceneMgr			=	NULL;
camController				g_camController;

imc::CLinkedList<imcscene::ISceneNode*> g_effectNodes;

void UpdateParticle(float elapsedTime);
BOOL InitApp(HWND hWnd, int nWndWidth,int nWndHeight);
BOOL InitConfig();

void Text(int y, int x1, char*sz1, DWORD dwColor1, int x2, char*sz2, DWORD dwColor2)
{
	g_pDev->DrawText( x1, y, sz1, dwColor1 );
	g_pDev->DrawText( x2, y, sz2, dwColor2 );
}
BOOL IsInitialized()			{return g_isInit;}
void Toggle_ShowUI()			{g_isShowUI	= !g_isShowUI;}
void Pause(BOOL isPause)		{g_isPause	= isPause;}
void SetBGColor(COLORREF rgb)	{g_clrBG	= rgb;}

// @brief : 초기화 / 종료
BOOL Init(HWND hWnd, int wndWidth, int wndHeight)
{
	if ( FALSE == InitApp(hWnd, wndWidth, wndHeight) )
		return FALSE;	

	if ( FALSE == InitConfig() )		
		return FALSE;

	g_isInit = TRUE;
	
	return TRUE;
}

void Exit()
{	
	if (g_pGridNode) 
		imcscene::GetSceneMgr()->DestroySceneNode(g_pGridNode);

	if (g_pCopyLayer)
		SAFE_DELETE(g_pCopyLayer);	

	imcscene::DestroySceneMgr( g_SceneMgr );
	imcscene::Exit(); 
		
	imcUiRender::Exit();
	imcFont::Exit();
	imcinput::Exit();
	imclang::Exit();
	imcSound::Exit();	

	imcresmgr::Exit();
	imc3d::Exit();

	imcd3d9::Exit();	
	imccore::Exit();	
	imc::ExitTimer();
}

BOOL InitApp(HWND hWnd, int wndWidth, int wndHeight)
{	
	g_windowWidth	= wndWidth;
	g_windowHeight	= wndHeight;

	int screenWidth		= GetSystemMetrics(SM_CXSCREEN);
	int screenheight	= GetSystemMetrics(SM_CYSCREEN);       
	
	imc::InitTimer();
	imc::ResetTimer();
	imc::RandomSeed(imc::GetAppTime());

	imccore::Init("LoggerConfig_EffectEditor.xml");
	imcIES::Init();
	imcresmgr::Init();

	imcfile::SetUseCodeBasePath(FALSE);

	if ( imcd3d9::Init( hWnd, TRUE, g_windowWidth, g_windowHeight, 32 ) == FALSE ) 
	{
		imcd3d9::DEVICEERRORCODE errCode = imcd3d9::GetDevice()->GetErrorCode();
		return FALSE;
	}

	g_pDev = imcd3d9::GetDevice();

	if ( imc3d::Init(g_pDev)  == FALSE )
	{
		IMC_ASSERT(0);
		return FALSE;	
	}

	imc3dRenderer::GetRenderer()->RegisterRenderGroup(0);
	imc3dRenderer::GetRenderer()->SetRenderMode(imc3dRenderer::MRT_DEFAULT);

	std::string effectPath = imc::GetDataPath("effect\\", "");

	imcEffect::Init(effectPath.c_str(), TRUE);
	imcEffect::LoadData();
		
	imcscene::Init();
	g_SceneMgr		= imcscene::CreateSceneMgr();
	g_pCameraNode	= imcscene::GetSceneMgr()->GetActiveCamera();

	std::string rootPath	= imc::GetDataRootPath();
	std::string binPath		= imc::GetBinPath();
	std::string musicPath	= rootPath + "bgm\\";
	std::string soundPath	= rootPath + "se\\";

	imcinput::Init(hWnd, hWnd);
	
	imcSound::Init();
	imcSound::Path::SetMusicPath(musicPath.c_str());
	imcSound::Path::SetSoundPath(soundPath.c_str());
	imcSound::SetEnableReverb(TRUE);

	std::string soundListPath = rootPath + "xml_client\\datatable_soundlist.xml";

	imcSound::LoadSoundListFromXML(soundListPath.c_str(), TRUE);
	imcSound::AddListener("CAMERA");
	imcSound::AddListener("PC");

	imclang::Init();

	if ( imcUiRender::Init() == FALSE)
		return FALSE;

	std::string fontPath	= imc::GetDataRootPath();
	fontPath += "font\\";

	if ( imcFont::Init( fontPath.c_str() ) == FALSE)
		return FALSE;

	return TRUE;
}

BOOL InitConfig()
{
	imcd3d9::IDevice * pDev = imcd3d9::GetDevice();
	assert( NULL != pDev );

	// @brief : 렌더링 초기화 및 설정 	
	imcRenderer::Config::EnableOption(imcRenderer::RTYPE_GLOW, TRUE);
	imcRenderer::Config::EnableOption(imcRenderer::RTYPE_FOG, TRUE);
	imcRenderer::Config::EnableOption(imcRenderer::RTYPE_WATER, FALSE);
	imcscene::Config::EnableDebugging(TRUE);
	
	imcRenderer::IRenderer * pRenderer = NULL;
	if (imcscene::Util::QueryInterface(&pRenderer) == FALSE) // <= New : imcRenderer::IRenderer 인터페이스를 얻는다.
		return FALSE;
	
	g_pRenderer = imcscene::Renderer::GetRenderer();
	g_pRenderer->SetGlowBGColor(0xff888888);

	pDev->SetFogParm(0xFF7f8f4f, 200.0f, 10000.0f);
	pDev->SetViewport( 0, 0, g_windowWidth, g_windowHeight );
	g_pCameraNode->GetCamera()->Perspective( imc::DegToRad(45.0f), 0.9f, 25000.0f, float(g_windowWidth), float(g_windowHeight) );

	g_camController.Init();
	ApplyCamera();

	// NOTE : 그리드 설정
	imcscene::ISceneEntity * pNewShapeEntity = imcscene::CreateSceneEntity(imcscene::ENTITY_SHAPE, "TestShapeEntity");
	if (pNewShapeEntity != NULL)
	{
		pNewShapeEntity->GetAttributeList()->SetAttribute("ShapeType", imcscene::STYPE_GRID);

		g_pGridNode = imcscene::GetSceneMgr()->CreateSceneNode(pNewShapeEntity, "ShapeNode");
		g_pGridNode->GetController()->SetPos(imc::vector3(0.0f, 0.0f, 0.0f));
		g_pGridNode->Bind();

		imcscene::GetSceneMgr()->GetRootNode()->AddChild(g_pGridNode);	
	}

	return TRUE;
}

// @brief : 펄스

void UpdateEffectNode(float elapsedTime)
{
	if (g_pCurEffectNode)
	{
		imcscene::IController* pController = g_pCurEffectNode->GetController();

		//  모드에 따른 파티클 트랜스폼
		imc::quat4 quat;
		imc::matrix44 worldTM;
		imc::matrix44 worldTM2;
		imc::vector3  vStart(g_fRotationRadius, 0, 0);
		D3DXMatrixIdentity(&worldTM);
		D3DXMatrixIdentity(&worldTM2);

		switch (g_activeMode)
		{
		case AM_NONE:
			g_vPos.x	= 0.0f;
			g_vPos.y	= 0.0f;
			g_vPos.z	= 0.0f;
			g_fRotationCurAngle = 0.0f;
			break;

		case AM_LINEAR:
			g_vPos.z += g_fLinearSpd;
			if (g_vPos.z >= g_fLinearDist || g_vPos.z <= -g_fLinearDist)
				g_vPos.z = 0;
			break;

		case AM_CIRCUIT:
			g_fRotationCurAngle += g_fRotationSpd;

			if (g_fRotationCurAngle >= 360)
				g_fRotationCurAngle = 0;

			g_vPos = vStart;
			D3DXMatrixTranslation(&worldTM, g_vPos.x, g_vPos.y, g_vPos.z);	
			D3DXMatrixRotationY(&worldTM2, imc::DegToRad(g_fRotationCurAngle));
			D3DXMatrixMultiply(&worldTM, &worldTM, &worldTM2);
			g_vPos.x	=	worldTM._41;
			g_vPos.y	=	worldTM._42;
			g_vPos.z	=	worldTM._43;
			break;

		case AM_ROTATION:
			g_fRotationCurAngle += g_fRotationSpd;

			if (g_fRotationCurAngle >= 360)
				g_fRotationCurAngle = 0;

			D3DXMatrixRotationY(&worldTM, imc::DegToRad(g_fRotationCurAngle));
			break;

		case AM_SPLINE:
			if (g_vPos == g_vTarget)
			{
				g_vPos.x = 0;
				g_vPos.y = 0;
				g_vPos.z = 0;
			}
			else
			{
				float deltaX = g_vTarget.x - g_vPos.x;
				float deltaY = g_vTarget.y - g_vPos.y;
				float deltaZ = g_vTarget.z - g_vPos.z;

				//imc::vector3 vSpd(deltaX, deltaY, deltaZ);
				imc::vector3 vSpd(0, 0, 1);

				imc::Vec3Normalize(&vSpd, &vSpd);

				vSpd *= (g_fLinearSpd*10);

				float splineRate = g_vPos.z/g_vTarget.z;

				float offsetX  = sin((splineRate * 360) * 3.14f/180) * 200;					
				g_vPos += vSpd;
				g_vPos.x = offsetX;

				if (deltaZ <= 0)
					g_vPos = g_vTarget;
			}
			break;
		}

		D3DXVECTOR3 vUp(0,1,0);
		D3DXQuaternionRotationAxis(&quat, &vUp, imc::DegToRad(g_fRotationCurAngle));

		pController->SetPos(g_vPos);
		pController->SetRot(quat);
	}
}

void Update(float elapsedTime)
{
	Sleep(1);

	if (g_pGridNode)
		g_pGridNode->Update(elapsedTime);

	UpdateEffectNode(elapsedTime);

	UpdateParticle(elapsedTime);
}

void Render( float elapsedTime )
{
	imcscene::GetSceneMgr()->RenderAll(elapsedTime);
}

void Run()
{
	if (g_isPause == FALSE) 
	{
		float elapsedTime = imc::GetElapsedTime();

		Update(elapsedTime);
		Render(elapsedTime);
	}
}

// @brief : 카메라
void ApplyCamera()
{
	float latitude	= g_camController.latitude;
	float longitude = g_camController.longitude;
	imc::EarthToCart( &g_camController.atPos, 10.0f, imc::DegToRad( g_camController.latitude ), imc::DegToRad( g_camController.longitude ) );

 	g_camController.atPos += g_camController.eyePos;

	g_pCameraNode->SetAtPos(g_camController.atPos);
	g_pCameraNode->SetEyePos(g_camController.eyePos);
}

void FowardCamera(float distance)
{
	imc::vector3 vMove;
	imc::vector3 direction	= g_pCameraNode->GetDir();
	imc::Vec3Normalize(&vMove, &direction);

	vMove *= distance;
	g_camController.eyePos	+= vMove;
	g_camController.atPos	+= vMove;

	g_camController.distance += distance;

	g_pCameraNode->SetAtPos(g_camController.atPos);
	g_pCameraNode->SetEyePos(g_camController.eyePos);
}

imcd3d9::Camera* GetCamera()			
{
	return g_pCameraNode->GetCamera();
}

camController* GetCamCtrl()		
{
	return &g_camController;
}

void PrevEffect()
{
	if (g_effectIndex > 0)
		g_effectIndex--;
}

void NextEffect()
{	
	if (g_effectIndex < imcEffect::GetEffectCount() - 1) 
		g_effectIndex++;
}

int	GetEffectIndex()			
{
	return g_effectIndex;
}

int	GetLayerIndex()				
{
	return g_layerIndex;
}

void SetEffect(int effectIndex)		
{
	g_effectIndex = effectIndex;
}

void SetEffect(const char* effectName)
{
	if (effectName != NULL) 
	{
		g_effectIndex = -1;
		for (int i = 0; i < imcEffect::GetEffectCount(); ++i)
		{
			imcEffect::IEffect* pEffect = imcEffect::GetEffect(i);
			if (strcmp(pEffect->GetName(), effectName))
				continue;

			g_effectIndex	=	i;
			break;
		}
	}
}

void SetLayer(int layerIndex)		
{
	g_layerIndex = layerIndex;
}

void SetLayer(const char* layerName)
{
	imcEffect::IEffect* pEffect = imcEffect::GetEffect(g_layerIndex);
	
	if (pEffect != NULL)
	{
		int layerCount = pEffect->GetLayerCount();
		for (int i = 0; i < layerCount; i++)
		{
			imcEffect::ILayer* pLayer = pEffect->GetLayer(i);
			if (strcmp(layerName, pLayer->GetName()))
				continue;

			g_layerIndex	=	i;
			break;
		}
	}
}

// @brief : 파일 세이브 / 로드
BOOL SaveEffect_Xml()
{
	//return imcEffect::Util::SaveParticle_XML("test_TextureInfo.Xml","test_Effect.Xml");
	return FALSE;
}

BOOL SaveEffect_Bin()
{
	//return imcEffect::Util::SaveParticle_Bin("test_Effect.bin");
	return FALSE;
}

BOOL LoadEffect_Bin()
{
	//return imcEffect::LoadParticle_Bin("test_Effect.bin");
	return FALSE;
}

// @brief : Grid
BOOL GridToggle()
{
	g_bShowGrid = !g_bShowGrid;

	if (!g_bShowGrid)	imcscene::GetSceneMgr()->GetRootNode()->RemoveChild(g_pGridNode);
	else				imcscene::GetSceneMgr()->GetRootNode()->AddChild(g_pGridNode);

	return g_bShowGrid;
}

// @brief : 파티클 동작 설정
void SetMode(ActiveMode mode)
{
	if (g_activeMode != mode) 
	{
		g_activeMode	= mode;
		g_vPos.x		= 0;
		g_vPos.y		= 0;
		g_vPos.z		= 0;
		g_fRotationCurAngle	=	0.0f;
	}
	else
	{
		g_activeMode	= AM_NONE;
		g_vPos.x		= 0;
		g_vPos.y		= 0;
		g_vPos.z		= 0;
		g_fRotationCurAngle	=	0.0f;
	}
}

void AddSpd(float fSpd)
{
	switch (g_activeMode)
	{
		case AM_LINEAR:		
			g_fLinearSpd += fSpd;		
			break;

		case AM_CIRCUIT:	
		case AM_ROTATION:	
			g_fRotationSpd += fSpd * 0.1f;
			break;
	}
}

void AddRadius(float fRadius)
{
	switch (g_activeMode)
	{
		case AM_LINEAR:
			g_fLinearDist += fRadius;		
			break;

		case AM_CIRCUIT:
		case AM_ROTATION:
			g_fRotationRadius += fRadius;		
			break;
	}
}

// @brief : 레이어 복사/붙여넣기
imcEffect::ILayer* CreateNewLayer(const char* layerName)
{
	imcEffect::ILayer * pLayer	= NULL;

	/*
	switch (type)
	{
		case  imcEffect::PT_2D:
		{
			imcEffect::ILayer * pParticleLayer	= new imcEffect::CParticleLayer;		
			pNewInfo = pParticleLayer;	

			pParticleLayer->SetSize(100, 100);
			pParticleLayer->CalcTexelCoord();
		}
		break;

		case  imcEffect::PT_3D:
		{
			imcEffect::ILayer * pModelLayer = new imcEffect::CModelLayer;		
			pNewInfo = pModelLayer;
		}
		break;

		case  imcEffect::PT_SOUND:
			break;
	}

	// @brief : 디폴트
	int count = 1;
	pNewInfo->ParticleCount.AddKey(0, count);
	pNewInfo->GenLookupTable(64);
	*/

	return pLayer;
}

void _CopyLayer(imcEffect::ILayer** pPtclDest, imcEffect::ILayer* pPtclSrc)
{
	/*
	switch (pPtclSrc->GetType())
	{
		case imcEffect::PT_2D:
		{
			imcEffect::CParticleLayer* pDest = (imcEffect::CParticleLayer*)CreateNewLayer();
			imcEffect::CParticleLayer* pSrc =  (imcEffect::CParticleLayer*)pPtclSrc;

			*pDest = *pSrc;

			pDest->m_pTexAnim = NULL;
			imcEffect::CTexAnimation* pTexAnim = pSrc->m_pTexAnim;

			if (pTexAnim)
				pDest->CreateTexAnim(pTexAnim->XCnt, pTexAnim->YCnt, pTexAnim->Fps, pTexAnim->Loop);

			*pPtclDest = pDest;
		}
		break;

		case imcEffect::PT_3D:
		{
			imcEffect::CModelLayer* pDest = (imcEffect::CModelLayer*)CreateNewLayer();
			imcEffect::CModelLayer* pSrc =  (imcEffect::CModelLayer*)pPtclSrc;
			*pDest		= *pSrc;	
			*pPtclDest	= pDest;
		}
		break;

		case imcEffect::PT_SOUND:
		break;
	}
	*/
}

void CopyLayer(imcEffect::ILayer* pLayer)
{
	if (pLayer != NULL) 
	{		
		SAFE_DELETE(g_pCopyLayer);

		_CopyLayer(&g_pCopyLayer, pLayer);
	}
}

imcEffect::ILayer* PasteLayer()
{
	if (g_pCopyLayer != NULL)
	{
		imcEffect::ILayer* pCloneLayer;
		_CopyLayer(&pCloneLayer, g_pCopyLayer);

		return pCloneLayer;
	}

	return NULL;
}

// @brief : 모든 파티클 동작 정지 
void StopAllParticle()
{
	if (NULL != g_pCurEffectNode) 
	{
		imcscene::GetSceneMgr()->DestroySceneNode(g_pCurEffectNode);
		g_pCurEffectNode = NULL;
	}

	int index = g_effectNodes.Head();

	while(1) 
	{
		if (index == g_effectNodes.InvalidIndex())
			break;

		imcscene::ISceneNode* pEffectNode = g_effectNodes.Element(index);		
		imcscene::GetSceneMgr()->DestroySceneNode(pEffectNode);
		index = g_effectNodes.Next(index);
	}

	g_effectNodes.RemoveAll();
}

void MoveEffect(imc::vector3& pos)
{
	static float time		= 0;
	time += 0.05f;
	float radius = 25.0f;
	float x = radius * sinf(time);
	float y = radius * cosf(time);
	pos.x += x;
	pos.y += y;
}

// @brief : 파티클 진행 
void UpdateParticle(float elapsedTime)
{
	imcscene::ISceneNode* pCameraSceneNode = imcscene::GetCameraSceneNode(g_pCameraNode);
	pCameraSceneNode->Update(elapsedTime);
	imcd3d9::Camera* pCamera = g_pCameraNode->GetCamera();
	imcscene::GetSceneMgr()->ProcessScene(elapsedTime, pCamera);
	
	imcEffect::Process(elapsedTime);

	if (g_pCurEffectNode)
		g_pCurEffectNode->Update(elapsedTime);

	int index = g_effectNodes.Head();

	while(1) 
	{
		if (index == g_effectNodes.InvalidIndex())
			break;

		imcscene::ISceneNode* pEffectNode = g_effectNodes.Element(index);
		if (pEffectNode != NULL)
		{
			imcscene::IController* pController = pEffectNode->GetController();
			imc::vector3 pos = pController->GetPos();

			MoveEffect(pos);
			pEffectNode->Update(elapsedTime);
		}
		index = g_effectNodes.Next(index);
	}
}

// @brief : 선택된 파티클 출력 시작
void Play()
{
	if (g_effectIndex < 0)
		return;

	StopAllParticle();

	g_vPos.x			= 0;
	g_vPos.y			= 0;
	g_vPos.z			= 0;	
	g_fRotationCurAngle	= 0.0f;
	
	imcEffect::IEffect* pEffect = imcEffect::GetEffect(g_effectIndex);

	if (pEffect != NULL)
	{		
		std::string	ptclName = pEffect->GetName();	

		imcscene::ISceneEntity* pSceneEntity = imcscene::CreateSceneEntity(imcscene::ENTITY_EFFECT);

		if (pSceneEntity != NULL)
		{
			pSceneEntity->SetMeshName(imcscene::ISceneEntity::E_M0, ptclName.c_str());
			pSceneEntity->GetAttributeList()->SetAttribute("EffectType", imcscene::ETYPE_BASE);	
			g_pCurEffectNode = imcscene::GetSceneMgr()->CreateSceneNode(pSceneEntity);	

			if (g_pCurEffectNode == NULL)
				return;

			g_pCurEffectNode->Bind();

			imcscene::GetSceneMgr()->GetRootNode()->AddChild(g_pCurEffectNode);
		}
	}
}

} // end of Namespace 'app'