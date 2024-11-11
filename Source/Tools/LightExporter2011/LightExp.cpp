/***
*
*	Copyright (c) 2010, IMC. All rights reserved.
*
****/

#define _CRT_SECURE_NO_WARNINGS

#include "MAX.H"
#include "DECOMP.H"
#include "modstack.h"
#include "tinyxml.h"

#include "resource.h"
#include "LightExp.h"

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
static CLightExportDesc CLightExportCD;

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
		case 0: return &CLightExportCD;
		default: return 0;
	}
}

EXPORT_THIS const TCHAR *LibDescription()
{
	return _T("IMC Light Export");
}

EXPORT_THIS ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

//=====================================================================
// Methods for CLightExport
//

CONSTRUCTOR CLightExport::CLightExport(void)
{
	m_rgNodeExpInfo = NULL;
}

DESTRUCTOR CLightExport::~CLightExport(void)
{
}

int CLightExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options) 
{
	ExpInterface	*pexpiface = ei;	// Hungarian
	Interface		*piface = i;		// Hungarian

	// Reset the name-map property manager
	g_inmMac = 0;

	m_iCurNode = 0;
	m_iNodeExpInfoMac = 0;

	// Break up filename, re-assemble longer versions
	TSTR strPath, strFile, strExt;
	TCHAR szFile[MAX_PATH];
	SplitFilename(TSTR(name), &strPath, &strFile, &strExt);

	sprintf(szFile, "%s\\%s.%s", (char*)strPath, (char*)strFile, "3dlight");

	// Count nodes, label them, collect into array
	if (!CollectNodes(pexpiface))
		return 0;	/*fail*/

	// Output nodes
	if (!DumpNodes(pexpiface))
		return 0;	/*fail*/

	SaveDocument(szFile);

	// Tell user that exporting is finished (it can take a while with no feedback)
	char szExportComplete[300];
	sprintf(szExportComplete, "Exported %s.", szFile);
	MessageBox(GetActiveWindow(), szExportComplete, "Status", MB_OK);

	if (m_rgNodeExpInfo) {
		delete [] m_rgNodeExpInfo;
		m_rgNodeExpInfo = NULL;
	}

	return 1/*success*/;
}

BOOL CLightExport::SaveDocument(LPCTSTR lpszPathName)
{
	TiXmlDocument		saveFile;
	TiXmlDeclaration	decl("1.0", "", "yes");
	TiXmlElement		world("Lights");
	for( int i = 0; i < m_iNodeExpInfoMac; i++ ) {
		CLightExport::NodeExpInfo* pExpInfo = &m_rgNodeExpInfo[i];
		if(pExpInfo->type != CLightExport::NETYPE_LIGHT) {
			continue;
		}

		TiXmlElement elem("Omni");
		char tmp[1024];

		elem.SetAttribute("Name", pExpInfo->name);

		sprintf( tmp, "%.4f %.4f %.4f", pExpInfo->pos.x, pExpInfo->pos.y, pExpInfo->pos.z);
		elem.SetAttribute("Pos", tmp);
		sprintf( tmp, "%d %d %d", (int)pExpInfo->color.x, (int)pExpInfo->color.y, (int)pExpInfo->color.z);
		elem.SetAttribute("Color", tmp);
		sprintf( tmp, "%.4f", pExpInfo->intensity);
		elem.SetAttribute("Intensity", tmp);
		sprintf( tmp, "%.4f", pExpInfo->attenStart);
		elem.SetAttribute("AttenuationStart", tmp);
		sprintf( tmp, "%.4f", pExpInfo->attenEnd);
		elem.SetAttribute("AttenuationEnd", tmp);

		world.InsertEndChild(elem);
	}

	saveFile.InsertEndChild(decl);
	saveFile.InsertEndChild(world);
	saveFile.SaveFile(lpszPathName);

	return TRUE;        // success
}
	
BOOL CLightExport::CollectNodes( ExpInterface *pexpiface)
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

