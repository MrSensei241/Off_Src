#ifndef __LDPC_H__
#define __LDPC_H__

#include "GePC.h"

class CLDPC : public CPC
{
public:
	CLDPC();
	virtual ~CLDPC();

	int GetIFP();
	void SetIFP(int ifp);

protected:

	int m_IFP;

};

#endif