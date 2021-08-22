/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// Ammo.cpp
//
// implementation of CHudAmmo class
//
#include <metahook.h>
#include "parsemsg.h"
#include "pm_shared.h"
#include "msghook.h"
#include "command.h"

#include <math.h>
#include <mathlib.h>

#include "hud.h"
#include "weapon.h"

#include "vguilocal.h"
#include "CHudDelegate.h"

#include "exportfuncs.h"
#include <drawElement.h>

#include "weaponbank.h"
#include "historyresource.h"
#include "ammo.h"


CHudCustomAmmo m_HudCustomAmmo;

HSPRITE ghsprBuckets;					// Sprite for top row of weapons menu
int giBucketHeight, giBucketWidth, giABHeight, giABWidth; // Ammo Bar width and height

pfnUserMsgHook m_pfnCurWeapon;
pfnUserMsgHook m_pfnWeaponList;
pfnUserMsgHook m_pfnCustWeapon;
pfnUserMsgHook m_pfnAmmoPickup;
pfnUserMsgHook m_pfnWeapPickup;
pfnUserMsgHook m_pfnItemPickup;
pfnUserMsgHook m_pfnAmmoX;
pfnUserMsgHook m_pfnWeaponSpr;
int __MsgFunc_AmmoX(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();
	gWR.SetAmmo(iIndex, abs(iCount));
	return m_pfnAmmoX(pszName, iSize, pbuf);
}
int __MsgFunc_AmmoPickup(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	gHR.AddToHistory(HISTSLOT_AMMO, iIndex, abs(iCount));
	return m_pfnAmmoPickup(pszName, iSize, pbuf);
}
int __MsgFunc_WeapPickup(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_SHORT();
	gHR.AddToHistory(HISTSLOT_WEAP, iIndex);
	return m_pfnWeapPickup(pszName, iSize, pbuf);
}
int __MsgFunc_ItemPickup(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	const char* szName = READ_STRING();
	gHR.AddToHistory(HISTSLOT_ITEM, szName);
	return m_pfnItemPickup(pszName, iSize, pbuf);
}
int __MsgFunc_WeaponList(const char* pszName, int iSize, void* pbuf)
{
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

	int tw = gWR.GetWeaponIdBySlot(Weapon.iSlot, Weapon.iSlotPos);
	while (tw > 0)
	{
		Weapon.iSlotPos++;
		//草你真的应该去找服主排查下冲突
		if (Weapon.iSlotPos >= MAX_WEAPON_POSITIONS - 1)
			break;
		tw = gWR.GetWeaponIdBySlot(Weapon.iSlot, Weapon.iSlotPos);
	}
	gWR.AddWeapon(&Weapon);
	return m_pfnWeaponList(pszName, iSize, pbuf);
}
int __MsgFunc_CustWeapon(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	char name[128];
	strcpy_s(name, READ_STRING());
	if (name[0] != 0)
		gWR.LoadScriptWeaponSprites(id, name);
	gWR.PickupWeapon(id);
	return m_pfnCustWeapon(pszName, iSize, pbuf);

}
int __MsgFunc_WeaponSpr(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	char path[2048];
	strcpy_s(path, READ_STRING());
	return m_pfnWeaponSpr(pszName, iSize, pbuf);
}
int __MsgFunc_CurWeapon(const char* pszName, int iSize, void* pbuf)
{
	int bOnTarget = FALSE;
	BEGIN_READ(pbuf, iSize);
	int iState = READ_BYTE();
	if (iState > 0)
	{
		int iId = READ_SHORT();
		int iClip = READ_LONG();
		int iClip2 = READ_LONG();
		if (iState > 1)
			bOnTarget = TRUE;
		if(gHudDelegate->m_iPlayerHealth > 0)
			gHudDelegate->m_fPlayerDead = FALSE;
		WEAPON* pWeapon = gWR.GetWeapon(iId);
		if (!pWeapon)
			return m_pfnCurWeapon(pszName, iSize, pbuf);
		//更新弹匣信息
		pWeapon->iClip = iClip;
		pWeapon->iClip2 = iClip2;
		m_HudCustomAmmo.m_pWeapon = pWeapon;
		if (m_hfov >= gEngfuncs.pfnGetCvarFloat("default_fov"))
		{ 
			if (bOnTarget && m_HudCustomAmmo.m_pWeapon->hAutoaim)
				SetCrosshair(m_HudCustomAmmo.m_pWeapon->hAutoaim, m_HudCustomAmmo.m_pWeapon->rcAutoaim, 255, 255, 255);
			else
				SetCrosshair(m_HudCustomAmmo.m_pWeapon->hCrosshair, m_HudCustomAmmo.m_pWeapon->rcCrosshair, 255, 255, 255);
		}
		else
		{
			if (bOnTarget && m_HudCustomAmmo.m_pWeapon->hZoomedAutoaim)
				SetCrosshair(m_HudCustomAmmo.m_pWeapon->hZoomedAutoaim, m_HudCustomAmmo.m_pWeapon->rcZoomedAutoaim, 255, 255, 255);
			else
				SetCrosshair(m_HudCustomAmmo.m_pWeapon->hZoomedCrosshair, m_HudCustomAmmo.m_pWeapon->rcZoomedCrosshair, 255, 255, 255);

		}
	}
	else
	{
		int iFlag1 = READ_BYTE();
		int iFlag2 = READ_BYTE();
		int iAll = iFlag1 + iFlag2;
		switch (iAll)
		{
		case 0X1FE:{
			if(gHudDelegate->m_fPlayerDead)
				gWR.DropAllWeapons();
			gHudDelegate->m_fPlayerDead = TRUE;
			break;
		} 
		case 0X2:break;
		case 0:gWR.DropAllWeapons(); break;
		default: {
			WEAPON* wp = gWR.GetWeapon(iFlag1);
			if ((wp->iFlags & ITEM_FLAG_EXHAUSTIBLE) && !gWR.HasAmmo(wp))
				gWR.DropWeapon(wp);
			break;
		}
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
void __UserCmd_Slot1(void)
{
	m_HudCustomAmmo.SlotInput(0,1);
	return UserCmd_Slot1();
}
void __UserCmd_Slot2(void)
{
	m_HudCustomAmmo.SlotInput(1,1);
	return UserCmd_Slot2();
}
void __UserCmd_Slot3(void)
{
	m_HudCustomAmmo.SlotInput(2,1);
	return UserCmd_Slot3();
}
void __UserCmd_Slot4(void)
{
	m_HudCustomAmmo.SlotInput(3,1);
	return UserCmd_Slot4();
}
void __UserCmd_Slot5(void)
{
	m_HudCustomAmmo.SlotInput(4,1);
	return UserCmd_Slot5();
}
void __UserCmd_Slot6(void)
{
	m_HudCustomAmmo.SlotInput(5,1);
	return UserCmd_Slot6();
}
void __UserCmd_Slot7(void)
{
	m_HudCustomAmmo.SlotInput(6,1);
	return UserCmd_Slot7();
}
void __UserCmd_Slot8(void)
{
	m_HudCustomAmmo.SlotInput(7,1);
	return UserCmd_Slot8();
}
void __UserCmd_Slot9(void)
{
	m_HudCustomAmmo.SlotInput(8,1);
	return UserCmd_Slot9();
}
void __UserCmd_Slot10(void)
{
	m_HudCustomAmmo.SlotInput(9,1);
	return UserCmd_Slot10();
}
void __UserCmd_Close(void)
{

	return UserCmd_SlotClose();
}
void __UserCmd_NextWeapon(void)
{
	
	m_HudCustomAmmo.SlotInput(gWR.iNowSlot, 1);
	return UserCmd_NextWeapon();
}
void __UserCmd_PrevWeapon(void)
{
	m_HudCustomAmmo.SlotInput(gWR.iNowSlot, -1);
	return UserCmd_PrevWeapon();
}
void __UserCmd_Attack1(void)
{
	if (m_HudCustomAmmo.m_fFade > gEngfuncs.GetClientTime())
	{
		if (gHudDelegate->m_fPlayerDead)
			return UserCmd_Attack1();
		m_HudCustomAmmo.ChosePlayerWeapon();
	}
	gWR.iNowSelect = 0;
	gWR.iNowPos = 0;
	gWR.iNowSelect = 0;
	m_HudCustomAmmo.m_fFade = 0;
	m_HudCustomAmmo.m_bSelectMenuDisplay = false;
	return UserCmd_Attack1();
}
void __UserCmd_Drop(void)
{
	if (m_HudCustomAmmo.m_pWeapon->iId >0)
	{
		gWR.DropWeapon(m_HudCustomAmmo.m_pWeapon);
		gWR.iNowSelect = 0;
		gWR.iNowPos = 0;
		gWR.iNowSelect = 0;
		m_HudCustomAmmo.m_fFade = 0;
		m_HudCustomAmmo.m_bSelectMenuDisplay = false;
	}
	ServerCmd("drop");
}

int CHudCustomAmmo::Init(void)
{
	m_pfnCurWeapon = HOOK_MESSAGE(CurWeapon);
	m_pfnWeaponList = HOOK_MESSAGE(WeaponList);
	m_pfnCustWeapon = HOOK_MESSAGE(CustWeapon);
	m_pfnAmmoPickup = HOOK_MESSAGE(AmmoPickup);
	m_pfnWeapPickup = HOOK_MESSAGE(WeapPickup);
	m_pfnItemPickup = HOOK_MESSAGE(ItemPickup);
	m_pfnAmmoX = HOOK_MESSAGE(AmmoX);
	m_pfnWeaponSpr = HOOK_MESSAGE(WeaponSpr);
	
	gEngfuncs.pfnAddCommand("drop", __UserCmd_Drop);

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

	StartX = atof(pScheme->GetResourceString("AmmoHUD.StartX"));
	IconSize = atof(pScheme->GetResourceString("AmmoHUD.IconSize"));
	ElementGap = atof(pScheme->GetResourceString("AmmoHUD.ElementGap"));

	BackGroundY = atof(pScheme->GetResourceString("AmmoHUD.BackGroundY"));
	BackGroundLength = atof(pScheme->GetResourceString("AmmoHUD.BackGroundLength"));
	BackGroundAlpha = atof(pScheme->GetResourceString("AmmoHUD.BackGroundAlpha"));

	Ammo1IconColor = pScheme->GetColor("AmmoHUD.Ammo1IconColor", gDefaultColor);
	Ammo1BigTextColor = pScheme->GetColor("AmmoHUD.Ammo1BigTextColor", gDefaultColor);
	Ammo1TextColor = pScheme->GetColor("AmmoHUD.Ammo1TextColor", gDefaultColor);
	Ammo2IconColor = pScheme->GetColor("AmmoHUD.Ammo2IconColor", gDefaultColor);
	Ammo2BigTextColor = pScheme->GetColor("AmmoHUD.Ammo2BigTextColor", gDefaultColor);
	Ammo2TextColor = pScheme->GetColor("AmmoHUD.Ammo2TextColor", gDefaultColor);
	BackGroundColor = pScheme->GetColor("AmmoHUD.BackGroundColor", gDefaultColor);

	SelectCyclerColor = pScheme->GetColor("AmmoHUD.SelectCyclerColor", gDefaultColor);
	SelectCyclerRinColor = pScheme->GetColor("AmmoHUD.SelectCyclerRinColor", gDefaultColor);
	SelectCyclerIconColor = pScheme->GetColor("AmmoHUD.SelectCyclerIconColor", gDefaultColor);
	SelectCyclerTextColor = pScheme->GetColor("AmmoHUD.SelectCyclerTextColor", gDefaultColor);
	SelectCyclerEmptyColor = pScheme->GetColor("AmmoHUD.SelectCyclerEmptyColor", gDefaultColor);

	SelectCyclerOffset = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerOffset"));
	SelectCyclerSize = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerSize"));
	SelectCyclerRotate = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerRotate"));
	SelectCyclerAnimateTime = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerAnimateTime"));
	SelectCyclerFadeTime = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerFadeTime"));
	SelectCyclerHoldTime = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerHoldTime"));

	HUDFont = pScheme->GetFont("HUDShitFont", true);
	HUDSmallFont = pScheme->GetFont("HUDSmallShitFont", true);

	Reset();

	gWR.Init();
	gHR.Init();
	return 1;
};
void CHudCustomAmmo::Reset(void)
{
	m_fFade = 0;
	m_fAnimateTime = 0;
	m_bSelectMenuDisplay = false;
	iSelectCyclerSpr = gEngfuncs.pfnSPR_Load("abcenchance/spr/select_cycler.spr");
	iSelectCyclerRinSpr = gEngfuncs.pfnSPR_Load("abcenchance/spr/selected_rin.spr");
	gWR.Reset();
	gHR.Reset();
}
int CHudCustomAmmo::VidInit(void)
{
	m_HUD_bucket0 = gHudDelegate->GetSpriteIndex("bucket1");
	m_HUD_selection = gHudDelegate->GetSpriteIndex("selection");
	ghsprBuckets = gHudDelegate->GetSprite(m_HUD_bucket0);
	giBucketWidth = gHudDelegate->GetSpriteRect(m_HUD_bucket0).right - gHudDelegate->GetSpriteRect(m_HUD_bucket0).left;
	giBucketHeight = gHudDelegate->GetSpriteRect(m_HUD_bucket0).bottom - gHudDelegate->GetSpriteRect(m_HUD_bucket0).top;

	gHR.iHistoryGap = max(gHR.iHistoryGap, gHudDelegate->GetSpriteRect(m_HUD_bucket0).bottom - gHudDelegate->GetSpriteRect(m_HUD_bucket0).top);
	gWR.LoadAllWeaponSprites();

	if (ScreenWidth >= 640)
	{
		giABWidth = 20;
		giABHeight = 4;
	}
	else
	{
		giABWidth = 10;
		giABHeight = 2;
	}
	return 1;
}
int CHudCustomAmmo::Draw(float flTime)
{
	if (gClientData->health <= 0)
		return 1;
	// Draw Weapon Menu
	DrawWList(flTime);
	gHR.DrawAmmoHistory(flTime);
	if (!m_pWeapon)
		return 0;
	WEAPON* pw = m_pWeapon; // shorthand
	if ((pw->iAmmoType < 0) && (pw->iAmmo2Type < 0))
		return 0;

	int iFlags = DHN_DRAWZERO; // draw 0 values
	int r, g, b, a;
	float iSizeStep = (float)gScreenInfo.iWidth / 5 / BackGroundAlpha;
	float i = 0, nowX = gScreenInfo.iWidth;
	float flBackGroundY = gScreenInfo.iHeight * BackGroundY;
	float flBackGroundHeight = gScreenInfo.iHeight - flBackGroundY;
	int iStartX = gScreenInfo.iWidth / StartX;
	int nowY = 0;
	int iIconSize = flBackGroundHeight * IconSize;
	int iElementGap = flBackGroundHeight * ElementGap;
	int iTextHeight;
	int iTextWidth;
	wchar_t buf[16];

	if (pw->iAmmoType > 0)
	{

		BackGroundColor.GetColor(r, g, b, a);
		for (i = BackGroundAlpha; i > 0.0f; i--)
		{
			gEngfuncs.pfnFillRGBABlend(nowX, flBackGroundY, iSizeStep, flBackGroundHeight, r, g, b, i);
			nowX -= iSizeStep;
		}
		nowX += pw->iAmmo2Type > 0 ? iStartX : iStartX * 3;
		Ammo1IconColor.GetColor(r, g, b, a);
		//操你妈，吐了，毁灭吧，累了
		//nowY = flBackGroundY + iIconSize / 2;
		//float sprh = gEngfuncs.pfnSPR_Height(m_pWeapon->hAmmo, 0);
		//float sprw = gEngfuncs.pfnSPR_Width(m_pWeapon->hAmmo, 0);
		//DrawSPRIconClip(m_pWeapon->hAmmo, 0, 0, iIconSize, iIconSize, &m_pWeapon->rcAmmo, sprw, sprh, r, g, b, a);

		nowY = flBackGroundY + (m_pWeapon->rcAmmo.bottom - m_pWeapon->rcAmmo.top) / 2;
		gEngfuncs.pfnSPR_Set(m_pWeapon->hAmmo, r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, nowX, nowY, &m_pWeapon->rcAmmo);
		nowX += iIconSize + iElementGap;
		Ammo1BigTextColor.GetColor(r, g, b, a);
		if (pw->iClip >= 0)
		{
			wsprintfW(buf, L"%d", pw->iClip);
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDFont);
			nowY = flBackGroundY + (flBackGroundHeight - iTextHeight) / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDFont);
			nowX += iTextWidth;
			GetStringSize(L"/", &iTextWidth, NULL, HUDFont);
			DrawVGUI2String(L"/", nowX, nowY, r, g, b, HUDFont);
			nowX += iTextWidth;
			nowY += iTextHeight;
			Ammo1TextColor.GetColor(r, g, b, a);
			wsprintfW(buf, L"%d", gWR.CountAmmo(pw->iAmmoType));
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDSmallFont);
			nowY -= iTextHeight;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDSmallFont);
		}
		else
		{
			wsprintfW(buf, L"%d", gWR.CountAmmo(pw->iAmmoType));
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDFont);
			nowY = flBackGroundY + (flBackGroundHeight - iTextHeight) / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDFont);
		}
		nowX += iElementGap * 5;
	}

	if (pw->iAmmo2Type > 0)
	{
		Ammo2BigTextColor.GetColor(r, g, b, a);
		if (pw->iClip2 >= 0)
		{
			wsprintfW(buf, L"%d", pw->iClip2);
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDFont);
			nowY = flBackGroundY + (flBackGroundHeight - iTextHeight) / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDFont);
			nowX += iTextWidth;
			GetStringSize(L"/", &iTextWidth, NULL, HUDFont);
			DrawVGUI2String(L"/", nowX, nowY, r, g, b, HUDFont);
			Ammo2TextColor.GetColor(r, g, b, a);
			nowX += iTextWidth;
			nowY += iTextHeight;
			wsprintfW(buf, L"%d", gWR.CountAmmo(pw->iAmmo2Type));
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDSmallFont);
			nowY -= iTextHeight;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDSmallFont);
		}
		else
		{
			wsprintfW(buf, L"%d", gWR.CountAmmo(pw->iAmmo2Type));
			GetStringSize(buf, &iTextWidth, &iTextHeight, HUDFont);
			nowY = flBackGroundY + (flBackGroundHeight - iTextHeight) / 2;
			DrawVGUI2String(buf, nowX, nowY, r, g, b, HUDFont);
		}
		nowX += iElementGap;
		Ammo2IconColor.GetColor(r, g, b, a);
		nowY = flBackGroundY + (m_pWeapon->rcAmmo2.bottom - m_pWeapon->rcAmmo2.top) / 2;
		gEngfuncs.pfnSPR_Set(m_pWeapon->hAmmo2, r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, nowX, nowY, &m_pWeapon->rcAmmo2);
	}
	return 1;
}
void CHudCustomAmmo::ChosePlayerWeapon(void)
{
	if (gWR.iNowSelect >-1) {
		WEAPON* wp = gWR.GetWeapon(gWR.iNowSelect);
		if (!(wp->iFlags & ITEM_FLAG_SELECTONEMPTY) && !gWR.HasAmmo(wp))
			return;
		ServerCmd(wp->szName);
		gEngfuncs.pfnPlaySoundByName("common/wpn_select.wav", 1);
		gWR.iNowPos = -1;
		gWR.iNowSlot = -1;
		gWR.iNowSelect = -1;
	}
}
void CHudCustomAmmo::SlotInput(int iSlot, int fAdvance)
{
	if (gHookHud.m_Menu->m_fMenuDisplayed)
		return;
	if (m_fFade <= gEngfuncs.GetClientTime())
		gEngfuncs.pfnPlaySoundByName("common/wpn_hudon.wav", 1);
	gWR.SelectSlot(iSlot, fAdvance);
}
int CHudCustomAmmo::DrawWList(float flTime)
{
	if (m_fFade <= flTime)
	{
		if (m_bSelectMenuDisplay)
		{
			m_bSelectMenuDisplay = false;
			m_fAnimateTime = 0;
		}
		return 1;
	}
	
	gWR.FillMenuGrid();
	if(!iSelectCyclerSpr)
		iSelectCyclerSpr = gEngfuncs.pfnSPR_Load("abcenchance/spr/select_cycler.spr");
	if (!iSelectCyclerRinSpr)
		iSelectCyclerRinSpr = gEngfuncs.pfnSPR_Load("abcenchance/spr/selected_rin.spr");
	float flTimeDiffer = m_fFade - flTime;
	float flStartRot = SelectCyclerRotate;
	int iBackGroundHeight = SelectCyclerSize;
	int iOffset = SelectCyclerOffset;
	if (!m_bSelectMenuDisplay)
		m_fAnimateTime = flTime + SelectCyclerAnimateTime;
	if (m_fAnimateTime > flTime)
	{
		if (flTimeDiffer >= SelectCyclerHoldTime - SelectCyclerAnimateTime)
			iOffset *= ((float)(SelectCyclerHoldTime) - flTimeDiffer) / SelectCyclerAnimateTime;
	}	
	int i;
	float ac, as;
	vec2_t aryOut[10];
	vec2_t aryIn[10];

	//填充十边形坐标数组
	for (i = 0; i < 10; i++)
	{
		ac = cos(2 * M_PI * i / 10 + flStartRot);
		as = sin(2 * M_PI * i / 10 + flStartRot);

		aryIn[i][0] = iOffset * ac;
		aryIn[i][1] = iOffset * as;

		aryOut[i][0] = (iOffset + iBackGroundHeight) * ac;
		aryOut[i][1] = (iOffset + iBackGroundHeight) * as;
	}
	wchar_t buf[64];
	WEAPON* wp;
	int iHeight;
	int iWidth;
	int iTextWidth;
	int halfWidth = gScreenInfo.iWidth / 2;
	int halfHeight = gScreenInfo.iHeight / 2;
	int xpos;
	int ypos;
	vec2_t vecA, vecB, vecC, vecD;
	int a = 255;
	if (flTimeDiffer < SelectCyclerFadeTime)
		a *= flTimeDiffer / SelectCyclerFadeTime;
	int r, g, b, dummy;
	for (i = 0; i < 10; i++)
	{
		//CABD
		//↓→↑
		Vector2Copy(aryIn[i == 9 ? 0 :i + 1], vecA);
		Vector2Copy(aryIn[i], vecB);
		Vector2Copy(aryOut[i == 9 ? 0 : i + 1], vecC);
		Vector2Copy(aryOut[i], vecD);
		//变换为OpenGL屏幕坐标
		vecA[0] += halfWidth;
		vecA[1] = halfHeight - vecA[1];
		vecB[0] += halfWidth;
		vecB[1] = halfHeight - vecB[1];
		vecC[0] += halfWidth;
		vecC[1] = halfHeight - vecC[1];
		vecD[0] += halfWidth;
		vecD[1] = halfHeight - vecD[1];
		//CABD
		SelectCyclerColor.GetColor(r, g, b, dummy);
 		DrawSPRIconPos(iSelectCyclerSpr, vecC, vecA, vecB, vecD, r, g, b, a*0.3);
		if (gWR.gridDrawMenu[i] < 0 || i == gWR.iNowSlot)
			continue;
		wp = gWR.GetWeapon(gWR.gridDrawMenu[i]);
		if (!wp)
			continue;
		xpos = (vecA[0] + vecB[0] + vecC[0] + vecD[0]) / 4;
		ypos = (vecA[1] + vecB[1] + vecC[1] + vecD[1]) / 4;
		iHeight = wp->rcActive.bottom - wp->rcActive.top;
		iWidth = wp->rcActive.right - wp->rcActive.left;

		if(gWR.HasAmmo(wp))
			SelectCyclerIconColor.GetColor(r, g, b, dummy);
		else
			SelectCyclerEmptyColor.GetColor(r, g, b, dummy);
		ColorCalcuAlpha(r, g, b, a);
		SPR_Set(wp->hInactive, r, g, b);
		SPR_DrawAdditive(0, xpos - iWidth/2, ypos- iHeight/2, &wp->rcInactive);
		wsprintfW(buf, L"%d/%d", wp->iClip, gWR.CountAmmo(wp->iAmmoType));
		GetStringSize(buf, &iTextWidth, NULL, HUDFont);
		SelectCyclerTextColor.GetColor(r, g, b, dummy);
		ColorCalcuAlpha(r, g, b, a);
		DrawVGUI2String(buf, xpos - iTextWidth/2, ypos + iHeight, r, g, b, HUDFont, true);
	}
	//绘制已选
	if (gWR.iNowSelect > -1)
	{
		wp = gWR.GetWeapon(gWR.iNowSelect);
		Vector2Copy(aryIn[gWR.iNowSlot == 9 ? 0 : gWR.iNowSlot + 1], vecA);
		Vector2Copy(aryIn[gWR.iNowSlot], vecB);
		Vector2Copy(aryOut[gWR.iNowSlot == 9 ? 0 : gWR.iNowSlot + 1], vecC);
		Vector2Copy(aryOut[gWR.iNowSlot], vecD);
		//变换为OpenGL屏幕坐标
		vecA[0] += halfWidth;
		vecA[1] = halfHeight - vecA[1];
		vecB[0] += halfWidth;
		vecB[1] = halfHeight - vecB[1];
		vecC[0] += halfWidth;
		vecC[1] = halfHeight - vecC[1];
		vecD[0] += halfWidth;
		vecD[1] = halfHeight - vecD[1];
		xpos = (vecA[0] + vecB[0] + vecC[0] + vecD[0]) / 4;
		ypos = (vecA[1] + vecB[1] + vecC[1] + vecD[1]) / 4;
		iHeight = wp->rcActive.bottom - wp->rcActive.top;
		iWidth = wp->rcActive.right - wp->rcActive.left;
		if (gWR.HasAmmo(wp))
			SelectCyclerIconColor.GetColor(r, g, b, dummy);
		else
			SelectCyclerEmptyColor.GetColor(r, g, b, dummy);
		ColorCalcuAlpha(r, g, b, a);
		SPR_Set(wp->hActive, r, g, b);
		SPR_DrawAdditive(0, xpos - iWidth / 2, ypos - iHeight / 2, &wp->rcActive);
		wsprintfW(buf, L"%d/%d", wp->iClip, gWR.CountAmmo(wp->iAmmoType));
		WEAPON* t = gHookHud.m_Ammo->m_pWeapon;
		GetStringSize(buf, &iTextWidth, NULL, HUDFont);
		SelectCyclerTextColor.GetColor(r, g, b, dummy);
		ColorCalcuAlpha(r, g, b, a);
		DrawVGUI2String(buf, xpos - iTextWidth / 2, ypos + iHeight, r, g, b, HUDFont,true);
		SelectCyclerRinColor.GetColor(r, g, b, dummy);
		DrawSPRIconPos(iSelectCyclerRinSpr, vecC, vecA, vecB, vecD, r, g, b, a);
	}
	//绘制完毕，修改展示状态
	m_bSelectMenuDisplay = true;
	return 1;
}