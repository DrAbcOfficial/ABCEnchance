#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>
#include "tga_image.h"

CTGAImage::CTGAImage()
{
	_bAdditive = false;
	m_iTextureID = vgui::surface()->CreateNewTextureID();
}

CTGAImage::CTGAImage(const char *pFilePath)
    : CTGAImage()
{
	LoadImage(pFilePath);
}

void CTGAImage::LoadImage(const char *pFilePath)
{
	vgui::surface()->DrawSetTextureFile(m_iTextureID, pFilePath, true, false);
}

void CTGAImage::Paint()
{
	if (m_iTextureID != -1)
	{
		vgui::surface()->DrawSetTexture(m_iTextureID);
		vgui::surface()->DrawSetColor(m_Color);
		if (_bAdditive)
			vgui::surface()->DrawTexturedRectAdd(m_nX, m_nY, m_nX + m_wide, m_nY + m_tall);
		else
			vgui::surface()->DrawTexturedRect(m_nX, m_nY, m_nX + m_wide, m_nY + m_tall);
	}
}

void CTGAImage::SetPos(int x, int y)
{
	m_nX = x;
	m_nY = y;
}

void CTGAImage::GetContentSize(int &wide, int &tall)
{
	wide = m_wide;
	tall = m_tall;
}

void CTGAImage::GetSize(int &wide, int &tall)
{
	GetContentSize(wide, tall);
}

void CTGAImage::SetSize(int wide, int tall)
{
	m_wide = wide;
	m_tall = tall;
}

void CTGAImage::SetColor(Color col)
{
	m_Color = col;
}

void CTGAImage::Destroy(void)
{
	delete this;
}

void CTGAImage::SetAdditive(bool bIsAdditive)
{
	_bAdditive = bIsAdditive;
}

int CTGAImage::GetTextureId()
{
	return m_iTextureID;
}
