/***
*
*	Copyright (c) 2003, IMC. All rights reserved.
*
****/

#include "MAX.H"
#include "DECOMP.H"
#include "STDMAT.H"
#include "ANIMTBL.H"
#include "istdplug.h"
#include "phyexp.h"
#include <maxtypes.h>
#include <XRef/iXrefObj.h>

#include "resource.h"
#include "ObjLocExp.h"

#include "imcXml.h"
#include "imcStr.h"
#include "imcVector.h"
#include "imcCommandLine.h"
#include "tinyxml.h"

//===================================================================
// Prototype declarations
//
int GetIndexOfINode(INode *pnode,BOOL fAssertPropExists = TRUE);
void SetIndexOfINode(INode *pnode, int inode);
BOOL FUndesirableNode(INode *pnode);
BOOL FNodeMarkedToSkip(INode *pnode);
float FlReducenml_Rot(float fl);


bool ExtractNETName(const char* objName, char type, char* netName);


//===================================================================
// Global variable definitions
//

// Save for use with dialogs
static HINSTANCE hInstance;

// We just need one of these to hand off to 3DSMAX.
static CObjLocExportDesc CObjLocExportCD;

// For OutputDebugString and misc sprintf's
static char st_szDBG[300];

// INode mapping table
static int g_inmMac = 0;

//===================================================================
// Utility functions
//

static int AssertFailedFunc(char *sz)
{
	MessageBox(GetActiveWindow(), sz, "Assert failure", MB_OK);
	int Set_Your_Breakpoint_Here = 1;
	return 1;
}
#define ASSERT_MBOX(f, sz) ((f) ? 1 : AssertFailedFunc(sz))


//===================================================================
// Required plug-in export functions
//
BOOL WINAPI DllMain( HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved) 
{	
	static int fFirstTimeHere = TRUE;
	if (fFirstTimeHere)
	{
		fFirstTimeHere = FALSE;
		hInstance = hinstDLL;
	}
	return TRUE;
}

	
EXPORT_THIS int LibNumberClasses(void)
{
	return 1;
}

	
EXPORT_THIS ClassDesc *LibClassDesc(int iWhichClass)
{
	switch(iWhichClass)
	{
		case 0: return &CObjLocExportCD;
		default: return 0;
	}
}

	
EXPORT_THIS const TCHAR *LibDescription()
{
	return _T("IMC XRef export");
}

	
EXPORT_THIS ULONG LibVersion()
{
	return VERSION_3DSMAX;
}


//=====================================================================
// Methods for CObjLocExport
//

CONSTRUCTOR CObjLocExport::CObjLocExport(void)
{
	m_rgNodeExpInfo = NULL;
}


DESTRUCTOR CObjLocExport::~CObjLocExport(void)
{
}


int CObjLocExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options) 
{
	ExpInterface	*pexpiface = ei;	// Hungarian
	Interface		*piface = i;		// Hungarian
	
	// Reset the name-map property manager
	g_inmMac = 0;

	// Present the user with the Export Options dialog
//	if (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_OBJLOCFORMAT), GetActiveWindow(),
//						ExportObjLocFormatDlgProc, (LPARAM)this) <= 0)
//		return 0;		// error or cancel

	// Break up filename, re-assemble longer versions
	TSTR strPath, strFile, strExt;
	TCHAR szFile[MAX_PATH];
	SplitFilename(TSTR(name), &strPath, &strFile, &strExt);

	sprintf(szFile,  "%s\\%s.%s",  (char*)strPath, (char*)strFile, "objgroup");

	// Count nodes, label them, collect into array
	if (!CollectNodes(pexpiface))
		return 0;	/*fail*/

	m_iCurNode = 0;
	// Output nodes
	if (!DumpNodes(pexpiface))
	{
		return 0;	/*fail*/
	}

	SaveDocument( szFile );

	// Tell user that exporting is finished (it can take a while with no feedback)
	char szExportComplete[300];
	sprintf(szExportComplete, "Exported %s.", szFile);
	MessageBox(GetActiveWindow(), szExportComplete, "Status", MB_OK);


	if( m_rgNodeExpInfo ) {
		delete [] m_rgNodeExpInfo;
		m_rgNodeExpInfo = NULL;
	}
	

	return 1/*success*/;
}

