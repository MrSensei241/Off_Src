#ifndef __STDAFX
#define __STDAFX

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#pragma warning(disable:4800)
#pragma warning(disable:4996)
#pragma warning(disable:4819)

// #define GECLIENT
// #define UI_ENABLE

#include <string>
#include <windows.h>
#include <windowsx.h>
#include <imm.h>

#include <string>
#include <list>
#include <algorithm>

#include <d3d9types.h>		//	for definition of vector
#include <d3dx9math.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "ImcCore.h"
#include "imcDirMgr.h"
#include "imcRes.h"

#include "imcStr.h"
#include "imcMath.h"
#include "imcVector.h"
#include "imcIndexedList.h"
#include "imcLinkedList.h"
#include "imcTimer.h"
#include "imcIES.h"
#include "imcLanguage.h"
#include "imcScript.h"
#include "imcinput.h"
#include "imcIME.h"
#include "imcFile.h"

#define ON_MESSAGE(x)	\
	if (imc::StrIsSame(pMessage,#x)) \

#define LIB_INIT(x)		if (!x)	\
{					\
	Error(#x);		\
	return FALSE;	\
}					\

#endif
