#include <metahook.h>
#include <string>
#include <vector>
#include <algorithm>

#include "vguilocal.h"
#include "local.h"
#include "hud.h"
#include "weapon.h"

#include "parsemsg.h"
#include "mymathlib.h"
#include "exportfuncs.h"
#include "ClientParticleMan.h"

#include "player_info.h"
#include "basehud.h"

#include "CCustomHud.h"

#include "indicator.h"
#include "itemhighlight.h"
#include "eccobuymenu.h"

#include "vgui_controls/Controls.h"

#include "weaponbank.h"

#include "radar.h"
#include "ammostack.h"
#include "itemstack.h"
#include "weaponstack.h"
#include "scoreboard.h"
#include "weaponchoose.h"
#include "ammobar.h"
#include "Viewport.h"

#ifdef _DEBUG
#include "cctv.h"
#endif

CCustomHud gCustomHud;
cl_hookedHud gHookHud;

pfnUserMsgHook m_pfnInitHUD;
pfnUserMsgHook m_pfnCurWeapon;
pfnUserMsgHook m_pfnWeaponList;
pfnUserMsgHook m_pfnCustWeapon;
pfnUserMsgHook m_pfnAmmoX;
pfnUserMsgHook m_pfnHideWeapon;
pfnUserMsgHook m_pfnHideHUD;
pfnUserMsgHook m_pfnWeaponSpr;
pfnUserMsgHook m_pfnWeapPickup;
pfnUserMsgHook m_pfnAmmoPickup;
pfnUserMsgHook m_pfnItemPickup;
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

int __MsgFunc_InitHUD(const char* pszName, int iSize, void* pbuf) {
	if (g_pParticleMan)
		g_pParticleMan->ResetParticles();
	return m_pfnInitHUD(pszName, iSize, pbuf);
}
int __MsgFunc_AmmoX(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	gWR.SetAmmo(iIndex, abs(iCount));
	return m_pfnAmmoX(pszName, iSize, pbuf);
}
int __MsgFunc_WeaponList(const char* pszName, int iSize, void* pbuf) {
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
	if (!wp)
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
		gWR.LoadWeaponSprites(wp);
		delete Weapon;
	}
	return m_pfnWeaponList(pszName, iSize, pbuf);
}
int __MsgFunc_CustWeapon(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	std::string name = READ_STRING();
	if (name.size() != 0) {
		gWR.LoadWeaponSprites(id, name.c_str());
		g_pViewPort->GetWeaponChoosePanel()->ReloadWeaponSpr();
		g_pViewPort->GetWeaponStackPanel()->ReloadWeaponSpr();
	}
	return m_pfnCustWeapon(pszName, iSize, pbuf);

}
int __MsgFunc_CurWeapon(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int iState = READ_BYTE();
	if (iState & CAmmoPanel::WEAPONSTATE::VALID) {
		int iId = READ_SHORT();
		//sc反编汇后如此
		if (iId == -1) {
			gWR.DropAllWeapons();
			return m_pfnCurWeapon(pszName, iSize, pbuf);
		}
		int iClip = READ_LONG();
		int iClip2 = READ_LONG();
		WEAPON* pWeapon = gWR.GetWeapon(iId);
		gWR.m_pCurWeapon = pWeapon;
		gCustomHud.SetCurWeapon(pWeapon);
		if (!pWeapon)
			return m_pfnCurWeapon(pszName, iSize, pbuf);
		//更新弹匣信息
		pWeapon->iClip = iClip;
		pWeapon->iClip2 = iClip2;
		pWeapon->iState = iState;
	}
	else {
		int iFlag = READ_SHORT();
		switch (iFlag) {
		case -1:
		case 0:
			gWR.DropAllWeapons();
			break;
		}
	}
	return m_pfnCurWeapon(pszName, iSize, pbuf);
}
int __MsgFunc_HideWeapon(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	gCustomHud.HudHideCallBack(READ_BYTE());
	return m_pfnHideWeapon(pszName, iSize, pbuf);
}
int __MsgFunc_HideHUD(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	gCustomHud.HudHideCallBack(READ_BYTE());
	return m_pfnHideHUD(pszName, iSize, pbuf);
}
//uzi akimbo
//shit uzi
int __MsgFunc_WeaponSpr(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	std::string name = READ_STRING();
	if (name.size() > 0) {
		WEAPON* wp = gWR.GetWeapon(id);
		if (wp && wp->iId > 0) {
			strncpy_s(wp->szSprName, name.c_str(), name.size());
			gWR.LoadWeaponSprites(wp);
			g_pViewPort->GetWeaponChoosePanel()->ReloadWeaponSpr();
			g_pViewPort->GetWeaponStackPanel()->ReloadWeaponSpr();

		}
	}
	return m_pfnWeaponSpr(pszName, iSize, pbuf);
}
int __MsgFunc_WeapPickup(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_SHORT();
	g_pViewPort->GetWeaponStackPanel()->AddItemPickup(iIndex);
	return m_pfnWeapPickup(pszName, iSize, pbuf);
}
int __MsgFunc_AmmoPickup(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	g_pViewPort->GetAmmoStackPanel()->AddAmmoPickup(iIndex, iCount);
	return m_pfnAmmoPickup(pszName, iSize, pbuf);
}
int __MsgFunc_ItemPickup(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	const char* szName = READ_STRING();
	auto index = gCustomHud.GetSpriteIndex(szName);
	if(!index.has_value())
		return m_pfnItemPickup(pszName, iSize, pbuf);
	HSPRITE spr = gCustomHud.GetSprite(index.value());
	wrect_t* rect = gCustomHud.GetSpriteRect(index.value());
	g_pViewPort->GetItemStackPanel()->AddItemPickup(spr, rect->left, rect->right, rect->top, rect->bottom);
	return m_pfnItemPickup(pszName, iSize, pbuf);
}

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
	CPlayerInfo::GetPlayerInfo(clientIndex)->Update();
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
		g_pViewPort->FinishSendMOTD();
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
	if (g_pViewPort && g_pViewPort->TextMsg(pszName, iSize, pbuf))
		return 1;
	BEGIN_READ(pbuf, iSize);
	vgui::CViewport::HUDNOTICE msg_dest = static_cast<vgui::CViewport::HUDNOTICE>(READ_BYTE());
