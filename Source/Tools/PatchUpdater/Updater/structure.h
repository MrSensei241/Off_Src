#ifndef structure_h__
#define structure_h__

#pragma pack(1)

struct FILE_HEADER
{
	WORD FilenameLength;
	DWORD CRC32;
	DWORD CompressedSize;
	DWORD UnCompressedSize;
	DWORD Offset;
	WORD IPFnameLength;
};

struct IPFInfo
{
	WORD DataCount;
	DWORD DataOffset;
	WORD RemoveCount;
	DWORD RemoveOffset;
	DWORD _Signature;
	WORD Start;
	WORD End;
};

struct REMOVE_FILE_HEADER
{
	WORD IPFnameLength;
	WORD FilenameLength;
};

#pragma pack()

struct DATA_DESCRIPTOR
{
	FILE_HEADER header;

	std::string ipfname;
	std::string dataname;
	DWORD		Offset;
};

struct REMOVE_DATA_DESCRIPTOR
{
	REMOVE_FILE_HEADER header;

	std::string dataname;
	std::string ipfname;
};

struct CACHE_DATA
{
	CString		strIpfName;
	CString		strDataName;
	CString		strFilePath;
	WORD		strVersion;

	CACHE_DATA() : strIpfName(""), strDataName(""), strFilePath(""), strVersion(0) { }
	CACHE_DATA(CString _strIpfName, CString _strDataName, CString _strFilePath, WORD _strVersion) 
	{
		strIpfName	= _strIpfName;
		strDataName = _strDataName;
		strFilePath = _strFilePath;
		strVersion	= _strVersion;
	}
};

struct UPDATE_INFO
{
	DWORD dwHistory;
	DWORD dwMultiNum;
	CString strFileName;
	CString strFilePath;
	CString strSavePath;

	UPDATE_INFO() : dwHistory(0), dwMultiNum(0), strFileName(""), strFilePath(""), strSavePath("") { }
	UPDATE_INFO(const DWORD dwHistoryNum, const DWORD dwMultiNumber = 0, const CString name = ""
		, const CString path = "", const CString savepath = "") 
	{
		dwHistory	= dwHistoryNum;
		dwMultiNum	= dwMultiNumber;
		strFileName = name;
		strFilePath = path;
		strSavePath = savepath;
	}
};

#endif // structure_h__