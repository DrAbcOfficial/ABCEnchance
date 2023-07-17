//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <stdio.h>

#include <vgui/IBorder.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui/IBorder.h>

#include <tier1/KeyValues.h>

#include "ImageClipPanel.h"
#include "Image.h"
#include "Controls.h"

// memdbgon must be the last include file in a .cpp file!!!
//#include <tier0/memdbgon.h>

using namespace vgui;

DECLARE_BUILD_FACTORY( ImageClipPanel );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ImageClipPanel::ImageClipPanel(Panel *parent, const char *name) : ImagePanel(parent, name){
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImageClipPanel::PaintBackground(){
	if (m_FillColor[3] > 0){
		// draw the specified fill color
		int wide, tall;
		GetSize(wide, tall);
		surface()->DrawSetColor(m_FillColor);
		surface()->DrawFilledRect(0, 0, wide, tall);
	}
	if (m_pImage){
		surface()->DrawSetColor( 255, 255, 255, 255 );
		m_pImage->SetPos(0, 0);

		if (m_bScaleImage){
			// Image size is stored in the bitmap, so temporarily set its size
			// to our panel size and then restore after we draw it.

			int imageWide, imageTall;
			m_pImage->GetSize( imageWide, imageTall );

			if ( m_fScaleAmount > 0.0f ){
				float wide, tall;
				wide = static_cast<float>(imageWide) * m_fScaleAmount;
				tall = static_cast<float>(imageTall) * m_fScaleAmount;
				m_pImage->SetSize( static_cast<int>(wide), static_cast<int>(tall) );
			}
			else
				m_pImage->SetSize( m_iOldWide, m_iOldTall );
			m_pImage->SetColor( GetDrawColor() );
			m_pImage->Paint();
			m_pImage->SetSize( imageWide, imageTall );
		}
		else
			m_pImage->Paint();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Applies designer settings from res file
//-----------------------------------------------------------------------------
void ImageClipPanel::ApplySettings(KeyValues *inResourceData){
	BaseClass::ApplySettings(inResourceData);
	GetSize(m_iOldWide, m_iOldTall);
}

void vgui::ImageClipPanel::SetImageSize(int wide, int tall){
	m_iOldWide = wide;
	m_iOldTall = tall;
}

Color vgui::ImageClipPanel::GetDrawColor(void){
	return m_DrawColor;
}

void vgui::ImageClipPanel::SetDrawColor(Color inColor){
	m_DrawColor = inColor;
}

int ImageClipPanel::GetOldWide(){
	return m_iOldWide;
}

int ImageClipPanel::GetOldTall(){
	return m_iOldTall;
}
