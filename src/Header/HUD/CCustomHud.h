#pragma once
#include <vector>
#include <array>
#include <optional>

#include "core/resource/weaponresource.h"

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

	bool IsMouseVisible();
	bool IsTextMenuOpening();
	bool SelectTextMenuItem(int slot);
	void SetMouseVisible(bool state);

	void SetCurWeapon(Weapon* weapon);

	void OnMousePressed(int code);

	bool IsInScore();

	void RenderRadar();

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
	static void HideOriginalHud();
};
extern CCustomHud gCustomHud;
extern cl_hookedHud gHookHud;