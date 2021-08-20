#include <metahook.h>
#include <triangleapi.h>
#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>

#include "vguilocal.h"

void DrawSPRIcon(int SprHandle, float x, float y, float w, float h, int r, int g, int b, int a)
{
	gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)gEngfuncs.GetSpritePointer(SprHandle, SprHandle), 0);
	gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(x, y, 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(x, y + h, 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(x + w, y + h, 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(x + w, y, 0);
	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}
void DrawSPRIconPos(int SprHandle, vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int r, int g, int b, int a)
{
	gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)gEngfuncs.GetSpritePointer(SprHandle, SprHandle), 0);
	gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(p1[0], p1[1], 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(p2[0], p2[1], 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(p3[0], p3[1], 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(p4[0], p4[1], 0);
	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}
int GetHudFontHeight(vgui::HFont m_hFont)
{
	if (!m_hFont)
		return 0;

	return pSurface->GetFontTall(m_hFont);
}
void GetStringSize(const wchar_t* string, int* width, int* height, vgui::HFont m_hFont)
{
	int i;
	int len;
	int a, b, c;

	if (width)
		*width = 0;

	if (height)
		*height = 0;

	if (!m_hFont)
		return;

	len = wcslen(string);

	if (width)
	{
		for (i = 0; i < len; i++)
		{
			pSurface->GetCharABCwide(m_hFont, string[i], a, b, c);
			*width += a + b + c;
		}
	}

	if (height)
	{
		*height = GetHudFontHeight(m_hFont);
	}
}
int DrawVGUI2String(wchar_t* msg, int x, int y, float r, float g, float b, vgui::HFont m_hFont)
{
	if (r > 1.0)
		r /= 255;
	if (g > 1.0)
		g /= 255;
	if (b > 1.0)
		b /= 255;
	int i;
	int iOriginalX;
	int iTotalLines;
	int iCurrentLine;
	int w1, w2, w3;
	bool bHorzCenter;
	int len;
	wchar_t* strTemp;
	int fontheight;

	if (!m_hFont)
		return 0;

	iCurrentLine = 0;
	iOriginalX = x;
	iTotalLines = 1;
	bHorzCenter = false;
	len = wcslen(msg);

	for (strTemp = msg; *strTemp; strTemp++)
	{
		if (*strTemp == '\r')
			iTotalLines++;
	}

	if (x == -1)
	{
		bHorzCenter = true;
	}

	if (y == -1)
	{
		fontheight = pSurface->GetFontTall(m_hFont);
		y = (gScreenInfo.iHeight - fontheight) / 2;
	}

	for (i = 0; i < iTotalLines; i++)
	{
		wchar_t line[1024];
		int iWidth;
		int iHeight;
		int iTempCount;
		int j;
		int shadow_x;

		iTempCount = 0;
		iWidth = 0;
		iHeight = 0;

		for (strTemp = &msg[iCurrentLine]; *strTemp; strTemp++, iCurrentLine++)
		{
			if (*strTemp == '\r')
				break;

			if (*strTemp != '\n')
				line[iTempCount++] = *strTemp;
		}

		line[iTempCount] = 0;

		GetStringSize(line, &iWidth, &iHeight, m_hFont);

		if (bHorzCenter)
			x = (gScreenInfo.iWidth - iWidth) / 2;
		else
			x = iOriginalX;

		gEngfuncs.pfnDrawSetTextColor(0, 0, 0);

		shadow_x = x;

		for (j = 0; j < iTempCount; j++)
		{
			gEngfuncs.pfnVGUI2DrawCharacter(shadow_x, y, line[j], m_hFont);
			pSurface->GetCharABCwide(m_hFont, line[j], w1, w2, w3);

			shadow_x += w1 + w2 + w3;
		}

		gEngfuncs.pfnDrawSetTextColor(r, g, b);

		for (j = 0; j < iTempCount; j++)
		{
			gEngfuncs.pfnVGUI2DrawCharacter(x, y, line[j], m_hFont);
			pSurface->GetCharABCwide(m_hFont, line[j], w1, w2, w3);

			x += w1 + w2 + w3;
		}

		y += iHeight;
		iCurrentLine++;
	}

	return x;
}