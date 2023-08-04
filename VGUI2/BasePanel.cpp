#include <metahook.h>

#include "local.h"
#include "vguilocal.h"
#include "IFileSystem.h"
#include "avilib/avilib.h"

#include <Controls.h>

#include "BasePanel.h"

#include "exportfuncs.h"
#include "plugins.h"
#include <vector>

avireader_t* g_pReader = nullptr;
byte* g_pVideoBuffer = nullptr;
int i_stream = ~0u;
int g_iTextureID;
float g_frameTime;
float g_nextFrameTime;
uint g_maxFrame = 0;
uint g_Frameidx = 0;

void BackGroundVideoInit() {
	g_pReader = avireader_create();
	static char localPath[256];
	g_pFileSystem->GetLocalPath("abcenchance/avi/background.avi", localPath, 256);
	if (!avireader_open(g_pReader, localPath)) {
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not open avi/background.avi!");
		return;
	}
	avilib_BITMAPINFO bm_info = { 0 };
	avilib_streamtype_t streamtype = avilib_UnknownStreamtype;
	avilib_streamtype_t desired_streamtype = avilib_Video;
	uint32_t alloc_size;
	while (streamtype != desired_streamtype)
		avireader_get_stream_type(g_pReader, ++i_stream, &streamtype);
	if (streamtype == avilib_Video)
		avireader_get_vformat(g_pReader, i_stream, &bm_info);
	avireader_get_alloc_size(g_pReader, i_stream, &alloc_size);
	avireader_get_frame_count(g_pReader, i_stream, &g_maxFrame);
	double vrate;
	avireader_get_vrate(g_pReader, &vrate);
	g_frameTime = 1 / vrate;
	g_pVideoBuffer = new byte[alloc_size];
}

void BackGroundVideoClose() {
	avireader_destroy(g_pReader);
	delete g_pVideoBuffer;
}

void __fastcall CBasePanel_ApplySchemeSettings(void* pthis, int dummy, void* shcemebutidontcare) {
	g_iTextureID = vgui::surface()->CreateNewTextureID(true);
	gHookFuncs.CBasePanel_ApplySchemeSettings(pthis, dummy, shcemebutidontcare);
}

void __fastcall CBasePanel_PaintBackground(void* pthis, int dummy) {
	float time = vgui::system()->GetFrameTime();
	if (time >= g_nextFrameTime) {
		g_nextFrameTime = time + g_frameTime;
		g_Frameidx++;
		if (g_Frameidx >= g_maxFrame - 1)
			g_Frameidx = 0;
		int w, h;
		avireader_get_size(g_pReader, &w, &h);
		int readsize = avireader_read_frame(g_pReader, g_Frameidx, i_stream, g_pVideoBuffer);
		std::vector<byte> temp;
		byte u, y1, v, y2;
		size_t c = 0;
		const static auto yuv2rgb = [](byte yValue, byte uValue, byte vValue, byte* r, byte* g, byte* b) {
			*r = yValue + (1.370705 * (vValue - 128));
			*g = yValue - (0.698001 * (vValue - 128)) - (0.337633 * (uValue - 128));
			*b = yValue + (1.732446 * (uValue - 128));
			*r = clamp<byte>(*r, 0, 255);
			*g = clamp<byte>(*g, 0, 255);
			*b = clamp<byte>(*b, 0, 255);
		};
		for (uint i = 0; i < readsize / sizeof(byte); i++) {
			if (c == 0)
				u = g_pVideoBuffer[i];
			else if (c == 1)
				y1 = g_pVideoBuffer[i];
			else if (c == 2)
				v = g_pVideoBuffer[i];
			else {
				y2 = g_pVideoBuffer[i];
				byte r, g, b;
				yuv2rgb(y1, u, v, &r, &g, &b);
				temp.push_back(r);
				temp.push_back(g);
				temp.push_back(b);
				temp.push_back(255);
				yuv2rgb(y2, u, v, &r, &g, &b);
				temp.push_back(r);
				temp.push_back(g);
				temp.push_back(b);
				temp.push_back(255);
				c = 0;
				continue;
			}
			c++;
		}
		vgui::surface()->DrawSetTextureRGBA(g_iTextureID, temp.data(), w, h, false, false);
	}
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawSetTexture(g_iTextureID);
	vgui::surface()->DrawTexturedRect(0, 0, ScreenWidth, ScreenHeight);
}

void BasePanel_InstallHook(void)
{
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (hGameUI) {
		CreateInterfaceFn fnCreateInterface = Sys_GetFactory(hGameUI);
		if (fnCreateInterface) {
#define SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG "\x55\x8B\xEC\x83\xEC\x38\x53\x8D\x45\xCC\x8B\xD9\x50\x8D\x45\xC8\x89\x5D\xD0\x50\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8D\x4D\xD4\x51"
			Fill_Sig(SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG, hGameUI, g_pMetaHookAPI->GetModuleSize(hGameUI), CBasePanel_PaintBackground);
			Install_InlineHook(CBasePanel_PaintBackground);
#define SC_CBASEPANEL_APPLYSCHEMESETTING_SIG "\x55\x8B\xEC\x81\xEC\x20\x02\x00\x00\xA1\x2A\x2A\x2A\x2A\x33\xC5\x89\x45\xFC\x8B\x45\x08\x53\x56\x50"
			Fill_Sig(SC_CBASEPANEL_APPLYSCHEMESETTING_SIG, hGameUI, g_pMetaHookAPI->GetModuleSize(hGameUI), CBasePanel_ApplySchemeSettings);
			Install_InlineHook(CBasePanel_ApplySchemeSettings);
		}
	}
	else
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not find game ui for dynamic background.");
}