// WorldEditView.cpp : implementation of the CWorldEditView class
//

#include "stdafx.h"
#include "Mfc3dTest.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void	CCamera :: Init(float nearPlane, float farPlane, float width, float height)
{
//	D3DXMatrixOrthoRH(&m_projMatrix, width, height, nearPlane, farPlane);
	D3DXMatrixPerspectiveFovRH(&m_projMatrix, imc::DegToRad(45), width/height, nearPlane, farPlane); 
	m_width = width;
	m_height = height;
}

void	CCamera :: SetPos(imc::vector3 pos)
{
	m_pos = pos;
}

void	CCamera :: SetRot(float pitch, float yaw, float roll)
{
	m_pitch = pitch;
	m_yaw = yaw;
	m_roll = roll;
}

void	CCamera :: SetOffset(imc::vector3 offset)
{
	m_offset = offset;
}

void	CCamera :: BuildCameraMatrices()
{
	D3DXMATRIX	t, r, rx, ry, rz, temp, ir;

	D3DXMatrixRotationY(&ry, m_yaw);
	D3DXMatrixRotationX(&rx, m_pitch);
	D3DXMatrixRotationZ(&rz, m_roll);
	D3DXMatrixMultiply(&temp, &ry, &rx);
	D3DXMatrixMultiply(&r, &temp, &rz);

    D3DXVECTOR3	camVec(0.0f, 0.0f, -1.0f);
	D3DXMatrixInverse(&ir, NULL, &r);
	D3DXVec3TransformCoord(&camVec, &camVec, &ir);
	D3DXVec3Normalize(&camVec, &camVec);
	m_cameraVec.x = camVec.x;
	m_cameraVec.y = camVec.y;
	m_cameraVec.z = camVec.z;
	m_cameraVec.w = 1.0f;
	
	
    D3DXVECTOR3	Pos;
	Pos.x = m_pos.x;
	Pos.y = m_pos.y;
	Pos.z = m_pos.z;

    D3DXVECTOR3	invPos;
	D3DXVec3TransformCoord(&invPos, &Pos, &r);

    D3DXVECTOR3	offset;
	offset.x = m_offset.x;
	offset.y = m_offset.y;
	offset.z = m_offset.z;
	D3DXVec3Add(&invPos, &invPos, &offset);
	D3DXVec3Scale(&invPos, &invPos, -1);

	D3DXMatrixTranslation(&t, invPos.x, invPos.y, invPos.z);

	D3DXMatrixMultiply(&m_viewMatrix, &r, &t);
	D3DXMatrixInverse(&m_invViewMatrix, NULL, &m_viewMatrix);
}

void	CCamera :: GetPickingRay(float MouseX, float MouseY, imc::vector3 *Origin, imc::vector3 *Normal) const
{
	D3DXVECTOR3	screenPoint(MouseX, MouseY, 0);
	D3DVIEWPORT9 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = m_width;
	vp.Height = m_height;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;

	D3DXMATRIX	identity;
	D3DXMatrixIdentity(&identity);

	D3DXVECTOR3	worldPoint;
	D3DXVec3Unproject(&worldPoint, &screenPoint, &vp, &m_projMatrix, &m_viewMatrix, &identity);

    D3DXVECTOR3	origin;
	origin.x = m_invViewMatrix._41;
	origin.y = m_invViewMatrix._42;
	origin.z = m_invViewMatrix._43;

	D3DXVECTOR3	normal;
	D3DXVec3Subtract(&normal, &worldPoint, &origin);
	D3DXVec3Normalize(&normal, &normal);

	*Origin = origin;
	*Normal = normal;
}

void	CCamera :: GetPickingPos(float MouseX, float MouseY, float FixedY, float *wx, float *wz) const
{
	D3DXVECTOR3	screenPoint(MouseX, MouseY, 0);
	D3DVIEWPORT9 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = m_width;
	vp.Height = m_height;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;

	D3DXMATRIX	identity;
	D3DXMatrixIdentity(&identity);

	D3DXVECTOR3	worldPoint;
	D3DXVec3Unproject(&worldPoint, &screenPoint, &vp, &m_projMatrix, &m_viewMatrix, &identity);

    D3DXVECTOR3	origin;
	origin.x = m_invViewMatrix._41;
	origin.y = m_invViewMatrix._42;
	origin.z = m_invViewMatrix._43;

	D3DXVECTOR3	normal;
	D3DXVec3Subtract(&normal, &worldPoint, &origin);
	D3DXVec3Normalize(&normal, &normal);
	
	float	t;
	if (normal.y != 0.0f)
		t = (FixedY-origin.y)/normal.y;
	else
		t = 1;
	*wx = origin.x + t*normal.x;
	*wz = origin.z + t*normal.z;
}

