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
	void SetModelPos(int x, int y, int z);
	void SetModelRotate(int pitch, int yaw, int roll);
protected:
	virtual void Paint() override;
	virtual void ApplySchemeSettings(IScheme *pScheme) override;
private:
	StudioModel* m_Renderer;
	float m_aryOrigin[3] = {0,0,0};
	float m_aryRotate[3] = {0,0,0};
	float m_flFov;
	bool m_bAnimate;
	int m_iFrame;
	int m_iSequence;
	int m_iSkin;
	int m_iBodyGroup;

	uint m_hBufferTex;
	uint m_hBufferFBO;
	int m_oldFrameBuffer;
};

} // namespace vgui

#endif // MODELVIEWPANEL_H
