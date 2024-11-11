// MFC3DTestDoc.cpp : implementation of the CMFC3DTestDoc class
//

#include "stdafx.h"
#include "MFC3DTest.h"

#include "MFC3DTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTestDoc

IMPLEMENT_DYNCREATE(CMFC3DTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFC3DTestDoc, CDocument)
	//{{AFX_MSG_MAP(CMFC3DTestDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTestDoc construction/destruction

CMFC3DTestDoc::CMFC3DTestDoc()
{
	// TODO: add one-time construction code here
}

CMFC3DTestDoc::~CMFC3DTestDoc()
{
}

BOOL CMFC3DTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMFC3DTestDoc serialization

void CMFC3DTestDoc::Serialize(CArchive & ar)
{
	if (ar.IsStoring()) {
		// TODO: add storing code here
	} else {
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMFC3DTestDoc diagnostics

#ifdef _DEBUG
void CMFC3DTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFC3DTestDoc::Dump(CDumpContext & dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
