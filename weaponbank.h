#pragma once
constexpr size_t MAX_WEAPON_OLDPOSITIONS = 26;
constexpr size_t MAX_WEAPON_OLDSLOTS = 10;
constexpr size_t MAX_WEAPON_POSITIONS = 30;
constexpr size_t MAX_WEAPON_POSITIONS_USER = MAX_WEAPON_POSITIONS + 1;

constexpr size_t MAX_WEAPON_POS_INDEX = MAX_WEAPON_POSITIONS - 1;
constexpr size_t MAX_WEAPON_POSUSER_INDEX = MAX_WEAPON_POSITIONS_USER - 1;

constexpr size_t MAX_WEAPON_SLOTS = 10;
constexpr size_t MAX_WEAPON_SLOT_INDEX = MAX_WEAPON_SLOTS - 1;
constexpr size_t MAX_WEAPONS = MAX_WEAPON_POSITIONS * MAX_WEAPON_SLOTS;
constexpr size_t MAX_AMMO = 999;

typedef struct gridmenuitem_s{
	int iId = -1;
	int iPos = -1;
}gridmenuitem_t;

class WeaponsResource{
private:
	//同步到的所有武器数据
	WEAPON rgWeapons[MAX_WEAPONS];
	//所有武器依照Slot Pos填充的ID列表
	int gridSlotMap[MAX_WEAPON_SLOTS][MAX_WEAPON_POSITIONS_USER];
	//玩家持有的武器Slot Pos填充的ID列表
	int gridSlotPosDataMap[MAX_WEAPON_SLOTS][MAX_WEAPON_POSITIONS];
	//玩家持有的所有子弹数据
	int	riAmmo[MAX_AMMO];

public:
	//等待绘制的图标
	gridmenuitem_t gridDrawMenu[MAX_WEAPON_SLOTS];
	//目前选择的Slot
	int iNowSlot;

	void Init();
	void Reset();
	size_t CountGridWeapons();
	size_t CountWeapons();
	size_t CountMenuWeapons();
	int GetWeaponId(char* szName);
	WEAPON* GetWeapon(int iId);
	void AddWeapon(WEAPON* wp);
	void PickupWeapon(int id);
	void DropWeapon(int s, int p);
	void DropAllWeapons(void);
	WEAPON* GetWeaponSlot(int slot, int pos);
	int GetWeaponIdBySlot(int slot, int pos);
	int HasWeapon(int s, int p);
	int HasWeapon(WEAPON* wp);
	void LoadWeaponSprites(WEAPON* wp);
	void LoadScriptWeaponSprites(int iId, char* cust);
	void LoadAllWeaponSprites(void);
	void SelectSlot(int iSlot, int fAdvance, bool bJump);
	void SetUserSlot(int iSlot, int iId);
	WEAPON* GetFirstPos(int iSlot);
	WEAPON* GetLastPos(int iSlot);
	gridmenuitem_t* GetDrawMenuItem(size_t iSlot);
	void FillDrawMenuGrid();

	AMMO GetAmmo(int iId);
	void SetAmmo(int iId, int iCount);
	int CountAmmo(int iId);
	int HasAmmo(WEAPON* p);
	HSPRITE* GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect);
};
extern WeaponsResource gWR;