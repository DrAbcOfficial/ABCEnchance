#include <metahook.h>
#include "plugins.h"

#include "local.h"
#include "vguilocal.h"

#include <vector>
#include <string>
#include <thread>
#include <atomic>

#include "vpx/vpx_decoder.h"
#include "vpx/video_reader.h"
#include "vpx/tools_common.h"

#include "Controls.h"
#include "vgui_controls/ImagePanel.h"

#include "GameUI/BasePanel.h"

#include "exportfuncs.h"
#include "OptionAdvancedDlg.h"

extern DWORD g_dwEngineBuildnum;
extern IFileSystem* g_pFileSystem;

bool g_bInitialized = false;
int g_iTextureID;

vgui::IClientPanel* g_pBasePanel;

struct backgroundinfo_t {
	char video[MAX_PATH];
	char audio[MAX_PATH];
};
backgroundinfo_t* g_pNowChose;
std::vector<backgroundinfo_t*> g_aryBackGrounds;

std::atomic <const VpxVideoInfo*> g_pInfo;
std::atomic<vpx_codec_ctx_t*> g_pCodec;
std::atomic<CIVFVideoReader*> g_pReader;

struct asyncResult {
	byte* data;
	uint wide;
	uint tall;
};
std::atomic<asyncResult*> g_pVideoResult;
std::atomic_bool g_pThreadStop(false);
std::atomic_bool g_bPauseDecode;

std::thread g_pDecodeThread;


static vgui::DHANDLE<vgui::COptionsAdvanceDialog>g_pAdvanceOptPanel;
static vgui::PHandle g_pLogoImage;

