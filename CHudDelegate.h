#pragma once
#define MAX_SPRITE_NAME_LENGTH	24
#include "baseammo.h"
#include "basemenu.h"

typedef struct hud_playerinfo_s {
	int index;
	float frags;
	long death;
	float health;
	float armor;
	byte team;
	char donors;
	char admin;
}hud_playerinfo_t;
enum SC_DONER_ICON {
	DONER_NONE = 0,
	DONER_ELECTRIC_CROWBAR,
	DONER_GOLDED_UZI,
	DONER_GOLED_DOLLAR,
	DONER_TESTER,
	DONER_DEVELOEPR
};
enum SC_ADMIN_ICON {
	ADMIN_NONE = 0,
	ADMIN_OPRATER,
	ADMIN_SERVER_OWNER
};

class CHudDelegate{
public:
	void GL_Init(void);
	void HUD_Init(void);
	void HUD_VidInit(void);
	void HUD_Draw(float flTime);
	void HUD_Reset(void);
	void HUD_UpdateClientData(client_data_t* cdata, float time);
	void HUD_ClientMove(struct playermove_s* ppmove, qboolean server);
	void HUD_Clear(void);
	void HUD_PreRenderView(int a1);
	void HUD_PostRenderView(int a1);
	void IN_MouseEvent(int mstate);
	int HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding);
	void IN_Accumulate(void);
	void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active);

	vgui::ISurface* surface();
	~CHudDelegate();			// destructor, frees allocated memory

	int m_iPlayerHealth;
	int m_iVisibleMouse;
	int m_iIsOverView = 0;
	float m_flOverViewScale;
	float m_flOverViewYaw;
	float m_flOverViewZmax;
	float m_flOverViewZmin;
	vec3_t m_vecOverViewOrg;

	vec3_t m_vecClientEVPunch;
	vec2_t m_vecThirdPersonCrosshairPos;
	hud_playerinfo_t m_Playerinfo[33];

	client_sprite_t* m_pSpriteList;
	int m_iSpriteCountAllRes;

	HSPRITE GetSprite(int index){ return (index < 0) ? 0 : m_rghSprites[index];}
	wrect_t& GetSpriteRect(int index){ return m_rgrcRects[index]; }
	int GetSpriteIndex(const char* SpriteName);	// gets a sprite index, for use in the m_rghSprites[] array
	bool IsInSpectate();
private:
	int	 m_iSpriteCount;
	HSPRITE* m_rghSprites;	/*[HUD_SPRITE_COUNT]*/			// the sprites loaded from hud.txt
	wrect_t* m_rgrcRects;	/*[HUD_SPRITE_COUNT]*/
	char* m_rgszSpriteNames; /*[HUD_SPRITE_COUNT][MAX_SPRITE_NAME_LENGTH]*/
};
extern CHudDelegate* gHudDelegate;
typedef struct {
	CHudMenu* m_Menu;
	CHudAmmo* m_Ammo;
} cl_hookedHud;
extern cl_hookedHud gHookHud;