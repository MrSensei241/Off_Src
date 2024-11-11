// UnitTestAll.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <fstream>
#include <string>

#include "UnitTestInit.h"
#include "imcDirMgr.h"

int _tmain(int argc, _TCHAR* argv[])
{
	int		testResult = 0;

	std::string strTestPath = imc::GetBinPath();
	imccore::Init(std::string(strTestPath + "test\\CONFIG_EXAMPLE.xml").c_str());

	CUnitTestInit testInit;
	testInit.InitTest();

	if (argc > 1) {
		std::ofstream f(argv[1]);
		UnitTest::XmlTestReporter	reporter(f);
		testResult = UnitTest::RunAllTests(reporter, UnitTest::Test::GetTestList(), 0);
	}
	else
		testResult = UnitTest::RunAllTests();

	testInit.ExitTest();
	imccore::Exit();

	return testResult;
}