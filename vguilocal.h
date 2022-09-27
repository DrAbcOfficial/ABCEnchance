#pragma once
#include "Color.h" 
#include <vgui/IScheme.h>
#define VGUI_CREATE_NEWTGA_TEXTURE(tex, path) tex=vgui::surface()->CreateNewTextureID();vgui::surface()->DrawSetTextureFile(tex, path, true, false)

typedef struct ScreenInfo_s{
	int iWidth;
	int iHeight;
} ScreenInfo_t;
extern vgui::IScheme* pSchemeData;
extern ScreenInfo_t gScreenInfo;
extern Color gDefaultColor;

//VGUI2
#define VGUI2_ROOT_DIR "abcenchance/res/"
enum {
	SC_MAX_PLAYERS = 32,
	SC_MAX_PLAYER_NAME = 32,
	SC_MAX_TEAMS = 19,//No barnacle
	SC_MAX_TEAM_NAME = 32,
	SC_MAX_STEAMID = 32 // 0:0:4294967295, STEAM_ID_PENDING
};
enum {
	MAX_SERVERNAME_LENGTH = 64,
	MAX_PLAYERNAME_LENGTH = 32,
	MAX_TEAMNAME_SIZE = 32
};
//VGUI COLOR CODE
extern Color g_aryVGUIColorCode[];
enum {
	COLOR_PING = -2,
	COLOR_PERCENT = -1,
	COLOR_WHITE,
	COLOR_GREY,
	COLOR_BLACK,
	COLOR_RED,
	COLOR_ORANGE,
	COLOR_YELLOW,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_BLUE,
	COLOR_PURPLE
};