#pragma once
#ifndef VGUICUSTOMLOCAL_H
#define VGUICUSTOMLOCAL_H

#include "Color.h" 

extern void VGUI_CREATE_NEWTGA_TEXTURE(int tex, const char* path);

using ScreenInfo_t = struct ScreenInfo_s{
	int iWidth;
	int iHeight;
};

// ScreenHeight returns the height of the screen, in pixels
extern size_t ScreenHeight();
// ScreenWidth returns the width of the screen, in pixels
extern size_t ScreenWidth();
//VGUI2
#define VGUI2_ROOT_DIR "abcenchance/res/"
constexpr auto SC_MAX_STEAMID = 32; // 0:0:4294967295, STEAM_ID_PENDING
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