/***
*
*	Copyright (c) 2010, IMC. All rights reserved.
*
****/

#define _CRT_SECURE_NO_WARNINGS

#include "MAX.H"
#include "DECOMP.H"
#include "XRef/iXrefObj.h"
#include "modstack.h"
#include "tinyxml.h"
#include <string>
#include "util.h"
#include <map>
#include <vector>


#include "resource.h"
#include "ObjLocExp.h"

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
	return _T("IMC 3dWorld Export");
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

	m_iCurNode = 0;
	m_iNodeExpInfoMac = 0;

	// Break up filename, re-assemble longer versions
	TSTR strPath, strFile, strExt;
	SplitFilename(TSTR(name), &strPath, &strFile, &strExt);

	TCHAR szFile[MAX_PATH];
	TCHAR szPropFile[MAX_PATH];
	sprintf(szFile, "%s\\%s.%s", (char*)strPath, (char*)strFile, "3dworld");
	sprintf(szPropFile, "%s\\%s.%s", (char*)strPath, (char*)strFile, "3dprop");

	// Count nodes, label them, collect into array
	if (!CollectNodes(pexpiface))
		return 0;	/*fail*/

	// Output nodes
	if (!DumpNodes(pexpiface))
		return 0;	/*fail*/

	SaveDocument(szFile);
	SavePropDoc(szPropFile);

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

BOOL CObjLocExport::SaveDocument(LPCTSTR lpszPathName)
{
	TiXmlDocument		saveFile;
	TiXmlDeclaration	decl("1.0", "", "yes");
	TiXmlElement		world("World");
	for( int i = 0; i < m_iNodeExpInfoMac; i++ ) {
		CObjLocExport::NodeExpInfo* pExpInfo = &m_rgNodeExpInfo[i];
		switch(pExpInfo->type) {
			case CObjLocExport::NETYPE_AREA:
				{
					TiXmlElement elem("Area");
					char tmp[1024] = {NULL};
					sprintf( tmp, "%.4f %.4f %.4f", pExpInfo->area.Min().x, pExpInfo->area.Min().z, pExpInfo->area.Min().y);
					elem.SetAttribute("min", tmp);
					sprintf( tmp, "%.4f %.4f %.4f", pExpInfo->area.Max().x, pExpInfo->area.Max().z, pExpInfo->area.Max().y);
					elem.SetAttribute("max", tmp);
					world.InsertEndChild(elem);
				}
				break;
			case CObjLocExport::NETYPE_NORMAL:
				{
					TiXmlElement elem("Model");
					char tmp[1024];

					elem.SetAttribute("File", pExpInfo->objPath);
					elem.SetAttribute("Model", pExpInfo->objName);
					elem.SetAttribute("ShadowMap",  pExpInfo->shdMapName);

					sprintf( tmp, "%.4f %.4f %.4f", pExpInfo->pos.x, pExpInfo->pos.y, pExpInfo->pos.z);
					elem.SetAttribute("pos", tmp);
					sprintf( tmp, "%.4f %.4f %.4f %.4f", pExpInfo->rot.x, pExpInfo->rot.z, pExpInfo->rot.y, pExpInfo->rot.w);
					elem.SetAttribute("rot", tmp);
					sprintf( tmp, "%.4f %.4f %.4f", pExpInfo->scl.x, pExpInfo->scl.y, pExpInfo->scl.z);
					elem.SetAttribute("scale", tmp);
					world.InsertEndChild(elem);
				}
				break;
		}
	}

	saveFile.InsertEndChild(decl);
	saveFile.InsertEndChild(world);
	saveFile.SaveFile(lpszPathName);

	return TRUE;        // success
}

char * FindLastChar(char * str, char findChar)
{
	int len = strlen(str);
	for (int i = 1 ; i <= len ; i ++)
	{
		if (findChar == str[len - i])
			return str + len - i;
	}

	return NULL;
}

void LoadSTD3DProp(TiXmlDocument& doc, std::vector<TiXmlElement*>& list)
{
	TiXmlElement* root = doc.RootElement();
	TiXmlNode * pChildNode = NULL;
	while(pChildNode = root->IterateChildren(pChildNode))
	{
		TiXmlElement* childElem = pChildNode->ToElement();
		if (childElem)
		{
			const char * pLBuf = childElem->Value();
			if (pLBuf && strcmp(pLBuf, "Model") == 0)
			{
				list.push_back(childElem);
			}
		}
	}
}

void Make3DProp(TiXmlElement& world, CObjLocExport::NodeExpInfo* node, std::vector<TiXmlElement*>& modelElemList)
{
	TiXmlElement* copyFromElem = modelElemList[modelElemList.size() - 1];
	for (int i = 0 ; i < modelElemList.size() ; i ++)
	{
		TiXmlElement* elem = modelElemList[i];
		const char * path = elem->Attribute("Path");
		if (strstr(node->objPath, path))
		{
			copyFromElem = elem;
			break;
		}
	}

	TiXmlNode* newElem = copyFromElem->Clone();
	newElem->ToElement()->SetAttribute("Path", node->objPath);
	world.LinkEndChild(newElem);
}

