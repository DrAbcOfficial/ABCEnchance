#pragma once
#ifndef __WEAPON_RESOURCE_H__
#define __WEAPON_RESOURCE_H__

#include <map>
#include <unordered_map>
#include <string>

constexpr auto MAX_WEAPON_NAME = 128;
constexpr size_t MAX_WEAPON_SLOT = 10;
constexpr size_t INVALID_WEAPON_SLOT = MAX_WEAPON_SLOT + 1;
constexpr int INVALID_WEAPON_POS = -1;
constexpr auto WEAPON_IS_ONTARGET = 0x40;

enum ITEM_FLAGS {
	ITEM_FLAG_NONE = 0,
    ITEM_FLAG_SELECTONEMPTY = 1 << 0,
    ITEM_FLAG_NOAUTORELOAD = 1 << 1,
    ITEM_FLAG_NOAUTOSWITCHEMPTY = 1 << 2,
    ITEM_FLAG_LIMITINWORLD = 1 << 3,
    ITEM_FLAG_EXHAUSTIBLE = 1 << 4 // A player can totally exhaust their ammo supply and lose this weapon
};

typedef int HSPRITE;
typedef int AMMO;

#ifndef WRECTH
#include "wrect.h"
#endif // !WRECTH

class WeaponData
{
public:
    int			m_iId;
    int			m_iClip;

    float		m_flNextPrimaryAttack;
    float		m_flNextSecondaryAttack;
    float		m_flTimeWeaponIdle;

    int			m_fInReload;
    int			m_fInSpecialReload;
    float		m_flNextReload;
    float		m_flPumpTime;
    float		m_fReloadTime;

    float		m_fAimedDamage;
    float		m_fNextAimBonus;
    int			m_fInZoom;
    int			m_iWeaponState;

    int			m_iAmmoType;
    int			m_iAmmoLeft;
    int			m_iAmmoLeft2;
    int			m_iClip2;
    float		fuser1;
    float		fuser2;
    float		fuser3;
    float		fuser4;

    static const WeaponData* FromWeaponData(const struct weapon_data_s* data) {
        return reinterpret_cast<const WeaponData*>(data);
    }
};

class Weapon {
public:
    char	szName[MAX_WEAPON_NAME];
    char	szSprName[MAX_WEAPON_NAME]; //256
    int		iAmmoType;
    int		iAmmo2Type;
    int		iMax1;
    int		iMax2;
    size_t	iSlot;
    size_t	iSlotPos;
    int		iFlags;
    size_t	iId = 0;
    int		iClip;
    int		iClip2; //416

    int		iCount; //420

    int iState;//424

    HSPRITE hActive;//428
    wrect_t rcActive;//432
    HSPRITE hInactive;//436
    wrect_t rcInactive;//440
    HSPRITE	hAmmo;
    wrect_t rcAmmo;
    HSPRITE hAmmo2;
    wrect_t rcAmmo2;
    HSPRITE hCrosshair;
    wrect_t rcCrosshair;
    HSPRITE hAutoaim;
    wrect_t rcAutoaim;
    HSPRITE hZoomedCrosshair;
    wrect_t rcZoomedCrosshair;
    HSPRITE hZoomedAutoaim;
    wrect_t rcZoomedAutoaim;
};

struct WeaponSlotData {
    std::map<size_t, Weapon*> positions;
    size_t min_pos = 0;
    size_t max_pos = 0;

    void updateMinMax() {
        if (positions.empty()) {
            min_pos = max_pos = 0;
            return;
        }
        min_pos = positions.begin()->first;
        max_pos = positions.rbegin()->first;
    }
};

class WeaponList {
public:
    Weapon* operator[](size_t iId) const;
    Weapon* operator[](std::pair<size_t, size_t> slotPos) const;
    Weapon* operator[](std::string_view szName) const;

    void Add(Weapon* wp);
    void Remove(Weapon* wp);
    void RemoveAll();
    void Clear();
    bool Has(size_t iSlot, size_t iPos) const;

