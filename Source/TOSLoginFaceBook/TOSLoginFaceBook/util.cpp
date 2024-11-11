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

};

void MsgBox(System::String^ a)
{
	MessageBox::Show(a);
}