void	CCamera :: GetScreenPos(float x, float y, float z, float *MouseX, float *MouseY) const
{
	D3DXVECTOR3	worldPoint(x, y, z);
	D3DXVECTOR3	screenPoint;
	D3DVIEWPORT9 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = m_width;
	vp.Height = m_height;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;
	
	D3DXMATRIX	identity;
	D3DXMatrixIdentity(&identity);
	
	D3DXVec3Project(&screenPoint, &worldPoint, &vp, &m_projMatrix, &m_viewMatrix, &identity);
	*MouseX = screenPoint.x;
	*MouseY = screenPoint.y;
}



/////////////////////////////////////////////////////////////////////////////
// CWorldEditView

IMPLEMENT_DYNCREATE(CWorldEditView, CView)

BEGIN_MESSAGE_MAP(CWorldEditView, CView)
	//{{AFX_MSG_MAP(CWorldEditView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView construction/destruction

CWorldEditView::CWorldEditView()
{
	m_renderSignature = 0;
	m_renderer = imc3d::GetDevice()->CreateRenderer();
}

CWorldEditView::~CWorldEditView()
{
	imc3d::GetDevice()->DestroyRenderer(m_renderer);
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView drawing

void CWorldEditView::OnDraw(CDC* pDC)
{
	CWorldEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	RECT	r;
	GetClientRect(&r);

	imc3d::IDevice * pDev = imc3d::GetDevice();

	m_camera.Init(10, 5000, r.right, r.bottom);

	pDev->SetViewport(0, 0, r.right, r.bottom);

	pDev->SetViewTM(m_camera.GetViewMatrix());
	pDev->SetProjTM(m_camera.GetProjMatrix());

	pDev->Clear(0xffa0a0a0);
	if (pDev->BeginScene()) {
		m_axisGrid.Render(m_renderer, m_at);

		m_renderer->Flush();
		pDev->EndScene();
	}
	pDev->Flip(GetSafeHwnd());
	m_renderSignature++;
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView diagnostics

#ifdef _DEBUG
void CWorldEditView::AssertValid() const
{
	CView::AssertValid();
}

void CWorldEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWorldEditDoc* CWorldEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWorldEditDoc)));
	return (CWorldEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView message handlers

void	CWorldEditView::OnInitialUpdate() 
{
	m_distance = 500;
	m_latitude = 45.0;
	m_longitude = 0.0;
	m_at = imc::vector3(0.0f, 0.0f, 0.0f);
	UpdateCamera();
}

BOOL CWorldEditView::OnEraseBkgnd(CDC* pDC) 
{
	return	FALSE;
}

void	CWorldEditView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	switch (GetMainFrameWnd()->m_controlMode) {
		case	CMainFrame::MODE_SELECT:
			m_viewControlMode = VCM_SELECT1;
			BeginDragSelect(point, nFlags);
			SetCapture();
			return;
		case	CMainFrame::MODE_SELECTMOVE:
		case	CMainFrame::MODE_SELECTROTATE:
		case	CMainFrame::MODE_SELECTSCALE:
			BeginDragSelect(point, nFlags);
			break;				
		case	CMainFrame::MODE_PANVIEW:
			m_viewControlMode = VCM_PAN;
			BeginViewChange(point);
			SetCapture();
			return;
		case	CMainFrame::MODE_ROTATEVIEW:
			m_viewControlMode = VCM_ANGLE;
			BeginViewChange(point);
			SetCapture();
			return;
		case	CMainFrame::MODE_ZOOMVIEW:
			m_viewControlMode = VCM_ZOOM;
			BeginViewChange(point);
			SetCapture();
			return;
	}
}

void	CWorldEditView::OnMouseMove(UINT nFlags, CPoint point) 
{
	switch (m_viewControlMode) {
		case	VCM_NORMAL:
			break;
		case	VCM_EDITING:
			break;
		case	VCM_SELECT1:
			UpdateDragSelect(point, nFlags);
			break;
		case	VCM_PAN:
		case	VCM_ZOOM:
		case	VCM_ANGLE:
			UpdateViewChange(point);
			break;
	}
	m_lastPos = point;
}

void	CWorldEditView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	switch (m_viewControlMode) {
		case	VCM_SELECT1:
			FinishDragSelect(point);
			Invalidate();
			m_viewControlMode = VCM_NORMAL;
			ReleaseCapture();
			break;
		case	VCM_PAN:
		case	VCM_ZOOM:
		case	VCM_ANGLE:
			FinishViewChange(point);
			ReleaseCapture();
			break;
		case	VCM_EDITING:
			m_viewControlMode = VCM_NORMAL;
			ReleaseCapture();
			break;
	}
}

void	CWorldEditView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	switch (m_viewControlMode) {
		case	VCM_SELECT1:
			AbortDragSelect(point);
			Invalidate();
			m_viewControlMode = VCM_NORMAL;
			ReleaseCapture();
			break;
		case	VCM_PAN:
		case	VCM_ZOOM:
		case	VCM_ANGLE:
			AbortViewChange(point);
			Invalidate();
			ReleaseCapture();
			m_viewControlMode = VCM_NORMAL;
			break;
		case	VCM_EDITING:
			m_viewControlMode = VCM_NORMAL;
			ReleaseCapture();
			break;
	}
}

