// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifndef _WIN32_WINNT		// Windows XP 이상에서만 기능을 사용할 수 있습니다.                   
#define _WIN32_WINNT 0x0501	// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#include <stdio.h>
#include <tchar.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#pragma warning (disable:4996)	//strcpy, sprintf, _vsnprintf
#pragma warning (disable:4267)	//casting size_t To int.
#pragma warning (disable:4312)	//'type cast' : conversion from 'int' to 'imc::_STRING_ID' of greater size
#pragma warning (disable:4311)	//'type cast' : pointer truncation from 'int *' to 'int'

#include <Windows.h>
#include <d3dx9.h>

#include "../UnitTest++/src/UnitTest++.h"
#include "../UnitTest++/src/XmlTestReporter.h"
#include "imcCore.h"
#include "imcFile.h"
#include "imcScript.h"
#include "imcMath.h"
#include "imcDirMgr.h"
#include <string>




#define CHECK_ARRAY(pX, pY, szZ) \
	for ( int index = 0; index < static_cast<int>(szZ) ; ++index ) { \
	CHECK( *(pX+index) == *(pY+index) );	\
	}

#define _PERFCHK_
