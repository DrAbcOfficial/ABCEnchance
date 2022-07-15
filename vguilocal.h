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
#define VGUI2_ROOT_DIR "abcenchance/"
enum {
	SC_MAX_PLAYERS = 32,
	SC_MAX_PLAYER_NAME = 32,
	SC_MAX_TEAMS = 5,//R G B Y + Other
	SC_MAX_TEAM_NAME = 16,
	SC_MAX_TEAM_DISPLAY_NAME = 32, // Extended from 16 chars in HL to 32 chars
	SC_MAX_STEAMID = 32 // 0:0:4294967295, STEAM_ID_PENDING
};
enum {
	MAX_SERVERNAME_LENGTH = 64,
	MAX_PLAYERNAME_LENGTH = 32,
	MAX_TEAMNAME_SIZE = 32
};