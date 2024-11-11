// NexonLauncherClient.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
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

