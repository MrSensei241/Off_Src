#include "StdAfx.h"
#include "MainForm.h"
#include "util.h"

using namespace System::Xml;
using namespace System::IO;
using namespace System::Windows::Forms;

namespace R1MonRegister {


	void SetByString(System::Windows::Forms::ComboBox^ box, String^ value)
	{
		for (int i = 0; i < box->Items->Count; i++)
		{
			if (box->Items[i]->ToString() == value)
			{
				box->SelectedIndex = i;
				return;
			}
		}

		box->SelectedIndex = 0;
	}

	void MainForm::UpdateCategory()
	{
		m_category->Items->Clear();

		XmlNodeList^ cateList = m_monsterDoc->GetElementsByTagName("Category");
		int cnt = cateList->Count;
		for (int i = 0; i < cnt; i++)
		{
			XmlNode^ item = cateList->Item(i);
			String^ clsName = item->Attributes->GetNamedItem("Name")->Value;
			m_category->Items->Add(clsName);
		}

		SetByString(m_category, "CurCategory");

		{
			m_monClsList.Clear();
			XmlNodeList^ cateList = m_monsterDoc->GetElementsByTagName("Class");
			int cnt = cateList->Count;
			for (int i = 0; i < cnt; i++)
			{
				XmlNode^ item = cateList->Item(i);
				m_monClsList.Add(item);
			}
		}
	}

	System::Void MainForm::m_className_TextChanged(System::Object^  sender, System::EventArgs^  e)
	{
		bool isExist = false;
		for each (XmlNode^ nod in m_monClsList)
		{
			if (nod->Attributes->GetNamedItem("ClassName")->Value == m_className->Text)
				isExist = true;
		}

		if (isExist)
		{
			m_tClassName->ForeColor = System::Drawing::Color::Red;
		}
		else {
			m_tClassName->ForeColor = System::Drawing::Color::Black;
		}
	}
	
System::Void MainForm::m_fileName_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	SetIni("LastFile", m_fileName->Text);
	m_monsterDoc = util::LoadXml(m_dataPath + "xml\\" + m_fileName->Text);
	UpdateCategory();
}

void MainForm::Init()
{
	this->m_fileName->SelectedIndexChanged -= gcnew System::EventHandler(this, &MainForm::m_fileName_SelectedIndexChanged);
	m_fileName->Items->Add("monster.xml");
	m_fileName->Items->Add("monster_npc.xml");
	SetByString(m_fileName, GetIni("LastFile", "monster.xml"));
	this->m_fileName->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::m_fileName_SelectedIndexChanged);
	m_monsterDoc = util::LoadXml(m_dataPath + "xml\\" + m_fileName->Text);

	m_rotate->Items->Add("YES");
	m_rotate->Items->Add("NO");
	
	m_monRank->Items->Add("Normal");
	m_monRank->Items->Add("Boss");

	{
		XmlDocument^ doc = util::LoadXml(m_dataPath + "xml\\shape.xml");
		XmlNodeList^ cateList = doc->GetElementsByTagName("Class");
		int cnt = cateList->Count;
		for (int i = 0 ; i < cnt ; i++)
		{
			XmlNode^ item = cateList->Item(i);
			String^ clsName = item->Attributes->GetNamedItem("ClassName")->Value;
			m_size->Items->Add(clsName);
		}
	}
		
	UpdateCategory();
	
	SetByString(m_rotate, GetIni("LastRotate", "YES"));
	SetByString(m_size, GetIni("LastSize", "XL"));
	SetByString(m_monRank, GetIni("LastMonRank", "Boss"));
	
	m_mspd->Text = GetIni("LastMspd", "60");
	m_scale->Text = GetIni("LastScale", "1.0");
	m_customLife->Text = GetIni("LastLife", "3500");
	m_customAttack->Text = GetIni("LastAttack", "150");
}

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


	String^ clsName = m_className->Text;
	//String^ clsName = System::IO::Path::GetFileName(xacPath);
	//clsName = clsName->Substring(0, clsName->Length - 4);
	//if (clsName->EndsWith("_set"))
	//	clsName = clsName->Substring(0, clsName->Length - 4);
	

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

