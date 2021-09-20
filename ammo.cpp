#include <metahook.h>
#include "parsemsg.h"
#include "pm_shared.h"
#include "msghook.h"
#include "command.h"

#include <cmath>
#include <mathlib.h>

#include "hud.h"
#include "weapon.h"

#include "local.h"
#include "vguilocal.h"
#include "myconst.h"
#include "CHudDelegate.h"

#include "enginedef.h"
#include "exportfuncs.h"
#include "drawElement.h"
#include "utility.h"

#include "glew.h"
#include "gl_shader.h"
#include "gldef.h"
#include "glutility.h"

#include "weaponbank.h"
#include "historyresource.h"
#include "ammo.h"

CHudCustomAmmo m_HudCustomAmmo;
SHADER_DEFINE(pp_gaussianblurh);
SHADER_DEFINE(pp_gaussianblurv);

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
	if (m_HudCustomAmmo.m_fFade > gEngfuncs.GetClientTime()){
		if (m_HudCustomAmmo.m_bAcceptDeadMessage)
			return UserCmd_Attack1();
		m_HudCustomAmmo.ChosePlayerWeapon();
	}
	if (!m_HudCustomAmmo.m_bOpeningAnnularMenu) {
		gWR.iNowSlot = 0;
		m_HudCustomAmmo.m_fFade = 0;
		m_HudCustomAmmo.m_bSelectMenuDisplay = false;
	}
	return UserCmd_Attack1();
}
void __UserCmd_OpenAnnularMenu(void){
	if (!m_HudCustomAmmo.m_bOpeningAnnularMenu && !m_HudCustomAmmo.m_bSelectMenuDisplay) {
		if (m_HudCustomAmmo.m_fFade <= gEngfuncs.GetClientTime())
			gEngfuncs.pfnPlaySoundByName("common/wpn_hudon.wav", 1);
		m_HudCustomAmmo.m_bOpeningAnnularMenu = true;
		gHudDelegate->m_iVisibleMouse = true;
	}
}
void __UserCmd_CloseAnnularMenu(void){
	if (m_HudCustomAmmo.m_bOpeningAnnularMenu && m_HudCustomAmmo.m_bSelectMenuDisplay) {
		m_HudCustomAmmo.m_bOpeningAnnularMenu = false;
		m_HudCustomAmmo.m_fFade = 0;
		gHudDelegate->m_iVisibleMouse = false;
		m_HudCustomAmmo.m_bSetedCursor = false;
	}
}
void CustomSlotSetCallBack(cvar_t* vars){
	if (!vars->string || vars->string[0] == 0)
		return;
	int slot;
	sscanf_s(vars->name, "cl_customslot%d", &slot);
	slot--;
	gWR.SetUserSlot(slot, gWR.GetWeaponId(vars->string));
}

