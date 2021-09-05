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
	hud_playerinfo_t m_Playerinfo[33];
private:
	GLint iDeathIconTga;
	GLint iCrouchIconTga;
	GLint iMedkitIconTga;
	GLint iBackGroundTga;
	GLint iHealthBarTga;
	GLint iArmorBarTga;
};
extern CHudPlayerTitle m_HudPlayerTitle;