#pragma once
#include <metahook.h>
#include <map>
#include <vector>
#include <string>
#include <tier1/strtools.h>
#include "plugins.h"
#include "local.h"
#include "vguilocal.h"
#include "player_info.h"
#include "scoreboard.h"
#include <Viewport.h>
#include "net_api.h"
#include <exportfuncs.h>

typedef int GLint;
#include "hud.h"
#include "weapon.h"
#include <CCustomHud.h>

CPlayerInfo CPlayerInfo::m_sPlayerInfo[SC_MAX_PLAYERS + 1];
static CPlayerInfo *s_ThisPlayerInfo = nullptr;
bool g_bInPing = false;
std::wstring g_szPingBuffer;

#define UNDEFINEDTEAM_LOCALIZE_TOKEN "Scores_UndifinedTeam"

namespace
{

std::map<uint64_t, std::string> s_RealNames;

// UTF-friendly version instead of platform-specific ones
bool IsSpace(char c)
{
	// Everything before space is either whitespace or invalid.
	// UTF-8 chars all have bit 7 set to high
	return static_cast<unsigned char>(c) <= ' ';
}

/**
 * Parses string into a SteamID64. Returns 0 if failed.
 * Credits to voogru
 * https://forums.alliedmods.net/showthread.php?t=60899?t=60899
 */
uint64 ParseSteamID(const char *pszAuthID)
{
	if (!pszAuthID)
		return 0;

	int iServer = 0;
	int iAuthID = 0;

	char szAuthID[64];
	strncpy(szAuthID, pszAuthID, sizeof(szAuthID) - 1);
	szAuthID[sizeof(szAuthID) - 1] = '\0';

	char *szTmp = strtok(szAuthID, ":");
	while (szTmp = strtok(NULL, ":"))
	{
		char *szTmp2 = strtok(NULL, ":");
		if (szTmp2)
		{
			iServer = atoi(szTmp);
			iAuthID = atoi(szTmp2);
		}
	}

	if (iAuthID == 0)
		return 0;

	uint64 i64friendID = (long long)iAuthID * 2;

	//Friend ID's with even numbers are the 0 auth server.
	//Friend ID's with odd numbers are the 1 auth server.
	i64friendID += 76561197960265728 + iServer;

	return i64friendID;
}

void UnloadAuthID()
{
	s_RealNames.clear();

	for (int i = 1; i <= SC_MAX_PLAYERS; i++)
	{
		GetPlayerInfo(i)->ClearRealName();
	}
}

void LoadAuthID()
{
	UnloadAuthID();
	FileHandle_t hFile = g_pFullFileSystem->Open("realnames.txt", "r");

	if (hFile == FILESYSTEM_INVALID_HANDLE)
	{
		ConsoleWriteline("Failed to open realnames.txt");
		return;
	}

	char linebuf[512];

	for (int linenum = 1; g_pFullFileSystem->ReadLine(linebuf, sizeof(linebuf), hFile); linenum++)
	{
		char steamid[64];
		char name[64];
		int idx = 0;

		if (!linebuf[idx])
			continue; // Empty line

		// Skip spaces
		while (linebuf[idx] && IsSpace(linebuf[idx]))
			idx++;

		if (!linebuf[idx])
			continue; // Empty line

		// Find next space
		int steamidbegin = idx;
		while (linebuf[idx] && !IsSpace(linebuf[idx]))
			idx++;

		if (!linebuf[idx])
		{
			ConsoleWriteline("Line %d: unexpected end of line [1]\n", linenum);
			continue;
		}

		// Copy SteamID
		int steamidend = idx;
		int steamidlen = steamidend - steamidbegin;

		if (steamidlen >= 2)
		{
			if (linebuf[steamidbegin + 0] == '/' && linebuf[steamidbegin + 1] == '/')
			{
				// Skip comment
				continue;
			}
		}

		if (steamidlen >= sizeof(steamid))
		{
			ConsoleWriteline("Line %d: SteamID too long (%d > %d)\n", linenum, steamidlen, sizeof(steamid) - 1);
			continue;
		}

		memcpy(steamid, linebuf + steamidbegin, steamidlen);
		steamid[steamidlen] = '\0';

		// Skip spaces
		while (linebuf[idx] && IsSpace(linebuf[idx]))
			idx++;

		if (!linebuf[idx])
		{
			ConsoleWriteline("Line %d: unexpected end of line [2]\n", linenum);
			continue;
		}

		int namebegin = idx;

		// Find end of the string
		while (linebuf[idx])
			idx++;
		int nameend = idx;
		int namelen = nameend - namebegin;

		if (namelen >= sizeof(name))
		{
			ConsoleWriteline("Line %d: name too long (%d > %d)\n", linenum, namelen, sizeof(name) - 1);
			continue;
		}

		memcpy(name, linebuf + namebegin, namelen);
		name[namelen] = '\0';

		if (name[namelen - 1] == '\n')
			name[namelen - 1] = '\0';

		// Parse SteamID
		uint64_t steamid64 = ParseSteamID(steamid);

		if (steamid64 == 0)
		{
			ConsoleWriteline("Line %d: failed to parse SteamID\n", linenum);
			continue;
		}

		s_RealNames.insert({ steamid64, name });
	}

	ConsoleWriteline("Loaded %d realnames\n", s_RealNames.size());
}

}

