// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.


#include <string>
#include <windows.h>
#include <mmsystem.h>

#include "tinyXml.h"

#include "imcFSM.h"
#include "imcBounceCalculator.h"
#include "imcBitmanip.h"
#include "imcCore.h"
#include "imcSortVector.h"
#include "imcMath.h"
#include "imcFile.h"
#include "imcStr.h"
#include "imcTimer.h"
#include "imcMap.h"
#include "imcVector.h"
#include "imcIndexedList.h"
#include "imcStringTable.h"
#include "imcDirMgr.h"
#include "imcRes.h"
#include "imcXml.h"
#include "imcPRS.h"

#include "imcScript.h"
#include "imcCrypt.h"
#include "imcd3d9.h"
#include "imcd3d9Util.h"
#include "imcinput.h"
#include "imcScene.h"
#include "imcPerfManager.h"
#include "imcPathEngine.h"
#include "imcclientNet.h"

#include "imcActorEvent.h"

#include "ldClientShared.h"

#define ASIGN_STATE_STR(x)	{x, #x}