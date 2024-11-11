#pragma once

#include "util.h"

namespace R1BossRun {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	
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
	private: System::Windows::Forms::WebBrowser^  rankWeb;
	private: System::Windows::Forms::Label^  dbg_text;
	private: System::Windows::Forms::ComboBox^  jobdropList;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  input_name;
	private: System::Windows::Forms::Label^  label2;

	private: System::Windows::Forms::Button^  test_button;


	protected: 

	public:
		void Init();
		void SetBinPath(String^ a);
		void SetDataPath(String^ a);
		void SetIni(String^ iniName, String^ value);
		String^ GetIni(String^ iniName);

		void LoadJobList();
		void LoadWebAddr();

	protected: 
		String^	m_binPath;
		String^	m_dataPath;
		ArrayList m_jobList;
		String^ m_webAddr;

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
			this->rankWeb = (gcnew System::Windows::Forms::WebBrowser());
			this->test_button = (gcnew System::Windows::Forms::Button());
			this->dbg_text = (gcnew System::Windows::Forms::Label());
			this->jobdropList = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->input_name = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// rankWeb
			// 
			this->rankWeb->Location = System::Drawing::Point(9, 53);
			this->rankWeb->MinimumSize = System::Drawing::Size(20, 20);
			this->rankWeb->Name = L"rankWeb";
			this->rankWeb->Size = System::Drawing::Size(669, 516);
			this->rankWeb->TabIndex = 0;
			// 
			// test_button
			// 
			this->test_button->Location = System::Drawing::Point(9, 7);
			this->test_button->Name = L"test_button";
			this->test_button->Size = System::Drawing::Size(140, 39);
			this->test_button->TabIndex = 1;
			this->test_button->Text = L"도전하기";
			this->test_button->UseVisualStyleBackColor = true;
			this->test_button->Click += gcnew System::EventHandler(this, &MainForm::test_button_Click);
			// 
			// dbg_text
			// 
			this->dbg_text->AutoSize = true;
			this->dbg_text->Location = System::Drawing::Point(12, 177);
			this->dbg_text->Name = L"dbg_text";
			this->dbg_text->Size = System::Drawing::Size(0, 12);
			this->dbg_text->TabIndex = 2;
			// 
			// jobdropList
			// 
			this->jobdropList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->jobdropList->FormattingEnabled = true;
			this->jobdropList->Location = System::Drawing::Point(197, 9);
			this->jobdropList->Name = L"jobdropList";
			this->jobdropList->Size = System::Drawing::Size(121, 20);
			this->jobdropList->TabIndex = 3;
			this->jobdropList->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::jobdropList_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(161, 13);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(29, 12);
			this->label1->TabIndex = 4;
			this->label1->Text = L"직업";
			// 
			// input_name
			// 
			this->input_name->Location = System::Drawing::Point(374, 7);
			this->input_name->Name = L"input_name";
			this->input_name->Size = System::Drawing::Size(120, 21);
			this->input_name->TabIndex = 5;
			this->input_name->TextChanged += gcnew System::EventHandler(this, &MainForm::input_name_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(330, 12);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(37, 12);
			this->label2->TabIndex = 4;
			this->label2->Text = L"이름 :";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(682, 579);
			this->Controls->Add(this->input_name);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->jobdropList);
			this->Controls->Add(this->dbg_text);
			this->Controls->Add(this->test_button);
			this->Controls->Add(this->rankWeb);
			this->Name = L"MainForm";
			this->Text = L"R1 보스 랭킹";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		private: System::Void test_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void input_name_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void jobdropList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
};
}