CPlayerInfo *GetThisPlayerInfo()
{
	return s_ThisPlayerInfo;
}

void CPlayerInfo::InitPlayerInfos() {
	// Set player info IDs
	for (int i = 1; i < SC_MAX_PLAYERS; i++)
		CPlayerInfo::m_sPlayerInfo[i].m_iIndex = i;
}

int CPlayerInfo::GetIndex()
{
	return m_iIndex;
}

bool CPlayerInfo::IsConnected()
{
	return m_bIsConnected;
}

const char *CPlayerInfo::GetName()
{
	Assert(m_bIsConnected);
	return szName;
}

int CPlayerInfo::GetPing()
{
	Assert(m_bIsConnected);
	return iPing;
}

int CPlayerInfo::GetPacketLoss()
{
	Assert(m_bIsConnected);
	return iLoss;
}

bool CPlayerInfo::IsThisPlayer()
{
	Assert(m_bIsConnected);
	return m_iIndex == 1;
}

const char *CPlayerInfo::GetModel()
{
	Assert(m_bIsConnected);
	return szModel;
}

int CPlayerInfo::GetTopColor()
{
	Assert(m_bIsConnected);
	return iTopColor;
}

int CPlayerInfo::GetBottomColor()
{
	Assert(m_bIsConnected);
	return iBottomColor;
}

uint64 CPlayerInfo::GetValidSteamID64()
{
	return CSteamID(m_szSteamID,k_EUniversePublic).ConvertToUint64();
}

uint64 CPlayerInfo::GetStatusSteamID64()
{
	Assert(m_bIsConnected);
	return ParseSteamID(m_szSteamID);
}

int CPlayerInfo::GetFrags()
{
	Assert(m_bIsConnected);
	return m_ExtraInfo.frags;
}

int CPlayerInfo::GetDeaths()
{
	Assert(m_bIsConnected);
	return m_ExtraInfo.deaths;
}

int CPlayerInfo::GetHealth()
{
	Assert(m_bIsConnected);
	return m_ExtraInfo.health;
}

int CPlayerInfo::GetArmor()
{
	Assert(m_bIsConnected);
	return m_ExtraInfo.armor;
}

int CPlayerInfo::GetPlayerClass()
{
	Assert(m_bIsConnected);
	return m_ExtraInfo.playerclass;
}

int CPlayerInfo::GetTeamNumber()
{
	Assert(m_bIsConnected);
	return m_ExtraInfo.teamnumber;
}

const char *CPlayerInfo::GetTeamName()
{
	Assert(m_bIsConnected);
	return m_ExtraInfo.teamname;
}

