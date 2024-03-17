#pragma once
#ifndef STEAM_RICHPRESENCE_H
#define STEAM_RICHPRESENCE_H

class CSteamRichPresenceManager {
public:
	void SetDisplay(const char* pchValue);
	void SetConnectedServer(const char* ip, int size);
	void ClearConeectedServer();
};

CSteamRichPresenceManager* GetSteamRichPresenceManager();
#endif // !STEAM_RICHPRESENCE_H
