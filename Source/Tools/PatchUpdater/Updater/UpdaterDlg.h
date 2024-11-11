#pragma once

#include "shockwaveflashctrl.h"
#include "noticeDlg.h"
#include "CacheData.h"
#include "DownloadList.h"

const float UPDATER_VERSION = 30.0f;
/////////////////////////////////////////////////////////////////////////////
// CUpdaterDlg dialog

class CDownloader;

#pragma pack(1)
struct PATCH_FILE_HEADER
{
	WORD FilenameLength;
	DWORD CRC32;
	DWORD CompressedSize;			// 기록될 때에 생성된다
	DWORD UnCompressedSize;
};

struct PATCH_DATA_DESCRIPTOR
{
	PATCH_FILE_HEADER header;

	std::string dataname;
	DWORD		Offset;
};

#pragma pack()

class CUpdaterDlg : public CDialog, public CThread
{
public:
	friend class CDeleter;
	friend class CRepairer;

	enum PROGRESS_TYPE  { E_BYTE, E_COUNT };
	enum DOWN_LOAD_TYPE { E_DN_TYPE_FAIL, E_DN_TYPE_OK, E_DN_TYPE_EXIST, E_DN_TYPE_REPEAT };
	enum EXCEPTION_TYPE { E_FILE_EXCEPTION, E_ZIP_EXCEPTION, E_IPF_EXCEPTION, E_NONE };

	struct ERROR_DATA
	{
		CString strErrorMsg;
		EXCEPTION_TYPE eTypeException;

		ERROR_DATA()
		{
			strErrorMsg		= _T("");
			eTypeException	= E_NONE;
		}
	};

private:
	CNoticeDlg			m_noticeDlg;
	BOOL				m_dragging;

	UPDATE_HISTORY_LIST		  m_datadownlist;
	UPDATE_HISTORY_LIST		  m_releasedownlist;
	UPDATE_HISTORY_LIST		  m_preparedownlist;
	UPDATE_HISTORY_LIST		  m_reparedownlistD; //data
	UPDATE_HISTORY_LIST		  m_reparedownlistR; //release

	CHttpConnection*	m_connection;
	CString				m_path;
	int					m_iUpdateStep;			 // 업데이트 진행 상황 표시 변수 
	int					m_iUpdateStepTotal;		 // 업데이트 전체수 표시 변수 
	int					m_iWnuUpPartProg;		 // 업데이터 부분 프로그래스 관련 메시지 인덱스 
	int					m_iWnuUpPartProgByte;	 // 업데이터 부분 프로그래스 바이트 관련 메시지 인덱스 
	int					m_bNowMerging;			 // 파일 머징중인지 표시 플래그 
	int					m_iTimeOutCount;		 // 다운로드 TimeOut 카운트 수	
	int					m_iConnenctTimeOutCount; // 다운로드 서버 연결 TimeOut 카운트 수	
	bool				m_bNowUpdate;			 // 업데이트 진행중인지 표시 플래그 
	bool				m_bOptimize;			 // 중복 파일 정리 중인지 표시 플래그 
	bool				m_bExchangeFileName;	 // 새로 묶는 ipf 파일의 이름을 변경하고 있는지 표시 플래그 
	bool				m_bRequestedOptimize;	 // 중복 파일 정리 요청이 있는지 표시 플래그 	
	bool				m_bRequestedRepair;
	bool				m_bRepair;
	bool				m_bExportPak;
	bool				m_bOptimizeStop;
	bool				m_bPause;

	int					m_totalGauge;
	int					m_workGauge;

	std::vector<int> m_patchlist;
	CCacheData m_CacheDataClass;
	int ipfcount;

public:
	void Run();
	void SetVersion();
	bool InspectUpdaterVersion(CInternetSession& session);
 	bool DownloadHistory(const CString& rFtpFileName, const CString& rLocalFileName);
	
	void TotalUpdateProgress(CInternetSession& session, bool pre = true);
	void NormalUpdateProgress(CInternetSession& session);
	void ReleaseUpdateProgress(CInternetSession& session);
	void PrepareUpdateProgress(CInternetSession& session);
	void RepairUpdateProgress(CInternetSession& session);

	void SetTotalProgress(int per);														// 전체 업데이트 진행 상황 그래픽으로 보여줌(퍼센트에 의해)
	void SetPartWorkProgress(int per);													// 부분 업데이트 진행 상황 그래픽으로 보여줌(퍼센트에 의해)
	void SetPartWorkProgressGraphic(const UINT uCurrent, const UINT uMaximum);			// 부분 업데이트 진행 상황 그래픽으로 보여줌
	void SetWorkExplain(const char* work);												// 부분 업데이트 진행 상황 글로 설명(갱신중, 병합중등)
	void SetWorkingExplain(const CString strExplain, const CString strSecond = _T(""));	// 부분 업데이트 진행 상황 글로 설명(갱신중, 병합중등) - 파일 이름 같이 출력 가능
	void SetPartWorkProgress(const UINT iCurrent, const UINT iTotal);
	void SetPartWorkProgress(const char* str);
	void SetUpdaterVersion(const float version);
	void SetUpdateCurrentRevision(const int version);
	CString GetSpecificStringFromPath(const char* pFileName);
	const char* GetButtonText();

	void Execute();
	void ExchangeIpfFileName(const CString& filePath, const CString& fileName);
	void TokenizeString(std::vector<CString>& rvecString, const CString& rStrOrg, const CString& rToken);
	CString GetFileExceptionError(CFileException* exception);
	DOWN_LOAD_TYPE DownloadRevisionsProcess(UPDATE_HISTORY_LIST& vecHistory, int iStep);
	DOWN_LOAD_TYPE DownloadConnectProcess(CDownloader& rDownLoader, const int iDownLoadType);
	DOWN_LOAD_TYPE FileDownloadProcess(CDownloader& rDownLoader, bool& rbDownLoaded);

	void DownloadRevisionFile(CInternetSession& session, const CString& rFtpFileName, const CString& rLocalFileName);
	void MakeDownloadList(DWORD lastversion, CInternetSession& session, const CString& rFtpFileName, DownloadListMaker& rMaker, UPDATE_HISTORY_LIST& vecHistory);

	bool Repair(CInternetSession& session);
	void OptimizeIpfFiles();
	void UpdaterExecute();
	void DeleteAllFile(const char*);
	void DeleteAllFile(const char*, const char*);
	void ExportPakFile(const char*);

	bool IsOptimizeStop() {return m_bOptimizeStop;}
	bool IsPause() {return m_bPause;}

	void DeleteFolder(const char* lpFolder);

private:
	LRESULT OnUpdateWorkExplain(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdatePartWorkProgress(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdatePartWorkProgressBYTE(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdatePartWorkProgressCount(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateTotalWorkProgress(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateButtonText(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateUpdaterVersion(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateCurrentRevision(WPARAM wParam, LPARAM lParam);

public:
	CUpdaterDlg(CWnd* pParent = NULL);	// standard constructor

	enum { IDD = IDD_UPDATER_DIALOG };
	CShockwaveFlashCtrl	m_flashCtrl;

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	HICON m_hIcon;

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFSCommandShockwaveflash(LPCTSTR command, LPCTSTR args);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	DECLARE_EVENTSINK_MAP()
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
};
