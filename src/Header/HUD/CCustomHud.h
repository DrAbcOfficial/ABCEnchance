#pragma once
#include <optional>
#include <IMetaRenderer.h>

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

	void HUD_Init(void);
	void HUD_Reset(void);
	void HUD_UpdateClientData(client_data_t* cdata, float time);
	void IN_MouseEvent(int mstate);
	void HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);

	bool HasSuit();

	void OnMousePressed(int code);

	std::optional<int> m_bitsWeaponBits = 0;
};
extern CCustomHud gCustomHud;