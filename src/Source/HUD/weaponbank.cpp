#include <metahook.h>
#include <cmath>
#include <string>
#include <map>
#include <vector>

#include "mymathlib.h"
#include "cvardef.h"
#include "vguilocal.h"
#include "local.h"
#include "hud.h"
#include "weapon.h"
#include "glew.h"
#include "gl_def.h"
#include "weaponselect.h"
#include "ammo.h"
#include "CCustomHud.h"
#include "historyresource.h"
#include "weaponbank.h"
#include <weaponinfo_sven.h>

WEAPON* CWeaponData::operator [](size_t iId) {
	return this->m_dicWeaponIds.find(iId) != this->m_dicWeaponIds.end() ? this->m_dicWeaponIds[iId] : nullptr;
}
WEAPON* CWeaponData::operator [](std::pair<size_t, size_t> pair) {
	auto iter = this->m_dicWeaponSlots.find(pair.first);
	if (iter != this->m_dicWeaponSlots.end() && iter->second.find(pair.second) != iter->second.end())
		return iter->second[pair.second];
	return nullptr;
}
WEAPON* CWeaponData::operator [](std::string szName) {
	return this->m_dicWeaponNames.find(szName) != this->m_dicWeaponNames.end() ? this->m_dicWeaponNames[szName] : nullptr;
}
//删除所有链接，并且清理指向的对象内存
void CWeaponData::Clear() {
	//这个表包含十个std::map栈对象，所以需要手动清理每张表
	for (auto iter = this->m_dicWeaponSlots.begin(); iter != this->m_dicWeaponSlots.end(); iter++) {
		iter->second.clear();
	}
	this->m_dicWeaponSlots.clear();
	this->m_dicWeaponNames.clear();
	//所有map指向同一个对象，所以只需要第一张map释放WEAPON内存即可
	for (auto iter = this->m_dicWeaponIds.begin(); iter != this->m_dicWeaponIds.end(); iter++) {
		delete iter->second;
		iter->second = nullptr;
	}
	this->m_dicWeaponIds.clear();
}
bool CWeaponData::Has(size_t iSlot, size_t iPos) {
	if (!this->m_dicWeaponSlots[iSlot].count(iPos))
		return false;
	auto mapPos = this->m_dicWeaponSlots.find(iSlot)->second;
	return mapPos.find(iPos) != mapPos.end();
}
size_t CWeaponData::Size() {
	return m_dicWeaponIds.size();
}
size_t CWeaponData::Size(size_t iSlot) {
	return this->m_dicWeaponSlots[iSlot].size();
}
void CWeaponData::Add(WEAPON* wp) {
	this->m_dicWeaponIds.emplace(std::make_pair(wp->iId, wp));
	this->m_dicWeaponNames.emplace(std::make_pair(wp->szName, wp));
	this->m_dicWeaponSlots[wp->iSlot].emplace(std::make_pair(wp->iSlotPos, wp));
}
void CWeaponData::Remove(WEAPON* wp) {
	this->m_dicWeaponIds.erase(wp->iId);
	this->m_dicWeaponNames.erase(wp->szName);
	this->m_dicWeaponSlots[wp->iSlot].erase(wp->iSlotPos);
}
std::map<size_t, WEAPON*>::iterator CWeaponData::Begin() {
	return this->m_dicWeaponIds.begin();
}
std::map<size_t, WEAPON*>::iterator CWeaponData::End() {
	return this->m_dicWeaponIds.end();
}
std::map<size_t, WEAPON*>::iterator CWeaponData::PosBegin(size_t iSlot) {
	return this->m_dicWeaponSlots[iSlot].begin();
}
std::map<size_t, WEAPON*>::iterator CWeaponData::PosEnd(size_t iSlot) {
	return this->m_dicWeaponSlots[iSlot].end();
}
std::map<size_t, WEAPON*>::reverse_iterator CWeaponData::RPosBegin(size_t iSlot) {
	return this->m_dicWeaponSlots[iSlot].rbegin();
}
std::map<size_t, WEAPON*>::reverse_iterator CWeaponData::RPosEnd(size_t iSlot) {
	return this->m_dicWeaponSlots[iSlot].rend();
}
size_t CWeaponData::GetMaxPos(size_t iSlot) {
	if (this->m_dicWeaponSlots[iSlot].size() <= 0)
		return 0;
	size_t maxPos = 0;
	for (auto iter = this->m_dicWeaponSlots[iSlot].begin(); iter != this->m_dicWeaponSlots[iSlot].end(); iter++) {
		if (maxPos < iter->second->iSlotPos)
			maxPos = iter->second->iSlotPos;
	}
	return maxPos;
}
size_t CWeaponData::GetMinPos(size_t iSlot) {
	if (this->m_dicWeaponSlots[iSlot].size() <= 0)
		return 0;
	size_t minPos = this->m_dicWeaponSlots[iSlot].begin()->second->iSlotPos;
	for (auto iter = this->m_dicWeaponSlots[iSlot].begin(); iter != this->m_dicWeaponSlots[iSlot].end(); iter++) {
		if (minPos > iter->second->iSlotPos)
			minPos = iter->second->iSlotPos;
	}
	return minPos;
}
//删除所有链接，但是不清理内存
void CWeaponData::RemoveAll() {
	this->m_dicWeaponIds.clear();
	this->m_dicWeaponNames.clear();
	this->m_dicWeaponSlots.clear();
}
CWeaponData::CWeaponData() {
	for (size_t i = 0; i < MAX_WEAPON_SLOT; i++) {
		m_dicWeaponSlots.emplace(std::make_pair(i, std::map<size_t, WEAPON*>()));
	}
}

