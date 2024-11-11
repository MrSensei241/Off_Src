// R1BossRun.cpp : 기본 프로젝트 파일입니다.

#include "stdafx.h"
#include "MainForm.h"
#include "util.h"

using namespace R1BossRun;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// 컨트롤이 만들어지기 전에 Windows XP 시각 효과를 활성화합니다.
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	 // AllocConsole();

	wchar_t moduleFullPath[MAX_PATH];
	::GetModuleFileName(NULL, moduleFullPath, MAX_PATH);
	System::String^ mFileName = gcnew System::String(moduleFullPath);
	System::String^ binPath  = System::IO::Path::GetDirectoryName(mFileName);
	System::String^ r1Path = System::IO::Path::GetDirectoryName(binPath);


	MainForm^ mainForm = gcnew MainForm();
	mainForm->SetBinPath(binPath + "\\");
	mainForm->SetDataPath(r1Path + "\\data\\");
	mainForm->Init();
	// 주 창을 만들어 실행합니다.
	Application::Run(mainForm);
	return 0;
}
