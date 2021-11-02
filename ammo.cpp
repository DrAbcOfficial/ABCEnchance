#include <metahook.h>
#include "parsemsg.h"
#include "pm_shared.h"

#include <cmath>
#include <mathlib.h>

#include "hud.h"
#include "weapon.h"
#include "CColor.h"
#include "vguilocal.h"
#include "local.h"
#include "myconst.h"
#include "CHudDelegate.h"

#include "enginedef.h"
#include "exportfuncs.h"
#include "drawElement.h"
#include "utility.h"

#include "glew.h"
#include "IWeaponSelect.h"
#include "wmenu_annular.h"
#include "wmenu_slot.h"

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
int __MsgFunc_AmmoX(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	gWR.SetAmmo(iIndex, abs(iCount));
	return m_pfnAmmoX(pszName, iSize, pbuf);
}
int __MsgFunc_AmmoPickup(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	gHR.AddToHistory(HISTSLOT_AMMO, iIndex, abs(iCount));
	return m_pfnAmmoPickup(pszName, iSize, pbuf);
}
int __MsgFunc_WeapPickup(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_SHORT();
	gHR.AddToHistory(HISTSLOT_WEAP, iIndex);
	return m_pfnWeapPickup(pszName, iSize, pbuf);
}
int __MsgFunc_ItemPickup(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	const char* szName = READ_STRING();
	gHR.AddToHistory(HISTSLOT_ITEM, szName);
	return m_pfnItemPickup(pszName, iSize, pbuf);
}
int __MsgFunc_WeaponList(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);

	WEAPON Weapon;
	strcpy_s(Weapon.szName, READ_STRING());
	Weapon.iAmmoType = READ_CHAR();
	Weapon.iMax1 = READ_LONG();
	if (Weapon.iMax1 == 255)
		Weapon.iMax1 = -1;

	Weapon.iAmmo2Type = READ_CHAR();
	Weapon.iMax2 = READ_LONG();
	if (Weapon.iMax2 == 255)
		Weapon.iMax2 = -1;

	Weapon.iSlot = READ_CHAR();
	Weapon.iSlotPos = READ_CHAR();
	Weapon.iId = READ_SHORT();
	Weapon.iFlags = READ_BYTE();
	Weapon.iClip = 0;
	
	for (int i = 0; i < MAX_WEAPON_SLOTS; i++) {
		if (!gCVars.pAmmoCSlot[i]->string || gCVars.pAmmoCSlot[i]->string[0] == 0)
			continue;
		if (strcmp(Weapon.szName, gCVars.pAmmoCSlot[i]->string) == 0) {
			gWR.SetUserSlot(i, Weapon.iId);
			break;
		}
	}
	/*
	* * 在找到解决方案前暂时停止修复选枪冲突
	int posFlag = Weapon.iSlotPos;
	int tw = gWR.GetWeaponIdBySlot(Weapon.iSlot, posFlag);
	while (tw > 0){
		posFlag++;
		//草你真的应该去找服主排查下冲突
		if (Weapon.iSlotPos >= MAX_WEAPON_POSITIONS - 1) {
			posFlag = Weapon.iSlotPos;
			break;
		}
		tw = gWR.GetWeaponIdBySlot(Weapon.iSlot, posFlag);
	}
	if(posFlag != Weapon.iSlotPos)
		Weapon.iSlotPos = posFlag;
		*/
	gWR.AddWeapon(&Weapon);
	return m_pfnWeaponList(pszName, iSize, pbuf);
}
int __MsgFunc_CustWeapon(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	char name[128];
	strcpy_s(name, READ_STRING());
	if (name[0] != 0)
		gWR.LoadScriptWeaponSprites(id, name);
	gWR.PickupWeapon(id);
	return m_pfnCustWeapon(pszName, iSize, pbuf);

}
int __MsgFunc_CurWeapon(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int iState = READ_BYTE();
	if (iState > 0){
		int iId = READ_SHORT();
		//sc反编汇后如此
		if (iId == -1){
			gWR.DropAllWeapons();
			return m_pfnCurWeapon(pszName, iSize, pbuf);
		}
		int iClip = READ_LONG();
		int iClip2 = READ_LONG();
		if(gHudDelegate->m_iPlayerHealth > 0)
			m_HudCustomAmmo.m_bAcceptDeadMessage = FALSE;
		WEAPON* pWeapon = gWR.GetWeapon(iId);
		if (!pWeapon)
			return m_pfnCurWeapon(pszName, iSize, pbuf);
		//更新弹匣信息
		pWeapon->iClip = iClip;
		pWeapon->iClip2 = iClip2;
		m_HudCustomAmmo.m_pWeapon = pWeapon;
		m_HudCustomAmmo.m_bIsOnTarget = iState > 1;
	}
	else{
		int iFlag1 = READ_BYTE();
		int iFlag2 = READ_BYTE();
		int iAll = iFlag1 + iFlag2;
		switch (iAll){
		case 0X1FE:{
			if(m_HudCustomAmmo.m_bAcceptDeadMessage)
				gWR.DropAllWeapons();
			if(gHudDelegate->m_iPlayerHealth <= 0)
				m_HudCustomAmmo.m_bAcceptDeadMessage = TRUE;
			break;
		} 
		case 0:gWR.DropAllWeapons();
		}
	}
	return m_pfnCurWeapon(pszName, iSize, pbuf);
}

