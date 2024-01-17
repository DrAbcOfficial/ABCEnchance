#include <wtypes.h>
#include <strtools.h>

#include <win32helper.h>

#ifdef WIN32	
const char* Win32GetAttributesAsString(DWORD dwAttributes)
{
	static char out[256];
	out[0] = 0;
	if (dwAttributes & FILE_ATTRIBUTE_ARCHIVE)
	{
		Q_strncat(out, "A", sizeof(out), COPY_ALL_CHARACTERS);
	}
	if (dwAttributes & FILE_ATTRIBUTE_COMPRESSED)
	{
		Q_strncat(out, "C", sizeof(out), COPY_ALL_CHARACTERS);
	}
	if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		Q_strncat(out, "D", sizeof(out), COPY_ALL_CHARACTERS);
	}
	if (dwAttributes & FILE_ATTRIBUTE_HIDDEN)
	{
		Q_strncat(out, "H", sizeof(out), COPY_ALL_CHARACTERS);
	}
	if (dwAttributes & FILE_ATTRIBUTE_READONLY)
	{
		Q_strncat(out, "R", sizeof(out), COPY_ALL_CHARACTERS);
	}
	if (dwAttributes & FILE_ATTRIBUTE_SYSTEM)
	{
		Q_strncat(out, "S", sizeof(out), COPY_ALL_CHARACTERS);
	}
	if (dwAttributes & FILE_ATTRIBUTE_TEMPORARY)
	{
		Q_strncat(out, "T", sizeof(out), COPY_ALL_CHARACTERS);
	}
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
#endif