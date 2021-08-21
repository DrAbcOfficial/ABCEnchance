#pragma once
#define MAX_HISTORY 64
enum {
	HISTSLOT_EMPTY,
	HISTSLOT_AMMO,
	HISTSLOT_WEAP,
	HISTSLOT_ITEM,
};

class HistoryResource
{
private:
	struct HIST_ITEM {
		int type;
		float DisplayTime;  // the time at which this item should be removed from the history
		int iCount;
		int iId;
	};

	HIST_ITEM rgAmmoHistory[MAX_HISTORY];

public:

	void Init(void)
	{
		AmmoIconColor = pScheme->GetColor("AmmoHUD.AmmoPickUpIconColor", gDefaultColor);
		AmmoTextColor = pScheme->GetColor("AmmoHUD.AmmoPickUpTextColor", gDefaultColor);
		iAmmoDisplayCount = (int)atof(pScheme->GetResourceString("AmmoHUD.AmmoPickUpDisplayCount"));
		vecAmmoPickUpPos[0] = atof(pScheme->GetResourceString("AmmoHUD.AmmoPickUpX"));
		vecAmmoPickUpPos[1] = atof(pScheme->GetResourceString("AmmoHUD.AmmoPickUpY"));

		WeaponPickUpColor = pScheme->GetColor("AmmoHUD.WeaponPickUpColor", gDefaultColor);
		WeaponPickUpEmptyColor = pScheme->GetColor("AmmoHUD.WeaponPickUpEmptyColor", gDefaultColor);
		vecWeaponPickUpPos[0] = atof(pScheme->GetResourceString("AmmoHUD.WeaponPickUpX"));
		vecWeaponPickUpPos[1] = atof(pScheme->GetResourceString("AmmoHUD.WeaponPickUpY"));

		ItemPickUpColor = pScheme->GetColor("AmmoHUD.ItemPickUpColor", gDefaultColor);
		vecItemPickUpPos[0] = atof(pScheme->GetResourceString("AmmoHUD.ItemPickUpX"));
		vecItemPickUpPos[1] = atof(pScheme->GetResourceString("AmmoHUD.ItemPickUpY"));

		hFont = pScheme->GetFont("HUDSmallShitFont", true);
		Reset();
	}

	void Reset(void)
	{
		memset(rgAmmoHistory, 0, sizeof rgAmmoHistory);
	}

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
