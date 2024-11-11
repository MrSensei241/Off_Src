#include "stdafx.h"

#include "imcd3d9.h"

#include "imc3d.h"
#include "imc3dMotion.h"

#include "imcEffect.h"
#include "imcEffectUtil.h"

#include "App.h"
#include "AppObject.h"

#include "PaneView.h"
#include "PaneEffectList.h"
#include "PaneEmitterList.h"
#include "MainFrm.h"

namespace app
{

extern imcEffect::ITrailInstance*	s_pCurTrailInst;
extern CDummyObject*				s_pActionDummy;
extern float						s_processSpeed;

static imc3d::IModel*				s_pTrailDummyModel		= NULL;
static imc3d::ModelInfo*			s_pTrailDummyModelInfo	= NULL;
static imc3d::MotionInfo*			s_pTrailDummyMotionInfo	= NULL;

static std::string					s_trailSampleModelFileName	= "test_trail01.xac";
static std::string					s_trailSampleMotionFileName	= "None"; 
static std::string					s_trailKeyFrameSaveFileName	= "None"; 

static std::string					s_selTrailKeyFrameName	= "None";
static std::string					s_selTrailImageName	= "None";
static std::string					s_selTrailActionName	= "None";

void SelectTrailKeyFrameName(const char* name)
{
	if (name != NULL)
		s_selTrailKeyFrameName = name;
}

const char* GetSelectTrailKeyFrameName()
{
	return s_selTrailKeyFrameName.c_str();
}

void	SelectTrailImageName(const char* name)
{
	if (name != NULL)
		s_selTrailImageName = name;
}

const char* GetSelectTrailImageName()
{
	return s_selTrailImageName.c_str();
}

imcEffect::TRAIL_IMAGE*	GetSelectTrailImage()
{
	return imcEffect::GetTrailImage(s_selTrailImageName.c_str());
}

void	SelectTrailActionName(const char* name)
{
	if (name != NULL)
		s_selTrailActionName = name;
}

const char* GetSelectTrailActionName()
{
	return s_selTrailActionName.c_str();
}

imcEffect::TRAIL_ACTION* GetSelectTrailAction()
{
	return imcEffect::GetTrailAction(s_selTrailActionName.c_str());
}

void SetTrailSampleMotionFileName(const char* fileName)
{
	s_trailSampleMotionFileName = fileName;
}

const char* GetTrailSampleMotionFileName()
{
	return s_trailSampleMotionFileName.c_str();
}

void SetTrailKeyFrameSaveFileName(const char* fileName)
{
	s_trailKeyFrameSaveFileName = fileName;
}

void DeselectTrail()
{	
	s_selTrailKeyFrameName	= "None";
}

BOOL PlaySampleTrail()
{
	if (s_pCurTrailInst != NULL)
	{
		imcEffect::ReleaseTrailInst(s_pCurTrailInst);
		s_pCurTrailInst = NULL;
	}

	s_pCurTrailInst = imcEffect::CreateTrail("TrailTest");
	if (s_pCurTrailInst != NULL)
	{
		imcEffect::TRAIL_IMAGE* pTrailImage	= imcEffect::GetTrailImage(s_selTrailImageName.c_str());
		imcEffect::TRAIL_ACTION* pTrailAction	= imcEffect::GetTrailAction(s_selTrailActionName.c_str());

		if (pTrailAction != NULL && pTrailImage != NULL)
		{			
			s_pCurTrailInst->SetSpeed(s_processSpeed);
			s_pCurTrailInst->SetTrailInfo(pTrailImage->imageName.c_str(), pTrailAction->actionName.c_str());

			// NOTE :  샘플 사용
 			if (s_trailSampleMotionFileName != "None")
 			{
// 				// NOTE : 더미 생성				
// 				if (s_pTrailDummyModelInfo == NULL)
// 					s_pTrailDummyModelInfo = new imc3d::ModelInfo;
// 
// 				if (s_pTrailDummyMotionInfo == NULL)
// 					s_pTrailDummyMotionInfo = new imc3d::MotionInfo;
// 
// 				std::string modelFullPath = imcEffect::GetTrailSamplePath();
// 				modelFullPath += s_trailSampleModelFileName;
// 
// 				std::string motionFullPath = imcEffect::GetTrailSamplePath();		
// 				motionFullPath	+= s_trailSampleMotionFileName;
// 
// 				s_pTrailDummyModelInfo->SetMeshName(modelFullPath.c_str());			
// 				s_pTrailDummyMotionInfo->fullFileName		= motionFullPath;
// 				s_pTrailDummyMotionInfo->isRequiredRootCalc	= FALSE;
// 
// 				// NOTE : 기존 더미 삭제
// 				if (s_pTrailDummyModel != NULL)
// 				{			
// 					imc3d::DestroyModel(s_pTrailDummyModel);
// 					s_pTrailDummyModel = NULL;
// 				}
// 
// 				s_pTrailDummyModel = imc3d::CreateModelEx(*s_pTrailDummyModelInfo);
// 				if (s_pTrailDummyModel != NULL)
// 				{
// 					//s_pTrailDummyModel->GetMaterial()->SetUseThread(FALSE);
// 					s_pTrailDummyModel->Bind();
// 
// 					if (s_pTrailDummyMotionInfo != NULL)
// 					{
// 						imc3d::MotionPlayInfo motionPlayInfo;
// 						motionPlayInfo.blendInTime				= 0.0f;
// 						motionPlayInfo.blendOutTime				= 0.0f;	
// 						motionPlayInfo.numLoop					= 1;
// 
// 						imc3d::IMotionInst*	pMotionInst			= s_pTrailDummyModel->PlayMotion(*s_pTrailDummyMotionInfo, motionPlayInfo);
// 						pMotionInst->SetSpeed(s_processSpeed);
// 
// 						int nodeCount = s_pTrailDummyModel->GetNumNode();
// 						if (nodeCount == 2)
// 						{
// 							const char* nodeName1	= s_pTrailDummyModel->GetNodeName(0);
// 							imc3d::INode* pNode1	= s_pTrailDummyModel->GetNodeByName(nodeName1);
// 
// 							const char* nodeName2	= s_pTrailDummyModel->GetNodeName(1);
// 							imc3d::INode* pNode2	= s_pTrailDummyModel->GetNodeByName(nodeName2);
// 
// 							s_pCurTrailInst->SetNode(pNode1, pNode2);
// 							s_pCurTrailInst->Start();
// 							s_pCurTrailInst->Play(0.0f); // NOTE : 더미가 끝날떄까지 진행
// 						}
// 					}
// 				}

				SAFE_DELETE(s_pTrailDummyModelInfo);
				SAFE_DELETE(s_pTrailDummyMotionInfo);
			}
					
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PlayKeyFrameTrail()
{	
	if (s_pCurTrailInst != NULL)
	{
		imcEffect::ReleaseTrailInst(s_pCurTrailInst);
		s_pCurTrailInst = NULL;
	}

	s_pCurTrailInst = imcEffect::CreateTrail("TrailTest");
	if (s_pCurTrailInst != NULL)
	{
		imcEffect::TRAIL_IMAGE* pTrailImage	= imcEffect::GetTrailImage(s_selTrailImageName.c_str());
		imcEffect::TRAIL_ACTION* pTrailAction	= imcEffect::GetTrailAction(s_selTrailActionName.c_str());

		if (pTrailAction != NULL && pTrailImage != NULL)
		{			
			s_pCurTrailInst->SetSpeed(s_processSpeed);
			s_pCurTrailInst->SetTrailInfo(pTrailImage->imageName.c_str(), pTrailAction->actionName.c_str());

			// NOTE : 키프레임 트레일 사용			
			if (s_selTrailKeyFrameName != "None")
			{
				s_pCurTrailInst->SetTrailKeyFrame(s_selTrailKeyFrameName.c_str());
				s_pCurTrailInst->Start();
				s_pCurTrailInst->Play(0.0f);
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL UpdateTrail(float elapsedTime)
{
	if (s_pTrailDummyModel != NULL)
	{
		s_pTrailDummyModel->Update(elapsedTime);

		imc3d::IMotionInst* pMotionInst = NULL;
		if (s_pTrailDummyModel->GetNumMotionInst() == 0) 
		{
			imc3d::DestroyModel(s_pTrailDummyModel);
			s_pTrailDummyModel = NULL;

			if (s_pCurTrailInst != NULL)
			{
				// NOTE :  Recording 모드였다면 지우기 전에 파일로 저장
				if (s_pCurTrailInst->IsRecording() == TRUE)
				{
					s_pCurTrailInst->SaveKeyFrameToXML(s_trailKeyFrameSaveFileName.c_str());
				}

				imcEffect::ReleaseTrailInst(s_pCurTrailInst);
				s_pCurTrailInst = NULL;
			}
		}
	}

	if (s_pCurTrailInst != NULL)
	{
		s_pCurTrailInst->SetWorldTM(s_pActionDummy->GetTM());
		s_pCurTrailInst->Update(elapsedTime);
	}

	if (s_pCurTrailInst != NULL)
	{
		s_pCurTrailInst->Render();
		return TRUE;
	}
	
	return FALSE;
}

} 