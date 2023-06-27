//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TEXTIMAGEEX_H
#define TEXTIMAGEEX_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/TextImage.h>

namespace vgui
{
	//-----------------------------------------------------------------------------
	// Purpose: Image that handles drawing of a text string
	//-----------------------------------------------------------------------------
	class TextImageEx : public TextImage
	{
	public:
		using TextImage::TextImage;

		// draws the text
		virtual void Paint() override;
		virtual void RecalculateNewLinePositions() override;
		virtual void RecalculateCenterWrapIndents() override;
	protected:
		virtual void GetTextSize(int& wide, int& tall) override;
		virtual void RecalculateEllipsesPosition() override;
	private:
		bool m_bRAlign;
	};

} // namespace vgui2

#endif // TEXTIMAGEEX_H