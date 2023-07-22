#include <metahook.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include "glew.h"
#include "gl_shader.h"
#include "gl_def.h"

#include "pmtrace.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "pm_defs.h"
#include "parsemsg.h"
#include "mymathlib.h"
#include "exportfuncs.h"
#include "keydefs.h"

#include "local.h"
#include "gl_draw.h"
#include "steam_api.h"
#include "player_info.h"
#include "svc_hook.h"

#include "basehud.h"

#include "CCustomHud.h"

#include "ammo.h"
#include "indicator.h"
#include "deathmsg.h"
#include "radar.h"
#include "deathmsg.h"
#include "itemhighlight.h"
#include "eccobuymenu.h"

#include "vgui_controls/Controls.h"

#include "scoreboard.h"
#include "Viewport.h"

#ifdef _DEBUG
#include "cctv.h"
#endif

#include "weaponbank.h"
#include <CVector.h>


CCustomHud gCustomHud;
cl_hookedHud gHookHud;

pfnUserMsgHook m_pfnHealth;
pfnUserMsgHook m_pfnScoreInfo;
pfnUserMsgHook m_pfnSpectator;
pfnUserMsgHook m_pfnServerName;
pfnUserMsgHook m_pfnNextMap;
pfnUserMsgHook m_pfnTimeEnd;
pfnUserMsgHook m_pfnShowMenu;
pfnUserMsgHook m_pfnVoteMenu;
pfnUserMsgHook m_pfnEndVote;
pfnUserMsgHook m_pfnMOTD;
pfnUserMsgHook m_pfnFlashBat;
pfnUserMsgHook m_pfnFlashlight;
pfnUserMsgHook m_pfnTextMsg;
pfnUserMsgHook m_pfnMetaHook;
pfnUserMsgHook m_pfnDamage;
pfnUserMsgHook m_pfnBattery;

