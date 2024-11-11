// EffectEditorDoc.cpp : implementation of the CEffectEditorDoc class
//

#include "stdafx.h"

#include "app.h"

#include "PaneView.h"


#include "IESManager.h"
#include "IESManagerDoc.h"
#include "IESManagerView.h"
#include "MainFrm.h"

#include "PaneManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEffectEditorDoc

IMPLEMENT_DYNCREATE(CEffectEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CEffectEditorDoc, CDocument)	
//	ON_COMMAND(ID_EFFECT_SAVE, &CEffectEditorDoc::OnFileSaveAs)
//	ON_COMMAND(ID_FILE_EXPORTTOBIN, &CEffectEditorDoc::OnFileExporttobin)
//	ON_COMMAND(ID_FILE_IMPORTTOBIN, &CEffectEditorDoc::OnFileImporttobin)
END_MESSAGE_MAP()

// CEffectEditorDoc construction/destruction

CEffectEditorDoc::CEffectEditorDoc()
{
	// TODO: add one-time construction code here

}

CEffectEditorDoc::~CEffectEditorDoc()
{
}

BOOL CEffectEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_fileFullPath	= "";

	return TRUE;
}


// CEffectEditorDoc serialization

void CEffectEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CEffectEditorDoc diagnostics

#ifdef _DEBUG
void CEffectEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEffectEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEffectEditorDoc commands

BOOL CEffectEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (!m_fileFullPath.GetLength()) 
	{
// 		if (!OnSaveXml())
// 			return FALSE;
	}
	else {
		/*
		if (imcEffect::Util::SaveParticle_XML_FullPath((char*)m_fileFullPath.GetString()) == FALSE)
		{
			AfxMessageBox("XML 파일을 세이브할수 없습니다.");
			return FALSE;
		}
		*/
	}
	return FALSE;
}


BOOL CEffectEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	return FALSE;
}

