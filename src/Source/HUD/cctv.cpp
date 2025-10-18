#ifdef  _DEBUG
#include <metahook.h>
#include "glew.h"
#include "mymathlib.h"
#include "local.h"
#include "vguilocal.h"
#include "vgui_controls/Controls.h"
#include "CCustomHud.h"

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
	gCVars.pCCTV = CREATE_CVAR("hud_cctvlogo", "0", FCVAR_VALUE, nullptr);

	DownLogoStartX = CMathlib::GetScreenPixel(ScreenWidth(), DOWN_LOGO_STARTX);
	DownLogoStartY = CMathlib::GetScreenPixel(ScreenHeight(), DOWN_LOGO_STARTY);
	DownLogoSize = CMathlib::GetScreenPixel(ScreenHeight(), DOWN_LOGO_SIZE);

	LogoStartX = CMathlib::GetScreenPixel(ScreenWidth(), LOGO_STARTX);
	LogoStartY = CMathlib::GetScreenPixel(ScreenHeight(), LOGO_STARTY);
	LogoWidth = CMathlib::GetScreenPixel(ScreenWidth(), LOGO_WIDTH);
	LogoHeight = CMathlib::GetScreenPixel(ScreenHeight(), LOGO_HEIGHT);
	TileStartX = CMathlib::GetScreenPixel(ScreenWidth(), TILE_STARTX);
	TileStartY = CMathlib::GetScreenPixel(ScreenHeight(), TILE_STARTY);
	TileWidth = CMathlib::GetScreenPixel(ScreenWidth(), TILE_WIDHT);
	TileHeight = CMathlib::GetScreenPixel(ScreenHeight(), TILE_HEIGHT);
	Reset();
	return 0;
}
int CHudCCTV::Draw(float flTime){
	if (gCVars.pCCTV->value <= 0)
		return 1;

	vgui::surface()->DrawSetTexture(-1);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawSetTexture(CCTVDownTga);
	vgui::surface()->DrawTexturedRect(DownLogoStartX, DownLogoStartY, DownLogoStartX + DownLogoSize, DownLogoStartY + DownLogoSize);

	vgui::surface()->DrawSetTexture(CCTVLogoTga);
	vgui::surface()->DrawTexturedRect(LogoStartX, LogoStartY, LogoStartX + LogoWidth, LogoStartY + LogoHeight);

	vgui::surface()->DrawSetTexture(CCTVTileTga);
	vgui::surface()->DrawTexturedRect(TileStartX, TileStartY, TileStartX + TileWidth, TileStartY + TileHeight);
	return 0;
}
void CHudCCTV::Reset(){
	VGUI_CREATE_NEWTGA_TEXTURE(CCTVDownTga, "abcenchance/tga/cctv/cctv_down_logo");
	VGUI_CREATE_NEWTGA_TEXTURE(CCTVLogoTga, "abcenchance/tga/cctv/cctv_logo");
	VGUI_CREATE_NEWTGA_TEXTURE(CCTVTileTga, "abcenchance/tga/cctv/cctv_tile");
}
#endif