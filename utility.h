#pragma once
#define GET_SCREEN_PIXEL(h, str) GetScreenPixel(h, atof(pScheme->GetResourceString(str)))
void RGBToHSV(int r, int g, int b, float& h, float& s, float& v);
void HSVToRGB(float h, float s, float v, int& r, int& g, int& b);
int GetScreenPixel(bool h, float percent);