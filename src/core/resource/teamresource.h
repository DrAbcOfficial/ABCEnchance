#pragma once
#ifndef __TEAM_RESOURCE_H__
#define __TEAM_RESOURCE_H__

#pragma region stl
#include <array>
#include <string>
#pragma endregion

class TeamResource;

enum class TEAM_ID {
	FORCE_NONE = -1,
	NONE,
	MACHINE,
	PLAYER,
	HUMAN_PASSIVE,
	HUMAN_MILITARY,
	ALIEN_MILITARY,
	ALIEN_PASSIVE,
	ALIEN_MONSTER,
	ALIEN_PREY,
	ALIEN_PREDATOR,
	INSECT,
	PLAYER_ALLY,
	PLAYER_BIOWEAPON,
	ALIEN_BIOWEAPON,
	XRACE_PITDRONE,
	XRACE_SHOCK,
	TEAM1,
	TEAM2,
	TEAM3,
	TEAM4,
	BARNACLE = 99
};

struct PREDEFINED_TEAM {
	TEAM_ID iTeamId;
	const char* pszTeamToken;
};
constexpr auto PREDEFINED_TEAM_COUNT = 22;
extern const std::array<PREDEFINED_TEAM, PREDEFINED_TEAM_COUNT> g_aryPredefinedTeam;

class TeamInfo {
public:
	TeamInfo() = default;
	TeamInfo(PREDEFINED_TEAM& t);
	/**
	 * Returns team number.
	 */
	TEAM_ID GetNumber() const;

	/**
	 * Returns name of the team. This is the one returned by CPlayerInfo::GetTeamName().
	 */
	const char* GetName();
	/**
	 * Returns whether TeamScore message was used to override the scores.
	 * If true, use GetFrags and GetDeaths instead of calculated values.
	 */
	bool IsScoreOverriden() const;

	/**
	 * Returns frag count.
	 * Only valid if IsScoreOverriden() == true.
	 */
	int GetFrags() const;

	/**
	 * Returns death count.
	 * Only valid if IsScoreOverriden() == true.
	 */
	int GetDeaths() const;
private:
	TEAM_ID m_iNumber = TEAM_ID::FORCE_NONE;
	std::string m_Name;
	bool m_bScoreOverriden = false;
	int m_iFrags = 0;
	int m_iDeaths = 0;
	/**
	 * Called during VidInit to reset internal data.
	 * @param	number	Number of the team (used to set m_iNumber).
	 */
	void Reset();

	friend class TeamResource;
};

class TeamResource{
public:
	void Init();
	void ResetAll();
	/**
	 * Updates state of all teams.
	 */
	void UpdateAllTeams();
	std::array<TeamInfo, PREDEFINED_TEAM_COUNT>& GetTeamInfos();
	const char* GetTeamNameToken(TEAM_ID id);

	int GetTeamIndexByTeamID(TEAM_ID id);

	TeamInfo* GetTeamInfo(TEAM_ID id);
	TeamInfo* GetTeamInfo(int index);
private:
	std::array<TeamInfo, PREDEFINED_TEAM_COUNT> m_aryTeamInfo;
};
extern TeamResource gTeamRes;
#endif