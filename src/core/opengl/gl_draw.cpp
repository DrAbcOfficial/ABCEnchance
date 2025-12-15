#include <metahook.h>
#include "utility/vgui_util.h"
#include "gl_common.h"
#include "utility/util.h"
#include "vgui_controls/Controls.h"

#include <IMetaRenderer.h>

void GL_Bind(int gltexturenum)
{
	if (MetaRenderer())
	{
		MetaRenderer()->Bind(gltexturenum);
	}
}

using mspriteframe_t = struct mspriteframe_s
{
	int		width;
	int		height;
	float		up, down, left, right;
	int		gl_texturenum;
};

void DrawSPRIcon(int SprHandle, int mode, float x, float y, float w, float h, unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame) 
{
	SPR_Set(SprHandle, r, g, b);
	extern /* (msprite_s**) */ void* gpSprite;
	void* spr = *(void**)gpSprite;
	mspriteframe_t* memsprite = static_cast<mspriteframe_t*>(gHookFuncs.R_GetSpriteFrame(spr, frame));
	if (!memsprite)
		return;
	vgui::surface()->DrawSetTexture(memsprite->gl_texturenum);
	vgui::surface()->DrawSetColor(r, g, b, a);
	if (mode == kRenderTransAdd)
		vgui::surface()->DrawTexturedRectAdd(x, y, x + w, x + h);
	else
		vgui::surface()->DrawTexturedRect(x, y, x + w, x + h);
}

extern /* (msprite_s**) */ void* gpSprite;

