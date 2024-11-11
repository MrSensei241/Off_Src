#pragma once

namespace imcd3d9
{
	class Camera;
}

namespace appGizmo
{
void	Init(imcd3d9::Camera* pCamera);
void	Exit();

void	Update(float elapsedTime);
void	Render();

enum GIZMOTYPE
{
	GIZMO_MOVE,
	GIZMO_ROTATION,
	GIZMO_SCALE
};

void				SetGizmoType(GIZMOTYPE type);
GIZMOTYPE			GetGizmoType();
const imc::PRS*		GetGizmoPRS();

};