int __MsgFunc_Damage(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int armor = READ_BYTE();
	int damageTaken = READ_BYTE();
	int tiles = READ_LONG();
	vec3_t vecFrom;
	for (size_t i = 0; i < 3; i++) {
		vecFrom[i] = READ_COORD();
	}
	if(damageTaken > 0 || armor > 0)
		m_HudIndicator.AddIdicator(damageTaken, armor, vecFrom);
	g_pViewPort->UpdateTiles(tiles);
	return m_pfnDamage(pszName, iSize, pbuf);
}
int __MsgFunc_Battery(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int battery = READ_SHORT();
	g_pViewPort->SetArmor(battery);
	return m_pfnBattery(pszName, iSize, pbuf);
}
int __MsgFunc_Health(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int health = READ_LONG();
	g_pViewPort->SetHealth(health);
	return m_pfnHealth(pszName, iSize, pbuf);
}
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
		info->donors = READ_SHORT();
		info->admin = READ_SHORT();
	}
	GetPlayerInfo(clientIndex)->Update();
	return m_pfnScoreInfo(pszName, iSize, pbuf);
}
int __MsgFunc_Spectator(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	if (clientIndex - 1 < 32) {
		int beSpectator = READ_BYTE();
		gCustomHud.SetSpectator(clientIndex, beSpectator != 0);
	}
	return m_pfnSpectator(pszName, iSize, pbuf);
}
int __MsgFunc_ServerName(const char* pszName, int iSize, void* pbuf) {
	if (g_pViewPort)
	{
		BEGIN_READ(pbuf, iSize);
		char buf[MAX_SERVERNAME_LENGTH];
		strncpy_s(buf, READ_STRING(), MAX_SERVERNAME_LENGTH);
		snprintf(g_pViewPort->GetServerName(), MAX_SERVERNAME_LENGTH, "%s", buf);
		g_pViewPort->GetScoreBoard()->UpdateServerName();
	}
	return m_pfnServerName(pszName, iSize, pbuf);
}
int __MsgFunc_NextMap(const char* pszName, int iSize, void* pbuf) {
	if (g_pViewPort)
	{
		BEGIN_READ(pbuf, iSize);
		char buf[MAX_SERVERNAME_LENGTH];
		strncpy_s(buf, READ_STRING(), MAX_SERVERNAME_LENGTH);
		snprintf(g_pViewPort->GetNextMap(), MAX_SERVERNAME_LENGTH, "%s", buf);
		g_pViewPort->GetScoreBoard()->UpdateNextMap();
	}
	return m_pfnNextMap(pszName, iSize, pbuf);
}
int __MsgFunc_TimeEnd(const char* pszName, int iSize, void* pbuf) {
	
	if (g_pViewPort)
	{
		BEGIN_READ(pbuf, iSize);
		g_pViewPort->m_iTimeEnd = READ_LONG();
		g_pViewPort->GetScoreBoard()->UpdateTimeEnd();
	}
	return m_pfnTimeEnd(pszName, iSize, pbuf);
}
int __MsgFunc_ShowMenu(const char* pszName, int iSize, void* pbuf){
	//block hahahaha
	return g_pViewPort->MsgShowMenu(pszName, iSize, pbuf);
}
int __MsgFunc_VoteMenu(const char* pszName, int iSize, void* pbuf) {
	if (!g_pViewPort->IsVoteEnable())
		return m_pfnVoteMenu(pszName, iSize, pbuf);

	BEGIN_READ(pbuf, iSize);
	int iVoteType = READ_BYTE();
	char szContent[2048];
	char szYes[2048];
	char szNo[2048];
	Q_strcpy(szContent, READ_STRING());
	Q_strcpy(szYes, READ_STRING());
	Q_strcpy(szNo, READ_STRING());
	g_pViewPort->StartVote(szContent, szYes, szNo, iVoteType);
	return 1;
}
int __MsgFunc_EndVote(const char* pszName, int iSize, void* pbuf) {
	g_pViewPort->EndVote();
	return m_pfnEndVote(pszName, iSize, pbuf);
}
int __MsgFunc_MOTD(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int code = READ_BYTE();
	char* msg = READ_STRING();
	if(msg[0] != '\0')
		g_pViewPort->AppendMOTD(msg);
	else
		g_pViewPort->CloseMOTD();
	if (code > 0)
		g_pViewPort->ForeceBuildPage();
	return m_pfnMOTD(pszName, iSize, pbuf);
}
int __MsgFunc_FlashBat(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	g_pViewPort->SetFlashBattery(READ_BYTE());
	return m_pfnFlashBat(pszName, iSize, pbuf);
}
int __MsgFunc_Flashlight(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int on = READ_BYTE();
	int battery = READ_BYTE();
	g_pViewPort->SetFlashLight(on, battery);
	return m_pfnFlashlight(pszName, iSize, pbuf);
}
int __MsgFunc_TextMsg(const char* pszName, int iSize, void* pbuf) {
	if (!gCustomHud.TextDeathMsg(pszName, iSize, pbuf)) {
		BEGIN_READ(pbuf, iSize);
		CViewport::HUDNOTICE msg_dest = static_cast<CViewport::HUDNOTICE>(READ_BYTE());
#define BUFFER_SIZE 256
		static auto findLocalize = [](char* str, char* outbuffer) {
			if (str[0] == '#')
				Q_UnicodeToUTF8(vgui::localize()->Find(str), outbuffer, BUFFER_SIZE);
			else
				Q_strcpy(outbuffer, str);
		};
		int type = 0;
		char msg[BUFFER_SIZE];
		findLocalize(READ_STRING(), msg);
		char sstr1[BUFFER_SIZE];
		findLocalize(READ_STRING(), sstr1);
		if (strlen(sstr1) <= 0)
			type++;
		char sstr2[BUFFER_SIZE];
		findLocalize(READ_STRING(), sstr2);
		if (strlen(sstr2) <= 0)
			type++;
		char sstr3[BUFFER_SIZE];
		findLocalize(READ_STRING(), sstr3);
		if (strlen(sstr3) <= 0)
			type++;
		char sstr4[BUFFER_SIZE];
		findLocalize(READ_STRING(), sstr4);
		if (strlen(sstr4) <= 0)
			type++;
		char buffer[BUFFER_SIZE * 4];
#undef BUFFER_SIZE
		std::string szBuf;
		switch (type) {
		case 1:sprintf_s(buffer, msg, sstr1); szBuf = buffer; break;
		case 2:sprintf_s(buffer, msg, sstr1, sstr2); szBuf = buffer; break;
		case 3:sprintf_s(buffer, msg, sstr1, sstr2, sstr3); szBuf = buffer; break;
		case 4:sprintf_s(buffer, msg, sstr1, sstr2, sstr3, sstr4); szBuf = buffer; break;
		case 0:
		default:szBuf = msg; break;
		}
		if (szBuf.back() == '\n')
			szBuf.pop_back();
		std::replace(szBuf.begin(), szBuf.end(), '\r', '\n');

		switch (msg_dest)
		{
		case CViewport::HUDNOTICE::PRINTNOTIFY:
		case CViewport::HUDNOTICE::PRINTCENTER:
			if (g_pViewPort)
				g_pViewPort->ShowNotice(msg_dest, szBuf.c_str());
			break;
		default:
			return m_pfnTextMsg(pszName, iSize, pbuf);
		}
	}
	return 1;
}
int __MsgFunc_MetaHook(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int type = READ_BYTE();
	switch (type) {
	case CCustomHud::MetaHookMsgType::MHSV_CMD_ECCO_INFO: {
		buymenuitem_t item;
		item.id = READ_LONG();
		item.price = READ_LONG();
		item.modelindex = READ_LONG();
		item.sequence = READ_LONG();
		strcpy_s(item.name, READ_STRING());
		m_HudEccoBuyMenu.AddInfo(item);
		break;
	}
	case CCustomHud::MetaHookMsgType::MHSV_CMD_ECCO_MENU: {
		m_HudEccoBuyMenu.MenuList.clear();
		size_t pageLen = (size_t)READ_BYTE();
		m_HudEccoBuyMenu.MenuList.resize(pageLen);
		for (size_t i = 0; i < pageLen; i++) {
			m_HudEccoBuyMenu.MenuList[i] = READ_LONG();
		}
		m_HudEccoBuyMenu.OpenMenu();
		break;
	}
	case CCustomHud::MetaHookMsgType::MHSV_CMD_ABC_CUSTOM: {
		CCustomHud::ABCCustomMsg type = static_cast<CCustomHud::ABCCustomMsg>(READ_BYTE());
		switch (type) {
		case CCustomHud::ABCCustomMsg::POPNUMBER: {
			if (g_pViewPort->m_pPopNumber->value <= 0)
				return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
			CVector vecOrigin = { READ_COORD(), READ_COORD(), READ_COORD() };
			int iValue = READ_LONG();
			Color pColor = { READ_BYTE(), READ_BYTE() , READ_BYTE() ,READ_BYTE() };
			cl_entity_t* local = gEngfuncs.GetLocalPlayer();
			if (!local)
				return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
			//视角角度
			CVector vecView;
			gEngfuncs.GetViewAngles(vecView);
			mathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
			//计算我和目标的相对偏移
			CVector vecLength;
			mathlib::VectorSubtract(vecOrigin, local->curstate.origin, vecLength);
			vecLength = vecLength.Normalize();
			float angledotResult = mathlib::DotProduct(vecLength, vecView);
			//cos 60
			if (angledotResult > 0.5)
				g_pViewPort->AddPopNumber(vecOrigin, pColor, iValue);
			return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
		}
		}
		break;
	}
	default:break;
	}
	return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
}

