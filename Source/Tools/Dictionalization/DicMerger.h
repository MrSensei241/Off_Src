#pragma once

class CDicMerger
{
public:
	CDicMerger(void);
	~CDicMerger(void);

private:
	struct MERGE_DATA {
		imcDic::SENTENCE_ID		ID;
		bool		isUse;
		std::vector<std::string> translateDatas;// 1번은 무조건 한국어
		std::string fileName;
	};

public:
	void	StartMerge();

private:
	void	LoadBaseData(imcXml::CXmlDoc& doc, std::vector<MERGE_DATA>* pOutMerge);
	void	MergeDicData(const std::string& targetFullPath, imcXml::CXmlDoc& doc, const std::vector<MERGE_DATA>& mergeBase);
	void	SaveMergeData(const std::string& path, std::vector<MERGE_DATA>& mergeData);
};