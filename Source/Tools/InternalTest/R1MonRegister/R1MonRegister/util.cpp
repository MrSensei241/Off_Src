#include "stdafx.h"
#include "util.h"


namespace util
{
	char * StringToChar(System::String^ input)
	{
		return (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(input);
	}

	wchar_t* StringToUChar(System::String^ input)
	{
		return (wchar_t*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalUni(input);
	};

	String^ ToString(int iValue)
	{
		return String::Format("{0}", iValue);
	}

	XmlNode^ GetNodeByValue(XmlNodeList^ cateList, String^ valName, String^ attrName ,String^ attrValue)
	{
		int cnt = cateList->Count;
		for (int i = 0 ; i < cnt ; i++)
		{
			XmlNode^ item = cateList->Item(i);
			XmlNode^ attrNode = item->Attributes->GetNamedItem(attrName);
			if (!attrNode)
				continue;

			String^ clsName = attrNode->Value;
			if (nullptr == attrValue || clsName == attrValue)
				return item;
		}

		return nullptr;
	}

	XmlNode^ GetNodeByValue(XmlDocument^ doc, String^ valName, String^ attrName ,String^ attrValue)
	{
		XmlNodeList^ cateList = doc->GetElementsByTagName(valName);
		return GetNodeByValue(cateList, valName, attrName, attrValue);
	}

	XmlNode^ GetChild(XmlNode^ node, String^ valName, String^ attrName, String^ attrValue)
	{
		for (int i = 0 ; i < node->ChildNodes->Count ; i++)
		{
			XmlNode^ chl = node->ChildNodes[i];
			if (chl->Name == valName)
			{
				if (util::GetAttribute(chl, attrName) == attrValue){
					return chl;
				}
			}
		}

		return nullptr;
	}

	XmlNode^ GetChild(XmlNode^ node, String^ valName)
	{
		for (int i = node->ChildNodes->Count - 1 ; i >= 0 ; i--)
		{
			XmlNode^ chl = node->ChildNodes[i];
			if (chl->Name == valName)
			{
				return chl;
			}
		}

		return nullptr;
	}

	XmlNode^ GetLastNode(XmlDocument^ doc, String^ valName)
	{
		XmlNodeList^ cateList = doc->GetElementsByTagName(valName);
		int cnt = cateList->Count;
		if (cnt == 0)
			return nullptr;

		return cateList->Item(cateList->Count - 1);
	}

	String^ GetLastValue(XmlNode^ node, String^ valName, String^ attrName)
	{
		XmlNodeList^ cateList = node->ChildNodes;
		int cnt = cateList->Count;
		if (cnt == 0)
			return "";

		XmlNode^ item = cateList->Item(cateList->Count - 1);
		XmlNode^ attrNod = item->Attributes->GetNamedItem(attrName);
		if (!attrNod)
			return "";

		return attrNod->Value;
	}

	XmlNode^ GetLastChild(XmlNode^ node)
	{
		XmlNodeList^ cateList = node->ChildNodes;
		int cnt = cateList->Count;
		if (cnt == 0)
			return nullptr;

		return cateList->Item(cateList->Count - 1);		
	}

	void	SetAttribute(XmlNode^ nod, String^ attrName, int attrValue)
	{
		SetAttribute(nod, attrName, String::Format("{0}", attrValue));
	}

	void	SetAttribute(XmlNode^ nod, String^ attrName, String^ attrValue)
	{
		XmlNode^ attr = nod->Attributes->GetNamedItem(attrName);
		if (attr)
		{
			attr->Value = attrValue;
		}		
	}

	String^	GetAttribute(XmlNode^ nod, String^ attrName)
	{
		XmlNode^ attrNode = nod->Attributes->GetNamedItem(attrName);
		if (!attrNode)
			return "";

		return attrNode->Value;
	}

	int		GetUniqueValue(XmlDocument^ doc, String^ valName, String^ attrName, int value)
	{
		XmlNodeList^ cateList = doc->GetElementsByTagName(valName);
		while(true)
		{
			String^ strValue = String::Format("{0}", value);
			if (nullptr == GetNodeByValue(cateList, valName, attrName, strValue))
				return value;

			value++;
		}

		return value;
	}

	String^	GetUniqueValue(XmlDocument^ doc, String^ valName, String^ attrName, String^ value)
	{
		int index = 0;
		XmlNodeList^ cateList = doc->GetElementsByTagName(valName);
		while(true)
		{
			String^ tmp = index == 0 ? value : String::Format("{0}_{1}", value, index);
			if (nullptr == GetNodeByValue(cateList, valName, attrName, tmp))
				return tmp;

			index++;
		}

		return value;	
	}

	XmlDocument^ LoadXml(String^ fileName)
	{
		XmlDocument^ ret = gcnew XmlDocument;
		XmlTextReader^ reader = gcnew XmlTextReader(fileName);
		ret->Load(reader);
		reader->Close();
		delete reader;
		return ret;
	}
};

void MsgBox(System::String^ a)
{
	MessageBox::Show(a);
}




