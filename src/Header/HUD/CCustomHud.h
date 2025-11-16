#pragma once
#include <vector>
#include <array>
#include <optional>

#include "core/resource/weaponresource.h"
#include <IMetaRenderer.h>

typedef int HSPRITE;

class CCustomHud : public IMetaRendererCallbacks
{
public:
	/*
	 Called from GL_BeginRendering
	*/
	void OnBeginRendering(int* x, int* y, int* width, int* height) override;

	/*
		Called from GL_BeginRendering -> R_RenderFrameStart
	*/
	void OnRenderFrameStart() override;

	/*
		Called from GL_BeginRendering
	*/
	void OnBeginRenderingPost(int* x, int* y, int* width, int* height) override;

	/*
		Called from GL_FreeFrameBuffers
	*/
	void OnFreeFrameBuffers() override;

	/*
		Called from GL_GenerateFrameBuffers
	*/
	void OnGenerateFrameBuffers()  override;

	/*
		Called from R_PreRenderView
	*/
	void OnPreRenderView() override;

	/*
		Called from R_PreRenderView, after switching to target FBO
	*/
	void OnPreRenderViewPost() override;

	/*
		Called from R_PostRenderView
	*/
	void OnPostRenderView() override;

	/*
		Called from R_PostRenderView, after executing all built-in post-processes
	*/
	void OnPostRenderViewPost() override;

	/*
		Called from R_RenderEndFrame
	*/
	void OnRenderEndFrame()  override;

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
	vec3_t m_vecOverViewOrigin;
	vec3_t m_vecOverViewAngles;

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
};
extern CCustomHud gCustomHud;