//初始化武器仓库
void WeaponsResource::Init(void) {
	//this->pFastSwich = CVAR_GET_POINTER("hud_fastswitch");
}
//重置武器仓库
void WeaponsResource::Reset(void) {
	this->m_iNowSlot = INVALID_WEAPON_SLOT;
	this->m_pWeaponData.Clear();
	//上面那个兄弟已经完全清理了，所以只需要解除链接即可
	this->m_pOwnedWeaponData.RemoveAll();
	this->m_pAviliableWeaponData.RemoveAll();
	this->m_dicAmmos.clear();
	this->m_iNowSelected = nullptr;
}
CWeaponData* WeaponsResource::GetOwnedData() {
	return &this->m_pOwnedWeaponData;
}
//由武器ID获取武器数据
WEAPON* WeaponsResource::GetWeapon(size_t iId) {
	return this->m_pWeaponData[iId];
}
WEAPON* WeaponsResource::GetWeapon(char* szName) {
	return this->m_pWeaponData[szName];
}
WEAPON* WeaponsResource::GetWeapon(size_t slot, size_t pos) {
	return this->m_pWeaponData[std::make_pair(slot, pos)];
}
//从本地武器预测数组同步武器到菜单缓存
void WeaponsResource::SyncWeapon(const weapon_data_t* wd) {
	for (auto iter = this->m_pWeaponData.Begin(); iter != this->m_pWeaponData.End(); iter++) {
		WEAPON* weapon = iter->second;
		if (!weapon)
			continue;
		const weapon_sven_t* wp = (const weapon_sven_t*)wd + weapon->iId;
		if (wp->m_iId <= 0) {
			if (HasWeapon(weapon->iSlot, weapon->iSlotPos))
				DropWeapon(weapon->iSlot, weapon->iSlotPos);
		}
		else {
			if (!HasWeapon(weapon->iSlot, weapon->iSlotPos))
				PickupWeapon(wp->m_iId);
			//同步弹匣数据
			weapon->iClip = wp->m_iClip;
			if(wp->m_iClip2 > 0)
				weapon->iClip2 = wp->m_iClip2;
		}
	}
}
//添加新的武器数据到武器仓库
void WeaponsResource::AddWeapon(WEAPON* wp) {
	//防冲突
	while (this->m_pWeaponData.Has(wp->iSlot, wp->iSlotPos)) {
		wp->iSlotPos++;
	}
	this->m_pWeaponData.Add(wp);
	this->LoadWeaponSprites(wp);
}

//添加武器到菜单缓存
void WeaponsResource::PickupWeapon(size_t id) {
	this->m_pOwnedWeaponData.Add(this->GetWeapon(id));
}
//从武器菜单删除缓存
void WeaponsResource::DropWeapon(size_t s, size_t p) {
	this->m_pOwnedWeaponData.Remove(this->GetWeapon(s, p));
}
//删除所有缓存
void WeaponsResource::DropAllWeapons(void) {
	this->m_pOwnedWeaponData.RemoveAll();
}
//载入所有武器图标
void WeaponsResource::LoadAllWeaponSprites() {
	for (auto iter = this->m_pWeaponData.Begin(); iter != this->m_pWeaponData.End(); iter++) {
		LoadWeaponSprites(iter->second);
	}
}
//菜单缓存是否有这把武器(玩家是否拥有这把武器)
bool WeaponsResource::HasWeapon(size_t s, size_t p) {
	return this->m_pOwnedWeaponData.Has(s, p);
}
bool WeaponsResource::HasWeapon(WEAPON* wp) {
	return this->m_pOwnedWeaponData.Has(wp->iSlot, wp->iSlotPos);
}
//玩家选择武器
void WeaponsResource::SetSelectWeapon(WEAPON* wp, bool bWheel) {
	//if (pFastSwich->value > 0 && bWheel)
	//	ServerCmd(wp->szName);
	//else
		m_iNowSelected = wp;
}

