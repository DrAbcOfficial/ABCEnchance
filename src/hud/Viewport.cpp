#pragma once
#include <metahook.h>
#include <vector>
#include <string>
#include <regex>
#include <bitset>

#include <vgui/VGUI.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/IEngineVGui.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/AnimationController.h>

#include "utility/util.h"
#include "utility/vgui_util.h"

#include "core/events/playerinfo.h"
#include "core/events/networkmessage.h"
#include "core/events/command.h"
#include "core/events/hudevents.h"
#include "core/resource/playerresource.h"
#include "core/resource/spriteresource.h"

#include "core/library/mymathlib.h"

#include "cdll_int.h"

#include "hud/vgui/motd.h"
#include "hud/vgui/popnum.h"
#include "hud/vgui/playerboard.h"
#include "hud/vgui/scoreboard.h"
#include "hud/vgui/vote.h"
#include "hud/vgui/sidetext.h"
#include "hud/vgui/textmenu.h"
#include "hud/vgui/flashlight.h"
#include "hud/vgui/notice.h"
#include "hud/vgui/crosshair.h"
#include "hud/vgui/effect.h"
#include "hud/vgui/health.h"
#include "hud/vgui/ammobar.h"
#include "hud/vgui/dmgtiles.h"
#include "hud/vgui/gindicator.h"
#include "hud/vgui/deadmsg.h"
#include "hud/vgui/neteasemusic.h"
#include "hud/vgui/radar.h"
#include "hud/vgui/ammostack.h"
#include "hud/vgui/itemstack.h"
#include "hud/vgui/weaponstack.h"
#include "hud/vgui/weaponchoose.h"
#include "hud/vgui/itemhighlight.h"
#include "hud/vgui/indicator.h"
#include "Viewport.h"
#include "keydefs.h"

using namespace vgui;

const enum class HUDHIDE_BIT {
	HIDEWEAPONS = 0,
	HIDEFLASHLIGHT = 1,
	HIDEALL = 2,
	HIDEHEALTH = 3,
	HIDESELECTION = 4,
	HIDEBATTERY = 5,
	HIDECUSTOM1 = 6,
	HIDECUSTOM2 = 7
};

CViewport *g_pViewPort = nullptr;

vgui::HScheme GetViewPortBaseScheme() {
	return g_pViewPort->GetBaseScheme();
}

CViewport::CViewport(void) : Panel(nullptr, "ABCEnchanceViewport"){
	int swide, stall;
	surface()->GetScreenSize(swide, stall);
	SetScheme("ABCEnchanceScheme");
	SetBounds(0, 0, swide, stall);
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);

	m_hBaseScheme = vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "AbcScheme.res", "AbcScheme");
	vgui::GetAnimationController()->SetScriptFile(GetVPanel(), "abcenchance/HudAnimations.txt", true);

	//for popnumber
	m_pPlayerTitle = CREATE_CVAR("hud_playerinfo", "1", FCVAR_VALUE, nullptr);
	m_pPlayerTitleDanger = CREATE_CVAR("hud_playerinfo_danger", "30", FCVAR_VALUE, nullptr);
	m_pPopNumber = CREATE_CVAR("hud_popnumber", "1", FCVAR_VALUE, nullptr);
}

CViewport::~CViewport(void){
	for (auto panel : m_Panels) {
		panel->~IViewportPanel();
	}
}

