
#include <metahook.h>
#include <string>
#include <algorithm>

#pragma region hlsdk
#include "cdll_int.h"
#pragma endregion

#pragma region TODO: remove these mess
#include "utility/util.h"
#include "utility/vgui_util.h"
#pragma endregion

#include "hud/Viewport.h"
#include "core/events/networkmessage.h"
#include "core/events/command.h"
#include "core/events/hudevents.h"

#include "core/module/autofunc.h"

#include "weaponresource.h"


#undef max
#undef min

// CWeaponData 实现
Weapon* WeaponList::operator[](size_t iId) const {
    auto it = m_dicWeaponIds.find(iId);
    return it != m_dicWeaponIds.end() ? it->second : nullptr;
}

Weapon* WeaponList::operator[](std::pair<size_t, size_t> slotPos) const {
    const auto& slot = m_slots[slotPos.first];
    auto it = slot.positions.find(slotPos.second);
    return it != slot.positions.end() ? it->second : nullptr;
}

Weapon* WeaponList::operator[](std::string_view szName) const {
    auto it = m_dicWeaponNames.find(std::string(szName));
    return it != m_dicWeaponNames.end() ? it->second : nullptr;
}

void WeaponList::Clear() {
    for (auto& [id, wp] : m_dicWeaponIds) {
        delete wp;
    }
    m_dicWeaponIds.clear();
    m_dicWeaponNames.clear();
    for (auto& slot : m_slots) {
        slot.positions.clear();
        slot.min_pos = slot.max_pos = 0;
    }
}

bool WeaponList::Has(size_t iSlot, size_t iPos) const {
    return iSlot < MAX_WEAPON_SLOT &&
        m_slots[iSlot].positions.count(iPos) > 0;
}

void WeaponList::Add(Weapon* wp) {
    if (!wp) 
        return;

    // 避免重复添加
    if (m_dicWeaponIds.count(wp->iId)) 
        return;

    m_dicWeaponIds[wp->iId] = wp;
    m_dicWeaponNames[wp->szName] = wp;

    auto& slot = m_slots[wp->iSlot];
    // 处理位置冲突
    while (slot.positions.count(wp->iSlotPos)) {
        wp->iSlotPos++;
    }
    slot.positions[wp->iSlotPos] = wp;
    slot.updateMinMax();
}

void WeaponList::Remove(Weapon* wp) {
    if (!wp) 
        return;

    m_dicWeaponIds.erase(wp->iId);
    m_dicWeaponNames.erase(wp->szName);

    auto& slot = m_slots[wp->iSlot];
    slot.positions.erase(wp->iSlotPos);
    slot.updateMinMax();
}

void WeaponList::RemoveAll() {
    m_dicWeaponIds.clear();
    m_dicWeaponNames.clear();
    for (auto& slot : m_slots) {
        slot.positions.clear();
        slot.min_pos = slot.max_pos = 0;
    }
}

WeaponList::WeaponList() = default;

void WeaponsResource::Init() {
    g_EventAmmoX.append([&](int index, int count) {
        this->SetAmmo(index, count);
    });
    g_EventWeaponList.append([&](Weapon* recived) {
        Weapon* wp = gWR.GetWeapon(recived->iId);
        if (!wp) {
            Weapon* new_weapon = new Weapon();
            memcpy(new_weapon, recived, sizeof(Weapon));
            new_weapon->iClip = 0;
            this->AddWeapon(new_weapon);
        }
        else
            this->UpdateWeapon(recived);
    });
    g_EventCustWeapon.append([&](int id, const char* name) {
        this->LoadWeaponSprites(id, name);
    });
    g_EventCurWeapon.append([&](int state, int id, int clip, int clip2) {
        if (state > 0) {
            if (id == -1) {
                this->DropAllWeapons();
                return;
            }
            Weapon* pWeapon = this->GetWeapon(id);
            this->m_pCurWeapon = pWeapon;
            if (pWeapon) {
                pWeapon->iClip = clip;
                pWeapon->iClip2 = clip2;
                pWeapon->iState = state;
            }
        }
        else {
            switch (id) {
            case -1:
            case 0:
                this->DropAllWeapons();
                break;
            }
        }
    });
    g_EventWeaponSpr.append([&](int id, const char* name) {
        Weapon* wp = this->GetWeapon(id);
        if (wp && wp->iId > 0) {
            strncpy_s(wp->szSprName, sizeof(wp->szSprName), name, strlen(name));
            this->LoadWeaponSprites(wp);
        }
    });
    g_EventCmdSlot.append([&](int slot) {
        this->SelectSlot(slot, 1, false);
        return true;
    });
    g_EventCmdNextWeapon.append([&]() {
        this->SelectSlot(this->m_iNowSlot, 1, true);
        return true;
    });
    g_EventCmdPrevWeapon.append([&]() {
        this->SelectSlot(this->m_iNowSlot, -1, true);
        return true;
    });
    g_EventHudTxferPredictionData.append([&](struct entity_state_s*, const struct entity_state_s*, struct clientdata_s*, const struct clientdata_s*, struct weapon_data_s*, const struct weapon_data_s* pwd) {
        auto wp = WeaponData::FromWeaponData(pwd);
        this->SyncWeapon(wp);
    });
}

