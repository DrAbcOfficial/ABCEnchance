#pragma once
typedef struct hud_playerinfo_s {
	int index;
	float frags;
	long death;
	float health;
	float armor;
	byte team;
	char donors;
	char admin;
}hud_playerinfo_t;

enum SC_DONER_ICON {
	DONER_NONE = 0,
	DONER_ELECTRIC_CROWBAR,
	DONER_GOLDED_UZI,
	DONER_GOLED_DOLLAR,
	DONER_TESTER,
	DONER_DEVELOEPR
};

enum SC_ADMIN_ICON {
	ADMIN_NONE = 0,
	ADMIN_OPRATER,
	ADMIN_SERVER_OWNER
};

class CHudPlayerTitle : CHudBase {
public:
	int Init(void);
	int Draw(float flTime);
	void Reset();
	hud_playerinfo_t playerinfo[33];
private:
	int iDeathIconTga;
	int iBackGroundTga;
	int iHealthBarTga;
	int iArmorBarTga;
	int iIconBackGroundTga;
};
extern CHudPlayerTitle m_HudPlayerTitle;