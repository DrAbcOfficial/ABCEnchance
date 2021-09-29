#include <metahook.h>
#include <string>
#include <map>
#include "pmtrace.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "pm_defs.h"
#include "parsemsg.h"
#include "msghook.h"
#include "glew.h"
#include "drawElement.h"

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

#include "CHudDelegate.h"
#include <local.h>

CHudDelegate* gHudDelegate = NULL;
cl_hookedHud gHookHud;

pfnUserMsgHook m_pfnScoreInfo;
int __MsgFunc_ScoreInfo(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	//wtf is not this shit
	if (clientIndex >= 1 && clientIndex <= 32) {
		gHudDelegate->m_Playerinfo[clientIndex].index = clientIndex;
		gHudDelegate->m_Playerinfo[clientIndex].frags = READ_FLOAT();
		gHudDelegate->m_Playerinfo[clientIndex].death = READ_LONG();
		gHudDelegate->m_Playerinfo[clientIndex].health = READ_FLOAT();
		gHudDelegate->m_Playerinfo[clientIndex].armor = READ_FLOAT();
		gHudDelegate->m_Playerinfo[clientIndex].team = READ_BYTE();
		gHudDelegate->m_Playerinfo[clientIndex].donors = READ_CHAR();
		gHudDelegate->m_Playerinfo[clientIndex].admin = READ_CHAR();
	}
	return m_pfnScoreInfo(pszName, iSize, pbuf);
}

void CHudDelegate::GL_Init(void){
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
}
void CHudDelegate::HUD_VidInit(void){
	int iRes = ScreenWidth < 640 ? 320 : 640;
	if (!m_pSpriteList){
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList){
			m_iSpriteCount = 0;
			client_sprite_t* p = m_pSpriteList;
			int j;
			for (j = 0; j < m_iSpriteCountAllRes; j++){
				if (p->iRes == iRes)
					m_iSpriteCount++;
				p++;
			}
			m_rghSprites = new HSPRITE[m_iSpriteCount];
			m_rgrcRects = new wrect_t[m_iSpriteCount];
			m_rgszSpriteNames = new char[m_iSpriteCount * MAX_SPRITE_NAME_LENGTH];

			p = m_pSpriteList;
			int index = 0;
			for (j = 0; j < m_iSpriteCountAllRes; j++){
				if (p->iRes == iRes){
					char sz[256];
					sprintf(sz, "sprites/%s.spr", p->szSprite);
					m_rghSprites[index] = SPR_Load(sz);
					m_rgrcRects[index] = p->rc;
					strncpy(&m_rgszSpriteNames[index * MAX_SPRITE_NAME_LENGTH], p->szName, MAX_SPRITE_NAME_LENGTH);

					index++;
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
				m_rghSprites[index] = SPR_Load(sz);
				index++;
			}
			p++;
		}
	}
	m_HudArmorHealth.VidInit();
	m_HudCustomAmmo.VidInit();
	m_HudRadar.VidInit();
}
void CHudDelegate::HUD_Draw(float flTime){
	if (gCVars.pDynamicHUD->value <= 0)
		return;
	m_HudEfx.Draw(flTime);
	m_HudPlayerTitle.Draw(flTime);
	m_HudCustomAmmo.Draw(flTime);
	m_HudArmorHealth.Draw(flTime);
	m_HudRadar.Draw(flTime);
	m_HudDeathMsg.Draw(flTime);
	m_HudCrosshair.Draw(flTime);
	m_HudVote.Draw(flTime);
	m_HudEccoMoney.Draw(flTime);
	m_HudItemHighLight.Draw(flTime);
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
int CHudDelegate::GetSpriteIndex(const char* SpriteName){
	for (int i = 0; i < m_iSpriteCount; i++){
		if (strncmp(SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH) == 0)
			return i;
	}
	return -1; // invalid sprite
}
bool CHudDelegate::IsInSpectate(){
	return gEngfuncs.GetLocalPlayer()->curstate.iuser1 > 0;
}
void CHudDelegate::CL_CreateMove(float frametime, usercmd_s* cmd, int active){
}
vgui::ISurface* CHudDelegate::surface(){
	return g_pSurface;
}
CHudDelegate :: ~CHudDelegate(){
	delete[] m_rghSprites;
	delete[] m_rgrcRects;
	delete[] m_rgszSpriteNames;
}