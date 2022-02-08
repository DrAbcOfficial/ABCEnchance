#pragma once
#define MAX_SPRITE_NAME_LENGTH	24
#include <vector>
#include "baseammo.h"
#include "basemenu.h"
#include "basehealth.h"

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
typedef struct {
	CHudBattery* m_Battery;
	CHudHealth* m_Health;
	CHudMenu* m_Menu;
	CHudAmmo* m_Ammo;
} cl_hookedHud;
typedef struct cl_spritem_s {
	HSPRITE spr;
	wrect_t rect;
	char name[MAX_SPRITE_NAME_LENGTH];
}cl_spritem_t;
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

class CCustomHud{
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
	void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active);
	void IN_Accumulate(void);
	int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname);
	bool IsInSpectate();
	bool HasSuit();
	bool IsHudHide(int HideToken);
	bool IsHudEnable();

	HSPRITE GetSprite(int index);
	wrect_t* GetSpriteRect(int index);
	int GetSpriteIndex(const char* SpriteName);

	hud_playerinfo_t* GetPlayerHUDInfo(int index);

	vgui::ISurface* surface();
	~CCustomHud();			// destructor, frees allocated memory

	int m_iPlayerHealth = 0;
	bool m_bPlayerLongjump = false;
	int m_iVisibleMouse = 0;
	int m_iIsOverView = 0;
	int m_iHideHUDDisplay = 0;
	int m_iWeaponBits = 0;
	float m_flOverViewScale = 0;
	float m_flOverViewYaw = 0;
	float m_flOverViewZmax = 0;
	float m_flOverViewZmin = 0;
	float m_flCursorSize = 0;

	vec3_t m_vecOverViewOrg;
	vec3_t m_vecClientEVPunch;
	vec2_t m_vecThirdPersonCrosshairPos;
	
	client_sprite_t* m_pSpriteList;

	GLint m_iCursorTga = 0;
private:
	void SetBaseHudActivity();
	hud_playerinfo_t m_Playerinfo[33] = { 0 };
	int m_iSpriteCountAllRes;
	std::vector<cl_spritem_s*> m_arySprites; // the sprites loaded from hud.txt
};
extern CCustomHud gCustomHud;
extern cl_hookedHud gHookHud;