BOOL CObjLocExport::SaveDocument(LPCTSTR lpszPathName)
{
	int		i;
	TiXmlDocument		saveFile;
	TiXmlDeclaration	decl("1.0", "", "yes");
	TiXmlElement		world("Instances");
	world.SetAttribute("Name", "[Group]");
	world.SetAttribute("Opened", "false");
	for( i=0; i<m_iNodeExpInfoMac; i++ ) {
		CObjLocExport::NodeExpInfo* pExpInfo = &m_rgNodeExpInfo[i];
		if(pExpInfo->type==CObjLocExport::NETYPE_NORMAL) {
			TiXmlElement	elem("Instance");
			char				tmp[1024];
			
			elem.SetAttribute("Path",  pExpInfo->nml_RefObjName);
			elem.SetAttribute("Template", pExpInfo->nml_ShdObjName);
			elem.SetAttribute("Name", pExpInfo->nml_ShdMapName);
			sprintf( tmp, "%f %f %f", 
				pExpInfo->nml_Pos.x, 
				pExpInfo->nml_Pos.z, 
				pExpInfo->nml_Pos.y );
			elem.SetAttribute("pos", tmp);
			sprintf( tmp, "%f %f %f %f", 
				pExpInfo->nml_Rot.x, 
				pExpInfo->nml_Rot.z, 
				pExpInfo->nml_Rot.y, 
				pExpInfo->nml_Rot.w);
			elem.SetAttribute("rot", tmp);
			sprintf( tmp, "%f %f %f", 
				pExpInfo->nml_Scl.x, 
				pExpInfo->nml_Scl.z, 
				pExpInfo->nml_Scl.y);
			elem.SetAttribute("scale", tmp);
			world.InsertEndChild(elem);
		}
		else if(pExpInfo->type==CObjLocExport::NETYPE_SPEEDTREE) {
			TiXmlElement	elem("Tree");
			char				tmp[1024];
			elem.SetAttribute("File",  pExpInfo->spt_Name);
			sprintf( tmp, "%f %f %f", 
				pExpInfo->spt_Pos.x, 
				pExpInfo->spt_Pos.z, 
				pExpInfo->spt_Pos.y );
			elem.SetAttribute("pos",  tmp);
			world.InsertEndChild(elem);
		}
		else if(pExpInfo->type==CObjLocExport::NETYPE_EFFECT) {
			TiXmlElement	elem("Effect");
			char				tmp[1024];
			elem.SetAttribute("Name",  pExpInfo->eff_Name);
			sprintf( tmp, "%f %f %f", 
				pExpInfo->eff_Pos.x, 
				pExpInfo->eff_Pos.z, 
				pExpInfo->eff_Pos.y );
			elem.SetAttribute("pos",  tmp);
			sprintf( tmp, "%f %f %f %f", 
				pExpInfo->eff_Rot.x, 
				pExpInfo->eff_Rot.z, 
				pExpInfo->eff_Rot.y, 
				pExpInfo->eff_Rot.w);
			elem.SetAttribute("rot", tmp);
			sprintf( tmp, "%f %f %f", 
				pExpInfo->eff_Scl.x, 
				pExpInfo->eff_Scl.z, 
				pExpInfo->eff_Scl.y);
			elem.SetAttribute("scl", tmp);
			world.InsertEndChild(elem);
		}
		else if(pExpInfo->type==CObjLocExport::NETYPE_SOUND) {
			TiXmlElement	elem("Sound");
			char				tmp[1024];
			elem.SetAttribute("File",  pExpInfo->snd_Name);
			sprintf( tmp, "%f %f %f", 
				pExpInfo->snd_Pos.x, 
				pExpInfo->snd_Pos.z, 
				pExpInfo->snd_Pos.y );
			elem.SetAttribute("pos",  tmp);

			imc::StrFormat(tmp, "%f", pExpInfo->snd_Vol);
			elem.SetAttribute("vol", tmp);

			imc::StrFormat(tmp, "%f", pExpInfo->snd_Rad);
			elem.SetAttribute("rad", tmp);

			world.InsertEndChild(elem);
		}
	}
	
	saveFile.InsertEndChild(decl);
	saveFile.InsertEndChild(world);
	saveFile.SaveFile(lpszPathName);
	
	return TRUE;        // success

	
}
	
	
BOOL CObjLocExport::CollectNodes( ExpInterface *pexpiface)
{
	// Count total nodes in the model, so I can alloc array
	// Also "brands" each node with node index, or with "skip me" marker.
	CountNodesTEP procCountNodes;
	procCountNodes.m_cNodes = 0;
	(void) pexpiface->theScene->EnumTree(&procCountNodes);
	ASSERT_MBOX(procCountNodes.m_cNodes > 0, "No nodes!");

	// Alloc and fill array
	m_iNodeExpInfoMac = procCountNodes.m_cNodes;
	m_rgNodeExpInfo = new NodeExpInfo[m_iNodeExpInfoMac];
	::ZeroMemory(m_rgNodeExpInfo, sizeof(NodeExpInfo)*m_iNodeExpInfoMac);
	

	return TRUE;
}

