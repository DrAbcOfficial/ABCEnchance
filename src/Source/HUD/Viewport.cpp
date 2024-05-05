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
#include <vgui_controls/Label.h>
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"
#include "steam_api.h"
#include "player_info.h"
#include "hud.h"

#include "parsemsg.h"
#include "mymathlib.h"

#include "motd.h"
#include "popnum.h"
#include "playerboard.h"
#include "scoreboard.h"
#include "vote.h"
#include "sidetext.h"
#include "textmenu.h"
#include "flashlight.h"
#include "notice.h"
#include "crosshair.h"
#include "effect.h"
#include "health.h"
#include "ammobar.h"
#include "dmgtiles.h"
#include "gindicator.h"
#include "deadmsg.h"
#include "neteasemusic.h"
#include "radar.h"
#include "ammostack.h"
#include "itemstack.h"
#include "weaponstack.h"
#include "weaponchoose.h"
#include "itemhighlight.h"

#include "CCustomHud.h"

#include "Viewport.h"
#include "exportfuncs.h"
#include "keydefs.h"

using namespace vgui;

CViewport *g_pViewPort = nullptr;

vgui::HScheme GetViewPortBaseScheme() {
	return g_pViewPort->GetBaseScheme();
}

CViewport::CViewport(void) : Panel(nullptr, "ABCEnchanceViewport"){
	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	MakePopup(false, true);

	SetScheme("ABCEnchanceScheme");
	SetBounds(0, 0, swide, stall);
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	SetProportional(true);

	m_hBaseScheme = vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "AbcScheme.res", "AbcScheme");
	vgui::GetAnimationController()->SetScriptFile(GetVPanel(), "abcenchance/HudAnimations.txt", true);

	//for popnumber
	m_pPlayerTitle = CREATE_CVAR("cl_playertitle", "1", FCVAR_VALUE, nullptr);
	m_pPlayerTitleDanger = CREATE_CVAR("cl_playertitle_danger", "30", FCVAR_VALUE, nullptr);
	m_pPopNumber = CREATE_CVAR("cl_popnumber", "1", FCVAR_VALUE, nullptr);
}

CViewport::~CViewport(void){
	for (auto panel : m_Panels) {
		panel->~IViewportPanel();
	}
}

void CViewport::Start(void){
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
	SetVisible(false);
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
	Reset();
}

vgui::HScheme CViewport::GetBaseScheme() {
	return m_hBaseScheme;
}

void CViewport::Reset() {
	for (IViewportPanel* pPanel : m_Panels)
		pPanel->Reset();
	CPlayerInfo::GetThisPlayerInfo()->ResetAll();
	m_iInterMission = 0;
}

