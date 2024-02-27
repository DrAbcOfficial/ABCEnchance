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
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Think(void);
	void Reset(void);
	int DrawWList(float flTime);
	int MsgFunc_CurWeapon(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_WeaponList(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_AmmoX(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_AmmoPickup(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_WeapPickup(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ItemPickup(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_HideWeapon(const char* pszName, int iSize, void* pbuf);

	void SlotInput(int iSlot);
	void _cdecl UserCmd_Slot1(void);
	void _cdecl UserCmd_Slot2(void);
	void _cdecl UserCmd_Slot3(void);
	void _cdecl UserCmd_Slot4(void);
	void _cdecl UserCmd_Slot5(void);
	void _cdecl UserCmd_Slot6(void);
	void _cdecl UserCmd_Slot7(void);
	void _cdecl UserCmd_Slot8(void);
	void _cdecl UserCmd_Slot9(void);
	void _cdecl UserCmd_Slot10(void);
	void _cdecl UserCmd_Close(void);
	void _cdecl UserCmd_NextWeapon(void);
	void _cdecl UserCmd_PrevWeapon(void);

	float m_fFade;
	RGBA  m_rgba;
	WEAPON* m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;
};
class CHudAmmoSecondary : public CHudBase {
public:
	int Init(void);
	int VidInit(void);
	void Reset(void);
	int Draw(float flTime);

	int MsgFunc_SecAmmoVal(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_SecAmmoIcon(const char* pszName, int iSize, void* pbuf);

private:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};

	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};

class CHudBattery : public CHudBase{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_Battery(const char* pszName, int iSize, void* pbuf);

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
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);
	int MsgFunc_Flashlight(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_FlashBat(const char* pszName, int iSize, void* pbuf);

private:
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
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float fTime);
	virtual void Reset(void);
	int MsgFunc_Health(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_Damage(const char* pszName, int iSize, void* pbuf);
	int m_iHealth;
	int m_iDMGIconStart;
	int m_HUD_cross;
	float m_fAttackFront, m_fAttackRear, m_fAttackLeft, m_fAttackRight;
	void GetPainColor(int& r, int& g, int& b);
	float m_fFade;
	HSPRITE m_hSprite;
	HSPRITE m_hDamage;
	BASEDAMAGE_IMAGE m_dmg[12];
	int	m_bitsDamage;
	int DrawPain(float fTime);
	int DrawDamage(float fTime);
	void CalcDamageDirection(vec3_t vecFrom);
	void UpdateTiles(float fTime, long bits);
};