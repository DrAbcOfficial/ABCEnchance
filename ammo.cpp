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

#include "hud.h"
#include "weapon.h"

#include "vguilocal.h"
#include "CHudDelegate.h"

#include "exportfuncs.h"
#include <drawElement.h>

#include "weaponbank.h"
#include "historyresource.h"
#include "ammo.h"

WEAPON* gpActiveSel;	// NULL means off, 1 means just the menu bar, otherwise
						// this points to the active weapon menu item
WEAPON* gpLastSel;		// Last weapon menu selection 
CHudCustomAmmo m_HudCustomAmmo;

int __MsgFunc_AmmoX(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();
	gWR.SetAmmo(iIndex, abs(iCount));
	return 1;
}

int __MsgFunc_AmmoPickup(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	gHR.AddToHistory(HISTSLOT_AMMO, iIndex, abs(iCount));
	return 1;
}

int __MsgFunc_WeapPickup(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_SHORT();

	gHR.AddToHistory(HISTSLOT_WEAP, iIndex);
	return 1;
}

int __MsgFunc_ItemPickup(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	const char* szName = READ_STRING();
	gHR.AddToHistory(HISTSLOT_ITEM, szName);
	return 1;
}


int __MsgFunc_HideWeapon(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	READ_BYTE();
	if (gEngfuncs.IsSpectateOnly())
		return 1;

	if (m_HudCustomAmmo.m_pWeapon)
		SetCrosshair(m_HudCustomAmmo.m_pWeapon->hCrosshair, m_HudCustomAmmo.m_pWeapon->rcCrosshair, 255, 255, 255);

	return 1;
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

	gWR.AddWeapon(&Weapon);
	return 1;

}

int __MsgFunc_CustWeapon(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	char name[128];
	strcpy_s(name, READ_STRING());
	return 1;

}

int __MsgFunc_WeaponSpr(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	char path[2048];
	strcpy_s(path, READ_STRING());
	return 1;

}

int __MsgFunc_CurWeapon(const char* pszName, int iSize, void* pbuf)
{
	int fOnTarget = FALSE;

	BEGIN_READ(pbuf, iSize);

	int iState = READ_BYTE();
	int iId = READ_SHORT();
	int iClip = READ_LONG();
	int iClip2 = READ_LONG();

	// detect if we're also on target
	if (iState > 1)
		fOnTarget = TRUE;
	if (iId == -1 && iClip == -1)
	{
		gHudDelegate->m_fPlayerDead = TRUE;
		gpActiveSel = NULL;
		return 1;
	}
	gHudDelegate->m_fPlayerDead = FALSE;

	WEAPON* pWeapon = gWR.GetWeapon(iId);
	if (!pWeapon)
		return 0;
	if (iState == 0)	// we're not the current weapon, so update no more
		return 1;
	//¸üÐÂµ¯Ï»ÐÅÏ¢
	pWeapon->iClip = iClip;
	pWeapon->iClip2 = iClip2;
	m_HudCustomAmmo.m_pWeapon = pWeapon;
	int def_fov = gEngfuncs.pfnGetCvarFloat("default_fov");
	if (gClientData->fov >= def_fov)
	{ // normal crosshairs
		if (fOnTarget && m_HudCustomAmmo.m_pWeapon->hAutoaim)
			SetCrosshair(m_HudCustomAmmo.m_pWeapon->hAutoaim, m_HudCustomAmmo.m_pWeapon->rcAutoaim, 255, 255, 255);
		else
			SetCrosshair(m_HudCustomAmmo.m_pWeapon->hCrosshair, m_HudCustomAmmo.m_pWeapon->rcCrosshair, 255, 255, 255);
	}
	else
	{ // zoomed crosshairs
		if (fOnTarget && m_HudCustomAmmo.m_pWeapon->hZoomedAutoaim)
			SetCrosshair(m_HudCustomAmmo.m_pWeapon->hZoomedAutoaim, m_HudCustomAmmo.m_pWeapon->rcZoomedAutoaim, 255, 255, 255);
		else
			SetCrosshair(m_HudCustomAmmo.m_pWeapon->hZoomedCrosshair, m_HudCustomAmmo.m_pWeapon->rcZoomedCrosshair, 255, 255, 255);

	}
	m_HudCustomAmmo.m_fFade = 200.0f; //!!!
	m_HudCustomAmmo.m_iFlags |= HUD_ACTIVE;
	return 1;
}

