#include "StdAfx.h"
#include "TestMode.h"
#include "TestWorld.h"
#include "TestView.h"
#include "app.h"

namespace geapp{


CTestView::CTestView()
{

}

CTestView::~CTestView()
{

}

imcd3d9::Camera * CTestView::GetCamera()
{
	//return //app::GetSampleCamera();
	return app::GetCamera();
}


};
