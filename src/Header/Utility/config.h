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
private:
	std::vector<std::string> m_aryFavModels;

	std::string m_szSavePath;
};