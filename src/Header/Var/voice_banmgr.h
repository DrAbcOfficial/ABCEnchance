#pragma once
//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef VOICE_BANMGR_H
#define VOICE_BANMGR_H
#ifdef _WIN32
#pragma once
#endif

#include <vector>

// This class manages the (persistent) list of squelched players.
class CVoiceBanMgr{
public:
	CVoiceBanMgr();
	// Init loads the list of squelched players from disk.
	bool Init();
	// Saves the state into voice_squelch.dt.
	void Save();

	bool GetPlayerBan(uint64 steamid64);
	void SetPlayerBan(uint64 steamid64, bool bSquelch);

	// Call your callback for each banned player.
	void ForEachBannedPlayer(void (*callback)(uint64 steamid64));
protected:
	void				Clear();
	std::vector<uint64> m_aryBannedPlayer;
};


#endif // VOICE_BANMGR_H
