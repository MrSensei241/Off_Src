#include "StdAfx.h"
#include "GlyphExporter.h"


CTextExporter::CTextExporter(void)
{
}


CTextExporter::~CTextExporter(void)
{
}

void CTextExporter::Reset()
{
	m_data.clear();
}

void CTextExporter::AddChar( char glyph )
{
	m_data.push_back(glyph);
}

void CTextExporter::AddSentence( const std::string& sentence )
{
	m_data.append(sentence.c_str());
}

void CTextExporter::AddReturn()
{
	m_data.push_back('\n');
}

void CTextExporter::SaveAS( const std::string& fileName )
{
	FILE* fp;
	fp = fopen(fileName.c_str(), "wt");
	if ( fp ) {
		fprintf(fp, "%s", m_data.c_str());
		fclose(fp);
	}
}