void CViewport::Start(void){
	gPlayerRes.Init();
	gTeamRes.Init();
	gWR.Init();
#pragma region MyRegion
	g_EventPlayerInfoChanged.append([&](PlayerInfo* info) {
		this->m_pScorePanel->UpdateOnPlayerInfo(info->m_iIndex);
		});
	g_EventAmmoX.append([&](int, int) {
		this->m_pAmmoPanel->RefreshAmmo();
		});
	g_EventCustWeapon.append([&](int, const char*) {
		this->m_pWeaponChoose->ReloadWeaponSpr();
		this->m_pWeaponStack->ReloadWeaponSpr();
		});
	g_EventCurWeapon.append([&](int state, int id, int, int) {
		if (state > 0) {
			Weapon* pWeapon = gWR.GetWeapon(id);
			this->m_pAmmoPanel->SetWeapon(pWeapon);
			this->m_pCrossHairPanel->SetWeapon(pWeapon);
		}
		});
	auto hudhideCallback = [&](int token) {
		this->m_bitsHideHUDDisplay = token;
		auto bitSet = std::bitset<32>(token);
		if (bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEALL)))
			this->SetVisible(false);
		else
			this->SetVisible(true);
		this->m_pHealthPanel->SetArmorVisible(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEBATTERY)));
		this->m_pHealthPanel->SetHealthVisible(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEHEALTH)));
		if (HasSuit()) {
			this->m_pHealthPanel->ShowPanel(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEBATTERY)) || !bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEHEALTH)));
			this->m_pFlashLight->ShowPanel(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEFLASHLIGHT)));
			this->m_pAmmoPanel->ShowPanel(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEWEAPONS)));
		}
		};
	g_EventHideHUD.append(hudhideCallback);
	g_EventHideWeapon.append(hudhideCallback);
	g_EventWeaponSpr.append([&](int id, const char* name) {
		Weapon* wp = gWR.GetWeapon(id);
		if (wp && wp->iId > 0) {
			this->m_pWeaponChoose->ReloadWeaponSpr();
			this->m_pWeaponStack->ReloadWeaponSpr();
		}
		});
	g_EventWeapPickup.append([&](int index) {
		this->m_pWeaponStack->AddItemPickup(index);
		});
	g_EventAmmoPickup.append([&](int index, int count) {
		this->m_pAmmoStack->AddAmmoPickup(index, count);
		});
	g_EventItemPickup.append([&](const char* name) {
		auto index = gSpriteRes.GetSpriteIndex(name);
		if (!index.has_value())
			return;
		HSPRITE spr = gSpriteRes.GetSprite(index.value());
		wrect_t* rect = gSpriteRes.GetSpriteRect(index.value());
		this->m_pItemStack->AddItemPickup(spr, rect->left, rect->right, rect->top, rect->bottom);
		});
	g_EventDamage.append([&](int armor, int damage, int tiles, float* from) {
		this->UpdateTiles(tiles);
		if (!this->m_pIndicator->IsVisible())
			this->m_pIndicator->SetVisible(true);
		this->m_pIndicator->SetHitIndicator(damage, armor, from);
		});
	g_EventBattery.append([&](int armor) {
		this->m_pHealthPanel->SetArmor(armor);
		});
	g_EventHealth.append([&](int health) {
		this->m_pEffectPanel->SetHealth(health);
		this->m_pHealthPanel->SetHealth(health);
		});
	g_EventServerName.append([&](const char* name) {
		strncpy_s(this->m_szServerName, name, MAX_SERVERNAME_LENGTH - 1);
		this->m_pScorePanel->UpdateServerName();
		});
	g_EventNextMap.append([&](const char* map) {
		strncpy_s(m_szNextMapName, map, MAX_SERVERNAME_LENGTH - 1);
		this->m_pScorePanel->UpdateNextMap();
		});
	g_EventTimeEnd.append([&](int time) {
		this->m_iTimeEnd = time;
		this->m_pScorePanel->UpdateTimeEnd();
		});
	g_EventShowMenu.append([&](int slot, int time, int bits, const char* message) {
		this->m_pTextMenu->MsgShowMenu(slot, time, bits, message);
		});
	g_EventVoteMenu.append([&](int type, const char* content, const char* yes, const char* no) {
		this->m_pVotePanel->StartVote(content, yes, no, type);
		});
	g_EventEndVote.append([&]() {
		this->m_pVotePanel->EndVote();
		});
	g_EventMOTD.append([&](int code, const char* msg) {
		if (strlen(msg) > 0)
			this->m_pMOTDPanel->AppendMotd(msg);
		else
			this->m_pMOTDPanel->ShowPanel(false);
		if (code > 0)
			this->m_pMOTDPanel->FinishSendMOTD();
		});
	g_EventFlashBat.append([&](int flash) {
		this->m_pFlashLight->SetFlashBattery(flash);
		});
	g_EventFlashlight.append([&](bool on, int flash) {
		this->m_pFlashLight->SetFlashLight(on, flash);
		});
	g_EventTextMsg.append([&](int target, const char* msg, const char* sstr1, const char* sstr2, const char* sstr3, const char* sstr4) {
		const static std::wregex parttenSuicide(L" committed suicide.");
		const static std::wregex parttenKilled(L" was killed by a ");
		const static std::wregex parttenPlayer(L" : (.*) : ");
		vgui::CViewport::HUDNOTICE msg_dest = static_cast<vgui::CViewport::HUDNOTICE>(target);
		if (msg_dest == CViewport::HUDNOTICE::PRINTNOTIFY) {
			if (msg[0] == '\0')
				return true;
			wchar_t wideBuf[256];
			Q_UTF8ToUnicode(msg, wideBuf, sizeof(wideBuf));
			std::wstring stdSzBuf = wideBuf;
			std::wsmatch matched;
			bool found = false;
			found = regex_search(stdSzBuf, matched, parttenSuicide);
			if (found) {
				m_pDeahMsg->AddItem(matched.prefix().str().c_str(), L"", vgui::localize()->Find("DeathMsg_Suicide"));
				return false;
			}
			found = regex_search(stdSzBuf, matched, parttenKilled);
			if (found) {
				std::wstring k = matched.suffix().str();
				k.erase(k.end() - 2);
				m_pDeahMsg->AddItem(matched.prefix().str().c_str(), k.c_str(), vgui::localize()->Find("DeathMsg_MonsterKill"));
				return false;
			}
			found = regex_search(stdSzBuf, matched, parttenPlayer);
			if (found) {
				std::wstring e = matched.str();
				std::wstring sub = e.substr(3, e.length() - 6);
				std::wstring k = matched.prefix().str();
				m_pDeahMsg->AddItem(matched.suffix().str().c_str(), k.c_str(), sub.c_str());
				return false;
			}
		}
#define BUFFER_SIZE 256
		static auto findLocalize = [](std::string_view str) {
			if (str.front() == '#') {
				const wchar_t* find = vgui::localize()->Find(str.data());
				if (find) {
					char buffer[BUFFER_SIZE];
					Q_UnicodeToUTF8(find, buffer, BUFFER_SIZE);
					str = buffer;
				}
			}
			};
		int type = 0;
		findLocalize(msg);
		findLocalize(sstr1);
		if (strlen(sstr1) > 0)
			type++;
		findLocalize(sstr2);
		if (strlen(sstr2) > 0)
			type++;
		findLocalize(sstr3);
		if (strlen(sstr3) > 0)
			type++;
		findLocalize(sstr4);
		if (strlen(sstr4) > 0)
			type++;
		char buffer[BUFFER_SIZE * 4];
#undef BUFFER_SIZE
		std::string szBuf;
		switch (type) {
		case 1:Q_snprintf(buffer, msg, sstr1); szBuf = buffer; break;
		case 2:Q_snprintf(buffer, msg, sstr1, sstr2); szBuf = buffer; break;
		case 3:Q_snprintf(buffer, msg, sstr1, sstr2, sstr3); szBuf = buffer; break;
		case 4:Q_snprintf(buffer, msg, sstr1, sstr2, sstr3, sstr4); szBuf = buffer; break;
		case 0:
		default:szBuf = msg; break;
		}
		if (szBuf.size() > 0 && szBuf.back() == '\n')
			szBuf.pop_back();
		std::replace(szBuf.begin(), szBuf.end(), '\r', '\n');
		switch (msg_dest)
		{
		case vgui::CViewport::HUDNOTICE::PRINTNOTIFY:
		case vgui::CViewport::HUDNOTICE::PRINTCENTER:
			this->ShowNotice(msg_dest, szBuf.c_str());
			return false;
		}
		return true;
		});
	g_EventMetaHook.append([&](int type, mh_package_t* package) {
		switch (package->subtype) {
		case ABCCustomMsg::POPNUMBER: {
			if (this->m_pPopNumber->value <= 0)
				return;
			cl_entity_t* local = gEngfuncs.GetLocalPlayer();
			if (!local)
				return;
			Vector vecView;
			gEngfuncs.GetViewAngles(vecView);
			CMathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
			Vector vecLength;
			CMathlib::VectorSubtract(package->popnum.origin, local->curstate.origin, vecLength);
			vecLength = vecLength.Normalize();
			float angledotResult = CMathlib::DotProduct(vecLength, vecView);
			if (angledotResult > 0.5) {
				Color clr{
					package->popnum.color[0],
					package->popnum.color[1],
					package->popnum.color[2],
					package->popnum.color[3]
				};
				this->AddPopNumber(package->popnum.origin, clr, package->popnum.value);
			}
			break;
		}
		}
		});
	g_EventCmdSlot.append([&](int) {
		return this->IsTextMenuOpen();
		});
	g_EventCmdMissionBrief.append([&]() {
		this->ShowMOTD();
		return false;
		});
	g_EventCmdOpenScoreboard.append([&]() {
		this->m_bInScore = true;
		if (this && !this->m_iInterMission)
			this->ShowScoreBoard();
		return false;
		});
	g_EventCmdCloseScoreboard.append([&]() {
		this->m_bInScore = false;
		if (this && !this->m_iInterMission)
			this->HideScoreBoard();
		return false;
		});
	g_EventCmdVoteMenu.append([&]() {
		extern void OpenVoteMenuDialog();
		OpenVoteMenuDialog();
		return false;
		});
	g_EventCmdAttack1.append([&]() {
		return !this->m_pWeaponChoose->BlockAttackOnce();
		});
	g_EventHudUpdateClientData.append([&](client_data_t* cdata, float time) {
		//check spectate
		float newuser = gEngfuncs.GetLocalPlayer()->curstate.iuser1;
		static float iuser;
		if (iuser != newuser) {
			this->SetSpectate(newuser > 0);
			iuser = newuser;
		}
		if (!this->m_bitsWeaponBits.has_value() || this->m_bitsWeaponBits != cdata->iWeaponBits) {
			bool hasSuit = (cdata->iWeaponBits & (1 << WEAPON_SUIT)) != 0;
			m_pSidePanel->ShowPanel(hasSuit && (gCVars.pEccoEnable->value > 0));
			m_pFlashLight->ShowPanel(hasSuit);
			m_pCrossHairPanel->ShowPanel(hasSuit && (gCVars.pDynamicCrossHair->value > 0));
			m_pHealthPanel->ShowPanel(hasSuit);
			m_pAmmoPanel->ShowPanel(hasSuit);
			m_pDmgTiles->ShowPanel(hasSuit);
			m_pWeaponChoose->ShowPanel(hasSuit);
		}
		m_bitsWeaponBits = cdata->iWeaponBits;

		//check lj
		static bool lj = false;
		bool nlj = std::atoi(gEngfuncs.PhysInfo_ValueForKey("slj"));
		if (lj != nlj) {
			m_pHealthPanel->SetLongJump(nlj);
			lj = nlj;
		}
	});
	g_EventHudMousePressed.append([&](int code) {
		switch (code) {
		case vgui::MouseCode::MOUSE_LEFT: {
			this->GetWeaponChoosePanel()->SelectWeapon();
			break;
		}
		}
	});
