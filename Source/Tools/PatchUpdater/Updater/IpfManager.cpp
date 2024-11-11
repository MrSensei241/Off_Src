#include "stdafx.h"
#include "IpfManager.h"
#include "Updater.h"

const DWORD Signature = 0x06054b50;

bool CIpfManager::isIPF(const char* ipfFilePath)
{
	FILE* m_pIpfFile;
	fopen_s(&m_pIpfFile, ipfFilePath, "rb+");
	if (m_pIpfFile==NULL)
		return false;

	int size = sizeof(IPFInfo);
	fseek(m_pIpfFile, -size, SEEK_END);
	fread(&m_info, size, 1, m_pIpfFile);

	if (m_info._Signature != Signature)
	{
		fclose(m_pIpfFile);
		return false;
	}

	fclose(m_pIpfFile);
	return true;
}

int CIpfManager::OpenIPF(const char* ipfFilePath)
{
	FILE* m_pIpfFile;
	fopen_s(&m_pIpfFile, ipfFilePath, "rb+");
	if (m_pIpfFile==NULL)
		return -1;

	WORD m_iDataCount;
	DWORD m_dwOffset;

	int size = sizeof(IPFInfo);
	fseek(m_pIpfFile, -size, SEEK_END);
	fread(&m_info, size, 1, m_pIpfFile);

	if (m_info._Signature != Signature)
	{
		fclose(m_pIpfFile);
		return -2;
	}

	m_iDataCount = m_info.DataCount;
	m_dwOffset = m_info.DataOffset;
	fseek(m_pIpfFile, m_dwOffset, SEEK_SET);

	m_CentralDirectoryMap.clear();
	m_RemoveDataList.clear();

	while (m_iDataCount)
	{
		DATA_DESCRIPTOR	TempDataDescriptor;
		char			TempFilenameBuffer[MAX_PATH] = {0,};
		char			TempIPFnameBuffer[MAX_PATH] = {0,};

		fread(&TempDataDescriptor.header, sizeof(FILE_HEADER), 1, m_pIpfFile);
		fread(TempIPFnameBuffer, TempDataDescriptor.header.IPFnameLength, 1, m_pIpfFile);
		fread(TempFilenameBuffer, TempDataDescriptor.header.FilenameLength, 1, m_pIpfFile);

		TempDataDescriptor.ipfname = TempIPFnameBuffer;
		TempDataDescriptor.dataname = TempFilenameBuffer;

		char TempKey[MAX_PATH] = {0,};
		sprintf_s(TempKey, "%s_%s", TempDataDescriptor.ipfname.data(), TempDataDescriptor.dataname.data());

		std::string Key(TempKey, strlen(TempKey));

		typedef std::map<std::string, DATA_DESCRIPTOR>::iterator ITER;
		ITER FindResult = m_CentralDirectoryMap.find(Key);
		if (FindResult==m_CentralDirectoryMap.end())
			m_CentralDirectoryMap.insert(std::pair<std::string,DATA_DESCRIPTOR>(Key,TempDataDescriptor));
		else
			FindResult->second = TempDataDescriptor;

		--m_iDataCount;
	}

	WORD removeCount = m_info.RemoveCount;

	fseek(m_pIpfFile, m_info.RemoveOffset, SEEK_SET);
	while (removeCount)
	{
		REMOVE_DATA_DESCRIPTOR	TempDataDescriptor;
		char					TempIPFnameBuffer[MAX_PATH] = {0,};
		char					TempFilenameBuffer[MAX_PATH] = {0,};

		fread(&TempDataDescriptor.header, sizeof(REMOVE_FILE_HEADER), 1, m_pIpfFile);
		fread(TempIPFnameBuffer, TempDataDescriptor.header.IPFnameLength, 1, m_pIpfFile);
		fread(TempFilenameBuffer, TempDataDescriptor.header.FilenameLength, 1, m_pIpfFile);
		TempDataDescriptor.dataname = TempFilenameBuffer;
		TempDataDescriptor.ipfname = TempIPFnameBuffer;

		char TempKey[MAX_PATH] = {0,};
		sprintf_s(TempKey, "%s_%s", TempIPFnameBuffer, TempFilenameBuffer);

		std::string Key(TempKey, strlen(TempKey));

		m_RemoveDataList.insert(std::pair<std::string,REMOVE_DATA_DESCRIPTOR>(Key,TempDataDescriptor));

		--removeCount;
	}

	fclose(m_pIpfFile);

	return 0;
}

