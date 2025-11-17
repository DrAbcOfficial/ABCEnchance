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