#pragma endregion

	AddNewPanel(m_pRadar = new CRadarPanel());
	AddNewPanel(m_pEffectPanel = new CEffectPanel());
	AddNewPanel(m_pSidePanel = new CSidePanel());
	AddNewPanel(m_pItemHighLightPanel = new CItemHighLightPanel());
	AddNewPanel(m_pGIndicator = new CGenadeIndicatorPanel());
	AddNewPanel(m_pFlashLight = new CFlashLightPanel());
	AddNewPanel(m_pMOTDPanel = new CMotdPanel());
	for (size_t i = 0; i < 32; i++) {
		AddNewPanel(m_pPlayerInfoPanels[i] = new CPlayerInfoPanel());
		m_pPlayerInfoPanels[i]->SetId(i);
	}
	AddNewPanel(m_pHealthPanel = new CHealthPanel());
	AddNewPanel(m_pDeahMsg = new CDeathMsgPanel());
	AddNewPanel(m_pDmgTiles = new CDmgTilesPanel());
	AddNewPanel(m_pAmmoPanel = new CAmmoPanel());
	AddNewPanel(m_pNotice = new CNoticePanel("NoticePanel"));
	AddNewPanel(m_pNoticeCenter = new CNoticePanel("NoticeCenterPanel"));
	AddNewPanel(m_pTextMenu = new CTextMenu()); 
	AddNewPanel(m_pCrossHairPanel = new CCrosshairPanel());
