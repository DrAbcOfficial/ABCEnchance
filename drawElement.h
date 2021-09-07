#pragma once
char* UnicodeToUtf8(const wchar_t* unicode);
void DrawSPRIcon(int SprHandle, float x, float y, float w, float h, int r, int g, int b, int a);
void DrawSPRIconPos(int SprHandle, vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int r, int g, int b, int a);
void DrawSPRIconClip(int SprHandle, float x, float y, float w, float h, wrect_t* prc, float width, float height, int r, int g, int b, int a);
int GetHudFontHeight(vgui::HFont m_hFont);
void GetStringSize(const wchar_t* string, int* width, int* height, vgui::HFont m_hFont);
int DrawVGUI2String(wchar_t* msg, int x, int y, float r, float g, float b, vgui::HFont m_hFont, bool add = false);
void ScaleColors(int& r, int& g, int& b, int a);
inline void UnpackRGB(int& r, int& g, int& b, unsigned long ulRGB)\
{\
r = (ulRGB & 0xFF0000) >> 16; \
g = (ulRGB & 0xFF00) >> 8; \
b = ulRGB & 0xFF; \
}