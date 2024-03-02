#pragma once
#ifndef VGUICUSTOMLOCAL_H
#define VGUICUSTOMLOCAL_H

#include "Color.h" 
#include <vgui/IScheme.h>
void VGUI_CREATE_NEWTGA_TEXTURE(int tex, const char* path);
size_t GET_SCREEN_PIXEL(bool h, const char* str);

typedef struct ScreenInfo_s{
	int iWidth;
	int iHeight;
} ScreenInfo_t;
extern vgui::IScheme* pSchemeData;
extern ScreenInfo_t gScreenInfo;
extern Color gDefaultColor;

// ScreenHeight returns the height of the screen, in pixels
size_t ScreenHeight();
// ScreenWidth returns the width of the screen, in pixels
size_t ScreenWidth();
//VGUI2
#define VGUI2_ROOT_DIR "abcenchance/res/"
enum {
	SC_MAX_PLAYERS = 32,
	SC_MAX_TEAMS = 19,//No barnacle
	SC_MAX_STEAMID = 32 // 0:0:4294967295, STEAM_ID_PENDING
};
enum {
	MAX_SERVERNAME_LENGTH = 64,
	MAX_PLAYERNAME_LENGTH = 32,
	MAX_TEAMNAME_LENGTH = 32
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
#endif // !VGUICUSTOMLOCAL_H