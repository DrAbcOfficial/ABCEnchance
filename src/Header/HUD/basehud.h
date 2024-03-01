#pragma once
typedef struct {
	float fExpire;
	float fBaseline;
	int	x, y;
} BASEDAMAGE_IMAGE;

#ifndef __AMMO_H__
class WEAPON;
#endif

class CHudAmmo : public CHudBase {
public:
	float m_fFade;
	RGBA  m_rgba;
	WEAPON* m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;
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
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	wrect_t* m_prc1;
	wrect_t* m_prc2;
	int	  m_iBat;
	int	  m_iBatMax;
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};

class CHudFlashlight : public CHudBase
{
public:
	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	HSPRITE m_hBeam;
	wrect_t* m_prc1;
	wrect_t* m_prc2;
	wrect_t* m_prcBeam;
	float m_flBat;
	int	  m_iBat;
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;		// width of the battery innards
};

class CHudHealth : public CHudBase{
public:
	int m_iHealth;
	int m_iDMGIconStart;
	int m_HUD_cross;
	float m_fAttackFront, m_fAttackRear, m_fAttackLeft, m_fAttackRight;
	float m_fFade;
	HSPRITE m_hSprite;
	HSPRITE m_hDamage;
	BASEDAMAGE_IMAGE m_dmg[12];
	int	m_bitsDamage;
};