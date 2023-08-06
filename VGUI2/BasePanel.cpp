#include <metahook.h>

#include "local.h"
#include "vguilocal.h"
#include "IFileSystem.h"

#include <vector>
#include <string>

#include "vpx/vpx_decoder.h"
#include "vpx/video_reader.h"
#include "vpx/tools_common.h"

#include "Controls.h"

#include "BasePanel.h"

#include "exportfuncs.h"
#include "plugins.h"

int g_iTextureID;
float g_flNextFrameTime;

typedef struct backgroundinfo_s {
	char video[MAX_PATH];
	char audio[MAX_PATH];
} backgroundinfo_t;

backgroundinfo_t* g_pNowChose;
std::vector<backgroundinfo_t*> g_aryBackGrounds;

const VpxVideoInfo* g_pInfo;
const VpxInterface* g_pDecoder;
vpx_codec_ctx_t* g_pCodec;
VpxVideoReader* g_pReader;

HMODULE g_pVpxdll;

void ReadBackGroundList() {
	char buffer2[MAX_PATH];
	char buffer1[MAX_PATH];

	char* pfile = (char*)gEngfuncs.COM_LoadFile(const_cast<char*>("abcenchance/scence/background.txt"), 5, nullptr);
	size_t c = 0;
	while (true) {
		if (c >= 1)
			strcpy(buffer1, buffer2);
		pfile = gEngfuncs.COM_ParseFile(pfile, buffer2);
		if (!pfile)
			break;
		if (c >= 1) {
			backgroundinfo_t* info = new backgroundinfo_t();
			g_pFileSystem->GetLocalPath(buffer1, info->video, MAX_PATH);
			strcpy(info->audio, buffer2);
			g_aryBackGrounds.push_back(info);
			c = 0;
		}
		c++;
	}
}
void OpenVideo(bool flushsound = true) {
	g_pReader = vpx_video_reader_open(g_pNowChose->video);
	g_pInfo = vpx_video_reader_get_info(g_pReader);
	g_pDecoder = get_vpx_decoder_by_fourcc(g_pInfo->codec_fourcc);
	g_pCodec = new vpx_codec_ctx_t();
	vpx_codec_dec_init(g_pCodec, g_pDecoder->codec_interface(), nullptr, 0);

	if (flushsound) {
		char soundcmd[MAX_PATH + 8];
		Q_snprintf(soundcmd, "mp3 loop %s", g_pNowChose->audio);
		EngineClientCmd(soundcmd);
	}
}
void CloseVideo() {
	vpx_codec_destroy(g_pCodec);
	vpx_video_reader_close(g_pReader);
	delete g_pCodec;
	g_pCodec = nullptr;
	g_pReader = nullptr;
}
void BackGroundVideoInit() {
	g_pVpxdll = LoadLibrary("vpx.dll");
	if (g_pVpxdll) {
#define GetFuncVPX(name) name = (decltype(name))(GetProcAddress(g_pVpxdll, #name))
		GetFuncVPX(vpx_codec_dec_init_ver);
		GetFuncVPX(vpx_codec_peek_stream_info);
		GetFuncVPX(vpx_codec_get_stream_info);
		GetFuncVPX(vpx_codec_decode);
		GetFuncVPX(vpx_codec_get_frame);
		GetFuncVPX(vpx_codec_register_put_frame_cb);
		GetFuncVPX(vpx_codec_register_put_slice_cb);
		GetFuncVPX(vpx_codec_set_frame_buffer_functions);
		GetFuncVPX(vpx_codec_vp8_dx);
		GetFuncVPX(vpx_codec_vp9_dx);
		GetFuncVPX(vpx_codec_destroy);
		GetFuncVPX(vpx_codec_error);
		GetFuncVPX(vpx_codec_error_detail);
		GetFuncVPX(vpx_img_wrap);
		GetFuncVPX(vpx_img_free);
#undef GetFuncVPX
	}
	else
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not open vpx.dll!");
	gCVars.pDynamicBackground = CREATE_CVAR("hud_dynamic_background", "1", FCVAR_VALUE, [](cvar_t* cvar){
		if (cvar->value > 0)
			OpenVideo();
		else
			CloseVideo();
	});
	ReadBackGroundList();
	g_pNowChose = g_aryBackGrounds[rand() % g_aryBackGrounds.size()];
	OpenVideo();
}

void BackGroundVideoClose() {
	CloseVideo();
	for (auto iter = g_aryBackGrounds.begin(); iter != g_aryBackGrounds.end(); iter++) {
		delete (*iter);
	}
	g_aryBackGrounds.clear();
}

void* __fastcall CBasePanel_ctor(void* pthis, int dummy) {
	g_iTextureID = vgui::surface()->CreateNewTextureID(true);
	return gHookFuncs.CBasePanel_ctor(pthis, dummy);
}

