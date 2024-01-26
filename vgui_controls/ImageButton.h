#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#ifdef _WIN32
#pragma once
#endif
#include <vgui_controls/Button.h>

namespace vgui
{

	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	class ImageButton : public Button
	{
		DECLARE_CLASS_SIMPLE(ImageButton, Button);

	public:
		ImageButton(Panel* parent, const char* panelname, const char* image, Panel* pActionSignalTarget = NULL, const char* pCmd = NULL);

		virtual void SetImage(IImage* image);
		virtual void SetImage(const char* imageName);
		virtual IImage* GetImage();

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
	protected:
		virtual void Paint() override;
		virtual void ApplySettings(KeyValues* inResourceData);
	private:
		IImage* m_pImage;

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

#endif // BUTTON_H