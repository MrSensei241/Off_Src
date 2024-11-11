// NexonSSOClient.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//


#include "stdafx.h"
#include "geClientNexonSSO.h"
#include "NexonSSOClient.h"

 #include "geNexonSSO.h"


DLLFunction void _cdecl SetPassport(const char* passPort){
	geClientNexonSSO::SetPassport(passPort);
}

DLLFunction const char * _cdecl GetPassport(){
	return geClientNexonSSO::GetPassport();
}

DLLFunction void _cdecl SetLogSID(const char* sid){
	geClientNexonSSO::SetLogSID(sid);
}

DLLFunction const char * _cdecl GetLogSID(){
	return geClientNexonSSO::GetLogSID();
}

DLLFunction void _cdecl SetGameCode(const char * gamecode){
	geClientNexonSSO::SetGameCode(gamecode);
}

DLLFunction const char * _cdecl GetGameCode(){
	return geClientNexonSSO::GetGameCode();
}

DLLFunction LRESULT _cdecl OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	return geNexonSSO::OnWndProc(hWnd, message, wParam, lParam);
}

DLLFunction void _cdecl UpdateNexonPassport(){
	geNexonSSO::UpdateNexonPassport();
}

DLLFunction bool _cdecl nexonSSOInit(HWND& hWnd, const char* nexonssoserver, const char* paramPassport, const char* serviceNation){
	return geNexonSSO::Init(hWnd, nexonssoserver, paramPassport, serviceNation);
}