BOOL CObjLocExport::DumpNodes(ExpInterface *pexpiface)
{
	// Dump bone names
	DumpNodesTEP procDumpNodes;
	procDumpNodes.m_phec = this;

	(void) pexpiface->theScene->EnumTree(&procDumpNodes);

	return TRUE;
}

//=============================================================================
//							TREE-ENUMERATION PROCEDURES
//=============================================================================

#define ASSERT_AND_ABORT(f, sz)							\
	if (!(f))											\
	{													\
		ASSERT_MBOX(FALSE, sz);							\
		cleanup( );										\
		return TREE_ABORT;								\
	}


//=================================================================
// Methods for CountNodesTEP
//
int CountNodesTEP::callback( INode *node)
{
	INode *pnode = node; // Hungarian

	ASSERT_MBOX(!(pnode)->IsRootNode(), "Encountered a root node!");

	if (::FUndesirableNode(pnode))
	{
		// Mark as skippable
		::SetIndexOfINode(pnode, CObjLocExport::UNDESIRABLE_NODE_MARKER);
		return TREE_CONTINUE;
	}
	
	// Establish "node index"--just ascending ints
	::SetIndexOfINode(pnode, m_cNodes);

	m_cNodes++;
	
	return TREE_CONTINUE;
}


// #define DEBUG_MESH_DUMP

//===========================================================
// Dialog proc for export options
//
static BOOL CALLBACK ExportObjLocFormatDlgProc(
	HWND	hDlg,
	UINT	message,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static CObjLocExport *pexp;
	switch (message)
	{
	case WM_INITDIALOG:
		pexp = (CObjLocExport*) lParam;
		CheckRadioButton(hDlg, IDC_FORMAT_TXT, IDC_FORMAT_BIN, IDC_FORMAT_XML);
		return FALSE;
	case WM_DESTROY:
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, 1);		// 1 indicates "ok to export"
			return TRUE;
		case IDCANCEL:				// 0 indicates "cancel export"
			EndDialog(hDlg, 0);
			return TRUE;
		case IDC_FORMAT_TXT:
			CheckRadioButton(hDlg, IDC_FORMAT_TXT, IDC_FORMAT_BIN, IDC_FORMAT_TXT);
			break;
		case IDC_FORMAT_XML:
			CheckRadioButton(hDlg, IDC_FORMAT_TXT, IDC_FORMAT_BIN, IDC_FORMAT_XML);
			break;
		case IDC_FORMAT_BIN:
			CheckRadioButton(hDlg, IDC_FORMAT_TXT, IDC_FORMAT_BIN, IDC_FORMAT_BIN);
			break;
		}
	}
	return FALSE;
}



//========================================================================
// Utility functions for getting/setting the personal "node index" property.
// NOTE: I'm storing a string-property because I hit a 3DSMax bug in v1.2 when I
// NOTE: tried using an integer property.
// FURTHER NOTE: those properties seem to change randomly sometimes, so I'm
// implementing my own.

