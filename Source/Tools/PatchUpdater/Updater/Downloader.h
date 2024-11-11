#pragma once

class CDownloader {
public:
	enum STATE {
		FAIL,
		SUCCESS,
		DOING,
		REPEAT
	};
	
public:
						CDownloader();
						~CDownloader();
	STATE				Init(CHttpConnection* connection,const char* httpFileName,const char* localFileName,bool continuing,DWORD blockSize = 8192);
	void				Exit();
	STATE				Do();
	ULONGLONG			GetReceivedByte();
	ULONGLONG			GetTotalByte();
	std::string			m_localFileName;

private:
	CHttpFile*			m_httpFile;
	CFile				m_file;
	BYTE*				m_block;
	DWORD				m_blockSize;
	ULONGLONG			m_totalBytes;
};