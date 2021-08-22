#pragma once
#define MAX_WEAPON_POSITIONS 50
#define MAX_WEAPON_SLOTS 10
#define MAX_WEAPONS MAX_WEAPON_POSITIONS * MAX_WEAPON_SLOTS
#define MAX_AMMO 255

class WeaponsResource
{
private:
	WEAPON rgWeapons[MAX_WEAPONS];
	int gridSlotPosDataMap[MAX_WEAPON_SLOTS][MAX_WEAPON_POSITIONS];

	int gridSlotMap[MAX_WEAPON_SLOTS][MAX_WEAPON_POSITIONS];
	int	riAmmo[MAX_AMMO];

public:
	int gridDrawMenu[MAX_WEAPON_SLOTS];
	int iNowSlot;
	int iNowPos;
	int iNowSelect;

	void Init(void)
	{
		Reset();
	}
	void Reset(void)
	{
		iNowSlot = 0;
		iNowPos = 0;
		memset(rgWeapons, 0, sizeof rgWeapons);
		memset(riAmmo, 0, sizeof riAmmo);
		memset(gridSlotPosDataMap, -1, sizeof gridSlotPosDataMap);
		memset(gridSlotMap, -1, sizeof gridSlotMap);
		memset(gridDrawMenu, -1, sizeof gridDrawMenu);
	}
	WEAPON* GetWeapon(int iId)
	{ 
		return &rgWeapons[iId];
	}
	void AddWeapon(WEAPON* wp)
	{
		rgWeapons[wp->iId] = *wp;
		gridSlotPosDataMap[wp->iSlot][wp->iSlotPos] = wp->iId;
		LoadWeaponSprites(&rgWeapons[wp->iId]);
	}
	void PickupWeapon(int id)
	{
		WEAPON* wp = &rgWeapons[id];
		gridSlotMap[wp->iSlot][wp->iSlotPos] = id;
	}
	void PickupWeapon(WEAPON* wp)
	{
		gridSlotMap[wp->iSlot][wp->iSlotPos] = wp->iId;
	}
	void DropWeapon(WEAPON* wp)
	{
		gridSlotMap[wp->iSlot][wp->iSlotPos] = -1;
	}
	void DropAllWeapons(void)
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			if (rgWeapons[i].iId)
				DropWeapon(&rgWeapons[i]);
		}
	}
	WEAPON* GetWeaponSlot(int slot, int pos) 
	{ 
		return &rgWeapons[gridSlotMap[slot][pos]];
	}
	int GetWeaponIdBySlot(int slot, int pos)
	{
		return gridSlotPosDataMap[slot][pos];
	}

	void LoadWeaponSprites(WEAPON* wp);
	void WeaponsResource::LoadScriptWeaponSprites(int iId, char* cust);
	void LoadAllWeaponSprites(void);
	void SelectSlot(int iSlot, int fAdvance);
	void FillMenuGrid();

	AMMO GetAmmo(int iId)
	{ 
		return iId; 
	}
	void SetAmmo(int iId, int iCount) 
	{ 
		riAmmo[iId] = iCount; 
	}
	int CountAmmo(int iId);
	int HasAmmo(WEAPON* p);
	HSPRITE* GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect);
};

extern WeaponsResource gWR;