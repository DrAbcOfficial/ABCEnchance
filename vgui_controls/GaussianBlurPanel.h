//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef GAUSSIANBLURPANEL_H
#define GAUSSIANBLURPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include "Panel.h"

#include <IMetaRenderer.h>

namespace vgui
{
//-----------------------------------------------------------------------------
// Purpose: Panel that holds a clipable single image
//-----------------------------------------------------------------------------
class GaussianBlurPanel : public Panel
{
	DECLARE_CLASS_SIMPLE(GaussianBlurPanel, Panel);
public:
	GaussianBlurPanel(Panel *parent, const char *name);
	~GaussianBlurPanel();

	size_t GetBlurness();
	void SetBlurness(size_t f);
protected:
	virtual void PaintBackground();
	virtual void ApplySettings(KeyValues *inResourceData);
private:
	FBO_Container_t m_BlurFBO[2]{};

	size_t m_iBlurIteration = 3;
	size_t m_iBlurOffset = 5;
};

} // namespace vgui

#endif // IMAGEPANEL_H