void(*UserCmd_Slot1)(void);
void(*UserCmd_Slot2)(void);
void(*UserCmd_Slot3)(void);
void(*UserCmd_Slot4)(void);
void(*UserCmd_Slot5)(void);
void(*UserCmd_Slot6)(void);
void(*UserCmd_Slot7)(void);
void(*UserCmd_Slot8)(void);
void(*UserCmd_Slot9)(void);
void(*UserCmd_Slot10)(void);
void(*UserCmd_SlotClose)(void);
void(*UserCmd_NextWeapon)(void);
void(*UserCmd_PrevWeapon)(void);
void(*UserCmd_Attack1)(void);
void __UserCmd_Slot1(void){
	m_HudCustomAmmo.SlotInput(0,1);
	return UserCmd_Slot1();
}
void __UserCmd_Slot2(void){
	m_HudCustomAmmo.SlotInput(1,1);
	return UserCmd_Slot2();
}
void __UserCmd_Slot3(void){
	m_HudCustomAmmo.SlotInput(2,1);
	return UserCmd_Slot3();
}
void __UserCmd_Slot4(void){
	m_HudCustomAmmo.SlotInput(3,1);
	return UserCmd_Slot4();
}
void __UserCmd_Slot5(void){
	m_HudCustomAmmo.SlotInput(4,1);
	return UserCmd_Slot5();
}
void __UserCmd_Slot6(void){
	m_HudCustomAmmo.SlotInput(5,1);
	return UserCmd_Slot6();
}
void __UserCmd_Slot7(void){
	m_HudCustomAmmo.SlotInput(6,1);
	return UserCmd_Slot7();
}
void __UserCmd_Slot8(void){
	m_HudCustomAmmo.SlotInput(7,1);
	return UserCmd_Slot8();
}
void __UserCmd_Slot9(void){
	m_HudCustomAmmo.SlotInput(8,1);
	return UserCmd_Slot9();
}
void __UserCmd_Slot10(void){
	m_HudCustomAmmo.SlotInput(9,1);
	return UserCmd_Slot10();
}
void __UserCmd_Close(void){

	return UserCmd_SlotClose();
}
void __UserCmd_NextWeapon(void){
	
	m_HudCustomAmmo.SlotInput(gWR.iNowSlot, 1);
	return UserCmd_NextWeapon();
}
void __UserCmd_PrevWeapon(void){
	m_HudCustomAmmo.SlotInput(gWR.iNowSlot, -1);
	return UserCmd_PrevWeapon();
}
void __UserCmd_Attack1(void){
	m_HudCustomAmmo.m_pNowSelectMenu->Select();
	if (gCVars.pAmmoMenuStyle->value <= 0 && m_HudWMenuAnnular.m_bOpeningMenu)
		m_HudWMenuAnnular.Select();
	return UserCmd_Attack1();
}
void CustomSlotSetCallBack(cvar_t* vars){
	if (!vars->string || vars->string[0] == 0)
		return;
	int slot;
	sscanf_s(vars->name, "cl_customslot%d", &slot);
	slot--;
	gWR.SetUserSlot(slot, gWR.GetWeaponId(vars->string));
}
void ChangeWMenuStyleCallBack(cvar_t* vars) {
	if (vars->value > 0)
		m_HudCustomAmmo.m_pNowSelectMenu = &m_HudWMenuAnnular;
	else
		m_HudCustomAmmo.m_pNowSelectMenu = &m_HudWMenuSlot;
}
void CHudCustomAmmo::GLInit(){
	//所有选择菜单都要加载
	m_HudWMenuAnnular.GLInit();
}
int CHudCustomAmmo::Init(void){
	m_pfnCurWeapon = HOOK_MESSAGE(CurWeapon);
	m_pfnWeaponList = HOOK_MESSAGE(WeaponList);
	m_pfnCustWeapon = HOOK_MESSAGE(CustWeapon);
	m_pfnAmmoPickup = HOOK_MESSAGE(AmmoPickup);
	m_pfnWeapPickup = HOOK_MESSAGE(WeapPickup);
	m_pfnItemPickup = HOOK_MESSAGE(ItemPickup);
	m_pfnAmmoX = HOOK_MESSAGE(AmmoX);

	UserCmd_Slot1 = HOOK_COMMAND("slot1", Slot1);
	UserCmd_Slot2 = HOOK_COMMAND("slot2", Slot2);
	UserCmd_Slot3 = HOOK_COMMAND("slot3", Slot3);
	UserCmd_Slot4 = HOOK_COMMAND("slot4", Slot4);
	UserCmd_Slot5 = HOOK_COMMAND("slot5", Slot5);
	UserCmd_Slot6 = HOOK_COMMAND("slot6", Slot6);
	UserCmd_Slot7 = HOOK_COMMAND("slot7", Slot7);
	UserCmd_Slot8 = HOOK_COMMAND("slot8", Slot8);
	UserCmd_Slot9 = HOOK_COMMAND("slot9", Slot9);
	UserCmd_Slot10 = HOOK_COMMAND("slot10", Slot10);
	UserCmd_SlotClose = HOOK_COMMAND("cancelselect", Close);
	UserCmd_NextWeapon = HOOK_COMMAND("invnext", NextWeapon);
	UserCmd_PrevWeapon = HOOK_COMMAND("invprev", PrevWeapon);
	UserCmd_Attack1 = HOOK_COMMAND("+attack", Attack1);

	gCVars.pAmmoCSlot[0] = CREATE_CVAR("cl_customslot1", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[1] = CREATE_CVAR("cl_customslot2", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[2] = CREATE_CVAR("cl_customslot3", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[3] = CREATE_CVAR("cl_customslot4", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[4] = CREATE_CVAR("cl_customslot5", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[5] = CREATE_CVAR("cl_customslot6", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[6] = CREATE_CVAR("cl_customslot7", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[7] = CREATE_CVAR("cl_customslot8", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[8] = CREATE_CVAR("cl_customslot9", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoCSlot[9] = CREATE_CVAR("cl_customslot10", "", FCVAR_VALUE, CustomSlotSetCallBack);
	gCVars.pAmmoMenuDrawPos = CREATE_CVAR("cl_menudrawpos", "0", FCVAR_VALUE, NULL);
	gCVars.pAmmoMenuDrawRainbow = CREATE_CVAR("cl_rainbowmenu", "1", FCVAR_VALUE, NULL);
	gCVars.pAmmoMenuStyle = CREATE_CVAR("cl_wmenustyle", "1", FCVAR_VALUE, ChangeWMenuStyleCallBack);
	
	IconSize = GET_SCREEN_PIXEL(true, "AmmoHUD.IconSize");
	ElementGap = GET_SCREEN_PIXEL(true, "AmmoHUD.ElementGap");
	BackGroundY = GET_SCREEN_PIXEL(true, "AmmoHUD.BackGroundY");
	BackGroundLength = GET_SCREEN_PIXEL(false, "AmmoHUD.BackGroundLength");

	Ammo1IconColor = pScheme->GetColor("AmmoHUD.Ammo1IconColor", gDefaultColor);
	Ammo1BigTextColor = pScheme->GetColor("AmmoHUD.Ammo1BigTextColor", gDefaultColor);
	Ammo1TextColor = pScheme->GetColor("AmmoHUD.Ammo1TextColor", gDefaultColor);
	Ammo2IconColor = pScheme->GetColor("AmmoHUD.Ammo2IconColor", gDefaultColor);
	Ammo2BigTextColor = pScheme->GetColor("AmmoHUD.Ammo2BigTextColor", gDefaultColor);
	Ammo2TextColor = pScheme->GetColor("AmmoHUD.Ammo2TextColor", gDefaultColor);

	HUDFont = pScheme->GetFont("HUDShitFont", true);
	HUDSmallFont = pScheme->GetFont("HUDSmallShitFont", true);

	//所有选择菜单都要加载
	m_HudWMenuAnnular.Init();
	m_HudWMenuSlot.Init();
	Reset();
	//设置所选菜单
	if(gCVars.pAmmoMenuStyle->value > 0)
		m_pNowSelectMenu = &m_HudWMenuAnnular;
	else
		m_pNowSelectMenu = &m_HudWMenuSlot;
	gWR.Init();
	gHR.Init();
	return 1;
};
void CHudCustomAmmo::Reset(void){
	m_fNextSyncTime = 0;
	m_pWeapon = nullptr;
	m_bIsOnTarget = false;
	VGUI_CREATE_NEWTGA_TEXTURE(iBackGroundTga, "abcenchance/tga/ammobar_background");

	//所有选择菜单都要加载
	m_HudWMenuAnnular.Reset();
	m_HudWMenuSlot.Reset();

	gWR.Reset();
	gHR.Reset();
}
int CHudCustomAmmo::VidInit(void){
	gWR.LoadAllWeaponSprites();
	m_HudWMenuSlot.VidInit();
	return 1;
}
void CHudCustomAmmo::SyncWeapon(){
	for (int i = 0; i < MAX_WEAPON_OLDSLOTS; i++){
		for (int j = 0; j < MAX_WEAPON_OLDPOSITIONS; j++){
			//没有WeaponList信息，丢弃
			if (gWR.GetWeaponIdBySlot(i, j) <= 0)
				continue;
			//获得原位武器信息
			baseweapon_t* bwp = (*g_rgBaseSlots)[i][j];
			//若原位已没有
			if (!bwp) {
				//现位还有，丢弃武器
				if (gWR.HasWeapon(i, j))
					gWR.DropWeapon(i, j);
			}
			//若原位已有
			else {
				//现位没有，捡起武器
				if (!gWR.HasWeapon(i, j))
					gWR.PickupWeapon(bwp->iId);
				//同步弹匣数据
				WEAPON* wp = gWR.GetWeapon(bwp->iId);
				wp->iClip = bwp->iClip;
				wp->iClip2 = bwp->iClip2;
			}
		}
	}
}
int CHudCustomAmmo::Draw(float flTime){
	if (gHudDelegate->IsInSpectate())
		return 1;
	//IDK Why, this var is totally useless for sven coop
	//but ghidra never lie
	if (!(gClientData->weapons & 0x80000000))
		return 1;
	if (gClientData->health <= 0)
		return 1;
	//0.5s 进行一次武器同步
	if (gEngfuncs.GetClientTime() > m_fNextSyncTime) {
		SyncWeapon();
		m_fNextSyncTime = gEngfuncs.GetClientTime() + 0.5F;
	}
	// Draw Weapon Menu
	DrawWList(flTime);
	gHR.DrawAmmoHistory(flTime);
	if (!m_pWeapon)
		return 0;
	WEAPON* pw = m_pWeapon;
	if (pw->iId <= 0)
		return 0;
	if ((pw->iAmmoType < 0) && (pw->iAmmo2Type < 0))
		return 0;
	int r, g, b, a;
	float nowX = gScreenInfo.iWidth - BackGroundLength;
	float flBackGroundHeight = gScreenInfo.iHeight - BackGroundY;
	float flCenterX = nowX + (gScreenInfo.iWidth - nowX) / 2, flCenterY = gScreenInfo.iHeight - flBackGroundHeight / 2;
	int nowY;
	int iTextHeight;
	int iTextWidth;
	wchar_t buf[16];
	if (pw->iAmmoType > 0){
		gHudDelegate->surface()->DrawSetTexture(-1);
		gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255);
		gHudDelegate->surface()->DrawSetTexture(iBackGroundTga);
		gHudDelegate->surface()->DrawTexturedRect(nowX, BackGroundY, gScreenInfo.iWidth, gScreenInfo.iHeight);
 		nowX = flCenterX;
		nowY = flCenterY;

		if (pw->iClip >= 0){
			Ammo1TextColor.GetColor(r, g, b, a);
			wsprintfW(buf, L"%d", gWR.CountAmmo(pw->iAmmoType));
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDSmallFont);
			nowX -= iTextWidth;
			nowY -= iTextHeight / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDSmallFont);

			Ammo1BigTextColor.GetColor(r, g, b, a);
			wsprintfW(buf, L"%d/", pw->iClip);
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDFont);
			nowX -= iTextWidth;
			nowY = flCenterY - iTextHeight / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDFont);
		}
		else{
			Ammo1BigTextColor.GetColor(r, g, b, a);
			wsprintfW(buf, L"%d", gWR.CountAmmo(pw->iAmmoType));
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDFont);
			nowX -= iTextWidth;
			nowY -= iTextHeight / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDFont);
		}

		Ammo1IconColor.GetColor(r, g, b, a);
		nowY = flCenterY - (m_pWeapon->rcAmmo.bottom - m_pWeapon->rcAmmo.top) / 2;
		nowX -= ElementGap + IconSize;
		gEngfuncs.pfnSPR_Set(m_pWeapon->hAmmo, r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, nowX, nowY, &m_pWeapon->rcAmmo);
	}

	if (pw->iAmmo2Type > 0){
		nowX = flCenterX + ElementGap * 2;
		if (pw->iClip2 >= 0){
			Ammo2BigTextColor.GetColor(r, g, b, a);
			wsprintfW(buf, L"%d/", pw->iClip2);
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDFont);
			nowY = flCenterY - iTextHeight / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDFont);
			nowX += iTextWidth;
	
			Ammo2TextColor.GetColor(r, g, b, a);
			wsprintfW(buf, L"%d", gWR.CountAmmo(pw->iAmmo2Type));
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDSmallFont);
			nowY = flCenterY - iTextHeight / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDSmallFont);
			nowX += iTextWidth;
		}
		else{
			Ammo2BigTextColor.GetColor(r, g, b, a);
			wsprintfW(buf, L"%d", gWR.CountAmmo(pw->iAmmo2Type));
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDFont);
			nowY = flCenterY - iTextHeight / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDFont);
			nowX += iTextWidth;
		}
		Ammo2IconColor.GetColor(r, g, b, a);
		nowX += ElementGap;
		nowY = flCenterY - (m_pWeapon->rcAmmo2.bottom - m_pWeapon->rcAmmo2.top) / 2;
		gEngfuncs.pfnSPR_Set(m_pWeapon->hAmmo2, r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, nowX, nowY, &m_pWeapon->rcAmmo2);
	}
	return 1;
}
void CHudCustomAmmo::ChosePlayerWeapon(void){
	if (gWR.gridDrawMenu[gWR.iNowSlot].iId > -1) {
		WEAPON* wp = gWR.GetWeapon(gWR.gridDrawMenu[gWR.iNowSlot].iId);
		if (!(wp->iFlags & ITEM_FLAG_SELECTONEMPTY) && !gWR.HasAmmo(wp))
			return;
		ServerCmd(wp->szName);
		PlaySoundByName("common/wpn_select.wav", 1);
		gWR.iNowSlot = -1;
	}
}
void CHudCustomAmmo::SlotInput(int iSlot, int fAdvance){
	if (gHookHud.m_Menu->m_fMenuDisplayed)
		return;
	gWR.SelectSlot(iSlot, fAdvance);
}
int CHudCustomAmmo::DrawWList(float flTime){
	m_pNowSelectMenu->DrawWList(flTime);
	if (gCVars.pAmmoMenuStyle->value <= 0 && m_HudWMenuAnnular.m_bOpeningMenu)
		m_HudWMenuAnnular.DrawWList(flTime);
	return 1;
}
void CHudCustomAmmo::ClientMove(struct playermove_s* ppmove, qboolean server){
	if (m_HudWMenuAnnular.m_bOpeningMenu)
		m_HudWMenuAnnular.m_fFade = gEngfuncs.GetClientTime() + m_HudWMenuAnnular.SelectHoldTime;
}
void CHudCustomAmmo::IN_Accumulate(){
	if (m_HudWMenuAnnular.m_bOpeningMenu) {
		int x, y;
		gEngfuncs.GetMousePosition(&x, &y);
		if (!m_HudWMenuAnnular.m_bSetedCursor) {
			gEngfuncs.pfnSetMousePos(gScreenInfo.iWidth / 2, gScreenInfo.iHeight / 2);
			m_HudWMenuAnnular.m_bSetedCursor = true;
		}
		x -= gScreenInfo.iWidth / 2;
		y -= gScreenInfo.iHeight / 2;
		y = -y;
		m_HudWMenuAnnular.m_fCursorAngle = atan2(y, x);
		int s = m_HudWMenuAnnular.m_fCursorAngle / (0.2 * M_PI);
		s = m_HudWMenuAnnular.m_fCursorAngle >= 0 ? s : 9 + s;
		if (gWR.gridDrawMenu[s].iId > -1)
			gWR.iNowSlot = s;
	}
}
void CHudCustomAmmo::Clear(){
	//所有选择列表都需要清理
	m_HudWMenuAnnular.Clear();
}