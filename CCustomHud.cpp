#include <metahook.h>
#include <string>
#include <map>

#include "glew.h"
#include "gl_shader.h"
#include "gl_def.h"

#include "pmtrace.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "pm_defs.h"
#include "parsemsg.h"
#include "mathlib.h"
#include "exportfuncs.h"

#include "local.h"
#include "gl_draw.h"

#include "CCustomHud.h"

#include "ammo.h"
#include "healthhud.h"
#include "deathmsg.h"
#include "radar.h"
#include "deathmsg.h"
#include "crosshair.h"
#include "playertitle.h"
#include "vote.h"
#include "eccomoney.h"
#include "efxhud.h"
#include "itemhighlight.h"
#include "eccobuymenu.h"
#include "grenadeindicator.h"

#ifdef _DEBUG
#include "cctv.h"
#endif

#include "weaponbank.h"
#include "wmenu_annular.h"

CCustomHud gCustomHud;
cl_hookedHud gHookHud;

pfnUserMsgHook m_pfnScoreInfo;
int __MsgFunc_ScoreInfo(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	//wtf is not this shit
	if (clientIndex >= 1 && clientIndex <= 33) {
		hud_playerinfo_t* info = gCustomHud.GetPlayerHUDInfo(clientIndex);
		info->index = clientIndex;
		info->frags = READ_FLOAT();
		info->death = READ_LONG();
		info->health = READ_FLOAT();
		info->armor = READ_FLOAT();
		info->team = READ_BYTE();
		info->donors = READ_CHAR();
		info->admin = READ_CHAR();
	}
	return m_pfnScoreInfo(pszName, iSize, pbuf);
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
void __UserCmd_Slot1(void) {
	m_HudCustomAmmo.SlotInput(0, 1);
	m_HudEccoBuyMenu.SlotCallBack(0);
	return UserCmd_Slot1();
}
void __UserCmd_Slot2(void) {
	m_HudCustomAmmo.SlotInput(1, 1);
	m_HudEccoBuyMenu.SlotCallBack(1);
	return UserCmd_Slot2();
}
void __UserCmd_Slot3(void) {
	m_HudCustomAmmo.SlotInput(2, 1);
	m_HudEccoBuyMenu.SlotCallBack(2);
	return UserCmd_Slot3();
}
void __UserCmd_Slot4(void) {
	m_HudCustomAmmo.SlotInput(3, 1);
	m_HudEccoBuyMenu.SlotCallBack(3);
	return UserCmd_Slot4();
}
void __UserCmd_Slot5(void) {
	m_HudCustomAmmo.SlotInput(4, 1);
	m_HudEccoBuyMenu.SlotCallBack(4);
	return UserCmd_Slot5();
}
void __UserCmd_Slot6(void) {
	m_HudCustomAmmo.SlotInput(5, 1);
	m_HudEccoBuyMenu.SlotCallBack(5);
	return UserCmd_Slot6();
}
void __UserCmd_Slot7(void) {
	m_HudCustomAmmo.SlotInput(6, 1);
	m_HudEccoBuyMenu.SlotCallBack(6);
	return UserCmd_Slot7();
}
void __UserCmd_Slot8(void) {
	m_HudCustomAmmo.SlotInput(7, 1);
	m_HudEccoBuyMenu.SlotCallBack(7);
	return UserCmd_Slot8();
}
void __UserCmd_Slot9(void) {
	m_HudCustomAmmo.SlotInput(8, 1);
	m_HudEccoBuyMenu.SlotCallBack(8);
	return UserCmd_Slot9();
}
void __UserCmd_Slot10(void) {
	m_HudCustomAmmo.SlotInput(9, 1);
	m_HudEccoBuyMenu.SlotCallBack(9);
	m_HudEccoBuyMenu.CloseMenu();
	return UserCmd_Slot10();
}
void __UserCmd_Close(void) {
	return UserCmd_SlotClose();
}
void __UserCmd_NextWeapon(void) {
	m_HudCustomAmmo.SlotInput(gWR.iNowSlot, 1, true);
	return UserCmd_NextWeapon();
}
void __UserCmd_PrevWeapon(void) {
	m_HudCustomAmmo.SlotInput(gWR.iNowSlot, -1, true);
	return UserCmd_PrevWeapon();
}
void __UserCmd_Attack1(void) {
	if (gCVars.pAmmoMenuStyle->value <= 0 && m_HudWMenuAnnular.m_bOpeningMenu) {
		m_HudWMenuAnnular.Select();
		return;
	}
	m_HudCustomAmmo.m_pNowSelectMenu->Select();
	if (m_HudEccoBuyMenu.SelectMenu())
		return;
	return UserCmd_Attack1();
}

void CCustomHud::GL_Init(void){
	m_HudRadar.GLInit();
	m_HudCustomAmmo.GLInit();
	m_HudArmorHealth.GLInit();
	m_HudEccoBuyMenu.GLInit();
#ifdef _DEBUG
	m_HudCCTV.GLInit();
#endif
}
void CCustomHud::HUD_Init(void){
	m_pfnScoreInfo = HOOK_MESSAGE(ScoreInfo);

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

	gCVars.pDynamicHUD = CREATE_CVAR("cl_hud_csgo", "1", FCVAR_VALUE, nullptr);

	m_HudArmorHealth.Init();
	m_HudCustomAmmo.Init();
	m_HudRadar.Init();
	m_HudDeathMsg.Init();
	m_HudCrosshair.Init();
	m_HudPlayerTitle.Init();
	m_HudVote.Init();
	m_HudEccoMoney.Init();
	m_HudEfx.Init();
	m_HudItemHighLight.Init();
	m_HudEccoBuyMenu.Init();
	m_HudGrenadeIndicator.Init();
#ifdef _DEBUG
	m_HudCCTV.Init();
#endif

	m_flCursorSize = GET_SCREEN_PIXEL(true, "Common.CursorSize");
}
void CCustomHud::HUD_VidInit(void){
	int iRes = ScreenWidth < 640 ? 320 : 640;
	if (!m_pSpriteList){
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList){
			client_sprite_t* p = m_pSpriteList;
			for (int j = 0; j < m_iSpriteCountAllRes; j++){
				if (p->iRes == iRes){
					char sz[256];
					sprintf(sz, "sprites/%s.spr", p->szSprite);
					cl_spritem_s* item = new cl_spritem_s();
					item->spr = SPR_Load(sz);
					item->rect = p->rc;
					strncpy_s(item->name, p->szName, MAX_SPRITE_NAME_LENGTH);
					m_arySprites.push_back(item);
				}
				p++;
			}
		}
	}
	else{
		client_sprite_t* p = m_pSpriteList;
		int index = 0;
		for (int j = 0; j < m_iSpriteCountAllRes; j++){
			if (p->iRes == iRes){
				char sz[256];
				sprintf(sz, "sprites/%s.spr", p->szSprite);
				m_arySprites[index]->spr = SPR_Load(sz);
				index++;
			}
			p++;
		}
	}
	m_HudArmorHealth.VidInit();
	m_HudCustomAmmo.VidInit();
	m_HudRadar.VidInit();
	m_HudVote.VidInit();
	m_HudEccoBuyMenu.VidInit();
}
void CCustomHud::HUD_Draw(float flTime){
	SetBaseHudActivity();
	if (!IsHudEnable())
		return;
	m_HudEfx.Draw(flTime);
	m_HudPlayerTitle.Draw(flTime);
	m_HudArmorHealth.Draw(flTime);
	m_HudRadar.Draw(flTime);
	m_HudCustomAmmo.Draw(flTime);
	m_HudDeathMsg.Draw(flTime);
	m_HudCrosshair.Draw(flTime);
	m_HudVote.Draw(flTime);
	m_HudEccoMoney.Draw(flTime);
	m_HudItemHighLight.Draw(flTime);
	m_HudEccoBuyMenu.Draw(flTime);
	m_HudGrenadeIndicator.Draw(flTime);
#ifdef _DEBUG
	m_HudCCTV.Draw(flTime);
#endif
}
void CCustomHud::HUD_Reset(void){
	m_iPlayerHealth = 100;
	m_flOverViewScale = 0;
	m_HudArmorHealth.Reset();
	m_HudCustomAmmo.Reset();
	m_HudRadar.Reset();
	m_HudDeathMsg.Reset();
	m_HudPlayerTitle.Reset();
	m_HudVote.Reset();
	m_HudEccoMoney.Reset();
	m_HudEfx.Reset();
	m_HudItemHighLight.Reset();
	m_HudEccoBuyMenu.Reset();
	m_HudGrenadeIndicator.Reset();
#ifdef _DEBUG
	m_HudCCTV.Reset();
#endif
	memset(m_Playerinfo, 0, sizeof(m_Playerinfo));
	VGUI_CREATE_NEWTGA_TEXTURE(m_iCursorTga, "abcenchance/tga/cursor");
}
void CCustomHud::HUD_UpdateClientData(client_data_t* cdata, float time){
	if (!IsHudEnable())
		return;
	m_iWeaponBits = cdata->iWeaponBits;
	m_bPlayerLongjump = mathlib::fatoi(gEngfuncs.PhysInfo_ValueForKey("slj"));
}
void CCustomHud::HUD_ClientMove(struct playermove_s* ppmove, qboolean server){
	if (!IsHudEnable())
		return;
	m_HudCustomAmmo.ClientMove(ppmove, server);
}
void CCustomHud::HUD_Clear(void){
	m_HudRadar.Clear();
	m_HudCustomAmmo.Clear();
	m_HudEccoBuyMenu.Clear();
	m_HudArmorHealth.Clear();
	m_HudGrenadeIndicator.Clear();
}
void CCustomHud::HUD_BlitRadarFramebuffer()
{
	m_HudRadar.BlitFramebuffer();
}
void CCustomHud::IN_MouseEvent(int mstate){
	if (!IsHudEnable())
		return;
}
int CCustomHud::HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding){
	if (!IsHudEnable())
		return 1;
	int result = 1;
	result &= m_HudVote.HUD_KeyEvent(eventcode, keynum, pszCurrentBinding);
	return result;
}
void CCustomHud::IN_Accumulate(void){
	if (!IsHudEnable())
		return;
	m_HudCustomAmmo.IN_Accumulate();
}
int CCustomHud::HUD_AddEntity(int type, cl_entity_s* ent, const char* modelname){
	if (!IsHudEnable())
		return true;
	bool result = true;
	result = result && m_HudEccoBuyMenu.AddEntity(type, ent, modelname);
	m_HudItemHighLight.AddEntity(type, ent, modelname);
	m_HudGrenadeIndicator.HUD_AddEntity(type, ent, modelname);
	return result;
}
void CCustomHud::CL_CreateMove(float frametime, usercmd_s* cmd, int active) {
}
bool CCustomHud::IsInSpectate() {
	return gEngfuncs.GetLocalPlayer()->curstate.iuser1 > 0;
}
bool CCustomHud::HasSuit() {
	return (m_iWeaponBits & (1 << WEAPON_SUIT)) != 0;
}
bool CCustomHud::IsHudHide(int HideToken) {
	return (m_iHideHUDDisplay & HideToken) != 0;
}
bool CCustomHud::IsHudEnable() {
	return gCVars.pDynamicHUD->value > 0;
}
void CCustomHud::SetBaseHudActivity() {
	if (IsHudEnable()) {
		if (gHookHud.m_Ammo)
			gHookHud.m_Ammo->m_iFlags &= ~HUD_ACTIVE;
		if (gHookHud.m_Battery)
			gHookHud.m_Battery->m_iFlags &= ~HUD_ACTIVE;
		if (gHookHud.m_Health)
			gHookHud.m_Health->m_iFlags &= ~HUD_ACTIVE;
	}
	else {
		if (gHookHud.m_Ammo)
			gHookHud.m_Ammo->m_iFlags |= HUD_ACTIVE;
		if (gHookHud.m_Battery)
			gHookHud.m_Battery->m_iFlags |= HUD_ACTIVE;
		if (gHookHud.m_Health)
			gHookHud.m_Health->m_iFlags |= HUD_ACTIVE;
	}
}

HSPRITE CCustomHud::GetSprite(size_t index) {
	return (index < 0) ? 0 : m_arySprites[index]->spr;
}
wrect_t* CCustomHud::GetSpriteRect(size_t index) {
	if(index >= 0 && index < m_arySprites.size())
		return &m_arySprites[index]->rect;
	return nullptr;
}
int CCustomHud::GetSpriteIndex(const char* SpriteName){
	for (size_t i = 0; i < m_arySprites.size(); i++){
		if (strncmp(SpriteName, m_arySprites[i]->name, MAX_SPRITE_NAME_LENGTH) == 0)
			return i;
	}
	return -1; // invalid sprite
}
hud_playerinfo_t* CCustomHud::GetPlayerHUDInfo(int index){
	if (index > 0 && index <= 33)
		return &m_Playerinfo[index];
	return nullptr;
}
vgui::ISurface* CCustomHud::surface(){
	return g_pSurface;
}
CCustomHud :: ~CCustomHud(){
	m_arySprites.clear();
}