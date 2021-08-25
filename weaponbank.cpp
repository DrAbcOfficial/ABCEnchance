#include <metahook.h>
#include <map>
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "ammo.h"
#include "CHudDelegate.h"
#include "historyresource.h"
#include "weaponbank.h"

client_sprite_t* GetSpriteList(client_sprite_t* pList, const char* psz, int iRes, int iCount)
{
	if (!pList)
		return NULL;

	int i = iCount;
	client_sprite_t* p = pList;

	while (i--)
	{
		if ((!strcmp(psz, p->szName)) && (p->iRes == iRes))
			return p;
		p++;
	}

	return NULL;
}

void WeaponsResource::Init(void)
{
	Reset();
}
void WeaponsResource::Reset(void)
{
	iNowSlot = -1;
	memset(rgWeapons, 0, sizeof rgWeapons);
	memset(riAmmo, 0, sizeof riAmmo);
	memset(gridSlotPosDataMap, -1, sizeof gridSlotPosDataMap);
	memset(gridSlotMap, -1, sizeof gridSlotMap);
	memset(gridDrawMenu, -1, sizeof gridDrawMenu);
}
int WeaponsResource::CountGridWeapons() {
	int i, j, c = 0;
	for (i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		for (j = 0; j < MAX_WEAPON_POSITIONS; j++)
		{
			if (gridSlotMap[i][j] >= 0)
				c++;
		}
	}
	return c;
}
int WeaponsResource::CountWeapons() {
	int i, c = 0;
	for (i = 0; i < MAX_WEAPONS; i++)
	{
		if (GetWeapon(i)->iId > 0)
			c++;
	}
	return c;
}
int WeaponsResource::CountMenuWeapons() {
	int i, c = 0;
	for (i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		if (gridDrawMenu[i].iId > 0)
			c++;
	}
	return c;
}
WEAPON* WeaponsResource::GetWeapon(int iId)
{
	return &rgWeapons[iId];
}
void WeaponsResource::AddWeapon(WEAPON* wp)
{
	rgWeapons[wp->iId] = *wp;
	gridSlotPosDataMap[wp->iSlot][wp->iSlotPos] = wp->iId;
	LoadWeaponSprites(&rgWeapons[wp->iId]);
}
void WeaponsResource::PickupWeapon(int id)
{
	WEAPON* wp = &rgWeapons[id];
	gridSlotMap[wp->iSlot][wp->iSlotPos] = id;
	//野蛮修复武器弹匣无法获取问题，必定要改
	if (wp->iAmmoType > 0 && !HasAmmo(wp))
		wp->iClip = 1;
}
void WeaponsResource::DropWeapon(WEAPON* wp)
{
	gridSlotMap[wp->iSlot][wp->iSlotPos] = -1;
}
void WeaponsResource::DropAllWeapons(void)
{
	memset(gridSlotMap, -1, sizeof gridSlotMap);
}
WEAPON* WeaponsResource::GetWeaponSlot(int slot, int pos)
{
	return &rgWeapons[gridSlotMap[slot][pos]];
}
int WeaponsResource::GetWeaponIdBySlot(int slot, int pos)
{
	return gridSlotPosDataMap[slot][pos];
}
void WeaponsResource::LoadAllWeaponSprites(void)
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if (rgWeapons[i].iId)
			LoadWeaponSprites(&rgWeapons[i]);
	}
}
void WeaponsResource::LoadWeaponSprites(WEAPON* pWeapon)
{
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

	sprintf(sz, "sprites/%s.txt", pWeapon->szName);
	client_sprite_t* pList = SPR_GetList(sz, &i);

	if (!pList)
		return;

	client_sprite_t* p;

	p = GetSpriteList(pList, "crosshair", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hCrosshair = SPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = NULL;

	p = GetSpriteList(pList, "autoaim", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAutoaim = SPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = GetSpriteList(pList, "zoom", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedCrosshair = SPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else
	{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair; //default to non-zoomed crosshair
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = GetSpriteList(pList, "zoom_autoaim", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedAutoaim = SPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else
	{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;  //default to zoomed crosshair
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = GetSpriteList(pList, "weapon", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hInactive = SPR_Load(sz);
		pWeapon->rcInactive = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hInactive = 0;

	p = GetSpriteList(pList, "weapon_s", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hActive = SPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = GetSpriteList(pList, "ammo", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo = SPR_Load(sz);
		pWeapon->rcAmmo = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo = 0;

	p = GetSpriteList(pList, "ammo2", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo2 = SPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo2 = 0;

}
void WeaponsResource::LoadScriptWeaponSprites(int iId, char* cust)
{
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
	sprintf(sz, "sprites/%s/%s.txt", cust, pWeapon->szName);
	client_sprite_t* pList = SPR_GetList(sz, &i);

	if (!pList)
		return;

	client_sprite_t* p;

	p = GetSpriteList(pList, "crosshair", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hCrosshair = SPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = NULL;

	p = GetSpriteList(pList, "autoaim", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAutoaim = SPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = GetSpriteList(pList, "zoom", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedCrosshair = SPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else
	{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair; //default to non-zoomed crosshair
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = GetSpriteList(pList, "zoom_autoaim", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedAutoaim = SPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else
	{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;  //default to zoomed crosshair
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = GetSpriteList(pList, "weapon", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hInactive = SPR_Load(sz);
		pWeapon->rcInactive = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hInactive = 0;

	p = GetSpriteList(pList, "weapon_s", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hActive = SPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = GetSpriteList(pList, "ammo", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo = SPR_Load(sz);
		pWeapon->rcAmmo = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo = 0;

	p = GetSpriteList(pList, "ammo2", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo2 = SPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;

		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo2 = 0;

}
HSPRITE* WeaponsResource::GetAmmoPicFromWeapon(int iAmmoId, wrect_t& rect)
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if (rgWeapons[i].iAmmoType == iAmmoId)
		{
			rect = rgWeapons[i].rcAmmo;
			return &rgWeapons[i].hAmmo;
		}
		else if (rgWeapons[i].iAmmo2Type == iAmmoId)
		{
			rect = rgWeapons[i].rcAmmo2;
			return &rgWeapons[i].hAmmo2;
		}
	}

	return NULL;
}
void WeaponsResource::SelectSlot(int iSlot, int fAdvance)
{
	if (m_HudCustomAmmo.m_bAcceptDeadMessage)
		return;
	if (iSlot >= MAX_WEAPON_SLOTS || iSlot < 0)
		return;

	if (iSlot == iNowSlot) {
		gridDrawMenu[iNowSlot].iPos += fAdvance;
		if (gridDrawMenu[iNowSlot].iPos < 0)
			gridDrawMenu[iNowSlot].iPos = MAX_WEAPON_POSITIONS-1;
		else if (gridDrawMenu[iNowSlot].iPos >= MAX_WEAPON_POSITIONS)
			gridDrawMenu[iNowSlot].iPos = 0;
	}
	else
		iNowSlot = iSlot;

	int iCount = 0;
	for (int i = 0; i < MAX_WEAPON_POSITIONS; i++)
	{
		if (gridSlotMap[iNowSlot][i] >= 0)
			iCount++;
	}

	if (iCount <= 0) {
		gridDrawMenu[iNowSlot].iId = -1;
		return;
	}
	while (gridDrawMenu[iNowSlot].iPos < MAX_WEAPON_POSITIONS)
	{
		if (gridSlotMap[iNowSlot][gridDrawMenu[iNowSlot].iPos] >= 0) {
			gridDrawMenu[iNowSlot].iId = gridSlotMap[iNowSlot][gridDrawMenu[iNowSlot].iPos];
			break;
		}
		gridDrawMenu[iNowSlot].iPos++;
		if (gridDrawMenu[iNowSlot].iPos >= MAX_WEAPON_POSITIONS)
			gridDrawMenu[iNowSlot].iPos = 0;
	}
	gEngfuncs.pfnPlaySoundByName("common/wpn_moveselect.wav", 1);
	m_HudCustomAmmo.m_fFade = gEngfuncs.GetClientTime() + m_HudCustomAmmo.SelectCyclerHoldTime;
}
void WeaponsResource::FillMenuGrid()
{
	int i,j;
	for (i = 0; i < MAX_WEAPON_SLOTS; i++){
		if (gridDrawMenu[i].iId > 0 && gridSlotMap[i][gridDrawMenu[i].iPos] > 0){
				continue;
		}
		else{
			for (j = 0; j < MAX_WEAPON_POSITIONS; j++){
				if (gridSlotMap[i][j] > 0){
					gridDrawMenu[i] = { gridSlotMap[i][j],j };
					break;
				}
				else
					gridDrawMenu[i] = { -1,-1 };
			}
		}
	}
}

int WeaponsResource::CountAmmo(int iId)
{
	if (iId < 0)
		return 0;
	return  riAmmo[iId];
}
int WeaponsResource::HasAmmo(WEAPON* p)
{
	if (!p)
		return false;
	if (p->iMax1 == -1 && p->iMax2 == -1)
		return true;
	bool bFlag = false;
	if (p->iAmmoType > -1)
	{
		bFlag = (p->iClip > 0) || CountAmmo(p->iAmmoType);
		if (p->iAmmo2Type > -1 && !bFlag)
			return CountAmmo(p->iAmmo2Type) || (p->iClip2 > 0);
		else
			return bFlag;
	}
}
AMMO WeaponsResource::GetAmmo(int iId)
{
	return iId;
}
void WeaponsResource::SetAmmo(int iId, int iCount)
{
	riAmmo[iId] = iCount;
}
WeaponsResource gWR;