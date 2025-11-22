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
#include "core/opengl/gl_common.h"

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

	void LoadModel(const char* model);
	void ChangeModel(const char* model);

	void SetModelPos(float x, float y, float z);
	void SetModelRotate(float pitch, float yaw, float roll);

	void GetModelPos(float& x, float& y, float& z);
	void GetModelRotate(float& pitch, float& yaw, float& roll);

	float GetFOV() const;
	void SetFOV(float fov);

	bool GetAnimate() const;
	void SetAnimate(bool state);

	int GetFrame();
	void SetFrame(int frame);

	float GetFrameRate();
	void SetFrameRate(float rate);

	int GetSequence();
	void SetSequnce(int seq);

	int GetSkin();
	void SetSkin(int skin);

	int GetBodygroup();
	void SetBodygroup(int bodygroup);

	float GetBlend(size_t blend);
	void SetBlend(size_t blend, float value);

	float GetController(size_t idx);
	void SetController(size_t idx, float value);

	byte GetMouth();
	void SetMouth(byte mouth);

	void SetAmbientLight(int light);
	void SetShadeLight(int light);
	void SetLightColor(int r, int g, int b);
	void SetLightOrigin(float x, float y, float z);
protected:
	virtual void Paint() override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
private:
	FBO_Container_t m_ModelFBO{ };
	float m_flFov = 90;
	bool m_bAnimate = false;
	char m_szModel[MAX_PATH] = {};

	struct cl_entity_s* m_pModelEntity = nullptr;
};

} // namespace vgui

#endif // MODELVIEWPANEL_H