void CViewport::Init(void){
	CPlayerInfo::GetThisPlayerInfo()->InitPlayerInfos();
	CTeamInfo::InitTeamInfos();
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
int CViewport::GetInterMission() {
	return m_iInterMission;
}

bool CViewport::LoacalPlayerAvilable(){
	return gEngfuncs.GetLocalPlayer() != nullptr;
}

bool CViewport::IsScoreBoardVisible(){
	return m_pScorePanel->IsVisible();
}
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
void CViewport::HudHideCallBack(int code){
	auto bitSet = std::bitset<32>(code);
	if (bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEALL)))
		SetVisible(false);
	else
		SetVisible(true);
	m_pHealthPanel->SetArmorVisible(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEBATTERY)));
	m_pHealthPanel->SetHealthVisible(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEHEALTH)));
	if (HasSuit()) {
		m_pHealthPanel->ShowPanel(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEBATTERY)) || !bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEHEALTH)));
		m_pFlashLight->ShowPanel(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEFLASHLIGHT)));
		m_pAmmoPanel->ShowPanel(!bitSet.test(static_cast<size_t>(HUDHIDE_BIT::HIDEWEAPONS)));
	}	
}
void CViewport::ShowScoreBoard(){
	m_pScorePanel->ShowPanel(true);
}
void CViewport::LongjumpCallBack(bool state){
	m_pHealthPanel->SetLongJump(state);
}
void CViewport::HideScoreBoard(){
	m_pScorePanel->ShowPanel(false);
}
long CViewport::GetTimeEnd() {
	return m_iTimeEnd;
}
char* CViewport::GetServerName(){
	return m_szServerName;
}
char* CViewport::GetNextMap() {
	return m_szNextMapName;
}
bool CViewport::IsPlayerTileEnable() {
	return m_pPlayerTitle->value > 0;
}
bool CViewport::IsVoteEnable(){
	return m_pVotePanel->IsVoteEnable();
}
void CViewport::StartVote(char* szContent, char* szYes, char* szNo, int iVoteType){
	m_pVotePanel->StartVote(szContent, szYes, szNo, iVoteType);
}
void CViewport::EndVote(){
	m_pVotePanel->EndVote();
}
void CViewport::AddPopNumber(vec3_t vecOrigin, Color& pColor, int value){
	CPopNumberPanel* p = new CPopNumberPanel(vecOrigin, pColor, value);
	p->SetParent(GetVPanel());
	dynamic_cast<vgui::Panel*>(p)->MakeReadyForUse();
	p->ShowPanel(true);
}
void CViewport::AppendMOTD(char* szMessage) {
	m_pMOTDPanel->AppendMotd(szMessage);
}
void CViewport::ShowMOTD(){
	m_pMOTDPanel->ShowMotd();
}
void CViewport::CloseMOTD(){
	m_pMOTDPanel->ShowPanel(false);
}
void CViewport::FinishSendMOTD() {
	m_pMOTDPanel->FinishSendMOTD();
}
void CViewport::ShowSideText(bool state){
	m_pSidePanel->ShowPanel(state);
}