void	CWorldEditView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnRButtonUp(nFlags, point);
}

void CWorldEditView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditView viewport manipulation

void	CWorldEditView::UpdateCamera()
{
	m_camera.SetPos(m_at);
	m_camera.SetRot(imc::DegToRad(m_latitude), imc::DegToRad(m_longitude), 0);
	m_camera.SetOffset(imc::vector3(0, 0, m_distance));
	m_camera.BuildCameraMatrices();
}

void	CWorldEditView::BeginViewChange(CPoint p)
{
	//	백업
	m_undoDistance = m_distance;
	float	wx, wz;
	m_camera.GetPickingPos(p.x, p.y, 0, &wx, &wz);
	m_lastPan = imc::vector3(wx, 0, wz);
	m_startAtX = m_at.x;
	m_startAtZ = m_at.z;
	m_undoLatitude = m_latitude;
	m_undoLongitude = m_longitude;
	m_lastPos = p;
}

void	CWorldEditView::UpdateViewChange(CPoint p)
{
	int		dx = p.x - m_lastPos.x;
	int		dy = p.y - m_lastPos.y;
	CString		t;
	imc::vector3		d;
	float	wx, wz;

	switch (m_viewControlMode) {
		case	VCM_ZOOM:
			m_distance += dy * 2;

			if (m_distance < 50)
				m_distance = 50;

			UpdateCamera();
			Invalidate();
			t.Format("시점과의 거리 : %f", m_distance);
			GetMainFrameWnd()->SetPaneText(t);
			break;

		case	VCM_PAN:
			m_camera.GetPickingPos(p.x, p.y, 0, &wx, &wz);
			d = imc::vector3(wx, 0, wz);
			m_at.x -= d.x-m_lastPan.x;
			m_at.z -= d.z-m_lastPan.z;
			UpdateCamera();
			Invalidate();
			m_camera.GetPickingPos(p.x, p.y, 0, &wx, &wz);
			d = imc::vector3(wx, 0, wz);
			break;

		case	VCM_ANGLE:
			m_longitude += float(dx) / 2;
			m_latitude  += float(dy) / 2;

			while (m_longitude<0)
				m_longitude += 360;

			while (m_longitude>=360)
				m_longitude -= 360;

			while (m_latitude < -90)
				m_latitude = -90;

			while (m_latitude > 90)
				m_latitude = 90;

			t.Format("경도 : %f, 위도 : %f", m_longitude, m_latitude);
			GetMainFrameWnd()->SetPaneText(t);
			UpdateCamera();
			Invalidate();
			break;
	}
	m_lastPos = p;
	m_lastPan = d;
}

void	CWorldEditView::FinishViewChange(CPoint p)
{
	m_viewControlMode = VCM_NORMAL;
}

void	CWorldEditView::AbortViewChange(CPoint p)
{
	//	백업한걸로 되돌림
	m_viewControlMode = VCM_NORMAL;
	m_distance = m_undoDistance;
	m_at.x = m_startAtX;
	m_at.z = m_startAtZ;
	m_latitude = m_undoLatitude;
	m_longitude = m_undoLongitude;
	UpdateCamera();
}

void	CWorldEditView::BeginDragSelect(CPoint p, UINT nFlags)
{
	CWorldEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_dragStart = m_lastDragPos = p;

	//	make ray
	imc::ray3	ray;
	m_camera.GetPickingRay(p.x, p.y, &ray.org, &ray.dir);

	Invalidate();
}

//	frustum pick
void	CWorldEditView::UpdateDragSelect(CPoint p, UINT nFlags)
{
	CWorldEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	RECT	r;
	GetClientRect(&r);

	m_lastDragPos = p;

	//	make frustum (with m_dragStart, m_lastDragPos)
	imc::Frustum	vfPicker;
	float	left, right, top, bottom;	//	range: -1.0f ~ 1.0f
	left = ((float(min(m_dragStart.x, m_lastDragPos.x)) / float(r.right))-0.5f)*2;
	right = ((float(max(m_dragStart.x, m_lastDragPos.x)) / float(r.right))-0.5f)*2;
	bottom = ((float(min(r.bottom-m_dragStart.y, r.bottom-m_lastDragPos.y)) / float(r.bottom))-0.5f)*2;
	top = ((float(max(r.bottom-m_dragStart.y, r.bottom-m_lastDragPos.y)) / float(r.bottom))-0.5f)*2;
//	vfPicker.Build((imc::matrix44 *)m_camera.GetViewMatrix(), (imc::matrix44 *)m_camera.GetProjMatrix(), left, right, top, bottom);
	Invalidate();
}

void	CWorldEditView::FinishDragSelect(CPoint p)
{
}

void	CWorldEditView::AbortDragSelect(CPoint p)
{
}

