#include "playerresource.h"
#include "teamresource.h"


const std::array<PREDEFINED_TEAM, PREDEFINED_TEAM_COUNT> g_aryPredefinedTeam = {
		PREDEFINED_TEAM{ TEAM_INDEX::FORCE_NONE, "Team_ForceNone" },
		PREDEFINED_TEAM{ TEAM_INDEX::NONE, "Team_None" },
		PREDEFINED_TEAM{ TEAM_INDEX::MACHINE, "Team_Machine" },
		PREDEFINED_TEAM{ TEAM_INDEX::PLAYER, "Team_Player" },
		PREDEFINED_TEAM{ TEAM_INDEX::HUMAN_PASSIVE, "Team_HumanPassive" },
		PREDEFINED_TEAM{ TEAM_INDEX::HUMAN_MILITARY, "Team_HumanMilitary" },
		PREDEFINED_TEAM{ TEAM_INDEX::ALIEN_MILITARY, "Team_AlienMilitary" },
		PREDEFINED_TEAM{ TEAM_INDEX::ALIEN_PASSIVE, "Team_AlienPassive" },
		PREDEFINED_TEAM{ TEAM_INDEX::ALIEN_MONSTER, "Team_AlienMonster" },
		PREDEFINED_TEAM{ TEAM_INDEX::ALIEN_PREY, "Team_AlienPrey" },
		PREDEFINED_TEAM{ TEAM_INDEX::ALIEN_PREDATOR, "Team_AlienPredator" },
		PREDEFINED_TEAM{ TEAM_INDEX::INSECT, "Team_Insect" },
		PREDEFINED_TEAM{ TEAM_INDEX::PLAYER_ALLY, "Team_PlayerAlly" },
		PREDEFINED_TEAM{ TEAM_INDEX::PLAYER_BIOWEAPON, "Team_PlayerBioweapon" },
		PREDEFINED_TEAM{ TEAM_INDEX::ALIEN_BIOWEAPON, "Team_AlienBioweapon" },
		PREDEFINED_TEAM{ TEAM_INDEX::XRACE_PITDRONE, "Team_XracePitdrone" },
		PREDEFINED_TEAM{ TEAM_INDEX::XRACE_SHOCK, "Team_XraceShock" },
		PREDEFINED_TEAM{ TEAM_INDEX::TEAM1, "Team_Team1" },
		PREDEFINED_TEAM{ TEAM_INDEX::TEAM2, "Team_Team2" },
		PREDEFINED_TEAM{ TEAM_INDEX::TEAM3, "Team_Team3" },
		PREDEFINED_TEAM{ TEAM_INDEX::TEAM4, "Team_Team4" },
		PREDEFINED_TEAM{ TEAM_INDEX::BARNACLE, "Team_Barnacle" }
};
std::array<TeamInfo, PREDEFINED_TEAM_COUNT>& TeamResource::GetTeamInfos() {
	return m_aryTeamInfo;
}
const char* TeamResource::GetTeamNameToken(TEAM_INDEX i) {
	for(auto& team : g_aryPredefinedTeam) {
		if (team.iTeamIndex == i) {
			return team.pszTeamToken;
		}
	}
	return "Team_Unkown";
}
void TeamResource::Init() {
	for (auto i = PREDEFINED_TEAM_COUNT; i < PREDEFINED_TEAM_COUNT; i++) {
		m_aryTeamInfo[i].m_iNumber = g_aryPredefinedTeam[i].iTeamIndex;
		m_aryTeamInfo[i].m_Name = g_aryPredefinedTeam[i].pszTeamToken;
	}
}
void TeamResource::ResetAll() {
	for(auto& team : m_aryTeamInfo) {
		team.Reset();
	}
}
void TeamResource::UpdateAllTeams() {
	for (int i = 1; i <= SC_MAX_PLAYERS; i++) {
		PlayerInfo* pi = gPlayerRes.GetPlayerInfo(i);
		if (!pi->IsValid())
			continue;
		if (pi->m_iTeamNumber < TEAM_INDEX::NONE)
			continue;
	}
}
TeamInfo* TeamResource::GetTeamInfo(int number) {
	return &m_aryTeamInfo[number];
}

TeamResource gTeamRes;

TeamInfo::TeamInfo(PREDEFINED_TEAM& t){
	m_iNumber = t.iTeamIndex;
	m_Name = t.pszTeamToken;
}
TEAM_INDEX TeamInfo::GetNumber() const {
	return m_iNumber;
}
const char* TeamInfo::GetName() {
	return m_Name.c_str();
}
bool TeamInfo::IsScoreOverriden() const {
	return m_bScoreOverriden;
}
int TeamInfo::GetFrags() const {
	return m_iFrags;
}
int TeamInfo::GetDeaths() const {
	return m_iDeaths;
}
void TeamInfo::Reset() {
	m_bScoreOverriden = false;
	m_iFrags = 0;
	m_iDeaths = 0;
}