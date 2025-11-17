#include <metahook.h>
#include <string>
#include <vector>
#include <algorithm>

#include "vguilocal.h"
#include "local.h"

#include "parsemsg.h"
#include "mymathlib.h"
#include "exportfuncs.h"
#include "autofunc.h"
#include "ClientParticleMan.h"

#include "core/resource/playerresource.h"
#include "core/resource/spriteresource.h"
#include "core/resource/weaponresource.h"

#include "CCustomHud.h"

#include "indicator.h"
#include "itemhighlight.h"

#include "vgui_controls/Controls.h"

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

#pragma region UserMsg Varibles
static pfnUserMsgHook m_pfnInitHUD;
static pfnUserMsgHook m_pfnCurWeapon;
static pfnUserMsgHook m_pfnCustWeapon;
static pfnUserMsgHook m_pfnHideWeapon;
static pfnUserMsgHook m_pfnHideHUD;
static pfnUserMsgHook m_pfnWeaponSpr;
static pfnUserMsgHook m_pfnWeapPickup;
static pfnUserMsgHook m_pfnAmmoPickup;
static pfnUserMsgHook m_pfnItemPickup;
static pfnUserMsgHook m_pfnHealth;
static pfnUserMsgHook m_pfnScoreInfo;
static pfnUserMsgHook m_pfnSpectator;
static pfnUserMsgHook m_pfnServerName;
static pfnUserMsgHook m_pfnNextMap;
static pfnUserMsgHook m_pfnTimeEnd;
static pfnUserMsgHook m_pfnShowMenu;
static pfnUserMsgHook m_pfnVoteMenu;
static pfnUserMsgHook m_pfnEndVote;
static pfnUserMsgHook m_pfnMOTD;
static pfnUserMsgHook m_pfnFlashBat;
static pfnUserMsgHook m_pfnFlashlight;
static pfnUserMsgHook m_pfnTextMsg;
static pfnUserMsgHook m_pfnMetaHook;
static pfnUserMsgHook m_pfnDamage;
static pfnUserMsgHook m_pfnBattery;
static pfnUserMsgHook m_pfnClExtrasInfo;
static pfnUserMsgHook m_pfnMapList;
#pragma endregion

#pragma region UserMsg Hooks


static int __MsgFunc_HideWeapon(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	gCustomHud.HudHideCallBack(READ_BYTE());
	return m_pfnHideWeapon(pszName, iSize, pbuf);
}
static int __MsgFunc_HideHUD(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	gCustomHud.HudHideCallBack(READ_BYTE());
	return m_pfnHideHUD(pszName, iSize, pbuf);
}
//uzi akimbo
//shit uzi
static int __MsgFunc_WeaponSpr(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	std::string name = READ_STRING();
	if (name.size() > 0) {
		Weapon* wp = gWR.GetWeapon(id);
		if (wp && wp->iId > 0) {
			strncpy(wp->szSprName, name.c_str(), sizeof(wp->szSprName) - 1);
			wp->szSprName[sizeof(wp->szSprName) - 1] = 0;
			gWR.LoadWeaponSprites(wp);
			GetBaseViewPort()->GetWeaponChoosePanel()->ReloadWeaponSpr();
			GetBaseViewPort()->GetWeaponStackPanel()->ReloadWeaponSpr();

		}
	}
	return m_pfnWeaponSpr(pszName, iSize, pbuf);
}
static int __MsgFunc_WeapPickup(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_SHORT();
	GetBaseViewPort()->GetWeaponStackPanel()->AddItemPickup(iIndex);
	return m_pfnWeapPickup(pszName, iSize, pbuf);
}
static int __MsgFunc_AmmoPickup(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	GetBaseViewPort()->GetAmmoStackPanel()->AddAmmoPickup(iIndex, iCount);
	return m_pfnAmmoPickup(pszName, iSize, pbuf);
}
static int __MsgFunc_ItemPickup(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	const char* szName = READ_STRING();
	auto index = gSpriteRes.GetSpriteIndex(szName);
	if(!index.has_value())
		return m_pfnItemPickup(pszName, iSize, pbuf);
	HSPRITE spr = gSpriteRes.GetSprite(index.value());
	wrect_t* rect = gSpriteRes.GetSpriteRect(index.value());
	GetBaseViewPort()->GetItemStackPanel()->AddItemPickup(spr, rect->left, rect->right, rect->top, rect->bottom);
	return m_pfnItemPickup(pszName, iSize, pbuf);
}

