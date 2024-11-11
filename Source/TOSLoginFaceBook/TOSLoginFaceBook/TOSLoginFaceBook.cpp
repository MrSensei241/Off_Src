// TOSLoginFaceBook.cpp : 기본 프로젝트 파일입니다.

#include "stdafx.h"
#include "Form1.h"

using namespace TOSLoginFaceBook;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// 컨트롤이 만들어지기 전에 Windows XP 시각 효과를 활성화합니다.
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
	// 주 창을 만들어 실행합니다.
	Application::Run(mainForm);
	return 0;
}
