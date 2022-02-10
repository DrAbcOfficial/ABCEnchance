#pragma once
#define MAX_HISTORY 16
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
	enum {
		HISTSLOT_AMMO,
		HISTSLOT_ITEM,
		HISTSLOT_WEAP,
		HISTSLOT_EMPTY
	};
private:
	void SetFreeSlot(int iType, int iId, int iCount);
	float GetAlphaScale(float a, float flTimediffer);
	//0 Ammo
	//1 Item
	//2 Weapon
	HIST_ITEM rgHistory[3][MAX_HISTORY];

	cvar_t* pHudDrawTime;

	Color AmmoIconColor;
	Color AmmoTextColor;
	Color WeaponPickUpColor;
	Color WeaponPickUpEmptyColor;
	Color ItemPickUpColor;

	vec2_t vecAmmoPickUpPos;
	vec2_t vecWeaponPickUpPos;
	vec2_t vecItemPickUpPos;

	vgui::HFont hFont;
};
extern HistoryResource gHR;