typedef struct
{
	char	szNodeName[CObjLocExport::MAX_NAME_CHARS];
	int		iNode;
} NAMEMAP;
const int MAX_NAMEMAP = 8192;
static NAMEMAP g_rgnm[MAX_NAMEMAP];

int GetIndexOfINode(INode *pnode, BOOL fAssertPropExists)
{
	TSTR strNodeName(pnode->GetName());
	for (int inm = 0; inm < g_inmMac; inm++)
		if (FStrEq(g_rgnm[inm].szNodeName, (char*)strNodeName))
			return g_rgnm[inm].iNode;
	if (fAssertPropExists)
		ASSERT_MBOX(FALSE, "No NODEINDEXSTR property");
	return -7777;
}

	
void SetIndexOfINode(INode *pnode, int inode)
{
	TSTR strNodeName(pnode->GetName());
	NAMEMAP *pnm;
	int inm = 0;
	for ( inm = 0; inm < g_inmMac; inm++)
		if (FStrEq(g_rgnm[inm].szNodeName, (char*)strNodeName))
			break;
	if (inm < g_inmMac)
		pnm = &g_rgnm[inm];
	else
	{
		ASSERT_MBOX(g_inmMac < MAX_NAMEMAP, "NAMEMAP is full");
		pnm = &g_rgnm[g_inmMac++];
		strcpy(pnm->szNodeName, (char*)strNodeName);
	}
	pnm->iNode = inode;
}

//=============================================================
// Returns TRUE if a node should be ignored during tree traversal.
//
BOOL FUndesirableNode(INode *pnode)
{
	// Get Node's underlying object, and object class name
	Object *pobj = pnode->GetObjectRef();

	// Don't care about lights, dummies, and cameras
	if (pobj->SuperClassID() == CAMERA_CLASS_ID)
		return TRUE;
	if (pobj->SuperClassID() == LIGHT_CLASS_ID)
		return TRUE;

	return FALSE;

	// Actually, if it's not selected, pretend it doesn't exist!
	//if (!pnode->Selected())
	//	return TRUE;
	//return FALSE;
}


//=============================================================
// Returns TRUE if a node has been marked as skippable
//
BOOL FNodeMarkedToSkip(INode *pnode)
{
	return (::GetIndexOfINode(pnode) == CObjLocExport::UNDESIRABLE_NODE_MARKER);
}

	
//=============================================================
// Reduces a nml_Rot to within the -2PI..2PI range.
//
static float FlReducenml_Rot(float fl)
{
	while (fl >= TWOPI)
		fl -= TWOPI;
	while (fl <= -TWOPI)
		fl += TWOPI;
	return fl;
}

//=================================================================
// node export type 이름을 추출한다.
bool ExtractNETName(const char* objName, char type, char* netName)
{
	if(objName[0]!='@')		return false;
	if(objName[1]!=type)	return false;

	int nameLen = strlen(objName);
	for(int i=3; i<nameLen; ++i) {
		if(objName[i]==']') {
			int wordLen = i - 3;
			strncpy(netName, &objName[3], wordLen);
			netName[wordLen] = '\0';
		}
	}

	return true;
}