static int __MsgFunc_Damage(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int armor = READ_BYTE();
	int damageTaken = READ_BYTE();
	int tiles = READ_LONG();
	if (armor + damageTaken + tiles == 0)
		return m_pfnDamage(pszName, iSize, pbuf);
	vec3_t vecFrom;
	for (size_t i = 0; i < 3; i++) {
		vecFrom[i] = READ_COORD();
	}
	if(damageTaken > 0 || armor > 0)
		m_HudIndicator.AddIdicator(damageTaken, armor, vecFrom);
	GetBaseViewPort()->UpdateTiles(tiles);
	AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_DAMAGE, 
		std::to_string(damageTaken).c_str(), std::to_string(armor).c_str(), std::to_string(tiles).c_str());
	return m_pfnDamage(pszName, iSize, pbuf);
}
static int __MsgFunc_Battery(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int battery = READ_SHORT();
	GetBaseViewPort()->SetArmor(battery);
	AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_BATTERY, std::to_string(battery).c_str());
	return m_pfnBattery(pszName, iSize, pbuf);
}
static int __MsgFunc_Health(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int health = READ_LONG();
	GetBaseViewPort()->SetHealth(health);
	AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_HEALTH, std::to_string(health).c_str());
	return m_pfnHealth(pszName, iSize, pbuf);
}
static int __MsgFunc_ScoreInfo(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	//wtf is not this shit
	if (clientIndex >= 1 && clientIndex <= 33) {
		auto infos = gPlayerRes.GetPlayerInfo(clientIndex);
		infos->m_iFrags = READ_FLOAT();
		infos->m_iDeaths = READ_LONG();
		infos->m_iHealth = READ_FLOAT();
		infos->m_iArmor = READ_FLOAT();
		infos->m_iTeamNumber = static_cast<TEAM_ID>(READ_BYTE());
		infos->m_eHideExtra = static_cast<PlayerInfo::HIDE_EXTRA>(READ_BYTE());
		infos->m_iAdmin = static_cast<PlayerInfo::ADMIN>(READ_BYTE());
		//Real donor hide in ScorePanel + 0x9247
		infos->Update();
	}
	return m_pfnScoreInfo(pszName, iSize, pbuf);
}
static int __MsgFunc_Spectator(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	if (clientIndex > 0 && clientIndex <= 32) {
		int beSpectator = READ_BYTE();
		auto pi = gPlayerRes.GetPlayerInfo(clientIndex);
		pi->m_bIsSpectate = beSpectator != 0;
	}
	return m_pfnSpectator(pszName, iSize, pbuf);
}
static int __MsgFunc_ServerName(const char* pszName, int iSize, void* pbuf) {
	if (GetBaseViewPort())
	{
		BEGIN_READ(pbuf, iSize);
		GetBaseViewPort()->SetServerName(READ_STRING());
		GetBaseViewPort()->GetScoreBoard()->UpdateServerName();
	}
	return m_pfnServerName(pszName, iSize, pbuf);
}
static int __MsgFunc_NextMap(const char* pszName, int iSize, void* pbuf) {
	if (GetBaseViewPort())
	{
		BEGIN_READ(pbuf, iSize);
		GetBaseViewPort()->SetNextMap(READ_STRING());
		GetBaseViewPort()->GetScoreBoard()->UpdateNextMap();
	}
	return m_pfnNextMap(pszName, iSize, pbuf);
}
static int __MsgFunc_TimeEnd(const char* pszName, int iSize, void* pbuf) {
	
	if (GetBaseViewPort())
	{
		BEGIN_READ(pbuf, iSize);
		GetBaseViewPort()->m_iTimeEnd = READ_LONG();
		GetBaseViewPort()->GetScoreBoard()->UpdateTimeEnd();
	}
	return m_pfnTimeEnd(pszName, iSize, pbuf);
}
static int __MsgFunc_ShowMenu(const char* pszName, int iSize, void* pbuf){
	//block hahahaha
	return GetBaseViewPort()->MsgShowMenu(pszName, iSize, pbuf);
}
static int __MsgFunc_VoteMenu(const char* pszName, int iSize, void* pbuf) {
	if (!GetBaseViewPort()->IsVoteEnable())
		return m_pfnVoteMenu(pszName, iSize, pbuf);

	BEGIN_READ(pbuf, iSize);
	int iVoteType = READ_BYTE();
	char szContent[2048];
	char szYes[2048];
	char szNo[2048];
	Q_strcpy(szContent, READ_STRING());
	Q_strcpy(szYes, READ_STRING());
	Q_strcpy(szNo, READ_STRING());
	GetBaseViewPort()->StartVote(szContent, szYes, szNo, iVoteType);
	return 1;
}
static int __MsgFunc_EndVote(const char* pszName, int iSize, void* pbuf) {
	GetBaseViewPort()->EndVote();
	return m_pfnEndVote(pszName, iSize, pbuf);
}
static int __MsgFunc_MOTD(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int code = READ_BYTE();
	char* msg = READ_STRING();
	if(msg[0] != '\0')
		GetBaseViewPort()->AppendMOTD(msg);
	else
		GetBaseViewPort()->CloseMOTD();
	if (code > 0)
		GetBaseViewPort()->FinishSendMOTD();
	return m_pfnMOTD(pszName, iSize, pbuf);
}
static int __MsgFunc_FlashBat(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int flash = READ_BYTE();
	GetBaseViewPort()->SetFlashBattery(flash);
	AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_FLASHBATTERY, std::to_string(flash).c_str());
	return m_pfnFlashBat(pszName, iSize, pbuf);
}
static int __MsgFunc_Flashlight(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int on = READ_BYTE();
	int battery = READ_BYTE();
	GetBaseViewPort()->SetFlashLight(on, battery);
	return m_pfnFlashlight(pszName, iSize, pbuf);
}
static int __MsgFunc_TextMsg(const char* pszName, int iSize, void* pbuf) {
	if (GetBaseViewPort() && GetBaseViewPort()->TextMsg(pszName, iSize, pbuf))
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
		if (GetBaseViewPort())
			GetBaseViewPort()->ShowNotice(msg_dest, szBuf.c_str());
		return 0;
	}

	return m_pfnTextMsg(pszName, iSize, pbuf);
}
static int __MsgFunc_ClExtrasInfo(const char* pszName, int iSize, void* pbuf) {
	//Why Encrypt it? is it aes?
	//Funny Encrypt here, plain text length 33, sent length 105
	//x3 Network traffic, lets fuck more server operator
	/*BEGIN_READ(pbuf, iSize);
	int plainDataLength = READ_LONG();
	int ivLength = READ_LONG();
	std::vector<byte> iv{};
	for (int i = 0; i < ivLength; i++) {
		iv.push_back(READ_BYTE());
	}
	int encryptLength = READ_LONG();
	std::vector<byte> encrypt{};
	for (int i = 0; i < encryptLength; i++) {
		encrypt.push_back(READ_BYTE());
	}
	int enctryptDigestLength = READ_LONG();
	std::vector<byte> digest{};
	for (int i = 0; i < enctryptDigestLength; i++) {
		digest.push_back(READ_BYTE());
	}*/
	int result = m_pfnClExtrasInfo(pszName, iSize, pbuf);
	gPlayerRes.UpdateAll();
	return result;
}

