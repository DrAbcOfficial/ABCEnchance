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
	void Render();
	void SetModelPos(float x, float y, float z);
	void SetModelRotate(float pitch, float yaw, float roll);

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
protected:
	virtual void Paint() override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
private:
	float m_aryOrigin[3] = {0,0,0};
	float m_aryRotate[3] = {0,0,0};
	float m_flFov;
	bool m_bAnimate;
	float m_flFrameRate;
	int m_iFrame;
	int m_iSequence;
	int m_iSkin;
	int m_iGroup;
	int m_iBody;
	int m_iMouth;
	char m_szModel[MAX_PATH];
	typedef struct blendinfo_s{
		int m_iIdx;
		float m_flValue;
	}blendinto_t;
	blendinto_t m_aryBlend[2];
	blendinto_t m_aryController[4];

	float m_flPrevAnimeTime;

	StudioModel* m_Renderer;

	uint m_hBufferTex = 0;
	uint m_hBufferFBO = 0;
	uint m_iFboWidth = 0;
	uint m_iFboHeight = 0;
	int m_oldFrameBuffer = 0;
};

} // namespace vgui

#endif // MODELVIEWPANEL_H
