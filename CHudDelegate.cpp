#include <metahook.h>

#include "vguilocal.h"

#include "hud.h"
#include "weapon.h"
#include "ammo.h"
#include "healthhud.h"

#include "CHudDelegate.h"



CHudDelegate* gHudDelegate = NULL;
cl_hookedHud gHookHud;

void CHudDelegate::HUD_Init(void)
{
	m_HudArmorHealth.Init();
	m_HudCustomAmmo.Init();
}
void CHudDelegate::HUD_VidInit(void)
{

	if (ScreenWidth < 640)
		m_iRes = 320;
	else
		m_iRes = 640;

	// Only load this once
	if (!m_pSpriteList)
	{
		// we need to load the hud.txt, and all sprites within
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList)
		{
			// count the number of sprites of the appropriate res
			m_iSpriteCount = 0;
			client_sprite_t* p = m_pSpriteList;
			int j;
			for (j = 0; j < m_iSpriteCountAllRes; j++)
			{
				if (p->iRes == m_iRes)
					m_iSpriteCount++;
				p++;
			}

			// allocated memory for sprite handle arrays
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
		// we have already have loaded the sprite reference from hud.txt, but
		// we need to make sure all the sprites have been loaded (we've gone through a transition, or loaded a save game)
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
	m_HudCustomAmmo.VidInit();
}
void CHudDelegate::HUD_Draw(float flTime)
{
	m_HudArmorHealth.Draw(flTime);
	m_HudCustomAmmo.Draw(flTime);
}
void CHudDelegate::HUD_Reset(void)
{
	m_HudArmorHealth.Reset();
	m_HudCustomAmmo.Reset();
}

int CHudDelegate::GetSpriteIndex(const char* SpriteName)
{
	// look through the loaded sprite name list for SpriteName
	for (int i = 0; i < m_iSpriteCount; i++)
	{
		if (strncmp(SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH) == 0)
			return i;
	}

	return -1; // invalid sprite
}

// CHud destructor
// cleans up memory allocated for m_rg* arrays
CHudDelegate :: ~CHudDelegate()
{
	delete[] m_rghSprites;
	delete[] m_rgrcRects;
	delete[] m_rgszSpriteNames;
}