void WeaponsResource::Reset() {
    m_iNowSlot = INVALID_WEAPON_SLOT;
    m_pWeaponData.Clear();
    m_pOwnedWeaponData.RemoveAll();
    m_pAviliableWeaponData.RemoveAll();
    m_dicAmmos.clear();
    m_pNowSelected = nullptr;
    m_pCurWeapon = nullptr;
}

void WeaponsResource::VidInit() {
    LoadAllWeaponSprites();
}

void WeaponsResource::SyncWeapon(const WeaponData* wd) {
    for (auto it = m_pWeaponData.Begin(); it != m_pWeaponData.End(); ++it) {
        auto* weapon = it->second;
        if (!weapon) continue;
        const auto* wp = wd + weapon->iId;
        if (wp->m_iId <= 0) {
            if (HasWeapon(weapon->iSlot, weapon->iSlotPos)) {
                DropWeapon(weapon->iSlot, weapon->iSlotPos);
            }
        }
        else {
            if (!HasWeapon(weapon->iSlot, weapon->iSlotPos)) {
                PickupWeapon(wp->m_iId);
            }
            weapon->iClip = wp->m_iClip;
            if (wp->m_iClip2 > 0) {
                weapon->iClip2 = wp->m_iClip2;
            }
        }
    }
}

void WeaponsResource::AddWeapon(Weapon* wp) {
    if (!wp) return;
    m_pWeaponData.Add(wp);
    LoadWeaponSprites(wp);
}

extern void Viewport_PickupWeapon(Weapon* wp);
extern void Viewport_DropWeapon(Weapon* wp);
extern void Viewport_ChooseWeapon(Weapon* wp);
extern void Viewport_DropAllWeapon();

void WeaponsResource::PickupWeapon(size_t id) {
    if (auto* wp = GetWeapon(id)) {
        m_pOwnedWeaponData.Add(wp);
        AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_ADDWEAPON, wp->szName);
        Viewport_PickupWeapon(wp);
    }
}

void WeaponsResource::DropWeapon(size_t s, size_t p) {
    if (auto* wp = GetWeapon(s, p)) {
        m_pOwnedWeaponData.Remove(wp);
        AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_DROPWEAPON, wp->szName);
        Viewport_DropWeapon(wp);
    }
}

void WeaponsResource::DropAllWeapons() {
    m_pOwnedWeaponData.RemoveAll();
    Viewport_DropAllWeapon();
}

void WeaponsResource::LoadAllWeaponSprites() {
    for (auto it = m_pWeaponData.Begin(); it != m_pWeaponData.End(); ++it) {
        LoadWeaponSprites(it->second);
    }
}

void WeaponsResource::SetSelectWeapon(Weapon* wp, bool bWheel) {
    if (!wp) 
        return;
    if (CVAR_GET_FLOAT("hud_fastswitch") > 0) {
        ServerCmd(wp->szName);
    }
    Viewport_ChooseWeapon(wp);
    m_pNowSelected = wp;
}

void WeaponsResource::BuildAviliableWeapons() {
    m_pAviliableWeaponData.RemoveAll();
    for (auto it = m_pOwnedWeaponData.Begin(); it != m_pOwnedWeaponData.End(); ++it) {
        auto* wp = it->second;
        if (HasAmmo(wp) || (wp->iFlags & ITEM_FLAG_SELECTONEMPTY)) {
            m_pAviliableWeaponData.Add(wp);
        }
    }
}


static client_sprite_t* GetSpriteList(client_sprite_t* pList, const char* psz, int iRes, int iCount) {
    if (!pList) 
        return nullptr;

    for (int i = 0; i < iCount; ++i, ++pList) {
        if (!strcmp(psz, pList->szName) && pList->iRes == iRes) {
            return pList;
        }
    }
    return nullptr;
}

