#pragma once
#include "resource.h"

class CUpdaterApp : public CWinApp
{
public:
	CUpdaterApp();

	void EnsureFlashPlayer();
	void CheckBlockProcess();
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()

	HANDLE m_mutex;
};

extern bool g_manualy;
extern CString g_fullPath;
extern CString g_gameRootPath;
extern CString g_datafolder;
extern CString g_patchfolder;
extern CString g_datainfofolder;
extern HANDLE g_mmap;