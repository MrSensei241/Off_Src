#pragma once

namespace TOSLoginFaceBook {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1�� ���� ����Դϴ�.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: ������ �ڵ带 ���⿡ �߰��մϴ�.
			//
		}

	protected:
		/// <summary>
		/// ��� ���� ��� ���ҽ��� �����մϴ�.
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
		/// �ʼ� �����̳� �����Դϴ�.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �����̳� ������ �ʿ��� �޼����Դϴ�.
		/// �� �޼����� ������ �ڵ� ������� �������� ���ʽÿ�.
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

