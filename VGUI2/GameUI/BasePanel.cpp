#include <metahook.h>

#include "local.h"
#include "vguilocal.h"
#include "IFileSystem.h"
#include "IVanilliaPanel.h"

#include <vgui_controls/Slider.h>
#include <ModelViewPanel.h>

#include <vector>
#include <string>
#include <thread>
#include <atomic>

#include "vpx/vpx_decoder.h"
#include "vpx/video_reader.h"
#include "vpx/tools_common.h"

#include "Controls.h"

#include "GameUI/BasePanel.h"

#include "exportfuncs.h"
#include "plugins.h"

bool g_bInitialized = false;
int g_iTextureID;

IVanilliaPanel* g_pBasePanel;

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
			vgui::filesystem()->GetLocalPath(buffer1, info->video, MAX_PATH);
			strcpy(info->audio, buffer2);
			g_aryBackGrounds.push_back(info);
			c = 0;
		}
		else
			c++;
	}
	gEngfuncs.COM_FreeFile(pfile);
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
			//not 444, fuck it
			if ((img->fmt != VPX_IMG_FMT_I444) && (img->fmt != VPX_IMG_FMT_I44416))
				return;
			static size_t s_iArea;
			static byte* s_pBuf;
			if (s_iArea < img->d_w * img->d_h) {
				s_iArea = img->d_w * img->d_h;
				delete[] s_pBuf;
				s_pBuf = new byte[img->d_w * img->d_h * 4];
			}
			size_t c = 0;
			size_t enumW = img->stride[VPX_PLANE_Y];
			size_t enumH = img->h;
			asyncResult returnVal;
			const static auto YUV2RGB = [](int Y, int U, int V, int* R, int* G, int* B) {
				int iTmpR = 0;
				int iTmpG = 0;
				int iTmpB = 0;

				iTmpR = (((int)Y) << 14) + 22970 * (((int)V) - 128);
				iTmpG = (((int)Y) << 14) - 5638 * (((int)U) - 128) - 11700 * (((int)V) - 128);
				iTmpB = (((int)Y) << 14) + 29032 * (((int)U) - 128);
				const static constexpr auto RoundShr = [](int d, int s) -> int {
					return d >= 0 ?
						-((-d & (1 << (s - 1))) ? ((-(d)) >> (s)) + 1 : ((-(d)) >> (s))) :
						d & (1 << ((s)-1)) ? (d >> s) + 1 : (d >> s);
					};
				iTmpR = RoundShr(iTmpR, 14);
				iTmpG = RoundShr(iTmpG, 14);
				iTmpB = RoundShr(iTmpB, 14);

				*R = clamp<int>(iTmpR, 0, 255);
				*G = clamp<int>(iTmpG, 0, 255);
				*B = clamp<int>(iTmpB, 0, 255);
				};
			for (size_t h = 0; h < enumH; h++) {
				if (h >= img->d_h)
					continue;
				for (size_t w = 0; w < enumW; w++) {
					if (w >= img->d_w)
						break;
					size_t i = w + (h * enumW);
					//use int damit!
					int r, g, b, a;
					YUV2RGB(
						img->planes[VPX_PLANE_Y][i],
						img->planes[VPX_PLANE_U][i],
						img->planes[VPX_PLANE_V][i],
						&r, &g, &b
					);
					a = img->planes[VPX_PLANE_ALPHA] ? img->planes[VPX_PLANE_ALPHA][i] : 255;
					s_pBuf[c * 4 + 0] = r;
					s_pBuf[c * 4 + 1] = g;
					s_pBuf[c * 4 + 2] = b;
					s_pBuf[c * 4 + 3] = a;
					c++;
				}
			}
			returnVal.data = s_pBuf;
			returnVal.wide = img->d_w;
			returnVal.tall = img->d_h;
			g_pVideoResult = &returnVal;
		};
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
		auto tbr = std::chrono::microseconds(static_cast<int>(static_cast<float>(g_pInfo.load()->time_base.numerator) / static_cast<float >(g_pInfo.load()->time_base.denominator) * 1000));
		std::this_thread::sleep_for(std::chrono::microseconds(duration > tbr ? std::chrono::microseconds(0) : tbr - duration));
	} while (!g_pThreadStop);
	CloseVideo();
}
void BackGroundVideoInit() {
	gCVars.pDynamicBackground = CREATE_CVAR("hud_dynamic_background", "0", FCVAR_VALUE, [](cvar_t* cvar) {
		if (g_bInitialized)
			ConsoleWriteline("Background will changed in next lauch.\n");
	});
	ReadBackGroundList();
	g_pNowChose = g_aryBackGrounds[gEngfuncs.pfnRandomLong(0, g_aryBackGrounds.size()-1)];
}
void BackGroundVideoPostInit(){
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
IVanilliaPanel* BasePanel(){
	return g_pBasePanel;
}

void* __fastcall CBasePanel_ctor(void* pthis, int dummy) {
	g_pBasePanel = static_cast<IVanilliaPanel*>(gHookFuncs.CBasePanel_ctor(pthis, dummy));
	g_iTextureID = vgui::surface()->CreateNewTextureID(true);
	return g_pBasePanel;
}
void __fastcall CBasePanel_RunMenuCommand(void* pthis, int dummy, const char* command) {
	gHookFuncs.CBasePanel_RunMenuCommand(pthis, dummy, command);
}
void __fastcall CBasePanel_PaintBackground(void* pthis, int dummy) {
	if (!g_bInitialized) {
		BackGroundVideoPostInit();
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
	vgui::surface()->DrawTexturedRect(0, 0, ScreenWidth, ScreenHeight);
}
vgui::ModelViewPanel* g_modelviewPanel;
vgui::Slider* g_modelviewSlider;
void* __fastcall COptionsSubMultiplayer_ctor(vgui::Panel* pthis, int dummy, void* parent) {
	IVanilliaEditablePanel* res = reinterpret_cast<IVanilliaEditablePanel*>(gHookFuncs.COptionsSubMultiplayer_ctor(pthis, dummy, parent));
	IVanilliaPanel* modelbitmap = *reinterpret_cast<IVanilliaPanel**>(reinterpret_cast<int>(res) + 0xb8);
	modelbitmap->SetVisible(false);
	vgui::ModelViewPanel* panel = new vgui::ModelViewPanel(pthis, "ModelImage");
	int x, y, w, h;
	vgui::ipanel()->GetPos(modelbitmap->GetVPanel(), x, y);
	vgui::ipanel()->GetSize(modelbitmap->GetVPanel(), w, h);

	int w2, h2;
	IVanilliaPanel* slider2 = *reinterpret_cast<IVanilliaPanel**>(reinterpret_cast<int>(res) + 0x10c);
	vgui::ipanel()->GetSize(slider2->GetVPanel(), w2, h2);

	panel->SetBounds(x, y, w, h - h2);
	panel->SetupTexBuffer();
	panel->SetModelPos(0, 45, 0);
	panel->SetAnimate(true);
	panel->SetBgColor(modelbitmap->GetBgColor());
	g_modelviewPanel = panel;

	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile("resource/ClientScheme.res", nullptr);
	vgui::Slider* slider = new vgui::Slider(pthis, "Rotate Slider");
	slider->SetRange(0, 360);
	slider->SetBounds(x, y + h - h2, w, h2);
	slider->SetScheme(scheme);
	slider->AddActionSignalTarget(pthis);
	g_modelviewSlider = slider;
	return res;
}
void __fastcall RemapPalette(vgui::Panel* pthis, int dummy, char* modelname, int color1, int color2) {
	char temp[MAX_PATH];
	std::snprintf(temp, MAX_PATH, "models/player/%s/%s.mdl", modelname, modelname);
	g_modelviewPanel->LoadModel(temp);
	g_modelviewPanel->SetModelRotate(0, g_modelviewSlider->GetValue(), 0);
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
#define SC_CBASEPANEL_RUNMENUCOMMAND_SIG "\x55\x8B\xEC\x6A\xFF\x68\x2A\x2A\x2A\x2A\x64\xA1\x2A\x2A\x2A\x2A\x50\x51\x53\x56\x57\xA1\x2A\x2A\x2A\x2A\x33\xC5\x50\x8D\x45\xF4\x64\xA3\x2A\x2A\x2A\x2A\x8B\xD9\x8B\x75\x08\x68\x2A\x2A\x2A\x2A\x56"
			Fill_Sig(SC_CBASEPANEL_RUNMENUCOMMAND_SIG, hGameUI, moduleSize, CBasePanel_RunMenuCommand);
			Install_InlineHook(CBasePanel_RunMenuCommand);
#define SC_COPTIONSUBMULTIPLAYER_CTOR_SIG "\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x6A\xFF\x68\x2A\x2A\x2A\x2A\x64\xA1\x2A\x2A\x2A\x2A\x50\x53\x81\xEC\x68\x01\x00\x00"
			Fill_Sig(SC_COPTIONSUBMULTIPLAYER_CTOR_SIG, hGameUI, moduleSize, COptionsSubMultiplayer_ctor);
			Install_InlineHook(COptionsSubMultiplayer_ctor);
#define REMAPPALLET_SIG "\x55\x8B\xEC\x6A\xFF\x68\x2A\x2A\x2A\x2A\x64\xA1\x2A\x2A\x2A\x2A\x50\x81\xEC\x2C\x02\x00\x00\xA1\x2A\x2A\x2A\x2A\x33\xC5\x89\x45\xF0\x53\x56\x57\x50\x8D\x45\xF4\x64\xA3\x00\x00\x00\x00\x8B\x75\x08\x8D\x8D\xC8\xFD\xFF\xFF\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x56\x56"
			Fill_Sig(REMAPPALLET_SIG, hGameUI, moduleSize, RemapPalette);
			auto x = gHookFuncs.RemapPalette;
			Install_InlineHook(RemapPalette);
		}
	}
	else
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not find game ui for dynamic background.");

	HMODULE hVPX= LoadLibrary("vpx.dll");
	if (hVPX) {
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
	else
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not open vpx.dll!");
}