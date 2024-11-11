// Mfc3dTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Mfc3dTest.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp

BEGIN_MESSAGE_MAP(CWorldEditApp, CWinApp)
	//{{AFX_MSG_MAP(CWorldEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp construction

CWorldEditApp::CWorldEditApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWorldEditApp object

CWorldEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp initialization

BOOL CWorldEditApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("3d World Editor"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	
	// Register document templates

	m_pDocTemplate = new CMultiDocTemplate(
		IDR_WORLDTYPE,
		RUNTIME_CLASS(CWorldEditDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CWorldEditView));
	AddDocTemplate(m_pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// create d3d
	imc::CDirSet curDir(imc::eCodeBaseDir);
	imc::CDirMgr * pDirMgr = imc::GetDirMgr();
	pDirMgr->SetCurDir(imc::eShaderDir, pDirMgr->GetSlotPath(imc::eCodeBaseDir, imc::DIR_DEFAULT));

	if (!imc3d::Init())
		return FALSE;
	imcresmgr::Init();

	imc3d::IDevice * pDev = imc3d::GetDevice();
	if (!pDev->Init(pMainFrame->GetSafeHwnd(), TRUE, 1280, 1024, 32))
		Error("cannot create 3d device");

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CWorldEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp message handlers


int CWorldEditApp::ExitInstance() 
{
	imc3d::GetDevice()->Exit();
	imcresmgr::Exit();
	imc3d::Exit();
	return CWinApp::ExitInstance();
}