#ifdef __HAS_NETEASE_API
	AddNewPanel(m_pNeteaseMusic = new CNeteasePanel());
#endif
	AddNewPanel(m_pVotePanel = new vgui::CVotePanel());
	AddNewPanel(m_pScorePanel = new vgui::CScorePanel());
	AddNewPanel(m_pAmmoStack = new CAmmoStackPanel());
	AddNewPanel(m_pItemStack = new CItemStackPanel());
	AddNewPanel(m_pWeaponStack = new CWeaponStackPanel());
	AddNewPanel(m_pWeaponChoose = new CWeaponChoosePanel());
	AddNewPanel(m_pIndicator = new CIndicatorPanel());
}

void CViewport::SetParent(VPANEL vPanel){
	BaseClass::SetParent(vPanel);
	m_pScorePanel->SetParent(GetVPanel());
	m_pVotePanel->SetParent(GetVPanel());
	m_pMOTDPanel->SetParent(GetVPanel());
	m_pSidePanel->SetParent(GetVPanel());
	m_pTextMenu->SetParent(GetVPanel());
	m_pFlashLight->SetParent(GetVPanel());
	m_pNotice->SetParent(GetVPanel());
	m_pNoticeCenter->SetParent(GetVPanel());
	m_pCrossHairPanel->SetParent(GetVPanel());
	m_pEffectPanel->SetParent(GetVPanel());
	m_pHealthPanel->SetParent(GetVPanel());
	m_pAmmoPanel->SetParent(GetVPanel());
	m_pDmgTiles->SetParent(GetVPanel());
	m_pGIndicator->SetParent(GetVPanel());
	m_pDeahMsg->SetParent(GetVPanel());
#ifdef __HAS_NETEASE_API
	m_pNeteaseMusic->SetParent(GetVPanel());
#endif
	m_pRadar->SetParent(GetVPanel());
	m_pAmmoStack->SetParent(GetVPanel());
	m_pItemStack->SetParent(GetVPanel());
	m_pWeaponStack->SetParent(GetVPanel());
	m_pWeaponChoose->SetParent(GetVPanel());
	m_pIndicator->SetParent(GetVPanel());
	for (size_t i = 0; i < 32; i++) {
		m_pPlayerInfoPanels[i]->SetParent(GetVPanel());
	}
}

