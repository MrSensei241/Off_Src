#pragma once
#include <string>
#include "structure.h"

class CIpfManager
{
public:
	/// open
	bool	isIPF(const char* ipfFilePath);
	int		OpenIPF(const char* ipfFilePath);
	void	GetInternalPath( const char* ipfInternalDirToRead, char (**ppInternalFilePathArrayOut)[260]
							, size_t* pInternalFilePathArraySizeOut );

	void	GetRemoveFile( const char* ipfInternalDirToRead, char (**ppInternalFilePathArrayOut)[260]
							, size_t* pInternalFilePathArraySizeOut );

	/// create
	void	CreateIPF(const char* ipfFilePath);
	void	WriteIpfData(CACHE_DATA, std::map<std::string, DATA_DESCRIPTOR>);
	void	WriteIpfData(WORD version);
	void	Close();

public:
	std::map<std::string, DATA_DESCRIPTOR> m_CentralDirectoryMap;
	std::map<std::string, REMOVE_DATA_DESCRIPTOR> m_RemoveDataList;

	IPFInfo m_info;

protected:
	FILE* m_newfile;
	std::vector<DATA_DESCRIPTOR> OptimizeMap;
};
