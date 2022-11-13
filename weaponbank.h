#pragma once
#define MAX_WEAPON_SLOT 10
#define INVALID_WEAPON_SLOT MAX_WEAPON_SLOT + 1
#define INVALID_WEAPON_POS -1

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
	size_t GetMaxPos(size_t iSlot);
	size_t GetMinPos(size_t iSlot);
	size_t Size();
	size_t Size(size_t iSlot);
	size_t MaxID();
	CWeaponData();
private:
	//��ǰ���ID
	size_t m_iMaxID;
	//����Id������
	std::map<size_t, WEAPON*> m_dicWeaponIds;
	//����Slot Pos������
	std::map<size_t, std::map<size_t, WEAPON*>> m_dicWeaponSlots;
	//��������������
	std::map<std::string, WEAPON*> m_dicWeaponNames;
};


class WeaponsResource{
private:
	//��ҿ��Ի�ȡ��������������
	CWeaponData m_pWeaponData;
	//��ҳ��е�����
	CWeaponData m_pOwnedWeaponData;
	//��ҳ��е������ӵ�����
	std::map<size_t, int> m_dicAmmos;
	//hud_fastswitch
	cvar_t* pFastSwich = nullptr;

public:
	//1~0ʮ���˵�ѡ�е�Pos
	int m_aryDrawMenu[MAX_WEAPON_SLOT];
	//Ŀǰѡ���Slot
	size_t m_iNowSlot;

	void Init();
	void Reset();

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
	size_t GetFirstPosNumber(size_t iSlot);
	size_t GetLastPosNumber(size_t iSlot);
	size_t GetDrawMenuPos(size_t iSlot);
	void SyncWeapon(const weapon_data_t* wd);

	void SetAmmo(size_t iId, int iCount);
	int CountAmmo(size_t iId);
	bool HasAmmo(WEAPON* p);
	bool IsSelectable(WEAPON* p);
	HSPRITE* GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect);
private:
	//ѡ��ѡ����������˵�
	void SetSelectWeapon(int iId, int iPos, bool bWheel);
	client_sprite_t* WeaponsResource::GetSpriteList(client_sprite_t* pList, const char* psz, int iRes, int iCount);
};
extern WeaponsResource gWR;