#ifndef ____FUNCTION___EXTRACTOR___
#define ____FUNCTION___EXTRACTOR___

namespace imc
{
	struct FUNCTION_INFO
	{
		imc::CStringID	Name;
		imc::CStringID	Params;
		imc::CStringID	Returns;
		imc::CStringID	comment;
		imc::CStringID	Description;
		imc::CStringID	LastDate;
		int				LastRevision;
		imc::CStringID	LastEditor;
		imc::CStringID	FileName;

		inline bool operator < (const FUNCTION_INFO& rhs) const
		{
			//if (LastRevision != rhs.LastRevision)
				//return LastRevision < rhs.LastRevision;
			//printf("\n [%s][%s][%d]", LastDate.c_str(), rhs.LastDate.c_str(), strcmp(LastDate.c_str(), rhs.LastDate.c_str()) < 0);
			return (strcmp(LastDate.c_str(), rhs.LastDate.c_str()) > 0);
		}

	};

	typedef imc::CVector<FUNCTION_INFO*>	FUNCTION_LIST;

	void ExtactFunctions(const char * pFileNamePath, const char * pFileName);
	FUNCTION_LIST*	GetFuncInfoList();
	void MakeFileHeader(FILE* pfile);
	void MakeFileTail(FILE* pFile);
	void MakeTableHeader(FILE* pFile);
	void MakeTableInfo(FUNCTION_INFO* pInfo, FILE* pFile);

};


#endif // __CLASS
