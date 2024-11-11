#if !defined(AFX_WORLDEDITVIEW_H__5F999CB2_0671_4BE5_9F13_90A51F73BF19__INCLUDED_)
#define AFX_WORLDEDITVIEW_H__5F999CB2_0671_4BE5_9F13_90A51F73BF19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class	CWorldEditDoc;

class	CCamera {
public:
	void	Init(float nearPlane, float farPlane, float width, float height);
	void	SetPos(imc::vector3 pos);
	void	SetRot(float pitch, float yaw, float roll);
	void	SetOffset(imc::vector3 pos);
	void	BuildCameraMatrices();
	void	GetPickingRay(float MouseX, float MouseY, imc::vector3 *Origin, imc::vector3 *Normal) const;
	void	GetPickingPos(float MouseX, float MouseY, float FixedY, float *wx, float *wz) const;
	void	GetScreenPos(float x, float y, float z, float *MouseX, float *MouseY) const;
	const imc::matrix44 *GetViewMatrix() const { return (imc::matrix44 *)&m_viewMatrix; }
	const imc::matrix44 *GetProjMatrix() const { return (imc::matrix44 *)&m_projMatrix; }
	const imc::vector3 * GetCameraVec() const { return (imc::vector3 *)&m_cameraVec; }
	
private:
	D3DXMATRIX		m_viewMatrix;
	D3DXMATRIX		m_invViewMatrix;
	D3DXMATRIX		m_projMatrix;
	D3DXVECTOR4		m_cameraVec;
	imc::vector3	m_pos;
	float			m_pitch, m_yaw, m_roll;
	imc::vector3	m_offset;
	float			m_width, m_height;
};

class CWorldEditView : public CView {
protected: // create from serialization only
	CWorldEditView();
	DECLARE_DYNCREATE(CWorldEditView)

// Attributes
public:
	CWorldEditDoc* GetDocument();

// Operations
public:
	//	manipulation
	//	뷰포트 조작에 관련된 변수와 함수들
	enum	ViewControlMode {
		VCM_NORMAL,		//	아무것도 수행하지 않고 마우스만 움직이는 상태
		VCM_SELECT1,	//	왼쪽 버턴으로 셀렉트하는 상태
		VCM_SELECT2,	//	오른쪽 버턴으로 셀렉트하는 상태
		VCM_EDITING,	//	오브젝트를 편집중인 상태
		VCM_PAN,		//	화면 스크롤
		VCM_ZOOM,		//	화면 확대축소
		VCM_ANGLE		//	화면 각도 조절
	};
	float		m_distance;
	imc::vector3	m_at;
	float		m_longitude, m_latitude;
	void		UpdateCamera();
	ViewControlMode	m_viewControlMode;
	void		BeginViewChange(CPoint p);
	void		UpdateViewChange(CPoint p);
	void		FinishViewChange(CPoint p);
	void		AbortViewChange(CPoint p);
	imc::vector3	m_lastPan;
	float		m_undoLatitude, m_undoLongitude, m_undoDistance;
	float		m_startAtX, m_startAtZ;
	CPoint		m_lastPos;
	
	//	Selection
	void		BeginDragSelect(CPoint p, UINT nFlags);
	void		UpdateDragSelect(CPoint p, UINT nFlags);
	void		FinishDragSelect(CPoint p);
	void		AbortDragSelect(CPoint p);
	CPoint		m_dragStart, m_lastDragPos;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorldEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	imc3d::IRenderer *	m_renderer;
	imc3d::CAxisGrid	m_axisGrid;
	CCamera		m_camera;
	DWORD	m_renderSignature;

// Generated message map functions
protected:
	//{{AFX_MSG(CWorldEditView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WorldEditView.cpp
inline CWorldEditDoc* CWorldEditView::GetDocument()
   { return (CWorldEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDEDITVIEW_H__5F999CB2_0671_4BE5_9F13_90A51F73BF19__INCLUDED_)
