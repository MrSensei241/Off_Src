#pragma once

#include <iostream>
#include <fstream>

class CTextLineParser
{
public:
	CTextLineParser(void);
	~CTextLineParser(void);

public:
	void			Init(const std::string& dataFileName);
	void			Reset();

public:
	std::string		GetGlyph();
	bool			IsEnd() const;

private:
	std::ifstream	m_stream;
};