//由txt获取图标信息
client_sprite_t* WeaponsResource::GetSpriteList(client_sprite_t* pList, const char* psz, int iRes, int iCount) {
	if (!pList)
		return nullptr;
	int i = iCount;
	client_sprite_t* p = pList;
	while (i--) {
		if ((!strcmp(psz, p->szName)) && (p->iRes == iRes))
			return p;
		p++;
	}
	return nullptr;
}
void WeaponsResource::BuildAviliableWeapons(){
	m_pAviliableWeaponData.RemoveAll();
	for (auto iter = m_pOwnedWeaponData.Begin(); iter != m_pOwnedWeaponData.End(); iter++) {
		if (HasAmmo(iter->second) || iter->second->iFlags & ITEM_FLAG_SELECTONEMPTY)
			m_pAviliableWeaponData.Add(iter->second);
	}
}
//载入武器图标
void WeaponsResource::LoadWeaponSprites(WEAPON* pWeapon, char* cust) {
	if (!pWeapon)
		return;
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
	if (cust == nullptr)
		sprintf_s(sz, "sprites/%s.txt", pWeapon->szSprName);
	else
		sprintf_s(sz, "sprites/%s/%s.txt", cust, pWeapon->szSprName);
	client_sprite_t* pList = SPR_GetList(sz, &i);
	if (!pList) {
		//载入默认Error
		pList = SPR_GetList("abcenchance/spr/weapon_error.txt", &i);
		if (!pList)
			return;
	}
	auto fSetupSprInfo = [&](const char* szType, HSPRITE* spr, wrect_t* rc, HSPRITE* dspr = nullptr, wrect_t* drc = nullptr) {
		int iRes = ScreenWidth() < 640 ? 320 : 640;
		client_sprite_t* p = this->GetSpriteList(pList, szType, iRes, i);
		if (p != nullptr) {
			sprintf_s(sz, "sprites/%s.spr", p->szSprite);
			*spr = SPR_Load(sz);
			*rc = p->rc;
		}
		else if (dspr != nullptr && drc != nullptr) {
			*spr = *dspr;
			*rc = *drc;
		}
		else
			*spr = 0;
	};
	fSetupSprInfo("crosshair", &pWeapon->hCrosshair, &pWeapon->rcCrosshair);
	fSetupSprInfo("autoaim", &pWeapon->hAutoaim, &pWeapon->rcAutoaim);
	fSetupSprInfo("zoom", &pWeapon->hZoomedCrosshair, &pWeapon->rcZoomedCrosshair, &pWeapon->hCrosshair, &pWeapon->rcCrosshair);
	fSetupSprInfo("zoom_autoaim", &pWeapon->hZoomedAutoaim, &pWeapon->rcZoomedAutoaim, &pWeapon->hZoomedCrosshair, &pWeapon->rcZoomedCrosshair);
	fSetupSprInfo("weapon", &pWeapon->hInactive, &pWeapon->rcInactive);
	fSetupSprInfo("weapon_s", &pWeapon->hActive, &pWeapon->rcActive);
	fSetupSprInfo("ammo", &pWeapon->hAmmo, &pWeapon->rcAmmo);
	fSetupSprInfo("ammo2", &pWeapon->hAmmo2, &pWeapon->rcAmmo2, &pWeapon->hAmmo, &pWeapon->rcAmmo);
}
void WeaponsResource::LoadWeaponSprites(size_t iId, char* cust) {
	this->LoadWeaponSprites(this->GetWeapon(iId), cust);
}
//从武器载入子弹Spr
HSPRITE* WeaponsResource::GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect) {
	for (auto iter = this->m_pWeaponData.Begin(); iter != this->m_pWeaponData.End(); iter++) {
		if (!iter->second)
			continue;
		if (iter->second->iAmmoType == iAmmoId) {
			rect = iter->second->rcAmmo;
			return &iter->second->hAmmo;
		}
		else if (iter->second->iAmmo2Type == iAmmoId) {
			rect = iter->second->rcAmmo2;
			return &iter->second->hAmmo2;
		}
	}
	return nullptr;
}

