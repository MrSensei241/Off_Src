#ifndef __TESTOBJECT
#define __TESTOBJECT

class CTestObject:public CBaseObject
{
public:
	CBaseObject*			Clone();
};

inline CBaseObject* CTestObject::Clone()
{
	return new CTestObject;
}

#endif
