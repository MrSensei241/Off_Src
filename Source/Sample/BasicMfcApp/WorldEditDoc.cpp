// WorldEditDoc.cpp : implementation of the CWorldEditDoc class
//

#include "stdafx.h"
#include "Mfc3dTest.h"

#include "WorldEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldEditDoc

IMPLEMENT_DYNCREATE(CWorldEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CWorldEditDoc, CDocument)
	//{{AFX_MSG_MAP(CWorldEditDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldEditDoc construction/destruction

CWorldEditDoc::CWorldEditDoc()
{
}

CWorldEditDoc::~CWorldEditDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditDoc diagnostics

#ifdef _DEBUG
void CWorldEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWorldEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