//=================================================================
// Methods for DumpNodesTEP
//
// 참고 : speed tree 의 class id = Class_ID(0x49914863, 0x2bb77810) 인 듯
//			VC 디버깅으로 알아낸 값으로 undocumented
int DumpNodesTEP::callback(INode *pnode)
{
	ASSERT_MBOX(!(pnode)->IsRootNode(), "Encountered a root node!");

	CObjLocExport::NodeExpInfo* pExpInfo = &m_phec->m_rgNodeExpInfo[m_phec->m_iCurNode];

	IXRefObject *ix;
	Object *pobj;
	AffineParts affparts;	
	decomp_affine(pnode->GetNodeTM(0), &affparts);
	TSTR	strNodeName;
	Class_ID classID;
	
	if (::FNodeMarkedToSkip(pnode)) {
		pExpInfo->type = CObjLocExport::NETYPE_INVALID;
		goto __End;
	}

	if(pnode->IsHidden()) {
		pExpInfo->type = CObjLocExport::NETYPE_INVALID;
		goto __End;
	}
	
	// 일반 오브젝트의 경우 XRef로 배치된 정보만을 사용한다.
	// The model's root is a child of the real "scene root"	
	pobj = pnode->GetObjectRef();
	classID = Class_ID(XREFOBJ_CLASS_ID );
	if(pobj->ClassID() != classID ) {
		pExpInfo->type = CObjLocExport::NETYPE_INVALID;
		goto __End;
	}	
	
	if(pobj->SuperClassID()==SYSTEM_CLASS_ID) {
		ix = (IXRefObject *)pobj;
		strNodeName = ix->GetObjName(FALSE);
		 
		if(strNodeName[0]=='@' && strNodeName[1]=='T') {	// case of NETYPE_SPEEDTREE		
			pExpInfo->type = CObjLocExport::NETYPE_SPEEDTREE;
			ExtractNETName(pnode->GetName(), 'T', pExpInfo->spt_Name);
			strcat(pExpInfo->spt_Name, ".spt");
			pExpInfo->spt_Pos = affparts.t;		
		}
		else if(strNodeName[0]=='@' && strNodeName[1]=='E') {	// case of NETYPE_EFFECT	
			pExpInfo->type = CObjLocExport::NETYPE_EFFECT;
			ExtractNETName(pnode->GetName(), 'E', pExpInfo->eff_Name);			
			pExpInfo->eff_Pos = affparts.t;
			pExpInfo->eff_Scl = affparts.k;
			pExpInfo->eff_Rot = affparts.q;
		}
		else if(strNodeName[0]=='@' && strNodeName[1]=='S') {	// case of NETYPE_SOUND	
			pExpInfo->type = CObjLocExport::NETYPE_SOUND;
			ExtractNETName(pnode->GetName(), 'S', pExpInfo->snd_Name);
			strcat(pExpInfo->snd_Name, ".wav");

			TSTR usrPropBuf;
			pnode->GetUserPropBuffer(usrPropBuf);
			imccmdline::Init(usrPropBuf);

			pExpInfo->snd_Pos = affparts.t;
			pExpInfo->snd_Vol = imccmdline::ParmValue("-VOL", 255.0f);
			pExpInfo->snd_Rad = imccmdline::ParmValue("-RAD", 10000.0f);
		}
		else {		// case of NETYPE_NORMAL		
			TSTR	strXnml_RefObjNameNoPath;
			TSTR	strXnml_RefObjName = ix->GetFileName(FALSE);
			
			int		ilen;		
			if( ilen = strlen(strXnml_RefObjName) ) {
				ilen--;
				while(strXnml_RefObjName[ilen] || ilen) {
					if( strXnml_RefObjName[ilen] == '\\' )
						break; 
					ilen--;
				}
				strXnml_RefObjNameNoPath = &(strXnml_RefObjName[ilen+1]);
			}
			
			pExpInfo->type = CObjLocExport::NETYPE_NORMAL;
			strcpy(pExpInfo->nml_ShdMapName, pnode->GetName());
			
			// Dump node description
			strcpy( pExpInfo->nml_ShdObjName, strNodeName );
			strcpy( pExpInfo->nml_RefObjName, strXnml_RefObjNameNoPath );
			
			ilen = strlen( pExpInfo->nml_RefObjName );
			strcpy( &(pExpInfo->nml_RefObjName[ilen-3]), "xac" );
			
			pExpInfo->nml_Pos = affparts.t;
			pExpInfo->nml_Scl = affparts.k;
			affparts.q.GetEuler(&pExpInfo->nml_Rot.x, &pExpInfo->nml_Rot.y, &pExpInfo->nml_Rot.z);
			pExpInfo->nml_Rot.x = -pExpInfo->nml_Rot.x;
			pExpInfo->nml_Rot.y = -pExpInfo->nml_Rot.y;
			pExpInfo->nml_Rot.z = -pExpInfo->nml_Rot.z;
		}
	}
	else {
		pExpInfo->type = CObjLocExport::NETYPE_INVALID;
		goto __End;
	}

	m_phec->m_iCurNode++;

__End:	
	return TREE_CONTINUE;

}
