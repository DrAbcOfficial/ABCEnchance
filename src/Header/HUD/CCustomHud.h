#pragma once
#include <vector>
#include <array>
#include <optional>
#include "player_infosc.h"

enum SC_DONER_ICON {
	DONER_NONE = 65536,
	DONER_ELECTRIC_CROWBAR,
	DONER_GOLDED_UZI,
	DONER_GOLED_DOLLAR,
	DONER_TESTER,
	DONER_ARTIST,
	DONER_DEVELOEPR
};
enum SC_ADMIN_ICON {
	ADMIN_NONE = 0,
	ADMIN_OPRATER,
	ADMIN_SERVER_OWNER
};
using hud_playerinfo_t = struct hud_playerinfo_s {
	int index;
	float frags;
	long death;
	float health;
	float armor;
	int team;
	bool isdonor;
	SC_ADMIN_ICON admin;
	bool spectate;
};
using hud_nativeplayerinfo_t = struct hud_nativeplayerinfo_s {
	float frags;
	int death;
	int classify;
	char unknown[64];
	float health;
	float armor;
	SC_DONER_ICON donor;
	SC_ADMIN_ICON admin;
};

class CHudBattery;
class CHudHealth;
class CHudAmmo;
class CHudFlashlight;
using cl_hookedHud = struct {
	CHudBattery* m_Battery;
	CHudHealth* m_Health;
	CHudAmmo* m_Ammo;
	CHudFlashlight* m_Flash;
};

typedef int HSPRITE;

#ifndef __AMMO_H__
class WEAPON;
#endif
class CCustomHud{
public:
	void GL_Init(void);
	void HUD_Init(void);
	void HUD_VidInit(void);
	void HUD_Draw(float flTime);
	void HUD_Reset(void);
	void HUD_UpdateClientData(client_data_t* cdata, float time);
	void HUD_Clear(void);
	void IN_MouseEvent(int mstate);
	int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname);
	void HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);

	bool IsInSpectate();

	bool HasSuit();
	void WeaponBitsChangeCallBack(int bits);

	bool IsHudHide(int HideToken);
	void HudHideCallBack(int hidetoken);

	bool IsSpectator(int client);
	void SetSpectator(int client, bool value);

	bool IsMouseVisible();
	bool IsTextMenuOpening();
	bool SelectTextMenuItem(int slot);
	void SetMouseVisible(bool state);

	void SetCurWeapon(WEAPON* weapon);

	void OnMousePressed(int code);

	HSPRITE GetSprite(size_t index);
	wrect_t* GetSpriteRect(size_t index);
	std::optional<int> GetSpriteIndex(const char* SpriteName);

	hud_playerinfo_t* GetPlayerHUDInfo(int index);

	bool IsInScore();

	void RenderRadar();
	~CCustomHud();			// destructor, frees allocated memory

	int m_bitsHideHUDDisplay = 0;
	std::optional<int> m_bitsWeaponBits = 0;

	float m_flOverViewScale = 0.0f;
	float m_flOverViewYaw = 0.0f;
	float m_flOverViewZmax = 0.0f;
	float m_flOverViewZmin = 0.0f;
	vec3_t m_vecOverViewOrg;

	size_t m_flCursorSize = 0;
	int m_iCursorTga = 0;

	bool m_bInScore = false;

	enum MetaHookMsgType {
		MHSV_CMD_QUERY_PLUGIN = 1,
		MHSV_CMD_QUERY_CVAR,
		MHSV_CMD_ABC_CUSTOM = 77
	};
	enum class ABCCustomMsg {
		POPNUMBER = 0
	};

	static player_infosc_t* GetPlayerInfoEx(int index);
	static void HideOriginalHud();
private:
	std::array<hud_playerinfo_t, 32> m_aryPlayerInfos;
	std::vector<client_sprite_t*> m_arySprites; // the sprites loaded from hud.txt
};
extern CCustomHud gCustomHud;
extern cl_hookedHud gHookHud;