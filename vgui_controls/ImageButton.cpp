#include <vgui_controls/ImageButton.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

ImageButton::ImageButton(Panel* parent, const char* panelname, const char* image, Panel* pActionSignalTarget, const char* pCmd) :
	BaseClass(parent, panelname, "", pActionSignalTarget, pCmd){
	m_pImage = NULL;
	m_bCenterImage = false;
	m_bScaleImage = false;
	m_bTileImage = false;
	m_bTileHorizontally = false;
	m_bTileVertically = false;
	m_bPositionImage = true;
	m_fScaleAmount = 0.0f;
	m_FillColor = Color(0, 0, 0, 0);
	m_DrawColor = Color(255, 255, 255, 255);

	SetImage(scheme()->GetImage(image, true));

	REGISTER_COLOR_AS_OVERRIDABLE(m_FillColor, "fillcolor_override");
	REGISTER_COLOR_AS_OVERRIDABLE(m_DrawColor, "drawcolor_override");
}

void vgui::ImageButton::SetImage(IImage* image){
	m_pImage = image;
	Repaint();
}

void vgui::ImageButton::SetImage(const char* imageName){
	SetImage(scheme()->GetImage(imageName, true));
	InvalidateLayout(false, true); // force applyschemesettings to run
}

IImage* vgui::ImageButton::GetImage(){
	return m_pImage;
}

void vgui::ImageButton::SetShouldScaleImage(bool state){
	m_bScaleImage = state;
}

bool vgui::ImageButton::GetShouldScaleImage(){
	return m_bScaleImage;
}

void vgui::ImageButton::SetScaleAmount(float scale){
	m_fScaleAmount = scale;
}

float vgui::ImageButton::GetScaleAmount(void){
	return m_fScaleAmount;
}

void vgui::ImageButton::SetFillColor(Color col){
	m_FillColor = col;
}

Color vgui::ImageButton::GetFillColor(){
	return m_FillColor;
}

Color vgui::ImageButton::GetDrawColor(){
	return m_DrawColor;
}

void vgui::ImageButton::SetDrawColor(Color drawColor){
	m_DrawColor = drawColor;
}

void ImageButton::Paint(){
	if (!ShouldPaint())
		return;

	if (m_FillColor[3] > 0){
		int wide, tall;
		GetSize(wide, tall);
		surface()->DrawSetColor(m_FillColor);
		surface()->DrawFilledRect(0, 0, wide, tall);
	}
	if (m_pImage){
		m_pImage->SetColor(GetDrawColor());
		if (m_bPositionImage){
			if (m_bCenterImage){
				int wide, tall;
				GetSize(wide, tall);

				int imageWide, imageTall;
				m_pImage->GetSize(imageWide, imageTall);

				if (m_bScaleImage && m_fScaleAmount > 0.0f){
					imageWide = static_cast<int>(static_cast<float>(imageWide) * m_fScaleAmount);
					imageTall = static_cast<int>(static_cast<float>(imageTall) * m_fScaleAmount);
				}

				m_pImage->SetPos((wide - imageWide) / 2, (tall - imageTall) / 2);
			}
			else{
				m_pImage->SetPos(0, 0);
			}
		}

		if (m_bScaleImage){
			int imageWide, imageTall;
			m_pImage->GetSize(imageWide, imageTall);

			if (m_fScaleAmount > 0.0f){
				float wide, tall;
				wide = static_cast<float>(imageWide) * m_fScaleAmount;
				tall = static_cast<float>(imageTall) * m_fScaleAmount;
				m_pImage->SetSize(static_cast<int>(wide), static_cast<int>(tall));
			}
			else{
				int wide, tall;
				GetSize(wide, tall);
				m_pImage->SetSize(wide, tall);
			}
			m_pImage->Paint();
			m_pImage->SetSize(imageWide, imageTall);
		}
		else if (m_bTileImage || m_bTileHorizontally || m_bTileVertically){
			int wide, tall;
			GetSize(wide, tall);
			int imageWide, imageTall;
			m_pImage->GetSize(imageWide, imageTall);
			int y = 0;
			while (y < tall){
				int x = 0;
				while (x < wide){
					m_pImage->SetPos(x, y);
					m_pImage->Paint();

					x += imageWide;

					if (!m_bTileHorizontally)
						break;
				}
				y += imageTall;
				if (!m_bTileVertically)
					break;
			}
			if (m_bPositionImage){
				m_pImage->SetPos(0, 0);
			}
		}
		else{
			m_pImage->SetColor(GetDrawColor());
			m_pImage->Paint();
		}
	}

	if (HasFocus() && IsEnabled() && IsDrawingFocusBox()){
		int x0, y0, x1, y1;
		int wide, tall;
		GetSize(wide, tall);
		x0 = 3, y0 = 3, x1 = wide - 4, y1 = tall - 2;
		DrawFocusBorder(x0, y0, x1, y1);
	}
}

void ImageButton::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
	m_bCenterImage = inResourceData->GetBool("centerImage", true);
	m_bPositionImage = inResourceData->GetBool("positionImage", false);
	m_bScaleImage = inResourceData->GetBool("scaleImage", true);
	m_fScaleAmount = inResourceData->GetFloat("scaleAmount", 0.0f);
	m_bTileImage = inResourceData->GetBool("tileImage", false);
	m_bTileHorizontally = inResourceData->GetBool("tileHorizontally", m_bTileImage);
	m_bTileVertically = inResourceData->GetBool("tileVertically", m_bTileImage);

	const char* pszFillColor = inResourceData->GetString("fillcolor", "");
	if (*pszFillColor)
	{
		int r = 0, g = 0, b = 0, a = 255;
		if (sscanf(pszFillColor, "%d %d %d %d", &r, &g, &b, &a) >= 3)
		{
			// it's a direct color
			m_FillColor = Color(r, g, b, a);
		}
		else
		{
			IScheme* pScheme = scheme()->GetIScheme(GetScheme());
			m_FillColor = pScheme->GetColor(pszFillColor, Color(0, 0, 0, 0));
		}
	}
	const char* pszDrawColor = inResourceData->GetString("drawcolor", "");
	if (*pszDrawColor)
	{
		int r = 255, g = 255, b = 255, a = 255;
		if (sscanf(pszDrawColor, "%d %d %d %d", &r, &g, &b, &a) >= 3)
		{
			// it's a direct color
			m_DrawColor = Color(r, g, b, a);
		}
		else
		{
			IScheme* pScheme = scheme()->GetIScheme(GetScheme());
			m_DrawColor = pScheme->GetColor(pszDrawColor, Color(255, 255, 255, 255));
		}
	}
}
