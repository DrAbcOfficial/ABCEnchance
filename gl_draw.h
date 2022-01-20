#pragma once
#define glBind(tx) gHookFuncs.GL_Bind(tx)

void DrawSPRIcon(int SprHandle, int mode, float x, float y, float w, float h, int r, int g, int b, int a, int frame = 0);
void DrawSPRIconPos(int SprHandle, int mode, vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int r, int g, int b, int a);
int GetHudFontHeight(vgui::HFont m_hFont);
void GetStringSize(const wchar_t* string, int* width, int* height, vgui::HFont m_hFont);
int DrawVGUI2String(wchar_t* msg, int x, int y, float r, float g, float b, vgui::HFont m_hFont, bool add = false);
void ScaleColors(int& r, int& g, int& b, int a);
void DrawQuad(int w, int h);
void DrawScreenQuad();
void DrawGaussianBlur(GLint dstfbo, GLint srctex, GLint dsttex, float ratio, bool v, int w, int h);