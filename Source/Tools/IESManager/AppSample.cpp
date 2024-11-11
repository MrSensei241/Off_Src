#include "stdafx.h"

#include "imcd3d9.h"

#include "imcEffect.h"
#include "imcEffectUtil.h"

#include "imcfont.h"
#include "imcUiRender.h"

#include "App.h"

namespace app
{
extern imcd3d9::IDevice*	s_pDev;

extern imcd3d9::Camera*		s_pSampleCam;
extern imc::vector3			s_sampleEyePos;
extern imcd3d9::ITexture*	s_pBgSample;

extern HWND					s_sampleWindowHandle;

imcEffect::IComponent*		s_pComponentSample		= NULL;
std::string					s_componentSampleName	= "None";
imcEffect::IEffectInstance* s_pComponentEffect		= NULL;
BOOL						s_isDirtyComponentSample = TRUE;

imc::matrix44				s_trailTM;
float						s_trailSpeed = 0.0f;
float						s_currentAngle = 0.0f;


void SetSampleEyePos(imc::vector3& eyePos)
{
	s_sampleEyePos			= eyePos;
}

void SetSampleHandle(HWND hWnd)
{
	s_sampleWindowHandle	= hWnd;
}

void ResetSampleHandle()
{
	s_sampleWindowHandle = NULL;	
	ResetComponentSampleEffect();
}

void SetSampleComponent(const char* componentName)
{
	IMC_ASSERT(componentName);

	if (s_componentSampleName != componentName)
	{
		s_componentSampleName = componentName;

		s_pComponentSample = imcEffect::util::GetComponentByString(s_componentSampleName.c_str());
	}

	s_isDirtyComponentSample = TRUE;
}

void ResetComponentSampleEffect()
{
	if (s_pComponentEffect != NULL)
	{
		imcEffect::ReleaseEffectInst(s_pComponentEffect, TRUE);
		s_pComponentEffect = NULL;
	}
}

void UpdateSampleCamera(float elapsedTime)
{
	imc::vector3 eye	= s_sampleEyePos;
	imc::vector3 at		= imc::vOrigin;

	eye += at;

	s_pSampleCam->LookAt(eye, at, imc::vUp);

	s_pSampleCam->Update(elapsedTime);

	s_pDev->SetViewTM(s_pSampleCam->GetViewTM());
	s_pDev->SetProjTM(s_pSampleCam->GetProjTM());

	imcEffect::SetViewInfo(s_pSampleCam);
}


} 