static std::vector<std::string> s_aryMapList;
std::vector<std::string>& GetGameMapList() {
	return s_aryMapList;
}
static int __MsgFunc_MapList(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int clearall = READ_BYTE();
	if (clearall == 0) {
		//need clear list
		int all_count = READ_SHORT();
		s_aryMapList.clear();
	}
	else {
		int start = READ_SHORT();
		int end = READ_SHORT();
		for (int i = start; i < end; i++) {
			char* map = READ_STRING();
			s_aryMapList.push_back(map);
		}
	}
	return m_pfnMapList(pszName,iSize,pbuf);
}
static int __MsgFunc_MetaHook(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int type = READ_BYTE();
	switch (type) {
	case CCustomHud::MetaHookMsgType::MHSV_CMD_ABC_CUSTOM: {
		CCustomHud::ABCCustomMsg type = static_cast<CCustomHud::ABCCustomMsg>(READ_BYTE());
		switch (type) {
		case CCustomHud::ABCCustomMsg::POPNUMBER: {
			if (GetBaseViewPort()->m_pPopNumber->value <= 0)
				return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
			Vector vecOrigin = { READ_COORD(), READ_COORD(), READ_COORD() };
			int iValue = READ_LONG();
			Color pColor = { READ_BYTE(), READ_BYTE() , READ_BYTE() ,READ_BYTE() };
			cl_entity_t* local = gEngfuncs.GetLocalPlayer();
			if (!local)
				return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
			//�ӽǽǶ�
			Vector vecView;
			gEngfuncs.GetViewAngles(vecView);
			CMathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
			//�����Һ�Ŀ������ƫ��
			Vector vecLength;
			CMathlib::VectorSubtract(vecOrigin, local->curstate.origin, vecLength);
			vecLength = vecLength.Normalize();
			float angledotResult = CMathlib::DotProduct(vecLength, vecView);
			//cos 60
			if (angledotResult > 0.5)
				GetBaseViewPort()->AddPopNumber(vecOrigin, pColor, iValue);
			return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
		}
		}
		break;
	}
	default:break;
	}
	return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
}
#pragma endregion

