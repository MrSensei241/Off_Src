// Mfc3dTest.h : main header file for the Mfc3dTest application
//

#if !defined(AFX_Mfc3dTest_H__5DDA0BC7_7306_450A_922A_4A321FC665C0__INCLUDED_)
#define AFX_Mfc3dTest_H__5DDA0BC7_7306_450A_922A_4A321FC665C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp:
// See Mfc3dTest.cpp for the implementation of this class
//

class CWorldEditApp : public CWinApp {
public:
	CWorldEditApp();
	CMultiDocTemplate* m_pDocTemplate;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldEditApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWorldEditApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Mfc3dTest_H__5DDA0BC7_7306_450A_922A_4A321FC665C0__INCLUDED_)
