#pragma once

class CTextExporter
{
public:
	CTextExporter(void);
	~CTextExporter(void);

public:
	void		Reset();

	void		AddChar(char glyph);
	void		AddReturn();
	void		AddSentence(const std::string& sentence);

	void		SaveAS(const std::string& fileName);

private:
	std::string		m_data;
};