#include <wtypes.h>
#include <win32helper.h>

#ifdef WIN32	
const char* Win32GetAttributesAsString(DWORD dwAttributes)
{
	static char out[256];
	out[0] = 0;
	if (dwAttributes & FILE_ATTRIBUTE_ARCHIVE)
		std::strncat(out, "A", 1);
	if (dwAttributes & FILE_ATTRIBUTE_COMPRESSED)
		std::strncat(out, "C", 1);
	if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
		std::strncat(out, "D", 1);
	if (dwAttributes & FILE_ATTRIBUTE_HIDDEN)
		std::strncat(out, "H", 1);
	if (dwAttributes & FILE_ATTRIBUTE_READONLY)
		std::strncat(out, "R", 1);
	if (dwAttributes & FILE_ATTRIBUTE_SYSTEM)
		std::strncat(out, "S", 1);
	if (dwAttributes & FILE_ATTRIBUTE_TEMPORARY)
		std::strncat(out, "T", 1);
	return out;
}

std::string Win32GetFileDescriptionByExtension(const char* extension) {
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_CLASSES_ROOT, NULL, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		std::string subKey = Win32GetDefaultValueFromRegistry(hKey, extension);
		std::string description = Win32GetDefaultValueFromRegistry(hKey, subKey);
		RegCloseKey(hKey);
		return description;
	}
	return "";
}
std::string Win32GetStringValueFromRegistry(HKEY hKey, const std::string& subKey, const std::string& valueName) {
	std::string result;
	DWORD size = 0;
	if (RegGetValueA(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, NULL, NULL, &size) == ERROR_SUCCESS) {
		char* buffer = new char[size];
		if (RegGetValueA(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, NULL, buffer, &size) == ERROR_SUCCESS)
			result = std::string(buffer, size - 1);
		delete[] buffer;
	}
	return result;
}
std::string Win32GetDefaultValueFromRegistry(HKEY hKey, const std::string& subKey) {
	return Win32GetStringValueFromRegistry(hKey, subKey, "");
}
HICON Win32GetFileIconByExtension(const std::string& extension) {
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_CLASSES_ROOT, NULL, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		std::string subKey = Win32GetDefaultValueFromRegistry(hKey, extension);
		std::string iconPath = Win32GetDefaultValueFromRegistry(hKey, subKey + "\\DefaultIcon");
		RegCloseKey(hKey);
		if (iconPath.size() == 0)
			return NULL;
		size_t pos = iconPath.find_last_of(',');
		std::string path = iconPath.substr(0, pos);
		int index = std::stoi(iconPath.substr(pos + 1));
		char expandedPath[MAX_PATH];
		ExpandEnvironmentStringsA(path.c_str(), expandedPath, MAX_PATH);
		HICON smallIcon;
		ExtractIconExW(std::wstring(expandedPath, expandedPath + strlen(expandedPath)).c_str(), index, NULL, &smallIcon, 1);
		return smallIcon;
	}
	return NULL;
}
BITMAP Win32GetBitmapInfo(HICON hIcon) {
	BITMAP bmp;
	ICONINFO iconInfo;
	GetIconInfo(hIcon, &iconInfo);
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmp);
	return bmp;
}
std::vector<unsigned char> Win32ConvertHICONToRGBA(HICON hIcon, size_t& ww, size_t& hh) {
	BITMAP bmp = Win32GetBitmapInfo(hIcon);
	int width = bmp.bmWidth;
	int height = bmp.bmHeight;
	ww = width;
	hh = height;
	HDC hdc = CreateCompatibleDC(NULL);
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	unsigned char* bits = new unsigned char[width * height * 4];
	HBITMAP hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
	SelectObject(hdc, hBmp);
	DrawIconEx(hdc, 0, 0, hIcon, width, height, 0, NULL, DI_NORMAL);
	std::vector<unsigned char> rgba;
	for (int i = 0; i < width * height * 4; i += 4) {
		rgba.push_back(bits[i + 2]);
		rgba.push_back(bits[i + 1]);
		rgba.push_back(bits[i]);
		rgba.push_back(bits[i + 3]);
	}
	DeleteObject(hBmp);
	DeleteDC(hdc);
	return rgba;
}
void Sys_GetRegKeyValueUnderRoot(HKEY rootKey, const char* pszSubKey, const char* pszElement, char* pszReturnString, int nReturnLength, const char* pszDefaultValue){
	LONG lResult;
	HKEY hKey;
	char szBuff[128];
	DWORD dwDisposition;
	DWORD dwType;
	DWORD dwSize;
	snprintf(pszReturnString, nReturnLength, pszDefaultValue);
	lResult = RegCreateKeyEx(
		rootKey,
		pszSubKey,
		0,
		(LPSTR)"String",
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisposition);
	if (lResult != ERROR_SUCCESS)
		return;
	if (dwDisposition == REG_CREATED_NEW_KEY)
		lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (CONST BYTE*)pszDefaultValue, strlen(pszDefaultValue) + 1);
	else{
		dwSize = nReturnLength;
		lResult = RegQueryValueEx(hKey, pszElement, 0, &dwType, (unsigned char*)szBuff, &dwSize);
		if (lResult == ERROR_SUCCESS){
			if (dwType == REG_SZ){
				strncpy(pszReturnString, szBuff, nReturnLength);
				pszReturnString[nReturnLength - 1] = '\0';
			}
		}
		else
			lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (CONST BYTE*)pszDefaultValue, strlen(pszDefaultValue) + 1);
	};
	RegCloseKey(hKey);
}
void Win32GetRegKeyValue(char* pszSubKey, char* pszElement, char* pszReturnString, int nReturnLength, char* pszDefaultValue){
	Sys_GetRegKeyValueUnderRoot(HKEY_CURRENT_USER, pszSubKey, pszElement, pszReturnString, nReturnLength, pszDefaultValue);
}
#endif