bool CPlayerInfo::IsSpectator()
{
	return m_bIsSpectator;
}

const char *CPlayerInfo::GetDisplayName()
{
	const char *name = nullptr;
	if (m_szRealName[0])
		name = m_szRealName;
	else
		name = GetName();
	return name;
}

const char* CPlayerInfo::GetSteamIDString()
{
	return m_szSteamID;
}
CPlayerInfo *CPlayerInfo::Update()
{
	hud_player_info_t hInfo;
	gEngfuncs.pfnGetPlayerInfo(m_iIndex, &hInfo);
	bool bWasConnected = m_bIsConnected;
	bool bIsConnected = hInfo.name != nullptr;
	m_bIsConnected = bIsConnected;

	if (bIsConnected != bWasConnected)
	{
		m_szSteamID[0] = '\0';
		m_szRealName[0] = '\0';
		m_bRealNameChecked = false;
		m_iStatusPenalty = 0;
		m_flLastStatusRequest = 0;
		g_pViewPort->GetScoreBoard()->UpdateOnPlayerInfo(GetIndex());
	}

	if (bIsConnected)
	{
		if (!m_szSteamID[0])
		{
			// Player has no SteamID, update it
			float period = (m_iStatusPenalty < STATUS_PENALTY_THRESHOLD) ? STATUS_PERIOD : STATUS_BUGGED_PERIOD;
			if (!g_bInPing && m_flLastStatusRequest + period < gEngfuncs.GetAbsoluteTime())
			{
				g_bInPing = true;
				ServerCmd("status");
				m_flLastStatusRequest = gEngfuncs.GetAbsoluteTime();
			}
		}

		hud_playerinfo_t* extraInfo = gCustomHud.GetPlayerHUDInfo(m_iIndex);
		//Ping loss
		player_info_t* info = gEngineStudio.PlayerInfo(m_iIndex);
		iPing = info->ping;
		iLoss = info->packet_loss;
		iTopColor = info->topcolor;
		iBottomColor = info->bottomcolor;
		strcpy_s(szName, hInfo.name);
		m_bIsSpectator = gCustomHud.IsSpectator(m_iIndex);

		m_ExtraInfo.armor = extraInfo->armor;
		m_ExtraInfo.frags = extraInfo->frags;
		m_ExtraInfo.deaths = extraInfo->death;
		m_ExtraInfo.health = extraInfo->health;
		m_ExtraInfo.teamnumber = extraInfo->team;
		Q_strcpy(m_ExtraInfo.teamname, GetTeamInfo(0)->GetNameByIndex(extraInfo->team));
		
		//RealNameGet
		if (!m_bRealNameChecked && m_szSteamID[0] != '\0')
		{
			m_bRealNameChecked = true;

			if (!s_RealNames.empty())
			{
				// Find real name
				uint64_t steamid64 = GetStatusSteamID64();
				auto it = s_RealNames.find(steamid64);

				if (it != s_RealNames.end())
				{
					Q_strncpy(m_szRealName, it->second.c_str(), sizeof(m_szRealName));
				}
			}
		}

		if (IsThisPlayer())
			s_ThisPlayerInfo = this;
	}

	return this;
}

void CPlayerInfo::UpdateAll() {
	for (int i = 1; i < SC_MAX_PLAYERS; i++)
		CPlayerInfo::m_sPlayerInfo[i].Update();
}

bool CPlayerInfo::HasRealName()
{
	return m_szRealName[0] != '\0';
}

void CPlayerInfo::ClearRealName()
{
	m_szRealName[0] = '\0';
	m_bRealNameChecked = false;
}

player_info_t *CPlayerInfo::GetEnginePlayerInfo()
{
	Assert(m_bIsConnected);
	if (!szName)
		return nullptr;
	player_info_t *ptr = reinterpret_cast<player_info_t *>(szName - offsetof(player_info_t, name));
	return ptr;
}

