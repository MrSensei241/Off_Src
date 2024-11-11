#ifdef STEAM_DLL_EXPORTS
#define DLLFunction _declspec(dllexport)
#else
#define DLLFunction _declspec(dllimport)
#endif

extern "C"{
	DLLFunction int _cdecl Steam_Init_DLL(const char * binPath, std::string& errorCode, void(*steamAPIDebugTextHookFunc)(int, const char *));
	DLLFunction bool _cdecl Steam_GetLoginInfo_DLL(BYTE * rgchToken, uint32& unTokenLen, bool useEncrypt);
	DLLFunction bool _cdecl ClearLoginInfo_DLL();
	DLLFunction UINT64 _cdecl GetSteamID_DLL();
	DLLFunction const char * _cdecl GetNameByID_DLL(UINT64 id);
}

