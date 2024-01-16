//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef MODELVIEWPANEL_H
#define MODELVIEWPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "Panel.h"

class StudioModel;
namespace vgui
{
//-----------------------------------------------------------------------------
// Purpose: Holds and displays a chart
//-----------------------------------------------------------------------------
class ModelViewPanel : public Panel
{
	DECLARE_CLASS_SIMPLE(ModelViewPanel, Panel );
public:
	ModelViewPanel(Panel *parent, const char *name);
	~ModelViewPanel();

	void SetupTexBuffer();
	void LoadModel(const char* model);
	void ChangeModel(const char* model);
	void Render();
	void SetModelPos(float x, float y, float z);
	void SetModelRotate(float pitch, float yaw, float roll);

	void GetModelPos(float& x, float& y, float& z);
	void GetModelRotate(float& pitch, float& yaw, float& roll);

	float GetFOV();
	void SetFOV(float fov);

	bool GetAnimate();
	void SetAnimate(bool state);

	int GetFrame();
	void SetFrame(int frame);

	float GetFrameRate();
	void SetFrameRate(float rate);

	int GetSequence();
	void SetSequnce(int seq);

	int GetSkin();
	void SetSkin(int skin);

	int GetGroup();
	int GetBody();
	void SetBodygroup(int group, int body);

	float GetBlend(int blend);
	void SetBlend(int blend, float value);

	float GetController(int idx);
	void SetController(int idx, float value);

	int GetMouth();
	void SetMouth(int mouth);

	void SetAmbientLight(int light);
	void SetShadeLight(int light);
	void SetLightColor(int r, int g, int b);
	void SetLightOrigin(float x, float y, float z);
protected:
	virtual void Paint() override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
private:
	float m_aryOrigin[3] = {0,0,0};
	float m_aryRotate[3] = {0,0,0};
	float m_flFov = 90;
	bool m_bAnimate = false;
	float m_flFrameRate = 1;
	int m_iFrame = 0;
	int m_iSequence = 0;
	int m_iSkin = 0;
	int m_iGroup = 0;
	int m_iBody = 0;
	int m_iMouth = 0;
	char m_szModel[MAX_PATH] = {};
	typedef struct blendinfo_s{
		int m_iIdx = 0;
		float m_flValue = 0;
	}blendinto_t;
	blendinto_t m_aryBlend[2] = {};
	blendinto_t m_aryController[4] = {};

	float m_flPrevAnimeTime = 0;

	StudioModel* m_Renderer = nullptr;

	uint m_hBufferTex = 0;
	uint m_hBufferFBO = 0;
	uint m_hBufferRBO = 0;
	uint m_iFboWidth = 0;
	uint m_iFboHeight = 0;
	int m_oldFrameBuffer = 0;
};

} // namespace vgui

#endif // MODELVIEWPANEL_H