void CPlayerInfo::Reset()
{
	m_ExtraInfo = extra_player_info_t();
	m_bIsConnected = false;
	m_bIsSpectator = false;
	memset(m_szSteamID, 0, sizeof(m_szSteamID));
}
void CPlayerInfo::ResetAll() {
	for (int i = 1; i < SC_MAX_PLAYERS; i++)
		CPlayerInfo::m_sPlayerInfo[i].Reset();
}

//-----------------------------------------------------

CTeamInfo CTeamInfo::m_sTeamInfo[SC_MAX_TEAMS + 1];
enum {
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
#define CLASS_NONE_TOKEN "Team_None"
#define CLASS_PLAYER_TOKEN "Team_Player"
#define CLASS_OTHER_TOKEN "Team_Other"
#define CLASS_RED_TOKEN "Team_Red"
#define CLASS_BLUE_TOKEN "Team_Blue"
#define CLASS_GREEN_TOKEN "Team_Green"
#define CLASS_YELLOW_TOKEN "Team_Yellow"
const char* CTeamInfo::GetNameByIndex(uint i) {
	switch (i) {
		case CLASS_FORCE_NONE:
		case CLASS_NONE: {
			return CLASS_NONE_TOKEN;
			break;
		}
		case CLASS_PLAYER: {
			return CLASS_PLAYER_TOKEN;
			break;
		}
		case CLASS_TEAM1: {
			return CLASS_BLUE_TOKEN;
			break;
		}
		case CLASS_TEAM2: {
			return CLASS_RED_TOKEN;
			break;
		}
		case CLASS_TEAM3: {
			return CLASS_YELLOW_TOKEN;
			break;
		}
		case CLASS_TEAM4: {
			return CLASS_GREEN_TOKEN;
			break;
		}
		default:
		case CLASS_MACHINE:
		case CLASS_HUMAN_PASSIVE:
		case CLASS_HUMAN_MILITARY:
		case CLASS_ALIEN_MILITARY:
		case CLASS_ALIEN_PASSIVE:
		case CLASS_ALIEN_MONSTER:
		case CLASS_ALIEN_PREY:
		case CLASS_ALIEN_PREDATOR:
		case CLASS_INSECT:
		case CLASS_PLAYER_ALLY:
		case CLASS_PLAYER_BIOWEAPON:
		case CLASS_ALIEN_BIOWEAPON:
		case CLASS_XRACE_PITDRONE:
		case CLASS_XRACE_SHOCK:
		case CLASS_BARNACLE: {
			return CLASS_OTHER_TOKEN;
			break;
		}
	}
	return CLASS_OTHER_TOKEN;
}

int CTeamInfo::GetNumber()
{
	return m_iNumber;
}

const char *CTeamInfo::GetName()
{
	return m_Name;
}

bool CTeamInfo::IsScoreOverriden()
{
	return m_bScoreOverriden;
}

int CTeamInfo::GetFrags()
{
	Assert(m_bScoreOverriden);
	return m_iFrags;
}

int CTeamInfo::GetDeaths()
{
	Assert(m_bScoreOverriden);
	return m_iDeaths;
}

void CTeamInfo::InitTeamInfos() {
	for(size_t i = 0; i < SC_MAX_TEAMS;i++){
		strcpy_s(m_sTeamInfo[i].m_Name, GetNameByIndex(i));
	}
}

void CTeamInfo::Reset()
{
	m_bScoreOverriden = false;
	m_iFrags = 0;
	m_iDeaths = 0;
}

void CTeamInfo::ResetAll() {
	for (CTeamInfo t : m_sTeamInfo) {
		t.Reset();
	}
}

void CTeamInfo::UpdateAllTeams()
{
	for (int i = 1; i <= SC_MAX_PLAYERS; i++)
	{
		CPlayerInfo *pi = GetPlayerInfo(i);

		if (!pi->IsConnected())
			continue;

		if (pi->GetTeamNumber() < 0 || pi->GetTeamNumber() > SC_MAX_TEAMS)
			continue;
	}
}
