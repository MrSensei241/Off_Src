/***
*
*	Copyright (c) 2003, IMC. All rights reserved.
*	
****/

//===================================================================
// Useful macros
//
#define CONSTRUCTOR
#define DESTRUCTOR

#define EXPORT_THIS		__declspec(dllexport)

#define DEFAULT_EXT		_T("objgroup")

#define FStrEq(sz1, sz2) (strcmp((sz1), (sz2)) == 0)

//===================================================================
// Class that implements the scene-export.
//
class CObjLocExport : public SceneExport
{
	friend BOOL CALLBACK ExportObjLocFormatDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	friend class DumpNodesTEP;

public:
	CONSTRUCTOR	CObjLocExport(void);
	DESTRUCTOR	~CObjLocExport(void);

	// Required by classes derived from SceneExport
	virtual int				ExtCount		(void)		{ return 1;						}
	virtual const TCHAR*	Ext				(int i)		{ return DEFAULT_EXT;			}
	virtual const TCHAR*	LongDesc		(void)		{ return _T("IMC GromExporter for 3D Studio Max");	}
	virtual const TCHAR*	ShortDesc		(void)		{ return _T("IMC GromRef");			}
	virtual const TCHAR*	AuthorName		(void)		{ return _T("IMC");			}
	virtual const TCHAR*	CopyrightMessage(void)		{ return _T("Copyright (c) 2009");			}
	virtual const TCHAR*	OtherMessage1	(void)		{ return _T("");				}
	virtual const TCHAR*	OtherMessage2	(void)		{ return _T("");				}
	virtual unsigned int	Version			(void)		{ return 201;					}
	virtual void			ShowAbout		(HWND hWnd)	{ return;						}
	// virtual int				DoExport		(const TCHAR *name, ExpInterface *ei, Interface *i);
	virtual int		DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export	file

	// Integer constants for this class
	enum
	{
		MAX_NAME_CHARS			= 70,
		UNDESIRABLE_NODE_MARKER	= -7777
	};

	enum NodeExpType {
		NETYPE_INVALID,		// invalid
		NETYPE_NORMAL,		// normal max object
		NETYPE_SPEEDTREE,	// speedtree object
		NETYPE_EFFECT,		// effect object
		NETYPE_SOUND,		// sound object
	};
	
	typedef struct {
		NodeExpType type;

		union {
			// case of NETYPE_NORMAL
			struct {
				char		nml_RefObjName[MAX_NAME_CHARS];
				char		nml_ShdObjName[MAX_NAME_CHARS];
				char		nml_ShdMapName[MAX_NAME_CHARS];
				Point3		nml_Pos;
				Point3		nml_Scl;
				Quat		nml_Rot;
			};
			// case of NETYPE_SPEEDTREE
			struct {
				char		spt_Name[MAX_NAME_CHARS];
				Point3		spt_Pos;
			};
			// case of NETYPE_EFFECT
			struct {
				char		eff_Name[MAX_NAME_CHARS];
				Point3		eff_Pos;
				Point3		eff_Scl;
				Quat		eff_Rot;
			};
			// case of NETYPE_SOUND
			struct {
				char		snd_Name[MAX_NAME_CHARS];
				Point3		snd_Pos;
				float		snd_Vol, snd_Rad;
			};
		};		
	} NodeExpInfo;
	
	NodeExpInfo		*m_rgNodeExpInfo;		// array of nodes

	int			m_iNodeExpInfoMac;		// # of nodes
	int			m_iCurNode;

private:
	BOOL	CollectNodes(ExpInterface *expiface);
	BOOL	DumpNodes(ExpInterface *pexpiface);
	BOOL	SaveDocument(LPCTSTR lpszPathName);

	// Is this MAX file just the reference frame, or an animation?
	// If TRUE, the "bones" and "mesh" files will be created.
	// If FALSE, the "rots" file will be created.
	BOOL		m_fReferenceFrame;
};


//===================================================================
// Basically just a ClassFactory for communicating with 3DSMAX.
//
class CObjLocExportDesc : public ClassDesc
{
public:
	int				IsPublic		(void)					{ return TRUE;								}
	void *			Create			(BOOL loading=FALSE)	{ return new CObjLocExport;					}
	const TCHAR *	ClassName		(void)					{ return _T("Object Loction Export");					}
	SClass_ID 		SuperClassID	(void)					{ return SCENE_EXPORT_CLASS_ID;				}
	Class_ID 		ClassID			(void)					{ return Class_ID(0x4bfc605f, 0x1a860a0f);	}
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
	CObjLocExport			*m_phec;
};