#pragma region UserCmd Varibles
void(*UserCmd_Slots[10])(void);
void(*UserCmd_NextWeapon)(void);
void(*UserCmd_PrevWeapon)(void);
void(*UserCmd_ShowScores)(void);
void(*UserCmd_HideScores)(void);
void(*UserCmd_Attack1)(void);
void(*UserCmd_MissionBrief)(void);
void(*UserCmd_VoteMenu)(void);
#pragma endregion

#pragma region UserCmd Hooks
static void UserCmd_SlotInput(int i) {
	bool bVisible = gCustomHud.IsTextMenuOpening();
	gWR.SelectSlot(i, 1, false);
	if(!bVisible)
		UserCmd_Slots[i]();
}
static void __UserCmd_Slot1(void) {
	UserCmd_SlotInput(0);
}
static void __UserCmd_Slot2(void) {
	UserCmd_SlotInput(1);
}
static void __UserCmd_Slot3(void) {
	UserCmd_SlotInput(2);
}
static void __UserCmd_Slot4(void) {
	UserCmd_SlotInput(3);
}
static void __UserCmd_Slot5(void) {
	UserCmd_SlotInput(4);
}
static void __UserCmd_Slot6(void) {
	UserCmd_SlotInput(5);
}
static void __UserCmd_Slot7(void) {
	UserCmd_SlotInput(6);
}
static void __UserCmd_Slot8(void) {
	UserCmd_SlotInput(7);
}
static void __UserCmd_Slot9(void) {
	UserCmd_SlotInput(8);
}
static void __UserCmd_Slot10(void) {
	bool bVisible = gCustomHud.IsTextMenuOpening();
	gWR.SelectSlot(9, 1, false);
	if (!bVisible)
		UserCmd_Slots[9]();
}
static void __UserCmd_MissionBrief(void) {
	GetBaseViewPort()->ShowMOTD();
}
static void __UserCmd_NextWeapon(void) {
	gWR.SelectSlot(gWR.m_iNowSlot, 1, true);
	return UserCmd_NextWeapon();
}
static void __UserCmd_PrevWeapon(void) {
	gWR.SelectSlot(gWR.m_iNowSlot, -1, true);
	return UserCmd_PrevWeapon();
}
static void __UserCmd_OpenScoreboard(void) {
	gCustomHud.m_bInScore = true;
	if (GetBaseViewPort() && !GetBaseViewPort()->GetInterMission())
		GetBaseViewPort()->ShowScoreBoard();
	return;
	//UserCmd_ShowScores();
}
static void __UserCmd_CloseScoreboard(void) {
	gCustomHud.m_bInScore = false;
	if (GetBaseViewPort() && !GetBaseViewPort()->GetInterMission())
		GetBaseViewPort()->HideScoreBoard();
	//UserCmd_HideScores();
}
static void __UserCmd_Attack1(void) {
	if (GetBaseViewPort()->GetWeaponChoosePanel()->BlockAttackOnce())
		return;
	return UserCmd_Attack1();
}
static void __UserCmd_VoteMenu(void) {
	extern void OpenVoteMenuDialog();
	OpenVoteMenuDialog();
	//UserCmd_VoteMenu();
}
#pragma endregion

