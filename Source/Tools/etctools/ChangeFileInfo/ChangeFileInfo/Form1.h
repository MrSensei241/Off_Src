﻿#pragma once


#include <stdio.h>
#include <string.h>
#include <WTypes.h>

namespace ChangeFileInfo {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
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
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textBox1;

	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Label^  t_ext;
	private: System::Windows::Forms::TextBox^  label_ext;
	protected: 

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
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->t_ext = (gcnew System::Windows::Forms::Label());
			this->label_ext = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(12, 12);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(175, 21);
			this->textBox1->TabIndex = 0;
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &Form1::textBox1_TextChanged);
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->ItemHeight = 12;
			this->listBox1->Location = System::Drawing::Point(12, 71);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(783, 124);
			this->listBox1->TabIndex = 2;
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(193, 12);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(196, 21);
			this->textBox2->TabIndex = 1;
			// 
			// t_ext
			// 
			this->t_ext->AutoSize = true;
			this->t_ext->Location = System::Drawing::Point(13, 40);
			this->t_ext->Name = L"t_ext";
			this->t_ext->Size = System::Drawing::Size(41, 12);
			this->t_ext->TabIndex = 3;
			this->t_ext->Text = L"확장자";
			// 
			// label_ext
			// 
			this->label_ext->Location = System::Drawing::Point(73, 40);
			this->label_ext->Name = L"label_ext";
			this->label_ext->Size = System::Drawing::Size(100, 21);
			this->label_ext->TabIndex = 4;
			// 
			// Form1
			// 
			this->AllowDrop = true;
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(807, 209);
			this->Controls->Add(this->label_ext);
			this->Controls->Add(this->t_ext);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->listBox1);
			this->Controls->Add(this->textBox1);
			this->Name = L"Form1";
			this->Text = L"동진씨";
			this->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::Form1_DragDrop);
			this->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::Form1_DragOver);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e)
				 {

				 }

				 array<String^>^ GetDirectoryFilePaths(String^ DirectoryPath, String^ ext)
				 {
					 if (false == IO::Directory::Exists(DirectoryPath))
					 {
						 return nullptr;
					 }

					 ArrayList^ pFilepaths = gcnew ArrayList;

					 String^ MultipleFilter = "*." + ext;
					 array<String^>^ FileFilters = MultipleFilter->Split('|');
					 for each (String^ FileFilter in FileFilters)
						 pFilepaths->AddRange(IO::Directory::GetFiles(DirectoryPath, FileFilter, IO::SearchOption::AllDirectories));

					 return reinterpret_cast<array<String^>^>(pFilepaths->ToArray(String::typeid));
				 }


				 #define LOGS(x)  DbgLog(__FILE__ + " " + __LINE__); DbgLog("["+##x+"]");  

	void DbgLog(String^ log)
	{
		listBox1->Items->Add(log);
	}

	char * StringToChar(System::String^ input)
	{
		return (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(input);
	}

	String^ FileGets(FILE* pFile)
	{
		char tmp[2048];
		if (NULL == fgets(tmp, 2048, pFile))
			return nullptr;

		int len = strlen(tmp);
		int idx = 1;
		while(true)
		{
			if ( (tmp[len - idx] != 10)
				&&
				(tmp[len - idx] != 13))
			{
				tmp[len - idx + 1] = '\0';
				break;
			};

			idx++;
		}

		int strLen = strlen(tmp);
		WCHAR	uChar[2048];
		MultiByteToWideChar(CP_ACP, 0, tmp, -1, uChar, strLen + 1);
		return gcnew String ( uChar );
	}



	void ExecChangeFromPath(String^ path)
	{
		array<String^>^ names = GetDirectoryFilePaths(path, label_ext->Text);
		listBox1->Items->Clear();

		String^ from = textBox1->Text;
		String^ to = textBox2->Text;
		if (from == "")
			return;

		for each (String^ fullPath in names)
		{
			/*String^ fileName = System::IO::Path::GetFileName(fullPath);
			String^ filePath = System::IO::Path::GetDirectoryName(fullPath);
			if (fileName->Contains(from))
			{
				String^ replaced = fileName->Replace(from, to);
				String^ renammed = filePath + "\\" + replaced;
				System::IO::File::Move( fullPath, renammed);
				String^ logText = fileName  + " -> " + renammed;
				DbgLog(logText);
				//String^ reName = filepath->
			}*/
			DbgLog(fullPath);
			FILE* fp = fopen(StringToChar(fullPath), "r");
			int idx = 0;
			String^ fullStr = "";
			if (fp)
			{
				while(true)
				{
					String^ str = FileGets(fp);
					if (!str)
						break;

					fullStr = fullStr + str;
					fullStr = fullStr + "\r\n";
				}
				fclose(fp);
			}

			String^ replaced = fullStr->Replace(from, to);
			FILE* wfp = fopen(StringToChar(fullPath), "wb");
			if (wfp)
			{
				fputs(StringToChar(replaced), wfp);
				fclose(wfp);
			}

			

			

			//LOGS(filepath);
		}

		//System::IO::File::
	}

	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				 if (folderBrowserDialog1->ShowDialog() != System::Windows::Forms::DialogResult::OK)
					 return;


				 String^ path = folderBrowserDialog1->SelectedPath;
				 ExecChangeFromPath(path);

			 }
		private: System::Void Form1_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
			 }
private: System::Void Form1_DragOver(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
		 }

		 public:
		 virtual void		OnDragEnter(System::Windows::Forms::DragEventArgs^  e) override
		 {
			 if (e->Data->GetDataPresent(DataFormats::FileDrop))
			 {
				 e->Effect = DragDropEffects::Move;
			 }
		 }
		 virtual void		OnDragDrop(System::Windows::Forms::DragEventArgs^  e) override
		 {
			 if (e->Data->GetDataPresent(DataFormats::FileDrop))
			 {
				 array<String^>^ filenames = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);
				 
				 for each (String^ path in filenames)
				 {
					 ExecChangeFromPath(path);
				 }
			 }
		 }
};
}

