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

	void LoadModel(const char* model);
	void Render();
	void Pan(int x, int y);
	void Zoom(int x, int y);
protected:
	virtual void Paint() override;
	virtual void ApplySchemeSettings(IScheme *pScheme) override;
private:
	int m_iOriginalxpos = -1;
	int m_iOriginalypos = -1;

	StudioModel* m_Renderer;
	float m_aryOrigin[3];
	float m_aryRotate[3];
	float m_flFov;

	uint m_hBufferTex;
	uint m_hBufferFBO;
	int m_oldFrameBuffer;
};

} // namespace vgui

#endif // MODELVIEWPANEL_H