void ReadBackGroundList() {
	char buffer2[MAX_PATH];
	char buffer1[MAX_PATH];
	const char* pfile = const_cast<const char*>(reinterpret_cast<char*>(gEngfuncs.COM_LoadFile("abcenchance/scence/background.txt", 5, nullptr)));
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
		else
			c++;
	}
	gEngfuncs.COM_FreeFile(const_cast<void*>(reinterpret_cast<const void*>(pfile)));
}
void OpenVideo() {
	static CIVFVideoReader pReader = CIVFVideoReader(g_pNowChose->video);
	g_pInfo = pReader.GetInfo();
	const VpxInterface* pDecoder = get_vpx_decoder_by_fourcc(g_pInfo.load()->codec_fourcc);
	static vpx_codec_ctx_t s_Codec;
	vpx_codec_dec_init(&s_Codec, pDecoder->codec_interface(), nullptr, 0);
	g_pCodec = &s_Codec;
	g_pReader = &pReader;
}
void CloseVideo() {
	if (g_pCodec != nullptr) {
		vpx_codec_destroy(g_pCodec.load());
		g_pCodec = nullptr;
	}
	if (g_pReader != nullptr) {
		g_pReader.load()->Close();
		g_pReader = nullptr;
	}
	g_pVideoResult = nullptr;
}
void PlayMp3() {
	char soundcmd[MAX_PATH + 8];
	Q_snprintf(soundcmd, "mp3 stop;mp3 loop %s ui", g_pNowChose->audio);
	EngineClientCmd(soundcmd);
}
void StopMp3() {
	EngineClientCmd("mp3 stop");
}
void DecodeVideo() {
	do {
		if (g_bPauseDecode)
			continue;
		auto start = std::chrono::high_resolution_clock::now();
		int result = g_pReader.load()->ReadFrame();
		if (!result) {
			g_pReader.load()->ResetToBegine();
			g_pReader.load()->ReadFrame();
		}
		size_t frame_size = 0;
		const byte* frame = g_pReader.load()->GetFrame(&frame_size);
		vpx_codec_err_t err = vpx_codec_decode(g_pCodec.load(), frame, frame_size, nullptr, 0);
		vpx_codec_iter_t iter = nullptr;
		vpx_image_t* img = vpx_codec_get_frame(g_pCodec.load(), &iter);
		if (img) {
			static asyncResult returnVal;
			//wtf
			if ((img->fmt & VPX_IMG_FMT_PLANAR) == 0) {
				SYS_ERROR("VPX: unsupported video pixel format.");
				return;
			}
			static uint64_t s_iArea;
			static byte* s_pBuf;
			size_t imageWide = img->d_w;
			size_t imageHeight = img->d_h;
			if (s_iArea < imageWide * imageHeight) {
				s_iArea = imageWide * imageHeight;
				delete[] s_pBuf;
				s_pBuf = new byte[imageWide * imageHeight * 4];
			}
			size_t c = 0;
			int y_stride = img->stride[VPX_PLANE_Y];
			int u_stride = img->stride[VPX_PLANE_U];
			int v_stride = img->stride[VPX_PLANE_V];
			int a_stride = img->stride[VPX_PLANE_ALPHA];
			const static auto YUV2RGB = [](int Y, int U, int V, int& R, int& G, int& B) {
				const static constexpr auto RoundShr = [](int d, int s) -> int {
					return d >= 0 ?
						-((-d & (1 << (s - 1))) ? ((-(d)) >> (s)) + 1 : ((-(d)) >> (s))) :
						d & (1 << ((s)-1)) ? (d >> s) + 1 : (d >> s);
				};
				R = clamp<int>(RoundShr((((int)Y) << 14) + 22970 * (((int)V) - 128), 14), 0, 255);
				G = clamp<int>(RoundShr((((int)Y) << 14) - 5638 * (((int)U) - 128) - 11700 * (((int)V) - 128), 14), 0, 255);
				B = clamp<int>(RoundShr((((int)Y) << 14) + 29032 * (((int)U) - 128), 14), 0, 255);
				};
			for (size_t h = 0; h < imageHeight; h++) {
				for (size_t w = 0; w < imageWide; w++) {
					int y, u, v;
					int r, g, b, a;
					y = img->planes[VPX_PLANE_Y][h * y_stride + w];
					if ((img->fmt == VPX_IMG_FMT_I420) || (img->fmt == VPX_IMG_FMT_I42016) || 
						(img->fmt == VPX_IMG_FMT_NV12) || (img->fmt == VPX_IMG_FMT_YV12)) {
						u = img->planes[VPX_PLANE_U][h / 2 * u_stride + w / 2];
						v = img->planes[VPX_PLANE_V][h / 2 * v_stride + w / 2];
					}
					else if((img->fmt == VPX_IMG_FMT_I422) || (img->fmt == VPX_IMG_FMT_I42216)) {
						u = img->planes[VPX_PLANE_U][h * u_stride + w / 2];
						v = img->planes[VPX_PLANE_V][h * v_stride + w / 2];
					}
					else if ((img->fmt == VPX_IMG_FMT_I440) || (img->fmt == VPX_IMG_FMT_I44016)) {
						u = img->planes[VPX_PLANE_U][h / 2 * u_stride + w];
						v = img->planes[VPX_PLANE_V][h / 2 * v_stride + w];
					}
					else if ((img->fmt == VPX_IMG_FMT_I444) || (img->fmt == VPX_IMG_FMT_I44416)) {
						u = img->planes[VPX_PLANE_U][h * u_stride + w];
						v = img->planes[VPX_PLANE_V][h * v_stride + w];
					}
					YUV2RGB(y, u, v, r, g, b);
					a = img->planes[VPX_PLANE_ALPHA] ? img->planes[VPX_PLANE_ALPHA][h * a_stride + w] : 255;
					s_pBuf[c * 4 + 0] = r;
					s_pBuf[c * 4 + 1] = g;
					s_pBuf[c * 4 + 2] = b;
					s_pBuf[c * 4 + 3] = a;
					c++;
				}
			}
			returnVal.data = s_pBuf;
			returnVal.wide = imageWide;
			returnVal.tall = imageHeight;
			g_pVideoResult = &returnVal;
		};
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
		auto tbr = std::chrono::microseconds(static_cast<int>(static_cast<float>(g_pInfo.load()->time_base.numerator) / static_cast<float>(g_pInfo.load()->time_base.denominator) * 1000));
		std::this_thread::sleep_for(std::chrono::microseconds(duration > tbr ? std::chrono::microseconds(0) : tbr - duration));
	} while (!g_pThreadStop);
	CloseVideo();
}
void BackGroundVideoInit() {
	gCVars.pDynamicBackground = CREATE_CVAR("hud_dynamic_background", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		if (g_bInitialized)
			ConsoleWriteline("Background will changed in next start game.\n");
		});
	gCVars.pDynamicBackgroundLogo = CREATE_CVAR("hud_mainmenu_logo", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		if (g_pLogoImage)
			g_pLogoImage.Get()->SetVisible(cvar->value > 0);
	});
	gCVars.pDynamicBackgroundLogoXPos = CREATE_CVAR("hud_mainmenu_logo_xpos", "0", FCVAR_VALUE, [](cvar_t* cvar) {
		if (g_pLogoImage) {
			int w, h;
			auto panel = g_pLogoImage.Get();
			vgui::ipanel()->GetSize(panel->GetVParent(), w, h);
			panel->SetPos(w * cvar->value, panel->GetYPos());
		}
	});
	gCVars.pDynamicBackgroundLogoYPos = CREATE_CVAR("hud_mainmenu_logo_ypos", "0.3", FCVAR_VALUE, [](cvar_t* cvar) {
		if (g_pLogoImage) {
			int w, h;
			auto panel = g_pLogoImage.Get();
			vgui::ipanel()->GetSize(panel->GetVParent(), w, h);
			panel->SetPos(panel->GetXPos(), h * cvar->value);
		}
	});
	gCVars.pDynamicBackgroundLogoWide = CREATE_CVAR("hud_mainmenu_logo_wide", "0.5", FCVAR_VALUE, [](cvar_t* cvar) {
		if (g_pLogoImage) {
			int w, h;
			auto panel = g_pLogoImage.Get();
			vgui::ipanel()->GetSize(panel->GetVParent(), w, h);
			panel->SetWide(w * cvar->value);
		}
	});
	gCVars.pDynamicBackgroundLogoTall = CREATE_CVAR("hud_mainmenu_logo_tall", "0.25", FCVAR_VALUE, [](cvar_t* cvar) {
		if (g_pLogoImage) {
			int w, h;
			auto panel = g_pLogoImage.Get();
			vgui::ipanel()->GetSize(panel->GetVParent(), w, h);
			panel->SetTall(h * cvar->value);
		}
	});
	ReadBackGroundList();
	g_pNowChose = g_aryBackGrounds[gEngfuncs.pfnRandomLong(0, g_aryBackGrounds.size() - 1)];
}
void BackGroundVideoPostInit() {
	if (gCVars.pDynamicBackground->value > 0) {
		OpenVideo();
		PlayMp3();
		g_pThreadStop = false;
		g_pDecodeThread = std::thread(DecodeVideo);
		g_pDecodeThread.detach();
	}
	else {
		g_pThreadStop = true;
		EngineClientCmd("mp3 stop;mp3 loop media/gamestartup.mp3 ui");
	}
}
void BackGroundVideoClose() {
	g_pThreadStop = true;
	for (auto iter = g_aryBackGrounds.begin(); iter != g_aryBackGrounds.end(); iter++) {
		delete (*iter);
	}
	g_aryBackGrounds.clear();
}
void BackGroundSetDecodeState(bool state) {
	if (g_pThreadStop)
		return;
	if (state) {
		g_pReader.load()->ResetToBegine();
		PlayMp3();
	}
	else
		StopMp3();
	g_bPauseDecode = !state;
}
void BackGroundInitMusic() {
	if (!g_pThreadStop)
		PlayMp3();
}
vgui::IClientPanel* BasePanel() {
	return g_pBasePanel;
}

