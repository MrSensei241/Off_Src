#pragma once

#ifdef NEXONLAUNCHERCLIENT_EXPORTS
#define DLLFunction _declspec(dllexport)
#else
#define DLLFunction _declspec(dllimport)
#endif

extern "C"
{
	DLLFunction int _cdecl nexon_nxapi_init(const char *productId);
	DLLFunction int _cdecl nexon_nxapi_shutdown();
	DLLFunction const char * _cdecl nexon_GetProductTicket();
}