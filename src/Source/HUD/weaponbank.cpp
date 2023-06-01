#include <metahook.h>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include "mymathlib.h"
#include "cvardef.h"
#include "vguilocal.h"
#include "local.h"
#include "hud.h"
#include "weaponinfo_sven.h"
#include "weapon.h"
#include "glew.h"
#include "gl_def.h"
#include "IWeaponSelect.h"
#include "ammo.h"
#include "CCustomHud.h"
#include "historyresource.h"
#include "weaponbank.h"

void CPlayerWeaponData::Remove(size_t s, size_t p){
	for (size_t i = 0; i < m_aryWeapons.size(); i++) {
		auto iter = m_aryWeapons[i];
		if (iter->iSlot == s && iter->iSlotPos == p) {
			m_aryWeapons.erase(m_aryWeapons.begin() + i);
			break;
		}
	}
}

void CPlayerWeaponData::Add(WEAPON* wp) {
	m_aryWeapons.push_back(wp);
	std::sort(m_aryWeapons.begin(), m_aryWeapons.end(),[&](WEAPON* a, WEAPON* b) {
		return a->iSlot == b->iSlot ? a->iSlotPos < b->iSlotPos : a->iSlot < b->iSlot;
	});
	if (wp->iId > m_iMaxID)
		m_iMaxID = wp->iId;
}

//删除所有链接，并且清理指向的对象内存
void CWeaponData::Clear() {
	for (WEAPON* iter : m_aryWeapons) {
		delete iter;
	}
	m_aryWeapons.clear();
}
bool CWeaponData::Has(size_t iSlot, size_t iPos){
	for (WEAPON* iter : m_aryWeapons) {
		if (iter->iSlot == iSlot && iter->iSlotPos == iPos)
			return true;
	}
	return false;
}
size_t CWeaponData::MaxID(){
	return this->m_iMaxID;
}
void CWeaponData::Add(WEAPON* wp) {
	m_aryWeapons.push_back(wp);
	if (wp->iId > m_iMaxID)
		m_iMaxID = wp->iId;
}
WEAPON* CWeaponData::Get(size_t iId) {
	for (WEAPON* iter : m_aryWeapons) {
		if (iter->iId == iId)
			return iter;
	}
	return nullptr;
}
WEAPON* CWeaponData::Get(size_t iSlot, size_t iPos) {
	for (WEAPON* iter : m_aryWeapons) {
		if (iter->iSlot == iSlot && iter->iSlotPos == iPos)
			return iter;
	}
	return nullptr;
}
WEAPON* CWeaponData::Get(char* szName) {
	for (WEAPON* iter : m_aryWeapons) {
		if (!strcmp(szName, iter->szName))
			return iter;
	}
	return nullptr;
}

//初始化武器仓库
void WeaponsResource::Init(void){
	//this->pFastSwich = CVAR_GET_POINTER("hud_fastswitch");
	Reset();
}
//重置武器仓库
void WeaponsResource::Reset(void){
	this->m_iLastChoseId = -1;
	this->m_iNowSlot = INVALID_WEAPON_SLOT;
	this->m_pWeaponData.Clear();
	this->m_pPlayerWeapon.Clear();
	this->m_dicAmmos.clear();
	memset(this->m_aryDrawMenu, -1, sizeof(this->m_aryDrawMenu));
}

CPlayerWeaponData* WeaponsResource::GetOwnedData(){
	return &m_pPlayerWeapon;
}

//由武器ID获取武器数据
WEAPON* WeaponsResource::GetWeapon(size_t iId) {
	return this->m_pWeaponData.Get(iId);
}
WEAPON* WeaponsResource::GetWeapon(char* szName) {
	return this->m_pWeaponData.Get(szName);
}
WEAPON* WeaponsResource::GetWeapon(size_t slot, size_t pos) {
	return this->m_pWeaponData.Get(slot, pos);
}
//从本地武器预测数组同步武器到菜单缓存
void WeaponsResource::SyncWeapon(const weapon_data_t* _wd) {
	const weapon_sven_t* wd = (weapon_sven_t*)_wd;
	for (size_t i = 0; i <= this->m_pWeaponData.MaxID(); i++) {
		WEAPON* weapon = GetWeapon(i);
		if (!weapon)
			continue;
		const weapon_sven_t* wp = wd + i;
		if (wp->m_iId <= 0) {
			if (HasWeapon(weapon->iSlot, weapon->iSlotPos))
				DropWeapon(weapon->iSlot, weapon->iSlotPos);
		}
		else {
			if (!HasWeapon(weapon->iSlot, weapon->iSlotPos))
				PickupWeapon(wp->m_iId);
			//同步弹匣数据
			WEAPON* pwd = gWR.GetWeapon(wp->m_iId);
			pwd->iClip = wp->m_iClip;
			//pwd->iClip2 = wp->m_iClip2;
		}
	}
}
//添加新的武器数据到武器仓库
void WeaponsResource::AddWeapon(WEAPON* wp){
	//防冲突
	while (this->m_pWeaponData.Has(wp->iSlot, wp->iSlotPos)) {
		wp->iSlotPos++;
	}
	this->m_pWeaponData.Add(wp);
	this->LoadWeaponSprites(wp);
}