void SetAdvanceOptPanelVisible(bool state) {
	if (g_pAdvanceOptPanel) {
		if (state)
			g_pAdvanceOptPanel->Activate();
		else
			g_pAdvanceOptPanel->Close();
	}
}
void OpenAdvanceOptPanel(vgui::Panel* pthis) {
	if (g_pAdvanceOptPanel == nullptr) {
		g_pAdvanceOptPanel = new vgui::COptionsAdvanceDialog(pthis);
	}
	g_pAdvanceOptPanel->Activate();
}
void* __fastcall CBasePanel_ctor(void* pthis, int dummy) {
	g_pBasePanel = static_cast<vgui::IClientPanel*>(gHookFuncs.CBasePanel_ctor(pthis, dummy));
	g_iTextureID = vgui::surface()->CreateNewTextureID(true);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "gameui/OptionsAdvanceDialogScheme.res", "OptionsAdvanceDialogScheme");
	return g_pBasePanel;
}
void __fastcall CBasePanel_PaintBackground(void* pthis, int dummy) {
	if (!g_bInitialized) {
		BackGroundVideoPostInit();
		vgui::ImagePanel* img = new vgui::ImagePanel(g_pBasePanel->GetPanel(), "Logo");
		img->SetShouldScaleImage(true);
		if (g_metaplugins.renderer.has)
			img->SetImage("abcenchance/tga/sven_logo");
		else
			img->SetImage("abcenchance/tga/sven_logo_small");
		img->SetVisible(gCVars.pDynamicBackgroundLogo->value > 0);
		int x, y, w, h;
		vgui::ipanel()->GetSize(g_pBasePanel->GetVPanel(), w, h);
		x = w * gCVars.pDynamicBackgroundLogoXPos->value;
		y = h * gCVars.pDynamicBackgroundLogoYPos->value;
		int nw = w * gCVars.pDynamicBackgroundLogoWide->value;
		int nh = h * gCVars.pDynamicBackgroundLogoTall->value;
		img->SetBounds(x, y, nw, nh);
		g_pLogoImage = img;
		g_bInitialized = true;
	}
	if (g_pThreadStop) {
		gHookFuncs.CBasePanel_PaintBackground(pthis, dummy);
		return;
	}
	if (g_pVideoResult.is_lock_free()) {
		asyncResult* value = g_pVideoResult.load();
		if (value)
			vgui::surface()->DrawSetTextureRGBA(g_iTextureID, value->data, value->wide, value->tall, true, false);
	}
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawSetTexture(g_iTextureID);
	vgui::surface()->DrawTexturedRect(0, 0, ScreenWidth(), ScreenHeight());
}

