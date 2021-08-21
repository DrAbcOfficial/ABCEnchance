class CHudAmmo : public CHudBase
{
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
class CHudAmmoSecondary : public CHudBase
{
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