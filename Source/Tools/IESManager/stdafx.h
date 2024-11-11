// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400       // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400     // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINDOWS      // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE           // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400    // Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#pragma warning(disable: 4996) 
#pragma warning(disable: 4099) 
#pragma warning(disable: 4819) 

#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components
#include <XTPResource.h>

#include <winsock2.h>
#include <string>
#include <windows.h>
#include <sqltypes.h>
#include <mmsystem.h>


#include "resource.h"

#define VLD_START_DISABLED // 기본적으로 VLD 는 Disable 한다.
#define VLD_AGGREGATE_DUPLICATES // 동일한 위치의 누수는 한번만 표시한다.

#include "../../ExternLib/VLD/Include/vld.h"


#include <string>
#include <windows.h>
#include <windowsx.h>
#include <imm.h>
#include <assert.h>
#include <stdio.h>

#include "imcCore.h"
#include "imcDebug.h"
#include "imcMath.h"
#include "imcPRS.h"
#include "imcStr.h"
#include "imcTimer.h"
#include "imcDirMgr.h"
#include "imcRandom.h"
#include "imcRes.h"
#include "imcMap.h"
#include "imcIES.h"
#include "imcFile.h"
#include "imcInput.h"

#include "imcd3d9.h"
#include "imcd3d9Util.h"
#include "imcd3d9ToolUtil.h"

#include "imcIME.h"
#include "imcLanguage.h"
#include "imcFont.h"

#include "tinyxml.h"

#include "imcXml.h"
#include "imcIndexedList.h"
#include "imcSortVector.h"
#include "imcPriorityQueue.h"
#include "imcSortVector.h"
#include "imcFile.h"
#include "imcPerfManager.h"

#include "imcMath.h"
#include "imcPRS.h"
#include "imcVisitor.h"
#include "imcRes.h"
#include "imcDelayCounter.h"
#include "imcFSM.h"
#include "imcCommandLine.h"
#include "imcStr.h"
#include "imcBinary.h"
#include "imcRandom.h"
#include "imcTimer.h"
#include "imcCrypt.h"
#include "imcObjPool.h"
#include "imcPool.h"
#include "imcTokenizer.h"
#include "imcFileFind.h"
#include "imcDateTime.h"
#include "imcSafeLongRef.h"
#include "imcPdhQuery.h"
#include "imcDirMgr.h"


#include "imc2d.h"
#include "imc3d.h"
#include "imc3dRenderer.h"

#include "imcSound.h"
#include "imcSoundUtil.h"
#include "imcSortVector.h"


#include "imcIES.h"
#include "imcScript.h"

#include "imcPathEngine.h"

#include "imcinput.h"
#include "imcclientNet.h"
#include "imcd3d9.h"
#include "imcd3d9Util.h"
#include "imcBitmanip.h"
#include "imcScene.h"
#include "imcLanguage.h"
#include "imcFont.h"

#include "imcActor.h"
#include "imcActorEvent.h"



#include "imcActorEvent.h"

#include "geSharedDefine.h"

#include "GeMode.h"
#include "GeModeMgr.h"
#include "imcscene.h"
#include "GeClientShared.h"
#include "GePath.h"
#include "GeConfig.h"
#include "GeApp.h"
#include "GePacketUtil.h"
#include "gePacketTable.h"
#include "geBCPacket.h"
#include "geZCPacket.h"
#include "GeNetUtil.h"


#include "geShared.h"



#include "GeClientShared.h"
#include "GePath.h"
#include "GeConfig.h"
#include "GeActor.h"
#include "GePacketUtil.h"
#include "GeNetUtil.h"
#include "gePacketBase.h"
#include "geClientPacket.h"