    auto Begin() const { return m_dicWeaponIds.begin(); }
    auto End() const { return m_dicWeaponIds.end(); }
    auto PosBegin(size_t iSlot) const { return m_slots[iSlot].positions.begin(); }
    auto PosEnd(size_t iSlot) const { return m_slots[iSlot].positions.end(); }
    auto RPosBegin(size_t iSlot) const { return m_slots[iSlot].positions.rbegin(); }
    auto RPosEnd(size_t iSlot) const { return m_slots[iSlot].positions.rend(); }

    size_t GetMaxPos(size_t iSlot) const { return m_slots[iSlot].max_pos; }
    size_t GetMinPos(size_t iSlot) const { return m_slots[iSlot].min_pos; }
    size_t Size() const { return m_dicWeaponIds.size(); }
    size_t Size(size_t iSlot) const { return m_slots[iSlot].positions.size(); }

    WeaponList();
    WeaponList(const WeaponList&) = delete;
    WeaponList& operator=(const WeaponList&) = delete;

private:
    std::unordered_map<size_t, Weapon*> m_dicWeaponIds;
    std::unordered_map<std::string, Weapon*> m_dicWeaponNames;
    WeaponSlotData m_slots[MAX_WEAPON_SLOT]{};
};

class WeaponsResource {
private:
    WeaponList m_pWeaponData;
    WeaponList m_pOwnedWeaponData;
    WeaponList m_pAviliableWeaponData;
    std::unordered_map<size_t, int> m_dicAmmos;

public:
    Weapon* m_pNowSelected = nullptr;
    Weapon* m_pCurWeapon = nullptr;
    size_t m_iNowSlot = INVALID_WEAPON_SLOT;

    void Init();
    void Reset();
    void VidInit();
    WeaponList* GetOwnedData() { return &m_pOwnedWeaponData; }

    Weapon* GetWeapon(char* szName) { return m_pWeaponData[szName]; }
    Weapon* GetWeapon(size_t iId) { return m_pWeaponData[iId]; }
    Weapon* GetWeapon(size_t slot, size_t pos) { return m_pWeaponData[{slot, pos}]; }

    void AddWeapon(Weapon* wp);
    void PickupWeapon(size_t id);
    void DropWeapon(size_t s, size_t p);
    void DropAllWeapons();

    bool HasWeapon(size_t s, size_t p) { return m_pOwnedWeaponData.Has(s, p); }
    bool HasWeapon(Weapon* wp) { return wp && m_pOwnedWeaponData.Has(wp->iSlot, wp->iSlotPos); }

    void LoadWeaponSprites(Weapon* wp, const char* cust = nullptr);
    void LoadWeaponSprites(size_t iId, const char* cust) { LoadWeaponSprites(GetWeapon(iId), cust); }
    void LoadAllWeaponSprites();

    void SelectSlot(size_t iSlot, int fAdvance, bool bWheel);
    Weapon* GetFirstPos(size_t iSlot) { return GetWeapon(iSlot, m_pAviliableWeaponData.GetMinPos(iSlot)); }
    Weapon* GetLastPos(size_t iSlot) { return GetWeapon(iSlot, m_pAviliableWeaponData.GetMaxPos(iSlot)); }

    void SyncWeapon(const WeaponData* wd);
    bool HasUsableWeaponSize();

    void SetAmmo(size_t iId, int iCount) { m_dicAmmos[iId] = iCount; }
    int CountAmmo(size_t iId) { return iId < 0 ? 0 : m_dicAmmos[iId]; }
    bool HasAmmo(Weapon* p);
    HSPRITE* GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect);

    // 更新指定ID的武器信息
    bool UpdateWeapon(const Weapon* newWeaponData);

private:
    void BuildAviliableWeapons();
    void SetSelectWeapon(Weapon* wp, bool bWheel);

    size_t getMinSlot() const;
    size_t getMaxSlot() const;
    size_t getNextPos(Weapon* wp) const;
    size_t getLastPos(Weapon* wp) const;
};

extern WeaponsResource gWR;

#endif