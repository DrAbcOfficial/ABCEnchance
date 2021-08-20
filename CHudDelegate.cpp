#include <metahook.h>

#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>

#include "CColor.h"
#include "hud.h"
#include "ammo.h"
#include "healthhud.h"
#include "CHudDelegate.h"

CHudDelegate* gHudDelegate = NULL;
void CHudDelegate::HUD_Init(void)
{
	m_HudArmorHealth.Init();
}
void CHudDelegate::HUD_Draw(float flTime)
{
	m_HudArmorHealth.Draw(flTime);
}
void CHudDelegate::HUD_Reset(void)
{
	m_HudArmorHealth.Reset();
}