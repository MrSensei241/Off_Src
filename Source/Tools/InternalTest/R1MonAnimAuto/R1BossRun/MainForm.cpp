#include "StdAfx.h"
#include "MainForm.h"
#include "util.h"

using namespace System::Xml;


namespace R1BossRun {

	void MainForm::LoadWebAddr()
	{
		String^ xmlName = m_binPath + "toolfiles\\toolclient.xml";
		XmlDocument^ doc = gcnew XmlDocument;
		XmlTextReader^ reader = gcnew XmlTextReader(xmlName);
		doc->Load(reader);
		reader->Close();
		delete reader;

		XmlNodeList^ clsList = doc->GetElementsByTagName("BossWebAddr");
		if (clsList->Count == 0)
			return;

		XmlNode^ node = clsList->Item(0);
		m_webAddr = node->Attributes->GetNamedItem("Value")->Value;
	}

void MainForm::LoadJobList()
{
	String^ jobXml = m_dataPath + "xml\\job.xml";
	XmlDocument^ doc = gcnew XmlDocument;
	XmlTextReader^ reader = gcnew XmlTextReader(jobXml);
	doc->Load(reader);
	reader->Close();
	delete reader;

	//XmlNodeList^ idspcs = doc->GetElementsByTagName("idspace");
	XmlNodeList^ clsList = doc->GetElementsByTagName("Class");
	int cnt = clsList->Count;
	for (int i = 0 ; i < cnt ; i++)
	{
		XmlNode^ item = clsList->Item(i);
		String^ clsName = item->Attributes->GetNamedItem("ClassName")->Value;
		String^ name = item->Attributes->GetNamedItem("Name")->Value;
		R1_JOB^ job = gcnew R1_JOB;
		job->className = clsName;
		job->name = name;
		m_jobList.Add(job);
	}
	
	String^ selJob = GetIni("SELECTEDJOB");
	int selIndex = 0;
	for (int i = 0 ; i < m_jobList.Count ; i++)
	{
		R1_JOB^ job = (R1_JOB^) m_jobList[i];
		jobdropList->Items->Add(job->name);
		if (selJob == job->className)
		{
			selIndex = i;
		}
	}

	this->jobdropList->SelectedIndexChanged -= gcnew System::EventHandler(this, &MainForm::jobdropList_SelectedIndexChanged);
	jobdropList->SelectedIndex = selIndex;
	this->jobdropList->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::jobdropList_SelectedIndexChanged);
}

System::Void MainForm::jobdropList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (jobdropList->SelectedIndex == -1 || jobdropList->SelectedIndex >= m_jobList.Count)
		return;

	R1_JOB^ job = (R1_JOB^) m_jobList[jobdropList->SelectedIndex];
	SetIni("SELECTEDJOB", job->className);
}

void MainForm::Init()
{
	input_name->Text = GetIni("PCNAME");
	LoadJobList();
	LoadWebAddr();
	rankWeb->Navigate(m_webAddr);
}

void MainForm::SetBinPath(String^ a)
{
	m_binPath = a;
}

void MainForm::SetDataPath(String^ a)
{
	m_dataPath = a;
}

String^ GetProfileStr(String^ cate, String^ name, String^ initPath)
{
	TCHAR szBuffer[256];
	DWORD d = GetPrivateProfileString(LPCWSTR(util::StringToUChar(cate)), LPCWSTR(util::StringToUChar(name)), LPCTSTR(""), szBuffer, 256, LPCWSTR(util::StringToUChar(initPath)));
	String^ ret = gcnew String(szBuffer);
	if (d == 0)
	{
		return "";
	}

	return ret;
}

void MainForm::SetIni(String^ iniName, String^ value)
{
	String^ iniFilePath = m_binPath + "toolfiles\\bossrun.ini";
	WritePrivateProfileString(LPCWSTR(util::StringToUChar("bossrun")), LPCWSTR(util::StringToUChar(iniName)), LPCWSTR(util::StringToUChar(value)), LPCWSTR(util::StringToUChar(iniFilePath)));
}

String^ MainForm::GetIni(String^ iniName)
{
	String^ iniFilePath = m_binPath + "toolfiles\\bossrun.ini";
	return GetProfileStr("bossrun", iniName, iniFilePath);
}

System::Void MainForm::input_name_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	SetIni("PCNAME", input_name->Text);
}

System::Void MainForm::test_button_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (input_name->Text == "")
	{
		MsgBox("이름을 입력해주세요");
		return;
	}
	
	R1_JOB^ job = (R1_JOB^) m_jobList[jobdropList->SelectedIndex];
	String^ jobName = job->className;

	String^ bossName = rankWeb->Document->Title;
	String^ runFileName = m_binPath;
	runFileName += "Client_Release.exe";
	String^ argments = "-SERVICE -SINGLE -BOSS " + jobName + "#" + bossName + "#" + input_name->Text;
	
	System::Diagnostics::ProcessStartInfo^ pInfo = gcnew System::Diagnostics::ProcessStartInfo(runFileName);
	pInfo->Arguments = argments;
	System::Diagnostics::Process^ p = System::Diagnostics::Process::Start(pInfo);	
	
}

};