void(*UserCmd_Slots[10])(void);
void(*UserCmd_NextWeapon)(void);
void(*UserCmd_PrevWeapon)(void);
void(*UserCmd_ShowScores)(void);
void(*UserCmd_HideScores)(void);
void(*UserCmd_Attack1)(void);
void(*UserCmd_MissionBrief)(void);

void UserCmd_SlotInput(int i) {
	bool bVisible = gCustomHud.IsTextMenuOpening();
	m_HudCustomAmmo.SlotInput(i, 1);
	m_HudEccoBuyMenu.SlotCallBack(i);
	if(!bVisible)
		UserCmd_Slots[i]();
}
void __UserCmd_Slot1(void) {
	UserCmd_SlotInput(0);
}
void __UserCmd_Slot2(void) {
	UserCmd_SlotInput(1);
}
void __UserCmd_Slot3(void) {
	UserCmd_SlotInput(2);
}
void __UserCmd_Slot4(void) {
	UserCmd_SlotInput(3);
}
void __UserCmd_Slot5(void) {
	UserCmd_SlotInput(4);
}
void __UserCmd_Slot6(void) {
	UserCmd_SlotInput(5);
}
void __UserCmd_Slot7(void) {
	UserCmd_SlotInput(6);
}
void __UserCmd_Slot8(void) {
	UserCmd_SlotInput(7);
}
void __UserCmd_Slot9(void) {
	UserCmd_SlotInput(8);
}
void __UserCmd_Slot10(void) {
	bool bVisible = gCustomHud.IsTextMenuOpening();
	m_HudCustomAmmo.SlotInput(9, 1);
	m_HudEccoBuyMenu.SlotCallBack(9);
	m_HudEccoBuyMenu.CloseMenu();
	if (!bVisible)
		UserCmd_Slots[9]();
}
void __UserCmd_MissionBrief(void) {
	g_pViewPort->ShowMOTD();
}
void __UserCmd_NextWeapon(void) {
	m_HudCustomAmmo.SlotInput(gWR.m_iNowSlot, 1, true);
	return UserCmd_NextWeapon();
}
void __UserCmd_PrevWeapon(void) {
	m_HudCustomAmmo.SlotInput(gWR.m_iNowSlot, -1, true);
	return UserCmd_PrevWeapon();
}
void __UserCmd_OpenScoreboard(void) {
	gCustomHud.m_bInScore = true;
	if (g_pViewPort && !g_pViewPort->GetInterMission())
		g_pViewPort->ShowScoreBoard();
}
void __UserCmd_CloseScoreboard(void) {
	gCustomHud.m_bInScore = false;
	if(g_pViewPort && !g_pViewPort->GetInterMission())
		g_pViewPort->HideScoreBoard();
}
void __UserCmd_Attack1(void) {
	if (m_HudCustomAmmo.BlockAttackOnce())
		return;
	return UserCmd_Attack1();
}

