#pragma once

#include "util.h"

namespace R1MonRegister {

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

		void UpdateCategory();
		void Register();
		String^ SaveXAC(String^ xacPath, String^ texturePath, int * xacID);
		String^ SaveModelList(int xacID, String^ xacName, int faceXacID);

	protected: 
		String^	m_binPath;
		String^	m_dataPath;
		ArrayList m_jobList;
		String^ m_webAddr;
		XmlDocument^ m_monsterDoc;
		ArrayList m_monClsList;

	private: System::Windows::Forms::TextBox^  m_className;
	private: System::Windows::Forms::ComboBox^  m_category;
	private: System::Windows::Forms::Label^  m_tClassName;
	protected: 

	protected: 


	protected: 



	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  m_name;

	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  m_mspd;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::ComboBox^  m_rotate;



	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  m_customAttack;

	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::TextBox^  m_customLife;

	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::TextBox^  m_xac;
	private: System::Windows::Forms::Button^  m_btnXac;


	private: System::Windows::Forms::TextBox^  m_animPath;
	private: System::Windows::Forms::Button^  m_btnAnimPath;


	private: System::Windows::Forms::TextBox^  m_texturePath;
	private: System::Windows::Forms::Button^  m_btnTexture;
	private: System::Windows::Forms::Button^  m_execRegister;



