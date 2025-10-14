#include <metahook.h>
#include <triangleapi.h>
#include "studio.h"

#include "mymathlib.h"
#include "glew.h"
#include "vguilocal.h"
#include "gl_utility.h"
#include "gl_shader.h"
#include "gl_def.h"
#include "hud.h"
#include "local.h"
#include "exportfuncs.h"
#include "vgui_controls/Controls.h"

#include "gl_draw.h"

void glBind(GLint tex){
	gHookFuncs.GL_Bind(tex);
}

using mspriteframe_t = struct mspriteframe_s
{
	int		width;
	int		height;
	float		up, down, left, right;
	int		gl_texturenum;
};

void DrawSPRIcon(int SprHandle, int mode, float x, float y, float w, float h, 
	unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame) {
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
void DrawSPRIconRect(int SprHandle, int mode, float x, float y, float w, float h, float left, float right, float top, float bottom, 
	unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame) {
	SPR_Set(SprHandle, r, g, b);
	extern /* (msprite_s**) */ void* gpSprite;
	void* spr = *(void**)gpSprite;
	mspriteframe_t* memsprite = static_cast<mspriteframe_t*>(gHookFuncs.R_GetSpriteFrame(spr, frame));
	if (!memsprite)
		return;
	if (mode == kRenderTransAdd) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
	}
	glDepthMask(0);
	glBind(memsprite->gl_texturenum);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 0x46040000); //0x46040000 ?
	glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
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
	glDepthMask(1);
	if (mode == kRenderTransAdd) {
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}
void DrawSPRIconPos(int SprHandle, int mode, float p1[2], float p2[2], float p3[2], float p4[2], 
	unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	SPR_Set(SprHandle, r, g, b);
	extern /* (msprite_s**) */ void* gpSprite;
	void* spr = *(void**)gpSprite;
	mspriteframe_t* memsprite = static_cast<mspriteframe_t*>(gHookFuncs.R_GetSpriteFrame(spr, 0));
	if (!memsprite)
		return;
	if (mode == kRenderTransAdd) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
	}
	glDepthMask(0);
	glBind(memsprite->gl_texturenum);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 0x46040000); //0x46040000 ?
	glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(p1[0], p1[1]);
	glTexCoord2f(0, 1);
	glVertex2f(p2[0], p2[1]);
	glTexCoord2f(1, 1);
	glVertex2f(p3[0], p3[1]);
	glTexCoord2f(1, 0);
	glVertex2f(p4[0], p4[1]);
	glEnd();
	glDepthMask(1);
	if (mode == kRenderTransAdd) {
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}
int GetHudFontHeight(vgui::HFont m_hFont) {
	if (!m_hFont)
		return 0;

	return vgui::surface()->GetFontTall(m_hFont);
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
		*height = GetHudFontHeight(m_hFont);
}
int DrawVGUI2String(wchar_t* msg, int x, int y, int r, int g, int b, vgui::HFont m_hFont, bool add) {
	return DrawVGUI2String(msg, x, y, (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, m_hFont, add);
}

int DrawVGUI2String(wchar_t* msg, int x, int y, float r, float g, float b, vgui::HFont m_hFont, bool add) {
	
	int iOriginalX;
	int iTotalLines;
	int iCurrentLine;
	int w1, w2, w3;
	bool bHorzCenter;
	wchar_t* strTemp;

	if (!m_hFont)
		return 0;

	iCurrentLine = 0;
	iOriginalX = x;
	iTotalLines = 1;
	bHorzCenter = false;

	for (strTemp = msg; *strTemp; strTemp++) {
		if (*strTemp == '\r')
			iTotalLines++;
	}

	if (x == -1)
		bHorzCenter = true;

	if (y == -1)
		y = (gScreenInfo.iHeight - vgui::surface()->GetFontTall(m_hFont)) / 2;

	for (int i = 0; i < iTotalLines; i++) {
		wchar_t line[1024];
		int iWidth;
		int iHeight;
		int iTempCount;
		int j;

		iTempCount = 0;
		iWidth = 0;
		iHeight = 0;

		for (strTemp = &msg[iCurrentLine]; *strTemp; strTemp++, iCurrentLine++) {
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

		if (!add) {
			gEngfuncs.pfnDrawSetTextColor(0, 0, 0);
			int shadow_x = x;
			for (j = 0; j < iTempCount; j++) {
				gEngfuncs.pfnVGUI2DrawCharacter(shadow_x, y, line[j], m_hFont);
				vgui::surface()->GetCharABCwide(m_hFont, line[j], w1, w2, w3);
				shadow_x += w1 + w2 + w3;
			}
		}

		gEngfuncs.pfnDrawSetTextColor(r, g, b);

		for (j = 0; j < iTempCount; j++) {
			if (add)
				gEngfuncs.pfnVGUI2DrawCharacterAdd(x, y, line[j], (int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f), m_hFont);
			else
				gEngfuncs.pfnVGUI2DrawCharacter(x, y, line[j], m_hFont);
			vgui::surface()->GetCharABCwide(m_hFont, line[j], w1, w2, w3);

			x += w1 + w2 + w3;
		}

		y += iHeight;
		iCurrentLine++;
	}

	return x;
}
void ScaleColors(int& r, int& g, int& b, int a) {
	float x = (float)a / 255;
	r = (int)(r * x);
	g = (int)(g * x);
	b = (int)(b * x);
}
void DrawQuadPos(int x, int y, int w, int h) {
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex3i(x, y + h, -1);
	glTexCoord2i(0, 1);
	glVertex3i(x, y, -1);
	glTexCoord2i(1, 1);
	glVertex3i(x + w, y, -1);
	glTexCoord2i(1, 0);
	glVertex3i(x + w, y + h, -1);
	glEnd();
}
void DrawQuad(int w, int h) {
	DrawQuadPos(0, 0, w, h);
}
void DrawScreenQuad() {
	DrawQuad(ScreenWidth(), ScreenHeight());
}
void DrawKawaseBlur(GLint tex, size_t blurness, float offsetw, float offseth, int w,int h) {
	static auto rendershader = [](pp_kawaseblur_program_t shader, GLint tex, float offsetw, float offseth, int w, int h) {
		glBind(tex);
		GL_UseProgram(shader.program);
		GL_Uniform2f(shader.offset, offsetw, offseth);
		GL_Uniform2f(shader.iResolution, w, h);
		GL_Uniform2f(shader.halfpixel, 0.5f / (float)w, 0.5f / (float)h);
		glColor4ub(255, 255, 255, 255);
		DrawQuad(w, h);
		GL_UseProgram(0);
	};
	glEnable(GL_TEXTURE_2D);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, w, h);
	for (size_t i = 0; i < blurness; i++) {
		rendershader(pp_kawaseblur_down, tex, offsetw, offseth, w, h);
		rendershader(pp_kawaseblur_up, tex, offsetw, offseth, w, h);
	}
	glPopAttrib();
	glDisable(GL_TEXTURE_2D);
}