void CViewport::AddNewPanel(IViewportPanel* panel){
	m_Panels.push_back(panel);
	panel->SetParent(GetVPanel());
	dynamic_cast<vgui::Panel*>(panel)->MakeReadyForUse();
}

void CViewport::Think(void){
	vgui::GetAnimationController()->UpdateAnimations(gEngfuncs.GetClientTime());
#ifdef __HAS_NETEASE_API
	m_pNeteaseMusic->Think();
#endif
	for (size_t i = 0; i < 32; i++) {
		m_pPlayerInfoPanels[i]->Think();
	}
}

void CViewport::VidInit(void){
	gSpriteRes.VidInit();
	gWR.VidInit();
	Reset();
}

vgui::HScheme CViewport::GetBaseScheme() {
	return m_hBaseScheme;
}

void CViewport::Reset() {
	for (IViewportPanel* pPanel : m_Panels)
		pPanel->Reset();
	gPlayerRes.ResetAll();
	gTeamRes.ResetAll();
	m_iInterMission = 0;
	m_bitsWeaponBits.reset();
	extern void CloseVoteMenuDialog();
	CloseVoteMenuDialog();
}

void CViewport::ActivateClientUI(void){
	SetVisible(true);
}

void CViewport::HideClientUI(void){
	SetVisible(false);
}

bool CViewport::KeyInput(int down, int keynum, const char* pszCurrentBinding){
	if (down){
		if (IsScoreBoardVisible()){
			if (!m_pScorePanel->IsMouseInputEnabled()){
				if (keynum == K_MOUSE2){
					m_pScorePanel->EnableMousePointer(true);
					return false;
				}
			}
		}
		return m_pVotePanel->KeyCodeTyped(keynum);
	}
	return true;
}
void CViewport::AddEntity(int type, cl_entity_s* ent, const char* modelname){
	m_pGIndicator->AddEntity(ent, modelname);
	m_pItemHighLightPanel->AddEntity(type, ent, modelname);
}

void CViewport::SetInterMission(int intermission) {
	if(intermission != m_iInterMission)
		m_pScorePanel->ShowPanel(intermission > 0);
	m_iInterMission = intermission;
}

bool CViewport::LoacalPlayerAvilable(){
	return gEngfuncs.GetLocalPlayer() != nullptr;
}