bool CViewport::MsgShowMenu(const char* pszName, int iSize, void* pbuf){
	return m_pTextMenu->MsgShowMenu(pszName, iSize, pbuf);
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
void CViewport::SetFlashLight(bool on, int battery){
	m_pFlashLight->SetFlashLight(on, battery);
}
void CViewport::SetFlashBattery(int battery){
	m_pFlashLight->SetFlashBattery(battery);
}
void CViewport::SetHealth(int health){
	m_pEffectPanel->SetHealth(health);
	m_pHealthPanel->SetHealth(health);
}
void CViewport::SetArmor(int armor) {
	m_pHealthPanel->SetArmor(armor);
}
void CViewport::UpdateTiles(long tiles){
	m_pDmgTiles->UpdateTiles(tiles);
}
void CViewport::SetSpectate(bool state){
	m_pHealthPanel->ShowPanel(!state);
	m_pAmmoPanel->ShowPanel(!state);
}
bool CViewport::TextMsg(const char* pszName, int iSize, void* pbuf){
	const static std::wregex parttenSuicide(L" committed suicide.");
	const static std::wregex parttenKilled(L" was killed by a ");
	const static std::wregex parttenPlayer(L" : (.*) : ");
	BEGIN_READ(pbuf, iSize);
	if (READ_BYTE() == static_cast<int>(CViewport::HUDNOTICE::PRINTNOTIFY)) {
		char* msg_text = READ_STRING();
		//什么鸡巴东西
		if (msg_text[0] == '\0')
			return false;
		wchar_t wideBuf[256];
		Q_UTF8ToUnicode(msg_text, wideBuf, sizeof(wideBuf));
		std::wstring stdSzBuf = wideBuf;
		//剩下的老子才没兴趣
		//正则捕获匹配
		std::wsmatch matched;
		bool found = false;
		found = regex_search(stdSzBuf, matched, parttenSuicide);
		if (found) {
			m_pDeahMsg->AddItem(matched.prefix().str().c_str(), L"", vgui::localize()->Find("DeathMsg_Suicide"));
			return true;
		}
		found = regex_search(stdSzBuf, matched, parttenKilled);
		if (found) {
			std::wstring k = matched.suffix().str();
			k.erase(k.end() - 2);
			m_pDeahMsg->AddItem(matched.prefix().str().c_str(), k.c_str(), vgui::localize()->Find("DeathMsg_MonsterKill"));
			return true;
		}
		found = regex_search(stdSzBuf, matched, parttenPlayer);
		if (found) {
			std::wstring e = matched.str();
			std::wstring sub = e.substr(3, e.length() - 6);
			std::wstring k = matched.prefix().str();
			m_pDeahMsg->AddItem(matched.suffix().str().c_str(), k.c_str(), sub.c_str());
			return true;
		}
	}
	return false;
}
void CViewport::ShowDeathMsg(bool state){
	m_pDeahMsg->ShowPanel(state);
}
void CViewport::EraseHighLight(cl_entity_t* var, int modelindex){
	m_pItemHighLightPanel->EraseHighLight(var, modelindex);
}
#ifdef __HAS_NETEASE_API
void CViewport::ShowMusic(bool state){
	m_pNeteaseMusic->ShowPanel(state);
}
CNeteasePanel* CViewport::GetMusicPanel(){
	return m_pNeteaseMusic;
}
#endif
CAmmoStackPanel* CViewport::GetAmmoStackPanel(){
	return m_pAmmoStack;
}
CItemStackPanel* CViewport::GetItemStackPanel(){
	return m_pItemStack;
}
CWeaponStackPanel* CViewport::GetWeaponStackPanel(){
	return m_pWeaponStack;
}
CWeaponChoosePanel* CViewport::GetWeaponChoosePanel(){
	return m_pWeaponChoose;
}
void CViewport::SetCurWeapon(WEAPON* weapon){
	m_pAmmoPanel->SetWeapon(weapon);
	m_pCrossHairPanel->SetWeapon(weapon);
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

CScorePanel* CViewport::GetScoreBoard() {
	return m_pScorePanel;
}

CVotePanel* CViewport::GetVotePanel(){
	return m_pVotePanel;
}

CMotdPanel* CViewport::GetMotdPanel(){
	return m_pMOTDPanel;
}

CAmmoPanel* CViewport::GetAmmoPanel(){
	return m_pAmmoPanel;
}

Color CViewport::GetPlayerColor(int index){
	vec3_t color;
	CMathlib::VectorCopy(gHookFuncs.GetClientColor(index), color);
	return Color(color[0] * 255, color[1] * 255, color[2] * 255, 255);
}

void CViewport::ShowCrossHair(bool on) {
	m_pCrossHairPanel->ShowPanel(on);
}

bool CViewport::IsInSpectate() {
	return gCustomHud.IsInSpectate();
}
bool CViewport::HasSuit() {
	return gCustomHud.HasSuit();
}
void CViewport::WeaponBitsChangeCallback(int bits){
	bool hasSuit = (bits & (1 << WEAPON_SUIT)) != 0;
	m_pSidePanel->ShowPanel(hasSuit && (gCVars.pEccoEnable->value > 0));
	m_pFlashLight->ShowPanel(hasSuit);
	m_pCrossHairPanel->ShowPanel(hasSuit && (gCVars.pDynamicCrossHair->value > 0));
	m_pHealthPanel->ShowPanel(hasSuit);
	m_pAmmoPanel->ShowPanel(hasSuit);
	m_pDmgTiles->ShowPanel(hasSuit);
	m_pWeaponChoose->ShowPanel(hasSuit);
}
bool CViewport::IsHudHide(int HideToken) {
	return gCustomHud.IsHudHide(HideToken);
}
void Viewport_PickupWeapon(WEAPON* wp) {
	g_pViewPort->GetWeaponChoosePanel()->InsertWeapon(wp);
}
void Viewport_DropWeapon(WEAPON* wp) {
	g_pViewPort->GetWeaponChoosePanel()->RemoveWeapon(wp);
}
void Viewport_ChooseWeapon(WEAPON* wp) {
	g_pViewPort->GetWeaponChoosePanel()->ChooseWeapon(wp);
}
void Viewport_SelectWeapon() {
	g_pViewPort->GetWeaponChoosePanel()->SelectWeapon();
}
void Viewport_DropAllWeapon() {
	g_pViewPort->GetWeaponChoosePanel()->Reset();
}