void WeaponsResource::LoadWeaponSprites(Weapon* pWeapon, const char* cust) {
    if (!pWeapon) 
        return;

    // 重置 sprite 数据
    memset(&pWeapon->rcActive, 0, sizeof(wrect_t));
    memset(&pWeapon->rcInactive, 0, sizeof(wrect_t));
    memset(&pWeapon->rcAmmo, 0, sizeof(wrect_t));
    memset(&pWeapon->rcAmmo2, 0, sizeof(wrect_t));
    pWeapon->hInactive = 0;
    pWeapon->hActive = 0;
    pWeapon->hAmmo = 0;
    pWeapon->hAmmo2 = 0;

    char sz[256];
    int i;
    if (cust) {
        sprintf_s(sz, "sprites/%s/%s.txt", cust, pWeapon->szSprName);
    }
    else {
        sprintf_s(sz, "sprites/%s.txt", pWeapon->szSprName);
    }

    auto* pList = SPR_GetList(sz, &i);
    if (!pList) {
        pList = SPR_GetList("abcenchance/spr/weapon_error.txt", &i);
        if (!pList) return;
    }

    auto loadSprite = [&](const char* szType, HSPRITE* spr, wrect_t* rc,
        HSPRITE* dspr = nullptr, wrect_t* drc = nullptr) {
            const int iRes = ScreenWidth() < 640 ? 320 : 640;
            if (auto* p = GetSpriteList(pList, szType, iRes, i)) {
                sprintf_s(sz, "sprites/%s.spr", p->szSprite);
                *spr = SPR_Load(sz);
                *rc = p->rc;
            }
            else if (dspr && drc) {
                *spr = *dspr;
                *rc = *drc;
            }
            else {
                *spr = 0;
            }
        };

    loadSprite("crosshair", &pWeapon->hCrosshair, &pWeapon->rcCrosshair);
    loadSprite("autoaim", &pWeapon->hAutoaim, &pWeapon->rcAutoaim);
    loadSprite("zoom", &pWeapon->hZoomedCrosshair, &pWeapon->rcZoomedCrosshair,
        &pWeapon->hCrosshair, &pWeapon->rcCrosshair);
    loadSprite("zoom_autoaim", &pWeapon->hZoomedAutoaim, &pWeapon->rcZoomedAutoaim,
        &pWeapon->hZoomedCrosshair, &pWeapon->rcZoomedCrosshair);
    loadSprite("weapon", &pWeapon->hInactive, &pWeapon->rcInactive);
    loadSprite("weapon_s", &pWeapon->hActive, &pWeapon->rcActive);
    loadSprite("ammo", &pWeapon->hAmmo, &pWeapon->rcAmmo);
    loadSprite("ammo2", &pWeapon->hAmmo2, &pWeapon->rcAmmo2,
        &pWeapon->hAmmo, &pWeapon->rcAmmo);
}

HSPRITE* WeaponsResource::GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect) {
    for (auto it = m_pWeaponData.Begin(); it != m_pWeaponData.End(); ++it) {
        auto* wp = it->second;
        if (!wp) continue;

        if (wp->iAmmoType == iAmmoId) {
            rect = wp->rcAmmo;
            return &wp->hAmmo;
        }
        else if (wp->iAmmo2Type == iAmmoId) {
            rect = wp->rcAmmo2;
            return &wp->hAmmo2;
        }
    }
    return nullptr;
}

size_t WeaponsResource::getMinSlot() const {
    size_t slot = MAX_WEAPON_SLOT;
    for (auto it = m_pAviliableWeaponData.Begin(); it != m_pAviliableWeaponData.End(); ++it) {
        slot = std::min(slot, it->second->iSlot);
    }
    return slot;
}

size_t WeaponsResource::getMaxSlot() const {
    size_t slot = 0;
    for (auto it = m_pAviliableWeaponData.Begin(); it != m_pAviliableWeaponData.End(); ++it) {
        slot = std::max(slot, it->second->iSlot);
    }
    return slot;
}

size_t WeaponsResource::getNextPos(Weapon* wp) const {
    if (!wp) return 0;

    const auto& slot = m_pAviliableWeaponData.PosBegin(wp->iSlot);
    const auto& end = m_pAviliableWeaponData.PosEnd(wp->iSlot);

    for (auto it = slot; it != end; ++it) {
        if (it->second->iSlotPos > wp->iSlotPos) {
            return it->second->iSlotPos;
        }
    }
    return m_pAviliableWeaponData.GetMinPos(wp->iSlot);
}

