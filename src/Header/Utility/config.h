#pragma once
#include <string>
#include <vector>

class CABCConfig {
public:
	static void Init();
	static CABCConfig* GetConfig();
	static void Close();
public:
	CABCConfig();
	CABCConfig(const char* path);
	void Load(const char* path);
	void Save();
public:
	std::vector<std::string> m_aryFavModels;
private:
	std::string m_szSavePath;
};