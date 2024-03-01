/***
*
*	Copyright (c) 2003', Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H
#include <array>
#include <string>
#include "steam_api.h"

typedef struct player_info_s player_info_t;

//-----------------------------------------------------
class CPlayerInfo{
public:
	int GetIndex();
	bool IsConnected();
	bool IsValid();
	// Engine info
	const char *GetName();
	int GetPing();
	int GetPacketLoss();
	bool IsThisPlayer();
	const char *GetModel();
	int GetTopColor();
	int GetBottomColor();
	uint64 GetSteamID64();
	// Extra info (from HUD messages)
	int GetFrags();
	int GetDonor();
	int GetAdmin();
	int GetDeaths();
	int GetHealth();
	int GetArmor();
	int GetTeamNumber();
	const char *GetTeamName();
	bool IsSpectator();
	/**
	 * Returns display name. It should be used in text displayed on HUD.
	 * @param	bNoColorCodes	If true and ColorCodeAction != Ignore, color codes will be removed.
	 * @return	Display name stored in internal buffer. It can handle up to 8 calls before overwriting.
	 */
	const char *GetRealName();
	/**
	 * Returns SteamID string. Requires SVC hook.
	 */
	CSteamID* GetSteamID();
	const char* GetSteamIDString();
	const char* GetSteamIDString64();
	void UpdatePing();
	// Should be called before reading engine info.
	// Returns this
	CPlayerInfo *Update();
	void UpdateAll();
	/**
	 * Returns whether the player has a real name.
	 */
	bool HasRealName();
	void Reset();
	void ResetAll();

	//static shit
	static std::array<CPlayerInfo, SC_MAX_PLAYERS + 1>& GetPlayerInfos();
	static void InitPlayerInfos();
	static CPlayerInfo* GetPlayerInfo(int idx);
	static CPlayerInfo* GetThisPlayerInfo();

	struct extra_player_info {
		int frags;
		int deaths;
		int armor;
		int health;
		bool dead;
		short teamnumber = -1;
		char teamname[MAX_TEAMNAME_LENGTH];
		short donor;
		short admin;
	};
private:
	/**
	 * Sometimes players get stuck in connecting state and won't be visible in status output.
	 * If player wasn't found in status this many times, increase status request period.
	 * Prevents status spamming every 2 seconds.
	 */
	static constexpr int STATUS_PENALTY_THRESHOLD = 4;
	static constexpr float STATUS_PERIOD = 2.f;
	static constexpr float STATUS_BUGGED_PERIOD = 10.f;

	int m_iIndex = -1;
	int m_iPing;
	int m_iLoss;
	std::string m_szModel;
	std::string m_szName;
	std::string m_szRealName;
	int m_iTopColor;
	int m_iBottomColor;
	extra_player_info m_ExtraInfo;
	bool m_bIsConnected;
	CSteamID m_pSteamId;
};

//-----------------------------------------------------
class CTeamInfo{
public:
	/**
	 * Returns team number.
	 */
	int GetNumber();

	/**
	 * Returns name of the team. This is the one returned by CPlayerInfo::GetTeamName().
	 */
	const char *GetName();
	/**
	 * Returns whether TeamScore message was used to override the scores.
	 * If true, use GetFrags and GetDeaths instead of calculated values.
	 */
	bool IsScoreOverriden();

	/**
	 * Returns frag count.
	 * Only valid if IsScoreOverriden() == true.
	 */
	int GetFrags();

	/**
	 * Returns death count.
	 * Only valid if IsScoreOverriden() == true.
	 */
	int GetDeaths();
	void ResetAll();
	//static shit
	static std::array<CTeamInfo, SC_MAX_TEAMS + 1>& GetTeamInfos();
	static const char* GetNameByIndex(uint index);
	static void InitTeamInfos();
	static CTeamInfo* GetTeamInfo(int number);

	enum class TEAM_INDEX{
		CLASS_FORCE_NONE = -1,
		CLASS_NONE,
		CLASS_MACHINE,
		CLASS_PLAYER,
		CLASS_HUMAN_PASSIVE,
		CLASS_HUMAN_MILITARY,
		CLASS_ALIEN_MILITARY,
		CLASS_ALIEN_PASSIVE,
		CLASS_ALIEN_MONSTER,
		CLASS_ALIEN_PREY,
		CLASS_ALIEN_PREDATOR,
		CLASS_INSECT,
		CLASS_PLAYER_ALLY,
		CLASS_PLAYER_BIOWEAPON,
		CLASS_ALIEN_BIOWEAPON,
		CLASS_XRACE_PITDRONE,
		CLASS_XRACE_SHOCK,
		CLASS_TEAM1,
		CLASS_TEAM2,
		CLASS_TEAM3,
		CLASS_TEAM4,
		CLASS_BARNACLE = 99
	};
private:
	int m_iNumber = -1;
	std::string m_Name;
	bool m_bScoreOverriden = false;
	int m_iFrags = 0;
	int m_iDeaths = 0;

	/**
	 * Called during VidInit to reset internal data.
	 * @param	number	Number of the team (used to set m_iNumber).
	 */
	void Reset();

	/**
	 * Updates state of all teams.
	 */
	static void UpdateAllTeams();
};
#endif
