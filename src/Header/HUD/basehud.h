#pragma once
typedef struct {
	float fExpire;
	float fBaseline;
	int	x, y;
} BASEDAMAGE_IMAGE;


class CHudAmmo : public CHudBase {
public:

};
class CHudAmmoSecondary : public CHudBase {
public:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};
	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};

class CHudBattery : public CHudBase{
public:

};

class CHudFlashlight : public CHudBase
{
public:
};

class CHudHealth : public CHudBase{
public:

};