String^ MainForm::SaveModelList(int xacID, String^ xacName, int faceXacID)
{
	String^ fullPath;
	if (m_fileName->Text == "monster_npc.xml")
		fullPath = m_dataPath +  "xml\\modellist_npc.xml";
	else
		fullPath = m_dataPath +  "xml\\modellist.xml";

	XmlDocument^ modelDoc = util::LoadXml(fullPath);
	XmlNode^ nod = util::GetNodeByValue(modelDoc, "Class", "ClassID", util::ToString(xacID));
	if (nod)
	{
		return nod->Attributes->GetNamedItem("ClassName")->Value;
	}

	XmlNode^ cate = util::GetLastNode(modelDoc, "Category");

	String^ targetName = "Boss_Golem";
	bool useCopyTarget = false;
	if (textBox1->Text != ""){
		targetName = textBox1->Text;
		useCopyTarget = true;
	}


	XmlNode^ sampleNode = util::GetChild(cate, "Class", "ClassName", targetName);
	if (!sampleNode)
		sampleNode = util::GetChild(cate, "Class");

	XmlNode^ newNode = sampleNode->CloneNode(true);
	String^ uniqueName = util::GetUniqueValue(modelDoc, "Class", "ClassName", xacName);
	int uniqueID = util::GetUniqueValue(modelDoc, "Class", "ClassID", Int32::Parse(util::GetAttribute(sampleNode, "ClassID")));
	util::SetAttribute(newNode, "ClassName", uniqueName);
	util::SetAttribute(newNode, "RootName", uniqueName);
	util::SetAttribute(newNode, "FileName", uniqueName);
	util::SetAttribute(newNode, "ClassID", uniqueID);
	util::SetAttribute(newNode, "PT_Body", xacID);
	if (faceXacID)
		util::SetAttribute(newNode, "PT_Fcae", faceXacID);

	cate->InsertAfter(newNode, sampleNode);
	modelDoc->Save(fullPath);
	return uniqueName;
}

System::Void MainForm::Register()
{
	int xacID = 0;
	int faceXacID = 0;
	String^ xacName = SaveXAC(m_xac->Text, m_texturePath->Text, &xacID);
	String^ xacFaceName = "";
	if (m_xac_face->Text != "")
		SaveXAC(m_xac_face->Text, m_texturePath->Text, &faceXacID);

	String^ modelName = SaveModelList(xacID, xacName, faceXacID);
	
	SetIni("LastRotate", m_rotate->Text);
	SetIni("LastSize", m_size->Text);
	SetIni("LastMonRank", m_monRank->Text);
	SetIni("LastMspd", m_mspd->Text);
	SetIni("LastScale", m_scale->Text);
	SetIni("LastLife", m_customLife->Text);
	SetIni("LastAttack", m_customAttack->Text);
	SetIni("CurCategory", m_category->Text);

	XmlNode^ cateNode = util::GetNodeByValue(m_monsterDoc, "Category", "Name", m_category->Text);
	if (cateNode == nullptr)
		return;
	
	String^ targetName = "Boss_Golem";	
	bool useCopyTarget = false;
	if (textBox2->Text != ""){
		targetName = textBox2->Text;
		useCopyTarget = true;
	}

	XmlNode^ sampleNode = util::GetNodeByValue(m_monsterDoc, "Class", "ClassName", targetName);
	if (!sampleNode)
	{
		sampleNode = util::GetNodeByValue(m_monsterDoc, "Class", "ClassName", nullptr);
		if (!sampleNode)
		{
			MsgBox("에러 " + __FILE__ + " " + __LINE__);
			return;
		}
	}
	String^ lastID = util::GetLastValue(cateNode, "Class", "ClassID");
	int uniqueID = util::GetUniqueValue(m_monsterDoc, "Class", "ClassID", Int32::Parse(lastID));
	
	XmlNode^ clNode = sampleNode->CloneNode(true);	
	util::SetAttribute(clNode, "ClassID", uniqueID);
	util::SetAttribute(clNode, "ClassName", m_className->Text);
	util::SetAttribute(clNode, "EP_Name", m_name->Text);
	util::SetAttribute(clNode, "SET", modelName);
	util::SetAttribute(clNode, "SET_ANI", m_animPath->Text);

	if (useCopyTarget == false)
	{
		util::SetAttribute(clNode, "Scale", m_scale->Text);
		int mspd = Int32::Parse(m_mspd->Text);
		util::SetAttribute(clNode, "EP_RunMSPD", mspd);
		util::SetAttribute(clNode, "EP_WlkMSPD", mspd / 2);
		util::SetAttribute(clNode, "CT_Rotate", m_rotate->Text);
		util::SetAttribute(clNode, "CT_Size", m_size->Text);
		util::SetAttribute(clNode, "CT_MonRank", m_monRank->Text);
		util::SetAttribute(clNode, "CT_CustomLife", m_customLife->Text);
		util::SetAttribute(clNode, "CT_CustomAttack", m_customAttack->Text);
		util::SetAttribute(clNode, "CT_SkillType", m_className->Text);
	}
	
	

	XmlNode^ lastNode = util::GetLastChild(cateNode);
	cateNode->InsertAfter(clNode, lastNode);
	m_monsterDoc->Save(m_dataPath + "xml\\" + m_fileName->Text);

	UpdateCategory();
	MsgBox("완료");
}

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

