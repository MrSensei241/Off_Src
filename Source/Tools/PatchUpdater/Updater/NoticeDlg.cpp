#include "stdafx.h"
#include "Updater.h"
#include "NoticeDlg.h"
#include ".\noticedlg.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNoticeDlg::CNoticeDlg(CWnd* pParent)
	: CDialog(CNoticeDlg::IDD, pParent)
{
}

void CNoticeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER, m_explorerCtrl);
}


BEGIN_MESSAGE_MAP(CNoticeDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

BOOL CNoticeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_explorerCtrl.Navigate(Config::URL::notice, 0, NULL, NULL, NULL);

	return TRUE;
}

BOOL CNoticeDlg::Create(CWnd* pParentWnd) 
{	
	return CDialog::Create(IDD, pParentWnd);
}

void CNoticeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);	
}

void CNoticeDlg::Update()
{
	CRect rect;
	GetWindowRect(rect);
	m_explorerCtrl.SetWindowPos(&CWnd::wndTop, -2, -2, rect.Width()+4, rect.Height()+4, SWP_SHOWWINDOW);
}

void CNoticeDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (VK_ESCAPE == nChar)
		return;
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CNoticeDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CNoticeDlg::OnCancel()
{
	return;
}
