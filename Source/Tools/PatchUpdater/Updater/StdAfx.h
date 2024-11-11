#pragma once

#pragma warning(disable:4786)
#pragma warning(disable:4800)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxinet.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
	#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <Windows.h>
#include <mmsystem.h>
#include <process.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <shlwapi.h>

#include <afxdisp.h>

#define SAFE_DELETE(p)  { if(p) { delete (p); (p) = NULL; } }

inline BOOL FatalError(LPCSTR lpszFileName, int nLine, LPCSTR lpszComment = "")
{
	MSG msg;
	BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);

	CString temp = _T("");

#ifdef _DEBUG
	temp.Format("%s %d\n\n%s", lpszFileName, nLine, lpszComment);
#else
	temp.Format("%s (%d)", lpszComment, nLine);
#endif

	AfxMessageBox(temp, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);
	
	if (bQuit)
		PostQuitMessage((int)msg.wParam);
	
	return TRUE;
}

template <typename T>
inline void FATAL_ERROR(const T f, LPCSTR lpszFileName, int nLine, LPCSTR lpszComment = "")
{
	if(f)
	{
	} 
	else 
	{
		FatalError(lpszFileName, nLine, lpszComment);
		ExitProcess(0);
	}
}

class CThread {
public:
	CThread()
	{
		m_handle = INVALID_HANDLE_VALUE; 
		m_stop = false;
	}

	virtual ~CThread()
	{
	}

	HANDLE GetHandle()
	{
		return m_handle;
	}

	virtual bool Start()
	{
		FATAL_ERROR(INVALID_HANDLE_VALUE==m_handle, THIS_FILE, __LINE__, "Invalid Handle Value");
		m_handle = (HANDLE)::_beginthread(ThreadFunction, 0, (void*)this);		
		if (m_handle == INVALID_HANDLE_VALUE)
			return false;
		return true;
	}

	virtual void Stop(DWORD time = INFINITE)
	{
		FATAL_ERROR(INVALID_HANDLE_VALUE!=m_handle, THIS_FILE, __LINE__, "Invalid Handle Value");

		DWORD timeout = timeGetTime() + time;
		m_stop = true;

		while (TRUE) {
			if (false == m_stop)
				break;

			if (timeout != INFINITE) {
				if (timeGetTime() > timeout)
					break;
			}

			if (INVALID_HANDLE_VALUE == m_handle)
				break;

			Sleep(2);
		}
	}

	void Suspend()
	{
		FATAL_ERROR(INVALID_HANDLE_VALUE!=m_handle, THIS_FILE, __LINE__, "Invalid Handle Value");
		FATAL_ERROR(-1!=SuspendThread(m_handle), THIS_FILE, __LINE__, "Suspend Count Not Valid");
	}

	void Resume()
	{
		FATAL_ERROR(INVALID_HANDLE_VALUE!=m_handle, THIS_FILE, __LINE__, "Invalid Handle Value");
		FATAL_ERROR(-1!=ResumeThread(m_handle), THIS_FILE, __LINE__, "Resume Thread Failed");
	}

private:
	static void __cdecl ThreadFunction(void* p)
	{
		CThread* thread = (CThread*)p;
		if (NULL != thread) {
			thread->Run();
			thread->m_handle = INVALID_HANDLE_VALUE;
			thread->m_stop = false;
		}
	}

protected:
	virtual void Run() = 0;

protected:
	bool		m_stop;
	HANDLE		m_handle;
};

inline void ReportLastErrorExit(const char* pos)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	lpDisplayBuf = LocalAlloc(LMEM_ZEROINIT, strlen((const char*)lpMsgBuf)+strlen(pos)+40); 
	wsprintf((char*)lpDisplayBuf, "%s failed with error %d: %s", pos, dw, (const char*)lpMsgBuf); 
	MessageBox(NULL, (const char*)lpDisplayBuf, "Error", MB_OK | MB_SETFOREGROUND | MB_TOPMOST); 

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

#ifdef _DEBUG
	//{
	DebugBreak();
	//}
#endif

	ExitProcess(dw);
}

inline bool IsExistFile(const char* name)
{
	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(name, &data);

	if (INVALID_HANDLE_VALUE == handle) 
	{
		switch (GetLastError()) 
		{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			return false;
		default:
			{
				CString strComment;
				strComment.Format("find %s\n\n", name);
				ReportLastErrorExit(strComment);
			}
		}
	}
	FindClose(handle);
	return true;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

const int MERGE_BUFFER_SIZE		   = 1024*1024; //1메가
const int TIMEOUT_TIME			   = 5000;
const int TIMEOUT_REPEAT_MAX_COUNT = 5;

#define IPF_FOLDER_NAME				"IPF_FOLDER"
#define	IPF_FILE_LIST_NAME			"ipf_file_list.txt"
#define	IPF_REVISION_TEXT			"ipf_revision"

#define	VISION_FILE_NAME			"version.txt"
#define	RELEASE_VISION_FILE_NAME	"release.revision.txt"
#define	REVISION_FILE_NAME			"revision.txt"
#define	PREPARE_REVISION_FILE_NAME	"prepare_revision_v2.txt"
#define PREFIX_COPYING				".copying"
#define PREFIX_BACKUP				".backup"
#define PREFIX_ARRANGE				".arrange"
#define PREFIX_SOURCE				".source"
#define PREFIX_TEMP					".temp"
#define PREFIX_IPF					".ipf"

#define	PRE_REVISION_FILE_NAME		"pre.revision.txt"
#define	DATA_REVISION_FILE_NAME		"data.revision.txt"
#define	RELEASE_REVISION_FILE_NAME	"release.revision.txt"
#define	DATA_LIST_FILE_NAME			"data.file.list.txt"
#define	FULL_DATA_REVISION_FILE_NAME	"full.data.revision.txt"
#define	FULL_RELEASE_REVISION_FILE_NAME	"full.release.revision.txt"

#define FULL_DIRECTORY				"full/"
#define PARTIAL_DIRECTORY			"partial/"
#define UPDATER_DIRECTORY			"updater/"

#define DATA_DOWNLOAD				"data/"
#define RELEASE_DOWNLOAD			"release/"

#define UPDATER_FILE_NAME			"updater"

#define UPDATER_DOWNLOADER_EXE		"Updater_Downloader.exe"
#define UPDATER_VERSION_FILE_NAME	"patcher.version.txt"



#define LINE_MESSAGE_BOX_EX(msg, ...)\
{\
	char MsgBuf[1024]; \
	va_list args; \
	va_start(args, msg); \
	vsprintf_s(MsgBuf, 1024, msg, args); \
	va_end(args); \
	char _tempBuf[1024]; \
	sprintf(_tempBuf, "%s %s_%d", MsgBuf, __FILE__, __LINE__); \
	AfxMessageBox(_tempBuf, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);  }\

#define LINE_MESSAGE_BOX \
	\
{char _tempBuf[128]; \
	sprintf(_tempBuf, "%s_%d", __FILE__, __LINE__); \
	AfxMessageBox(_tempBuf, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);  }\


