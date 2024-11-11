#include "StdAfx.h"
#include "Downloader.h"

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

bool CDownloader::Init(const char* httpFileName,const char* localFileName,DWORD blockSize)
{
	TRY {
		m_httpFile = m_session.OpenURL(httpFileName, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		m_totalBytes = m_httpFile->SeekToEnd();
		m_httpFile->SeekToBegin();
	} CATCH (CInternetException, e) {
		e->ReportError();
		e->Delete();
		Exit();
		return false;
	} END_CATCH

	TRY {
		m_file.Open(localFileName, CFile::modeWrite | CFile::modeCreate);
	} CATCH (CFileException, e) {
		e->ReportError();
		e->Delete();
		Exit();
		return false;
	} END_CATCH

	m_block = new BYTE[blockSize];
	ASSERT(NULL!=m_block);

	m_blockSize = blockSize;

	return true;
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

	m_session.Close();
}

CDownloader::STATE CDownloader::Do()
{		
	TRY {
		UINT bytes = m_httpFile->Read(m_block, m_blockSize);
		m_file.Write(m_block, bytes);
	} CATCH(CFileException, e) {
		e->ReportError();
		e->Delete();
		Exit();
		return FAIL;
	} END_CATCH

	if (GetReceivedByte() == GetTotalByte())
		return SUCCESS;

	return DOING;
}

DWORD CDownloader::GetReceivedByte()
{
	return m_file.GetPosition();
}

DWORD CDownloader::GetTotalByte()
{
	return m_totalBytes;
}