
// SmtpTester.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CSmtpTesterApp:
// �� Ŭ������ ������ ���ؼ��� SmtpTester.cpp�� �����Ͻʽÿ�.
//

class CSmtpTesterApp : public CWinApp
{
public:
	CSmtpTesterApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CSmtpTesterApp theApp;