void YUV2RGB(byte y, byte u, byte v, byte* r, byte* g, byte* b){
	*r = clamp<byte>(y + (v - 128) * 1.14, 0.0, 255.0); // clamp the value to [0, 255]
	*g = clamp<byte>(y - (u - 128) * 0.39 - (v - 128) * 0.58, 0.0, 255.0);
	*b = clamp<byte>(y + (u - 128) * 2.03, 0.0, 255.0);
}

void __fastcall CBasePanel_PaintBackground(void* pthis, int dummy) {
	if (gCVars.pDynamicBackground->value <= 0) {
		gHookFuncs.CBasePanel_PaintBackground(pthis, dummy);
		return;
	}
	float time = vgui::system()->GetCurrentTime();
	if (time >= g_flNextFrameTime) {
		g_flNextFrameTime = time + (1 / g_pInfo->time_base.numerator);
		int result = vpx_video_reader_read_frame(g_pReader);
		if (!result) {
			CloseVideo();
			OpenVideo(false);
			vpx_video_reader_read_frame(g_pReader);
		}
		size_t frame_size = 0;
		const byte* frame = vpx_video_reader_get_frame(g_pReader, &frame_size);
		vpx_codec_err_t err = vpx_codec_decode(g_pCodec, frame, frame_size, nullptr, 0);
		vpx_codec_iter_t iter = nullptr;
		vpx_image_t* img = vpx_codec_get_frame(g_pCodec, &iter);
		if (img) {
			//not 444, fuck it
			if ((img->fmt != VPX_IMG_FMT_I444) && (img->fmt != VPX_IMG_FMT_I44416))
				return;
			size_t c = 0;
			byte* buf = new byte[img->d_w * img->d_h * 4];
			size_t enumW = img->stride[VPX_PLANE_Y];
			size_t enumH = img->h;
			for (size_t h = 0; h < enumH; h++) {
				if (h >= img->d_h)
					continue;
				for (size_t w = 0; w < enumW; w++) {
					if (w >= img->d_w)
						break;
					size_t i = w + (h * enumW);
					byte r, g, b, a;
					YUV2RGB(
						img->planes[VPX_PLANE_Y][i],
						img->planes[VPX_PLANE_U][i],
						img->planes[VPX_PLANE_V][i],
						&r, &g, &b
					);
					a = img->planes[VPX_PLANE_ALPHA] ? img->planes[VPX_PLANE_ALPHA][i] : 255;
					buf[c * 4 + 0] = r;
					buf[c * 4 + 1] = g;
					buf[c * 4 + 2] = b;
					buf[c * 4 + 3] = a;
					c++;
				}
			}
			vgui::surface()->DrawSetTextureRGBA(g_iTextureID, buf, img->d_w, img->d_h, true, false);
			delete[] buf;
		};
	}
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawSetTexture(g_iTextureID);
	vgui::surface()->DrawTexturedRect(0, 0, ScreenWidth, ScreenHeight);
}

void BasePanel_InstallHook(void){
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (hGameUI) {
		CreateInterfaceFn fnCreateInterface = Sys_GetFactory(hGameUI);
		DWORD moduleSize = g_pMetaHookAPI->GetModuleSize(hGameUI);
		if (fnCreateInterface) {
#define SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG "\x55\x8B\xEC\x83\xEC\x38\x53\x8D\x45\xCC\x8B\xD9\x50\x8D\x45\xC8\x89\x5D\xD0\x50\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8D\x4D\xD4\x51"
			Fill_Sig(SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG, hGameUI, moduleSize, CBasePanel_PaintBackground);
			Install_InlineHook(CBasePanel_PaintBackground);
#define SC_CBASEPANEL_CTOR_SIG "\x55\x8B\xEC\x51\x56\x68\x2A\x2A\x2A\x2A\x8B\xF1\x6A\x00\x89\x75\xFC\xE8\x2A\x2A\x2A\x2A\xC7"
			Fill_Sig(SC_CBASEPANEL_CTOR_SIG, hGameUI, moduleSize, CBasePanel_ctor);
			Install_InlineHook(CBasePanel_ctor);
//#define SC_CBASEPANEL_SETBKGRRENDERSTATE_SIG "\x55\x8B\xEC\x8B\x45\x08\x89\x41\x70\x5D\xC2\x04\x00\xCC\xCC\xCC\x32\xC0\xC3"
			//Fill_Sig(SC_CBASEPANEL_SETBKGRRENDERSTATE_SIG, hGameUI, moduleSize, CBasePanel_SetBackgroundRenderState);
			//Install_InlineHook(CBasePanel_SetBackgroundRenderState);
		}
	}
	else
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not find game ui for dynamic background.");
}