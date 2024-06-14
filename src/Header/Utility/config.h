#pragma once
#include "aigcjson.hpp"

class CABCConfig {
public:
	std::set<std::string> m_aryFavModels;
	std::map<std::string, std::string> m_dicConcurrentCmd;
	std::map<std::string, std::string> m_dicEventsCmd;

	AIGC_JSON_HELPER(m_aryFavModels, m_dicConcurrentCmd, m_dicEventsCmd);
	AIGC_JSON_HELPER_RENAME("favmodels", "concurrent_cmd", "events_cmd");
};

namespace abcconfig {
	void LoadJson();
	void SaveJson();
	CABCConfig* GetConfig();
}