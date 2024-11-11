// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B835A5E1_82EA_491A_9AC9_2A09C7A16248__INCLUDED_)
#define AFX_STDAFX_H__B835A5E1_82EA_491A_9AC9_2A09C7A16248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <string>
#include <Shlwapi.h>

#include "imcCore.h"
#include "imcIES.h"
#include "IESInternal.h"
#include "imcMap.h"
#include "imcFile.h"
#include "imcRes.h"
#include "imcTimer.h"
#include "imcDirMgr.h"
#include "imcCommandLine.h"
#include "tinyXml.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B835A5E1_82EA_491A_9AC9_2A09C7A16248__INCLUDED_)
