#include <metahook.h>
#include <string>
#include "pmtrace.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "ammo.h"
#include "healthhud.h"
#include "deathmsg.h"
#include "glew.h"
#include "radar.h"
#include "deathmsg.h"
#include "crosshair.h"

#include "CHudDelegate.h"
#include <local.h>

CHudDelegate* gHudDelegate = NULL;
cl_hookedHud gHookHud;

void CHudDelegate::GL_Init(void)
{
	m_HudRadar.GLInit();
}
void CHudDelegate::HUD_Init(void)
{
	m_HudArmorHealth.Init();
	m_HudCustomAmmo.Init();
	m_HudRadar.Init();
	m_HudDeathMsg.Init();
	m_HudCrosshair.Init();
}
void CHudDelegate::HUD_VidInit(void)
{
	if (ScreenWidth < 640)
		m_iRes = 320;
	else
		m_iRes = 640;
	if (!m_pSpriteList)
	{
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList)
		{
			m_iSpriteCount = 0;
			client_sprite_t* p = m_pSpriteList;
			int j;
			for (j = 0; j < m_iSpriteCountAllRes; j++)
			{
				if (p->iRes == m_iRes)
					m_iSpriteCount++;
				p++;
			}
			m_rghSprites = new HSPRITE[m_iSpriteCount];
			m_rgrcRects = new wrect_t[m_iSpriteCount];
			m_rgszSpriteNames = new char[m_iSpriteCount * MAX_SPRITE_NAME_LENGTH];

			p = m_pSpriteList;
			int index = 0;
			for (j = 0; j < m_iSpriteCountAllRes; j++)
			{
				if (p->iRes == m_iRes)
				{
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
	else
	{
		client_sprite_t* p = m_pSpriteList;
		int index = 0;
		for (int j = 0; j < m_iSpriteCountAllRes; j++)
		{
			if (p->iRes == m_iRes)
			{
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
void CHudDelegate::HUD_Draw(float flTime)
{
	if (gCVars.pDynamicHUD->value <= 0)
		return;
	m_HudArmorHealth.Draw(flTime);
	m_HudCustomAmmo.Draw(flTime);
	m_HudRadar.Draw(flTime);
	m_HudDeathMsg.Draw(flTime);
	m_HudCrosshair.Draw(flTime);
}
void CHudDelegate::HUD_Reset(void)
{
	m_iPlayerHealth = 100;
	m_flOverViewScale = 0;
	m_HudArmorHealth.Reset();
	m_HudCustomAmmo.Reset();
	m_HudRadar.Reset();
	m_HudDeathMsg.Reset();
	m_HudCrosshair.Reset();
}
void CHudDelegate::HUD_UpdateClientData(client_data_t* cdata, float time)
{
}
void CHudDelegate::HUD_ClientMove(struct playermove_s* ppmove, qboolean server)
{
	m_HudCustomAmmo.ClientMove(ppmove, server);
}
void CHudDelegate::HUD_Clear(void)
{
	m_HudRadar.Clear();
}
void CHudDelegate::HUD_PreRenderView(int a1)
{
	if (gCVars.pDynamicHUD->value <= 0)
		return;
	m_HudRadar.PreRenderView(a1);
}
void CHudDelegate::IN_MouseEvent(int mstate)
{
	if (gCVars.pDynamicHUD->value <= 0)
		return;
	m_HudCustomAmmo.IN_MouseEvent(mstate);
}
void CHudDelegate::IN_Accumulate(void)
{
	if (gCVars.pDynamicHUD->value <= 0)
		return;
	m_HudCustomAmmo.IN_Accumulate();
}
void CHudDelegate::CL_CreateMove(float frametime, usercmd_s* cmd, int active)
{
	
}
int CHudDelegate::GetSpriteIndex(const char* SpriteName)
{
	for (int i = 0; i < m_iSpriteCount; i++)
	{
		if (strncmp(SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH) == 0)
			return i;
	}
	return -1; // invalid sprite
}
bool CHudDelegate::IsInSpectate()
{
	return gEngfuncs.GetLocalPlayer()->curstate.iuser1 > 0;
}
vgui::ISurface* CHudDelegate::surface()
{
	return g_pSurface;
}
CHudDelegate :: ~CHudDelegate()
{
	delete[] m_rghSprites;
	delete[] m_rgrcRects;
	delete[] m_rgszSpriteNames;
}