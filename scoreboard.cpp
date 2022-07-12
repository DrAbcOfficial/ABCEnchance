#include <metahook.h>

#include <SteamSDK/steamclientpublic.h>
#include <SteamSDK/isteamutils.h>
#include <SteamSDK/isteamfriends.h>
#include <scoreboard.h>

void CHudCustomScoreboard::GLInit(){
}

int CHudCustomScoreboard::Init(void){
	return 0;
}

int CHudCustomScoreboard::Draw(float flTime){
	return 0;
}

void CHudCustomScoreboard::Reset(void){
}

void CHudCustomScoreboard::Clear(){
}

void CHudCustomScoreboard::GetPlayerAvatar(size_t uiIndex){
	char szSteamID[16];
	if (gEngfuncs.GetPlayerUniqueID(uiIndex, szSteamID)) {
		CSteamID pId = CSteamID(szSteamID, k_EUniversePublic);
		ISteamFriends* iSteamFriend = SteamFriends();
		int hImage = iSteamFriend->GetSmallFriendAvatar(pId);
		SteamUtils()->GetImageRGBA(hImage, aryAvatarCache[uiIndex], 32 * 32);

		if (iSteamFriend->HasFriend(pId, EFriendFlags::k_EFriendFlagImmediate))
			aryFriendStats[uiIndex] |= (int)EPlayerStatus::Friend;
		if (iSteamFriend->HasFriend(pId, EFriendFlags::k_EFriendFlagClanMember))
			aryFriendStats[uiIndex] |= (int)EPlayerStatus::Clan;
		delete &pId;
	}
	
}
