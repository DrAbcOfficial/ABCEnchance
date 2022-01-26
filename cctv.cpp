#include <metahook.h>
#include "glew.h"
#include "mathlib.h"
#include "local.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "CHudDelegate.h"

#include "gl_shader.h"
#include "gl_def.h"
#include "gl_utility.h"
#include "gl_draw.h"

#include "cctv.h"

#define DOWN_LOGO_STARTX 0.05
#define DOWN_LOGO_STARTY 0.75
#define DOWN_LOGO_SIZE 0.15

#define LOGO_STARTX 0.05
#define LOGO_STARTY 0.05
#define LOGO_WIDTH 0.2
#define LOGO_HEIGHT 0.1


#define TILE_STARTX 0.95
#define TILE_STARTY 0.35
#define TILE_WIDHT 0.04
#define TILE_HEIGHT 0.35

CHudCCTV m_HudCCTV;

void CHudCCTV::GLInit() {

}
int CHudCCTV::Init(){
	gCVars.pCCTV = CREATE_CVAR("cl_cctvlogo", "0", FCVAR_VALUE, nullptr);

	DownLogoStartX = mathlib::GetScreenPixel(ScreenWidth, DOWN_LOGO_STARTX);
	DownLogoStartY = mathlib::GetScreenPixel(ScreenHeight, DOWN_LOGO_STARTY);
	DownLogoSize = mathlib::GetScreenPixel(ScreenHeight, DOWN_LOGO_SIZE);

	LogoStartX = mathlib::GetScreenPixel(ScreenWidth, LOGO_STARTX);
	LogoStartY = mathlib::GetScreenPixel(ScreenHeight, LOGO_STARTY);
	LogoWidth = mathlib::GetScreenPixel(ScreenWidth, LOGO_WIDTH);
	LogoHeight = mathlib::GetScreenPixel(ScreenHeight, LOGO_HEIGHT);
	TileStartX = mathlib::GetScreenPixel(ScreenWidth, TILE_STARTX);
	TileStartY = mathlib::GetScreenPixel(ScreenHeight, TILE_STARTY);
	TileWidth = mathlib::GetScreenPixel(ScreenWidth, TILE_WIDHT);
	TileHeight = mathlib::GetScreenPixel(ScreenHeight, TILE_HEIGHT);
	Reset();
	return 0;
}
int CHudCCTV::Draw(float flTime){
	if (gCVars.pCCTV->value <= 0)
		return 1;

	gHudDelegate->surface()->DrawSetTexture(-1);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255);
	gHudDelegate->surface()->DrawSetTexture(CCTVDownTga);
	gHudDelegate->surface()->DrawTexturedRect(DownLogoStartX, DownLogoStartY, DownLogoStartX + DownLogoSize, DownLogoStartY + DownLogoSize);

	gHudDelegate->surface()->DrawSetTexture(CCTVLogoTga);
	gHudDelegate->surface()->DrawTexturedRect(LogoStartX, LogoStartY, LogoStartX + LogoWidth, LogoStartY + LogoHeight);

	gHudDelegate->surface()->DrawSetTexture(CCTVTileTga);
	gHudDelegate->surface()->DrawTexturedRect(TileStartX, TileStartY, TileStartX + TileWidth, TileStartY + TileHeight);
	return 0;
}
void CHudCCTV::Reset(){
	VGUI_CREATE_NEWTGA_TEXTURE(CCTVDownTga, "abcenchance/tga/cctv/cctv_down_logo");
	VGUI_CREATE_NEWTGA_TEXTURE(CCTVLogoTga, "abcenchance/tga/cctv/cctv_logo");
	VGUI_CREATE_NEWTGA_TEXTURE(CCTVTileTga, "abcenchance/tga/cctv/cctv_tile");
}
