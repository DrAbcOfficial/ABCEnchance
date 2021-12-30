#include <metahook.h>

#include <cmath>
#include "mathlib.h"
#include "vguilocal.h"

#include "utility.h"

using namespace mathlib;

void RGBToHSV(int r, int g, int b, float& h, float& s, float& v){
	float fr = r / 255.0, fg = g / 255.0, fb = b / 255.0;
	float max = max3(fr, fg, fb);
	float min = min3(fr, fg, fb);
	float range = max - min;
	//H
	if (range <= 0)
		h = 0;
	else if (max == r)
		h = 60 * (fg - fb) / range + (g >= b ? 0 : 360);
	else if (max == g)
		h = 60 * (fb - fr) / range + 120;
	else
		h = 60 * (fr - fg) / range + 240;
	if (abs(h) >= 360)
		h = fmod(h, 360);
	//S
	s = max <= 0 ? 0 : range / max;
	//V
	v = max <= 0 ? 0 : max;
}
void HSVToRGB(float h, float s, float v, int& r, int& g, int& b){
	//0<=h<360
	//0<=s<=1
	//0<=v<=1
	h = fmod(h, 360);
	s = clamp(s, 0.0, 1.0);
	v = clamp(v, 0.0, 1.0);
	float section = h / 60;
	float c = v * s;
	float x = c * (1 - abs(fmod(section, 2) - 1));
	float hr = 0, hg = 0, hb = 0;
	switch ((int)section) {
	case 0:hr = c, hg = x, hb = 0; break;
	case 1:hr = x; hg = c; hb = 0; break;
	case 2:hr = 0; hg = c; hb = x; break;
	case 3:hr = 0; hg = x; hb = c; break;
	case 4:hr = x; hg = 0; hb = c; break;
	case 5:hr = c; hg = 0; hb = x; break;
	}
	float m = v - c;
	r = (hr + m) * 255;
	g = (hg + m) * 255;
	b = (hb + m) * 255;
}
int GetScreenPixel(bool h, float percent) {
	return (float)(h ? gScreenInfo.iHeight : gScreenInfo.iWidth) * clamp(percent, 0.0F, 1.0F);
}