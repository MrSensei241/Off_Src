#include "StdAfx.h"
#include "MainForm.h"
#include "util.h"

using namespace System::Xml;
using namespace System::IO;
using namespace System::Windows::Forms;

namespace R1ItemDrop {


	void SetByString(System::Windows::Forms::ComboBox^ box, String^ value)
	{
		for (int i = 0 ; i < box->Items->Count; i ++)
		{
			if (box->Items[i]->ToString() == value)
			{
				box->SelectedIndex = i;
				return;
			}
		}

		box->SelectedIndex = 0;
	}
	
	

void MainForm::Init()
{
	/*
	SetByString(m_rotate, GetIni("LastRotate", "YES"));
	SetByString(m_size, GetIni("LastSize", "XL"));
	SetByString(m_monRank, GetIni("LastMonRank", "Boss"));
	
	m_mspd->Text = GetIni("LastMspd", "60");
	m_scale->Text = GetIni("LastScale", "1.0");
	m_customLife->Text = GetIni("LastLife", "3500");
	m_customAttack->Text = GetIni("LastAttack", "150");*/
}
/*
String^ MainForm::SaveXAC(String^ xacPath, String^ texturePath, int* xacID)
{
	String^ expectName = Path::GetFileName(Path::GetDirectoryName(texturePath));
	XmlDocument^ xacDoc = util::LoadXml(m_dataPath +  "xml_client\\xac.xml");

	XmlNodeList^ cateList = xacDoc->GetElementsByTagName("Class");
	int cnt = cateList->Count;
	for (int i = 0 ; i < cnt ; i++)
	{
		XmlNode^ nod = cateList->Item(i);
		String^ mesh = nod->Attributes->GetNamedItem("Mesh")->Value;
		String^ path = nod->Attributes->GetNamedItem("Path")->Value;
		if (xacPath == mesh && texturePath == path)
		{
			*xacID = Int32::Parse(nod->Attributes->GetNamedItem("ClassID")->Value);
			return nod->Attributes->GetNamedItem("ClassName")->Value;
		}
	}

	String^ clsName = System::IO::Path::GetFileName(xacPath);
	clsName = clsName->Substring(0, clsName->Length - 4);
	if (clsName->EndsWith("_set"))
		clsName = clsName->Substring(0, clsName->Length - 4);
	
	XmlNode^ cate = util::GetLastNode(xacDoc, "Category");
	String^ lastID = util::GetLastValue(cate, "Class", "ClassID");
	int uniqueID = util::GetUniqueValue(xacDoc, "Class", "ClassID", Int32::Parse(lastID));
	String^ uniqueName = util::GetUniqueValue(xacDoc, "Class", "ClassName", clsName);

	XmlNode^ lastCls = util::GetLastNode(xacDoc, "Class");
	XmlNode^ newNode = lastCls->CloneNode(true);
	util::SetAttribute(newNode, "ClassID", uniqueID);
	util::SetAttribute(newNode, "ClassName", uniqueName);
	util::SetAttribute(newNode, "Mesh", xacPath);
	util::SetAttribute(newNode, "Path", texturePath);
	
	cate->InsertAfter(newNode, lastCls);
	xacDoc->Save(m_dataPath +  "xml_client\\xac.xml");

	*xacID = uniqueID;
	return uniqueName;
}
*/

void MainForm::SetBinPath(String^ a)
{
	m_binPath = a;
}

void MainForm::SetDataPath(String^ a)
{
	m_dataPath = a;
}

String^ GetProfileStr(String^ cate, String^ name, String^ initPath, String^ defValue)
{
	TCHAR szBuffer[256];
	DWORD d = GetPrivateProfileString(LPCWSTR(util::StringToUChar(cate)), LPCWSTR(util::StringToUChar(name)), LPCTSTR(""), szBuffer, 256, LPCWSTR(util::StringToUChar(initPath)));
	String^ ret = gcnew String(szBuffer);
	if (d == 0)
	{
		return defValue;
	}
	return ret;
}

void MainForm::SetIni(String^ iniName, String^ value)
{
	String^ iniFilePath = m_binPath + "toolfiles\\monregister.ini";
	WritePrivateProfileString(LPCWSTR(util::StringToUChar("bossrun")), LPCWSTR(util::StringToUChar(iniName)), LPCWSTR(util::StringToUChar(value)), LPCWSTR(util::StringToUChar(iniFilePath)));
}

String^ MainForm::GetIni(String^ iniName, String^ defValue)
{
	String^ iniFilePath = m_binPath + "toolfiles\\monregister.ini";
	return GetProfileStr("bossrun", iniName, iniFilePath, defValue);
}

};