#pragma once

#include <vector>
#include <deque>

namespace XML {

struct _tagAttr;
typedef _tagAttr Attr, *LPAttr;
typedef std::vector<LPAttr> Attrs;

struct _tagNode;
typedef _tagNode Node, *LPNode;
typedef std::vector<LPNode> Nodes, *LPNodes;

struct _tagDocument;
typedef struct _tagDocument Doc, *LPDoc;

// Entity Encode/Decode Support
typedef struct _tagmlEntity
{
	TCHAR entity;					// entity ( & " ' < > )
	TCHAR ref[10];					// entity reference ( &amp; &quot; etc )
	int ref_len;					// entity reference length
}ENTITY,*LPENTITY;

typedef struct _tagEntitys : public std::vector<ENTITY>
{
	LPENTITY GetEntity( int entity );
	LPENTITY GetEntity( LPTSTR entity );	
	int GetEntityCount( LPCTSTR str );
	int Ref2Entity( LPCTSTR estr, LPTSTR str, int strlen );
	int Entity2Ref( LPCTSTR str, LPTSTR estr, int estrlen );
	CString Ref2Entity( LPCTSTR estr );
	CString Entity2Ref( LPCTSTR str );	

	_tagEntitys(){};
	_tagEntitys( LPENTITY entities, int count );
}ENTITYS,*LPENTITYS;
extern ENTITYS entityDefault;
CString Ref2Entity( LPCTSTR estr );
CString Entity2Ref( LPCTSTR str );	

typedef enum 
{
	PIE_PARSE_WELFORMED	= 0,
	PIE_ALONE_NOT_CLOSED,
	PIE_NOT_CLOSED,
	PIE_NOT_NESTED,
	PIE_ATTR_NO_VALUE
}PCODE;

// Parse info.
typedef struct _tagParseInfo
{
	bool		trim_value;			// [set] do trim when parse?
	bool		entity_value;		// [set] do convert from reference to entity? ( &lt; -> < )
	LPENTITYS	entitys;			// [set] entity table for entity decode
	TCHAR		escape_value;		// [set] escape value (default '\\')
	bool		force_parse;		// [set] force parse even if xml is not welformed

	LPTSTR		xml;				// [get] xml source
	bool		erorr_occur;		// [get] is occurance of error?
	LPTSTR		error_pointer;		// [get] error position of xml source
	PCODE		error_code;			// [get] error code
	CString		error_string;		// [get] error string

	LPDoc		doc;
	_tagParseInfo() { trim_value = false; entity_value = true; force_parse = false; entitys = &entityDefault; xml = NULL; erorr_occur = false; error_pointer = NULL; error_code = PIE_PARSE_WELFORMED; escape_value = '\\'; }
}PARSEINFO,*LPPARSEINFO;
extern PARSEINFO piDefault;

// display optional environment
typedef struct _tagDispOption
{
	bool newline;			// newline when new tag
	bool reference_value;	// do convert from entity to reference ( < -> &lt; )
	char value_quotation_mark;	// val="" (default value quotation mark "
	LPENTITYS	entitys;	// entity table for entity encode

	int tab_base;			// internal usage
	_tagDispOption() { newline = true; reference_value = true; entitys = &entityDefault; tab_base = 0; value_quotation_mark = '"'; }
}DISP_OPT, *LPDISP_OPT;
extern DISP_OPT optDefault;

// Attr : Attribute Implementation
typedef struct _tagAttr
{
	CString name;
	CString	value;
	
	_tagNode*	parent;

	CString Get( LPDISP_OPT opt = &optDefault );
}Attr, *LPAttr;

typedef enum
{
	NODE_ELEMENT,				// general node '<element>...</element>' or <element/>
	NODE_PI,					// <?xml version="1.0" ?>
	NODE_COMMENT,				// <!-- comment -->
	NODE_CDATA,				// <![CDATA[ cdata ]]>
	NODE_DOC,					// internal virtual root
}NODE_TYPE;

// Node structure
typedef struct _tagNode
{
	// name and value
	CString name;
	CString	value;

	// internal variables
	LPNode	parent;		// parent node
	Nodes	childs;		// child node
	Attrs	attrs;		// attributes
	NODE_TYPE type;		// node type 
	LPDoc	doc;		// document

	// Load/Save 
	LPTSTR	Load( LPCTSTR pszml, LPPARSEINFO pi = &piDefault );
	CString Get( LPDISP_OPT opt = &optDefault );
	CString GetText( LPDISP_OPT opt = &optDefault );

	// internal load functions
	LPTSTR	LoadAttributes( LPCTSTR pszAttrs, LPPARSEINFO pi = &piDefault );
	LPTSTR	LoadAttributes( LPCTSTR pszAttrs, LPCTSTR pszEnd, LPPARSEINFO pi = &piDefault );
	LPTSTR	LoadProcessingInstrunction( LPCTSTR pszml, LPPARSEINFO pi = &piDefault );
	LPTSTR	LoadComment( LPCTSTR pszml, LPPARSEINFO pi = &piDefault ); 
	LPTSTR	LoadCDATA( LPCTSTR pszml, LPPARSEINFO pi = &piDefault ); 

	// in own attribute list
	LPAttr	GetAttr( LPCTSTR attrname ); 
	LPCTSTR	GetAttrValue( LPCTSTR attrname ); 
	Attrs	GetAttrs( LPCTSTR name ); 

	// in one level child nodes
	LPNode	GetChild( LPCTSTR name ); 
	LPCTSTR	GetChildValue( LPCTSTR name ); 
	CString	GetChildText( LPCTSTR name, LPDISP_OPT opt = &optDefault );
	Nodes	GetChilds( LPCTSTR name ); 
	Nodes	GetChilds(); 

	LPAttr GetChildAttr( LPCTSTR name, LPCTSTR attrname );
	LPCTSTR GetChildAttrValue( LPCTSTR name, LPCTSTR attrname );
	
	// search node
	LPNode	Find( LPCTSTR name );

	// modify DOM 
	int		GetChildCount();
	LPNode GetChild( int i );
	Nodes::iterator GetChildIterator( LPNode node );
	LPNode CreateNode( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPNode	AppendChild( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPNode	AppendChild( LPNode node );
	bool	RemoveChild( LPNode node );
	LPNode DetachChild( LPNode node );

	// node/branch copy
	void	CopyNode( LPNode node );
	void	CopyBranch( LPNode branch );
	void	_CopyBranch( LPNode node );
	LPNode	AppendChildBranch( LPNode node );

	// modify attribute
	LPAttr GetAttr( int i );
	Attrs::iterator GetAttrIterator( LPAttr node );
	LPAttr CreateAttr( LPCTSTR anem = NULL, LPCTSTR value = NULL );
	LPAttr AppendAttr( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPAttr	AppendAttr( LPAttr attr );
	bool	RemoveAttr( LPAttr attr );
	LPAttr DetachAttr( LPAttr attr );

	// operator overloads
	LPNode operator [] ( int i ) { return GetChild(i); }
	Node& operator = ( Node& node ) { CopyBranch(&node); return *this; }

	_tagNode() { parent = NULL; doc = NULL; type = NODE_ELEMENT; }
	~_tagNode();

	void Close();
}Node, *LPNode;

// Document structure
typedef struct _tagDocument : public Node
{
	PARSEINFO	parse_info;

	_tagDocument() { parent = NULL; doc = this; type = NODE_DOC; }
	
	LPTSTR	Load( LPCTSTR pszml, LPPARSEINFO pi = NULL );
	LPNode	GetRoot();

}Doc, *LPDoc;

// Helper Funtion
inline long Str2Int( LPCTSTR str, long default_value = 0 )
{
	return ( str && *str ) ? _ttol(str) : default_value;
}

inline bool IsEmptyString( LPCTSTR str )
{
	CString s(str);
	s.TrimLeft();
	s.TrimRight();

	return ( s.IsEmpty() || s == _T("") );
}

}
