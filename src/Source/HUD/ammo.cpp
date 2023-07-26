#include <metahook.h>
#include "parsemsg.h"
#include "pm_shared.h"

#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <mymathlib.h>
#include <mathlib/Vector.h>

#include "hud.h"
#include "weapon.h"
#include "Color.h"
#include "vguilocal.h"
#include "vgui_controls/Controls.h"
#include "local.h"
#include "glew.h"
#include "gl_def.h"
#include "gl_draw.h"
#include "CCustomHud.h"

#include "enginedef.h"
#include "exportfuncs.h"

#include "weaponselect.h"

#include "weaponbank.h"
#include "historyresource.h"
#include "ammo.h"

CHudCustomAmmo m_HudCustomAmmo;

pfnUserMsgHook m_pfnCurWeapon;
pfnUserMsgHook m_pfnWeaponList;
pfnUserMsgHook m_pfnCustWeapon;
pfnUserMsgHook m_pfnAmmoPickup;
pfnUserMsgHook m_pfnWeapPickup;
pfnUserMsgHook m_pfnItemPickup;
pfnUserMsgHook m_pfnAmmoX;
pfnUserMsgHook m_pfnHideWeapon;
pfnUserMsgHook m_pfnHideHUD;
pfnUserMsgHook m_pfnWeaponSpr;

int __MsgFunc_AmmoX(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	gWR.SetAmmo(iIndex, abs(iCount));
	gCustomHud.SetCurWeapon(gCustomHud.GetCurWeapon());
	return m_pfnAmmoX(pszName, iSize, pbuf);
}
int __MsgFunc_AmmoPickup(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	gHR.AddToHistory(HistoryResource::HISTSLOT_AMMO, iIndex, abs(iCount));
	return m_pfnAmmoPickup(pszName, iSize, pbuf);
}
int __MsgFunc_WeapPickup(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_SHORT();
	gHR.AddToHistory(HistoryResource::HISTSLOT_WEAP, iIndex);
	return m_pfnWeapPickup(pszName, iSize, pbuf);
}
int __MsgFunc_ItemPickup(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	const char* szName = READ_STRING();
	gHR.AddToHistory(HistoryResource::HISTSLOT_ITEM, szName);
	return m_pfnItemPickup(pszName, iSize, pbuf);
}
int __MsgFunc_WeaponList(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);

	WEAPON* Weapon = new WEAPON();
	strcpy_s(Weapon->szName, READ_STRING());
	strcpy_s(Weapon->szSprName, Weapon->szName);
	Weapon->iAmmoType = READ_CHAR();
	Weapon->iMax1 = READ_LONG();
	if (Weapon->iMax1 == 255)
		Weapon->iMax1 = -1;

	Weapon->iAmmo2Type = READ_CHAR();
	Weapon->iMax2 = READ_LONG();
	if (Weapon->iMax2 == 255)
		Weapon->iMax2 = -1;

	Weapon->iSlot = READ_CHAR();
	Weapon->iSlotPos = READ_CHAR();
	Weapon->iId = READ_SHORT();
	Weapon->iFlags = READ_BYTE();
	Weapon->iClip = 0;

	WEAPON* wp = gWR.GetWeapon(Weapon->iId);
	if(!wp)
		gWR.AddWeapon(Weapon);
	else {
		strcpy_s(wp->szName, Weapon->szName);
		strcpy_s(wp->szSprName, Weapon->szName);
		wp->iAmmoType = Weapon->iAmmoType;
		wp->iMax1 = Weapon->iMax1;
		wp->iAmmo2Type = Weapon->iAmmo2Type;
		wp->iMax2 = Weapon->iMax2;
		wp->iSlot = Weapon->iSlot;
		wp->iSlotPos = Weapon->iSlotPos;
		wp->iFlags = Weapon->iFlags;
		delete Weapon;
	}
	return m_pfnWeaponList(pszName, iSize, pbuf);
}
int __MsgFunc_CustWeapon(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	char name[128];
	strcpy_s(name, READ_STRING());
	if (name[0] != 0)
		gWR.LoadWeaponSprites(id, name);;
	return m_pfnCustWeapon(pszName, iSize, pbuf);

}

