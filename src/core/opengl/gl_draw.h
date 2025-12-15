#pragma once

extern void GL_Bind(int gltexturenum);
extern void DrawSPRIcon(int SprHandle, int mode, float x, float y, float w, float h, unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame = 0);
extern void DrawSPRIconRect(int SprHandle, int mode, float x, float y, float w, float h, float left, float right, float top, float bottom, unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame);
extern void DrawSPRIconPos(int SprHandle, int mode, float p1[2], float p2[2], float p3[2], float p4[2], unsigned char r, unsigned char g, unsigned char b, unsigned char a);
extern void DrawTexturePos(int tex, int mode, float p1[2], float p2[2], float p3[2], float p4[2], unsigned char r, unsigned char g, unsigned b, unsigned a);
extern void GetStringSize(const wchar_t* string, int* width, int* height, vgui::HFont m_hFont);
extern void ScaleColors(int& r, int& g, int& b, int a);