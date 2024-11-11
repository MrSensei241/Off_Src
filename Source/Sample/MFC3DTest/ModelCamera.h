/// @file	ModelCamera.h

#ifndef __MODELCAMERA
#define __MODELCAMERA

/// 카메라 조작모드
enum CAMERA_MODE {
	CM_PAN_UPDOWN,
	CM_PAN,
	CM_ZOOM,
	CM_ANGLE,
};


/// 카메라 클래스
class CModelCamera : public imc3d::CCamera {
public:
					CModelCamera();
					~CModelCamera();

	void			InitVar();
	void			Cleanup();

	void			Init(float nearPlane, float farPlane, float width, float height);

	void			BeginViewChange(CAMERA_MODE cameraMode, CPoint point);
	void			EndViewChange(CAMERA_MODE cameraMode, CPoint point);
	void			UpdateViewChange(CAMERA_MODE cameraMode, CPoint point);
	void			Apply(float windowWidth, float windowHeight);
	void			UpdateCamera(float windowWidth, float windowHeight);
	void			UpdateCamera();
	void			UndoViewChange();
	void			Reset();

	imc::vector3	m_at;
	float			m_distance;
	float			m_latitude;
	float			m_longitude;

	imc::vector3	m_atUndo;
	float			m_distanceUndo;
	float			m_latitudeUndo;
	float			m_longitudeUndo;

	CPoint			m_lastPoint;
	imc::vector3	m_lastPan;

	float			m_windowWidth;
	float			m_windowHeight;
};

#endif