void DrawSPRIconRect(int SprHandle, int mode, float x, float y, float w, float h, float left, float right, float top, float bottom, unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame)
{
	SPR_Set(SprHandle, r, g, b);
	void* spr = *(void**)gpSprite;
	mspriteframe_t* memsprite = static_cast<mspriteframe_t*>(gHookFuncs.R_GetSpriteFrame(spr, frame));
	if (!memsprite)
		return;

	uint64_t programState = DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED;
	if (mode == kRenderTransAdd) {
		programState = DRAW_TEXTURED_RECT_ADDITIVE_BLEND_ENABLED;
	}

	float color4v[4]{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
	
	/*
	It was:
	glBegin(GL_QUADS);
	glTexCoord2f(left, top);
	glVertex2f(x, y);
	glTexCoord2f(left, bottom);
	glVertex2f(x, y + h);
	glTexCoord2f(right, bottom);
	glVertex2f(x + w, y + h);
	glTexCoord2f(right, top);
	glVertex2f(x + w, y);
	glEnd();
	*/
	if (MetaRenderer())
	{
		// ʹ�� DrawTexturedRect ����ȷ������������
		texturedrectvertex_t vertices[4];

		// ���Ͻ� (left, top)
		vertices[0].pos[0] = x;
		vertices[0].pos[1] = y;
		vertices[0].texcoord[0] = left;
		vertices[0].texcoord[1] = top;
		vertices[0].col[0] = color4v[0]; vertices[0].col[1] = color4v[1];
		vertices[0].col[2] = color4v[2]; vertices[0].col[3] = color4v[3];

		// ���½� (left, bottom)
		vertices[1].pos[0] = x;
		vertices[1].pos[1] = y + h;
		vertices[1].texcoord[0] = left;
		vertices[1].texcoord[1] = bottom;
		vertices[1].col[0] = color4v[0]; vertices[1].col[1] = color4v[1];
		vertices[1].col[2] = color4v[2]; vertices[1].col[3] = color4v[3];

		// ���½� (right, bottom)
		vertices[2].pos[0] = x + w;
		vertices[2].pos[1] = y + h;
		vertices[2].texcoord[0] = right;
		vertices[2].texcoord[1] = bottom;
		vertices[2].col[0] = color4v[0]; vertices[2].col[1] = color4v[1];
		vertices[2].col[2] = color4v[2]; vertices[2].col[3] = color4v[3];

		// ���Ͻ� (right, top)
		vertices[3].pos[0] = x + w;
		vertices[3].pos[1] = y;
		vertices[3].texcoord[0] = right;
		vertices[3].texcoord[1] = top;
		vertices[3].col[0] = color4v[0]; vertices[3].col[1] = color4v[1];
		vertices[3].col[2] = color4v[2]; vertices[3].col[3] = color4v[3];

		// ����: ���������� (0,1,2) �� (0,2,3)
		uint32_t indices[6] = { 0, 1, 2, 0, 2, 3 };

		MetaRenderer()->DrawTexturedRect(memsprite->gl_texturenum, vertices, 4, indices, 6, programState, "ABC::DrawSPRIconRect");
	}
}

void DrawTexturePos(int tex, int mode, float p1[2], float p2[2], float p3[2], float p4[2], unsigned char r, unsigned char g, unsigned b, unsigned a)
{
	if (MetaRenderer())
	{
		uint64_t programState = DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED;
		if (mode == kRenderTransAdd) {
			programState = DRAW_TEXTURED_RECT_ALPHA_BASED_ADDITIVE_ENABLED;
		}
		float color4v[4]{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
		texturedrectvertex_t vertices[4];
		vertices[0].pos[0] = p1[0]; vertices[0].pos[1] = p1[1];
		vertices[0].texcoord[0] = 0.0f; vertices[0].texcoord[1] = 0.0f;
		vertices[0].col[0] = color4v[0]; vertices[0].col[1] = color4v[1];
		vertices[0].col[2] = color4v[2]; vertices[0].col[3] = color4v[3];

		vertices[1].pos[0] = p2[0]; vertices[1].pos[1] = p2[1];
		vertices[1].texcoord[0] = 0.0f; vertices[1].texcoord[1] = 1.0f;
		vertices[1].col[0] = color4v[0]; vertices[1].col[1] = color4v[1];
		vertices[1].col[2] = color4v[2]; vertices[1].col[3] = color4v[3];

		vertices[2].pos[0] = p3[0]; vertices[2].pos[1] = p3[1];
		vertices[2].texcoord[0] = 1.0f; vertices[2].texcoord[1] = 1.0f;
		vertices[2].col[0] = color4v[0]; vertices[2].col[1] = color4v[1];
		vertices[2].col[2] = color4v[2]; vertices[2].col[3] = color4v[3];

		vertices[3].pos[0] = p4[0]; vertices[3].pos[1] = p4[1];
		vertices[3].texcoord[0] = 1.0f; vertices[3].texcoord[1] = 0.0f;
		vertices[3].col[0] = color4v[0]; vertices[3].col[1] = color4v[1];
		vertices[3].col[2] = color4v[2]; vertices[3].col[3] = color4v[3];
		uint32_t indices[6] = { 0, 1, 2, 0, 2, 3 };
		MetaRenderer()->DrawTexturedRect(tex, vertices, 4, indices, 6, programState, "ABC::DrawTexturePos");
	}
}

void DrawSPRIconPos(int SprHandle, int mode, float p1[2], float p2[2], float p3[2], float p4[2], unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	SPR_Set(SprHandle, r, g, b);
	void* spr = *(void**)gpSprite;
	mspriteframe_t* memsprite = static_cast<mspriteframe_t*>(gHookFuncs.R_GetSpriteFrame(spr, 0));
	if (!memsprite)
		return;
	DrawTexturePos(memsprite->gl_texturenum, mode, p1, p2, p3, p4, r, g, b, a);
}

void GetStringSize(const wchar_t* string, int* width, int* height, vgui::HFont m_hFont) {
	if (width)
		*width = 0;
	if (height)
		*height = 0;
	if (!m_hFont)
		return;
	size_t len = wcslen(string);
	if (width) {
		int a, b, c;
		for (size_t i = 0; i < len; i++) {
			vgui::surface()->GetCharABCwide(m_hFont, string[i], a, b, c);
			*width += a + b + c;
		}
	}

	if (height)
		*height = m_hFont ? vgui::surface()->GetFontTall(m_hFont) : 0;
}
void ScaleColors(int& r, int& g, int& b, int a) {
	float x = (float)a / 255;
	r = (int)(r * x);
	g = (int)(g * x);
	b = (int)(b * x);
}