/*

BOOL CEffectEditorDoc::OnOpenXml()
{
	OPENFILENAME File_Effect,File_TextureInfo;
	memset(&File_Effect,NULL,sizeof(OPENFILENAME));	
	memset(&File_TextureInfo,NULL,sizeof(OPENFILENAME));	

	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();

	char szFile_Effect[128]			=	{0};	
	char szTitle_Effect[64]			=	{"Select Effect XML file"};

	File_Effect.lpstrTitle			=	szTitle_Effect;
	File_Effect.hwndOwner			=	pFrm->GetSafeHwnd();
	File_Effect.hInstance			=	_AtlBaseModule.m_hInst;
	File_Effect.Flags				=	OFN_FILEMUSTEXIST;
	File_Effect.lStructSize			=	sizeof(OPENFILENAME);
	File_Effect.lpstrFile			=	szFile_Effect;
	File_Effect.lpstrFilter			=	"Xml File(*.Xml)\0*.Xml\0\0";
	File_Effect.nMaxFile			=	MAX_PATH;

	if (!GetOpenFileName(&File_Effect))	
		return FALSE;

	if (!imcEffect::LoadEffectList(szFile_Effect))
	{
		AfxMessageBox("XML 파일을 로딩할수 없습니다.");
		return FALSE;
	}

	pFrm->GetEffectListPane()->BuildTree();

	m_fileFullPath = szFile_Effect;
	SetTitle(m_fileFullPath);

	((CMainFrame*)AfxGetMainWnd())->ShowMessage("이펙트가 정상적으로 로딩되었습니다.");

	return TRUE;
}
void CEffectEditorDoc::OnFileSaveAs()
{
	OnSaveXml();		
}

BOOL CEffectEditorDoc::OnSaveXml()
{
	// 이펙트 파일만 저장 한다.
	OPENFILENAME File_Effect;
	memset(&File_Effect,NULL,sizeof(OPENFILENAME));	

	char szFile_Effect[128]		=	{0};		
	char szTitle_Effect[64]		=	{"Select Save Effect XML file"};

	CMainFrame* pFrm			= (CMainFrame*)AfxGetMainWnd();

	File_Effect.lpstrTitle		=	szTitle_Effect;
	File_Effect.hwndOwner		=	pFrm->GetSafeHwnd();
	File_Effect.hInstance		=	_AtlBaseModule.m_hInst;
	File_Effect.Flags			=	OFN_FILEMUSTEXIST;
	File_Effect.lStructSize		=	sizeof(OPENFILENAME);
	File_Effect.lpstrFile		=	szFile_Effect;
	File_Effect.lpstrFilter		=	"All FIles(*.*)\0*.*\0\0";
	File_Effect.nMaxFile		=	MAX_PATH;

	if (!GetSaveFileNameA(&File_Effect))		
		return FALSE;

	if (imcEffect::SaveParticle_XML_FullPath(szFile_Effect) == FALSE)
	{
		AfxMessageBox("XML 파일을 세이브할수 없습니다.");
		return FALSE;
	}

	m_fileFullPath = szFile_Effect;
	SetTitle(m_fileFullPath);
	return TRUE;
}

BOOL CEffectEditorDoc::OnOpenBin()
{
	OPENFILENAME File_Effect,File_TextureInfo;
	memset(&File_Effect,NULL,sizeof(OPENFILENAME));	
	memset(&File_TextureInfo,NULL,sizeof(OPENFILENAME));	

	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();

	char szFile_Effect[128]			=	{0};	
	char szTitle_Effect[64]			=	{"Select Effect Bin file"};

	File_Effect.lpstrTitle			=	szTitle_Effect;
	File_Effect.hwndOwner			=	pFrm->GetSafeHwnd();
	File_Effect.hInstance			=	_AtlBaseModule.m_hInst;
	File_Effect.Flags				=	OFN_FILEMUSTEXIST;
	File_Effect.lStructSize			=	sizeof(OPENFILENAME);
	File_Effect.lpstrFile			=	szFile_Effect;
	File_Effect.lpstrFilter			=	"Binary File(*.Bin)\0*.Bin\0\0";
	File_Effect.nMaxFile			=	MAX_PATH;

	if (!GetOpenFileName(&File_Effect))	
		return FALSE;

	if (imcEffect::LoadParticle_Bin_FullPath(szFile_Effect) == FALSE)
	{
		AfxMessageBox("XML 파일을 로딩할수 없습니다.");
		return FALSE;
	}

	return FALSE;
	
	pFrm->GetEffectListPane()->BuildTree();

	m_fileFullPath = szFile_Effect;
	SetTitle(m_fileFullPath);
	return TRUE;
}

BOOL CEffectEditorDoc::OnSaveBin()
{
	// 이펙트 파일만 저장 한다.
	OPENFILENAME File_Effect;
	memset(&File_Effect,NULL,sizeof(OPENFILENAME));	

	char szFile_Effect[128]		=	{0};		
	char szTitle_Effect[64]		=	{"Select Save Effect Binary file"};

	CMainFrame* pFrm			= (CMainFrame*)AfxGetMainWnd();

	File_Effect.lpstrTitle		=	szTitle_Effect;
	File_Effect.hwndOwner		=	pFrm->GetSafeHwnd();
	File_Effect.hInstance		=	_AtlBaseModule.m_hInst;
	File_Effect.Flags			=	OFN_FILEMUSTEXIST;
	File_Effect.lStructSize		=	sizeof(OPENFILENAME);
	File_Effect.lpstrFile		=	szFile_Effect;
	File_Effect.lpstrFilter		=	"All FIles(*.*)\0*.*\0\0";
	File_Effect.nMaxFile		=	MAX_PATH;

	if (!GetSaveFileNameA(&File_Effect))
		return FALSE;

	if (!imcEffect::SaveParticle_Bin_FullPath(szFile_Effect) ) {
		AfxMessageBox("XML 파일을 세이브할수 없습니다.");
		return FALSE;
	}
	return FALSE;

	m_fileFullPath = szFile_Effect;
	SetTitle(m_fileFullPath);
	return TRUE;
}

void CEffectEditorDoc::OnFileExporttobin()
{
	OnSaveBin();
}

void CEffectEditorDoc::OnFileImporttobin()
{
	OnOpenBin();
}

//void CEffectEditorDoc::OnOpenDocument()
//{
//	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//}
*/