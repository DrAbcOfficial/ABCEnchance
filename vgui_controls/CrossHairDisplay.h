//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CROSSHAIRDISPLAY_H
#define CROSSHAIRDISPLAY_H

#ifdef _WIN32
#pragma once
#endif

#include <array>
#include "Panel.h"

namespace vgui
{

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CrossHairDisplay : public Panel
{
	DECLARE_CLASS_SIMPLE(CrossHairDisplay, Panel );
public:
	CrossHairDisplay(Panel *parent, const char *panelName);

	void SetDot(bool state);
	void SetT(bool T);
	void SetOutline(bool o);
	void SetWidth(int w);
	void SetLength(int l);
	void SetOutlineWidth(int o);
	void SetOffset(int o);
	void SetCrossHairColor(Color c);
	void SetOutlineColor(Color c);
protected:
	virtual void PerformLayout();
private:
	bool m_bHasDot;
	bool m_bT;
	bool m_bOutline;
	int m_iWidth;
	int m_iLength;
	int m_iOutlineWidth;
	int m_iOffset;

	Color m_cCrosshair;
	Color m_cOutline;

	std::array<vgui::Panel*, 5> m_aryCrosshair;
	std::array<vgui::Panel*, 5> m_aryCrosshairBorder;
};

} // namespace vgui

#endif // MENUBAR_H

