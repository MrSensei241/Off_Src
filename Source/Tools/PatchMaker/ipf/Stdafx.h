﻿// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.

#pragma once

#undef _SECURE_SCL	//Warning����. 
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


//���� �ڵ� ������(949)���� ǥ���� �� ���� ���ڰ� ���Ͽ� ��� �ֽ��ϴ�. 
//�����Ͱ� �սǵ��� �ʰ� �Ϸ��� �ش� ������ �����ڵ� �������� �����Ͻʽÿ�.
#pragma warning (disable:4819)

using namespace std;