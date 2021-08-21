#pragma once
#define MAX_HISTORY 12
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
		Reset();
	}

	void Reset(void)
	{
		memset(rgAmmoHistory, 0, sizeof rgAmmoHistory);
	}

	int iHistoryGap;
	int iCurrentHistorySlot;

	void AddToHistory(int iType, int iId, int iCount = 0);
	void AddToHistory(int iType, const char* szName, int iCount = 0);

	void CheckClearHistory(void);
	int DrawAmmoHistory(float flTime);
};

extern HistoryResource gHR;
