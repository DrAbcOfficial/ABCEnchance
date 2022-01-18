#include <metahook.h>
#include <string>
#include <map>

#include "glew.h"
#include "gl_shader.h"
#include "gldef.h"

#include "pmtrace.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "pm_defs.h"
#include "parsemsg.h"
#include "drawElement.h"
#include "local.h"

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

#include "CHudDelegate.h"

CHudDelegate* gHudDelegate = NULL;
cl_hookedHud gHookHud;

SHADER_DEFINE(pp_gaussianblurh)
SHADER_DEFINE(pp_gaussianblurv)

pfnUserMsgHook m_pfnScoreInfo;
int __MsgFunc_ScoreInfo(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	//wtf is not this shit
	if (clientIndex >= 1 && clientIndex <= 33) {
		hud_playerinfo_t* info = gHudDelegate->GetPlayerHUDInfo(clientIndex);
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

void CHudDelegate::GL_Init(void){
	pp_gaussianblurh.program = R_CompileShaderFileEx("renderer\\shader\\fullscreentriangle.vert.glsl", "renderer\\shader\\gaussian_blur_16x.frag.glsl", "", "#define BLUR_HORIZONAL\n", NULL);
	if (pp_gaussianblurh.program)
		SHADER_UNIFORM(pp_gaussianblurh, du, "du");
	pp_gaussianblurv.program = R_CompileShaderFileEx("renderer\\shader\\fullscreentriangle.vert.glsl", "renderer\\shader\\gaussian_blur_16x.frag.glsl", "", "#define BLUR_VERTICAL\n", NULL);
	if (pp_gaussianblurv.program)
		SHADER_UNIFORM(pp_gaussianblurv, du, "du");

	m_HudRadar.GLInit();
	m_HudCustomAmmo.GLInit();
}
void CHudDelegate::HUD_Init(void){
	m_pfnScoreInfo = HOOK_MESSAGE(ScoreInfo);

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
}
void CHudDelegate::HUD_VidInit(void){
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
}
void CHudDelegate::HUD_Draw(float flTime){
	if (gCVars.pDynamicHUD->value <= 0)
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
}
void CHudDelegate::HUD_Reset(void){
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
	memset(m_Playerinfo, 0, sizeof(m_Playerinfo));
}
void CHudDelegate::HUD_UpdateClientData(client_data_t* cdata, float time){
	m_bPlayerLongjump = atoi(gEngfuncs.PhysInfo_ValueForKey("slj"));
}
void CHudDelegate::HUD_ClientMove(struct playermove_s* ppmove, qboolean server){
	m_HudCustomAmmo.ClientMove(ppmove, server);
}
void CHudDelegate::HUD_Clear(void){
	m_HudRadar.Clear();
	m_HudCustomAmmo.Clear();
	m_HudEccoBuyMenu.Clear();
}
void CHudDelegate::HUD_PreRenderView(int a1){
	if (gCVars.pDynamicHUD->value <= 0)
		return;
	m_HudRadar.PreRenderView(a1);
}
void CHudDelegate::HUD_PostRenderView(int a1){
	if (gCVars.pDynamicHUD->value <= 0)
		return;
}
void CHudDelegate::IN_MouseEvent(int mstate){
	if (gCVars.pDynamicHUD->value <= 0)
		return;
}
int CHudDelegate::HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding){
	int result = 1;
	result &= m_HudVote.HUD_KeyEvent(eventcode, keynum, pszCurrentBinding);
	return result;
}
void CHudDelegate::IN_Accumulate(void){
	if (gCVars.pDynamicHUD->value <= 0)
		return;
	m_HudCustomAmmo.IN_Accumulate();
}
int CHudDelegate::HUD_AddEntity(int type, cl_entity_s* ent, const char* modelname)
{
	m_HudItemHighLight.AddEntity(type, ent, modelname);
	return 0;
}
void CHudDelegate::CL_CreateMove(float frametime, usercmd_s* cmd, int active) {
}
bool CHudDelegate::IsInSpectate() {
	return gEngfuncs.GetLocalPlayer()->curstate.iuser1 > 0;
}

HSPRITE CHudDelegate::GetSprite(int index) {
	return (index < 0) ? 0 : m_arySprites[index]->spr;
}
wrect_t* CHudDelegate::GetSpriteRect(int index) {
	if(index >= 0 && index < m_arySprites.size())
		return &m_arySprites[index]->rect;
	return nullptr;
}
int CHudDelegate::GetSpriteIndex(const char* SpriteName){
	for (int i = 0; i < m_arySprites.size(); i++){
		if (strncmp(SpriteName, m_arySprites[i]->name, MAX_SPRITE_NAME_LENGTH) == 0)
			return i;
	}
	return -1; // invalid sprite
}
hud_playerinfo_t* CHudDelegate::GetPlayerHUDInfo(int index){
	if (index > 0 && index <= 33)
		return &m_Playerinfo[index];
	return nullptr;
}
vgui::ISurface* CHudDelegate::surface(){
	return g_pSurface;
}
CHudDelegate :: ~CHudDelegate(){
	m_arySprites.clear();
}