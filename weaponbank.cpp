#include <metahook.h>
#include <cmath>
#include "mathlib.h"
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

client_sprite_t* GetSpriteList(client_sprite_t* pList, const char* psz, int iRes, int iCount){
	if (!pList)
		return nullptr;

	int i = iCount;
	client_sprite_t* p = pList;

	while (i--){
		if ((!strcmp(psz, p->szName)) && (p->iRes == iRes))
			return p;
		p++;
	}

	return nullptr;
}

void WeaponsResource::Init(void){
	Reset();
}
void WeaponsResource::Reset(void){
	iNowSlot = -1;
	memset(rgWeapons, 0, sizeof rgWeapons);
	memset(riAmmo, 0, sizeof riAmmo);
	memset(gridSlotPosDataMap, -1, sizeof gridSlotPosDataMap);
	memset(gridSlotMap, -1, sizeof gridSlotMap);
	memset(gridDrawMenu, -1, sizeof gridDrawMenu);
}
size_t WeaponsResource::CountGridWeapons() {
	size_t i, j;
	size_t c = 0;
	for (i = 0; i < MAX_WEAPON_SLOTS; i++){
		for (j = 0; j < MAX_WEAPON_POSITIONS; j++){
			if (gridSlotMap[i][j] >= 0)
				c++;
		}
	}
	return c;
}
size_t WeaponsResource::CountWeapons() {
	size_t i, c = 0;
	for (i = 0; i < MAX_WEAPONS; i++){
		if (GetWeapon(i)->iId > 0)
			c++;
	}
	return c;
}
size_t WeaponsResource::CountMenuWeapons() {
	size_t i, c = 0;
	for (i = 0; i < MAX_WEAPON_SLOTS; i++){
		if (gridDrawMenu[i].iId > 0)
			c++;
	}
	return c;
}
int WeaponsResource::GetWeaponId(char* szName){
	for (const WEAPON& var : rgWeapons) {
		if (!var.iId)
			continue;
		if (strcmp(var.szName, szName) == 0) {
			return var.iId;
		}
	}
	return -1;
}
WEAPON* WeaponsResource::GetWeapon(int iId){
	return &rgWeapons[iId];
}
void WeaponsResource::AddWeapon(WEAPON* wp){
	rgWeapons[wp->iId] = *wp;
	gridSlotPosDataMap[wp->iSlot][wp->iSlotPos] = wp->iId;
	LoadWeaponSprites(&rgWeapons[wp->iId]);
}
void WeaponsResource::PickupWeapon(int id){
	WEAPON* wp = &rgWeapons[id];
	gridSlotMap[wp->iSlot][wp->iSlotPos] = id;
}
void WeaponsResource::DropWeapon(int s,int p){
	gridSlotMap[s][p] = -1;
}
void WeaponsResource::DropAllWeapons(void){
	memset(gridSlotMap, -1, sizeof gridSlotMap);
}
WEAPON* WeaponsResource::GetWeaponSlot(int slot, int pos){
	int id = gridSlotMap[slot][pos];
	if (id <= 0)
		return nullptr;
	return &rgWeapons[id];
}
int WeaponsResource::GetWeaponIdBySlot(int slot, int pos){
	return gridSlotPosDataMap[slot][pos];
}
void WeaponsResource::LoadAllWeaponSprites(void){
	for (size_t i = 0; i < MAX_WEAPONS; i++){
		if (rgWeapons[i].iId)
			LoadWeaponSprites(&rgWeapons[i]);
	}
}
int WeaponsResource::HasWeapon(int s, int p) {
	return gridSlotMap[s][p] > 0;
}
int WeaponsResource::HasWeapon(WEAPON* wp){
	return gridSlotMap[wp->iSlot][wp->iSlotPos] > 0;
}
void WeaponsResource::LoadWeaponSprites(WEAPON* pWeapon){
	int i, iRes;

	if (ScreenWidth < 640)
		iRes = 320;
	else
		iRes = 640;

	char sz[128];

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

	sprintf_s(sz, "sprites/%s.txt", pWeapon->szName);
	client_sprite_t* pList = SPR_GetList(sz, &i);

	if (!pList)
		return;

	client_sprite_t* p;

	p = GetSpriteList(pList, "crosshair", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hCrosshair = SPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = NULL;

	p = GetSpriteList(pList, "autoaim", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAutoaim = SPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = GetSpriteList(pList, "zoom", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedCrosshair = SPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair; //default to non-zoomed crosshair
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = GetSpriteList(pList, "zoom_autoaim", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedAutoaim = SPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;  //default to zoomed crosshair
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = GetSpriteList(pList, "weapon", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hInactive = SPR_Load(sz);
		pWeapon->rcInactive = p->rc;
	}
	else
		pWeapon->hInactive = 0;

	p = GetSpriteList(pList, "weapon_s", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hActive = SPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = GetSpriteList(pList, "ammo", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo = SPR_Load(sz);
		pWeapon->rcAmmo = p->rc;
	}
	else
		pWeapon->hAmmo = 0;

	p = GetSpriteList(pList, "ammo2", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo2 = SPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;
	}
	else
		pWeapon->hAmmo2 = 0;

}
void WeaponsResource::LoadScriptWeaponSprites(int iId, char* cust){
	int i, iRes;
	if (ScreenWidth < 640)
		iRes = 320;
	else
		iRes = 640;
	char sz[128];
	WEAPON* pWeapon = GetWeapon(iId);
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
	sprintf_s(sz, "sprites/%s/%s.txt", cust, pWeapon->szName);
	client_sprite_t* pList = SPR_GetList(sz, &i);

	if (!pList)
		return;

	client_sprite_t* p;

	p = GetSpriteList(pList, "crosshair", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hCrosshair = SPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = NULL;

	p = GetSpriteList(pList, "autoaim", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAutoaim = SPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = GetSpriteList(pList, "zoom", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedCrosshair = SPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair; //default to non-zoomed crosshair
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = GetSpriteList(pList, "zoom_autoaim", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedAutoaim = SPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;  //default to zoomed crosshair
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = GetSpriteList(pList, "weapon", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hInactive = SPR_Load(sz);
		pWeapon->rcInactive = p->rc;
	}
	else
		pWeapon->hInactive = 0;

	p = GetSpriteList(pList, "weapon_s", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hActive = SPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = GetSpriteList(pList, "ammo", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo = SPR_Load(sz);
		pWeapon->rcAmmo = p->rc;
	}
	else
		pWeapon->hAmmo = 0;

	p = GetSpriteList(pList, "ammo2", iRes, i);
	if (p){
		sprintf_s(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo2 = SPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;
	}
	else
		pWeapon->hAmmo2 = 0;

}
HSPRITE* WeaponsResource::GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect){
	for (size_t i = 0; i < MAX_WEAPONS; i++){
		if (rgWeapons[i].iAmmoType == iAmmoId){
			rect = rgWeapons[i].rcAmmo;
			return &rgWeapons[i].hAmmo;
		}
		else if (rgWeapons[i].iAmmo2Type == iAmmoId){
			rect = rgWeapons[i].rcAmmo2;
			return &rgWeapons[i].hAmmo2;
		}
	}

	return nullptr;
}
void WeaponsResource::SelectSlot(int iSlot, int fAdvance, bool bJump){
	if (m_HudCustomAmmo.m_bAcceptDeadMessage)
		return;
	m_HudCustomAmmo.m_pNowSelectMenu->m_fFade =
		gEngfuncs.GetClientTime() + m_HudCustomAmmo.m_pNowSelectMenu->SelectHoldTime;

	WEAPON* wp = nullptr;
	iSlot = mathlib::clamp(iSlot, 0, MAX_WEAPON_SLOT_INDEX);
	//如果是当前slot
	if (iSlot == iNowSlot) {
		//避免死循环
		if (gridDrawMenu[iNowSlot].iPos < 0 && gridDrawMenu[iNowSlot].iId < 0)
			return;
		int menuPos = mathlib::clamp(gridDrawMenu[iNowSlot].iPos + fAdvance, 0, MAX_WEAPON_POSUSER_INDEX);
		while (menuPos != gridDrawMenu[iNowSlot].iPos) {
			wp = GetWeaponSlot(iSlot, menuPos);
			menuPos += fAdvance;
			if (menuPos >= MAX_WEAPON_POSITIONS_USER)
				menuPos = 0;
			else if (menuPos < 0) 
				menuPos = MAX_WEAPON_POSUSER_INDEX;
			if (wp) {
				//经典样式
				if (gCVars.pAmmoMenuStyle->value <= 0 && HasAmmo(wp))
					break;
				//环形样式
				else if(gCVars.pAmmoMenuStyle->value >0)
					break;
			}
		}
		if (!wp)
			wp = fAdvance > 0 ? gWR.GetFirstPos(iNowSlot) : gWR.GetLastPos(iNowSlot);
			
		if (wp && wp->iId > 0) {
			gridDrawMenu[iNowSlot].iId = wp->iId;
			gridDrawMenu[iNowSlot].iPos = wp->iSlotPos;
		}
		else if (gCVars.pAmmoMenuStyle->value <= 0) {
			gridDrawMenu[iNowSlot].iId = gridDrawMenu[iNowSlot].iPos = -1;
			return;
		}
	}
	//如果不是
	else {
		iNowSlot = iSlot;
		//经典样式
		if (gCVars.pAmmoMenuStyle->value <= 0) {
			wp = gWR.GetFirstPos(iNowSlot);
			if (wp && wp->iId > 0) {
				gridDrawMenu[iNowSlot].iId = wp->iId;
				gridDrawMenu[iNowSlot].iPos = wp->iSlotPos;
			}
			else{
				gridDrawMenu[iNowSlot].iId = gridDrawMenu[iNowSlot].iPos = -1;
				return;
			}
		}
	}
}
WEAPON* WeaponsResource::GetFirstPos(int iSlot){
	if (iSlot >= MAX_WEAPON_SLOTS)
		return nullptr;
	WEAPON* now = nullptr;
	for (size_t i = 0; i < MAX_WEAPON_POSITIONS_USER; i++){
		now = GetWeaponSlot(iSlot, i);
		if (HasWeapon(iSlot,i) && HasAmmo(now))
			return now;
	}
	return nullptr;
}
WEAPON* WeaponsResource::GetLastPos(int iSlot) {
	if (iSlot >= MAX_WEAPON_SLOTS)
		return nullptr;
	WEAPON* now = nullptr;
	for (size_t i = MAX_WEAPON_POSUSER_INDEX; i > 0; i--) {
		now = GetWeaponSlot(iSlot, i);
		if (HasWeapon(iSlot, i) && HasAmmo(now))
			return now;
	}
	return nullptr;
}
int WeaponsResource::CountAmmo(int iId){
	if (iId < 0)
		return 0;
	return  riAmmo[iId];
}
void WeaponsResource::SetUserSlot(int iSlot, int iId){
	gridSlotMap[iSlot][MAX_WEAPON_POSUSER_INDEX] = iId;
}
int WeaponsResource::HasAmmo(WEAPON* p){
	if (!p)
		return false;
	if (p->iMax1 == -1 && p->iMax2 == -1)
		return true;
	if (p->iAmmoType > -1){
		bool bFlag = (p->iClip > 0) || CountAmmo(p->iAmmoType);
		if (p->iAmmo2Type > -1 && !bFlag)
			return CountAmmo(p->iAmmo2Type) > 0 || (p->iClip2 > 0);
		else
			return bFlag;
	}
	return true;
}
AMMO WeaponsResource::GetAmmo(int iId){
	return iId;
}
void WeaponsResource::SetAmmo(int iId, int iCount){
	riAmmo[iId] = iCount;
}
gridmenuitem_t* WeaponsResource::GetDrawMenuItem(size_t iSlot) {
	return &gridDrawMenu[iSlot];
}
void WeaponsResource::FillDrawMenuGrid() {
	gridmenuitem_t* pItem = nullptr;
	for (size_t i = 0; i < MAX_WEAPON_SLOTS; i++) {
		pItem = GetDrawMenuItem(i);
		if (pItem->iId > 0 && HasWeapon(i, pItem->iPos))
			continue;
		else {
			for (size_t j = 0; j < MAX_WEAPON_POSITIONS_USER; j++) {
				if (HasWeapon(i, j)) {
					gridDrawMenu[i].iId = gridSlotMap[i][j];
					gridDrawMenu[i].iPos = (int)j;
					break;
				}
				else {
					gridDrawMenu[i].iId = -1;
					gridDrawMenu[i].iPos = -1;
				}
			}
		}
	}
}
WeaponsResource gWR;