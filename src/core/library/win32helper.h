#pragma once
#ifdef WIN32
#include <string>
#include <vector>

const char* Win32GetAttributesAsString(DWORD dwAttributes);
std::string Win32GetFileDescriptionByExtension(const char* extension);
std::string Win32GetStringValueFromRegistry(HKEY hKey, const std::string& subKey, const std::string& valueName);
std::string Win32GetDefaultValueFromRegistry(HKEY hKey, const std::string& subKey);
HICON Win32GetFileIconByExtension(const std::string& extension);
BITMAP Win32GetBitmapInfo(HICON hIcon);
std::vector<unsigned char> Win32ConvertHICONToRGBA(HICON hIcon, size_t& ww, size_t& hh);
void Win32GetRegKeyValue(char* pszSubKey, char* pszElement, char* pszReturnString, int nReturnLength, char* pszDefaultValue);
#endif // WIN32