#define BUFFER_SIZE 256
	static auto findLocalize = [](std::string& str) {
			if (str.front() == '#') {
				const wchar_t* find = vgui::localize()->Find(str.c_str());
				if (find) {
					char buffer[BUFFER_SIZE];
					Q_UnicodeToUTF8(find, buffer, BUFFER_SIZE);
					str = buffer;
				}	
			}
		};
	int type = 0;
	std::string msg = READ_STRING();
	std::string sstr1 = READ_STRING();
	std::string sstr2 = READ_STRING();
	std::string sstr3 = READ_STRING();
	std::string sstr4 = READ_STRING();
	findLocalize(msg);
	findLocalize(sstr1);
	if (sstr1.size() > 0)
		type++;
	findLocalize(sstr2);
	if (sstr2.size() > 0)
		type++;
	findLocalize(sstr3);
	if (sstr3.size() > 0)
		type++;
	findLocalize(sstr4);
	if (sstr4.size() > 0)
		type++;
	char buffer[BUFFER_SIZE * 4];
#undef BUFFER_SIZE
	std::string szBuf;
	switch (type) {
	case 1:Q_snprintf(buffer, msg.c_str(), sstr1.c_str()); szBuf = buffer; break;
	case 2:Q_snprintf(buffer, msg.c_str(), sstr1.c_str(), sstr2.c_str()); szBuf = buffer; break;
	case 3:Q_snprintf(buffer, msg.c_str(), sstr1.c_str(), sstr2.c_str(), sstr3.c_str()); szBuf = buffer; break;
	case 4:Q_snprintf(buffer, msg.c_str(), sstr1.c_str(), sstr2.c_str(), sstr3.c_str(), sstr4.c_str()); szBuf = buffer; break;
	case 0:
	default:szBuf = msg; break;
	}
	if (szBuf.back() == '\n')
		szBuf.pop_back();
	std::replace(szBuf.begin(), szBuf.end(), '\r', '\n');

	switch (msg_dest)
	{
	case vgui::CViewport::HUDNOTICE::PRINTNOTIFY:
	case vgui::CViewport::HUDNOTICE::PRINTCENTER:
		if (g_pViewPort)
			g_pViewPort->ShowNotice(msg_dest, szBuf.c_str());
		return 0;
	}

	return m_pfnTextMsg(pszName, iSize, pbuf);
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
			Vector vecOrigin = { READ_COORD(), READ_COORD(), READ_COORD() };
			int iValue = READ_LONG();
			Color pColor = { READ_BYTE(), READ_BYTE() , READ_BYTE() ,READ_BYTE() };
			cl_entity_t* local = gEngfuncs.GetLocalPlayer();
			if (!local)
				return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
			//视角角度
			Vector vecView;
			gEngfuncs.GetViewAngles(vecView);
			CMathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
			//计算我和目标的相对偏移
			Vector vecLength;
			CMathlib::VectorSubtract(vecOrigin, local->curstate.origin, vecLength);
			vecLength = vecLength.Normalize();
			float angledotResult = CMathlib::DotProduct(vecLength, vecView);
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
	gWR.SelectSlot(i, 1, false);
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
	gWR.SelectSlot(9, 1, false);
	m_HudEccoBuyMenu.SlotCallBack(9);
	m_HudEccoBuyMenu.CloseMenu();
	if (!bVisible)
		UserCmd_Slots[9]();
}
void __UserCmd_MissionBrief(void) {
	g_pViewPort->ShowMOTD();
}
void __UserCmd_NextWeapon(void) {
	gWR.SelectSlot(gWR.m_iNowSlot, 1, true);
	return UserCmd_NextWeapon();
}
void __UserCmd_PrevWeapon(void) {
	gWR.SelectSlot(gWR.m_iNowSlot, -1, true);
	return UserCmd_PrevWeapon();
}
void __UserCmd_OpenScoreboard(void) {
	gCustomHud.m_bInScore = true;
	if (g_pViewPort && !g_pViewPort->GetInterMission())
		g_pViewPort->ShowScoreBoard();
	return;
	//UserCmd_ShowScores();
}
void __UserCmd_CloseScoreboard(void) {
	gCustomHud.m_bInScore = false;
	if (g_pViewPort && !g_pViewPort->GetInterMission())
		g_pViewPort->HideScoreBoard();
	//UserCmd_HideScores();
}
void __UserCmd_Attack1(void) {
	if (g_pViewPort->GetWeaponChoosePanel()->BlockAttackOnce())
		return;
	return UserCmd_Attack1();
}

