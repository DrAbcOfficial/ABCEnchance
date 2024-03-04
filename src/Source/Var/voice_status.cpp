#include <metahook.h>
#include <assert.h>

#include "parsemsg.h"
#include "cvardef.h"
#include "local.h"
#include "player_info.h"

#include "voice_status.h"



#define VOICE_MODEL_INTERVAL		0.3
#define SCOREBOARD_BLINK_FREQUENCY	0.3	// How often to blink the scoreboard icons.
#define SQUELCHOSCILLATE_PER_SECOND	2.0f

// ---------------------------------------------------------------------- //
// The voice manager for the client.
// ---------------------------------------------------------------------- //
static CVoiceStatus g_VoiceStatus;

CVoiceStatus* GetClientVoiceMgr(){
	return &g_VoiceStatus;
}

// ---------------------------------------------------------------------- //
// CVoiceStatus.
// ---------------------------------------------------------------------- //

static CVoiceStatus *g_pInternalVoiceStatus = NULL;

pfnUserMsgHook pfnVoiceMask;
pfnUserMsgHook pfnReqState;
int __MsgFunc_VoiceMask(const char *pszName, int iSize, void *pbuf){
	if(g_pInternalVoiceStatus)
		g_pInternalVoiceStatus->HandleVoiceMaskMsg(iSize, pbuf);
	return pfnVoiceMask(pszName, iSize, pbuf);
}
int __MsgFunc_ReqState(const char *pszName, int iSize, void *pbuf){
	if(g_pInternalVoiceStatus)
		g_pInternalVoiceStatus->HandleReqStateMsg(iSize, pbuf);
	return pfnReqState(pszName, iSize, pbuf);
}

// ---------------------------------------------------------------------- //
// CVoiceStatus.
// ---------------------------------------------------------------------- //

CVoiceStatus::CVoiceStatus(){
	m_LastUpdateServerState = 0;
	m_bTalking = m_bServerAcked = false;
	m_bServerModEnable = -1;
}

CVoiceStatus::~CVoiceStatus(){
	g_pInternalVoiceStatus = nullptr;
	m_BanMgr.Save();
}

void CVoiceStatus::HUD_Init(){
	m_BanMgr.Init();

	assert(!g_pInternalVoiceStatus);
	g_pInternalVoiceStatus = this;
	m_bInSquelchMode = false;
	pfnVoiceMask = HOOK_MESSAGE(VoiceMask);
	pfnReqState = HOOK_MESSAGE(ReqState);

	ADD_COMMAND("voice_printbanned", []() {
		if (g_pInternalVoiceStatus) {
			gEngfuncs.pfnConsolePrint("------- BANNED PLAYERS -------\n");
			g_pInternalVoiceStatus->m_BanMgr.ForEachBannedPlayer([](uint64 steamid) {
				char str[70];
				sprintf_s(str, " - %llu\n", steamid);
				gEngfuncs.pfnConsolePrint(str);
				});
			gEngfuncs.pfnConsolePrint("------------------------------\n");
		}
		}
	);
}

void CVoiceStatus::HUD_Frame(double frametime){
	// check server banned players once per second
	if(gEngfuncs.GetClientTime() - m_LastUpdateServerState > 1)
		UpdateServerState(false);
}

void CVoiceStatus::UpdateSpeakerStatus( int entindex, bool bTalking ){
	cvar_t *pVoiceLoopback = nullptr;
	if ( CVAR_GET_FLOAT( "voice_clientdebug" ) ){
		char msg[256];
		snprintf( msg, sizeof( msg ), "CVoiceStatus::UpdateSpeakerStatus: ent %d talking = %d\n", entindex, bTalking );
		gEngfuncs.pfnConsolePrint( msg );
	}

	int iLocalPlayerIndex = gEngfuncs.GetLocalPlayer()->index;
	// Is it the local player talking?
	if ( entindex == -1 ){
		m_bTalking = bTalking;
		if( bTalking ){
			// Enable voice for them automatically if they try to talk.
			EngineClientCmd( "voice_modenable 1" );
		}
		// now set the player index to the correct index for the local player
		// this will allow us to have the local player's icon flash in the scoreboard
		entindex = iLocalPlayerIndex;
		pVoiceLoopback = CVAR_GET_POINTER( "voice_loopback" );
	}
	else if ( entindex == -2 )
		m_bServerAcked = bTalking;

	if ( entindex >= 0 && entindex <= VOICE_MAX_PLAYERS ){
		int iClient = entindex - 1;
		if ( iClient < 0 )
			return;
		if ( bTalking ){
			m_VoicePlayers[iClient] = true;
			m_VoiceEnabledPlayers[iClient] = true;
		}
		else
			m_VoicePlayers[iClient] = false;
	}
}


