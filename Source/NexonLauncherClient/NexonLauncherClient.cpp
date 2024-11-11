// NexonLauncherClient.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "NexonLauncherClient.h"
#include "nx_api.h"
#include "inxapi.h"

extern "C"
{
	DLLFunction int _cdecl nexon_nxapi_init(const char *productId)
	{
		int result = nxapi_init(productId);
		return result;
	}

	DLLFunction int _cdecl nexon_nxapi_shutdown()
	{
		int result = nxapi_shutdown();
		return result;
	}

	DLLFunction const char * _cdecl nexon_GetProductTicket()
	{
		INXAPI* nxapi = nxapi_get_api();
		if (!nxapi)
			return nullptr;
		const char * productTicket = nxapi->GetProductTicket();
		return productTicket;
	}
}

