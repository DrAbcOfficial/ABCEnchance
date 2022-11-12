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
#include "IWeaponSelect.h"
#include "ammo.h"
#include "CCustomHud.h"
#include "historyresource.h"
#include "weaponbank.h"

WEAPON* CWeaponData::operator [](size_t iId) {
	return this->m_dicWeaponIds[iId];
}
WEAPON* CWeaponData::operator [](std::pair<size_t, size_t> pair) {
	return this->m_dicWeaponSlots[pair.first][pair.second];
}
WEAPON* CWeaponData::operator [](std::string szName) {
	return this->m_dicWeaponNames[szName];
}
//ɾ���������ӣ���������ָ��Ķ����ڴ�
void CWeaponData::Clear() {
	//��������ʮ��std::mapջ����������Ҫ�ֶ�����ÿ�ű�
	for (auto iter = this->m_dicWeaponSlots.begin(); iter != this->m_dicWeaponSlots.end(); iter++) {
		iter->second.clear();
	}
	this->m_dicWeaponSlots.clear();
	this->m_dicWeaponNames.clear();
	//����mapָ��ͬһ����������ֻ��Ҫ��һ��map�ͷ�WEAPON�ڴ漴��
	for (auto iter = this->m_dicWeaponIds.begin(); iter != this->m_dicWeaponIds.end(); iter++) {
		delete iter->second;
		iter->second = nullptr;
	}
	this->m_dicWeaponIds.clear();
}
bool CWeaponData::Has(size_t iSlot, size_t iPos){
	if (!this->m_dicWeaponSlots[iSlot].count(iPos))
		return false;
	auto mapPos = this->m_dicWeaponSlots.find(iSlot)->second;
	return mapPos.find(iPos) != mapPos.end();
}
size_t CWeaponData::Size(){
	return m_dicWeaponIds.size();
}
size_t CWeaponData::Size(size_t iSlot){
	return this->m_dicWeaponSlots[iSlot].size();
}
void CWeaponData::Add(WEAPON* wp) {
	this->m_dicWeaponIds[wp->iId] = wp;
	this->m_dicWeaponNames[wp->szName] = wp;
	this->m_dicWeaponSlots[wp->iSlot][wp->iSlotPos] = wp;
}
void CWeaponData::Remove(WEAPON* wp){
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
size_t CWeaponData::GetMaxPos(size_t iSlot){
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
//ɾ���������ӣ����ǲ������ڴ�
void CWeaponData::RemoveAll() {
	this->m_dicWeaponIds.clear();
	this->m_dicWeaponNames.clear();
	this->m_dicWeaponSlots.clear();
}
CWeaponData::CWeaponData() {
	for (size_t i = 0; i < MAX_WEAPON_SLOT; i++) {
		m_dicWeaponSlots.insert(std::make_pair(i, std::map<size_t, WEAPON*>()));
	}
}

//��ʼ�������ֿ�
void WeaponsResource::Init(void){
	this->pFastSwich = CVAR_GET_POINTER("hud_fastswitch");
	Reset();
}
//���������ֿ�
void WeaponsResource::Reset(void){
	this->m_iNowSlot = INVALID_WEAPON_SLOT;
	//�����Ǹ��ֵ��Ѿ���ȫ�����ˣ�����ֻ��Ҫ������Ӽ���
	this->m_pOwnedWeaponData.RemoveAll();
	this->m_pWeaponData.Clear();

	this->m_dicAmmos.clear();
	memset(this->m_aryDrawMenu, -1, sizeof(this->m_aryDrawMenu));
}
CWeaponData* WeaponsResource::GetOwnedData(){
	return &this->m_pOwnedWeaponData;
}
//������ID��ȡ��������
WEAPON* WeaponsResource::GetWeapon(size_t iId) {
	return this->m_pWeaponData[iId];
}
WEAPON* WeaponsResource::GetWeapon(char* szName) {
	return this->m_pWeaponData[szName];
}
WEAPON* WeaponsResource::GetWeapon(size_t slot, size_t pos) {
	return this->m_pWeaponData[std::make_pair(slot, pos)];
}
//�ӱ�������Ԥ������ͬ���������˵�����
void WeaponsResource::SyncWeapon(const weapon_data_t* wd) {
	for (size_t i = 0; i < this->m_pWeaponData.Size(); i++) {
		WEAPON* weapon = GetWeapon(i);
		if (!weapon)
			continue;
		const weapon_data_t* wp = wd + i;
		if (wp->m_iId <= 0) {
			if (HasWeapon(weapon->iSlot, weapon->iSlotPos))
				DropWeapon(weapon->iSlot, weapon->iSlotPos);
		}
		else {
			if (!HasWeapon(weapon->iSlot, weapon->iSlotPos))
				PickupWeapon(wp->m_iId);
			//ͬ����ϻ����
			WEAPON* pwd = gWR.GetWeapon(wp->m_iId);
			pwd->iClip = pwd->iClip;
			pwd->iClip2 = pwd->iClip2;
		}
	}
}
//����µ��������ݵ������ֿ�
void WeaponsResource::AddWeapon(WEAPON* wp){
	//����ͻ
	while (this->m_pWeaponData.Has(wp->iSlot, wp->iSlotPos)) {
		wp->iSlotPos++;
	}
	this->m_pWeaponData.Add(wp);
	this->LoadWeaponSprites(wp);
}

//����������˵�����
void WeaponsResource::PickupWeapon(size_t id){
	this->m_pOwnedWeaponData.Add(this->GetWeapon(id));
}
//�������˵�ɾ������
void WeaponsResource::DropWeapon(size_t s,size_t p){
	this->m_pOwnedWeaponData.Remove(this->GetWeapon(s, p));
}
//ɾ�����л���
void WeaponsResource::DropAllWeapons(void){
	this->m_pOwnedWeaponData.RemoveAll();
}
//������������ͼ��
void WeaponsResource::LoadAllWeaponSprites(){
	for (auto iter = this->m_pWeaponData.Begin(); iter != this->m_pWeaponData.End(); iter++) {
		LoadWeaponSprites(iter->second);
	}
}
//�˵������Ƿ����������(����Ƿ�ӵ���������)
bool WeaponsResource::HasWeapon(size_t s, size_t p) {
	return this->m_pOwnedWeaponData.Has(s, p);
}
bool WeaponsResource::HasWeapon(WEAPON* wp){
	return this->m_pOwnedWeaponData.Has(wp->iSlot, wp->iSlotPos);
}
//���ѡ������
void WeaponsResource::SetSelectWeapon(int iId, int iPos, bool bWheel) {
	if (pFastSwich->value > 0 && bWheel)
		ServerCmd(GetWeapon(iId)->szName);
	else
		m_aryDrawMenu[m_iNowSlot] = iPos;
}

//��txt��ȡͼ����Ϣ
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
//��������ͼ��
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
		//����Ĭ��Error
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
//�����������ӵ�Spr
HSPRITE* WeaponsResource::GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect){
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

//slot 1234567890�л������ص�
void WeaponsResource::SelectSlot(size_t iSlot, int iAdvance, bool bWheel){
	if (m_HudCustomAmmo.m_bAcceptDeadMessage)
		return;

	if ((pFastSwich->value > 0 && !bWheel) || (pFastSwich->value <= 0))
		m_HudCustomAmmo.m_pNowSelectMenu->m_fFade =
			gEngfuncs.GetClientTime() + m_HudCustomAmmo.m_pNowSelectMenu->SelectHoldTime;

	auto changeSlot = [&] {
		if (iAdvance >= 0) {
			if (this->m_iNowSlot + 1 >= INVALID_WEAPON_SLOT)
				this->m_iNowSlot = 0;
			else
				this->m_iNowSlot += 1;
			return this->GetFirstPos(this->m_iNowSlot);
		}
		else {
			if ((int)this->m_iNowSlot - 1 < 0)
				this->m_iNowSlot = MAX_WEAPON_SLOT;
			else
				this->m_iNowSlot -= 1;
			return this->GetLastPos(this->m_iNowSlot);
		}
	};
	auto getValidWeapon = [&](WEAPON* firstValue) {
		WEAPON* wp = firstValue;
		//��ȡ��ǰ��Pos
		int iNowPos = this->m_aryDrawMenu[this->m_iNowSlot];
		//��ȡӦ��ѡ���Pos
		int iNextPos = iNowPos + iAdvance;
		while (wp == nullptr) {
			//�������������ų��Ի�ȡ����
			if (this->HasWeapon(this->m_iNowSlot, iNextPos)) {
				wp = this->GetWeapon(this->m_iNowSlot, iNextPos);
				//���ɱ�ѡ���ֱ������
				if (!IsSelectable(wp))
					wp = nullptr;
			}
			if (wp != nullptr || iNextPos == iNowPos || this->m_pOwnedWeaponData.Size(this->m_iNowSlot) == 0)
				break;
			else 
				iNextPos += iAdvance >= 0 ? 1 : -1;

			if (iAdvance >= 0) {
				if (iNextPos > this->m_pOwnedWeaponData.GetMaxPos(this->m_iNowSlot)) {
					iNextPos = this->m_pOwnedWeaponData.GetMinPos(this->m_iNowSlot);
					if (bWheel)
						wp = changeSlot();
				}
			}
			else if (iNextPos < (int)this->m_pOwnedWeaponData.GetMinPos(this->m_iNowSlot)) {
				iNextPos = this->m_pOwnedWeaponData.GetMaxPos(this->m_iNowSlot);
				if (bWheel)
					wp = changeSlot();
			}
		}
		return wp;
	};
	iSlot = mathlib::clamp(iSlot, 0, MAX_WEAPON_SLOT-1);
	//����ǵ�ǰslot
	if (iSlot == this->m_iNowSlot) {
		WEAPON* chose = getValidWeapon(nullptr);
		if (chose)
			SetSelectWeapon(chose->iId, chose->iSlotPos, bWheel);
		else if (gCVars.pAmmoMenuStyle->value <= 0) {
			this->m_aryDrawMenu[this->m_iNowSlot] = INVALID_WEAPON_POS;
			return;
		}
	}
	//�������
	else {
		this->m_iNowSlot = iSlot;
		//������ʽ
		if (gCVars.pAmmoMenuStyle->value <= 0) {
			WEAPON* chose = getValidWeapon(gWR.GetFirstPos(this->m_iNowSlot));
			if (chose)
				SetSelectWeapon(chose->iId, chose->iSlotPos, bWheel);
			else{
				this->m_aryDrawMenu[this->m_iNowSlot] = INVALID_WEAPON_POS;
				return;
			}
		}
	}
}
WEAPON* WeaponsResource::GetFirstPos(size_t iSlot){
	size_t iMinPos = this->m_pOwnedWeaponData.GetMinPos(iSlot);
	if (!HasWeapon(iSlot, iMinPos))
		return nullptr;
	WEAPON* wp = this->GetWeapon(iSlot, iMinPos);
	if (HasAmmo(wp))
		return wp;
	return nullptr;
}
WEAPON* WeaponsResource::GetLastPos(size_t iSlot) {
	size_t iMaxPos = this->m_pOwnedWeaponData.GetMaxPos(iSlot);
	if (!HasWeapon(iSlot, iMaxPos))
		return nullptr;
	WEAPON* wp = this->GetWeapon(iSlot, iMaxPos);
	if (HasAmmo(wp))
		return wp;
	return nullptr;
}
size_t WeaponsResource::GetFirstPosNumber(size_t iSlot) {
	return this->m_pOwnedWeaponData.GetMinPos(iSlot);
}
size_t WeaponsResource::GetLastPosNumber(size_t iSlot) {
	return this->m_pOwnedWeaponData.GetMaxPos(iSlot);
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
//�Ƿ�ɱ�ѡ��
bool WeaponsResource::IsSelectable(WEAPON* p) {
	if (HasAmmo(p))
		return true;
	return (p->iFlags & ITEM_FLAG_SELECTONEMPTY) != 0;
}

WeaponsResource gWR;