System::Void MainForm::m_btnTexture_Click(System::Object^  sender, System::EventArgs^  e)
{
	String^ ret = m_dataPath;
	ret += "char_texture\\monster\\";

	ret = GetIni("LastTexturePath", ret);
	textureDlg->InitialDirectory = System::IO::Path::GetDirectoryName(ret);
	textureDlg->FileName = ret;
	if (textureDlg->ShowDialog() != System::Windows::Forms::DialogResult::OK)
		return;

	SetIni("LastTexturePath", textureDlg->FileName);

	String^ fName = System::IO::Path::GetFileName(textureDlg->FileName);
	String^ dirName = System::IO::Path::GetDirectoryName(textureDlg->FileName);
	String^ finalName  = dirName->Substring(m_dataPath->Length, dirName->Length - m_dataPath->Length);
	finalName += "\\";
	m_texturePath->Text = finalName;

	if (m_animPath->Text == "")
	{
		String^ modelName = System::IO::Path::GetFileName(dirName);
		String^ expectTexturePath = "animation\\monster\\" + modelName;
		String^ fullPath = m_dataPath + expectTexturePath;
		if (System::IO::Directory::Exists(fullPath))
		{
			m_animPath->Text = modelName;
		}
	}
}

System::Void MainForm::m_btnAnimPath_Click(System::Object^  sender, System::EventArgs^  e)
{
	String^ ret = m_dataPath;
	ret += "animation\\monster\\";
	ret = GetIni("LastAnimPath", ret);

	textureDlg->InitialDirectory = System::IO::Path::GetDirectoryName(ret);
	textureDlg->FileName = ret;
	if (textureDlg->ShowDialog() != System::Windows::Forms::DialogResult::OK)
		return;

	SetIni("LastAnimPath", textureDlg->FileName);

	String^ fName = System::IO::Path::GetFileName(textureDlg->FileName);
	String^ dirName = System::IO::Path::GetDirectoryName(textureDlg->FileName);
	String^ topDir = System::IO::Path::GetFileName(dirName);
	dirName = System::IO::Path::GetDirectoryName(dirName);
	String^ secondDir = System::IO::Path::GetFileName(dirName);

	if (0 == String::Compare(secondDir, "MONSTER", true))
	{
		m_animPath->Text = topDir;		
	} else {
		String^ setAnimText = secondDir + "\\" + topDir;
		m_animPath->Text = setAnimText;
	}
}

System::Void MainForm::m_btnXac_Click(System::Object^  sender, System::EventArgs^  e)
{
	String^ ret = m_dataPath;
	ret += "char_hi\\monster\\";
	ret = GetIni("LastXACPath", ret);

	textureDlg->InitialDirectory = System::IO::Path::GetDirectoryName(ret);
	textureDlg->FileName = ret;
	if (textureDlg->ShowDialog() != System::Windows::Forms::DialogResult::OK)
		return;

	SetIni("LastXACPath", textureDlg->FileName);

	String^ selName = textureDlg->FileName;
	String^ finalName = selName->Substring(m_dataPath->Length, selName->Length - m_dataPath->Length);
	m_xac->Text = finalName;
	String^ charHi = "char_hi\\";
	String^ categoryFolder = finalName->Substring(charHi->Length, finalName->Length - charHi->Length);
	categoryFolder = categoryFolder->Substring(0, categoryFolder->IndexOf("\\"));
	String^ categoryFolder_Underbar = categoryFolder +"_";

	String^ texFolder = "char_texture\\" + categoryFolder + "\\";
	String^ expectModelName = System::IO::Path::GetFileName(selName);
	expectModelName = expectModelName->Substring(0, expectModelName->Length - 8);
	String^ expectModelName2 = "";
	if (expectModelName->StartsWith(categoryFolder_Underbar))
		expectModelName2= expectModelName->Substring(categoryFolder_Underbar->Length, expectModelName->Length - categoryFolder_Underbar->Length);

	if (m_texturePath->Text == "")
	{
		String^ expectTexturePath = texFolder + expectModelName;
		String^ fullPath = m_dataPath + expectTexturePath;
		if (System::IO::Directory::Exists(fullPath))
		{
			m_texturePath->Text = expectTexturePath + "\\";
		} else if (expectModelName->StartsWith(categoryFolder_Underbar)){
			String^ expectTexturePath2 = texFolder + expectModelName2;
			String^ fullPath2 = m_dataPath + expectTexturePath2;
			if (System::IO::Directory::Exists(fullPath2))
			{
				m_texturePath->Text = expectTexturePath2 + "\\";
			}
		}
	}
	
	if (m_animPath->Text == "")
	{
		{
			String^ expectTexturePath = "animation\\" + categoryFolder + "\\" + expectModelName;
			String^ fullPath = m_dataPath + expectTexturePath;
			if (System::IO::Directory::Exists(fullPath))
			{
				m_animPath->Text = categoryFolder + "\\" + expectModelName;
			}
		}
		
		if (m_animPath->Text == "")
		{
			String^ expectTexturePath =  "animation\\" + categoryFolder + "\\" + expectModelName2;
			String^ fullPath = m_dataPath + expectTexturePath;
			if (System::IO::Directory::Exists(fullPath))
			{
				m_animPath->Text = categoryFolder + "\\" + expectModelName2;
			}
		}
	}
}

};