#pragma once
#define MAX_WEAPON_OLDPOSITIONS 26
#define MAX_WEAPON_OLDSLOTS 10
#define MAX_WEAPON_POSITIONS 30
#define MAX_WEAPON_POSITIONS_USER MAX_WEAPON_POSITIONS + 1
#define MAX_WEAPON_SLOTS 10
#define MAX_WEAPONS MAX_WEAPON_POSITIONS * MAX_WEAPON_SLOTS
#define MAX_AMMO 999

typedef struct gridmenuitem_s{
	int iId = -1;
	int iPos = -1;
}gridmenuitem_t;

class WeaponsResource{
private:
	WEAPON rgWeapons[MAX_WEAPONS];
	int gridSlotPosDataMap[MAX_WEAPON_SLOTS][MAX_WEAPON_POSITIONS];
	int gridSlotMap[MAX_WEAPON_SLOTS][MAX_WEAPON_POSITIONS_USER];
	int	riAmmo[MAX_AMMO];

public:
	gridmenuitem_t gridDrawMenu[MAX_WEAPON_SLOTS];
	int iNowSlot;

	void Init(void);
	void Reset(void);
	int CountGridWeapons();
	int CountWeapons();
	int CountMenuWeapons();
	int GetWeaponId(char* szName);
	WEAPON* GetWeapon(int iId);
	void AddWeapon(WEAPON* wp);
	void PickupWeapon(int id);
	void DropWeapon(WEAPON* wp);
	void DropAllWeapons(void);
	WEAPON* GetWeaponSlot(int slot, int pos);
	int GetWeaponIdBySlot(int slot, int pos);
	int HasWeapon(WEAPON* wp);
	void LoadWeaponSprites(WEAPON* wp);
	void LoadScriptWeaponSprites(int iId, char* cust);
	void LoadAllWeaponSprites(void);
	void SelectSlot(int iSlot, int fAdvance);
	void FillMenuGrid();
	void SetUserSlot(int iSlot, int iId);

	AMMO GetAmmo(int iId);
	void SetAmmo(int iId, int iCount);
	int CountAmmo(int iId);
	int HasAmmo(WEAPON* p);
	HSPRITE* GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect);
};
extern WeaponsResource gWR;