size_t WeaponsResource::getLastPos(Weapon* wp) const {
    if (!wp) return 0;

    const auto& rbegin = m_pAviliableWeaponData.RPosBegin(wp->iSlot);
    const auto& rend = m_pAviliableWeaponData.RPosEnd(wp->iSlot);

    for (auto it = rbegin; it != rend; ++it) {
        if (it->second->iSlotPos < wp->iSlotPos) {
            return it->second->iSlotPos;
        }
    }
    return m_pAviliableWeaponData.GetMaxPos(wp->iSlot);
}

void WeaponsResource::SelectSlot(size_t iSlot, int iAdvance, bool bWheel) {
    if (GetBaseViewPort()->SelectTextMenuItem(iSlot + 1) || !GetBaseViewPort()->HasSuit()) {
        return;
    }

    BuildAviliableWeapons();
    if (m_pAviliableWeaponData.Size() == 0) return;

    iSlot = std::clamp<size_t>(iSlot, 0, MAX_WEAPON_SLOT - 1);

    if (bWheel && !m_pNowSelected) {
        auto it = m_pAviliableWeaponData.Begin();
        m_pNowSelected = it != m_pAviliableWeaponData.End() ? it->second : nullptr;
        if (!m_pNowSelected) return;
    }

    // 处理滚轮切换逻辑
    if (bWheel) {
        if (iAdvance > 0) {
            if (m_pAviliableWeaponData.GetMaxPos(iSlot) == m_pNowSelected->iSlotPos) {
                do {
                    iSlot = (iSlot >= getMaxSlot()) ? getMinSlot() : iSlot + 1;
                } while (m_pAviliableWeaponData.Size(iSlot) == 0);
            }
        }
        else {
            if (m_pAviliableWeaponData.GetMinPos(iSlot) == m_pNowSelected->iSlotPos) {
                do {
                    iSlot = (iSlot <= getMinSlot()) ? getMaxSlot() : iSlot - 1;
                } while (m_pAviliableWeaponData.Size(iSlot) == 0);
            }
        }
    }

    // 选择武器
    if (iSlot == m_iNowSlot && m_pNowSelected) {
        auto* chose = GetWeapon(iSlot, iAdvance > 0 ? getNextPos(m_pNowSelected) : getLastPos(m_pNowSelected));
        if (chose) SetSelectWeapon(chose, bWheel);
    }
    else {
        m_iNowSlot = iSlot;
        m_pNowSelected = (iAdvance > 0) ? GetFirstPos(iSlot) : GetLastPos(iSlot);
        if (m_pNowSelected) SetSelectWeapon(m_pNowSelected, bWheel);
    }
}

bool WeaponsResource::HasUsableWeaponSize() {
    for (auto it = m_pOwnedWeaponData.Begin(); it != m_pOwnedWeaponData.End(); ++it) {
        auto* wp = it->second;
        if (HasAmmo(wp) || (wp->iFlags & ITEM_FLAG_SELECTONEMPTY)) {
            return true;
        }
    }
    return false;
}

bool WeaponsResource::HasAmmo(Weapon* p) {
    if (!p) return false;

    // 无需弹药的武器
    if (p->iMax1 == -1 && p->iMax2 == -1) return true;

    if (p->iAmmoType > -1) {
        bool hasAmmo = (p->iClip > 0) || CountAmmo(p->iAmmoType) > 0;
        if (p->iAmmo2Type > -1 && !hasAmmo) {
            return (p->iClip2 > 0) || CountAmmo(p->iAmmo2Type) > 0;
        }
        return hasAmmo;
    }
    return true;
}


bool WeaponsResource::UpdateWeapon(const Weapon* newWeaponData) {
    if (!newWeaponData) 
        return false;

    Weapon* existingWeapon = GetWeapon(newWeaponData->iId);
    if (!existingWeapon) 
        return false;

    size_t originalSlot = existingWeapon->iSlot;
    size_t originalPos = existingWeapon->iSlotPos;

    *existingWeapon = *newWeaponData;

    m_pWeaponData.Remove(existingWeapon);
    if (m_pOwnedWeaponData.Has(originalSlot, originalPos)) {
        m_pOwnedWeaponData.Remove(existingWeapon);
    }
    if (m_pAviliableWeaponData.Has(originalSlot, originalPos)) {
        m_pAviliableWeaponData.Remove(existingWeapon);
    }

    m_pWeaponData.Add(existingWeapon);
    if (HasWeapon(originalSlot, originalPos)) {
        m_pOwnedWeaponData.Add(existingWeapon);
    }

    LoadWeaponSprites(existingWeapon);
    BuildAviliableWeapons();

    return true;
}

WeaponsResource gWR;