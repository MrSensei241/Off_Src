#include "stdafx.h"
#include <vld.h>

class CNode {
public:
	virtual ~CNode() {
		printf("~CNode()");
	}
};

class CBase {
public:
	CBase() {}
	//virtual ~CBase(){ printf("~CBase()"); }
};

class CParticle : public CBase {
public:
	//CBase 에 가상 소멸자가 없을 경우 CParticle 의 맴버변수의 소멸자도 불려지지 않는다.
	std::string	m_ptclName;
	CNode m_node;
	
	CParticle() {}
	//virtual ~CParticle(){ printf("~CParticle()"); }
};

typedef imc::CVector<CBase*> EventMap;
EventMap m_eventMap;

void CreateEvent()
{
	CParticle* pEvent	= new CParticle;
	pEvent->m_ptclName  = "particle";
	m_eventMap.AddToTail(pEvent);
}

void DestoryEvent()
{
	int count = m_eventMap.Count();
	for(int i = 0; i < count; ++i) {	
		//CParticle * pParticle = (CParticle *)m_eventMap[i];
		//SAFE_DELETE(pParticle);
		SAFE_DELETE(m_eventMap[i]);
	}
	m_eventMap.RemoveAll();
}

int main(int argc, char* argv[]) 
{
	CreateEvent();	
	DestoryEvent();
	return 0;
}










