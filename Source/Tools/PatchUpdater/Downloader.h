#ifndef __DOWNLOADER
#define __DOWNLOADER

class CDownloader {
public:
	enum STATE {
		FAIL,
		SUCCESS,
		DOING,
	};
	
public:
						CDownloader();
						~CDownloader();
	bool				Init(const char* httpFileName,const char* localFileName,DWORD blockSize = 8192);
	void				Exit();
	STATE				Do();
	DWORD				GetReceivedByte();
	DWORD				GetTotalByte();	
private:
	CFile*				m_httpFile;
	CFile				m_file;
	BYTE*				m_block;
	DWORD				m_blockSize;
	CInternetSession	m_session;
	DWORD				m_totalBytes;
};

#endif