void CCustomHud::GL_Init(void){
	m_HudRadar.GLInit();
	m_HudIndicator.GLInit();
	m_HudEccoBuyMenu.GLInit();
#ifdef _DEBUG
	m_HudCCTV.GLInit();
#endif
}
void CCustomHud::HUD_Init(void){
	//m_pfnSVCPrint = SVC_HookFunc(svc_print, __SVCHook_Print);
	m_pfnDamage = HOOK_MESSAGE(Damage);
	m_pfnBattery = HOOK_MESSAGE(Battery);
	m_pfnHealth = HOOK_MESSAGE(Health);
	m_pfnScoreInfo = HOOK_MESSAGE(ScoreInfo);
	m_pfnSpectator = HOOK_MESSAGE(Spectator);
	m_pfnServerName = HOOK_MESSAGE(ServerName);
	m_pfnNextMap = HOOK_MESSAGE(NextMap);
	m_pfnTimeEnd = HOOK_MESSAGE(TimeEnd);
	m_pfnShowMenu = HOOK_MESSAGE(ShowMenu);
	m_pfnVoteMenu = HOOK_MESSAGE(VoteMenu);
	m_pfnEndVote = HOOK_MESSAGE(EndVote);
	m_pfnMOTD = HOOK_MESSAGE(MOTD);
	m_pfnFlashBat = HOOK_MESSAGE(FlashBat);
	m_pfnFlashlight = HOOK_MESSAGE(Flashlight);
	m_pfnTextMsg = HOOK_MESSAGE(TextMsg);
	m_pfnMetaHook = HOOK_MESSAGE(MetaHook);
	if(!m_pfnMetaHook)
		gEngfuncs.pfnHookUserMsg("MetaHook", __MsgFunc_MetaHook);

	UserCmd_Attack1 = HOOK_COMMAND("+attack", Attack1);
	UserCmd_Slots[0] = HOOK_COMMAND("slot1", Slot1);
	UserCmd_Slots[1] = HOOK_COMMAND("slot2", Slot2);
	UserCmd_Slots[2] = HOOK_COMMAND("slot3", Slot3);
	UserCmd_Slots[3] = HOOK_COMMAND("slot4", Slot4);
	UserCmd_Slots[4] = HOOK_COMMAND("slot5", Slot5);
	UserCmd_Slots[5] = HOOK_COMMAND("slot6", Slot6);
	UserCmd_Slots[6] = HOOK_COMMAND("slot7", Slot7);
	UserCmd_Slots[7] = HOOK_COMMAND("slot8", Slot8);
	UserCmd_Slots[8] = HOOK_COMMAND("slot9", Slot9);
	UserCmd_Slots[9] = HOOK_COMMAND("slot10", Slot10);

	UserCmd_MissionBrief = HOOK_COMMAND("missionbriefing", MissionBrief);
	UserCmd_NextWeapon = HOOK_COMMAND("invnext", NextWeapon);
	UserCmd_PrevWeapon = HOOK_COMMAND("invprev", PrevWeapon);
	UserCmd_ShowScores = HOOK_COMMAND("+showscores", OpenScoreboard);
	UserCmd_HideScores = HOOK_COMMAND("-showscores", CloseScoreboard);

	gCVars.pDamageScreenFilter = CREATE_CVAR("cl_damageshock", "1", FCVAR_VALUE, nullptr);
	gCVars.pDamageScreenFactor = CREATE_CVAR("cl_damageshock_factor", "0.015", FCVAR_VALUE, nullptr);
	gCVars.pDamageScreenBase = CREATE_CVAR("cl_damageshock_base", "15", FCVAR_VALUE, nullptr);
	gCVars.pDangerHealth = CREATE_CVAR("cl_dangerhealth", "45", FCVAR_VALUE, nullptr);
	gCVars.pDangerArmor = CREATE_CVAR("cl_dangerarmor", "45", FCVAR_VALUE, nullptr);
	gCVars.pDynamicHUD = CREATE_CVAR("cl_hud_csgo", "1", FCVAR_VALUE, nullptr);

	m_HudIndicator.Init();
	m_HudCustomAmmo.Init();
	m_HudRadar.Init();
	m_HudDeathMsg.Init();
	g_HudItemHighLight.Init();
	m_HudEccoBuyMenu.Init();
#ifdef _DEBUG
	m_HudCCTV.Init();
#endif
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
	m_HudDeathMsg.VidInit();
	m_HudIndicator.VidInit();
	m_HudCustomAmmo.VidInit();
	m_HudRadar.VidInit();
	m_HudEccoBuyMenu.VidInit();

	m_flCursorSize = GET_SCREEN_PIXEL(true, "Common.CursorSize");
}
void CCustomHud::HUD_Draw(float flTime){
	SetBaseHudActivity();
	CheckSpectator();

	m_HudDeathMsg.Draw(flTime);
	m_HudRadar.Draw(flTime);
	m_HudEccoBuyMenu.Draw(flTime);

#ifdef _DEBUG
	m_HudCCTV.Draw(flTime);
#endif

	if (!IsHudEnable())
		return;
	m_HudIndicator.Draw(flTime);
	m_HudCustomAmmo.Draw(flTime);
}
void CCustomHud::HUD_Reset(void){
	m_iPlayerHealth = 100;
	m_flOverViewScale = 0;
	m_HudIndicator.Reset();
	m_HudCustomAmmo.Reset();
	m_HudRadar.Reset();
	m_HudDeathMsg.Reset();
	g_HudItemHighLight.Reset();
	m_HudEccoBuyMenu.Reset();
#ifdef _DEBUG
	m_HudCCTV.Reset();
#endif

	m_bInScore = false;
	m_bRenderRadarView = false;
	m_iMouseState = 0;
	m_iLastClick = 5;
	memset(m_SpectatePlayer, 0, sizeof(m_SpectatePlayer));
	memset(m_Playerinfo, 0, sizeof(m_Playerinfo));
	VGUI_CREATE_NEWTGA_TEXTURE(m_iCursorTga, "abcenchance/tga/cursor");
}
void CCustomHud::HUD_UpdateClientData(client_data_t* cdata, float time){
	if (!IsHudEnable())
		return;
	m_iWeaponBits = cdata->iWeaponBits;
	m_bPlayerLongjump = mathlib::fatoi(gEngfuncs.PhysInfo_ValueForKey("slj"));
	g_pViewPort->LongjumpCallBack(m_bPlayerLongjump);
}
void CCustomHud::HUD_ClientMove(struct playermove_s* ppmove, qboolean server){
	if (!IsHudEnable())
		return;
}
void CCustomHud::HUD_Clear(void){
	m_HudRadar.Clear();
	m_HudEccoBuyMenu.Clear();
	m_HudIndicator.Clear();
}
void CCustomHud::HUD_BlitRadarFramebuffer()
{
	m_HudRadar.BlitFramebuffer();
}
void CCustomHud::IN_MouseEvent(int mstate){
	if (!IsHudEnable())
		return;
	auto MouseTest = [&](int mstate, int testBit, vgui::MouseCode enumMouse) {
		//现在有
		if ((mstate & testBit) != 0) {
			//之前没有
			if ((m_iMouseState & testBit) == 0) {
				//Press
				this->OnMousePressed(enumMouse);
				//g_pViewPort->OnMousePressed(enumMouse);
				//加上Bit
				m_iMouseState += testBit;
				if (m_iLastClick == enumMouse) {
					//g_pViewPort->OnMouseDoublePressed(enumMouse);
					m_iLastClick = vgui::MouseCode::MOUSE_LAST;
				}
				else
					m_iLastClick = enumMouse;
			}
		}
		//现在没有之前有
		else if ((m_iMouseState & testBit) != 0) {
			//触发Release
			m_iMouseState -= testBit;
			//g_pViewPort->OnMouseReleased(enumMouse);
		}
	};
	/**	
	* 左键检测 0
	* 右键检测 1
	* 中键检测 2
	* 4键检测  3
	* 5键检测  4 
	**/
	for (size_t i = 0; i < 5; i++) {
		MouseTest(mstate, 1 << i, static_cast<vgui::MouseCode>(i));
	}
}
int CCustomHud::HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding){
	int result = 1;
	if(g_pViewPort)
		result = g_pViewPort->KeyInput(eventcode, keynum, pszCurrentBinding);
	if (!IsHudEnable())
		return 1;
	return result;
}
void CCustomHud::IN_Accumulate(void){
	if (!IsHudEnable())
		return;
}
int CCustomHud::HUD_AddEntity(int type, cl_entity_s* ent, const char* modelname){
	if (!IsHudEnable())
		return true;
	bool result = true;
	result = result && m_HudEccoBuyMenu.AddEntity(type, ent, modelname);
	g_HudItemHighLight.AddEntity(type, ent, modelname);
	g_pViewPort->AddEntity(type, ent, modelname);
	return result;
}
void CCustomHud::HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd) {
	gWR.SyncWeapon(pwd);
}
void CCustomHud::CL_CreateMove(float frametime, usercmd_s* cmd, int active) {
}
bool CCustomHud::HasSuit() {
	return (m_iWeaponBits & (1 << WEAPON_SUIT)) != 0;
}
void CCustomHud::HudHideCallBack(int hidetoken){
	m_iHideHUDDisplay = hidetoken;
	g_pViewPort->HudHideCallBack(hidetoken);
}
bool CCustomHud::IsHudHide(int HideToken) {
	return (m_iHideHUDDisplay & HideToken) != 0;
}
bool CCustomHud::IsHudEnable() {
	return gCVars.pDynamicHUD->value > 0;
}
bool CCustomHud::IsInSpectate() {
	return gEngfuncs.GetLocalPlayer()->curstate.iuser1 > 0;
}
bool CCustomHud::IsSpectator(int client){
	return m_SpectatePlayer[client];
}
void CCustomHud::SetSpectator(int client, bool value){
	m_SpectatePlayer[client] = value;
}
void CCustomHud::CheckSpectator(){
	float newuser = gEngfuncs.GetLocalPlayer()->curstate.iuser1;
	static float iuser;
	if(iuser != newuser)
		g_pViewPort->SetSpectate(newuser > 0);
	iuser = newuser;
}
bool CCustomHud::IsMouseVisible(){
	if(g_pViewPort)
		return g_pViewPort->IsMouseInputEnabled();

	return false;
}
bool CCustomHud::IsTextMenuOpening() {
	return g_pViewPort->IsTextMenuOpen();
}
bool CCustomHud::SelectTextMenuItem(int slot){
	if (IsTextMenuOpening()) {
		g_pViewPort->SelectMenuItem(slot);
		return true;
	}
	return false;
}
void CCustomHud::SetMouseVisible(bool state) {
	if(g_pViewPort)
		g_pViewPort->SetMouseInputEnabled(state);
}
bool CCustomHud::TextDeathMsg(const char* pszName, int iSize, void* pbuf){
	return m_HudDeathMsg.MsgFunc_TextMsg(pszName, iSize, pbuf);
}
WEAPON* CCustomHud::GetCurWeapon(){
	return m_HudCustomAmmo.GetCurWeapon();
}
void CCustomHud::SetCurWeapon(WEAPON* weapon){
	g_pViewPort->SetCurWeapon(weapon);
}
void CCustomHud::OnMousePressed(int code) {
	switch (code) {
		case vgui::MouseCode::MOUSE_LEFT: {
			m_HudCustomAmmo.Select();
			m_HudEccoBuyMenu.SelectMenu();
			break;
		}
	}
}
void CCustomHud::SetBaseHudActivity() {
	if (IsHudEnable()) {
		if (gHookHud.m_Ammo)
			gHookHud.m_Ammo->m_iFlags &= ~HUD_ACTIVE;
		if (gHookHud.m_Battery)
			gHookHud.m_Battery->m_iFlags &= ~HUD_ACTIVE;
		if (gHookHud.m_Health)
			gHookHud.m_Health->m_iFlags &= ~HUD_ACTIVE;
		if(gHookHud.m_Flash)
			gHookHud.m_Flash->m_iFlags &= ~HUD_ACTIVE;
	}
	else {
		if (gHookHud.m_Ammo)
			gHookHud.m_Ammo->m_iFlags |= HUD_ACTIVE;
		if (gHookHud.m_Battery)
			gHookHud.m_Battery->m_iFlags |= HUD_ACTIVE;
		if (gHookHud.m_Health)
			gHookHud.m_Health->m_iFlags |= HUD_ACTIVE;
		if (gHookHud.m_Flash)
			gHookHud.m_Flash->m_iFlags |= HUD_ACTIVE;
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
bool CCustomHud::IsInScore() {
	return m_bInScore;
}
player_infosc_t* CCustomHud::GetPlayerInfoEx(int index) {
	return (player_infosc_t*)IEngineStudio.PlayerInfo(index - 1);
}
CCustomHud :: ~CCustomHud(){
	m_arySprites.clear();
}