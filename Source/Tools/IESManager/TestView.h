#ifndef __TESTVIEW
#define __TESTVIEW


namespace geapp
{

class CTestView
	: public CView
{
public:

	CTestView();
	virtual					~CTestView();

	imcd3d9::Camera *		GetCamera() override;
};


};
#endif