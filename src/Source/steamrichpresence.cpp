#include <string>

#include "vgui_controls/Controls.h"
#include "isteamfriends.h"

#include "steamrichpresence.h"

static CSteamRichPresenceManager s_Manager;
void CSteamRichPresenceManager::SetDisplay(const char* pchValue){
	if (pchValue != nullptr && std::strlen(pchValue) > 1 && pchValue[0] == '#') {
		wchar_t* local = vgui::localize()->Find(pchValue + 1);
		if (local) {
			char utf8[k_cchMaxRichPresenceValueLength];
			vgui::localize()->ConvertUnicodeToANSI(local, utf8, k_cchMaxRichPresenceValueLength);
			SteamFriends()->SetRichPresence("steam_display", utf8);
		}
	}
	else
		SteamFriends()->SetRichPresence("steam_display", pchValue);
}

void CSteamRichPresenceManager::SetConnectedServer(const char* ip, int size){
	SteamFriends()->SetRichPresence("steam_player_group", ip);
	SteamFriends()->SetRichPresence("steam_player_group_size", std::to_string(size).c_str());
}

void CSteamRichPresenceManager::ClearConeectedServer(){
	SteamFriends()->SetRichPresence("steam_player_group", nullptr);
	SteamFriends()->SetRichPresence("steam_player_group_size", nullptr);
}

CSteamRichPresenceManager* GetSteamRichPresenceManager(){
	return &s_Manager;
}
