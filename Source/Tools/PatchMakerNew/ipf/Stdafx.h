// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.

#pragma once

#undef _SECURE_SCL	//Warning삭제. 
#define _SECURE_SCL 0
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <windows.h>
#include <mmSystem.h>
#include <zlib.h>
#include <map>
#include <algorithm>


//현재 코드 페이지(949)에서 표시할 수 없는 문자가 파일에 들어 있습니다. 
//데이터가 손실되지 않게 하려면 해당 파일을 유니코드 형식으로 저장하십시오.
#pragma warning (disable:4819)

using namespace std;