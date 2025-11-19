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
	GetBaseViewPort()->GetRadarPanel()->RenderRadar();
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
	gCVars.pDamageScreenFilter = CREATE_CVAR("hud_damageshock", "1", FCVAR_VALUE, nullptr);
	gCVars.pDamageScreenFactor = CREATE_CVAR("hud_damageshock_factor", "0.015", FCVAR_VALUE, nullptr);
	gCVars.pDamageScreenBase = CREATE_CVAR("hud_damageshock_base", "15", FCVAR_VALUE, nullptr);
	gCVars.pDangerHealth = CREATE_CVAR("hud_danger_health", "45", FCVAR_VALUE, nullptr);
	gCVars.pDangerArmor = CREATE_CVAR("hud_danger_armor", "45", FCVAR_VALUE, nullptr);

	m_HudIndicator.Init();
#ifdef _DEBUG
	m_HudCCTV.Init();
#endif
}

void CCustomHud::HUD_VidInit(void)
{
	gSpriteRes.VidInit();
	m_HudIndicator.VidInit();
	gWR.VidInit();
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
#ifdef _DEBUG
	m_HudCCTV.Reset();
#endif
	m_bitsWeaponBits.reset();
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
		GetBaseViewPort()->WeaponBitsChangeCallback(cdata->iWeaponBits);
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
void CCustomHud::OnMousePressed(int code) {
	switch (code) {
		case vgui::MouseCode::MOUSE_LEFT: {
			GetBaseViewPort()->GetWeaponChoosePanel()->SelectWeapon();
			break;
		}
	}
}
#pragma endregion