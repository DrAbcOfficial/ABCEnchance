#pragma once
#define MAX_SPRITE_NAME_LENGTH	24
#include "baseammo.h"
#include "basemenu.h"

class CHudDelegate
{
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

	client_sprite_t* m_pSpriteList;
	int m_iRes;
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