void CHudCustomAmmo::GLInit(){
	glGenFramebuffersEXT(1, &m_hGaussianBufferVFBO);
	m_hGaussianBufferVTex = GL_GenTextureRGB8(gScreenInfo.iWidth, gScreenInfo.iHeight);
	glGenFramebuffersEXT(1, &m_hGaussianBufferHFBO);
	m_hGaussianBufferHTex = GL_GenTextureRGB8(gScreenInfo.iWidth, gScreenInfo.iHeight);
}
int CHudCustomAmmo::Init(void){
	m_pfnCurWeapon = HOOK_MESSAGE(CurWeapon);
	m_pfnWeaponList = HOOK_MESSAGE(WeaponList);
	m_pfnCustWeapon = HOOK_MESSAGE(CustWeapon);
	m_pfnAmmoPickup = HOOK_MESSAGE(AmmoPickup);
	m_pfnWeapPickup = HOOK_MESSAGE(WeapPickup);
	m_pfnItemPickup = HOOK_MESSAGE(ItemPickup);
	m_pfnAmmoX = HOOK_MESSAGE(AmmoX);
	
	gEngfuncs.pfnAddCommand("+annularmenu", __UserCmd_OpenAnnularMenu);
	gEngfuncs.pfnAddCommand("-annularmenu", __UserCmd_CloseAnnularMenu);
	pp_gaussianblurh.program = R_CompileShaderFileEx("renderer\\shader\\fullscreentriangle.vert.glsl", "renderer\\shader\\gaussian_blur_16x.frag.glsl", "", "#define BLUR_HORIZONAL\n", NULL);
	if (pp_gaussianblurh.program)
		SHADER_UNIFORM(pp_gaussianblurh, du, "du");
	pp_gaussianblurv.program = R_CompileShaderFileEx("renderer\\shader\\fullscreentriangle.vert.glsl", "renderer\\shader\\gaussian_blur_16x.frag.glsl", "", "#define BLUR_VERTICAL\n", NULL);
	if (pp_gaussianblurv.program)
		SHADER_UNIFORM(pp_gaussianblurv, du, "du");
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

	SelectCyclerColor = pScheme->GetColor("AmmoHUD.SelectCyclerColor", gDefaultColor);
	SelectCyclerRinColor = pScheme->GetColor("AmmoHUD.SelectCyclerRinColor", gDefaultColor);
	SelectCyclerIconColor = pScheme->GetColor("AmmoHUD.SelectCyclerIconColor", gDefaultColor);
	SelectCyclerTextColor = pScheme->GetColor("AmmoHUD.SelectCyclerTextColor", gDefaultColor);
	SelectCyclerEmptyColor = pScheme->GetColor("AmmoHUD.SelectCyclerEmptyColor", gDefaultColor);
	SelectCyclerPointerColor = pScheme->GetColor("AmmoHUD.SelectCyclerPointerColor", gDefaultColor);

	SelectCyclerOffset = GET_SCREEN_PIXEL(true, "AmmoHUD.SelectCyclerOffset");
	SelectCyclerSize = GET_SCREEN_PIXEL(true, "AmmoHUD.SelectCyclerSize");
	SelectCyclerPointerSize = GET_SCREEN_PIXEL(true, "AmmoHUD.SelectCyclerPointerSize");
	SelectCyclerRotate = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerRotate"));
	SelectCyclerAnimateTime = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerAnimateTime"));
	SelectCyclerFadeTime = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerFadeTime"));
	SelectCyclerHoldTime = atof(pScheme->GetResourceString("AmmoHUD.SelectCyclerHoldTime"));

	if (SelectCyclerHoldTime <= 0)
		SelectCyclerHoldTime = 5;

	HUDFont = pScheme->GetFont("HUDShitFont", true);
	HUDSmallFont = pScheme->GetFont("HUDSmallShitFont", true);

	Reset();

	gWR.Init();
	gHR.Init();
	return 1;
};
void CHudCustomAmmo::Reset(void){
	m_fFade = 0;
	m_fAnimateTime = 0;
	m_fNextSyncTime = 0;
	m_pWeapon = NULL;
	m_bSelectMenuDisplay = false;
	m_bOpeningAnnularMenu = false;
	m_bIsOnTarget = false;
	m_bSetedCursor = false;
	iSelectCyclerSpr = gEngfuncs.pfnSPR_Load("abcenchance/spr/select_cycler.spr");
	iSelectCyclerRinSpr = gEngfuncs.pfnSPR_Load("abcenchance/spr/selected_rin.spr");
	iSelectCyclerCursorPointer = gEngfuncs.pfnSPR_Load("abcenchance/spr/select_pointer.spr");
	VGUI_CREATE_NEWTGA_TEXTURE(iBackGroundTga, "abcenchance/tga/ammobar_background");
	gWR.Reset();
	gHR.Reset();
}
int CHudCustomAmmo::VidInit(void){
	gWR.LoadAllWeaponSprites();
	return 1;
}
void CHudCustomAmmo::DrawScreenQuad(){
	glColor4ub(255, 255, 255, 255);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(1, 0);
	glVertex2f(gScreenInfo.iWidth, 0);
	glTexCoord2f(1, 1);
	glVertex2f(gScreenInfo.iWidth, gScreenInfo.iHeight);
	glTexCoord2f(0, 1);
	glVertex2f(0, gScreenInfo.iHeight);
	glEnd();
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
		gEngfuncs.pfnPlaySoundByName("common/wpn_select.wav", 1);
		gWR.iNowSlot = -1;
	}
}
void CHudCustomAmmo::SlotInput(int iSlot, int fAdvance){
	if (gHookHud.m_Menu->m_fMenuDisplayed)
		return;
	gWR.SelectSlot(iSlot, fAdvance);
}
void CHudCustomAmmo::DrawSelectIcon(WEAPON* wp, int a, int xpos, int ypos, int index){
	wchar_t buf[64];
	int iTextWidth;
	int r, g, b, dummy;
	float h, s, v;
	int iHeight = wp->rcActive.bottom - wp->rcActive.top;
	int iWidth = wp->rcActive.right - wp->rcActive.left;
	if (gWR.HasAmmo(wp) && gWR.HasWeapon(wp))
		SelectCyclerIconColor.GetColor(r, g, b, dummy);
	else
		SelectCyclerEmptyColor.GetColor(r, g, b, dummy);
	if (gCVars.pAmmoMenuDrawRainbow->value > 0) {
		RGBToHSV(r, g, b, h, s, v);
		h += 36 * index + 120;
		HSVToRGB(h, s, v, r, g, b);
	}
	ColorCalcuAlpha(r, g, b, a);
	SPR_Set(wp->hInactive, r, g, b);
	SPR_DrawAdditive(0, xpos - iWidth / 2, ypos - iHeight / 2, &wp->rcInactive);
	SelectCyclerTextColor.GetColor(r, g, b, dummy);
	if (gCVars.pAmmoMenuDrawRainbow->value > 0) {
		RGBToHSV(r, g, b, h, s, v);
		h += 36 * index + 120;
		HSVToRGB(h, s, v, r, g, b);
	}
	ColorCalcuAlpha(r, g, b, a);
	if (gCVars.pAmmoMenuDrawPos->value > 0) {
		wsprintfW(buf, L"·%d", wp->iSlotPos);
		GetStringSize(buf, &iTextWidth, NULL, HUDSmallFont);
		DrawVGUI2String(buf, xpos - iTextWidth - iWidth / 2, ypos - iHeight / 2, r, g, b, HUDSmallFont, true);
	}

	if (wp->iAmmoType >= 0) {
		if(wp->iClip >= 0)
			wsprintfW(buf, L"%d/%d", wp->iClip, gWR.CountAmmo(wp->iAmmoType));
		else
			wsprintfW(buf, L"%d", gWR.CountAmmo(wp->iAmmoType));
	}
	else
		wsprintfW(buf, L"");
	GetStringSize(buf, &iTextWidth, NULL, HUDFont);
	DrawVGUI2String(buf, xpos - iTextWidth / 2, ypos + iHeight, r, g, b, HUDFont, true);
}
int CHudCustomAmmo::DrawWList(float flTime){
	if (m_fFade <= flTime){
		if (m_bSelectMenuDisplay){
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
	if(!iSelectCyclerCursorPointer)
		iSelectCyclerCursorPointer = gEngfuncs.pfnSPR_Load("abcenchance/spr/select_pointer.spr");
	float flTimeDiffer = m_fFade - flTime;
	float flStartRot = SelectCyclerRotate;
	int iBackGroundHeight = SelectCyclerSize;
	int iOffset = SelectCyclerOffset;
	float flAnimationRatio = 1.0f;
	if (!m_bSelectMenuDisplay)
		m_fAnimateTime = flTime + SelectCyclerAnimateTime;
	if (m_fAnimateTime > flTime && SelectCyclerHoldTime > SelectCyclerAnimateTime) {
		flAnimationRatio = 1 - ((m_fAnimateTime - flTime) / SelectCyclerAnimateTime);
		iOffset *= flAnimationRatio;
		flAnimationRatio /= 100;
	}
	int i;
	float ac, as;
	vec2_t aryOut[10];
	vec2_t aryIn[10];
	WEAPON* wp;
	int halfWidth = gScreenInfo.iWidth / 2;
	int halfHeight = gScreenInfo.iHeight / 2;
	int xpos;
	int ypos;
	vec2_t vecA, vecB, vecC, vecD;
	int a = 255;
	int r, g, b, dummy;
	//填充十边形坐标数组
	for (i = 0; i < 10; i++){
		ac = cos(2 * M_PI * i / 10 + flStartRot);
		as = sin(2 * M_PI * i / 10 + flStartRot);

		aryIn[i][0] = iOffset * ac;
		aryIn[i][1] = iOffset * as;

		aryOut[i][0] = (iOffset + iBackGroundHeight) * ac;
		aryOut[i][1] = (iOffset + iBackGroundHeight) * as;
	}
	
	if (flTimeDiffer < SelectCyclerFadeTime)
		a *= flTimeDiffer / SelectCyclerFadeTime;

	if (m_HudCustomAmmo.m_bOpeningAnnularMenu) {
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_hOldBuffer);
		//复制tex到H
		glBindFramebuffer(GL_FRAMEBUFFER, m_hGaussianBufferHFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hGaussianBufferHTex, 0);
		GL_BlitFrameBufferToFrameBufferColorOnly(m_hOldBuffer, m_hGaussianBufferHFBO,
			gScreenInfo.iWidth, gScreenInfo.iHeight, gScreenInfo.iWidth, gScreenInfo.iHeight);
		//绘制到V
		glBindFramebuffer(GL_FRAMEBUFFER, m_hGaussianBufferVFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hGaussianBufferVTex, 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_hGaussianBufferHTex);
		//H模糊
		GL_UseProgram(pp_gaussianblurh.program);
		glUniform1f(0, 1.0f / gScreenInfo.iWidth);
		glUniform1f(pp_gaussianblurh.du, 2.0f / gScreenInfo.iWidth * flAnimationRatio);
		glColor4ub(255, 255, 255, 255);
		DrawScreenQuad();
		//绘制到主画布
		glBindTexture(GL_TEXTURE_2D, m_hGaussianBufferVTex);
		glBindFramebuffer(GL_FRAMEBUFFER, m_hOldBuffer);
		GL_UseProgram(pp_gaussianblurv.program);
		glUniform1f(0, 1.0f / gScreenInfo.iHeight);
		glUniform1f(pp_gaussianblurh.du, 2.0f / gScreenInfo.iHeight * flAnimationRatio);
		DrawScreenQuad();
		GL_UseProgram(0);
		//绘制鼠标指针
		SelectCyclerPointerColor.GetColor(r, g, b, dummy);
		ac = cos(m_fCursorAngle - 0.45 * M_PI);
		as = sin(m_fCursorAngle - 0.45 * M_PI);
		vecA[0] = -SelectCyclerPointerSize * ac - (iOffset - SelectCyclerPointerSize) * as;
		vecA[1] = -SelectCyclerPointerSize * as + (iOffset - SelectCyclerPointerSize) * ac;
		vecB[0] = SelectCyclerPointerSize * ac - (iOffset - SelectCyclerPointerSize) * as;
		vecB[1] = SelectCyclerPointerSize * as + (iOffset - SelectCyclerPointerSize) * ac;
		vecC[0] = -SelectCyclerPointerSize * ac - iOffset * as;
		vecC[1] = -SelectCyclerPointerSize * as + iOffset * ac;
		vecD[0] = SelectCyclerPointerSize * ac - iOffset * as;
		vecD[1] = SelectCyclerPointerSize * as + iOffset * ac;
		vecA[0] += halfWidth;
		vecA[1] = halfHeight - vecA[1];
		vecB[0] += halfWidth;
		vecB[1] = halfHeight - vecB[1];
		vecC[0] += halfWidth;
		vecC[1] = halfHeight - vecC[1];
		vecD[0] += halfWidth;
		vecD[1] = halfHeight - vecD[1];
		DrawSPRIconPos(iSelectCyclerCursorPointer, vecC, vecA, vecB, vecD, r, g, b, a);
	}
	//绘制十边形
	for (i = 0; i < 10; i++){
		//CABD
		//↓→↑
		Vector2Copy(aryIn[i == 9 ? 0 :i + 1], vecA);
		Vector2Copy(aryIn[i], vecB);
		Vector2Copy(aryOut[i == 9 ? 0 : i + 1], vecC);
		Vector2Copy(aryOut[i], vecD);
		//变换为OpenGL屏幕坐标
		CenterPos2OpenGLPos(vecA, halfWidth, halfHeight);
		CenterPos2OpenGLPos(vecB, halfWidth, halfHeight);
		CenterPos2OpenGLPos(vecC, halfWidth, halfHeight);
		CenterPos2OpenGLPos(vecD, halfWidth, halfHeight);
		//CABD
		SelectCyclerColor.GetColor(r, g, b, dummy);
		if (gCVars.pAmmoMenuDrawRainbow->value > 1) {
			float h, s, v;
			RGBToHSV(r, g, b, h, s, v);
			h += 36 * i;
			s = 1;
			HSVToRGB(h, s, v, r, g, b);
		}
		DrawSPRIconPos(iSelectCyclerSpr, vecC, vecA, vecB, vecD, r, g, b, a * 0.5);
		if (gWR.gridDrawMenu[i].iId <= 0 || i == gWR.iNowSlot)
			continue;
		wp = gWR.GetWeapon(gWR.gridDrawMenu[i].iId);
		if (!wp)
			continue;
		xpos = (vecA[0] + vecB[0] + vecC[0] + vecD[0]) / 4;
		ypos = (vecA[1] + vecB[1] + vecC[1] + vecD[1]) / 4;
		DrawSelectIcon(wp, a, xpos, ypos, i);
	}
	//绘制已选
	if (gWR.gridDrawMenu[gWR.iNowSlot].iId > -1 && gWR.iNowSlot >= 0){
		wp = gWR.GetWeapon(gWR.gridDrawMenu[gWR.iNowSlot].iId);
		Vector2Copy(aryIn[gWR.iNowSlot == 9 ? 0 : gWR.iNowSlot + 1], vecA);
		Vector2Copy(aryIn[gWR.iNowSlot], vecB);
		Vector2Copy(aryOut[gWR.iNowSlot == 9 ? 0 : gWR.iNowSlot + 1], vecC);
		Vector2Copy(aryOut[gWR.iNowSlot], vecD);
		CenterPos2OpenGLPos(vecA, halfWidth, halfHeight);
		CenterPos2OpenGLPos(vecB, halfWidth, halfHeight);
		CenterPos2OpenGLPos(vecC, halfWidth, halfHeight);
		CenterPos2OpenGLPos(vecD, halfWidth, halfHeight);
		xpos = (vecA[0] + vecB[0] + vecC[0] + vecD[0]) / 4;
		ypos = (vecA[1] + vecB[1] + vecC[1] + vecD[1]) / 4;
		DrawSelectIcon(wp, a, xpos, ypos, gWR.iNowSlot);
		SelectCyclerRinColor.GetColor(r, g, b, dummy);
		DrawSPRIconPos(iSelectCyclerRinSpr, vecC, vecA, vecB, vecD, r, g, b, a);
	}
	//绘制完毕，修改展示状态
	m_bSelectMenuDisplay = true;
	return 1;
}
void CHudCustomAmmo::ClientMove(struct playermove_s* ppmove, qboolean server){
	if (m_bOpeningAnnularMenu)
		m_fFade = gEngfuncs.GetClientTime() + SelectCyclerHoldTime;
	//0.5s 进行一次武器同步
	if (gEngfuncs.GetClientTime() > m_fNextSyncTime) {
		SyncWeapon();
		m_fNextSyncTime = gEngfuncs.GetClientTime() + 0.5F;
	}
}
void CHudCustomAmmo::IN_Accumulate(){
	if (m_bOpeningAnnularMenu) {
		int x, y;
		gEngfuncs.GetMousePosition(&x, &y);
		if (!m_bSetedCursor) {
			gEngfuncs.pfnSetMousePos(gScreenInfo.iWidth / 2, gScreenInfo.iHeight / 2);
			m_bSetedCursor = true;
		}
		x -= gScreenInfo.iWidth / 2;
		y -= gScreenInfo.iHeight / 2;
		y = -y;
		m_fCursorAngle = atan2(y, x);
		int s = m_fCursorAngle / (0.2 * M_PI);
		s = m_fCursorAngle >= 0 ? s : 9 + s;
		if (gWR.gridDrawMenu[s].iId > -1)
			gWR.iNowSlot = s;
	}
}
void CHudCustomAmmo::Clear(){
	if (m_hGaussianBufferVTex)
		glDeleteTextures(1, &m_hGaussianBufferVTex);
	if (m_hGaussianBufferHTex)
		glDeleteTextures(1, &m_hGaussianBufferHTex);
}