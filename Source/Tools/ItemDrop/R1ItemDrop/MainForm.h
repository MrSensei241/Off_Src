#pragma once

#include "util.h"

namespace R1ItemDrop {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Xml;
	
	/// <summary>
	/// MainForm에 대한 요약입니다.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
			
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}











	protected: 

	public:
		void Init();
		void SetBinPath(String^ a);
		void SetDataPath(String^ a);
		void SetIni(String^ iniName, String^ value);
		String^ GetIni(String^ iniName, String^ defValue);

	protected: 
		String^	m_binPath;
		String^	m_dataPath;
		ArrayList m_jobList;
		String^ m_webAddr;
		XmlDocument^ m_monsterDoc;
		ArrayList m_monClsList;




	protected: 

	protected: 


	protected: 






















	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;














	private: System::Windows::Forms::OpenFileDialog^  textureDlg;







	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->textureDlg = (gcnew System::Windows::Forms::OpenFileDialog());
			this->SuspendLayout();
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// textureDlg
			// 
			this->textureDlg->FileName = L"openFileDialog1";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1062, 376);
			this->Name = L"MainForm";
			this->Text = L"R1 몬스터 등록";
			this->ResumeLayout(false);

		}
#pragma endregion

	
};
}
