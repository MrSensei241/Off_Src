// TOSLoginFaceBook.cpp : �⺻ ������Ʈ �����Դϴ�.

#include "stdafx.h"
#include "Form1.h"

using namespace TOSLoginFaceBook;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// ��Ʈ���� ��������� ���� Windows XP �ð� ȿ���� Ȱ��ȭ�մϴ�.
	AllocConsole();

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	wchar_t moduleFullPath[MAX_PATH];
	::GetModuleFileName(NULL, moduleFullPath, MAX_PATH);
	System::String^ mFileName = gcnew System::String(moduleFullPath);
	System::String^ binPath = System::IO::Path::GetDirectoryName(mFileName);
	System::String^ r1Path = System::IO::Path::GetDirectoryName(binPath);

	Form1^ mainForm = gcnew Form1();
	mainForm->Init(binPath + "\\", r1Path + "\\data\\");
	// �� â�� ����� �����մϴ�.
	Application::Run(mainForm);
	return 0;
}
