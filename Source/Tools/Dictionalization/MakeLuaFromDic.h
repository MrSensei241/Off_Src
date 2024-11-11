#pragma once

#include "GlyphParser.h"
#include "GlyphExporter.h"

class CMakeLuaFromDic
{
public:
	CMakeLuaFromDic(void);
	~CMakeLuaFromDic(void);

public:
	void	StartMake(const std::string& filePath);

private:
	void	MakeFile(const std::string& fileName);
	std::string ChangeToDicDataRecursive(std::string sentence);

private:
	CTextLineParser		m_textParser;
	CTextExporter		m_texExporter;
};

