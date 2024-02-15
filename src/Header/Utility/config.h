#pragma once
#include "aigcjson.hpp"

class CABCConfig {
public:
	std::set<std::string> m_aryFavModels;

	AIGC_JSON_HELPER(m_aryFavModels);
	AIGC_JSON_HELPER_RENAME("favmodels");
};

namespace abcconfig {
	void LoadJson();
	void SaveJson();
	CABCConfig* GetConfig();
}