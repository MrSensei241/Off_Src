// MFC3DTest.h : main header file for the MFC3DTEST application
//

#if !defined(AFX_MFC3DTEST_H__37A65FD7_15DE_495F_A0BB_920E4F41902E__INCLUDED_)
#define AFX_MFC3DTEST_H__37A65FD7_15DE_495F_A0BB_920E4F41902E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"   	// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTestApp:
// See MFC3DTest.cpp for the implementation of this class
//

class CMFC3DTestApp : public CWinApp {
public:
	CMFC3DTestApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFC3DTestApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

	// Implementation
	//{{AFX_MSG(CMFC3DTestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC3DTEST_H__37A65FD7_15DE_495F_A0BB_920E4F41902E__INCLUDED_)
