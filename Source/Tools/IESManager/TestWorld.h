#ifndef __TESTWORLD
#define __TESTWORLD

#include "TestObject.h"

namespace geapp{

class CTestWorld:public CWorld
{
public:
	CTestWorld();
	virtual ~CTestWorld();

	void	OnCreate(const char* worldFileName, const char* renderFileName, const char* pMtlfileName, const char* pLightFileName);
};
};



#endif