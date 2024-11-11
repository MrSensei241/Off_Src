/// @file	ModelCamera.cpp
///

#include "StdAfx.h"
#include "ModelCamera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CModelCamera::CModelCamera()
{
	InitVar();
}

CModelCamera::~CModelCamera()
{
	Cleanup();
}

void CModelCamera::InitVar()
{
	m_at = imc::vector3(0, 50, -100);
	m_distance = 500.0f;
	m_latitude = 30.0;
	m_longitude = -90.0;

	m_windowWidth = 1280;
	m_windowHeight = 1024;
}

void CModelCamera::Cleanup()
{
}

void CModelCamera::Init(float nearPlane, float farPlane, float width, float height)
{
	CCamera::Init(45, nearPlane, farPlane, width, height);
}

void CModelCamera::BeginViewChange(CAMERA_MODE cameraMode, CPoint point)
{
	m_atUndo = m_at;
	m_distanceUndo = m_distance;
	m_latitudeUndo = m_latitude;
	m_longitudeUndo = m_longitude;

	float wx, wz;
	CCamera::GetPickingPos(float(point.x), float(point.y), 0, &wx, &wz);
	m_lastPan = imc::vector3(wx, 0, wz);
	m_lastPoint = point;
}

void CModelCamera::UpdateViewChange(CAMERA_MODE cameraMode, CPoint point)
{
	float dx = float(point.x - m_lastPoint.x) / 2.0f;
	float dy = float(point.y - m_lastPoint.y) / 2.0f;
	switch (cameraMode) {
	case CM_ZOOM:
		 {
			m_distance += dy * 2;

			if (m_distance < 10)
				m_distance = 10;

			UpdateCamera();
		}
		break;
	case CM_PAN_UPDOWN:
		 {
			m_at.y += dy;

			imc::vector3 sideVec = GetLeft();

			sideVec *= dx;
			m_at.x += sideVec.x;
			m_at.z += sideVec.z;

			UpdateCamera();
		}
		break;
	case CM_PAN:
		 {
			float wx, wz;
			CCamera::GetPickingPos(float(point.x), float(point.y), 0, &wx, &wz);
			imc::vector3 d;
			d = imc::vector3(wx, 0, wz);
			m_at.x -= d.x - m_lastPan.x;
			m_at.z -= d.z - m_lastPan.z;
			UpdateCamera();
			CCamera::GetPickingPos(float(point.x), float(point.y), 0, &wx, &wz);
			d = imc::vector3(wx, 0, wz);
			m_lastPan = d;
		}
		break;
	case CM_ANGLE:
		 {
			m_longitude -= float(dx) / 2;
			m_latitude += float(dy) / 2;

			while (m_longitude < 0)
				m_longitude += 360;

			while (m_longitude >= 360)
				m_longitude -= 360;

			while (m_latitude < -90 + 0.0001f)
				m_latitude = -90 + 0.0001f;

			while (m_latitude > 90 - 0.0001f)
				m_latitude = 90 - 0.0001f;

			UpdateCamera();
		}
		break;
	}
	m_lastPoint = point;
}

void CModelCamera::EndViewChange(CAMERA_MODE cameraMode, CPoint point)
{
	switch (cameraMode) {
	case CM_PAN:
		break;
	case CM_ZOOM:
		break;
	case CM_ANGLE:
		break;
	}
}

void CModelCamera::UndoViewChange()
{
	m_at = m_atUndo;
	m_distance = m_distanceUndo;
	m_latitude = m_latitudeUndo;
	m_longitude = m_longitudeUndo;
}

void CModelCamera::UpdateCamera(float windowWidth, float windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	CCamera::Init(45, 0.9f, 100000.0f, float(windowWidth), float(windowHeight)); // windowWidth, windowHeight

	D3DXVECTOR3 posEye;
	float radius = m_distance * cos(imc::DegToRad(m_latitude));
	posEye.y = m_at.y + m_distance * sin(imc::DegToRad(m_latitude));
	posEye.x = m_at.x + radius * cos(imc::DegToRad(m_longitude));
	posEye.z = m_at.z + radius * sin(imc::DegToRad(m_longitude));

	static const imc::vector3 unitY(0.0f, 1.0f, 0.0f);
	LookAt(posEye, m_at, unitY);

	BuildCameraMatrices();
}

void CModelCamera::UpdateCamera()
{
	UpdateCamera(m_windowWidth, m_windowHeight);
}

void CModelCamera::Reset()
{
	InitVar();
}