void BasePanel_InstallHook(void){
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (!hGameUI) {
		SYS_ERROR("Failed to locate GameUI.dll");
		return;
	}
	auto GameUIBase = g_pMetaHookAPI->GetModuleBase(hGameUI);
	auto GameUISize = g_pMetaHookAPI->GetModuleSize(hGameUI);

#define SC_CBASEPANEL_CTOR_SIG "\x55\x8B\xEC\x51\x56\x68\x2A\x2A\x2A\x2A\x8B\xF1\x6A\x00\x89\x75\xFC\xE8\x2A\x2A\x2A\x2A\xC7"
	Fill_Sig(SC_CBASEPANEL_CTOR_SIG, GameUIBase, GameUISize, CBasePanel_ctor);
	Install_InlineHook(CBasePanel_ctor);
#define SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG "\x55\x8B\xEC\x83\xEC\x38\x53\x8D\x45\xCC\x8B\xD9\x50\x8D\x45\xC8\x89\x5D\xD0\x50\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8D\x4D\xD4\x51"
	Fill_Sig(SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG, GameUIBase, GameUISize, CBasePanel_PaintBackground);
	Install_InlineHook(CBasePanel_PaintBackground);

	HMODULE hVPX = LoadLibrary("vpx.dll");
	if (!hVPX) {
		SYS_ERROR("Could not load vpx.dll!");
		return;
	}
#define GetFuncVPX(name) name = (decltype(name))(GetProcAddress(hVPX, #name))
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