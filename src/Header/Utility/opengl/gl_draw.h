#pragma once

void GL_Bind(GLint gltexturenum);
void DrawSPRIcon(int SprHandle, int mode, float x, float y, float w, float h, unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame = 0);
void DrawSPRIconRect(int SprHandle, int mode, float x, float y, float w, float h, float left, float right, float top, float bottom, unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame);
void DrawSPRIconPos(int SprHandle, int mode, float p1[2], float p2[2], float p3[2], float p4[2], unsigned char r, unsigned char g, unsigned char b, unsigned char a);
int GetHudFontHeight(vgui::HFont m_hFont);
void GetStringSize(const wchar_t* string, int* width, int* height, vgui::HFont m_hFont);
int DrawVGUI2String(wchar_t* msg, int x, int y, int r, int g, int b, vgui::HFont m_hFont, bool add = false);
int DrawVGUI2String(wchar_t* msg, int x, int y, float r, float g, float b, vgui::HFont m_hFont, bool add = false);
void ScaleColors(int& r, int& g, int& b, int a);
void DrawQuadPos(int x, int y, int w, int h);
void DrawQuad(int w, int h);
void DrawScreenQuad();
void DrawKawaseBlur(GLint tex, size_t blurness, float offsetw, float offseth, int w, int h);