bool CViewport::IsScoreBoardVisible(){
	return m_pScorePanel->IsVisible();
}
void CViewport::ShowScoreBoard(){
	m_pScorePanel->ShowPanel(true);
}
void CViewport::HideScoreBoard(){
	m_pScorePanel->ShowPanel(false);
}
long CViewport::GetTimeEnd() {
	return m_iTimeEnd;
}
const char* CViewport::GetServerName(){
	return m_szServerName;
}
const char* CViewport::GetNextMap() {
	return m_szNextMapName;
}
bool CViewport::IsPlayerTileEnable() {
	return m_pPlayerTitle->value > 0;
}
bool CViewport::IsVoteEnable(){
	return m_pVotePanel->IsVoteEnable();
}
void CViewport::AddPopNumber(vec3_t vecOrigin, Color& pColor, int value){
	CPopNumberPanel* p = new CPopNumberPanel(vecOrigin, pColor, value);
	p->SetParent(GetVPanel());
	dynamic_cast<vgui::Panel*>(p)->MakeReadyForUse();
	p->ShowPanel(true);
}
void CViewport::ShowMOTD(){
	m_pMOTDPanel->ShowMotd();
}
void CViewport::ShowSideText(bool state){
	m_pSidePanel->ShowPanel(state);
}

void CViewport::ShowTextMenu(bool state){
	m_pTextMenu->ShowPanel(state);
}
void CViewport::SelectMenuItem(int slot){
	m_pTextMenu->SelectMenuItem(slot);
}
bool CViewport::IsTextMenuOpen(){
	return m_pTextMenu->IsVisible();
}
void CViewport::UpdateTiles(long tiles){
	m_pDmgTiles->UpdateTiles(tiles);
}
void CViewport::SetSpectate(bool state){
	m_pHealthPanel->ShowPanel(!state);
	m_pAmmoPanel->ShowPanel(!state);
}
void CViewport::ShowDeathMsg(bool state){
	m_pDeahMsg->ShowPanel(state);
}
#ifdef __HAS_NETEASE_API
void CViewport::ShowMusic(bool state){
	m_pNeteaseMusic->ShowPanel(state);
}
CNeteasePanel* CViewport::GetMusicPanel(){
	return m_pNeteaseMusic;
}
#endif
bool CViewport::IsInScore() {
	return m_bInScore;
}
CWeaponChoosePanel* CViewport::GetWeaponChoosePanel(){
	return m_pWeaponChoose;
}
void CViewport::ShowNotice(HUDNOTICE type, const char* message){
	switch (type)
	{
	case CViewport::HUDNOTICE::PRINTNOTIFY:
		m_pNotice->ShowMessage(message); break;
	case CViewport::HUDNOTICE::PRINTCENTER:
		m_pNoticeCenter->ShowMessage(message); break;
	default:
		break;
	}
}
CRadarPanel* CViewport::GetRadarPanel(){
	return m_pRadar;
}
void CViewport::Paint(void){
	BaseClass::Paint();
}

CVotePanel* CViewport::GetVotePanel(){
	return m_pVotePanel;
}

CMotdPanel* CViewport::GetMotdPanel(){
	return m_pMOTDPanel;
}


Color CViewport::GetPlayerColor(int index){
	vec3_t color;
	CMathlib::VectorCopy(gHookFuncs.GetClientColor(index), color);
	return Color(color[0] * 255, color[1] * 255, color[2] * 255, 255);
}

void CViewport::ShowCrossHair(bool on) {
	m_pCrossHairPanel->ShowPanel(on);
}

bool CViewport::HasSuit() {
	if (!m_bitsWeaponBits.has_value())
		return false;
	constexpr auto WEAPON_SUIT = 31;
	return (m_bitsWeaponBits.value() & (1 << WEAPON_SUIT)) != 0;
}

bool CViewport::SelectTextMenuItem(int slot) {
	if (IsTextMenuOpen()) {
		SelectMenuItem(slot);
		return true;
	}
	return false;
}
bool CViewport::IsHudHide(int HideToken) {
	return (m_bitsHideHUDDisplay & HideToken) != 0;
}
void Viewport_PickupWeapon(Weapon* wp) {
	g_pViewPort->GetWeaponChoosePanel()->InsertWeapon(wp);
}
void Viewport_DropWeapon(Weapon* wp) {
	g_pViewPort->GetWeaponChoosePanel()->RemoveWeapon(wp);
}
void Viewport_ChooseWeapon(Weapon* wp) {
	g_pViewPort->GetWeaponChoosePanel()->ChooseWeapon(wp);
}
void Viewport_SelectWeapon() {
	g_pViewPort->GetWeaponChoosePanel()->SelectWeapon();
}
void Viewport_DropAllWeapon() {
	g_pViewPort->GetWeaponChoosePanel()->Reset();
}

inline vgui::CViewport* GetBaseViewPort()
{
	return g_pViewPort;
}
