#include "hud.h"

#include <metahook.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "demo_api.h"
#include "voice_status.h"
#include "r_efx.h"
#include "entity_types.h"
#include "cvardef.h"
#include <local.h>

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

int g_BannedPlayerPrintCount;
void ForEachBannedPlayer(char id[16]){
	char str[256];
	sprintf(str, "Ban %d: %2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x\n",
		g_BannedPlayerPrintCount++,
		id[0], id[1], id[2], id[3], 
		id[4], id[5], id[6], id[7], 
		id[8], id[9], id[10], id[11], 
		id[12], id[13], id[14], id[15]
		);
	strupr(str);
	gEngfuncs.pfnConsolePrint(str);
}


void ShowBannedCallback(){
	if(g_pInternalVoiceStatus){
		g_BannedPlayerPrintCount = 0;
		gEngfuncs.pfnConsolePrint("------- BANNED PLAYERS -------\n");
		g_pInternalVoiceStatus->m_BanMgr.ForEachBannedPlayer(ForEachBannedPlayer);
		gEngfuncs.pfnConsolePrint("------------------------------\n");
	}
}


// ---------------------------------------------------------------------- //
// CVoiceStatus.
// ---------------------------------------------------------------------- //

CVoiceStatus::CVoiceStatus(){
	m_bBanMgrInitialized = false;
	m_LastUpdateServerState = 0;
	m_bTalking = m_bServerAcked = false;
	m_bServerModEnable = -1;
	m_pchGameDir = nullptr;
}


CVoiceStatus::~CVoiceStatus(){
	g_pInternalVoiceStatus = nullptr;
	if(m_pchGameDir){
		if(m_bBanMgrInitialized)
			m_BanMgr.SaveState(m_pchGameDir);
		free(m_pchGameDir);
	}
}


int CVoiceStatus::Init(){
	if(gEngfuncs.pfnGetGameDirectory()){
		m_BanMgr.Init(gEngfuncs.pfnGetGameDirectory());
		m_bBanMgrInitialized = true;
	}
	assert(!g_pInternalVoiceStatus);
	g_pInternalVoiceStatus = this;
	m_bInSquelchMode = false;
	pfnVoiceMask = HOOK_MESSAGE(VoiceMask);
	pfnReqState = HOOK_MESSAGE(ReqState);
	// Cache the game directory for use when we shut down
	const char *pchGameDirT = gEngfuncs.pfnGetGameDirectory();
	m_pchGameDir = (char *)malloc(strlen(pchGameDirT) + 1);
	strcpy(m_pchGameDir, pchGameDirT);
	return 1;
}

void CVoiceStatus::Frame(double frametime){
	// check server banned players once per second
	if(gEngfuncs.GetClientTime() - m_LastUpdateServerState > 1)
		UpdateServerState(false);
}