//添加武器到菜单缓存
void WeaponsResource::PickupWeapon(size_t id){
	this->m_pPlayerWeapon.Add(GetWeapon(id));
}
//从武器菜单删除缓存
void WeaponsResource::DropWeapon(size_t s,size_t p){
	this->m_pPlayerWeapon.Remove(s, p);
}
//删除所有缓存
void WeaponsResource::DropAllWeapons(void){
	this->m_pPlayerWeapon.Clear();
}
//载入所有武器图标
void WeaponsResource::LoadAllWeaponSprites(){
	for(auto iter: m_pWeaponData.m_aryWeapons){
		LoadWeaponSprites(iter);
	}
}
//菜单缓存是否有这把武器(玩家是否拥有这把武器)
bool WeaponsResource::HasWeapon(size_t s, size_t p) {
	return this->m_pPlayerWeapon.Has(s, p);
}
bool WeaponsResource::HasWeapon(WEAPON* wp){
	return this->HasWeapon(wp->iSlot, wp->iSlotPos);
}
//玩家选择武器
void WeaponsResource::SetSelectWeapon(int iId, int iPos, bool bWheel) {
	//if (pFastSwich->value > 0 && bWheel)
	//	ServerCmd(GetWeapon(iId)->szName);
	//else
		m_aryDrawMenu[m_iNowSlot] = iPos;
	m_iNowChoseId = iId;
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
//载入武器图标
void WeaponsResource::LoadWeaponSprites(WEAPON* pWeapon, char* cust){
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
	if(cust == nullptr)
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
		int iRes = ScreenWidth < 640 ? 320 : 640;
		client_sprite_t* p = this->GetSpriteList(pList, szType, iRes, i);
		if (p != nullptr) {
			sprintf_s(sz, "sprites/%s.spr", p->szSprite);
			*spr = SPR_Load(sz);
			*rc = p->rc;
		}
		else if(dspr != nullptr && drc != nullptr) {
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
void WeaponsResource::LoadWeaponSprites(size_t iId, char* cust){
	this->LoadWeaponSprites(this->GetWeapon(iId), cust);
}
//从武器载入子弹Spr
HSPRITE* WeaponsResource::GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect){
	for (auto iter : m_pWeaponData.m_aryWeapons) {
		if (!iter)
			continue;
		if (iter->iAmmoType == iAmmoId) {
			rect = iter->rcAmmo;
			return &iter->hAmmo;
		}
		else if (iter->iAmmo2Type == iAmmoId) {
			rect = iter->rcAmmo2;
			return &iter->hAmmo2;
		}
	}
	return nullptr;
}

//slot 1234567890切换武器回调
void WeaponsResource::SelectSlot(size_t iSlot, int iAdvance, bool bWheel){
	if (m_HudCustomAmmo.m_bAcceptDeadMessage)
		return;
	//if ((pFastSwich->value > 0 && !bWheel) || (pFastSwich->value <= 0))
		m_HudCustomAmmo.m_pNowSelectMenu->m_fFade =
			gEngfuncs.GetClientTime() + m_HudCustomAmmo.m_pNowSelectMenu->SelectHoldTime;
	if (m_pPlayerWeapon.m_aryWeapons.size() == 0)
		return;
	if (iAdvance == 0)
		iAdvance = 1;
	iSlot = mathlib::clamp(iSlot, 0, MAX_WEAPON_SLOT-1);
	size_t index = 0;
	if (m_iLastChoseId >= 0) {
		for (index = 0; m_pPlayerWeapon.m_aryWeapons.size(); index++) {
			if (m_iLastChoseId == m_pPlayerWeapon.m_aryWeapons[index]->iId)
				break;
		}
	}
	int step = iAdvance > 0 ? 1 : -1;
	for (int i = index + step; i != index; i += step) {
		if (i >= m_pPlayerWeapon.m_aryWeapons.size())
			i = 0;
		else if (i <= 0)
			i = m_pPlayerWeapon.m_aryWeapons.size() - 1;
		WEAPON* wp = m_pPlayerWeapon.m_aryWeapons[i];
		if (HasAmmo(wp)) {
			SetSelectWeapon(wp->iId, wp->iSlotPos, bWheel);
			break;
		}
	}
}
WEAPON* WeaponsResource::GetFirstPos(size_t iSlot){
	for (auto iter : this->m_pPlayerWeapon.m_aryWeapons) {
		if (iter->iSlot == iSlot && HasAmmo(iter))
			return iter;
	}
	return nullptr;
}
WEAPON* WeaponsResource::GetLastPos(size_t iSlot) {
	for (size_t i = this->m_pPlayerWeapon.m_aryWeapons.size() - 1; i > 0; i--) {
		auto iter = this->m_pPlayerWeapon.m_aryWeapons[i];
		if (iter->iSlot == iSlot && HasAmmo(iter))
			return iter;
	}
	return nullptr;
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
size_t WeaponsResource::GetDrawMenuPos(size_t iSlot) {
	return this->m_aryDrawMenu[iSlot];
}
int WeaponsResource::CountAmmo(size_t iId){
	if (iId < 0)
		return 0;
	return this->m_dicAmmos[iId];
}
void WeaponsResource::SetAmmo(size_t iId, int iCount){
	this->m_dicAmmos[iId] = iCount;
}
//是否可被选择？
bool WeaponsResource::IsSelectable(WEAPON* p) {
	if (HasAmmo(p))
		return true;
	return (p->iFlags & ITEM_FLAG_SELECTONEMPTY) != 0;
}

WeaponsResource gWR;