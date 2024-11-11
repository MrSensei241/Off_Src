#pragma once

class CPatcherUpdaterDlg : public CDialog, public CThread
{
public:
	enum PROGRESS_TYPE  { E_BYTE, E_COUNT };
	enum DOWN_LOAD_TYPE { E_DN_TYPE_FAIL, E_DN_TYPE_OK, E_DN_TYPE_EXIST, E_DN_TYPE_REPEAT };
	enum EXCEPTION_TYPE { E_FILE_EXCEPTION, E_ZIP_EXCEPTION, E_IPF_EXCEPTION, E_NONE };
	enum { IDD = IDD_PATCHERUPDATER_DIALOG };

	struct UPDATE_HISTORY
	{
		bool  bSimpleZip;
		DWORD dwHistory;
		DWORD dwMultiNum;
		CString strFileName;
		CString strFilePath;
		CString strSavePath;

		UPDATE_HISTORY() : bSimpleZip(false), dwHistory(0), dwMultiNum(0) { }
		UPDATE_HISTORY(const bool bIsSimpleZip, const DWORD dwHistoryNum, const DWORD dwMultiNumber = 0, const CString name = "") 
		{
			bSimpleZip = bIsSimpleZip;
			dwHistory  = dwHistoryNum;
			dwMultiNum = dwMultiNumber;
			strFileName = name;
		}
	};

	CPatcherUpdaterDlg(CWnd* pParent = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void Run();

	void UpdateProgress(CInternetSession& session);
	bool DownloadHistory(const CString& rFtpFileName, const CString& rLocalFileName);
	DOWN_LOAD_TYPE DownloadProcess(std::vector<UPDATE_HISTORY>& vecHistory, int iStep);
	DOWN_LOAD_TYPE FileDownloadProcess(CDownloader& rDownLoader, bool& rbDownLoaded);
	DOWN_LOAD_TYPE DownloadConnectProcess(CDownloader& rDownLoader, const int iDownLoadType);
	bool AnalysisHistory(const CString& rRevisionFileName, std::vector<UPDATE_HISTORY>& vecHistory);

	void DeleteAllFile(const char*, const char*);
	void CommitUpdater();
	void Execute();

protected:
	std::vector<UPDATE_HISTORY>		m_history;
	CHttpConnection*				m_connection;
	CString							m_path;
	int								m_iTimeOutCount;
	int								m_version;
	int								m_iConnenctTimeOutCount;
	int								m_iUpdateStep;
	int								m_iUpdateStepTotal;
public:
	afx_msg void OnClose();
	CProgressCtrl m_TotalProg;
	CProgressCtrl m_PartProg;
};
