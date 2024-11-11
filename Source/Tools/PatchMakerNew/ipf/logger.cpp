#include "stdafx.h"
#include "logger.h"

namespace patchMakerLogger {

	std::string GetLogFileName()
	{
		std::string fileName = "PatchMakerLog_Def.txt";

		_finddata_t fd;
		long handle;
		int result = 1;
		handle = _findfirst("log\\*.*", &fd);  //현재 폴더 내 모든 파일을 찾는다.

		if (handle == -1)
		{
			return fileName;
		}

		while (result != -1)
		{
			std::string eachFileName = fd.name;

			if (fileName == "PatchMakerLog_Def.txt")
			{
				fileName = eachFileName;
			}
			else
			{
				if (fileName < eachFileName)
				{
					fileName = eachFileName;
				}
			}

			result = _findnext(handle, &fd);
		}

		_findclose(handle);


		return fileName;
	}

	void Log(const char* Msg, ...)
	{
		char retBuf[24096] = { 0, };
		char paramBuf[24096 - 256] = { 0, };
		va_list args;
		va_start(args, Msg);
		vsprintf_s(paramBuf, 24096 - 256, Msg, args);
		va_end(args);


		sprintf(retBuf, "[%s] \"%s\"", "PATCHMAKER_LOG", paramBuf);

		SYSTEMTIME time;
		GetLocalTime(&time);
		char stringBuf[128];
		sprintf_s(stringBuf, "[%02d/%02d %02d:%02d:%02d] ", time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

		std::string tmpstr = stringBuf;
		tmpstr += retBuf;
		tmpstr += "\n";

		printf("%s", tmpstr.c_str());

		std::string logFileName = GetLogFileName();
		logFileName = "log\\" + logFileName;

		std::ofstream output(logFileName.c_str(), std::ios::app);
		output << tmpstr << std::endl;
		output.close();
	}
}