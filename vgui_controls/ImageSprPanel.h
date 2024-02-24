//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef IMAGESPRPANEL_H
#define IMAGESPRPANEL_H

#ifdef _WIN32
#pragma once
#endif
#include <vgui/vgui.h>
#include <vgui_controls/Panel.h>

namespace vgui
{

	class CSPRImage;

	//-----------------------------------------------------------------------------
	// Purpose: Panel that holds a single image
	//-----------------------------------------------------------------------------
	class ImageSprPanel : public Panel
	{
		DECLARE_CLASS_SIMPLE(ImageSprPanel, Panel);
	public:
		ImageSprPanel(Panel* parent, const char* name);
		virtual ~ImageSprPanel();

		virtual void SetImage(CSPRImage* image);
		virtual void SetImage(const char* imageName);
		virtual void SetImage(int sprindex);
		virtual CSPRImage* GetImage();
		char* GetImageName();

		void SetShouldCenterImage(bool state) { m_bCenterImage = state; }
		bool GetShouldCenterImage() const { return m_bCenterImage; }

		// sets whether or not the image should scale to fit the size of the ImagePanel (defaults to false)
		void SetShouldScaleImage(bool state);
		bool GetShouldScaleImage();
		void SetScaleAmount(float scale);
		float GetScaleAmount(void);

		void SetTileImage(bool bTile) { m_bTileImage = bTile; }

		// set the color to fill with, if no image is specified
		void SetFillColor(Color col);
		Color GetFillColor();

		virtual Color GetDrawColor(void);
		virtual void SetDrawColor(Color drawColor);

		virtual void ApplySettings(KeyValues* inResourceData);

		// unhooks and evicts image if possible, caller must re-establish
		bool EvictImage();

		void SetFramerate(float fl);
		void SetRect(int l, int r, int t, int b);
		void SetRenderMode(int mode);
		void Reset();
		void Animate();
		void SetFrame(int frame);

	protected:
		virtual void PaintBackground();
		virtual void GetSettings(KeyValues* outResourceData);
		virtual const char* GetDescription();
		virtual void OnSizeChanged(int newWide, int newTall);
		virtual void ApplySchemeSettings(IScheme* pScheme);

		CSPRImage* m_pImage = nullptr;
		char* m_pszImageName;
		char* m_pszFillColorName;
		char* m_pszDrawColorName;
		bool m_bPositionImage;
		bool m_bCenterImage;
		bool m_bScaleImage;
		bool m_bTileImage;
		bool m_bTileHorizontally;
		bool m_bTileVertically;
		float m_fScaleAmount;
		Color m_FillColor;
		Color m_DrawColor;
	};

} // namespace vgui2

#endif // IMAGEPANEL_H