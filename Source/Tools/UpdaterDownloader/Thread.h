#pragma once

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