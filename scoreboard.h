#pragma once
enum class EPlayerStatus {
	None = 0,
	Friend = 1 << 1,
	Clan = 1 << 2
};
class CHudCustomScoreboard {
public:
	void GLInit();
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
	void Clear();
private:
	uint8* aryAvatarCache[32];
	int aryFriendStats[32];
	void GetPlayerAvatar(size_t uiIndex);
};
extern CHudCustomScoreboard m_HudCustomScoreboard;