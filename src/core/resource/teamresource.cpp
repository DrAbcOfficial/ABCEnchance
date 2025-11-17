#include "playerresource.h"
#include "teamresource.h"


const std::array<PREDEFINED_TEAM, PREDEFINED_TEAM_COUNT> g_aryPredefinedTeam = {
		PREDEFINED_TEAM{ TEAM_ID::FORCE_NONE, "Team_ForceNone" },
		PREDEFINED_TEAM{ TEAM_ID::NONE, "Team_None" },
		PREDEFINED_TEAM{ TEAM_ID::MACHINE, "Team_Machine" },
		PREDEFINED_TEAM{ TEAM_ID::PLAYER, "Team_Player" },
		PREDEFINED_TEAM{ TEAM_ID::HUMAN_PASSIVE, "Team_HumanPassive" },
		PREDEFINED_TEAM{ TEAM_ID::HUMAN_MILITARY, "Team_HumanMilitary" },
		PREDEFINED_TEAM{ TEAM_ID::ALIEN_MILITARY, "Team_AlienMilitary" },
		PREDEFINED_TEAM{ TEAM_ID::ALIEN_PASSIVE, "Team_AlienPassive" },
		PREDEFINED_TEAM{ TEAM_ID::ALIEN_MONSTER, "Team_AlienMonster" },
		PREDEFINED_TEAM{ TEAM_ID::ALIEN_PREY, "Team_AlienPrey" },
		PREDEFINED_TEAM{ TEAM_ID::ALIEN_PREDATOR, "Team_AlienPredator" },
		PREDEFINED_TEAM{ TEAM_ID::INSECT, "Team_Insect" },
		PREDEFINED_TEAM{ TEAM_ID::PLAYER_ALLY, "Team_PlayerAlly" },
		PREDEFINED_TEAM{ TEAM_ID::PLAYER_BIOWEAPON, "Team_PlayerBioweapon" },
		PREDEFINED_TEAM{ TEAM_ID::ALIEN_BIOWEAPON, "Team_AlienBioweapon" },
		PREDEFINED_TEAM{ TEAM_ID::XRACE_PITDRONE, "Team_XracePitdrone" },
		PREDEFINED_TEAM{ TEAM_ID::XRACE_SHOCK, "Team_XraceShock" },
		PREDEFINED_TEAM{ TEAM_ID::TEAM1, "Team_Team1" },
		PREDEFINED_TEAM{ TEAM_ID::TEAM2, "Team_Team2" },
		PREDEFINED_TEAM{ TEAM_ID::TEAM3, "Team_Team3" },
		PREDEFINED_TEAM{ TEAM_ID::TEAM4, "Team_Team4" },
		PREDEFINED_TEAM{ TEAM_ID::BARNACLE, "Team_Barnacle" }
};
std::array<TeamInfo, PREDEFINED_TEAM_COUNT>& TeamResource::GetTeamInfos() {
	return m_aryTeamInfo;
}
const char* TeamResource::GetTeamNameToken(TEAM_ID i) {
	for (auto& team : g_aryPredefinedTeam) {
		if (team.iTeamId == i) {
			return team.pszTeamToken;
		}
	}
	return "Team_Unknown";
}
void TeamResource::Init() {
	for (size_t i = 0; i < PREDEFINED_TEAM_COUNT; ++i) {
		m_aryTeamInfo[i].m_iNumber = g_aryPredefinedTeam[i].iTeamId;
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
		if (pi->m_iTeamNumber < TEAM_ID::NONE)
			continue;
	}
}
int TeamResource::GetTeamIndexByTeamID(TEAM_ID id){
	for (size_t i = 0; i < m_aryTeamInfo.size();i++) {
		if (m_aryTeamInfo[i].m_iNumber == id)
			return (int)i;
	}
	return -1;
}
TeamInfo* TeamResource::GetTeamInfo(TEAM_ID id){
	int idx = GetTeamIndexByTeamID(id);
	if(idx < 0)
		return nullptr;
	return GetTeamInfo(idx);
}
TeamInfo* TeamResource::GetTeamInfo(int index) {
	return &m_aryTeamInfo[index];
}

TeamResource gTeamRes;

TeamInfo::TeamInfo(PREDEFINED_TEAM& t){
	m_iNumber = t.iTeamId;
	m_Name = t.pszTeamToken;
}
TEAM_ID TeamInfo::GetNumber() const {
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