// ipf.h

#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

namespace imcIpf {
	class IPartialLoadIpf;
}

namespace imc {

	public ref class IPF
	{
	public:
		IPF();
		~IPF();

		static void Init();
		static void Exit();

		bool AddFileFromDirectory(String^ dir);
		bool AddFileFromFile(String^ dir, String^ name);
		bool AddFileFromFile(String^ dir, String^ name, int maxsize);
		void AddRemoveFileName(String^ ipfname, String^ name);

		List<String^>^ GetRemoveDataNames();
		List<String^>^ GetDataNames();
		List<String^>^ GetDataNames(String^ name);

		void ChangeVersion(String^ ipfname, int start, int end);
		int GetEndVersion();
		int GetStartVersion();

		bool CreatePartialLoadIpf(String^ name, int start, int end);
		bool CreateReleaseIpf(String^ name, bool usePassWord);
		bool LoadPartialIpf(String^ name);
		bool LoadReleaseIpf(String^ name);

		void SaveToFile(String^ name, String^ directory);

		void ExtractAllToFile(String^ outputDirectory);
		void ExtractToFile(String^ ipfInternalPath, String^ outputDirectory);

		void CloseIpf();

	protected:
		imcIpf::IPartialLoadIpf* m_pIpf;
	};
}
