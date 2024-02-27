#pragma once
#include <map>

#define MAX_WEAPON_SLOT 10
#define INVALID_WEAPON_SLOT MAX_WEAPON_SLOT + 1
#define INVALID_WEAPON_POS -1

class WEAPON;
class CWeaponData {
public:
	WEAPON* operator [](size_t iId);
	WEAPON* operator [](std::pair<size_t, size_t>);
	WEAPON* operator [](std::string szName);
	void Add(WEAPON* wp);
	void Remove(WEAPON* wp);
	void RemoveAll();
	void Clear();
	bool Has(size_t iSlot, size_t iPos);
	std::map<size_t, WEAPON*>::iterator Begin();
	std::map<size_t, WEAPON*>::iterator End();
	std::map<size_t, WEAPON*>::iterator PosBegin(size_t iSlot);
	std::map<size_t, WEAPON*>::iterator PosEnd(size_t iSlot);
	std::map<size_t, WEAPON*>::reverse_iterator RPosBegin(size_t iSlot);
	std::map<size_t, WEAPON*>::reverse_iterator RPosEnd(size_t iSlot);
	size_t GetMaxPos(size_t iSlot);
	size_t GetMinPos(size_t iSlot);
	size_t Size();
	size_t Size(size_t iSlot);
	CWeaponData();
private:
	//依照Id索引的
	std::map<size_t, WEAPON*> m_dicWeaponIds;
	//依照Slot Pos索引的
	std::map<size_t, std::map<size_t, WEAPON*>> m_dicWeaponSlots;
	//依照名称索引的
	std::map<std::string, WEAPON*> m_dicWeaponNames;
};


class WeaponsResource {
private:
	//玩家可以获取的所有武器数据
	CWeaponData m_pWeaponData;
	//玩家持有的武器
	CWeaponData m_pOwnedWeaponData;
	//玩家可切换出的武器
	CWeaponData m_pAviliableWeaponData;

	//玩家持有的所有子弹数据
	std::map<size_t, int> m_dicAmmos;
	//hud_fastswitch
	cvar_t* pFastSwich = nullptr;

public:
	WEAPON* m_pCurWeapon;
	bool m_bAcceptDeadMessage = false;

	WEAPON* m_pNowSelected;
	//目前选择的Slot
	size_t m_iNowSlot;

	void Init();
	void Reset();
	void VidInit();

	CWeaponData* GetOwnedData();

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
	void SyncWeapon(const weapon_data_t* wd);

	bool HasUsableWeaponSize();

	void SetAmmo(size_t iId, int iCount);
	int CountAmmo(size_t iId);
	bool HasAmmo(WEAPON* p);
	HSPRITE* GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect);
private:
	//构建AviliableWeapon
	void BuildAviliableWeapons();
	//选择选定的武器或菜单
	void SetSelectWeapon(WEAPON* wp, bool bWheel);
	client_sprite_t* GetSpriteList(client_sprite_t* pList, const char* psz, int iRes, int iCount);
};
extern WeaponsResource gWR;