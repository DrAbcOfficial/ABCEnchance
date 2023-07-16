//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef LABELEX_H
#define LABELEX_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/Label.h"

namespace vgui
{
	class TextImageEx;
	//-----------------------------------------------------------------------------
	// Purpose: Contains and displays a set of images
	//			By default starts with one TextImage
	//-----------------------------------------------------------------------------
	class LabelEx : public Label{
		DECLARE_CLASS_SIMPLE(LabelEx, Label);
	public:
		// Constructors
		LabelEx(Panel* parent, const char* panelName, const char* text);
		LabelEx(Panel* parent, const char* panelName, const wchar_t* wszText);
	};

} // namespace vgui2

#endif // LABEL_H