	private: System::Windows::Forms::OpenFileDialog^  textureDlg;
	private: System::Windows::Forms::ComboBox^  m_monRank;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ComboBox^  m_size;
private: System::Windows::Forms::TextBox^  m_scale;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::ComboBox^  m_fileName;
private: System::Windows::Forms::TextBox^  m_xac_face;

private: System::Windows::Forms::Button^  button1;
private: System::Windows::Forms::Label^  label11;



private: System::Windows::Forms::TextBox^  textBox2;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::TextBox^  textBox1;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label14;






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
			this->m_className = (gcnew System::Windows::Forms::TextBox());
			this->m_category = (gcnew System::Windows::Forms::ComboBox());
			this->m_tClassName = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->m_name = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->m_mspd = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->m_rotate = (gcnew System::Windows::Forms::ComboBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->m_customAttack = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->m_customLife = (gcnew System::Windows::Forms::TextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->m_xac = (gcnew System::Windows::Forms::TextBox());
			this->m_btnXac = (gcnew System::Windows::Forms::Button());
			this->m_animPath = (gcnew System::Windows::Forms::TextBox());
			this->m_btnAnimPath = (gcnew System::Windows::Forms::Button());
			this->m_texturePath = (gcnew System::Windows::Forms::TextBox());
			this->m_btnTexture = (gcnew System::Windows::Forms::Button());
			this->m_execRegister = (gcnew System::Windows::Forms::Button());
			this->textureDlg = (gcnew System::Windows::Forms::OpenFileDialog());
			this->m_monRank = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->m_size = (gcnew System::Windows::Forms::ComboBox());
			this->m_scale = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->m_fileName = (gcnew System::Windows::Forms::ComboBox());
			this->m_xac_face = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// m_className
			// 
			this->m_className->Location = System::Drawing::Point(107, 61);
			this->m_className->Name = L"m_className";
			this->m_className->Size = System::Drawing::Size(156, 21);
			this->m_className->TabIndex = 0;
			this->m_className->TextChanged += gcnew System::EventHandler(this, &MainForm::m_className_TextChanged);
			// 
			// m_category
			// 
			this->m_category->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_category->FormattingEnabled = true;
			this->m_category->Location = System::Drawing::Point(107, 37);
			this->m_category->Name = L"m_category";
			this->m_category->Size = System::Drawing::Size(156, 20);
			this->m_category->TabIndex = 0;
			// 
			// m_tClassName
			// 
			this->m_tClassName->AutoSize = true;
			this->m_tClassName->ForeColor = System::Drawing::SystemColors::ControlText;
			this->m_tClassName->Location = System::Drawing::Point(11, 65);
			this->m_tClassName->Name = L"m_tClassName";
			this->m_tClassName->Size = System::Drawing::Size(80, 12);
			this->m_tClassName->TabIndex = 1;
			this->m_tClassName->Text = L"ClassName :";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(11, 40);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(61, 12);
			this->label3->TabIndex = 1;
			this->label3->Text = L"카테고리 :";
			// 
			// m_name
			// 
			this->m_name->Location = System::Drawing::Point(107, 86);
			this->m_name->Name = L"m_name";
			this->m_name->Size = System::Drawing::Size(156, 21);
			this->m_name->TabIndex = 1;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(11, 90);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(37, 12);
			this->label4->TabIndex = 1;
			this->label4->Text = L"이름 :";
			// 
			// m_mspd
			// 
			this->m_mspd->Location = System::Drawing::Point(107, 136);
			this->m_mspd->Name = L"m_mspd";
			this->m_mspd->Size = System::Drawing::Size(156, 21);
			this->m_mspd->TabIndex = 6;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(11, 139);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(61, 12);
			this->label5->TabIndex = 1;
			this->label5->Text = L"이동속도 :";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(11, 165);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(61, 12);
			this->label7->TabIndex = 1;
			this->label7->Text = L"방향전환 :";
			// 
			// m_rotate
			// 
			this->m_rotate->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_rotate->FormattingEnabled = true;
			this->m_rotate->Location = System::Drawing::Point(107, 162);
			this->m_rotate->Name = L"m_rotate";
			this->m_rotate->Size = System::Drawing::Size(156, 20);
			this->m_rotate->TabIndex = 8;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(11, 220);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(66, 12);
			this->label6->TabIndex = 1;
			this->label6->Text = L"MonRank :";
			// 
			// m_customAttack
			// 
			this->m_customAttack->Location = System::Drawing::Point(107, 243);
			this->m_customAttack->Name = L"m_customAttack";
			this->m_customAttack->Size = System::Drawing::Size(156, 21);
			this->m_customAttack->TabIndex = 14;
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(10, 247);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(91, 12);
			this->label8->TabIndex = 1;
			this->label8->Text = L"CustomAttack :";
			// 
			// m_customLife
			// 
			this->m_customLife->Location = System::Drawing::Point(107, 267);
			this->m_customLife->Name = L"m_customLife";
			this->m_customLife->Size = System::Drawing::Size(156, 21);
			this->m_customLife->TabIndex = 16;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(11, 271);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(77, 12);
			this->label9->TabIndex = 1;
			this->label9->Text = L"CustomLife :";
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// m_xac
			// 
			this->m_xac->Location = System::Drawing::Point(376, 15);
			this->m_xac->Name = L"m_xac";
			this->m_xac->Size = System::Drawing::Size(156, 21);
			this->m_xac->TabIndex = 20;
			// 
			// m_btnXac
			// 
			this->m_btnXac->Location = System::Drawing::Point(277, 12);
			this->m_btnXac->Name = L"m_btnXac";
			this->m_btnXac->Size = System::Drawing::Size(92, 23);
			this->m_btnXac->TabIndex = 18;
			this->m_btnXac->Text = L"XAC";
			this->m_btnXac->UseVisualStyleBackColor = true;
			this->m_btnXac->Click += gcnew System::EventHandler(this, &MainForm::m_btnXac_Click);
			// 
			// m_animPath
			// 
			this->m_animPath->Location = System::Drawing::Point(376, 68);
			this->m_animPath->Name = L"m_animPath";
			this->m_animPath->Size = System::Drawing::Size(156, 21);
			this->m_animPath->TabIndex = 28;
			// 
			// m_btnAnimPath
			// 
			this->m_btnAnimPath->Location = System::Drawing::Point(277, 65);
			this->m_btnAnimPath->Name = L"m_btnAnimPath";
			this->m_btnAnimPath->Size = System::Drawing::Size(92, 23);
			this->m_btnAnimPath->TabIndex = 26;
			this->m_btnAnimPath->Text = L"애니폴더";
			this->m_btnAnimPath->UseVisualStyleBackColor = true;
			this->m_btnAnimPath->Click += gcnew System::EventHandler(this, &MainForm::m_btnAnimPath_Click);
			// 
			// m_texturePath
			// 
			this->m_texturePath->Location = System::Drawing::Point(376, 43);
			this->m_texturePath->Name = L"m_texturePath";
			this->m_texturePath->Size = System::Drawing::Size(156, 21);
			this->m_texturePath->TabIndex = 24;
			// 
			// m_btnTexture
			// 
			this->m_btnTexture->Location = System::Drawing::Point(277, 40);
			this->m_btnTexture->Name = L"m_btnTexture";
			this->m_btnTexture->Size = System::Drawing::Size(92, 23);
			this->m_btnTexture->TabIndex = 22;
			this->m_btnTexture->Text = L"텍스쳐경로";
			this->m_btnTexture->UseVisualStyleBackColor = true;
			this->m_btnTexture->Click += gcnew System::EventHandler(this, &MainForm::m_btnTexture_Click);
			// 
			// m_execRegister
			// 
			this->m_execRegister->Location = System::Drawing::Point(286, 243);
			this->m_execRegister->Name = L"m_execRegister";
			this->m_execRegister->Size = System::Drawing::Size(245, 45);
			this->m_execRegister->TabIndex = 30;
			this->m_execRegister->Text = L"등록";
			this->m_execRegister->UseVisualStyleBackColor = true;
			this->m_execRegister->Click += gcnew System::EventHandler(this, &MainForm::m_execRegister_Click);
			// 
			// textureDlg
			// 
			this->textureDlg->FileName = L"openFileDialog1";
			// 
			// m_monRank
			// 
			this->m_monRank->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_monRank->FormattingEnabled = true;
			this->m_monRank->Location = System::Drawing::Point(107, 217);
			this->m_monRank->Name = L"m_monRank";
			this->m_monRank->Size = System::Drawing::Size(156, 20);
			this->m_monRank->TabIndex = 12;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(11, 194);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(38, 12);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Size :";
			// 
			// m_size
			// 
			this->m_size->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_size->FormattingEnabled = true;
			this->m_size->Location = System::Drawing::Point(107, 191);
			this->m_size->Name = L"m_size";
			this->m_size->Size = System::Drawing::Size(156, 20);
			this->m_size->TabIndex = 10;
			// 
			// m_scale
			// 
			this->m_scale->Location = System::Drawing::Point(107, 111);
			this->m_scale->Name = L"m_scale";
			this->m_scale->Size = System::Drawing::Size(156, 21);
			this->m_scale->TabIndex = 4;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(11, 114);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(49, 12);
			this->label2->TabIndex = 1;
			this->label2->Text = L"스케일 :";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(11, 15);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(37, 12);
			this->label10->TabIndex = 1;
			this->label10->Text = L"파일 :";
			// 
			// m_fileName
			// 
			this->m_fileName->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_fileName->FormattingEnabled = true;
			this->m_fileName->Location = System::Drawing::Point(107, 12);
			this->m_fileName->Name = L"m_fileName";
			this->m_fileName->Size = System::Drawing::Size(156, 20);
			this->m_fileName->TabIndex = 0;
			this->m_fileName->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::m_fileName_SelectedIndexChanged);
			// 
			// m_xac_face
			// 
			this->m_xac_face->Location = System::Drawing::Point(376, 207);
			this->m_xac_face->Name = L"m_xac_face";
			this->m_xac_face->Size = System::Drawing::Size(156, 21);
			this->m_xac_face->TabIndex = 20;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(277, 204);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(92, 23);
			this->button1->TabIndex = 18;
			this->button1->Text = L"XAC_머리";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MainForm::m_btnXac_Click);
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(270, 186);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(87, 12);
			this->label11->TabIndex = 31;
			this->label11->Text = L"NPC 전용 메뉴";
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(376, 130);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(156, 21);
			this->textBox2->TabIndex = 35;
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(284, 111);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(156, 12);
			this->label12->TabIndex = 36;
			this->label12->Text = L"칼럼 복사 대상 ClassName";
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(376, 157);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(156, 21);
			this->textBox1->TabIndex = 37;
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(284, 135);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(76, 12);
			this->label13->TabIndex = 38;
			this->label13->Text = L"monster.xml";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(284, 160);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(81, 12);
			this->label14->TabIndex = 39;
			this->label14->Text = L"modellist.xml";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(543, 327);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->m_execRegister);
			this->Controls->Add(this->m_btnAnimPath);
			this->Controls->Add(this->m_btnTexture);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->m_btnXac);
			this->Controls->Add(this->m_size);
			this->Controls->Add(this->m_monRank);
			this->Controls->Add(this->m_rotate);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->m_fileName);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->m_category);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->m_customLife);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->m_customAttack);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->m_mspd);
			this->Controls->Add(this->m_tClassName);
			this->Controls->Add(this->m_scale);
			this->Controls->Add(this->m_name);
			this->Controls->Add(this->m_texturePath);
			this->Controls->Add(this->m_animPath);
			this->Controls->Add(this->m_xac_face);
			this->Controls->Add(this->m_xac);
			this->Controls->Add(this->m_className);
			this->Name = L"MainForm";
			this->Text = L"R1 몬스터 등록";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void m_btnTexture_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btnAnimPath_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_btnXac_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_className_TextChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_execRegister_Click(System::Object^  sender, System::EventArgs^  e){
				 Register();
			 }
	private: System::Void m_fileName_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
};
}
