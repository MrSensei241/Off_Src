#ifndef ____FUNCTION___EXTRACTOR___
#define ____FUNCTION___EXTRACTOR___

namespace imc
{
	void TestMergeEffect(const char * pFileNamePath, const char * pFileName);
	void RegisterEffectCmds(imc::CVector<std::string>& list);
	imc::CVector<std::string>* GetCmdsList();
};

#endif // __CLASS