void CVoiceStatus::UpdateSpeakerStatus( int entindex, qboolean bTalking )
{
	cvar_t *pVoiceLoopback = nullptr;
	if ( CVAR_GET_FLOAT( "voice_clientdebug" ) ){
		char msg[256];
		_snprintf( msg, sizeof( msg ), "CVoiceStatus::UpdateSpeakerStatus: ent %d talking = %d\n", entindex, bTalking );
		gEngfuncs.pfnConsolePrint( msg );
	}
	int iLocalPlayerIndex = gEngfuncs.GetLocalPlayer()->index;
	// Is it the local player talking?
	if ( entindex == -1 ){
		m_bTalking = !!bTalking;
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
		m_bServerAcked = !!bTalking;

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
	if( pLevelName[0] == 0 ){
		if( CVAR_GET_FLOAT("voice_clientdebug") )
			gEngfuncs.pfnConsolePrint( "CVoiceStatus::UpdateServerState: pLevelName[0]==0\n" );
		return;
	}
	int bCVarModEnable = !!CVAR_GET_FLOAT("voice_modenable");
	if(bForce || m_bServerModEnable != bCVarModEnable){
		m_bServerModEnable = bCVarModEnable;
		char str[256];
		_snprintf(str, sizeof(str), "VModEnable %d", m_bServerModEnable);
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

	for(unsigned long dw=0; dw < VOICE_MAX_PLAYERS_DW; dw++){	
		unsigned long serverBanMask = 0;
		unsigned long banMask = 0;
		for(unsigned long i=0; i < 32; i++){
			char playerID[16];
			if(!gEngfuncs.GetPlayerUniqueID(i+1, playerID))
				continue;
			if(m_BanMgr.GetPlayerBan(playerID))
				banMask |= 1 << i;
			if(m_ServerBannedPlayers[dw*32 + i])
				serverBanMask |= 1 << i;
		}
		if(serverBanMask != banMask)
			bChange = true;
		// Ok, the server needs to be updated.
		char numStr[512];
		sprintf(numStr, " %x", banMask);
		strcat(str, numStr);
	}
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

	unsigned long dw;
	for(dw=0; dw < VOICE_MAX_PLAYERS_DW; dw++)
	{
		m_AudiblePlayers.SetDWord(dw, (unsigned long)READ_LONG());
		m_ServerBannedPlayers.SetDWord(dw, (unsigned long)READ_LONG());

		if(CVAR_GET_FLOAT("voice_clientdebug"))
		{
			char str[256];
			gEngfuncs.pfnConsolePrint("CVoiceStatus::HandleVoiceMaskMsg\n");
			
			sprintf(str, "    - m_AudiblePlayers[%d] = %lu\n", dw, m_AudiblePlayers.GetDWord(dw));
			gEngfuncs.pfnConsolePrint(str);
			
			sprintf(str, "    - m_ServerBannedPlayers[%d] = %lu\n", dw, m_ServerBannedPlayers.GetDWord(dw));
			gEngfuncs.pfnConsolePrint(str);
		}
	}

	m_bServerModEnable = READ_BYTE();
}
void CVoiceStatus::HandleReqStateMsg(int iSize, void *pbuf){
	if(CVAR_GET_FLOAT("voice_clientdebug"))
	{
		gEngfuncs.pfnConsolePrint("CVoiceStatus::HandleReqStateMsg\n");
	}

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

bool CVoiceStatus::IsInSquelchMode(){
	return m_bInSquelchMode;
}

//-----------------------------------------------------------------------------
// Purpose: returns true if the target client has been banned
// Input  : playerID - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CVoiceStatus::IsPlayerBlocked(int iPlayer){
	char playerID[16];
	if (!gEngfuncs.GetPlayerUniqueID(iPlayer, playerID))
		return false;

	return m_BanMgr.GetPlayerBan(playerID);
}

//-----------------------------------------------------------------------------
// Purpose: returns true if the player can't hear the other client due to game rules (eg. the other team)
// Input  : playerID - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CVoiceStatus::IsPlayerAudible(int iPlayer){
	return !!m_AudiblePlayers[iPlayer-1];
}

//-----------------------------------------------------------------------------
// Purpose: blocks/unblocks the target client from being heard
// Input  : playerID - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
void CVoiceStatus::SetPlayerBlockedState(int iPlayer, bool blocked){
	if (CVAR_GET_FLOAT("voice_clientdebug"))
		gEngfuncs.pfnConsolePrint( "CVoiceStatus::SetPlayerBlockedState part 1\n" );
	char playerID[16];
	if (!gEngfuncs.GetPlayerUniqueID(iPlayer, playerID))
		return;
	if (CVAR_GET_FLOAT("voice_clientdebug"))
		gEngfuncs.pfnConsolePrint( "CVoiceStatus::SetPlayerBlockedState part 2\n" );

	// Squelch or (try to) unsquelch this player.
	if (CVAR_GET_FLOAT("voice_clientdebug")){
		char str[256];
		sprintf(str, "CVoiceStatus::SetPlayerBlockedState: setting player %d ban to %d\n", iPlayer, !m_BanMgr.GetPlayerBan(playerID));
		gEngfuncs.pfnConsolePrint(str);
	}
	m_BanMgr.SetPlayerBan( playerID, blocked );
	UpdateServerState(false);
}
