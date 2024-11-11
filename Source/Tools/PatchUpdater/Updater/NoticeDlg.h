#pragma once
#include "webbrowserctrl.h"

class CNoticeDlg : public CDialog
{
public:
	CNoticeDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_NOTICE };
	CWebBrowserCtrl	m_explorerCtrl;

	void Update();
	virtual BOOL CNoticeDlg::Create(CWnd* pParentWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
};