void CVoiceStatus::UpdateServerState(bool bForce){
	// Can't do anything when we're not in a level.
	char const *pLevelName = gEngfuncs.pfnGetLevelName();
	if( !pLevelName || strlen(pLevelName) == 0){
		if( CVAR_GET_FLOAT("voice_clientdebug") )
			gEngfuncs.pfnConsolePrint( "CVoiceStatus::UpdateServerState: pLevelName[0]==0\n" );
		return;
	}
	int bCVarModEnable = static_cast<int>(CVAR_GET_FLOAT("voice_modenable"));
	if(bForce || m_bServerModEnable != bCVarModEnable){
		m_bServerModEnable = bCVarModEnable;
		char str[256];
		snprintf(str, sizeof(str), "VModEnable %d", m_bServerModEnable);
		ServerCmd(str);
		if(CVAR_GET_FLOAT("voice_clientdebug")){
			char msg[256];
			sprintf(msg, "CVoiceStatus::UpdateServerState: Sending '%s'\n", str);
			gEngfuncs.pfnConsolePrint(msg);
		}
	}
	char str[2048];
	sprintf(str, "vban");
	bool bChange = false;

	unsigned long serverBanMask = 0;
	unsigned long banMask = 0;
	for (unsigned long i = 1; i < 33; i++) {
		CPlayerInfo* info = CPlayerInfo::GetPlayerInfo(i);
		if (!info)
			continue;
		if (m_BanMgr.GetPlayerBan(info->GetSteamID64()))
			banMask |= 1 << i;
	}
	serverBanMask = m_ServerBannedPlayers.to_ulong();
	if(serverBanMask != banMask)
		bChange = true;
	// Ok, the server needs to be updated.
	char numStr[512];
	sprintf(numStr, " %x", banMask);
	strcat(str, numStr);

	if(bChange || bForce){
		if(CVAR_GET_FLOAT("voice_clientdebug")){
			char msg[256];
			sprintf(msg, "CVoiceStatus::UpdateServerState: Sending '%s'\n", str);
			gEngfuncs.pfnConsolePrint(msg);
		}
		gEngfuncs.pfnServerCmdUnreliable(str);	// Tell the server..
	}
	else if (CVAR_GET_FLOAT("voice_clientdebug"))
		gEngfuncs.pfnConsolePrint( "CVoiceStatus::UpdateServerState: no change\n" );
	m_LastUpdateServerState = gEngfuncs.GetClientTime();
}

void CVoiceStatus::HandleVoiceMaskMsg(int iSize, void *pbuf){
	BEGIN_READ( pbuf, iSize );
	unsigned long audiable = static_cast<unsigned long>(READ_LONG());
	unsigned long serverbanned = static_cast<unsigned long>(READ_LONG());
	m_AudiblePlayers = CPlayerBitVec(audiable);
	m_ServerBannedPlayers = CPlayerBitVec(serverbanned);
	if(CVAR_GET_FLOAT("voice_clientdebug")){
		char str[256];
		gEngfuncs.pfnConsolePrint("CVoiceStatus::HandleVoiceMaskMsg\n");
		sprintf(str, "    - m_AudiblePlayers = %s\n", m_AudiblePlayers.to_string().c_str());
		gEngfuncs.pfnConsolePrint(str);
		sprintf(str, "    - m_ServerBannedPlayers = %s\n", m_ServerBannedPlayers.to_string().c_str());
		gEngfuncs.pfnConsolePrint(str);
	}
	m_bServerModEnable = READ_BYTE();
}
void CVoiceStatus::HandleReqStateMsg(int iSize, void *pbuf){
	if(CVAR_GET_FLOAT("voice_clientdebug"))
		gEngfuncs.pfnConsolePrint("CVoiceStatus::HandleReqStateMsg\n");
	UpdateServerState(true);	
}

void CVoiceStatus::StartSquelchMode(){
	if(m_bInSquelchMode)
		return;
	m_bInSquelchMode = true;
}

void CVoiceStatus::StopSquelchMode(){
	m_bInSquelchMode = false;
}

bool CVoiceStatus::IsInSquelchMode() const{
	return m_bInSquelchMode;
}

//-----------------------------------------------------------------------------
// Purpose: returns true if the target client has been banned
// Input  : playerID - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CVoiceStatus::IsPlayerBlocked(int iPlayer){
	CPlayerInfo* info = CPlayerInfo::GetPlayerInfo(iPlayer);
	if (!info)
		return false;
	return m_BanMgr.GetPlayerBan(info->GetSteamID64());
}

//-----------------------------------------------------------------------------
// Purpose: returns true if the player can't hear the other client due to game rules (eg. the other team)
// Input  : playerID - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CVoiceStatus::IsPlayerAudible(int iPlayer) const{
	return m_AudiblePlayers.test(iPlayer - 1);
}

//-----------------------------------------------------------------------------
// Purpose: blocks/unblocks the target client from being heard
// Input  : playerID - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
void CVoiceStatus::SetPlayerBlockedState(int iPlayer, bool blocked){
	if (CVAR_GET_FLOAT("voice_clientdebug"))
		gEngfuncs.pfnConsolePrint( "CVoiceStatus::SetPlayerBlockedState part 1\n" );
	CPlayerInfo* info = CPlayerInfo::GetPlayerInfo(iPlayer);
	if (!info)
		return;
	if (CVAR_GET_FLOAT("voice_clientdebug"))
		gEngfuncs.pfnConsolePrint( "CVoiceStatus::SetPlayerBlockedState part 2\n" );

	// Squelch or (try to) unsquelch this player.
	if (CVAR_GET_FLOAT("voice_clientdebug")){
		char str[256];
		sprintf(str, "CVoiceStatus::SetPlayerBlockedState: setting player %llu ban to %d\n", info->GetSteamID64(), blocked);
		gEngfuncs.pfnConsolePrint(str);
	}
	m_BanMgr.SetPlayerBan( info->GetSteamID64(), blocked);
	UpdateServerState(false);
}
