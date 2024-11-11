#pragma once

#define LOGS(x)  Console::WriteLine(__FILE__ + " " + __LINE__); Console::WriteLine("["+ (##x) +"]");  

ref struct R1_JOB
{
	String^ className;
	String^ name;
};

namespace util
{
	char * StringToChar(System::String^ input);
	wchar_t* StringToUChar(System::String^ input);
};

void MsgBox(System::String^ a);
