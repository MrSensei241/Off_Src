#include "stdafx.h"

#include "imcd3d9.h"
#include "imcToolKit.h"

#include "AppGizmo.h"

namespace appGizmo
{
static imcGizmo::IGizmo* s_pCurrentGizmo	= NULL;

static imcGizmo::IGizmo* s_pMoveGizmo		= NULL;
static imcGizmo::IGizmo* s_pRotationGizmo	= NULL;
static imcGizmo::IGizmo* s_pScaleGizmo		= NULL;

void Init(imcd3d9::Camera* pCamera)
{
	imcGizmo::Init(pCamera);

	s_pMoveGizmo		= imcGizmo::CreateGizmo(imcGizmo::GT_MOVE, "Move");
	s_pRotationGizmo	= imcGizmo::CreateGizmo(imcGizmo::GT_ROTATION, "Rotation");
	s_pScaleGizmo		= imcGizmo::CreateGizmo(imcGizmo::GT_SCALE, "Scale");

	//SetGizmoType(GIZMO_MOVE);
}

void Exit()
{
	imcGizmo::DestroyGizmo(s_pMoveGizmo);
	imcGizmo::DestroyGizmo(s_pRotationGizmo);
	imcGizmo::DestroyGizmo(s_pScaleGizmo);

	imcGizmo::Exit();
}

void Update(float elapsedTime)
{	
	imcGizmo::SetMouseState(imcinput::GetX(), imcinput::GetY(), imcinput::IsLBtnPressed(), imcinput::IsRBtnPressed());

	if (s_pCurrentGizmo != NULL)
		s_pCurrentGizmo->Update(elapsedTime);	
}

void Render()
{
	if (s_pCurrentGizmo != NULL)
		s_pCurrentGizmo->Render();
}

GIZMOTYPE s_gizmoType = GIZMO_MOVE;
void SetGizmoType(GIZMOTYPE type)
{
	s_gizmoType = type;
	switch(s_gizmoType)
	{
	case GIZMO_MOVE:
		s_pCurrentGizmo = s_pMoveGizmo;
		break;

	case GIZMO_ROTATION:
		s_pCurrentGizmo = s_pRotationGizmo;
		break;

	case GIZMO_SCALE:
		s_pCurrentGizmo = s_pScaleGizmo;
		break;
	}
}

GIZMOTYPE GetGizmoType()
{
	return s_gizmoType;
}

const imc::PRS* GetGizmoPRS()
{
	if (s_pCurrentGizmo != NULL)
		return s_pCurrentGizmo->GetPRS();	

	return NULL;
}

} 