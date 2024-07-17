#pragma once
//========= Copyright ?1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef VOICE_STATUS_H
#define VOICE_STATUS_H
#pragma once

#include <bitset>
#include "voice_banmgr.h"

#define VOICE_MAX_PLAYERS		32

class CVoiceStatus{
	typedef std::bitset<VOICE_MAX_PLAYERS> CPlayerBitVec;
public:
	CVoiceStatus();
	virtual		~CVoiceStatus();
public:
	// Initialize the cl_dll's voice manager.
	virtual void HUD_Init();
	// ackPosition is the bottom position of where CVoiceStatus will draw the voice acknowledgement labels.
public:

	// Call from HUD_Frame each frame.
	void HUD_Frame(double frametime);
	// For save data
	void HUD_Shutdown();
	// Called when a player starts or stops talking.
	// entindex is -1 to represent the local client talking (before the data comes back from the server). 
	// When the server acknowledges that the local client is talking, then entindex will be gEngfuncs.GetLocalPlayer().
	// entindex is -2 to represent the local client's voice being acked by the server.
	void	UpdateSpeakerStatus(int entindex, bool bTalking);
	// Called when the server registers a change to who this client can hear.
	void	HandleVoiceMaskMsg(int iSize, void* pbuf);
	// The server sends this message initially to tell the client to send their state.
	void	HandleReqStateMsg(int iSize, void* pbuf);


	// Squelch mode functions.
public:

	// When you enter squelch mode, pass in 
	void	StartSquelchMode();
	void	StopSquelchMode();
	bool	IsInSquelchMode() const;

	// returns true if the target client has been banned
	// playerIndex is of range 1..maxplayers
	bool	IsPlayerBlocked(int iPlayerIndex);
	// returns false if the player can't hear the other client due to game rules (eg. the other team)
	bool    IsPlayerAudible(int iPlayerIndex) const;
	// blocks the target client from being heard
	void	SetPlayerBlockedState(int iPlayerIndex, bool blocked);

public:
	void	UpdateServerState(bool bForce);
public:
	float			m_LastUpdateServerState;		// Last time we called this function.
	int				m_bServerModEnable;				// What we've sent to the server about our "voice_modenable" cvar.
	CPlayerBitVec	m_VoicePlayers;		// Who is currently talking. Indexed by client index.

	// This is the gamerules-defined list of players that you can hear. It is based on what teams people are on 
	// and is totally separate from the ban list. Indexed by client index.
	CPlayerBitVec	m_AudiblePlayers;

	// Players who have spoken at least once in the game so far
	CPlayerBitVec	m_VoiceEnabledPlayers;

	// This is who the server THINKS we have banned (it can become incorrect when a new player arrives on the server).
	// It is checked periodically, and the server is told to squelch or unsquelch the appropriate players.
	CPlayerBitVec	m_ServerBannedPlayers;

	cl_entity_s		m_VoiceHeadModels[VOICE_MAX_PLAYERS];			// These aren't necessarily in the order of players. They are just
																	// a place for it to put data in during CreateEntities.
	// Squelch mode stuff.
	bool				m_bInSquelchMode;

	bool				m_bTalking;				// Set to true when the client thinks it's talking.
	bool				m_bServerAcked;			// Set to true when the server knows the client is talking.
public:
	CVoiceBanMgr		m_BanMgr;				// Tracks which users we have squelched and don't want to hear.
};


// Get the (global) voice manager. 
CVoiceStatus* GetClientVoiceMgr();
#endif // VOICE_STATUS_H
