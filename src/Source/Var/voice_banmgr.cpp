//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include <fstream>

#include "Controls.h"
#include "voice_banmgr.h"


#define BANMGR_FILEVERSION	1
char const g_pBanMgrFilename[] = "abcenchance/";

CVoiceBanMgr::CVoiceBanMgr(){
	Clear();
}
bool CVoiceBanMgr::Init(){
	char filename[MAX_PATH];
	const char* result = vgui::filesystem()->GetLocalPath(g_pBanMgrFilename, filename, MAX_PATH);
	if (result != nullptr) {
		Q_snprintf(filename, MAX_PATH, "%s/voice_ban.dt", filename);
		std::ifstream ifs(filename, std::ios::in | std::ios::binary);
		if (ifs.is_open()) {
			while (!ifs.eof()) {
				uint64 steamid = 0;
				ifs.read(reinterpret_cast<char*>(&steamid), sizeof(uint64));
				if(steamid != 0)
					m_aryBannedPlayer.push_back(steamid);
			}
			ifs.close();
		}
		return true;
	}
	return false;
}
void CVoiceBanMgr::Save(){
	char filename[MAX_PATH];
	const char* result = vgui::filesystem()->GetLocalPath(g_pBanMgrFilename, filename, MAX_PATH);
	if (result != nullptr) {
		Q_snprintf(filename, MAX_PATH, "%s/voice_ban.dt", filename);
		std::ofstream ofs(filename, std::ios::out | std::ios::binary);
		if (ofs.is_open()) {
			for (auto iter = m_aryBannedPlayer.begin(); iter != m_aryBannedPlayer.end(); iter++) {
				uint64 steamid = *iter;
				ofs.write(reinterpret_cast<char*>(&steamid), sizeof(uint64));
			}
			ofs.close();
		}
	}
}
bool CVoiceBanMgr::GetPlayerBan(uint64 steamid){
	for (auto iter = m_aryBannedPlayer.begin(); iter != m_aryBannedPlayer.end(); iter++) {
		if (steamid == *iter)
			return true;
	}
	return false;
}
void CVoiceBanMgr::SetPlayerBan(uint64 steamid, bool bSquelch){
	if (bSquelch) {
		if (GetPlayerBan(steamid))
			return;
		m_aryBannedPlayer.push_back(steamid);
	}
	else {
		if (GetPlayerBan(steamid)) {
			for (auto iter = m_aryBannedPlayer.begin(); iter != m_aryBannedPlayer.end(); iter++) {
				if (steamid == *iter) {
					m_aryBannedPlayer.erase(iter);
					return;
				}
			}
		}
	}
}
void CVoiceBanMgr::ForEachBannedPlayer(void (*callback)(uint64 steamid)){
	for (auto iter = m_aryBannedPlayer.begin(); iter != m_aryBannedPlayer.end(); iter++) {
		callback(*iter);
	}
}
void CVoiceBanMgr::Clear(){
	m_aryBannedPlayer.clear();
}