#pragma once

class CUnitTestInit
{
public:
	CUnitTestInit(void);
	~CUnitTestInit(void);
public:
	void		InitTest();
	void		ExitTest();

private:
	void		InitIES();
	void		InitWin32();

private:
	void		ExitIES();
	void		ExitWin32();
};
