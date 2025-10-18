#pragma once

#ifndef __PLAYER_RESOURCE_H__
#define __PLAYER_RESOURCE_H__

//FUCK MICROSOFT
#undef clamp

#include <array>
#include <string>

#include "steam_api.h"

#include "core/event.h"

#include "teamresource.h"

constexpr auto SC_MAX_PLAYERS = 32;
constexpr auto MAX_PLAYERNAME_LENGTH = 32;
constexpr auto MAX_TEAMNAME_LENGTH = 32;

class PlayerInfo {
public:
	bool IsValid() const;
	// Engine info
	const char* GetName() const;
	bool IsThisPlayer() const;
	void UpdatePing();
	// Should be called before reading engine info.
	// Returns this
	PlayerInfo* Update();
	/**
	 * Returns whether the player has a real name.
	 */
	bool HasRealName();
	void Reset();

	int m_iIndex = -1;
private:
	/**
	 * Sometimes players get stuck in connecting state and won't be visible in status output.
	 * If player wasn't found in status this many times, increase status request period.
	 * Prevents status spamming every 2 seconds.
	 */
	static constexpr int STATUS_PENALTY_THRESHOLD = 4;
	static constexpr float STATUS_PERIOD = 2.f;
	static constexpr float STATUS_BUGGED_PERIOD = 10.f;

public:
	int m_iPing;
	int m_iLoss;
	std::string m_szModel;
	std::string m_szName;
	std::string m_szRealName;
	int m_iTopColor;
	int m_iBottomColor;
	int m_iFrags;
	int m_iDeaths;
	int m_iArmor;
	int m_iHealth;
	bool m_bDead;
	TEAM_INDEX m_iTeamNumber;
	char m_szTeamName[MAX_TEAMNAME_LENGTH];
	char m_szSteamID64[20];
	enum class ADMIN {
		NONE = 0,
		OPRATER,
		SERVER_OWNER
	};
	ADMIN m_iAdmin;
	bool m_bIsConnected;
	bool m_bIsSpectate;
	enum class HIDE_EXTRA {
		NO = 0,
		HIDE_ADMIN = 1,
		HIDE_OWNER = 2
	};
	HIDE_EXTRA m_eHideExtra = HIDE_EXTRA::NO;
	enum class DONOR {
		NONE = 65536,
		ELECTRIC_CROWBAR,
		GOLDED_UZI,
		GOLED_DOLLAR,
		TESTER,
		ARTIST,
		DEVELOEPR
	};
	DONOR m_eDonor;
	CSteamID m_pSteamId;
};

class PlayerResource {
public:
	void Init();
	void ResetAll();
	void UpdateAll();

	PlayerInfo* GetPlayerInfo(int idx);
	PlayerInfo* GetLocalPlayerInfo();

	bool IsInSpectate(int i);
private:
	std::array<PlayerInfo, SC_MAX_PLAYERS + 1> m_aryPlayerInfos;
};

class PlayerInfoUpdateEvent : public ConcreteEvent<PlayerInfoUpdateEvent> {
public:
	PlayerInfo* m_pPlayerInfo;
	PlayerInfoUpdateEvent(PlayerInfo* p);
	virtual const std::string_view GetName() const {
		return "PlayerInfoUpdate";
	}
};

extern PlayerResource gPlayerRes;

#endif