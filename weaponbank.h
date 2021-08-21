#pragma once
#define MAX_WEAPONS 255
#define MAX_AMMO 255
#define MAX_WEAPON_POSITIONS 25
#define MAX_WEAPON_SLOTS 10

class WeaponsResource
{
private:
	WEAPON rgWeapons[MAX_WEAPONS];
	WEAPON* rgSlots[MAX_WEAPON_SLOTS + 1][MAX_WEAPON_POSITIONS + 1];
	int		riAmmo[MAX_AMMO];

public:
	void Init(void)
	{
		memset(rgWeapons, 0, sizeof rgWeapons);
		Reset();
	}

	void Reset(void)
	{
		iOldWeaponBits = 0;
		memset(rgSlots, 0, sizeof rgSlots);
		memset(riAmmo, 0, sizeof riAmmo);
	}

	///// WEAPON /////
	int			iOldWeaponBits;

	WEAPON* GetWeapon(int iId) { return &rgWeapons[iId]; }
	void AddWeapon(WEAPON* wp)
	{
		rgWeapons[wp->iId] = *wp;
		LoadWeaponSprites(&rgWeapons[wp->iId]);
	}

	void PickupWeapon(WEAPON* wp)
	{
		rgSlots[wp->iSlot][wp->iSlotPos] = wp;
	}

	void DropWeapon(WEAPON* wp)
	{
		rgSlots[wp->iSlot][wp->iSlotPos] = NULL;
	}

	void DropAllWeapons(void)
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			if (rgWeapons[i].iId)
				DropWeapon(&rgWeapons[i]);
		}
	}

	WEAPON* GetWeaponSlot(int slot, int pos) { return rgSlots[slot][pos]; }

	void LoadWeaponSprites(WEAPON* wp);
	void LoadAllWeaponSprites(void);
	WEAPON* GetFirstPos(int iSlot);
	void SelectSlot(int iSlot, int fAdvance, int iDirection);
	WEAPON* GetNextActivePos(int iSlot, int iSlotPos);

	int HasAmmo(WEAPON* p);

	///// AMMO /////
	AMMO GetAmmo(int iId) { return iId; }

	void SetAmmo(int iId, int iCount) { riAmmo[iId] = iCount; }

	int CountAmmo(int iId);

	HSPRITE* GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect);
};

extern WeaponsResource gWR;