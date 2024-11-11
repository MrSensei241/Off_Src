#ifndef __TESTMODE
#define __TESTMODE

#include "GeMode.h"
#include "TestWorld.h"
#include "TestView.h"

namespace geapp
{

class CTestMode:public CMode
{
public: 
	CTestMode() {};
	DECLARE_MODE(CTestMode)
	
	void		Init(const char* worldFileName);
	BOOL		Update();
	void		Run() override;
protected:
	BOOL		LoadZone(BOOL enableSubDir, gepath::DIRTYPE bgDirType, const char * fileName) override;

};

inline CTestWorld* TestWorld()
{
	return ((CTestWorld*)World());
}

inline CTestView* TestView()
{
	return ((CTestView*)View());
}

inline CTestMode* TestMode()
{
	return ((CTestMode*)Mode());
}

};

#endif