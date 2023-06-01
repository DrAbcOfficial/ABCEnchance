#include <metahook.h>
#include "encode.h"

void ANSIToUnicode(const std::string& str, std::wstring& out){
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
	out.resize(len);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), (LPWSTR)out.data(), len);
}
void UnicodeToANSI(const std::wstring& str, std::string& out){
	int len = WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
	out.resize(len);
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.length(), (LPSTR)out.data(), len, NULL, NULL);
}
void UnicodeToUTF8(const std::wstring& str, std::string& out){
	int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
	out.resize(len);
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), (LPSTR)out.data(), len, NULL, NULL);
}
void UnicodeToUTF8(wchar_t* str, char* out) {
	UnicodeToUTF8((const std::wstring&)str, (std::string&)out);
}
void UTF8ToUnicode(const std::string& str, std::wstring& out){
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
	out.resize(len);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), (LPWSTR)out.data(), len);
}