#include "StdAfx.h"

#include "imcStr.h"
#include "GlyphParser.h"


CTextLineParser::CTextLineParser(void)
{
}


CTextLineParser::~CTextLineParser(void)
{
}

void CTextLineParser::Reset()
{
	m_stream.clear();
}

void CTextLineParser::Init( const std::string& dataFileName )
{
	m_stream = std::ifstream(dataFileName.c_str(), std::ios::in);
}

std::string CTextLineParser::GetGlyph()
{
	char text[4096]  = {NULL};
	std::string utf8Data;
	//while( utf8Data.empty()&& IsEnd() == false )  {
	m_stream.getline(text, 4096);
		utf8Data = text;
	//}
	return utf8Data;
}

bool CTextLineParser::IsEnd() const
{
	return m_stream.eof();
}
	