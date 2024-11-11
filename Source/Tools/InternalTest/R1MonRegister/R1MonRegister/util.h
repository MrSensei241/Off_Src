#pragma once

#define LOGS(x)  Console::WriteLine(__FILE__ + " " + __LINE__); Console::WriteLine("["+ (##x) +"]");  

ref struct R1_JOB
{
	String^ className;
	String^ name;
};

using namespace System::Windows::Forms;
using namespace System;
using namespace System::Xml;

namespace util
{
	char * StringToChar(System::String^ input);
	wchar_t* StringToUChar(System::String^ input);
	String^ ToString(int iValue);

	XmlNode^ GetNodeByValue(XmlDocument^ doc, String^ valName, String^ attrName, String^ attrValue);
	String^ GetLastValue(XmlNode^ node, String^ valName, String^ attrName);
	XmlNode^ GetLastNode(XmlDocument^ doc, String^ valName);
	XmlNode^ GetChild(XmlNode^ node, String^ valName, String^ attrName, String^ attrValue);
	XmlNode^ GetChild(XmlNode^ node, String^ valName);

	XmlNode^ GetLastChild(XmlNode^ node);
	int		GetUniqueValue(XmlDocument^ doc, String^ valName, String^ attrName, int value);
	String^	GetUniqueValue(XmlDocument^ doc, String^ valName, String^ attrName, String^ value);
	void	SetAttribute(XmlNode^ nod, String^ attrName, String^ attrValue);
	void	SetAttribute(XmlNode^ nod, String^ attrName, int attrValue);
	String^	GetAttribute(XmlNode^ nod, String^ attrName);

	XmlDocument^ LoadXml(String^ fileName);
};

void MsgBox(System::String^ a);
