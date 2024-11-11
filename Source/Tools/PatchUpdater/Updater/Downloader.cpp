#include "StdAfx.h"
#include "Downloader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDownloader::CDownloader()
{
	m_httpFile = NULL;
	m_block = NULL;
	m_totalBytes = 0;
}

CDownloader::~CDownloader()
{
	ASSERT(NULL==m_block);
}

CDownloader::STATE CDownloader::Init(CHttpConnection* connection,const char* httpFileName,const char* localFileName,bool continuing,DWORD blockSize)
{
	LINE_MESSAGE_BOX_EX(httpFileName);
	LINE_MESSAGE_BOX_EX(localFileName);

	m_localFileName = localFileName;
	ULONGLONG downloaded = 0;

	if (true == continuing) 
	{
		CFile file;

		if (TRUE == file.Open(localFileName, CFile::modeRead)) 
		{
			downloaded = file.GetLength();
			file.Close();
		}
	}

	try 
	{
		m_httpFile = connection->OpenRequest("GET", httpFileName);

		CString header;

		if (true == continuing) 
			header.Format("Range: bytes=%I64d-\r\n", downloaded);

		if (FALSE == m_httpFile->SendRequest(header)) 
			return FAIL;

		DWORD state;
		if (FALSE == m_httpFile->QueryInfoStatusCode(state)) 
			return FAIL;

		if (300 < state &&
			600 > state &&
			416 != state) 
		{ 
			return FAIL;
		}

		if (416 == state) 
		{
			ASSERT(true==continuing);
			m_totalBytes = downloaded;
		} 
		else 
		{
			DWORD len = 0, retLen = sizeof(DWORD);

			if (FALSE == m_httpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER , &len, &retLen, NULL)) 
				return FAIL;

			m_totalBytes = len;
			m_totalBytes += downloaded;
		}
	} 
	catch (CInternetException* e) 
	{
		if(12002 == e->m_dwError) // 12002 : Connection Timeout
		{	
			e->Delete();
			return REPEAT;
		}

		e->ReportError(MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		e->Delete();
		return FAIL;
	}

	try 
	{
		if (true == continuing) 
		{
			if (FALSE == m_file.Open(localFileName, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate))
				return FAIL;
			m_file.SeekToEnd();
		}
		else 
		{
			if (FALSE == m_file.Open(localFileName, CFile::modeWrite | CFile::modeCreate))
				return FAIL;
		}
	} 
	catch (CFileException* e)
	{
		e->ReportError(MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		e->Delete();
		return FAIL;
	}

	m_block = new BYTE[blockSize];
	ASSERT(NULL!=m_block);

	m_blockSize = blockSize;

	return SUCCESS;
}

void CDownloader::Exit()
{
	if (NULL != m_httpFile)
		m_httpFile->Close();
	m_httpFile = NULL;

	if (CFile::hFileNull != m_file.m_hFile)
		m_file.Close();

	if (NULL != m_block)
		delete [] m_block;
	m_block = NULL;

	m_totalBytes = 0;
}

CDownloader::STATE CDownloader::Do()
{
	try {
		ASSERT(m_file.GetPosition()<=m_totalBytes);
		if (m_file.GetPosition() == m_totalBytes)
			return SUCCESS;
	} catch (CFileException* e) {
		e->ReportError(MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		e->Delete();
		return FAIL;
	}

	UINT bytes = 0;

	try {
		DWORD beginTime = timeGetTime();
		bytes = m_httpFile->Read(m_block, m_blockSize);
		TRACE("time %d\n", timeGetTime()-beginTime);
	} catch (CInternetException* e) {

		if(12002 == e->m_dwError) // 12002 : Connection Timeout
		{
			e->Delete();
			return REPEAT;
		}

		e->ReportError(MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		e->Delete();
		return FAIL;
	}

	try {
		m_file.Write(m_block, bytes);
	} catch (CFileException* e) {
		e->ReportError(MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		e->Delete();
		return FAIL;
	}

	if (m_blockSize != bytes)
		return SUCCESS;

	return DOING;
}

ULONGLONG CDownloader::GetReceivedByte()
{
	ASSERT(CFile::hFileNull!=m_file.m_hFile);

	ULONGLONG received = 0;
	try {
		received = m_file.GetPosition();
	} catch (CFileException* e) {
		e->ReportError(MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		e->Delete();
		return FAIL;
	}

	return received;
}

ULONGLONG CDownloader::GetTotalByte()
{
	ASSERT(NULL!=m_httpFile);
	return m_totalBytes;
}