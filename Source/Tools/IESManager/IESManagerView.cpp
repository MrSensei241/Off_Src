// EffectEditorView.cpp : implementation of the CEffectEditorView class
//

#include "stdafx.h"

#include "app.h"

#include "PaneView.h"

#include "IESManager.h"
#include "IESManagerDoc.h"
#include "IESManagerView.h"
#include "MainFrm.h"

#include "PaneManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEffectEditorView

IMPLEMENT_DYNCREATE(CEffectEditorView, CView)

BEGIN_MESSAGE_MAP(CEffectEditorView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_SIZE()	
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()	
	ON_WM_KEYDOWN()		
END_MESSAGE_MAP()

// NOTE :	생성/소멸자
CEffectEditorView::CEffectEditorView()			
{	
}

CEffectEditorView::~CEffectEditorView()	
{
	app::Exit();
}

// NOTE : 페인팅
BOOL CEffectEditorView::OnEraseBkgnd(CDC* pDC)	
{
	return TRUE;
}

void CEffectEditorView::OnDraw(CDC* /*pDC*/)
{
	CEffectEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)	return;
}

// CEffectEditorView printing
BOOL CEffectEditorView::OnPreparePrinting(CPrintInfo* pInfo)	{return DoPreparePrinting(pInfo);}
void CEffectEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)		{}
void CEffectEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)		{}

// CEffectEditorView diagnostics
#ifdef _DEBUG
void CEffectEditorView::AssertValid() const				{CView::AssertValid();}
void CEffectEditorView::Dump(CDumpContext& dc) const		{CView::Dump(dc);}
CEffectEditorDoc* CEffectEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEffectEditorDoc)));
	return (CEffectEditorDoc*)m_pDocument;
}
#endif //_DEBUG

// NOTE : 메세지 핸들러
BOOL CEffectEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// NOTE : 모듈 초기화
void CEffectEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	if (!app::IsInitialized())
	{		
		GetPaneManager().Init((CMainFrame*)AfxGetMainWnd());
		app::Init(GetSafeHwnd(), 900, 800);
	}
}

// NOTE : 리사이징 처리
void CEffectEditorView::OnSize(UINT nType, int cx, int cy)
{	
	if (app::IsInitialized())
	{		
		app::Resize(cx, cy);	
	}
	CView::OnSize(nType, cx, cy);
}

// NOTE : 커서 이동
void CEffectEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint deltaPoint = point - m_oldCursorPos;	
	m_oldCursorPos = point;
	
	app::MouseMove(point.x, point.y);
	CView::OnMouseMove(nFlags, point);
}

BOOL CEffectEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CEffectEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
		
	//ShowCursor(FALSE);
	SetCapture();

	m_capturePos = point;
	m_oldCursorPos = point;
	CView::OnLButtonDown(nFlags, point);
}

void CEffectEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		ClientToScreen(&m_capturePos);

		SetCursorPos(m_capturePos.x, m_capturePos.y);

		ShowCursor(TRUE);
		ReleaseCapture();
	}

	CView::OnLButtonUp(nFlags, point);
}

void CEffectEditorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	//ShowCursor(FALSE);
	SetCapture();

	m_capturePos = point;
	m_oldCursorPos = point;
	CView::OnRButtonDown(nFlags, point);
}

void CEffectEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{		
	if (GetCapture() == this)
	{
		ClientToScreen(&m_capturePos);
		SetCursorPos(m_capturePos.x, m_capturePos.y);

		ShowCursor(TRUE);
		ReleaseCapture();
	}

	CView::OnRButtonUp(nFlags, point);
}

void CEffectEditorView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{


	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CEffectEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{	
	case VK_SPACE: 
		{
			CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();
			//pFrame->Play();
		}
		break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

