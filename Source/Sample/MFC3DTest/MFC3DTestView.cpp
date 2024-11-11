// MFC3DTestView.cpp : implementation of the CMFC3DTest class
//

#include "stdafx.h"
#include "MFC3DTest.h"

#include "MFC3DTestDoc.h"
#include "MFC3DTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTest

IMPLEMENT_DYNCREATE(CMFC3DTestView, CView)

BEGIN_MESSAGE_MAP(CMFC3DTestView, CView)
	//{{AFX_MSG_MAP(CMFC3DTest)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTest construction/destruction

CMFC3DTestView::CMFC3DTestView()
{
	// TODO: add construction code here
	m_bActive = FALSE;
	m_bLClick = FALSE;
	m_bMClick = FALSE;

	m_bD3DInit = FALSE;

	m_renderer = NULL;

	m_checkFPS = FALSE;
	m_fTrsPerSec = 0.0f;
}

CMFC3DTestView::~CMFC3DTestView()
{
}

BOOL CMFC3DTestView::PreCreateWindow(CREATESTRUCT & cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTest drawing

void CMFC3DTestView::OnDraw(CDC * pDC)
{
	CMFC3DTestDoc * pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTest diagnostics

#ifdef _DEBUG
void CMFC3DTestView::AssertValid() const
{
	CView::AssertValid();
}

void CMFC3DTestView::Dump(CDumpContext & dc) const
{
	CView::Dump(dc);
}

CMFC3DTestDoc * CMFC3DTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFC3DTestDoc)));
	return (CMFC3DTestDoc *) m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTest message handlers

void CMFC3DTestView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	if (!m_bD3DInit) {
		CRect rect;
		GetClientRect(&rect);

		imc3d::IDevice * pDev = imc3d::GetDevice();
		if (!pDev->Init(GetSafeHwnd(), TRUE, rect.Width(), rect.Height(), ::GetDeviceCaps(::GetDC(GetSafeHwnd()), BITSPIXEL)))
			Error("cannot create 3d device");


		//렌더러 생성
		m_renderer = pDev->CreateRenderer();

		// start time caculator : elapsed time calculator
		m_bD3DInit = TRUE;
	}
}

int CMFC3DTestView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	//3d 초기화
	imc::CDirSet curDir(imc::eCodeBaseDir);
	imc::CDirMgr * pDirMgr = imc::GetDirMgr();
	pDirMgr->SetCurDir(imc::eShaderDir, pDirMgr->GetSlotPath(imc::eCodeBaseDir, imc::DIR_DEFAULT));

	if (!imc3d::Init())
		return FALSE;
	imcresmgr::Init();
	imcscene::Init();	

	return 0;
}

void CMFC3DTestView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	//3D해제
	if (m_bD3DInit) {
		imc3d::IDevice * pDev = imc3d::GetDevice();
		// reset time calculator
		pDev->DestroyRenderer(m_renderer);
		pDev->Exit();
		m_bD3DInit = FALSE;
	}

	imcscene::Exit();
	imcresmgr::Exit();
	imc3d::Exit();
}

void CMFC3DTestView::Render()
{
	if (m_bD3DInit) {
		imc3d::IDevice * pDev = imc3d::GetDevice();

		CRect crect;
		GetClientRect(&crect);
		m_modelCamera.UpdateCamera(float(crect.Width()), float(crect.Height()));
		//pDev->SetViewportSize( 0, 0, crect.Width(), crect.Height());

		pDev->SetViewTM(m_modelCamera.GetViewMatrix());
		pDev->SetProjTM(m_modelCamera.GetProjMatrix());

		pDev->Clear(0xffa0a0a0);
		if (pDev->BeginScene()) {
			m_axisGrid.Render(m_renderer, m_modelCamera.m_at);

			m_renderer->Flush();
			pDev->EndScene();
		}
		pDev->Flip(GetSafeHwnd());
	}
}

void CMFC3DTestView::SetVMode()
{
	BOOL prevActive = m_bActive;

	m_bActive = TRUE;
	if (GetAsyncKeyState(VK_LMENU)) {
		if (m_bLClick && m_bMClick) {
			m_nVMode = CM_ZOOM;		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM));
		} else if (m_bLClick) {
			m_nVMode = CM_ANGLE;		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ROTATE));
		} else if (m_bMClick) {
			m_nVMode = CM_PAN_UPDOWN;	SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE));
		} else
			m_bActive = FALSE;
	} else
		m_bActive = FALSE;		

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if (!prevActive && m_bActive) {
		m_modelCamera.BeginViewChange(m_nVMode, point);
		SetCapture();
	}
	if (prevActive && !m_bActive) {
		//		TRACE2( "view end:%d,%d\n", point.x, point.y );
		m_modelCamera.EndViewChange(m_nVMode, point);
		ReleaseCapture();
	}
}

BOOL CMFC3DTestView::PreTranslateMessage(MSG * pMsg)
{
	switch (pMsg->message) {
	case WM_LBUTTONDOWN:
		//		TRACE( "mouse L down\n" );
		m_bLClick = TRUE;
		break;
	case WM_LBUTTONUP:
		//		TRACE( "mouse L up\n" );
		m_bLClick = FALSE;
		break;
	case WM_MBUTTONDOWN:
		//		TRACE( "mouse M down\n" );
		m_bMClick = TRUE;
		break;
	case WM_MBUTTONUP:
		//		TRACE( "mouse M up\n" );
		m_bMClick = FALSE;
		break;
	}
	SetVMode();

	return CWnd::PreTranslateMessage(pMsg);
}

void CMFC3DTestView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bActive) {
		m_modelCamera.UpdateViewChange(m_nVMode, point);
	} else {
		imc::ray3 ray;
		m_modelCamera.GetPickingRay(float(point.x), float(point.y), &ray.org, &ray.dir);
	}
	CWnd::OnMouseMove(nFlags, point);
	return;
}