#pragma region CCustomHud

/*
 Called from GL_BeginRendering
*/
void CCustomHud::OnBeginRendering(int* x, int* y, int* width, int* height)
{

}

/*
	Called from GL_BeginRendering -> R_RenderFrameStart
*/
void CCustomHud::OnRenderFrameStart()
{

}

/*
	Called from GL_BeginRendering
*/
void CCustomHud::OnBeginRenderingPost(int* x, int* y, int* width, int* height)
{

}

/*
	Called from GL_FreeFrameBuffers
*/
void CCustomHud::OnFreeFrameBuffers()
{

}

/*
	Called from GL_GenerateFrameBuffers
*/
void CCustomHud::OnGenerateFrameBuffers()
{

}

/*
	Called from R_PreRenderView
*/
void CCustomHud::OnPreRenderView()
{
	RenderRadar();
}

/*
	Called from R_PreRenderView, after switching to target FBO
*/
void CCustomHud::OnPreRenderViewPost()
{

}

/*
	Called from R_PostRenderView
*/
void CCustomHud::OnPostRenderView()
{

}

/*
	Called from R_PostRenderView, after executing all built-in post-processes
*/
void CCustomHud::OnPostRenderViewPost()
{

}

/*
	Called from R_RenderEndFrame
*/
void CCustomHud::OnRenderEndFrame()
{

}


void CCustomHud::GL_Init(void)
{
	m_HudIndicator.GLInit();
#ifdef _DEBUG
	m_HudCCTV.GLInit();
#endif

	if (MetaRenderer())
	{
		MetaRenderer()->RegisterRenderCallbacks(this);
	}
}

void CCustomHud::HUD_Init(void)
{
	//m_pfnSVCPrint = SVC_HookFunc(svc_print, __SVCHook_Print);
	m_pfnInitHUD = HOOK_MESSAGE(InitHUD);

	m_pfnCurWeapon = HOOK_MESSAGE(CurWeapon);
	
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
	m_pfnClExtrasInfo = HOOK_MESSAGE(ClExtrasInfo)
	m_pfnMapList = HOOK_MESSAGE(MapList);
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
	UserCmd_VoteMenu = HOOK_COMMAND("votemenu", VoteMenu);

	gCVars.pDamageScreenFilter = CREATE_CVAR("hud_damageshock", "1", FCVAR_VALUE, nullptr);
	gCVars.pDamageScreenFactor = CREATE_CVAR("hud_damageshock_factor", "0.015", FCVAR_VALUE, nullptr);
	gCVars.pDamageScreenBase = CREATE_CVAR("hud_damageshock_base", "15", FCVAR_VALUE, nullptr);
	gCVars.pDangerHealth = CREATE_CVAR("hud_danger_health", "45", FCVAR_VALUE, nullptr);
	gCVars.pDangerArmor = CREATE_CVAR("hud_danger_armor", "45", FCVAR_VALUE, nullptr);

	m_HudIndicator.Init();
	gWR.Init();
#ifdef _DEBUG
	m_HudCCTV.Init();
#endif
}

void CCustomHud::HUD_VidInit(void)
{
	gSpriteRes.VidInit();
	m_HudIndicator.VidInit();
	gWR.VidInit();

	m_flCursorSize = GET_SCREEN_PIXEL(true, "Common.CursorSize");
}

void CCustomHud::HUD_Draw(float flTime)
{
#ifdef _DEBUG
	m_HudCCTV.Draw(flTime);
#endif
	m_HudIndicator.Draw(flTime);
}