//slot 1234567890切换武器回调
void WeaponsResource::SelectSlot(size_t iSlot, int iAdvance, bool bWheel) {
	if (m_HudCustomAmmo.m_bAcceptDeadMessage)
		return;
	m_HudWMenuSlot.m_fFade = gEngfuncs.GetClientTime() + m_HudWMenuSlot.SelectHoldTime;

	BuildAviliableWeapons();

	if (m_pAviliableWeaponData.Size() == 0)
		return;
	static auto getMinSlot = [&] {
		size_t slot = (*m_pAviliableWeaponData.Begin()).second->iSlot;
		for (auto iter = m_pAviliableWeaponData.Begin(); iter != m_pAviliableWeaponData.End(); iter++) {
			WEAPON* compare = (*iter).second;
			if (compare->iSlot < slot)
				slot = compare->iSlot;
		}
		return slot;
	};
	static auto getMaxSlot = [&] {
		size_t slot = (*m_pAviliableWeaponData.Begin()).second->iSlot;
		for (auto iter = m_pAviliableWeaponData.Begin(); iter != m_pAviliableWeaponData.End(); iter++) {
			WEAPON* compare = (*iter).second;
			if (compare->iSlot > slot)
				slot = compare->iSlot;
		}
		return slot;
	};
	if (!m_iNowSelected) {
		m_iNowSelected = GetFirstPos(getMinSlot());
		SetSelectWeapon(m_iNowSelected, bWheel);
		return;
	}
	iSlot = mathlib::clamp<size_t>(iSlot, 0, MAX_WEAPON_SLOT - 1);
	static auto getNext = [&](WEAPON* wp) {
		size_t pos = wp->iSlotPos;
		for (auto iter = m_pAviliableWeaponData.PosBegin(wp->iSlot); iter != m_pAviliableWeaponData.PosEnd(wp->iSlot); iter++) {
			WEAPON* compare = (*iter).second;
			if (compare->iSlotPos > pos) {
				pos = compare->iSlotPos;
				break;
			}	
		}
		if (pos == wp->iSlotPos) 
			return m_pAviliableWeaponData.GetMinPos(wp->iSlot);
		return pos;
	};
	static auto getLast = [&](WEAPON* wp) {
		size_t pos = wp->iSlotPos;
		for (auto iter = m_pAviliableWeaponData.RPosBegin(wp->iSlot); iter != m_pAviliableWeaponData.RPosEnd(wp->iSlot); iter++) {
			WEAPON* compare = (*iter).second;
			if (compare->iSlotPos < pos) {
				pos = compare->iSlotPos;
				break;
			}
		}
		if (pos == wp->iSlotPos)
			return m_pAviliableWeaponData.GetMaxPos(wp->iSlot);
		return pos;
	};
	if (bWheel) {
		//如果是当前slot
		if (iAdvance > 0) {
			if (m_pAviliableWeaponData.GetMaxPos(iSlot) == m_iNowSelected->iSlotPos) {
				do {
					iSlot++;
					if (iSlot > getMaxSlot())
						iSlot = getMinSlot();
				} while (m_pAviliableWeaponData.Size(iSlot) == 0);
			}
		}
		else if (m_pAviliableWeaponData.GetMinPos(iSlot) == m_iNowSelected->iSlotPos) {
			do {
				if (((int)iSlot) - 1 < (int)getMinSlot())
					iSlot = getMaxSlot();
				else
					iSlot--;
			} while (m_pAviliableWeaponData.Size(iSlot) == 0);
		}
	}

	if (iSlot == m_iNowSlot) {
		WEAPON* chose = GetWeapon(iSlot, iAdvance > 0 ? getNext(m_iNowSelected) : getLast(m_iNowSelected));
		if (chose)
			SetSelectWeapon(chose, bWheel);
	}
	//如果不是
	else {
		m_iNowSlot = iSlot;
		m_iNowSelected = iAdvance > 0 ? GetFirstPos(iSlot) : GetLastPos(iSlot);
		SetSelectWeapon(m_iNowSelected, bWheel);
	}
}
WEAPON* WeaponsResource::GetFirstPos(size_t iSlot) {
	return GetWeapon(iSlot, m_pAviliableWeaponData.GetMinPos(iSlot));
}
WEAPON* WeaponsResource::GetLastPos(size_t iSlot) {
	return GetWeapon(iSlot, this->m_pAviliableWeaponData.GetMaxPos(iSlot));
}
bool WeaponsResource::HasUsableWeaponSize(){
	for (auto iter = m_pOwnedWeaponData.Begin(); iter != m_pOwnedWeaponData.End(); iter++) {
		if (HasAmmo(iter->second) || iter->second->iFlags & ITEM_FLAG_SELECTONEMPTY)
			return true;
	}
	return false;
}
bool WeaponsResource::HasAmmo(WEAPON* p) {
	if (!p)
		return false;
	if (p->iMax1 == -1 && p->iMax2 == -1)
		return true;
	if (p->iAmmoType > -1) {
		bool bFlag = (p->iClip > 0) || CountAmmo(p->iAmmoType);
		if (p->iAmmo2Type > -1 && !bFlag)
			return CountAmmo(p->iAmmo2Type) > 0 || (p->iClip2 > 0);
		else
			return bFlag;
	}
	return true;
}
int WeaponsResource::CountAmmo(size_t iId) {
	if (iId < 0)
		return 0;
	return this->m_dicAmmos[iId];
}
void WeaponsResource::SetAmmo(size_t iId, int iCount) {
	this->m_dicAmmos[iId] = iCount;
}

WeaponsResource gWR;