/***
*
*	Copyright (c) 2010, IMC. All rights reserved.
*	
****/

//===================================================================
// Useful macros
//
#define CONSTRUCTOR
#define DESTRUCTOR

#define EXPORT_THIS		__declspec(dllexport)

#define DEFAULT_EXT		_T("3dlight")

#define FStrEq(sz1, sz2) (strcmp((sz1), (sz2)) == 0)

//===================================================================
// Class that implements the scene-export.
//
class CLightExport : public SceneExport
{
	friend class DumpNodesTEP;

public:
	CONSTRUCTOR	CLightExport(void);
	DESTRUCTOR	~CLightExport(void);

	// Required by classes derived from SceneExport
	virtual int				ExtCount		(void)		{ return 1; }
	virtual const TCHAR*	Ext				(int i)		{ return DEFAULT_EXT; }
	virtual const TCHAR*	LongDesc		(void)		{ return _T("IMC Light Exporter for 3D Studio Max 2011");	}
	virtual const TCHAR*	ShortDesc		(void)		{ return _T("IMC Light"); }
	virtual const TCHAR*	AuthorName		(void)		{ return _T("IMC"); }
	virtual const TCHAR*	CopyrightMessage(void)		{ return _T("Copyright (c) 2010"); }
	virtual const TCHAR*	OtherMessage1	(void)		{ return _T(""); }
	virtual const TCHAR*	OtherMessage2	(void)		{ return _T(""); }
	virtual unsigned int	Version			(void)		{ return 201; }
	virtual void			ShowAbout		(HWND hWnd)	{ return; }
	virtual int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export file

	// Integer constants for this class
	enum
	{
		MAX_NAME_CHARS			= 70,
		UNDESIRABLE_NODE_MARKER	= -7777
	};

	enum NodeExpType {
		NETYPE_INVALID,
		NETYPE_LIGHT,
	};

	typedef struct {
		NodeExpType type;
		char		name[MAX_NAME_CHARS];
		Point3		pos;
		Point3		color;
		float		intensity;
		float		attenStart;
		float		attenEnd;
	} NodeExpInfo;

	NodeExpInfo*	m_rgNodeExpInfo;		// array of nodes
	int				m_iNodeExpInfoMac;		// # of nodes
	int				m_iCurNode;

private:
	BOOL			CollectNodes(ExpInterface *expiface);
	BOOL			DumpNodes(ExpInterface *pexpiface);
	BOOL			SaveDocument(LPCTSTR lpszPathName);
};


//===================================================================
// Basically just a ClassFactory for communicating with 3DSMAX.
//
class CLightExportDesc : public ClassDesc
{
public:
	int				IsPublic		(void)					{ return TRUE;								}
	void *			Create			(BOOL loading=FALSE)	{ return new CLightExport;					}
	const TCHAR *	ClassName		(void)					{ return _T("Light Exporter");				}
	SClass_ID 		SuperClassID	(void)					{ return SCENE_EXPORT_CLASS_ID;				}
	Class_ID 		ClassID			(void)					{ return Class_ID(0x16073622, 0x5cc69cb);	}
	const TCHAR *	Category		(void)					{ return _T("");							}
};


//===================================================================
// Tree Enumeration Callback
//		Just counts the nodes in the node tree
//
class CountNodesTEP : public ITreeEnumProc
{
public:
	virtual int				callback(INode *node);
	int						m_cNodes;		// running count of nodes
};

//===================================================================
// Tree Enumeration Callback
//		Dumps the bone offsets to a file.
//
class DumpNodesTEP : public ITreeEnumProc
{
public:
	virtual int				callback(INode *node);
	CLightExport			*m_phec;
};
