

#pragma once
#pragma region MetaHook
#include "metahook.h"
#include "exportfuncs.h"
#pragma endregion

#pragma region hlsdk
#include "com_model.h"
#pragma endregion

#include "playerresource.h"

PlayerResource gPlayerRes;

constexpr auto UNDEFINEDTEAM_LOCALIZE_TOKEN = "Scores_UndifinedTeam";

void PlayerResource::Init() {
	for (int i = 1; i <= SC_MAX_PLAYERS; ++i) {
		m_aryPlayerInfos[i].m_iIndex = i;
	}
}

void PlayerResource::ResetAll() {
	for (auto& info : m_aryPlayerInfos) {
		info.Reset();
	}
}

void PlayerResource::UpdateAll(){
	for(auto& info : m_aryPlayerInfos) {
		info.Update();
	}
}

//Entindex
PlayerInfo* PlayerResource::GetPlayerInfo(int idx) {
	return &m_aryPlayerInfos[idx];
}

PlayerInfo* PlayerResource::GetLocalPlayerInfo() {
	cl_entity_t* ent = gEngfuncs.GetLocalPlayer();
	if (ent)
		return GetPlayerInfo(ent->index);
	return nullptr;
}

bool PlayerResource::IsInSpectate(int i)
{
	auto plr = gEngfuncs.GetEntityByIndex(i);
	if (plr)
		return plr->curstate.iuser1 > 0;
	else
		return false;
}

bool PlayerInfo::IsValid() const {
	return m_bIsConnected && m_szName.size() > 0;
}
bool PlayerInfo::IsThisPlayer() const {
	return m_iIndex == gEngfuncs.GetLocalPlayer()->index;
}

void PlayerInfo::UpdatePing() {
	auto info = gEngineStudio.PlayerInfo(m_iIndex - 1);
	m_iPing = info->ping;
	m_iLoss = info->packet_loss;
}
PlayerInfo* PlayerInfo::Update() {
	auto info = reinterpret_cast<player_info_sc_t*>(gEngineStudio.PlayerInfo(m_iIndex - 1));
	bool bWasConnected = m_bIsConnected;
	bool bIsConnected = info != nullptr;
	m_bIsConnected = bIsConnected;
	if (bIsConnected != bWasConnected) {
		// Player connected or disconnected
		m_szName.clear();
		m_szRealName.clear();
		m_szModel.clear();
		m_pSteamId.Clear();
		g_EventManager.TriggerEvent(PlayerInfoUpdateEvent(this));
	}
	if (bIsConnected) {
		if (!m_pSteamId.IsValid() || info->m_nSteamID != m_pSteamId.ConvertToUint64()) {
			// Player has no SteamID or not the past guy, update it
			if (m_iIndex == gEngfuncs.GetLocalPlayer()->index)
				m_pSteamId = SteamUser()->GetSteamID();
			else
				m_pSteamId = CSteamID(info->m_nSteamID);
			//RealNameGet
			if (m_pSteamId.IsValid()) {
				m_szRealName = SteamFriends()->GetFriendPersonaName(m_pSteamId);
				snprintf(m_szSteamID64, sizeof(m_szSteamID64), "%llu", m_pSteamId.ConvertToUint64());
			}
		}
		m_iTopColor = info->topcolor;
		m_iBottomColor = info->bottomcolor;
		m_iPing = info->ping;
		m_iLoss = info->packet_loss;
		m_szName = info->name;
		m_szModel = info->model;

		m_eDonor = m_eHideExtra != HIDE_EXTRA::NO ?
			static_cast<DONOR>(g_aryNativeHUDPanelInfo[m_iIndex - 1].donor) :
			DONOR::NONE;
	}
	return this;
}

const char* PlayerInfo::GetName() const {
	return m_szName.c_str();
}
bool PlayerInfo::HasRealName() {
	return m_szRealName[0] != '\0';
}
void PlayerInfo::Reset() {
	m_iPing = 0;
	m_iLoss = 0;
	m_szModel.clear();
	m_szName.clear();
	m_szRealName.clear();
	m_iTopColor = 0;
	m_iBottomColor = 0;
	m_iFrags = 0;
	m_iDeaths = 0;
	m_iArmor = 0;
	m_iHealth = 0;
	m_bDead = false;
	m_iTeamNumber = TEAM_ID::NONE;
	memset(m_szTeamName, 0, sizeof(m_szTeamName));
	memset(m_szSteamID64, 0, sizeof(m_szSteamID64));
	m_iAdmin = ADMIN::NONE;
	m_bIsConnected = false;
	m_bIsSpectate = false;
	m_eHideExtra = HIDE_EXTRA::NO;
	m_eDonor = DONOR::NONE;
	m_pSteamId.Clear();
}

PlayerInfoUpdateEvent::PlayerInfoUpdateEvent(PlayerInfo* p)
{
	m_pPlayerInfo = p;
}