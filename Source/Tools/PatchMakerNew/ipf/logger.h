// ipf.h



#pragma once



#ifdef PATCH_MAKER
#include <iostream>
#include <fstream>
#include <io.h>
#include <conio.h>
#include <stdio.h>
#endif

#ifdef PATCH_MAKER

namespace patchMakerLogger {

	std::string GetLogFileName();
	void Log(const char* Msg, ...);
}


// 특정 Logger에 설정된 방식대로 메세지를 처리한다
#define IMC_LOG(code, addinfo, ...)					{char temp[24096]; sprintf_s(temp, 24095,  "%s (%s %d Line)", addinfo, __FILE__, __LINE__); \
						patchMakerLogger::Log(temp, __VA_ARGS__);}


#endif
