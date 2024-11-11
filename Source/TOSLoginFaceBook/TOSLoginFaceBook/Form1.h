#pragma once

namespace TOSLoginFaceBook {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1에 대한 요약입니다.
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
	private: System::Windows::Forms::WebBrowser^  m_web;
	protected:

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
			this->m_web = (gcnew System::Windows::Forms::WebBrowser());
			this->SuspendLayout();
			// 
			// m_web
			// 
			this->m_web->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_web->Location = System::Drawing::Point(0, 0);
			this->m_web->MinimumSize = System::Drawing::Size(20, 20);
			this->m_web->Name = L"m_web";
			this->m_web->Size = System::Drawing::Size(657, 479);
			this->m_web->TabIndex = 0;
			this->m_web->DocumentCompleted += gcnew System::Windows::Forms::WebBrowserDocumentCompletedEventHandler(this, &Form1::webBrowser1_DocumentCompleted);
			this->m_web->Navigated += gcnew System::Windows::Forms::WebBrowserNavigatedEventHandler(this, &Form1::webBrowser1_Navigated);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(657, 479);
			this->Controls->Add(this->m_web);
			this->Name = L"Form1";
			this->Text = L"TOS Facebook Login";
			this->ResumeLayout(false);

		}
#pragma endregion

	public:
		void	Init(String^ binPath, String^ dataPath);

	protected:
		String^ m_binPath;
		String^ m_dataPath;
		String^ m_webUrl;
		String^ m_appID;
		String^ m_graphAccessCode;
		String^	m_appSecret;
		String^ m_authState;
		private: System::Void webBrowser1_DocumentCompleted(System::Object^  sender, System::Windows::Forms::WebBrowserDocumentCompletedEventArgs^  e);
		private: System::Void webBrowser1_Navigated(System::Object^  sender, System::Windows::Forms::WebBrowserNavigatedEventArgs^  e);
	};
}

