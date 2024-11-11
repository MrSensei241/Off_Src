// ite.cpp: implementation of the ite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XML.h"
#include <iostream>
#include <sstream>
#include <string>

namespace XML {

static const TCHAR chTagOpen		= '<';
static const TCHAR chTagClose	= '>';
static const TCHAR chTagPre	= '/';
static const TCHAR chEscape = '\\';	// for value field escape

static const TCHAR szPIOpen[] = _T("<?");
static const TCHAR szPIClose[] = _T("?>");
static const TCHAR szCommentOpen[] = _T("<!--");
static const TCHAR szCommentClose[] = _T("-->");
static const TCHAR szCDATAOpen[] = _T("<![CDATA[");
static const TCHAR szCDATAClose[] = _T("]]>");

static const ENTITY x_EntityTable[] = {
		{ '&', _T("&amp;"), 5 } ,
		{ '\"', _T("&quot;"), 6 } ,
		{ '\'', _T("&apos;"), 6 } ,
		{ '<', _T("&lt;"), 4 } ,
		{ '>', _T("&gt;"), 4 } 
	};

PARSEINFO piDefault;
DISP_OPT optDefault;
ENTITYS entityDefault((LPENTITY)x_EntityTable, sizeof(x_EntityTable)/sizeof(x_EntityTable[0]) );
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//========================================================
// Name   : _tcschrs
// Desc   : same with _tcspbrk 
// Param  :
// Return :
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tcschrs( LPCTSTR psz, LPCTSTR pszchs )
{
	while( psz && *psz )
	{
		if( strchr( pszchs, *psz ) )
			return (LPTSTR)psz;
		psz++;
	}
	return NULL;
}

//========================================================
// Name   : _tcsskip
// Desc   : skip space
// Param  : 
// Return : skiped string
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tcsskip( LPCTSTR psz )
{
	//while( psz && *psz == ' ' && *psz == 13 && *psz == 10 ) psz++;
	while( psz && isspace(*psz) ) psz++;
		
	return (LPTSTR)psz;
}

//========================================================
// Name   : _tcsechr
// Desc   : similar with _tcschr with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tcsechr( LPCTSTR psz, int ch, int escape )
{
	LPTSTR pch = (LPTSTR)psz;

	while( pch && *pch )
	{
		if( escape != 0 && *pch == escape )
			pch++;
		else
		if( *pch == ch ) 
			return (LPTSTR)pch;
		pch++;
	}
	return pch;
}

//========================================================
// Name   : _tcselen
// Desc   : similar with _tcslen with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
int _tcselen( int escape, LPTSTR srt, LPTSTR end = NULL ) 
{
	int len = 0;
	LPTSTR pch = srt;
	if( end==NULL ) end = (LPTSTR)sizeof(long);
	LPTSTR prev_escape = NULL;
	while( pch && *pch && pch<end )
	{
		if( escape != 0 && *pch == escape && prev_escape == NULL )
			prev_escape = pch;
		else
		{
			prev_escape = NULL;
			len++;
		}
		pch++;
	}
	return len;
}

//========================================================
// Name   : _tcsecpy
// Desc   : similar with _tcscpy with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
void _tcsecpy( LPTSTR psz, int escape, LPTSTR srt, LPTSTR end = NULL )
{
	LPTSTR pch = srt;
	if( end==NULL ) end = (LPTSTR)sizeof(long);
	LPTSTR prev_escape = NULL;
	while( pch && *pch && pch<end )
	{
		if( escape != 0 && *pch == escape && prev_escape == NULL )
			prev_escape = pch;
		else
		{
			prev_escape = NULL;
			*psz++ = *pch;
		}

		pch++;
	}

	*psz = '\0';
}

//========================================================
// Name   : _tcsepbrk
// Desc   : similar with _tcspbrk with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tcsepbrk( LPCTSTR psz, LPCTSTR chset, int escape )
{
	LPTSTR pch = (LPTSTR)psz;
	LPTSTR prev_escape = NULL;
	while( pch && *pch )
	{
		if( escape != 0 && *pch == escape && prev_escape == NULL )
			prev_escape = pch;
		else
		{
			prev_escape = NULL;
			if( _tcschr( chset, *pch ) )
				return (LPTSTR)pch;		
		}
		pch++;
	}
	return pch;
}

//========================================================
// Name   : _tcsenicmp
// Desc   : similar with _tcsnicmp with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
int _tcsenicmp( LPCTSTR psz, LPCTSTR str, int len, int escape )
{
	LPTSTR pch = (LPTSTR)psz;
	LPTSTR prev_escape = NULL;
	LPTSTR des = (LPTSTR)str;
	int i = 0;
	
	while( pch && *pch && i < len )
	{
		if( escape != 0 && *pch == escape && prev_escape == NULL )
			prev_escape = pch;
		else
		{
			prev_escape = NULL;
			if( tolower(*pch) != tolower(des[i]) )
				break;
			i++;
		}
		pch ++;
	}
	
	// find
	if( i == len )
		return 0;
	if( psz[i] > des[i] )
		return 1;
	return -1;
}

//========================================================
// Name   : _tcsenistr
// Desc   : similar with _tcsistr with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tcsenistr( LPCTSTR psz, LPCTSTR str, int len, int escape )
{
	LPTSTR pch = (LPTSTR)psz;
	LPTSTR prev_escape = NULL;
	LPTSTR des = (LPTSTR)str;
	int i = 0;
	
	while( pch && *pch )
	{
		if( escape != 0 && *pch == escape && prev_escape == NULL )
			prev_escape = pch;
		else
		{
			prev_escape = NULL;
			if( _tcsenicmp( pch, str, len, escape ) == 0 )
				return (LPTSTR)pch;
		}
		pch++;
	}
	return pch;
}

//========================================================
// Name   : _tcseistr
// Desc   : similar with _tcsistr with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tcseistr( LPCTSTR psz, LPCTSTR str, int escape )
{
	int len = _tcslen( str );
	return _tcsenistr( psz, str, len, escape );
}

//========================================================
// Name   : _SetString
// Desc   : put string of (psz~end) on ps string
// Param  : trim - will be trim?
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
void _SetString( LPTSTR psz, LPTSTR end, CString* ps, bool trim = FALSE, int escape = 0 )
{
	//trim
	if( trim )
	{
		while( psz && psz < end && _istspace(*psz) ) psz++;
		while( (end-1) && psz < (end-1) && _istspace(*(end-1)) ) end--;
	}
	int len = end - psz;
	if( len <= 0 ) return;
	if( escape )
	{
		len = _tcselen( escape, psz, end );
		LPTSTR pss = ps->GetBufferSetLength( len );
		_tcsecpy( pss, escape, psz, end );
	}
	else
	{
		LPTSTR pss = ps->GetBufferSetLength(len + 1 );
		memcpy( pss, psz, len );
		pss[len] = '\0';
	}
}

_tagNode::~_tagNode()
{
	Close();
}

void _tagNode::Close()
{
	for( int i = 0 ; i < (int)childs.size(); i ++)
	{
		LPNode p = childs[i];
		if( p )
		{
			delete p; childs[i] = NULL;
		}
	}
	childs.clear();
	
	for( int i = 0 ; i < (int)attrs.size(); i ++)
	{
		LPAttr p = attrs[i];
		if( p )
		{
			delete p; attrs[i] = NULL;
		}
	}
	attrs.clear();
}
	
// attr1="value1" attr2='value2' attr3=value3 />
//                                            ^- return pointer
//========================================================
// Name   : LoadAttributes
// Desc   : loading attribute plain xml text
// Param  : pszAttrs - xml of attributes
//          pi = parser information
// Return : advanced string pointer. (error return NULL)
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tagNode::LoadAttributes( LPCTSTR pszAttrs , LPPARSEINFO pi /*= &piDefault*/)
{
	LPTSTR xml = (LPTSTR)pszAttrs;

	while( xml && *xml )
	{
		if( xml = _tcsskip( xml ) )
		{
			// close tag
			if( *xml == chTagClose || *xml == chTagPre )
				// wel-formed tag
				return xml;

			//  Attr Name
			TCHAR* pEnd = _tcspbrk( xml, " =" );
			if( pEnd == NULL ) 
			{
				// error
				if( pi->erorr_occur == false ) 
				{
					pi->erorr_occur = true;
					pi->error_pointer = xml;
					pi->error_code = PIE_ATTR_NO_VALUE;
					pi->error_string.Format( _T("<%s> attribute has error "), name );
				}
				return NULL;
			}
			
			LPAttr attr = new Attr;
			attr->parent = this;

			//  Attr Name
			_SetString( xml, pEnd, &attr->name );
			
			// add new attribute
			attrs.push_back( attr );
			xml = pEnd;
			
			//  Attr Value
			if( xml = _tcsskip( xml ) )
			{
				//if( xml = _tcschr( xml, '=' ) )
				if( *xml == '=' )
				{
					if( xml = _tcsskip( ++xml ) )
					{
						// if " or '
						// or none quote
						int quote = *xml;
						if( quote == '"' || quote == '\'' )
							pEnd = _tcsechr( ++xml, quote, chEscape );
						else
						{
							//attr= value> 
							// none quote mode
							//pEnd = _tcsechr( xml, ' ', '\\' );
							pEnd = _tcsepbrk( xml, _T(" >"), chEscape );
						}

						bool trim = pi->trim_value;
						TCHAR escape = pi->escape_value;
						//_SetString( xml, pEnd, &attr->value, trim, chEscape );	
						_SetString( xml, pEnd, &attr->value, trim, escape );
						xml = pEnd;
						// ATTRVALUE 
						if( pi->entity_value && pi->entitys )
							attr->value = pi->entitys->Ref2Entity(attr->value);

						if( quote == '"' || quote == '\'' )
							xml++;
					}
				}
			}
		}
	}

	// not wel-formed tag
	return NULL;
}

// attr1="value1" attr2='value2' attr3=value3 />
//                                            ^- return pointer
//========================================================
// Name   : LoadAttributes
// Desc   : loading attribute plain xml text
// Param  : pszAttrs - xml of attributes
//          pszEnd - last string
//          pi = parser information
// Return : advanced string pointer. (error return NULL)
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2004-06-14
//========================================================
LPTSTR _tagNode::LoadAttributes( LPCTSTR pszAttrs, LPCTSTR pszEnd, LPPARSEINFO pi /*= &piDefault*/ )
{
	LPTSTR xml = (LPTSTR)pszAttrs;

	while( xml && *xml )
	{
		if( xml = _tcsskip( xml ) )
		{
			// close tag
			if( xml >= pszEnd )
				// wel-formed tag
				return xml;

			//  Attr Name
			TCHAR* pEnd = _tcspbrk( xml, " =" );
			if( pEnd == NULL ) 
			{
				// error
				if( pi->erorr_occur == false ) 
				{
					pi->erorr_occur = true;
					pi->error_pointer = xml;
					pi->error_code = PIE_ATTR_NO_VALUE;
					pi->error_string.Format( _T("<%s> attribute has error "), name );
				}
				return NULL;
			}
			
			LPAttr attr = new Attr;
			attr->parent = this;

			//  Attr Name
			_SetString( xml, pEnd, &attr->name );
			
			// add new attribute
			attrs.push_back( attr );
			xml = pEnd;
			
			//  Attr Value
			if( xml = _tcsskip( xml ) )
			{
				//if( xml = _tcschr( xml, '=' ) )
				if( *xml == '=' )
				{
					if( xml = _tcsskip( ++xml ) )
					{
						// if " or '
						// or none quote
						int quote = *xml;
						if( quote == '"' || quote == '\'' )
							pEnd = _tcsechr( ++xml, quote, chEscape );
						else
						{
							//attr= value> 
							// none quote mode
							//pEnd = _tcsechr( xml, ' ', '\\' );
							pEnd = _tcsepbrk( xml, _T(" >"), chEscape );
						}

						bool trim = pi->trim_value;
						TCHAR escape = pi->escape_value;
						//_SetString( xml, pEnd, &attr->value, trim, chEscape );	
						_SetString( xml, pEnd, &attr->value, trim, escape );
						xml = pEnd;
						// ATTRVALUE 
						if( pi->entity_value && pi->entitys )
							attr->value = pi->entitys->Ref2Entity(attr->value);

						if( quote == '"' || quote == '\'' )
							xml++;
					}
				}
			}
		}
	}

	// not wel-formed tag
	return NULL;
}

// <?xml version="1.0"?>
//                      ^- return pointer
//========================================================
// Name   : LoadProcessingInstrunction
// Desc   : loading processing instruction
// Param  : pszml - PI string
//          pi - parser information
// Return : advanced string pointer. (error return NULL)
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2004-06-14
//========================================================
LPTSTR _tagNode::LoadProcessingInstrunction( LPCTSTR pszml, LPPARSEINFO pi /*= &piDefault*/ )
{
	// find the end of pi
	LPTSTR end = _tcsenistr( pszml, szPIClose, sizeof(szPIClose)-1, pi ? pi->escape_value : 0 );
	if( end == NULL )
		return NULL;

	// process pi
	if( doc )
	{
		LPTSTR xml = (LPTSTR)pszml;

		LPNode node = new Node;
		node->parent = this;
		node->doc = doc;
		node->type = NODE_PI;
		
		xml += sizeof(szPIOpen)-1;
		TCHAR* pTagEnd = _tcspbrk( xml, " ?>" );
		_SetString( xml, pTagEnd, &node->name );
		xml = pTagEnd;
		
		node->LoadAttributes( xml, end, pi );

		doc->childs.push_back( node );
	}

	end += sizeof(szPIClose)-1;
	return end;
}

// <!-- comment -->
//                 ^- return pointer
//========================================================
// Name   : LoadComment
// Desc   : loading comment
// Param  : pszml - comment string
//          pi - parser information
// Return : advanced string pointer. (error return NULL)
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2004-06-14
//========================================================
LPTSTR _tagNode::LoadComment( LPCTSTR pszml, LPPARSEINFO pi /*= &piDefault*/ )
{
	// find the end of comment
	LPTSTR end = _tcsenistr( pszml, szCommentClose, sizeof(szCommentClose)-1, pi ? pi->escape_value : 0 );
	if( end == NULL )
		return NULL;

	// process comment
	LPNode par = parent;
	if( parent == NULL && doc )
		par = (LPNode)&doc;
	if( par )
	{
		LPTSTR xml = (LPTSTR)pszml;
		xml += sizeof(szCommentOpen)-1;
		
		LPNode node = new Node;
		node->parent = this;
		node->doc = doc;
		node->type = NODE_COMMENT;
		node->name = _T("#COMMENT");
		_SetString( xml, end, &node->value, FALSE );

		par->childs.push_back( node );
	}

	end += sizeof(szCommentClose)-1;
	return end;
}

// <![CDATA[ cdata ]]>
//                    ^- return pointer
//========================================================
// Name   : LoadCDATA
// Desc   : loading CDATA
// Param  : pszml - CDATA string
//          pi - parser information
// Return : advanced string pointer. (error return NULL)
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2004-06-14
//========================================================
LPTSTR _tagNode::LoadCDATA( LPCTSTR pszml, LPPARSEINFO pi /*= &piDefault*/ )
{
	// find the end of CDATA
	LPTSTR end = _tcsenistr( pszml, szCDATAClose, sizeof(szCDATAClose)-1, pi ? pi->escape_value : 0 );
	if( end == NULL )
		return NULL;

	// process CDATA
	LPNode par = parent;
	if( parent == NULL && doc )
		par = (LPNode)&doc;
	if( par )
	{
		LPTSTR xml = (LPTSTR)pszml;
		xml += sizeof(szCDATAOpen)-1;
		
		LPNode node = new Node;
		node->parent = this;
		node->doc = doc;
		node->type = NODE_CDATA;
		node->name = _T("#CDATA");
		_SetString( xml, end, &node->value, FALSE );

		par->childs.push_back( node );
	}

	end += sizeof(szCDATAClose)-1;
	return end;
}

//========================================================
// Name   : LoadOtherNodes
// Desc   : internal function for loading PI/CDATA/Comment
// Param  : node - current xml node
//          pbRet - error occur
//          pszml - CDATA string
//          pi - parser information
// Return : advanced string pointer. (error return NULL)
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2004-06-14
//========================================================
LPTSTR LoadOtherNodes( LPNode node, bool* pbRet, LPCTSTR pszml, LPPARSEINFO pi /*= &piDefault*/ )
{
	LPTSTR xml = (LPTSTR)pszml;
	bool do_other_type = true;
	*pbRet = false;

	while( xml && do_other_type )
	{
		do_other_type = false;

		xml = _tcsskip( xml );
		LPTSTR prev = xml;
		// is PI( Processing Instruction ) Node?
		if( _tcsnicmp( xml, szPIOpen, sizeof(szPIOpen)-1 ) == 0 )
		{
			// processing instrunction parse
			// return pointer is next node of pi
			xml = node->LoadProcessingInstrunction( xml, pi );
			//if( xml == NULL )
			//	return NULL;
			// restart xml parse
		}

		if( xml != prev )
			do_other_type = true;
		xml = _tcsskip( xml );
		prev = xml;

		// is comment Node?
		if( _tcsnicmp( xml, szCommentOpen, sizeof(szCommentOpen)-1 ) == 0 )
		{
			// processing comment parse
			// return pointer is next node of comment
			xml = node->LoadComment( xml, pi );
			// comment node is terminal node
			if( node->parent && node->parent->type != NODE_DOC 
				&& xml != prev )
			{
				*pbRet = true;
				return xml;
			}
			// restart xml parse when this node is root doc node
		}

		if( xml != prev )
			do_other_type = true;

		xml = _tcsskip( xml );
		prev = xml;
		// is CDATA Node?
		if( _tcsnicmp( xml, szCDATAOpen, sizeof(szCDATAOpen)-1 ) == 0 )
		{
			// processing CDATA parse
			// return pointer is next node of CDATA
			xml = node->LoadCDATA( xml, pi );
			// CDATA node is terminal node
			if( node->parent && node->parent->type != NODE_DOC 
				&& xml != prev )
			{
				*pbRet = true;
				return xml;
			}
			// restart xml parse when this node is root doc node
		}

		if( xml != prev )
			do_other_type = true;
	}

	return xml;
}

// <TAG attr1="value1" attr2='value2' attr3=value3 >
// </TAG>
// or
// <TAG />
//        ^- return pointer
//========================================================
// Name   : Load
// Desc   : load xml plain text
// Param  : pszml - plain xml text
//          pi = parser information
// Return : advanced string pointer  (error return NULL)
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tagNode::Load( LPCTSTR pszml, LPPARSEINFO pi /*= &piDefault*/ )
{
	// Close it
	Close();

	LPTSTR xml = (LPTSTR)pszml;

	xml = _tcschr( xml, chTagOpen );
	if( xml == NULL )
		return NULL;

	// Close Tag
	if( *(xml+1) == chTagPre ) // </Close
		return xml;

	// Load Other Node before <Tag>(pi, comment, CDATA etc)
	bool bRet = false;
	LPTSTR ret = NULL;
	ret = LoadOtherNodes( this, &bRet, xml, pi );
	if( ret != NULL ) 
		xml = ret;
	if( bRet ) 
		return xml;

	//  Node Tag Name Open
	xml++;
	TCHAR* pTagEnd = _tcspbrk( xml, " />\t\r\n" );
	_SetString( xml, pTagEnd, &name );
	xml = pTagEnd;
	// Generate  Attributte List
	if( xml = LoadAttributes( xml, pi ) )
	{
		// alone tag <TAG ... />
		if( *xml == chTagPre )
		{
			xml++;
			if( *xml == chTagClose )
				// wel-formed tag
				return ++xml;
			else
			{
				// error: <TAG ... / >
				if( pi->erorr_occur == false ) 
				{
					pi->erorr_occur = true;
					pi->error_pointer = xml;
					pi->error_code = PIE_ALONE_NOT_CLOSED;
					pi->error_string = _T("Element must be closed.");
				}
				// not wel-formed tag
				return NULL;
			}
		}
		else
		// open/close tag <TAG ..> ... </TAG>
		//                        ^- current pointer
		{
			// if text value is not exist, then assign value
			//if( this->value.IsEmpty() || this->value == _T("") )
			if( IsEmptyString( value ) )
			{
				// Text Value 
				TCHAR* pEnd = _tcsechr( ++xml, chTagOpen, chEscape );
				if( pEnd == NULL ) 
				{
					if( pi->erorr_occur == false ) 
					{
						pi->erorr_occur = true;
						pi->error_pointer = xml;
						pi->error_code = PIE_NOT_CLOSED;
						pi->error_string.Format(_T("%s must be closed with </%s>"), name );
					}
					// error cos not exist CloseTag </TAG>
					return NULL;
				}
				
				bool trim = pi->trim_value;
				TCHAR escape = pi->escape_value;
				//_SetString( xml, pEnd, &value, trim, chEscape );
				_SetString( xml, pEnd, &value, trim, escape );

				xml = pEnd;
				// TETVALUE reference
				if( pi->entity_value && pi->entitys )
					value = pi->entitys->Ref2Entity(value);
			}

			// generate child nodes
			while( xml && *xml )
			{
				LPNode node = new Node;
				node->parent = this;
				node->doc = doc;
				node->type = type;
				
				xml = node->Load( xml,pi );
				if( node->name.IsEmpty() == FALSE )
				{
					childs.push_back( node );

				}
				else
				{
					delete node;
				}

				// open/close tag <TAG ..> ... </TAG>
				//                             ^- current pointer
				// CloseTag case
				if( xml && *xml && *(xml+1) && *xml == chTagOpen && *(xml+1) == chTagPre )
				{
					// </Close>
					xml+=2; // C
					
					if( xml = _tcsskip( xml ) )
					{
						CString closename;
						TCHAR* pEnd = _tcspbrk( xml, " >" );
						if( pEnd == NULL ) 
						{
							if( pi->erorr_occur == false ) 
							{
								pi->erorr_occur = true;
								pi->error_pointer = xml;
								pi->error_code = PIE_NOT_CLOSED;
								pi->error_string.Format(_T("it must be closed with </%s>"), name );
							}
							// error
							return NULL;
						}
						_SetString( xml, pEnd, &closename );
						if( closename == this->name )
						{
							// wel-formed open/close
							xml = pEnd+1;
							// return '>' or ' ' after pointer
							return xml;
						}
						else
						{
							xml = pEnd+1;
							// 2004.6.15 - example <B> alone tag
							// now it can parse with attribute 'force_arse'
							if( pi->force_parse == false )
							{
								// not welformed open/close
								if( pi->erorr_occur == false ) 
								{
									pi->erorr_occur = true;
									pi->error_pointer = xml;
									pi->error_code = PIE_NOT_NESTED;
									pi->error_string.Format(_T("'<%s> ... </%s>' is not wel-formed."), name, closename );
								}
								return NULL;
							}
						}
					}
				}
				else	// Alone child Tag Loaded
						// else 해야하는지 말아야하는지 의심간다.
				{
					
					//if( xml && this->value.IsEmpty() && *xml !=chTagOpen )
					if( xml && IsEmptyString( value ) && *xml !=chTagOpen )
					{
						// Text Value 
						TCHAR* pEnd = _tcsechr( xml, chTagOpen, chEscape );
						if( pEnd == NULL ) 
						{
							// error cos not exist CloseTag </TAG>
							if( pi->erorr_occur == false )  
							{
								pi->erorr_occur = true;
								pi->error_pointer = xml;
								pi->error_code = PIE_NOT_CLOSED;
								pi->error_string.Format(_T("it must be closed with </%s>"), name );
							}
							return NULL;
						}
						
						bool trim = pi->trim_value;
						TCHAR escape = pi->escape_value;
						//_SetString( xml, pEnd, &value, trim, chEscape );
						_SetString( xml, pEnd, &value, trim, escape );

						xml = pEnd;
						//TETVALUE
						if( pi->entity_value && pi->entitys )
							value = pi->entitys->Ref2Entity(value);
					}
				}
			}
		}
	}

	return xml;
}

// <?xml version='1.0'?>
// <TAG attr1="value1" attr2='value2' attr3=value3 >
// </TAG>
// or
// <TAG />
//        ^- return pointer
//========================================================
// Name   : Load
// Desc   : load xml plain text for xml document
// Param  : pszml - plain xml text
//          pi = parser information
// Return : advanced string pointer  (error return NULL)
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPTSTR _tagDocument::Load( LPCTSTR pszml, LPPARSEINFO pi /*= NULL*/ )
{
	LPNode node = new Node;
	node->parent = (LPNode)this;
	node->type = NODE_ELEMENT;
	node->doc = this;
	LPTSTR end;
	
	if( pi == NULL )
		pi = &parse_info;

	if( (end = node->Load( pszml, pi )) == NULL )
	{
		delete node;
		return NULL;
	}

	childs.push_back( node );

	// Load Other Node after </Tag>(pi, comment, CDATA etc)
	LPTSTR ret;
	bool bRet = false;
	ret = LoadOtherNodes( node, &bRet, end, pi );
	if( ret != NULL ) 
		end = ret;

	return end;
}

LPNode	_tagDocument::GetRoot()
{
	Nodes::iterator it = childs.begin();
	for( ; it != childs.end() ; ++(it) )
	{
		LPNode node = *it;
		if( node->type == NODE_ELEMENT )
			return node;
	}
	return NULL;
}

//========================================================
// Name   : Get
// Desc   : convert plain xml text from parsed xml attirbute
// Param  :
// Return : converted plain string
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
CString _tagAttr::Get( LPDISP_OPT opt /*= &optDefault*/ )
{
	std::ostringstream os;
	//os << (LPCTSTR)name << "='" << (LPCTSTR)value << "' ";
	
	os << (LPCTSTR)name << "=" << (char)opt->value_quotation_mark 
		<< (LPCTSTR)(opt->reference_value&&opt->entitys?opt->entitys->Entity2Ref(value):value) 
		<< (char)opt->value_quotation_mark << " ";
	return os.str().c_str();
}

//========================================================
// Name   : Get
// Desc   : convert plain xml text from parsed xml node
// Param  :
// Return : converted plain string
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
CString _tagNode::Get( LPDISP_OPT opt /*= &optDefault*/ )
{
	std::ostringstream os;

	// tab
	if( opt && opt->newline )
	{
		if( opt && opt->newline )
			os << "\r\n";
		for( int i = 0 ; i < opt->tab_base ; i++)
			os << '\t';
	}

	if( type == NODE_DOC )
	{
		for( int i = 0 ; i < (int)childs.size(); i++ )
			os << (LPCTSTR)childs[i]->Get( opt );
		return os.str().c_str();
	}
	else
	if( type == NODE_PI )
	{
		// <?TAG
		os << szPIOpen << (LPCTSTR)name;
		// <?TAG Attr1="Val1" 
		if( attrs.empty() == false ) os << ' ';
		for( int i = 0 ; i < (int)attrs.size(); i++ )
		{
			os << (LPCTSTR)attrs[i]->Get(opt);
		}
		//?>
		os << szPIClose;	
		return os.str().c_str();
	}
	else
	if( type == NODE_COMMENT )
	{
		// <--comment
		os << szCommentOpen << (LPCTSTR)value;
		//-->
		os << szCommentClose;	
		return os.str().c_str();
	}
	else
	if( type == NODE_CDATA )
	{
		// <--comment
		os << szCDATAOpen << (LPCTSTR)value;
		//-->
		os << szCDATAClose;	
		return os.str().c_str();
	}

	// <TAG
	os << '<' << (LPCTSTR)name;

	// <TAG Attr1="Val1" 
	if( attrs.empty() == false ) os << ' ';
	for( int i = 0 ; i < (int)attrs.size(); i++ )
	{
		os << (LPCTSTR)attrs[i]->Get(opt);
	}
	
	if( childs.empty() && value.IsEmpty() )
	{
		// <TAG Attr1="Val1"/> alone tag 
		os << "/>";	
	}
	else
	{
		// <TAG Attr1="Val1"> and get child
		os << '>';
		if( opt && opt->newline && !childs.empty() )
		{
			opt->tab_base++;
		}

		for( int i = 0 ; i < (int)childs.size(); i++ )
			os << (LPCTSTR)childs[i]->Get( opt );
		
		// Text Value
		if( value != _T("") )
		{
			if( opt && opt->newline && !childs.empty() )
			{
				if( opt && opt->newline )
					os << "\r\n";
				for( int i = 0 ; i < opt->tab_base ; i++)
					os << '\t';
			}
			os << (LPCTSTR)(opt->reference_value&&opt->entitys?opt->entitys->Entity2Ref(value):value);
		}

		// </TAG> CloseTag
		if( opt && opt->newline && !childs.empty() )
		{
			os << "\r\n";
			for( int i = 0 ; i < opt->tab_base-1 ; i++)
				os << '\t';
		}
		os << "</" << (LPCTSTR)name << '>';

		if( opt && opt->newline )
		{
			if( !childs.empty() )
				opt->tab_base--;
		}
	}
	
	return os.str().c_str();
}

//========================================================
// 함수명 : GetText
// 설  명 : 노드 하나를 텍스트 문자열로 반환
// 인  자 :
// 리턴값 : 변환된 문자열
//--------------------------------------------------------
// 작성자   작성일                 작성이유
// 조경민   2004-06-15
//========================================================
CString _tagNode::GetText( LPDISP_OPT opt /*= &optDefault*/ )
{
	std::ostringstream os;

	if( type == NODE_DOC )
	{
		for( int i = 0 ; i < (int)childs.size(); i++ )
			os << (LPCTSTR)childs[i]->GetText( opt );
	}
	else
	if( type == NODE_PI )
	{
		// no text
	}
	else
	if( type == NODE_COMMENT )
	{
		// no text
	}
	else
	if( type == NODE_CDATA )
	{
		os << (LPCTSTR)value;
	}
	else
	if( type == NODE_ELEMENT )
	{
		if( childs.empty() && value.IsEmpty() )
		{
			// no text
		}
		else
		{
			// childs text
			for( int i = 0 ; i < (int)childs.size(); i++ )
				os << (LPCTSTR)childs[i]->GetText();
			
			// Text Value
			os << (LPCTSTR)(opt->reference_value&&opt->entitys?opt->entitys->Entity2Ref(value):value);
		}
	}
	
	return os.str().c_str();
}

//========================================================
// Name   : GetAttr
// Desc   : get attribute with attribute name
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPAttr	_tagNode::GetAttr( LPCTSTR attrname )
{
	for( int i = 0 ; i < (int)attrs.size(); i++ )
	{
		LPAttr attr = attrs[i];
		if( attr )
		{
			if( attr->name == attrname )
				return attr;
		}
	}
	return NULL;
}

//========================================================
// Name   : GetAttrs
// Desc   : find attributes with attribute name, return its list
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
Attrs _tagNode::GetAttrs( LPCTSTR name )
{
	Attrs attrs;
	for( int i = 0 ; i < (int)attrs.size(); i++ )
	{
		LPAttr attr = attrs[i];
		if( attr )
		{
			if( attr->name == name )
				attrs.push_back( attr );
		}
	}
	return attrs;
}

//========================================================
// Name   : GetAttrValue
// Desc   : get attribute with attribute name, return its value
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPCTSTR	_tagNode::GetAttrValue( LPCTSTR attrname )
{
	LPAttr attr = GetAttr( attrname );
	return attr ? (LPCTSTR)attr->value : NULL;
}

Nodes _tagNode::GetChilds()
{
	return childs;
}

//========================================================
// Name   : GetChilds
// Desc   : Find childs with name and return childs list
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
Nodes _tagNode::GetChilds( LPCTSTR name )
{
	Nodes nodes;
	for( int i = 0 ; i < (int)childs.size(); i++ )
	{
		LPNode node = childs[i];
		if( node )
		{
			if( node->name == name )
				nodes.push_back( node );
		}
	}
	return nodes;	
}

//========================================================
// Name   : GetChild
// Desc   : get child node with index
// Param  :
// Return : NULL return if no child.
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPNode _tagNode::GetChild( int i )
{
	if( i >= 0 && i < (int)childs.size() )
		return childs[i];
	return NULL;
}

//========================================================
// Name   : GetChildCount
// Desc   : get child node count
// Param  :
// Return : 0 return if no child
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-12-26
//========================================================
int	_tagNode::GetChildCount()
{
	return childs.size();
}

//========================================================
// Name   : GetChild
// Desc   : Find child with name and return child
// Param  :
// Return : NULL return if no child.
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPNode	_tagNode::GetChild( LPCTSTR name )
{
	for( int i = 0 ; i < (int)childs.size(); i++ )
	{
		LPNode node = childs[i];
		if( node )
		{
			if( node->name == name )
				return node;
		}
	}
	return NULL;
}

//========================================================
// Name   : GetChildValue
// Desc   : Find child with name and return child's value
// Param  :
// Return : NULL return if no child.
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPCTSTR	_tagNode::GetChildValue( LPCTSTR name )
{
	LPNode node = GetChild( name );
	return (node != NULL)? (LPCTSTR)node->value : NULL;
}

CString	_tagNode::GetChildText( LPCTSTR name, LPDISP_OPT opt /*= &optDefault*/ )
{
	LPNode node = GetChild( name );
	return (node != NULL)? node->GetText(opt) : _T("");
}

LPAttr _tagNode::GetChildAttr( LPCTSTR name, LPCTSTR attrname )
{
	LPNode node = GetChild(name);
	return node ? node->GetAttr(attrname) : NULL;
}

LPCTSTR _tagNode::GetChildAttrValue( LPCTSTR name, LPCTSTR attrname )
{
	LPAttr attr = GetChildAttr( name, attrname );
	return attr ? (LPCTSTR)attr->value : NULL;
}

//========================================================
// Name   : Find
// Desc   : find node with tag name from it's all childs
// Param  :
// Return : NULL return if no found node.
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPNode	_tagNode::Find( LPCTSTR name )
{
	Nodes::iterator it = childs.begin();
	for( ; it != childs.end(); ++(it))
	{
		LPNode child = *it;
		if( child->name == name )
			return child;

		Nodes::iterator it = child->childs.begin();
		for( ; it != child->childs.end(); ++(it))
		{
			LPNode find = child->Find( name );
			if( find != NULL )
				return find;
		}
	}

	return NULL;
}

//========================================================
// Name   : GetChildIterator
// Desc   : get child nodes iterator
// Param  :
// Return : NULL return if no childs.
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
Nodes::iterator _tagNode::GetChildIterator( LPNode node )
{
	Nodes::iterator it = childs.begin();
	for( ; it != childs.end() ; ++(it) )
	{
		if( *it == node )
			return it;
	}
	return it;
}

//========================================================
// Name   : AppendChild
// Desc   : add node
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPNode	_tagNode::AppendChild( LPCTSTR name /*= NULL*/, LPCTSTR value /*= NULL*/ )
{
	return AppendChild( CreateNode( name, value ) );
}

//========================================================
// Name   : AppendChild
// Desc   : add node
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPNode _tagNode::AppendChild( LPNode node )
{
	node->parent = this;
	node->doc = doc;
	childs.push_back( node );
	return node;
}

//========================================================
// Name   : RemoveChild
// Desc   : detach node and delete object
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
bool _tagNode::RemoveChild( LPNode node )
{
	Nodes::iterator it = GetChildIterator( node );
	if( childs.end() != it )
	{
		delete *it;
		childs.erase( it );
		return true;
	}
	return false;
}

//========================================================
// Name   : GetAttr
// Desc   : get attribute with index in attribute list
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPAttr _tagNode::GetAttr( int i )
{
	if( i >= 0 && i < (int)attrs.size() )
		return attrs[i];
	return NULL;
}

//========================================================
// Name   : GetAttrIterator
// Desc   : get attribute iterator
// Param  : 
// Return : std::vector<LPAttr>::iterator
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
Attrs::iterator _tagNode::GetAttrIterator( LPAttr attr )
{
	Attrs::iterator it = attrs.begin();
	for( ; it != attrs.end() ; ++(it) )
	{
		if( *it == attr )
			return it;
	}
	return it;
}

//========================================================
// Name   : AppendAttr
// Desc   : add attribute
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPAttr _tagNode::AppendAttr( LPAttr attr )
{
	attr->parent = this;
	attrs.push_back( attr );
	return attr;
}

//========================================================
// Name   : RemoveAttr
// Desc   : detach attribute and delete object
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
bool _tagNode::RemoveAttr( LPAttr attr )
{
	Attrs::iterator it = GetAttrIterator( attr );
	if( attrs.end() != it )
	{
		delete *it;
		attrs.erase( it );
		return true;
	}
	return false;
}

//========================================================
// Name   : CreateNode
// Desc   : Create node object and return it
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPNode _tagNode::CreateNode( LPCTSTR name /*= NULL*/, LPCTSTR value /*= NULL*/ )
{
	LPNode node = new Node;
	node->name = name;
	node->value = value;
	return node;
}

//========================================================
// Name   : CreateAttr
// Desc   : create Attribute object and return it
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPAttr _tagNode::CreateAttr( LPCTSTR name /*= NULL*/, LPCTSTR value /*= NULL*/ )
{
	LPAttr attr = new Attr;
	attr->name = name;
	attr->value = value;
	return attr;
}

//========================================================
// Name   : AppendAttr
// Desc   : add attribute
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPAttr _tagNode::AppendAttr( LPCTSTR name /*= NULL*/, LPCTSTR value /*= NULL*/ )
{
	return AppendAttr( CreateAttr( name, value ) );
}

//========================================================
// Name   : DetachChild
// Desc   : no delete object, just detach in list
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPNode _tagNode::DetachChild( LPNode node )
{
	Nodes::iterator it = GetChildIterator( node );
	if( childs.end() != it )
	{
		childs.erase( it );
		return node;
	}
	return NULL;
}

//========================================================
// Name   : DetachAttr
// Desc   : no delete object, just detach in list
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPAttr _tagNode::DetachAttr( LPAttr attr )
{
	Attrs::iterator it = GetAttrIterator( attr );
	if( attrs.end() != it )
	{
		attrs.erase( it );
		return attr;
	}
	return NULL;
}

//========================================================
// Name   : CopyNode
// Desc   : copy current level node with own attributes
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
void _tagNode::CopyNode( LPNode node )
{
	Close();

	doc = node->doc;
	parent = node->parent;
	name = node->name;
	value = node->value;
	type = node->type;

	// copy attributes
	for( int i = 0 ; i < (int)node->attrs.size(); i++)
	{
		LPAttr attr = node->attrs[i];
		if( attr )
			AppendAttr( attr->name, attr->value );
	}
}

//========================================================
// Name   : _CopyBranch
// Desc   : recursive internal copy branch 
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
void _tagNode::_CopyBranch( LPNode node )
{
	CopyNode( node );

	for( int i = 0 ; i < (int)node->childs.size(); i++)
	{
		LPNode child = node->childs[i];
		if( child )
		{
			LPNode mychild = new Node;
			mychild->CopyNode( child );
			AppendChild( mychild );

			mychild->_CopyBranch( child );
		}
	}
}

//========================================================
// Name   : AppendChildBranch
// Desc   : add child branch ( deep-copy )
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
LPNode	_tagNode::AppendChildBranch( LPNode node )
{
	LPNode child = new Node;
	child->CopyBranch( node );

	return AppendChild( child );
}

//========================================================
// Name   : CopyBranch
// Desc   : copy branch ( deep-copy )
// Param  :
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
void _tagNode::CopyBranch( LPNode branch )
{
	Close();
	
	_CopyBranch( branch );
}


_tagEntitys::_tagEntitys( LPENTITY entities, int count )
{
	for( int i = 0; i < count; i++)
		push_back( entities[i] );
}

LPENTITY _tagEntitys::GetEntity( int entity )
{
	for( int i = 0 ; i < (int)size(); i ++ )
	{
		if( at(i).entity == entity )
			return LPENTITY(&at(i));
	}
	return NULL;
}

LPENTITY _tagEntitys::GetEntity( LPTSTR entity )
{
	for( int i = 0 ; i < (int)size(); i ++ )
	{
		LPTSTR ref = (LPTSTR)at(i).ref;
		LPTSTR ps = entity;
		while( ref && *ref )
			if( *ref++ != *ps++ )
				break;
		if( ref && !*ref )	// found!
			return LPENTITY(&at(i));
	}
	return NULL;
}

int _tagEntitys::GetEntityCount( LPCTSTR str )
{
	int nCount = 0;
	LPTSTR ps = (LPTSTR)str;
	while( ps && *ps )
		if( GetEntity( *ps++ ) ) nCount ++;
	return nCount;
}

int _tagEntitys::Ref2Entity( LPCTSTR estr, LPTSTR str, int strlen )
{
	LPTSTR pes = (LPTSTR)estr;
	LPTSTR ps = str;
	LPTSTR ps_end = ps+strlen;
	while( pes && *pes && ps < ps_end )
	{
		LPENTITY ent = GetEntity( pes );
		if( ent )
		{
			// copy entity meanning char
			*ps = ent->entity;
			pes += ent->ref_len;
		}
		else
			*ps = *pes++;	// default character copy
		ps++;
	}
	*ps = '\0';
	
	// total copied characters
	return ps-str;	
}

int _tagEntitys::Entity2Ref( LPCTSTR str, LPTSTR estr, int estrlen )
{
	LPTSTR ps = (LPTSTR)str;
	LPTSTR pes = (LPTSTR)estr;
	LPTSTR pes_end = pes+estrlen;
	while( ps && *ps && pes < pes_end )
	{
		LPENTITY ent = GetEntity( *ps );
		if( ent )
		{
			// copy entity string
			LPTSTR ref = (LPTSTR)ent->ref;
			while( ref && *ref )
				*pes++ = *ref++;
		}
		else
			*pes++ = *ps;	// default character copy
		ps++;
	}
	*pes = '\0';
	
	// total copied characters
	return pes-estr;
}

CString _tagEntitys::Ref2Entity( LPCTSTR estr )
{
	CString es;
	if( estr )
	{
		int len = _tcslen(estr);
		LPTSTR esbuf = es.GetBufferSetLength( len+1 );
		if( esbuf )
			Ref2Entity( estr, esbuf, len );
	}
	return es;
}

CString _tagEntitys::Entity2Ref( LPCTSTR str )
{
	CString s;
	if( str )
	{
		int nEntityCount = GetEntityCount(str);
		if( nEntityCount == 0 )
			return CString(str);
		int len = _tcslen(str) + nEntityCount*10 ;
		LPTSTR sbuf = s.GetBufferSetLength( len+1 );
		if( sbuf )
			Entity2Ref( str, sbuf, len );
	}
	return s;
}

CString Ref2Entity( LPCTSTR estr )
{
	return entityDefault.Ref2Entity( estr );
}

CString Entity2Ref( LPCTSTR str )
{
	return entityDefault.Entity2Ref( str );
}

}