int __MsgFunc_CurWeapon(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int iState = READ_BYTE();
	if (iState){
		int iId = READ_SHORT();
		//sc反编汇后如此
		if (iId == -1){
			gWR.DropAllWeapons();
			return m_pfnCurWeapon(pszName, iSize, pbuf);
		}
		int iClip = READ_LONG();
		int iClip2 = READ_LONG();
		if(gCustomHud.m_iPlayerHealth > 0)
			m_HudCustomAmmo.m_bAcceptDeadMessage = false;
		WEAPON* pWeapon = gWR.GetWeapon(iId);
		if (!pWeapon)
			return m_pfnCurWeapon(pszName, iSize, pbuf);
		//更新弹匣信息
		pWeapon->iClip = iClip;
		pWeapon->iClip2 = iClip2;
		pWeapon->iState = iState;
		m_HudCustomAmmo.SetCurWeapon(pWeapon);
		gCustomHud.SetCurWeapon(pWeapon);
	}
	else{
		int iFlag1 = READ_BYTE();
		int iFlag2 = READ_BYTE();
		int iAll = iFlag1 + iFlag2;
		switch (iAll){
		case 0X1FE:{
			if(m_HudCustomAmmo.m_bAcceptDeadMessage)
				gWR.DropAllWeapons();
			if(gCustomHud.m_iPlayerHealth <= 0)
				m_HudCustomAmmo.m_bAcceptDeadMessage = true;
			break;
		} 
		case 0:gWR.DropAllWeapons();
		}
	}
	return m_pfnCurWeapon(pszName, iSize, pbuf);
}
int __MsgFunc_HideWeapon(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	gCustomHud.HudHideCallBack(READ_BYTE());
	return m_pfnHideWeapon(pszName, iSize, pbuf);
}
int __MsgFunc_HideHUD(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	gCustomHud.HudHideCallBack(READ_BYTE());
	return m_pfnHideHUD(pszName, iSize, pbuf);
}
//uzi akimbo
//shit uzi
int __MsgFunc_WeaponSpr(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	char name[128];
	strcpy_s(name, READ_STRING());
	if (name[0] != 0) {
		WEAPON* wp = gWR.GetWeapon(id);
		if (wp && wp->iId > 0) {
			strcpy_s(wp->szSprName, name);
			gWR.LoadWeaponSprites(wp);
		}
	}
	return m_pfnWeaponSpr(pszName, iSize, pbuf);
}
int CHudCustomAmmo::Init(void){
	m_pfnCurWeapon = HOOK_MESSAGE(CurWeapon);
	m_pfnWeaponList = HOOK_MESSAGE(WeaponList);
	m_pfnCustWeapon = HOOK_MESSAGE(CustWeapon);
	m_pfnAmmoPickup = HOOK_MESSAGE(AmmoPickup);
	m_pfnWeapPickup = HOOK_MESSAGE(WeapPickup);
	m_pfnItemPickup = HOOK_MESSAGE(ItemPickup);
	m_pfnAmmoX = HOOK_MESSAGE(AmmoX);
	m_pfnHideWeapon = HOOK_MESSAGE(HideWeapon);
	m_pfnHideHUD = HOOK_MESSAGE(HideHUD);
	m_pfnWeaponSpr = HOOK_MESSAGE(WeaponSpr);

	gCVars.pAmmoMenuDrawRainbow = CREATE_CVAR("cl_rainbowmenu", "0", FCVAR_VALUE, NULL);

	Reset();
	gWR.Init();
	gHR.Init();
	return 1;
};
void CHudCustomAmmo::Reset(void){
	m_bSelectBlock = false;
	//所有选择菜单都要加载
	m_HudWMenuSlot.Reset();

	gWR.Reset();
	gHR.Reset();
}
int CHudCustomAmmo::VidInit(void){
	gHR.VidInit();
	gWR.LoadAllWeaponSprites();
	m_HudWMenuSlot.VidInit();
	return 1;
}
bool CHudCustomAmmo::IsVisible() {
	return m_HudWMenuSlot.IsVisible();
}
bool CHudCustomAmmo::ShouldDraw() {
	if (gCustomHud.IsInSpectate())
		return false;
	if (gCustomHud.IsHudHide(HUD_HIDEALL | HUD_HIDEWEAPONS))
		return false;
	if (!gCustomHud.HasSuit())
		return false;
	if (gClientData->health <= 0)
		return false;
	return true;
}
bool CHudCustomAmmo::BlockAttackOnce() {
	if (m_bSelectBlock) {
		m_bSelectBlock = false;
		return true;
	}
	return false;
}
void CHudCustomAmmo::Select() {
	if (!IsVisible())
		return;
	if (m_HudWMenuSlot.m_fFade > gEngfuncs.GetClientTime()) {
		if (m_HudCustomAmmo.m_bAcceptDeadMessage)
			return;
		m_HudCustomAmmo.ChosePlayerWeapon();
	}
	m_HudWMenuSlot.Select();
	gWR.m_iNowSlot = -1;
	m_bSelectBlock = true;
}
int CHudCustomAmmo::Draw(float flTime){
	if (!ShouldDraw())
		return 1;
	// Draw Weapon Menu
	DrawWList(flTime);
	gHR.DrawAmmoHistory(flTime);
	return 1;
}
void CHudCustomAmmo::ChosePlayerWeapon(){
	if (gWR.m_iNowSelected != nullptr) {
		if (!(gWR.m_iNowSelected->iFlags & ITEM_FLAG_SELECTONEMPTY) && !gWR.HasAmmo(gWR.m_iNowSelected))
			return;
		ServerCmd(gWR.m_iNowSelected->szName);
		PlaySoundByName("common/wpn_select.wav", 1);
	}
}
void CHudCustomAmmo::SlotInput(int iSlot, int fAdvance, bool bWheel){
	if (!gCustomHud.IsHudEnable())
		return;
	if (gCustomHud.SelectTextMenuItem(iSlot + 1))
		return;
	if (!gCustomHud.HasSuit())
		return;
	gWR.SelectSlot(iSlot, fAdvance, bWheel);
}
WEAPON* CHudCustomAmmo::GetCurWeapon(){
	return m_pWeapon;
}
void CHudCustomAmmo::SetCurWeapon(WEAPON* wp){
	m_pWeapon = wp;
}
int CHudCustomAmmo::DrawWList(float flTime){
	return m_HudWMenuSlot.DrawWList(flTime);
}