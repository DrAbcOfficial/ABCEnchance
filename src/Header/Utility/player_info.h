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
#include <tier0/dbg.h>
#include <string>

typedef struct player_info_s player_info_t;

struct extra_player_info_t
{
	int frags;
	int deaths;
	int armor;
	int health;
	bool dead;
	short teamnumber = -1;
	char teamname[SC_MAX_TEAM_NAME];
	short donor;
	short admin;
};

//-----------------------------------------------------

class CPlayerInfo;
CPlayerInfo *GetPlayerInfo(int idx);
CPlayerInfo *GetThisPlayerInfo();

extern bool g_bInPing;
extern std::wstring g_szPingBuffer;

class CPlayerInfo
{
public:
	void InitPlayerInfos();
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

	/**
	 * Clears saved realname. Should be called when realnames are unloaded.
	 */
	void ClearRealName();
	void Reset();
	void ResetAll();

	CSteamID m_pSteamId;

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
	char szName[SC_MAX_PLAYER_NAME];
	int iPing;
	int iLoss;
	char szModel[SC_MAX_PLAYER_NAME];
	int iTopColor;
	int iBottomColor;
	extra_player_info_t m_ExtraInfo;
	bool m_bIsConnected;
	char m_szRealName[SC_MAX_PLAYER_NAME + 1];

	static CPlayerInfo m_sPlayerInfo[SC_MAX_PLAYERS + 1];
	friend CPlayerInfo *GetPlayerInfo(int idx);
	friend class CSvcMessages;
};

inline CPlayerInfo *GetPlayerInfo(int idx)
{
	Assert(idx >= 1 && idx <= SC_MAX_PLAYERS);
	return &CPlayerInfo::m_sPlayerInfo[idx];
}

//-----------------------------------------------------

class CTeamInfo;
CTeamInfo *GetTeamInfo(int number);

class CTeamInfo
{
public:
	const char* GetNameByIndex(uint index);
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
	void InitTeamInfos();

private:
	int m_iNumber = -1;
	char m_Name[SC_MAX_TEAM_NAME];
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

	static CTeamInfo m_sTeamInfo[SC_MAX_TEAMS + 1];
	friend CTeamInfo *GetTeamInfo(int number);
};

inline CTeamInfo *GetTeamInfo(int number)
{
	Assert(number >= 0 && number <= SC_MAX_TEAMS);
	return &CTeamInfo::m_sTeamInfo[number];
}

#endif