void __UserCmd_Slot1(void)
{
	m_HudCustomAmmo.SlotInput(0);
}

void __UserCmd_Slot2(void)
{
	m_HudCustomAmmo.SlotInput(1);
}

void __UserCmd_Slot3(void)
{
	m_HudCustomAmmo.SlotInput(2);
}

void __UserCmd_Slot4(void)
{
	m_HudCustomAmmo.SlotInput(3);
}

void __UserCmd_Slot5(void)
{
	m_HudCustomAmmo.SlotInput(4);
}

void __UserCmd_Slot6(void)
{
	m_HudCustomAmmo.SlotInput(5);
}

void __UserCmd_Slot7(void)
{
	m_HudCustomAmmo.SlotInput(6);
}

void __UserCmd_Slot8(void)
{
	m_HudCustomAmmo.SlotInput(7);
}

void __UserCmd_Slot9(void)
{
	m_HudCustomAmmo.SlotInput(8);
}

void __UserCmd_Slot10(void)
{
	m_HudCustomAmmo.SlotInput(9);
}

void __UserCmd_Close(void)
{
	if (gpActiveSel)
	{
		gpLastSel = gpActiveSel;
		gpActiveSel = NULL;
		//PlaySound("common/wpn_hudoff.wav", 1);
	}
	else
		EngineClientCmd("escape");
}

int CHudCustomAmmo::Init(void)
{
	HOOK_MESSAGE(CurWeapon);
	HOOK_MESSAGE(WeaponList);
	HOOK_MESSAGE(CustWeapon);
	HOOK_MESSAGE(AmmoPickup);
	HOOK_MESSAGE(WeapPickup);
	HOOK_MESSAGE(ItemPickup);
	HOOK_MESSAGE(AmmoX);
	HOOK_MESSAGE(WeaponSpr);
	

	HOOK_COMMAND("slot1", Slot1);
	HOOK_COMMAND("slot2", Slot2);
	HOOK_COMMAND("slot3", Slot3);
	HOOK_COMMAND("slot4", Slot4);
	HOOK_COMMAND("slot5", Slot5);
	HOOK_COMMAND("slot6", Slot6);
	HOOK_COMMAND("slot7", Slot7);
	HOOK_COMMAND("slot8", Slot8);
	HOOK_COMMAND("slot9", Slot9);
	HOOK_COMMAND("slot10", Slot10);
	HOOK_COMMAND("cancelselect", Close);

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

	HUDFont = pScheme->GetFont("HUDShitFont", true);
	HUDSmallFont = pScheme->GetFont("HUDSmallShitFont", true);

	Reset();

	gWR.Init();
	gHR.Init();
	m_iFlags |= HUD_ACTIVE; //!!!
	return 1;
};

void CHudCustomAmmo::Reset(void)
{
	m_fFade = 0;
	m_iFlags |= HUD_ACTIVE; //!!!

	gpActiveSel = NULL;

	gWR.Reset();
	gHR.Reset();
}

int CHudCustomAmmo::VidInit(void)
{
	// If we've already loaded weapons, let's get new sprites
	gWR.LoadAllWeaponSprites();
	return 1;
}
void CHudCustomAmmo::Think(void)
{
	//gHookHud.m_Ammo->Think();
}
void CHudCustomAmmo::SlotInput(int iSlot)
{
	//gHookHud.m_Ammo->SlotInput(iSlot);
}

int CHudCustomAmmo::Draw(float flTime)
{
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
		//²ÙÄãÂè£¬ÍÂÁË£¬»ÙÃð°É£¬ÀÛÁË
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
int CHudCustomAmmo::DrawWList(float flTime)
{
	//return gHookHud.m_Ammo->DrawWList(flTime);
	return 1;

}