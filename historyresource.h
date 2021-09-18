#pragma once
#define MAX_HISTORY 64
enum {
	HISTSLOT_EMPTY,
	HISTSLOT_AMMO,
	HISTSLOT_WEAP,
	HISTSLOT_ITEM,
};

class HistoryResource{
private:
	struct HIST_ITEM {
		int type;
		float DisplayTime;  // the time at which this item should be removed from the history
		int iCount;
		int iId;
	};

	HIST_ITEM rgAmmoHistory[MAX_HISTORY];

public:
	void Init(void);
	void Reset(void);

	int iHistoryGap;
	int iCurrentHistorySlot;

	Color AmmoIconColor;
	Color AmmoTextColor;
	vec2_t vecAmmoPickUpPos;
	int iAmmoDisplayCount;
	Color WeaponPickUpColor;
	Color WeaponPickUpEmptyColor;
	vec2_t vecWeaponPickUpPos;
	Color ItemPickUpColor;
	vec2_t vecItemPickUpPos;
	vgui::HFont hFont;

	void AddToHistory(int iType, int iId, int iCount = 0);
	void AddToHistory(int iType, const char* szName, int iCount = 0);
	void CheckClearHistory(void);
	int DrawAmmoHistory(float flTime);
};

extern HistoryResource gHR;
