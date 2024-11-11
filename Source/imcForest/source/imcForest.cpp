#include "stdafx.h"

#include "imcForest.h"
#include "imcFile.h"

using namespace SpeedTree;

namespace	imcForest {

const int							c_maxNumVisibleCells = 75;
const int							c_maxNumInstancesPerCell = 60;

static CForestRender				s_forestRender;
static SForestRenderInfo			s_forestRenderInfo;

static Vec4							s_lightAmbient;
static Vec4							s_lightSpecular;
static float						s_sunlightIntensity = 1.0f;
static float						s_specularIntensity = 1.0f;

static st_bool						s_enableFog = true;
static float						s_fogStartDistance = 0.0f;
static float						s_fogEndDistance = 0.0f;
static Vec3							s_fogColor = Vec3(1.0f, 1.0f, 1.0f);

static st_float32					s_currentTime = 0.0f;
static st_float32					s_frameTime = 0.0f;
static st_float32					s_timeMarker = 0.0f;
static CTimer						s_timer;

static CView						s_view;
static SForestCullResultsRender		s_visibleTrees;
static st_bool						s_cameraChanged = true;

static st_bool						s_readyToRender = false;
static st_bool						s_isTreeExist = false;

static ETextureAlphaRenderMode		s_textureAlphaRenderMode = TRANS_TEXTURE_ALPHA_TO_COVERAGE;

BOOL Init( const FOREST_INIT& init )
{
	IMC_ASSERT(init.pD3DDev);

	// example call to Authorize() activate an evaluation or lease license; the SDK will
	// also check the registry and use whichever of the two keys expires last
	CCore::Authorize("5F3C23977A98A38E");

	// check for authorized SDK before proceeding (failure is fatal)
	if (!CCore::IsAuthorized()) {
		return FALSE;
	}

	CCoordSys::SetCoordSys(CCoordSys::COORD_SYS_LEFT_HANDED_Y_UP);
	CCore::SetTextureFlip(true);

	if (init.pShaderPath) {
		s_forestRenderInfo.m_strShaderPath = init.pShaderPath;
	}
	s_forestRenderInfo.m_bSpecularLighting = true;
	s_forestRenderInfo.m_nNumShadowMaps = 0;
	s_forestRenderInfo.m_nShadowMapResolution = 0;
	s_forestRender.SetRenderInfo(s_forestRenderInfo);

	// set transparent texture render mode based on multisampling
	s_forestRender.TransparentTextureModeNotify(s_textureAlphaRenderMode);

	// set forest cell size
	s_forestRender.SetCullCellSize(1200.0f);

	// lighting
	s_forestRender.SetLightDir(Vec3(0.0f, 0.0f, 1.0f));


	// init graphics
	SpeedTree::DX9::SetDevice(init.pD3DDev);

	s_readyToRender = false;

	CRenderState::Initialize();

	const st_bool c_bUsePrecompiledShaders = init.usePrecompliedShaders; // works with DX9, 360 and PS3 platforms; not OpenGL or DX10
	if (s_forestRender.InitGraphics(c_bUsePrecompiledShaders)) {
		s_readyToRender = true;
	}

	// max number of cells in forest frustum (used for billboard system)
	s_forestRender.SetHint(CForest::HINT_MAX_NUM_VISIBLE_CELLS, c_maxNumVisibleCells);

	CCore::SetClipSpaceDepthRange(0.0f, 1.0f);

	if (s_readyToRender) {
		const TTreeArray& aBaseTrees = s_forestRender.GetBaseTrees();
		if (!aBaseTrees.empty()) {
			s_forestRender.SetWindLeader(&aBaseTrees[0]->GetWind());
		}
		s_forestRender.SetGlobalWindStrength(0.25f);
	}

	// IMC_DELETE temporary heap blocks used for initialization; if all blocks are cleared here, 
	// it may result it a couple of heap allocations during the first few frame renders, so we
	// leave a few smaller blocks ready to use
	CCore::DeleteAllTmpHeapBlocks(1024 * 256); // IMC_DELETE any blocks larger than 256 KB

	s_timer.Start();

	return s_readyToRender;
}

BOOL IsInitialize()
{
	return s_readyToRender;
}

BOOL IsTreeExist()
{
	return s_isTreeExist;
}

void Render()
{
	if (!s_visibleTrees.m_pBaseTrees) {
		return;
	}

	// set render states
	CRenderState::SetAlphaFunction(ALPHAFUNC_GREATER, 0.0f);
	// turn all modes off (no telling what render state the client application might be in before this call)
	CRenderState::SetBlending(false);
	CRenderState::SetAlphaTesting(false);
	CRenderState::SetAlphaToCoverage(false);
	switch (s_textureAlphaRenderMode)
	{
	case TRANS_TEXTURE_ALPHA_TESTING:
		CRenderState::SetAlphaTesting(true);
		break;
	case TRANS_TEXTURE_ALPHA_TO_COVERAGE:
		CRenderState::SetAlphaToCoverage(true);
		break;
	case TRANS_TEXTURE_BLENDING:
		CRenderState::SetBlending(true);
		break;
	default:
		// intentionally do nothing (TRANS_TEXTURE_NOTHING)
		break;
	}
	CRenderState::SetRenderStyle(RENDERSTYLE_SOLID);

	// start the forest render
	s_forestRender.StartRender();
	{
		// set the shader view parameters for the main view
		CCoordSys::SetCoordSys(CCoordSys::COORD_SYS_RIGHT_HANDED_Y_UP);
		s_forestRender.UploadViewShaderParameters(s_view);
		s_forestRender.RenderBranches(s_visibleTrees, RENDER_PASS_STANDARD);
		s_forestRender.RenderFronds(s_visibleTrees, RENDER_PASS_STANDARD);
		s_forestRender.RenderLeafMeshes(s_visibleTrees, RENDER_PASS_STANDARD);
		s_forestRender.RenderLeafCards(s_visibleTrees, RENDER_PASS_STANDARD, s_view);
		s_forestRender.RenderBillboards(s_visibleTrees, RENDER_PASS_STANDARD, s_view);		
		CCoordSys::SetCoordSys(CCoordSys::COORD_SYS_LEFT_HANDED_Y_UP);
	}
	s_forestRender.EndRender();

	// frame's over; update a few things
	s_cameraChanged = false;
}

void Update( const imc::vector3& cameraPos, const imc::matrix44& viewTM, const imc::matrix44& projTM, float cameraNear, float cameraFar )
{
	// update timer data
	s_timer.Stop();
	s_currentTime = s_timer.GetSec();
	s_frameTime = s_currentTime - s_timeMarker;
	s_timeMarker = s_currentTime;

	// updating the view is an important step because the forest population or positions
	// may have changed (normally happens if the SDK is embedded in a world-building app)
	st_bool viewChanged = s_view.Set(Vec3(cameraPos), Mat4x4(projTM), Mat4x4(viewTM), cameraNear, cameraFar);
	s_cameraChanged |= viewChanged;

	// advance wind
	s_forestRender.SetGlobalTime(s_currentTime);
	s_forestRender.AdvanceGlobalWind();


	// cull
	if (!s_cameraChanged && !s_forestRender.LightDirChanged()) {
		return;
	}

	// statistics gathering
	CTimer timer;
	timer.Start();
	SRenderStats& renderStats = s_forestRender.GetRenderStats();

	// forest culling
	s_forestRender.CullAndComputeLOD(s_view, s_visibleTrees, true);

	// records the total time to cull the forest, terrain, and grass, as well as populate
	// VB/IB buffers with billboards, terrain, and grass; normally stable but can cause
	// CPU spikes during heavy population events (e.g. where IMC_NEW grass, terrain, and billboard
	// buffer all need repopulation)
	timer.Stop();
	renderStats.m_fCullAndPopulateTime += timer.GetMicroSec();
}

void CreateTree( const char* fileName, const FOREST_TREE_INSTANCE& treeInstance )
{
	CTreeRender* pTree = NULL;
	const TTreeArray& baseTrees = s_forestRender.GetBaseTrees();
	for (st_int32 i = 0; i < st_int32(baseTrees.size()); i++ ) {
		if (_strcmpi(fileName, baseTrees[i]->GetFilename()) == 0) {
			pTree = (CTreeRender*)baseTrees[i];
			break;
		}
	}

	if (pTree == NULL) {
		pTree = (CTreeRender*)s_forestRender.AllocateTree();
		imcfile::IFileMem* pFileMem = imcfile::Load(fileName);
		if (pFileMem) {
			if (pTree->LoadTree((st_byte*)pFileMem->GetData(), pFileMem->GetSize(), true, 1.0f)) {
				s_forestRender.RegisterTree(pTree);
				s_forestRender.InitTreeGraphics(pTree, c_maxNumInstancesPerCell, false);
			}
		}
		imcfile::Close(pFileMem);
	}

	CInstance instance;
	instance.SetPos(Vec3(treeInstance.pos));
	instance.SetScale(treeInstance.scale);
	instance.SetRotation(treeInstance.radian);
	s_forestRender.AddInstances(pTree, &instance, 1);
	s_forestRender.UpdateTreeCellExtents();
	s_isTreeExist = true;
}

void ClearTrees()
{
	const TTreeArray& baseTrees = s_forestRender.GetBaseTrees();
	while (baseTrees.size() > 0) {
		CTree* pTree = baseTrees[0];
		if (s_forestRender.TreeIsRegistered(pTree)) {
			s_forestRender.UnregisterTree(pTree);
			st_delete<CTree>(pTree);
		}
	}
	s_forestRender.UpdateTreeCellExtents();
	s_isTreeExist = false;
}

void SetLightDir( const imc::vector3& dir )
{
	s_forestRender.SetLightDir(Vec3(dir));
}

void SetLightDiffuse( const imc::vector4& diffuse )
{
	s_forestRenderInfo.m_sLightMaterial.m_vDiffuse = Vec4(diffuse);
	s_forestRender.SetRenderInfo(s_forestRenderInfo);
}

void SetLightAmbient( const imc::vector4& ambient )
{
	s_lightAmbient = Vec4(ambient);
	s_forestRenderInfo.m_sLightMaterial.m_vAmbient = s_lightAmbient * s_sunlightIntensity;
	s_forestRender.SetRenderInfo(s_forestRenderInfo);
}

void SetLightSpecular( const imc::vector4& specular )
{
	s_lightSpecular = Vec4(specular);
	s_forestRenderInfo.m_sLightMaterial.m_vSpecular = s_lightSpecular * s_specularIntensity;
	s_forestRender.SetRenderInfo(s_forestRenderInfo);
}

void SetLightIntensity( float sunlightIntensity, float specularIntensity )
{
	s_sunlightIntensity = sunlightIntensity;
	s_specularIntensity = specularIntensity;
	s_forestRenderInfo.m_fGlobalLightScalar = s_sunlightIntensity;
	s_forestRenderInfo.m_sLightMaterial.m_vAmbient = s_lightAmbient * s_sunlightIntensity;
	s_forestRenderInfo.m_sLightMaterial.m_vSpecular = s_lightSpecular * s_specularIntensity;
	s_forestRender.SetRenderInfo(s_forestRenderInfo);

	EnableFog(s_enableFog);
}

void EnableWind( BOOL enable )
{
	if (enable) {
		s_forestRender.EnableWind(true);
	} else {
		s_forestRender.EnableWind(false);
	}
}

void EnableFog( BOOL enable )
{
	if (enable) {
		s_enableFog = true;
		s_forestRenderInfo.m_fFogStartDistance = s_fogStartDistance;
		s_forestRenderInfo.m_fFogEndDistance = s_fogEndDistance;
		s_forestRenderInfo.m_vFogColor = s_fogColor * s_sunlightIntensity;
	} else {
		s_enableFog = false;
		s_forestRenderInfo.m_fFogStartDistance = FLT_MAX;
		s_forestRenderInfo.m_fFogEndDistance = FLT_MAX;
	}
	s_forestRender.SetRenderInfo(s_forestRenderInfo);
}

void SetFog( float fogStart, float fogEnd, const imc::color& fogColor )
{
	s_fogStartDistance = fogStart;
	s_fogEndDistance = fogEnd;
	s_fogColor = Vec3(fogColor.r, fogColor.g, fogColor.b);
	if (s_enableFog) {
		s_forestRenderInfo.m_fFogStartDistance = s_fogStartDistance;
		s_forestRenderInfo.m_fFogEndDistance = s_fogEndDistance;
	}
	s_forestRenderInfo.m_vFogColor = s_fogColor * s_sunlightIntensity;
	s_forestRender.SetRenderInfo(s_forestRenderInfo);
}

void SetWindDir( const imc::vector3& dir )
{
	s_forestRender.SetGlobalWindDirection(Vec3(dir));
}

void SetWindStrength( float strength )
{
	s_forestRender.SetGlobalWindStrength(strength);
}

}
