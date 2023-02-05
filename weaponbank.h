#pragma once
#define MAX_WEAPON_SLOT 10
#define INVALID_WEAPON_SLOT MAX_WEAPON_SLOT + 1
#define INVALID_WEAPON_POS -1

class CWeaponData {
public:
	void Add(WEAPON* wp);
	void Clear();
	bool Has(size_t iSlot, size_t iPos);
	WEAPON* Get(size_t iId);
	WEAPON* Get(size_t iSlot, size_t iPos);
	WEAPON* Get(char* szName);
	size_t MaxID();

	std::vector<WEAPON*> m_aryWeapons;
protected:
	//当前最大ID
	size_t m_iMaxID = 0;
};

class CPlayerWeaponData : public CWeaponData {
public:
	void Remove(size_t s, size_t p);
	void Add(WEAPON* wp);
};


class WeaponsResource{
private:
	//玩家拥有的武器
	CPlayerWeaponData m_pPlayerWeapon;
	//玩家可以获取的所有武器数据
	CWeaponData m_pWeaponData;

	//玩家持有的所有子弹数据
	std::map<size_t, int> m_dicAmmos;
	//hud_fastswitch
	cvar_t* pFastSwich = nullptr;

public:
	//1~0十个菜单选中的Pos
	int m_aryDrawMenu[MAX_WEAPON_SLOT];
	//目前选择的Slot
	int m_iNowSlot;
	//目前选择的Id
	size_t m_iNowChoseId;
	//上次选择的武器ID
	int m_iLastChoseId;

	void Init();
	void Reset();

	CPlayerWeaponData* GetOwnedData();

	WEAPON* GetWeapon(char* szName);
	WEAPON* GetWeapon(size_t iId);
	WEAPON* GetWeapon(size_t slot, size_t pos);

	void AddWeapon(WEAPON* wp);
	void PickupWeapon(size_t id);
	void DropWeapon(size_t s, size_t p);
	void DropAllWeapons();
	
	bool HasWeapon(size_t s, size_t p);
	bool HasWeapon(WEAPON* wp);
	void LoadWeaponSprites(WEAPON* wp, char* cust = nullptr);
	void LoadWeaponSprites(size_t iId, char* cust);
	void LoadAllWeaponSprites();
	void SelectSlot(size_t iSlot, int fAdvance, bool bWheel);
	WEAPON* GetFirstPos(size_t iSlot);
	WEAPON* GetLastPos(size_t iSlot);
	size_t GetDrawMenuPos(size_t iSlot);
	void SyncWeapon(const weapon_data_t* wd);

	void SetAmmo(size_t iId, int iCount);
	int CountAmmo(size_t iId);
	bool HasAmmo(WEAPON* p);
	bool IsSelectable(WEAPON* p);
	HSPRITE* GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect);
private:
	//选择选定的武器或菜单
	void SetSelectWeapon(int iId, int iPos, bool bWheel);
	client_sprite_t* WeaponsResource::GetSpriteList(client_sprite_t* pList, const char* psz, int iRes, int iCount);
};
extern WeaponsResource gWR;