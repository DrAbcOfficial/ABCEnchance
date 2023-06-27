//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef IMAGECLIPPANEL_H
#define IMAGECLIPPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include "Panel.h"
#include "ImagePanel.h"

namespace vgui
{
//-----------------------------------------------------------------------------
// Purpose: Panel that holds a clipable single image
//-----------------------------------------------------------------------------
class ImageClipPanel : public ImagePanel
{
	DECLARE_CLASS_SIMPLE(ImageClipPanel, ImagePanel);
public:
	ImageClipPanel(Panel *parent, const char *name);
protected:
	virtual void PaintBackground();
	virtual void ApplySettings(KeyValues *inResourceData);

	void SetImageSize(int wide, int tall);

private:
	int m_iOldWide;
	int m_iOldTall;
};

} // namespace vgui

#endif // IMAGEPANEL_H
