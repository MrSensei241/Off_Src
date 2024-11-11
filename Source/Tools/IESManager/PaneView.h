// PaneView.h : header file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PANEVIEW_H__31D830D9_23DE_4FA5_9213_3FC8E271E0CD__INCLUDED_)
#define AFX_PANEVIEW_H__31D830D9_23DE_4FA5_9213_3FC8E271E0CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CPaneView window

#define ID_TREE_EFFECT			2001
#define ID_TREE_EMITTER		2002
#define ID_REPORT_PROGRESS		2003
#define ID_REPORT_TEXTURELIST	2005
#define ID_BTN_SAVETEXTURELIST	2006
#define ID_BTN_LOADTEXTURELIST	2007
#define ID_BTN_TEXTUREADD		2008
#define ID_BTN_TEXTUREDEL		2009
#define ID_TREE_MONSTER			2010

#define ID_VIEW_COMPONENT		2100
#define ID_TREE_COMPONENT		2101
#define ID_REPORT_COMPONENT_USEDEFFECT	2102
#define ID_BUTTON_COMPONENT_SAVE		2105
#define ID_BUTTON_COMPONENT_APPLY		2106
#define ID_BUTTON_COMPONENT_WHOLE_DIR	2107
#define ID_BUTTON_COMPONENT_DELETE		2108
#define ID_BUTTON_OFFSET_CHANGE			2109
#define ID_BUTTON_COMPONENT_SAVEALL		2110
#define ID_BUTTON_ALL_KEYFRAME			2111
#define ID_BUTTON_ALL_DIR				2112
#define ID_BUTTON_ALL_SPRITE			2113


#define ID_VIEW_KEYFRAME		2200
#define ID_REPORT_KEYFRAME		2201
#define ID_STATIC_KEYFRAME		2202
#define ID_COMBO_KEYFRAME_INTERPOLATION	2203
#define ID_CHECK_KEYFRAME_VISIBLE1		2204
#define	ID_CHECK_KEYFRAME_VISIBLE2		2205
#define ID_CHECK_KEYFRAME_VISIBLE3		2206
#define ID_BUTTON_KEYFRAME_ADDFRAME		2207
#define ID_BUTTON_KEYFRAME_DELETEFRAME	2208
#define ID_BUTTON_KEYFRAME_CLOSE		2209

#define ID_TREE_SOUNDEVENT		2300
#define ID_BTN_SOUNDEVENT_PLAY	2301
#define ID_BTN_SOUNDEVENT_SELECT	2302
#define ID_BTN_SOUNDEVENT_CANCEL	2303

#define ID_TAB_TRAIL_MODE			2400
#define ID_TREE_TRAIL				2401
#define ID_COMBO_TRAIL_IMAGE		2402
#define ID_COMBO_TRAIL_ACTION		2403
#define IDC_EDIT_TRAIL_SAMPLE		2404
#define ID_BTN_TRAIL_SELECTSAMPLE	2405
#define ID_CHECK_TRAIL_RECORDINGMODE	2406
#define IDC_EDIT_TRAIL_SAVE		2407
#define ID_BTN_TRAIL_SELECTSAVEFILE	2408

#define ID_CHECK_SHOWBG			3000
#define ID_DEPLOY				3001

#define ID_BTN_BGIMAGE_SELECT	4000
#define ID_BTN_BGIMAGE_CANCEL	4001

#define ID_DEADTOOL_TREE		5001
#define ID_DEADTOOL_KILL		5002
#define ID_DEADTOOL_SAVE		5003



enum LayoutType
{
	LAYOUT_FIXED,
	LAYOUT_RATE,
	LAYOUT_MARGIN
};

struct ControlProperty
{
	ControlProperty(){
		type	= LAYOUT_FIXED;
		pWnd	= NULL;	
	}

	LayoutType		type;
	CWnd*			pWnd;
	imc::vector4	vSize;
};

class CXTPPropertyGridItem;
typedef imc::CVector<CXTPPropertyGridItem*>	SPRITE_EFFECT_LIST;

class CPaneView : public CWnd{
public:

					CPaneView();
	virtual			~CPaneView();

	CWnd*			CreatePane(CWnd* pParentWnd);
	void			UpdatePane();
	void			SetID(int id);
	int				GetID();

	ControlProperty	GetControl()	{return m_Control;}
	
protected:
	virtual CWnd*	OnCreateView() = 0;
	virtual void	OnDestroyView() {};
	virtual void	OnUpdateView() {};

	void			GetControlRect(RECT &rect, ControlProperty& prop, int cx, int cy);

	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg	void	OnTimer(UINT_PTR nIDEvent);
	virtual afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

	int					m_id;
	int					m_updateTimer;

	CXTPToolBar			m_wndToolBar;
	ControlProperty		m_Control;
	BOOL				m_isUseMainControl;

	BOOL				m_isToolBarCreate;
	BOOL				m_isTimerUpdate;
	int					m_iUpdateTime;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PANEVIEW_H__31D830D9_23DE_4FA5_9213_3FC8E271E0CD__INCLUDED_)