void CCustomHud::HUD_Reset(void)
{
	m_HudIndicator.Reset();
	gWR.Reset();
#ifdef _DEBUG
	m_HudCCTV.Reset();
#endif

	m_bInScore = false;
	m_bitsHideHUDDisplay = 0;
	m_bitsWeaponBits.reset();
	VGUI_CREATE_NEWTGA_TEXTURE(m_iCursorTga, "abcenchance/tga/cursor");
}
void CCustomHud::HUD_UpdateClientData(client_data_t* cdata, float time){
	//check spectate
	float newuser = gEngfuncs.GetLocalPlayer()->curstate.iuser1;
	static float iuser;
	if (iuser != newuser) {
		GetBaseViewPort()->SetSpectate(newuser > 0);
		iuser = newuser;
	}
	if (!m_bitsWeaponBits.has_value() || m_bitsWeaponBits != cdata->iWeaponBits)
		WeaponBitsChangeCallBack(cdata->iWeaponBits);
	m_bitsWeaponBits = cdata->iWeaponBits;

	//check lj
	static bool lj = false;
	bool nlj = std::atoi(gEngfuncs.PhysInfo_ValueForKey("slj"));
	if (lj != nlj) {
		GetBaseViewPort()->LongjumpCallBack(nlj);
		lj = nlj;
	}	
}
void CCustomHud::HUD_Clear(void){
	m_HudIndicator.Clear();
}
void CCustomHud::IN_MouseEvent(int mstate){
	auto MouseTest = [&](int mstate, int testBit, vgui::MouseCode enumMouse) {
		static int s_mousebits = 0;
		static int s_lastclick = 5;
		//������
		if ((mstate & testBit) != 0) {
			//֮ǰû��
			if ((s_mousebits & testBit) == 0) {
				//Press
				OnMousePressed(enumMouse);
				//g_pViewPort->OnMousePressed(enumMouse);
				//����Bit
				s_mousebits += testBit;
				if (s_lastclick == enumMouse) {
					//g_pViewPort->OnMouseDoublePressed(enumMouse);
					s_lastclick = vgui::MouseCode::MOUSE_LAST;
				}
				else
					s_lastclick = enumMouse;
			}
		}
		//����û��֮ǰ��
		else if ((s_mousebits & testBit) != 0) {
			//����Release
			s_mousebits -= testBit;
			//g_pViewPort->OnMouseReleased(enumMouse);
		}
	};
	/**	
	* ������ 0
	* �Ҽ���� 1
	* �м���� 2
	* 4�����  3
	* 5�����  4 
	**/
	for (size_t i = 0; i < 5; i++) {
		MouseTest(mstate, 1 << i, static_cast<vgui::MouseCode>(i));
	}
}
int CCustomHud::HUD_AddEntity(int type, cl_entity_s* ent, const char* modelname){
	GetBaseViewPort()->AddEntity(type, ent, modelname);
	return 1;
}
void CCustomHud::HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd) {
	auto wp = WeaponData::FromWeaponData(pwd);
	gWR.SyncWeapon(wp);
}
bool CCustomHud::HasSuit() {
	if (!m_bitsWeaponBits.has_value())
		return false;
	constexpr auto WEAPON_SUIT = 31;
	return (m_bitsWeaponBits.value() & (1 << WEAPON_SUIT)) != 0;
}
void CCustomHud::WeaponBitsChangeCallBack(int bits){
	GetBaseViewPort()->WeaponBitsChangeCallback(bits);
}
void CCustomHud::HudHideCallBack(int hidetoken){
	m_bitsHideHUDDisplay = hidetoken;
	GetBaseViewPort()->HudHideCallBack(hidetoken);
}
bool CCustomHud::IsHudHide(int HideToken) {
	return (m_bitsHideHUDDisplay & HideToken) != 0;
}
bool CCustomHud::IsMouseVisible(){
	if(GetBaseViewPort())
		return GetBaseViewPort()->IsMouseInputEnabled();

	return false;
}
bool CCustomHud::IsTextMenuOpening() {
	return GetBaseViewPort()->IsTextMenuOpen();
}
bool CCustomHud::SelectTextMenuItem(int slot){
	if (IsTextMenuOpening()) {
		GetBaseViewPort()->SelectMenuItem(slot);
		return true;
	}
	return false;
}
void CCustomHud::SetMouseVisible(bool state) {
	if(GetBaseViewPort())
		GetBaseViewPort()->SetMouseInputEnabled(state);
}
void CCustomHud::SetCurWeapon(Weapon* weapon){
	GetBaseViewPort()->SetCurWeapon(weapon);
}
void CCustomHud::OnMousePressed(int code) {
	switch (code) {
		case vgui::MouseCode::MOUSE_LEFT: {
			GetBaseViewPort()->GetWeaponChoosePanel()->SelectWeapon();
			break;
		}
	}
}
bool CCustomHud::IsInScore() {
	return m_bInScore;
}
void CCustomHud::RenderRadar(){
	GetBaseViewPort()->GetRadarPanel()->RenderRadar();
}
#pragma endregion