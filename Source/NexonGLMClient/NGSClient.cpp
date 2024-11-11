// NGSClient.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"

#include "NGSClient.h"
#include "NGClient.h"

NGS::IClient* g_ngsClient = nullptr;
void Control_IMCDLL(PBYTE buf, DWORD bufsize)
{
	if (g_ngsClient)
		g_ngsClient->Control(buf, bufsize);
}

DLLFunction unsigned int	Start_IMCDLL(IN TSendCallback sc, IN bool needPatch)
{
	unsigned int rc = NGS::Start(sc, &g_ngsClient, needPatch);
	return rc;
}
DLLFunction void			Stop_IMCDLL()
{
	NGS::Stop();
}
DLLFunction void			SetNGLUserIDA_IMCDLL(IN const char* userid)
{
	NGS::SetNGLUserIDA(userid);
}
DLLFunction void			SetNGLUserIDW_IMCDLL(IN const wchar_t* userid)
{
	NGS::SetNGLUserIDW(userid);
}
DLLFunction void			GethardwareID_IMCDLL(OUT char * hwid)
{
	NGS::GethardwareID(hwid);
}