BOOL CObjLocExport::SavePropDoc(LPCTSTR lpszPathName)
{
	char moduleFullPath[MAX_PATH];
	::GetModuleFileName(NULL, moduleFullPath, MAX_PATH);
	

	char * fc = FindLastChar(moduleFullPath, '\\');
	if (fc)
	{
		strcpy(fc + 1, "plugins\\prop.3dprop");
	}

	TiXmlDocument doc;
	if (false == doc.LoadFile(moduleFullPath)){
		MessageBox(GetActiveWindow(), moduleFullPath, "Status", MB_OK);
		return FALSE;
	}
	
	std::vector<TiXmlElement*> modelElemList;
	LoadSTD3DProp(doc, modelElemList);

	TiXmlDocument		saveFile;
	TiXmlDeclaration	decl("1.0", "", "yes");
	TiXmlElement		world("Prop");

	std::map<std::string, CObjLocExport::NodeExpInfo*> xacList;
	for( int i = 0; i < m_iNodeExpInfoMac; i++ ) {
		CObjLocExport::NodeExpInfo* pExpInfo = &m_rgNodeExpInfo[i];
		switch(pExpInfo->type) {
		case CObjLocExport::NETYPE_NORMAL:
			{
				if (xacList.find(pExpInfo->objPath) == xacList.end())
				{
					xacList[pExpInfo->objPath] = pExpInfo;
				}
			}
			break;
		}
	}

	for (auto i = xacList.begin() ; i != xacList.end() ; i ++)
	{
		CObjLocExport::NodeExpInfo* node = i->second;
		Make3DProp(world, node, modelElemList);
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
	if (pobj->SuperClassID() == LIGHT_CLASS_ID)
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

	CObjLocExport::NodeExpInfo* pExpInfo = &m_phec->m_rgNodeExpInfo[m_phec->m_iCurNode];
	AffineParts affparts;
	decomp_affine(pNode->GetNodeTM(0), &affparts);

	if (::FNodeMarkedToSkip(pNode)) {
		pExpInfo->type = CObjLocExport::NETYPE_INVALID;
		return TREE_CONTINUE;
	}

	if(pNode->IsHidden()) {
		pExpInfo->type = CObjLocExport::NETYPE_INVALID;
		return TREE_CONTINUE;
	}

	//이름으로 구분한다 치고

	Object* pObj = pNode->GetObjectRef();
	Object* pRefObj = pObj;
	TSTR strNodeName;
	TSTR strFileName = "";
	switch(pObj->SuperClassID())
	{
	case SYSTEM_CLASS_ID:
		if (pObj->ClassID() == XREFOBJ_CLASS_ID) {
			IXRefObject *pXRefObj = (IXRefObject*)pObj;
			strNodeName = pXRefObj->GetObjName(FALSE);
			strFileName = pXRefObj->GetCurFile().GetFileName();
		} else {
			pExpInfo->type = CObjLocExport::NETYPE_INVALID;
			return TREE_CONTINUE;
		}
		break;
	case GEOMOBJECT_CLASS_ID:
		strNodeName = pNode->GetName();
		//strFileName = NULL;
		break;
	case GEN_DERIVOB_CLASS_ID:
		{
			IDerivedObject* pDerivedObj = (IDerivedObject*)pObj;
			pRefObj = pDerivedObj->GetObjRef();
			if (pRefObj->SuperClassID() != GEOMOBJECT_CLASS_ID) {
				pExpInfo->type = CObjLocExport::NETYPE_INVALID;
				return TREE_CONTINUE;
			}

			BOOL bFind = FALSE;
			for (int i = 0; i < m_phec->m_iNodeExpInfoMac; i++) {
				CObjLocExport::NodeExpInfo* pOtherExpInfo = &m_phec->m_rgNodeExpInfo[i];
				if (pRefObj == pOtherExpInfo->pObj) {
					strNodeName = pOtherExpInfo->objName;
					bFind = TRUE;
					break;
				}
			}
			if (!bFind) {
				strNodeName = pNode->GetName();
			}
		}
		break;
	default:
		pExpInfo->type = CObjLocExport::NETYPE_INVALID;
		return TREE_CONTINUE;
	}

	std::string nodeName = pNode->GetName();
	static const std::string areaAABB = "_area_";
	std::string::size_type findSize = nodeName.find(areaAABB);
	if ( findSize < nodeName.size() ) {
		Box3 box;
		pObj->GetWorldBoundBox(1, pNode, NULL, box);
		pExpInfo->type = CObjLocExport::NETYPE_AREA;
		pExpInfo->area = box;
	} else {
		pExpInfo->type = CObjLocExport::NETYPE_NORMAL;
		pExpInfo->pObj = pRefObj;
		strcpy(pExpInfo->objName, pNode->GetName());
		if (strcmp(strFileName, "") == 0)
		{
			strcpy(pExpInfo->objPath, strNodeName);
		} else {
			strcpy(pExpInfo->objPath, util::GetFilenameFromFullPath(strFileName));
			int fNameLen = strlen(pExpInfo->objPath);
			pExpInfo->objPath[fNameLen - 4] = 0;
		}

		strcat(pExpInfo->objPath, ".xac");

		strcpy(pExpInfo->objTemplate, strNodeName);
		strcpy(pExpInfo->shdMapName, pNode->GetName());

		pExpInfo->pos.x = abs(affparts.t.x) < 0.00005f ? 0.0f : affparts.t.x;
		pExpInfo->pos.y = abs(affparts.t.z) < 0.00005f ? 0.0f : affparts.t.z;
		pExpInfo->pos.z = abs(affparts.t.y) < 0.00005f ? 0.0f : affparts.t.y;
		pExpInfo->scl = affparts.k;
		pExpInfo->rot = affparts.q;
	}

	m_phec->m_iCurNode++;
	return TREE_CONTINUE;
}