BOOL CLightExport::DumpNodes(ExpInterface *pexpiface)
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
		::SetIndexOfINode(pnode, CLightExport::UNDESIRABLE_NODE_MARKER);
		return TREE_CONTINUE;
	}
	
	// Establish "node index"--just ascending ints
	::SetIndexOfINode(pnode, m_cNodes);

	m_cNodes++;
	
	return TREE_CONTINUE;
}

//========================================================================
// Utility functions for getting/setting the personal "node index" property.
// NOTE: I'm storing a string-property because I hit a 3DSMax bug in v1.2 when I
// NOTE: tried using an integer property.
// FURTHER NOTE: those properties seem to change randomly sometimes, so I'm
// implementing my own.

typedef struct
{
	char	szNodeName[CLightExport::MAX_NAME_CHARS];
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

	// Don't care about geometries, dummies, and cameras
	if (pobj->SuperClassID() == GEOMOBJECT_CLASS_ID)
		return TRUE;
	if (pobj->SuperClassID() == HELPER_CLASS_ID)
		return TRUE;
	if (pobj->SuperClassID() == CAMERA_CLASS_ID)
		return TRUE;

	return FALSE;
}


//=============================================================
// Returns TRUE if a node has been marked as skippable
//
BOOL FNodeMarkedToSkip(INode *pnode)
{
	return (::GetIndexOfINode(pnode) == CLightExport::UNDESIRABLE_NODE_MARKER);
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
// Node Export Type 이름을 추출한다.
//
bool ExtractNETName(const char* objName, char type, char* netName)
{
	if(objName[0]!='@')		return false;
	if(objName[1]!=type)	return false;

	int nameLen = (int)strlen(objName);
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
int DumpNodesTEP::callback(INode *pNode)
{
	ASSERT_MBOX(!(pNode)->IsRootNode(), "Encountered a root node!");

	CLightExport::NodeExpInfo* pExpInfo = &m_phec->m_rgNodeExpInfo[m_phec->m_iCurNode];
	AffineParts affparts;
	decomp_affine(pNode->GetNodeTM(0), &affparts);

	if (::FNodeMarkedToSkip(pNode)) {
		pExpInfo->type = CLightExport::NETYPE_INVALID;
		return TREE_CONTINUE;
	}

	if(pNode->IsHidden()) {
		pExpInfo->type = CLightExport::NETYPE_INVALID;
		return TREE_CONTINUE;
	}

	Object* pObj = pNode->GetObjectRef();
	Object* pRefObj = pObj;
	switch(pObj->SuperClassID())
	{
	case LIGHT_CLASS_ID:
		break;
	case GEN_DERIVOB_CLASS_ID:
		{
			IDerivedObject* pDerivedObj = (IDerivedObject*)pObj;
			pRefObj = pDerivedObj->GetObjRef();
			if (pRefObj->SuperClassID() != LIGHT_CLASS_ID) {
				pExpInfo->type = CLightExport::NETYPE_INVALID;
				return TREE_CONTINUE;
			}
		}
		break;
	default:
		pExpInfo->type = CLightExport::NETYPE_INVALID;
		return TREE_CONTINUE;
	}

	GenLight* pGenLight = (GenLight*)(pRefObj);
	if (pGenLight->Type() != OMNI_LIGHT) {
		pExpInfo->type = CLightExport::NETYPE_INVALID;
		return TREE_CONTINUE;
	}

	pExpInfo->type = CLightExport::NETYPE_LIGHT;
	strcpy(pExpInfo->name, pNode->GetName());
	pExpInfo->pos.x = abs(affparts.t.x) < 0.00005f ? 0.0f : affparts.t.x;
	pExpInfo->pos.y = abs(affparts.t.z) < 0.00005f ? 0.0f : affparts.t.z;
	pExpInfo->pos.z = abs(affparts.t.y) < 0.00005f ? 0.0f : affparts.t.y;
	pExpInfo->color = pGenLight->GetRGBColor(0) * 255;
	pExpInfo->intensity = pGenLight->GetIntensity(0);
	pExpInfo->attenStart = pGenLight->GetAtten(0, ATTEN_START);
	pExpInfo->attenEnd = pGenLight->GetAtten(0, ATTEN_END);

	m_phec->m_iCurNode++;

	return TREE_CONTINUE;
}
