// 기본 DLL 파일입니다.

#include "stdafx.h"
#include "Windows.h"

#include "ipf.h"
#include "imcIpf.h"

namespace imc {

IPF::IPF()
{
	m_pIpf = 0;
}

IPF::~IPF()
{

}

void IPF::Init()
{
	imcIpf::Init();
}

void IPF::Exit()
{
	imcIpf::Exit();
}

bool IPF::AddFileFromDirectory(String^ dir)
{
	if (m_pIpf) {
		char* _dir = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dir);

		m_pIpf->AddFileFromDirectory(_dir);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_dir));

		return true;
	}

	return false;
}

bool IPF::AddFileFromFile(String^ dir, String^ name)
{
	if (m_pIpf) {		
		if(name->Contains(" ") == false) // 빈칸있으면 여기서 실패한다
		{
			
			auto chararr = name->ToCharArray();
			for(int i = 0 ; i < name->Length ; ++i)
			{				
				auto chk = chararr[i];
				if(chk > 127)  // 한글이면 여기서 실패한다
				{					
						return false;
				}
			}			
			
			{
				char* _dir = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dir);
				char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

				m_pIpf->AddFileFromFile(_dir, _name);

				System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));
				System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_dir));

				return true;	
			}
		}
		
	}

	return false;
}

bool IPF::AddFileFromFile(String^ dir, String^ name, int maxsize)
{
	if (m_pIpf) {
		char* _dir = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dir);
		char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

		bool ret = m_pIpf->AddFileFromFile(_dir, _name, maxsize);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_dir));

		return ret;
	}

	return false;
}

void IPF::AddRemoveFileName(String^ ipfname, String^ name)
{
	if (m_pIpf) {
		char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);
		char* _ipf = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ipfname);
		m_pIpf->AddRemoveData(_ipf, _name);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_ipf));
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));
	}
}

void IPF::ChangeVersion(String^ ipfname, int start, int end)
{
	char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ipfname);

	if (ipfname->IndexOf(".ipf") != -1) {
		imcIpf::IPartialLoadIpf* pIpf = imcIpf::OpenPartialLoadIpf(_name);
		if (pIpf) {
			pIpf->ChangeVersion(start, end);
			imcIpf::CloseIpf(pIpf);
		}
	}

	System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));
}

void IPF::CloseIpf()
{
	if (m_pIpf) {
		m_pIpf->EndAddFiles();
		imcIpf::CloseIpf(m_pIpf);
		m_pIpf = 0;
	}
}

List<String^>^ IPF::GetRemoveDataNames()
{
	List<String^>^ _list = gcnew List<String^>();

	if (m_pIpf) {
		char (*pArray)[260];
		size_t arraySize;
		m_pIpf->GetRemoveFile(&pArray, &arraySize);

		for (size_t i = 0; i < arraySize; ++i) {
			String^ filename = System::Runtime::InteropServices::Marshal::PtrToStringAnsi((System::IntPtr)(char*)(pArray + i));
			_list->Add(filename);
		}
	}

	return _list;
}

List<String^>^ IPF::GetDataNames()
{
	List<String^>^ _list = gcnew List<String^>();

	if (m_pIpf) {
		char (*pArray)[260];
		size_t arraySize;
		m_pIpf->GetInternalPath("", &pArray, &arraySize);

		for (size_t i = 0; i < arraySize; ++i) {
			String^ filename = System::Runtime::InteropServices::Marshal::PtrToStringAnsi((System::IntPtr)(char*)(pArray + i));

			//m_pIpf->GetFileInfo((char*)(pArray + i), NULL, NULL, NULL, NULL, NULL);
			_list->Add(filename);
		}
	}

	return _list;
}

List<String^>^ IPF::GetDataNames(String^ name)
{
	List<String^>^ _list = gcnew List<String^>();

	if (m_pIpf) {
		char (*pArray)[260];
		size_t arraySize;
		char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);
		m_pIpf->GetInternalPath(_name, &pArray, &arraySize);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));

		for (size_t i = 0; i < arraySize; ++i) {
			String^ filename = System::Runtime::InteropServices::Marshal::PtrToStringAnsi((System::IntPtr)(char*)(pArray + i));

			//m_pIpf->GetFileInfo((char*)(pArray + i), NULL, NULL, NULL, NULL, NULL);
			_list->Add(filename);
		}
	}

	return _list;
}

int IPF::GetEndVersion()
{
	if (!m_pIpf) {
		return 0;
	}

	unsigned long start, end;
	m_pIpf->GetVersion(start, end);

	return end;
}

int IPF::GetStartVersion()
{
	if (!m_pIpf) {
		return 0;
	}

	unsigned long start, end;
	m_pIpf->GetVersion(start, end);

	return start;
}

bool IPF::CreatePartialLoadIpf(String^ name, int start, int end)
{
	if (m_pIpf) {
		return false;
	}

	char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);
	
	m_pIpf = imcIpf::CreatePartialLoadIpf(_name, start, end);
	m_pIpf->BeginAddFiles();

	System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));

	return true;
}

bool IPF::CreateReleaseIpf(String^ name, bool usePassWord)
{
	if (m_pIpf) {
		return false;
	}

	char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

	m_pIpf = imcIpf::CreatePatchLoadIpf(_name, usePassWord);
	m_pIpf->BeginAddFiles();

	System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));

	return true;
}

bool IPF::LoadPartialIpf(String^ name)
{
	if (m_pIpf) {
		return false;
	}

	char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

	m_pIpf = imcIpf::OpenPartialLoadIpf(_name);

	System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));

	return true;
}

bool IPF::LoadReleaseIpf(String^ name)
{
	if (m_pIpf) {
		return false;
	}

	char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

	m_pIpf = imcIpf::OpenPatchLoadIpf(_name);

	System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));

	return true;
}

void IPF::SaveToFile(String^ name, String^ directory)
{
	if (m_pIpf) {
		char* _dir = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(directory);

		if (System::String::IsNullOrEmpty(name) == true) {
			char* _name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);
			m_pIpf->ExtractToFile(_name, _dir, _dir);
			System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_name));
		} else {
			m_pIpf->ExtractAllToFile(_dir);
		}

		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_dir));
	}
}

void IPF::ExtractAllToFile(String^ outputDirectory)
{
	if (m_pIpf) {
		char* _dir = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(outputDirectory);
		m_pIpf->ExtractAllToFile(_dir);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_dir));
	}
}

void IPF::ExtractToFile(String^ ipfInternalPath, String^ outputDirectory)
{
	if (m_pIpf) {
		char* _dir = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(outputDirectory);
		char* _path = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ipfInternalPath);
		m_pIpf->ExtractToFile(_path, _dir, _dir);
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_path));
		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr(_dir));
	}
}

}