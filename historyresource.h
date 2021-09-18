#pragma once
#define MAX_HISTORY 64
enum {
	HISTSLOT_EMPTY,
	HISTSLOT_AMMO,
	HISTSLOT_WEAP,
	HISTSLOT_ITEM,
};
struct HIST_ITEM {
	int type;
	float DisplayTime;  // the time at which this item should be removed from the history
	int iCount;
	int iId;
};
class HistoryResource{
public:
	void Init(void);
	void Reset(void);
	void AddToHistory(int iType, int iId, int iCount = 0);
	void AddToHistory(int iType, const char* szName, int iCount = 0);
	int DrawAmmoHistory(float flTime);

	int iHistoryGap;
private:
	HIST_ITEM* GetFreeSlot();
	void SetFreeSlot(int iType, int iId, int iCount);
	void CheckClearHistory(void);
	
	HIST_ITEM rgAmmoHistory[MAX_HISTORY];
	int iCurrentHistorySlot;

	cvar_t* pHudDrawTime;

	Color AmmoIconColor;
	Color AmmoTextColor;
	Color WeaponPickUpColor;
	Color WeaponPickUpEmptyColor;
	Color ItemPickUpColor;

	vec2_t vecAmmoPickUpPos;
	vec2_t vecWeaponPickUpPos;
	vec2_t vecItemPickUpPos;

	int AmmoPickUpDisplayCount;
	vgui::HFont hFont;
};

extern HistoryResource gHR;