void CIpfManager::GetInternalPath( const char* ipfInternalDirToRead, char (**ppInternalFilePathArrayOut)[260], size_t* pInternalFilePathArraySizeOut )
{
	size_t ipfInternalParentDirLen = strlen(ipfInternalDirToRead);

	std::vector<std::string>	FileNames;
	{
		typedef std::map<std::string,DATA_DESCRIPTOR>::iterator ITER;
		ITER i = m_CentralDirectoryMap.begin();
		ITER CentralDirectoryMapEnd = m_CentralDirectoryMap.end();

		while (i != CentralDirectoryMapEnd)
		{
			if(ipfInternalParentDirLen)
			{
				if(!strcmp(ipfInternalDirToRead, i->second.ipfname.data()))
				{
					FileNames.push_back(i->second.dataname.data());
				}
			}
			else
			{
				if(FileNames.empty())
				{
					FileNames.push_back(i->second.ipfname.data());
				}
				else
				{
					if(std::find(FileNames.begin(), FileNames.end(), i->second.ipfname.data())
						== FileNames.end())
						FileNames.push_back(i->second.ipfname.data());
				}
			}

			++i;
		}
	}

	size_t StringCount = FileNames.size();
	*pInternalFilePathArraySizeOut = StringCount;

	{
		*ppInternalFilePathArrayOut = new char[StringCount][MAX_PATH];
		char (*pCurArr)[MAX_PATH] = *ppInternalFilePathArrayOut;

		typedef std::vector<std::string>::iterator ITER;
		ITER i = FileNames.begin();
		ITER iend = FileNames.end();
		while (i!=iend)
		{
			memcpy(pCurArr, i->c_str(), i->length()+1);
			++pCurArr;
			++i;
		}
	}
}

void CIpfManager::GetRemoveFile( const char* ipfInternalDirToRead, char (**ppInternalFilePathArrayOut)[260], size_t* pInternalFilePathArraySizeOut)
{
	size_t ipfInternalParentDirLen = strlen(ipfInternalDirToRead);

	std::vector<std::string>	FileNames;
	{
		typedef std::map<std::string,REMOVE_DATA_DESCRIPTOR>::iterator ITER;
		ITER i = m_RemoveDataList.begin();
		ITER CentralDirectoryMapEnd = m_RemoveDataList.end();

		while (i != CentralDirectoryMapEnd)
		{
			if(ipfInternalParentDirLen)
			{
				if(!strcmp(ipfInternalDirToRead, i->second.ipfname.data()))
				{
					FileNames.push_back(i->second.dataname.data());
				}
			}
			else
			{
				if(FileNames.empty())
				{
					FileNames.push_back(i->second.ipfname.data());
				}
				else
				{
					if(std::find(FileNames.begin(), FileNames.end(), i->second.ipfname.data())
						== FileNames.end())
						FileNames.push_back(i->second.ipfname.data());
				}
			}

			++i;
		}
	}

	size_t StringCount = FileNames.size();
	*pInternalFilePathArraySizeOut = StringCount;

	{
		*ppInternalFilePathArrayOut = new char[StringCount][MAX_PATH];
		char (*pCurArr)[MAX_PATH] = *ppInternalFilePathArrayOut;

		typedef std::vector<std::string>::iterator ITER;
		ITER i = FileNames.begin();
		ITER iend = FileNames.end();
		while (i!=iend)
		{
			memcpy(pCurArr, i->c_str(), i->length()+1);
			++pCurArr;
			++i;
		}
	}
}

void CIpfManager::CreateIPF(const char* ipfFilePath)
{
	fopen_s( &m_newfile, ipfFilePath, "wb+");
	OptimizeMap.clear();
}

void CIpfManager::WriteIpfData(WORD version)
{
	DWORD offset = ftell(m_newfile);

	for(unsigned int i = 0; i < OptimizeMap.size(); ++i)
	{
		const DATA_DESCRIPTOR& FileHeader = OptimizeMap[i];

		fwrite(&FileHeader.header, sizeof(FILE_HEADER), 1, m_newfile);

		fwrite(FileHeader.ipfname.data(), FileHeader.header.IPFnameLength, 1, m_newfile);
		fwrite(FileHeader.dataname.data(), FileHeader.header.FilenameLength, 1, m_newfile);
	}

	IPFInfo info;
	info.DataOffset = offset;
	info._Signature = Signature;
	info.DataCount = OptimizeMap.size();
	info.RemoveCount = 0;
	info.RemoveOffset = ftell(m_newfile);
	info.Start = 0;
	info.End = version;

	fwrite(&info, sizeof(IPFInfo), 1, m_newfile);
}

void CIpfManager::WriteIpfData(CACHE_DATA data, std::map<std::string, DATA_DESCRIPTOR> dataHeader)
{
	std::map<std::string, DATA_DESCRIPTOR>::iterator finddata;
	CString Key = data.strIpfName + "_" + data.strDataName;
	finddata = dataHeader.find(Key.GetBuffer());

	if(finddata != dataHeader.end())
	{
		DATA_DESCRIPTOR FileHeader = (*finddata).second;
		FileHeader.header.Offset = ftell(m_newfile);
		OptimizeMap.push_back(FileHeader);

		char* databuff;
		databuff = new char[(*finddata).second.header.CompressedSize];
		FILE* m_pIpfFile;
		fopen_s(&m_pIpfFile, g_gameRootPath + data.strFilePath, "rb+");
		fseek(m_pIpfFile, (*finddata).second.header.Offset, SEEK_SET);
		fread(databuff, (*finddata).second.header.CompressedSize, 1,m_pIpfFile);
		fclose(m_pIpfFile);

		fwrite(databuff, (*finddata).second.header.CompressedSize, 1, m_newfile);

		delete databuff;
	}
}

void CIpfManager::Close()
{
	fclose(m_newfile);
	OptimizeMap.clear();
	m_newfile = NULL;
}