void CCustomHud::GL_Init(void){
	m_HudIndicator.GLInit();
	m_HudEccoBuyMenu.GLInit();
#ifdef _DEBUG
	m_HudCCTV.GLInit();
#endif
}
void CCustomHud::HUD_Init(void){
	//m_pfnSVCPrint = SVC_HookFunc(svc_print, __SVCHook_Print);
	m_pfnInitHUD = HOOK_MESSAGE(InitHUD);
	m_pfnAmmoX = HOOK_MESSAGE(AmmoX);
	m_pfnCurWeapon = HOOK_MESSAGE(CurWeapon);
	m_pfnWeaponList = HOOK_MESSAGE(WeaponList);
	m_pfnCustWeapon = HOOK_MESSAGE(CustWeapon);
	m_pfnHideWeapon = HOOK_MESSAGE(HideWeapon);
	m_pfnHideHUD = HOOK_MESSAGE(HideHUD);
	m_pfnWeaponSpr = HOOK_MESSAGE(WeaponSpr);
	m_pfnItemPickup = HOOK_MESSAGE(ItemPickup);
	m_pfnAmmoPickup = HOOK_MESSAGE(AmmoPickup);
	m_pfnWeapPickup = HOOK_MESSAGE(WeapPickup);
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

	m_HudIndicator.Init();
	gWR.Init();
	m_HudEccoBuyMenu.Init();
#ifdef _DEBUG
	m_HudCCTV.Init();
#endif
}
void CCustomHud::HUD_VidInit(void){
	int iRes = ScreenWidth() < 640 ? 320 : 640;
	if (m_arySprites.size() == 0){
		int size;
		client_sprite_t* arySpritelist = SPR_GetList("sprites/hud.txt", &size);
		if (arySpritelist){
			client_sprite_t* p = arySpritelist;
			for (int i = 0; i < size; i++){
				if (p->iRes == iRes){
					char sz[MAX_PATH];
					std::sprintf(sz, "sprites/%s.spr", p->szSprite);
					client_sprite_t* item = new client_sprite_t();
					std::memcpy(item, p, sizeof(client_sprite_t));
					item->hspr = SPR_Load(sz);
					m_arySprites.push_back(item);
				}
				p++;
			}
		}
	}
	else{
		for (auto iter = m_arySprites.begin(); iter != m_arySprites.end(); iter++) {
			client_sprite_t* item = *iter;
			if (item->iRes == iRes) {
				char sz[MAX_PATH];
				std::sprintf(sz, "sprites/%s.spr", item->szSprite);
				item->hspr = SPR_Load(sz);
			}
		}
	}
	m_HudIndicator.VidInit();
	gWR.VidInit();
	m_HudEccoBuyMenu.VidInit();

	m_flCursorSize = GET_SCREEN_PIXEL(true, "Common.CursorSize");
}
void CCustomHud::HUD_Draw(float flTime){
	m_HudEccoBuyMenu.Draw(flTime);
#ifdef _DEBUG
	m_HudCCTV.Draw(flTime);
#endif
	m_HudIndicator.Draw(flTime);
}
void CCustomHud::HUD_Reset(void){
	m_HudIndicator.Reset();
	gWR.Reset();
	m_HudEccoBuyMenu.Reset();
#ifdef _DEBUG
	m_HudCCTV.Reset();
#endif

	m_bInScore = false;
	m_bitsHideHUDDisplay = 0;
	m_bitsWeaponBits.reset();
	std::fill(m_aryPlayerInfos.begin(), m_aryPlayerInfos.end(), hud_playerinfo_t{});
	VGUI_CREATE_NEWTGA_TEXTURE(m_iCursorTga, "abcenchance/tga/cursor");
}
void CCustomHud::HUD_UpdateClientData(client_data_t* cdata, float time){
	//check spectate
	float newuser = gEngfuncs.GetLocalPlayer()->curstate.iuser1;
	static float iuser;
	if (iuser != newuser) {
		g_pViewPort->SetSpectate(newuser > 0);
		iuser = newuser;
	}
	if (!m_bitsWeaponBits.has_value() || m_bitsWeaponBits != cdata->iWeaponBits)
		WeaponBitsChangeCallBack(cdata->iWeaponBits);
	m_bitsWeaponBits = cdata->iWeaponBits;

	//check lj
	static bool lj = false;
	bool nlj = std::atoi(gEngfuncs.PhysInfo_ValueForKey("slj"));
	if (lj != nlj) {
		g_pViewPort->LongjumpCallBack(nlj);
		lj = nlj;
	}	
}
void CCustomHud::HUD_Clear(void){
	m_HudEccoBuyMenu.Clear();
	m_HudIndicator.Clear();
}
void CCustomHud::IN_MouseEvent(int mstate){
	auto MouseTest = [&](int mstate, int testBit, vgui::MouseCode enumMouse) {
		static int s_mousebits = 0;
		static int s_lastclick = 5;
		//现在有
		if ((mstate & testBit) != 0) {
			//之前没有
			if ((s_mousebits & testBit) == 0) {
				//Press
				OnMousePressed(enumMouse);
				//g_pViewPort->OnMousePressed(enumMouse);
				//加上Bit
				s_mousebits += testBit;
				if (s_lastclick == enumMouse) {
					//g_pViewPort->OnMouseDoublePressed(enumMouse);
					s_lastclick = vgui::MouseCode::MOUSE_LAST;
				}
				else
					s_lastclick = enumMouse;
			}
		}
		//现在没有之前有
		else if ((s_mousebits & testBit) != 0) {
			//触发Release
			s_mousebits -= testBit;
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
int CCustomHud::HUD_AddEntity(int type, cl_entity_s* ent, const char* modelname){
	bool result = true;
	result = result && m_HudEccoBuyMenu.AddEntity(type, ent, modelname);
	g_pViewPort->AddEntity(type, ent, modelname);
	return result;
}
void CCustomHud::HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd) {
	gWR.SyncWeapon(pwd);
}
bool CCustomHud::HasSuit() {
	if (!m_bitsWeaponBits.has_value())
		return false;
	return (m_bitsWeaponBits.value() & (1 << WEAPON_SUIT)) != 0;
}
void CCustomHud::WeaponBitsChangeCallBack(int bits){
	g_pViewPort->WeaponBitsChangeCallback(bits);
}
void CCustomHud::HudHideCallBack(int hidetoken){
	m_bitsHideHUDDisplay = hidetoken;
	g_pViewPort->HudHideCallBack(hidetoken);
}
bool CCustomHud::IsHudHide(int HideToken) {
	return (m_bitsHideHUDDisplay & HideToken) != 0;
}
bool CCustomHud::IsInSpectate() {
	auto local = gEngfuncs.GetLocalPlayer();
	if (local)
		return local->curstate.iuser1 > 0;
	else
		return false;
}
bool CCustomHud::IsSpectator(int cliententindex){
	return m_aryPlayerInfos[cliententindex-1].spectate;
}
void CCustomHud::SetSpectator(int cliententindex, bool value){
	m_aryPlayerInfos[cliententindex-1].spectate = value;
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
void CCustomHud::SetCurWeapon(WEAPON* weapon){
	g_pViewPort->SetCurWeapon(weapon);
}
void CCustomHud::OnMousePressed(int code) {
	switch (code) {
		case vgui::MouseCode::MOUSE_LEFT: {
			g_pViewPort->GetWeaponChoosePanel()->SelectWeapon();
			m_HudEccoBuyMenu.SelectMenu();
			break;
		}
	}
}
void CCustomHud::HideOriginalHud() {
	if (gHookHud.m_Ammo)
		gHookHud.m_Ammo->m_iFlags &= ~HUD_ACTIVE;
	if (gHookHud.m_Battery)
		gHookHud.m_Battery->m_iFlags &= ~HUD_ACTIVE;
	if (gHookHud.m_Health)
		gHookHud.m_Health->m_iFlags &= ~HUD_ACTIVE;
	if(gHookHud.m_Flash)
		gHookHud.m_Flash->m_iFlags &= ~HUD_ACTIVE;
}
HSPRITE CCustomHud::GetSprite(size_t index) {
	return (index < 0) ? 0 : m_arySprites[index]->hspr;
}
wrect_t* CCustomHud::GetSpriteRect(size_t index) {
	if(index >= 0 && index < m_arySprites.size())
		return &m_arySprites[index]->rc;
	return nullptr;
}
std::optional<int> CCustomHud::GetSpriteIndex(const char* SpriteName){
	for (size_t i = 0; i < m_arySprites.size(); i++) {
		if(!std::strncmp(SpriteName, m_arySprites[i]->szName, 64))
			return i;
	}
	return std::nullopt;
}
hud_playerinfo_t* CCustomHud::GetPlayerHUDInfo(int index){
	if (index > 0 && index <= 33)
		return &m_aryPlayerInfos[index-1];
	return nullptr;
}
bool CCustomHud::IsInScore() {
	return m_bInScore;
}
void CCustomHud::RenderRadar(){
	g_pViewPort->GetRadarPanel()->RenderRadar();
}
player_infosc_t* CCustomHud::GetPlayerInfoEx(int index) {
	return reinterpret_cast<player_infosc_t*>(IEngineStudio.PlayerInfo(index - 1));
}
CCustomHud :: ~CCustomHud(){
	m_arySprites.clear();
}