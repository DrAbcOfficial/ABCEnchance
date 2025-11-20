#include "plugins.h"
#include <string>
#include <vector>
//Lib
#include "cl_entity.h"
#include "mymathlib.h"
#include "com_model.h"
#include "cvardef.h"
#include "httpclient.h"

#include "Task.h"
//Def
#include "vguilocal.h"
#include "exportfuncs.h"
#include "usercmd.h"
#include "extraprecache.h"
#include "pm_defs.h"
#include "entity_types.h"
#include "StudioModelRenderer.h"
#include "autofunc.h"

#include "vgui_controls/Controls.h"
#include "config.h"
#include "playertrace.h"
#include "core/events/networkmessage.h"
#include "core/events/command.h"
#include "core/events/hudevents.h"

#include "core/MetaRendererCallbacks.h"
//GL
#include "glew.h"
//Base HUD
#include "local.h"
#include "hud/Viewport.h"
//efx
#include "efxenchance.h"
#include "viewmodellag.h"
#include <voice_status.h>
#include <ClientParticleMan.h>

void GL_ShaderInit();
void GL_FreeShaders();
void MetaRenderer_Init();

#define _STR(s) #s
#define STR(s) _STR(s)

#pragma region External program management variables
void* g_pClientSoundEngine;
const clientdata_t* gClientData;
CGameStudioModelRenderer* g_StudioRenderer;
DWORD g_dwHUDListAddr;
hudpanel_info_t* g_aryNativeHUDPanelInfo;
/* (msprite_s**) */ void* gpSprite;
#pragma endregion

#pragma region Memcpy or internal management variable
//memcpy
engine_studio_api_t gEngineStudio;
cl_enginefunc_t gEngfuncs;
cl_exportfuncs_t gExportfuncs;
//new
playerstatus_t g_playerppmove;
metaplugins_t g_metaplugins;
static float s_flFov;
//hooks
static std::vector<hook_t*> s_aryEngineHook = {};
static std::vector<hook_t*> s_aryClientHook = {};
#pragma endregion

#pragma region Hooked Funcs
//FINAL SHIT
static void R_NewMap(void) {
	//まともになったんだよ~
	//これ　勣らないから
	ClearExtraPrecache();
	EFX_Reset();
	gHookFuncs.R_NewMap();
}
static int __fastcall R_CrossHair_ReDraw(void* pthis, int dummy, int param_1) {
	if (gCVars.pDynamicCrossHair->value > 0)
		return 0;
	return gHookFuncs.R_CrossHair_ReDraw(pthis, dummy, param_1);
}
static void __fastcall TFV_ShowScoreBoard(void* pthis) {
	return;
	//gHookFuncs.TFV_ShowScoreBoard(pthis);
}
static void __fastcall TFV_ShowVGUIMenu(void* pthis, int dummy, int iVguiMenu) {
	switch (iVguiMenu) {
		//MissionBrief
	case 0x4: return;
		//MOTD
	case 0x5: {
		if (gCVars.pMotd->value != 0) {
			if (gCVars.pMotd->value >= 1)
				GetBaseViewPort()->ShowMOTD();
			return;
		}
		break;
	}
			//TeamMenu
	case 0x2:
		//Vote
	case 0x16:
		//MapVote
	case 0x17:
		//VotePopUp
	case 0x18:
		//CameraMenu
	case 0x19:
		//InventroyMenu
	case 0x1A:
		//MediaMenu
	case 0x1E:
	default: break;
	}
	//mission brief shit2 is 4, but fku all vgui shit
	gHookFuncs.TFV_ShowVGUIMenu(pthis, dummy, iVguiMenu);
}
static void __fastcall CStudioModelRenderer_Init(void* pthis, int dummy) {
	gHookFuncs.CStudioModelRenderer_Init(pthis, dummy);
	g_StudioRenderer = static_cast<CGameStudioModelRenderer*>(pthis);
}

static void EVVectorScale(float* punchangle1, float scale, float* punchangle2) {
	gHookFuncs.EVVectorScale(punchangle1, scale, punchangle2);
	CMathlib::VectorCopy(punchangle1, GetBaseViewPort()->m_vecClientEVPunch);
}
static model_t* CL_GetModelByIndex(int index) {
	auto extra = GetExtraModelByModelIndex(index);
	if (extra)
		return extra;

	return gHookFuncs.CL_GetModelByIndex(index);
}
static void __fastcall CClient_SoundEngine_Initialize(void* pSoundEngine, int dummy) {
	gHookFuncs.CClient_SoundEngine_Initialize(pSoundEngine, dummy);
	g_pClientSoundEngine = pSoundEngine;
}
void __fastcall CClient_SoundEngine_PlayFMODSound(void* pSoundEngine, int dummy, int flags, int entindex, float* origin,
	int channel, const char* name, float fvol, float attenuation, int extraflags, int pitch, int sentenceIndex, float soundLength) {
#ifdef __HAS_NETEASE_API
	if (channel == 7 && GetBaseViewPort()->GetMusicPanel()->IsSuppressBackGroudMusic()) {
		if (GetBaseViewPort()->GetMusicPanel()->GetPlayListSize() > 0)
			return;
	}
#endif
	gHookFuncs.CClient_SoundEngine_PlayFMODSound(pSoundEngine, dummy, flags, entindex, origin, channel, name, fvol, attenuation, extraflags, pitch, sentenceIndex, soundLength);
}
#pragma endregion

#pragma region Fillup Address and Hook
void FillEngineAddress() {
	auto engineFactory = Sys_GetFactory((HINTERFACEMODULE)g_dwEngineBase);
	if (engineFactory && engineFactory("EngineSurface007", nullptr)) {
#define R_NEWMAP_SIG "\x55\x8B\xEC\x51\xC7\x45\xFC\x00\x00\x00\x00\xEB\x2A\x8B\x45\xFC\x83\xC0\x01\x89\x45\xFC\x81\x7D\xFC\x00\x01\x00\x00"
		Fill_Sig(R_NEWMAP_SIG, g_dwEngineBase, g_dwEngineSize, R_NewMap);
#define CL_FINDMODELBYINDEX_SIG "\x83\xEC\x08\x56\x57\x8B\x7C\x24\x14\x8B\x34\xBD\x2A\x2A\x2A\x2A\x85\xF6\x75\x08\x5F\x33\xC0\x5E\x83\xC4\x08\xC3"
		Fill_Sig(CL_FINDMODELBYINDEX_SIG, g_dwEngineBase, g_dwEngineSize, CL_GetModelByIndex);
#define CEngineClient_RenderView_SIG "\xFF\x74\x24\x04\x2A\x2A\x2A\x2A\x2A\x83\xC4\x04\x2A\x2A\x2A\x2A\x2A\x80\x7C\x24\x08\x00\xD9\xEE\x2A\x2A\x83\xEC\x10\xD9\x54\x24\x0C\xD9"
		Fill_Sig(CEngineClient_RenderView_SIG, g_dwEngineBase, g_dwEngineSize, CEngineClient_RenderView);
#define R_GetSpriteFrame_SIG "\x56\x8B\x2A\x2A\x2A\x2A\x33\xFF\x85\xF6\x75\x12\x68"
		Fill_Sig(R_GetSpriteFrame_SIG, g_dwEngineBase, g_dwEngineSize, R_GetSpriteFrame);
		DWORD addr;
#define GPSRITE_SIG "\x00\x00\x00\x8B\x89\x84\x01\x00\x00\x89\x0D"
		{
			addr = (ULONG_PTR)Search_Pattern(GPSRITE_SIG);
			Sig_AddrNotFound(gpSprite);
			gpSprite = (decltype(gpSprite))(*(ULONG_PTR*)(addr + 11));
		}
	}
}
void FillAddress() {
	g_dwClientTextBase = g_pMetaHookAPI->GetSectionByName(g_dwClientBase, ".text\0\0\0", &g_dwClientTextSize);
	if (!g_dwClientTextBase)
	{
		SYS_ERROR("Failed to locate section \".text\" in client.dll!");
		return;
	}
	g_dwClientDataBase = g_pMetaHookAPI->GetSectionByName(g_dwClientBase, ".data\0\0\0", &g_dwClientDataSize);
	if (!g_dwClientDataBase)
	{
		SYS_ERROR("Failed to locate section \".text\" in client.dll!");
		return;
	}
	auto pfnClientCreateInterface = g_pMetaHookAPI->GetClientFactory();
	auto SCClient001 = pfnClientCreateInterface("SCClientDLL001", 0);
	if (pfnClientCreateInterface && SCClient001) {
		//sig
#define SC_GETCLIENTCOLOR_SIG "\x8B\x4C\x24\x04\x85\xC9\x2A\x2A\x6B\xC1\x5C\x0F\xBF\x80\x2A\x2A\x2A\x2A\x48\x83"
		Fill_Sig(SC_GETCLIENTCOLOR_SIG, g_dwClientBase, g_dwClientSize, GetClientColor);
#define R_EVVECTORSCALE_SIG "\x8B\x4C\x24\x04\xF3\x0F\x10\x4C\x24\x08\x8B\x44\x24\x0C\x0F\x28\xC1\xF3\x0F\x59\x01\xF3\x0F\x11\x00\x0F\x28\xC1\xF3\x0F\x59\x41\x04\xF3\x0F\x11\x40\x04\xF3\x0F\x59\x49\x08\xF3\x0F\x11\x48\x08"
		Fill_Sig(R_EVVECTORSCALE_SIG, g_dwClientBase, g_dwClientSize, EVVectorScale);
#define R_CROSSHAIR_REDRAW_SIG "\x8B\x51\x14\x85\xD2\x0F\x84\x8B\x00\x00\x00\xA1\x2A\x2A\x2A\x2A\xF3\x0F\x2C\x40\x0C\x85\xC0\x7E\x2A\x83\x3D\x2A\x2A\x2A\x2A\x00\x53\x56\x57\x74\x2A\x80\x3D\x2A\x2A\x2A\x2A\x00\x75\x2A\xF3\x0F\x2C\x79\x34\xF3\x0F\x2C\x59\x38\xEB\x2A"
		Fill_Sig(R_CROSSHAIR_REDRAW_SIG, g_dwClientBase, g_dwClientSize, R_CrossHair_ReDraw);
#define TFV_SHOWSCOREBOARD_SIG "\x56\x8B\xF1\x8B\x8E\x2C\x2A\x00\x00\x85\xC9\x74\x0D\xE8\x2A\x2A\xFF\xFF\x8B\xCE\x5E\xE9\x2A\x2A\xFF\xFF\x5E\xC3\xCC\xCC\xCC\xCC"
		Fill_Sig(TFV_SHOWSCOREBOARD_SIG, g_dwClientBase, g_dwClientSize, TFV_ShowScoreBoard);
#define TFV_SHOWVGUIMENU_SHIT_SIG "\xA1\x2A\x2A\x2A\x2A\x57\x8B\xF9\x8B\x40\x04\xFF\xD0\x85\xC0\x0F\x85\x2A\x2A\x2A\x2A\x55\x8B\x6C\x24\x0C\x39\x05"
		Fill_Sig(TFV_SHOWVGUIMENU_SHIT_SIG, g_dwClientBase, g_dwClientSize, TFV_ShowVGUIMenu);
#define CStudioModelRenderer_Init_SIG "\xA1\x2A\x2A\x2A\x2A\x56\x68\x2A\x2A\x2A\x2A\x8B\xF1\x2A\x2A\x89\x46\x24\xA1"
		Fill_Sig(CStudioModelRenderer_Init_SIG, g_dwClientBase, g_dwClientSize, CStudioModelRenderer_Init);
#define Client_SoundEngine_Initialize_SIG "\x81\xEC\x8C\x00\x00\x00\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x84\x24\x88\x00\x00\x00\x53\x57\x6A\x00"
		Fill_Sig(Client_SoundEngine_Initialize_SIG, g_dwClientBase, g_dwClientSize, CClient_SoundEngine_Initialize);
#define Client_SoundEngine_PlayFMODSound_SIG "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC\x98\x00\x00\x00\xA1\x2A\x2A\x2A\x2A\x33"
		Fill_Sig(Client_SoundEngine_PlayFMODSound_SIG, g_dwClientBase, g_dwClientSize, CClient_SoundEngine_PlayFMODSound);
#define V_PunchAxis_SIG "\x8B\x44\x24\x04\xF3\x0F\x10\x44\x24\x08\xF3\x0F\x11\x04\x85\x2A\x2A\x2A\x2A\xC3\xCC"
		Fill_Sig(V_PunchAxis_SIG, g_dwClientBase, g_dwClientSize, V_PunchAxis);
#pragma region HUDList
		if (1) {
			PUCHAR addr = (PUCHAR)g_pMetaHookAPI->SearchPattern(g_pMetaSave->pExportFuncs->HUD_VidInit, 0x10, "\xB9", 1);
			g_dwHUDListAddr = *(DWORD*)(addr + 0x1);
		}
#pragma endregion
#pragma region Player Infos
		if (1) {
			/*
					10032b67 83 f8 07        CMP        pKey,0x7
					10032b6a 0f 87 1c        JA         LAB_10032c8c
							 01 00 00
					10032b70 6b c1 5c        IMUL       pKey,ECX,0x5c
					10032b73 c6 85 8f        MOV        byte ptr [EBP + local_175],0x1
							 fe ff ff 01
					10032b7a 66 89 90        MOV        word ptr [pKey + DAT_1064496c],DX                = ??
							 6c 49 64 10

					client.dll + 0x64496c = address of hud_nativeplayerinfo_t
			*/
			constexpr char pattern[] = "\xC6\x85\x8F\xFE\xFF\xFF\x01\x66\x89\x90";
			PUCHAR addr = (PUCHAR)Search_Pattern_From_Size(g_dwClientBase, g_dwClientSize, pattern);
			g_aryNativeHUDPanelInfo = reinterpret_cast<hudpanel_info_t*> (*(DWORD*)(addr + 10) + 8);
		}
#pragma endregion

	}
}
inline void AddHook(hook_t* h) {
	s_aryClientHook.push_back(h);
}
inline void AddEngineHook(hook_t* h) {
	s_aryEngineHook.push_back(h);
}
void InstallEngineHook() {
	Fill_InlineEfxHook(R_BloodSprite);
	Fill_InlineEfxHook(R_TempModel);
	Fill_EngFunc(pfnPlaybackEvent);

	Install_InlineEngHook(pfnPlaybackEvent);
	Install_InlineEngHook(R_NewMap);
	Install_InlineEngHook(CL_GetModelByIndex);
}
void InstallClientHook() {
	Install_InlineHook(EVVectorScale);
	Install_InlineHook(CClient_SoundEngine_PlayFMODSound);
	Install_InlineHook(CClient_SoundEngine_Initialize);
	Install_InlineHook(R_CrossHair_ReDraw);
	Install_InlineHook(TFV_ShowScoreBoard);
	Install_InlineHook(TFV_ShowVGUIMenu);
	Install_InlineHook(CStudioModelRenderer_Init);
}
void UninstallEngineHook() {
	for (hook_t* h : s_aryEngineHook) {
		Uninstall_Hook(h);
	}
	s_aryEngineHook.clear();
}
void UninstallClientHook() {
	for (hook_t* h : s_aryClientHook) {
		Uninstall_Hook(h);
	}
	s_aryClientHook.clear();
}
#pragma endregion

#pragma region Runtime Check

inline void CheckAsset() {
	if (!g_pFileSystem->FileExists("abcenchance/ABCEnchance.res"))
		SYS_ERROR("Missing resource files!\nPlease make sure the \"abcenchance/\" folder is placed correctly!");
}
#pragma endregion

#pragma region HUD_XXX Funcs

void GL_Init(void)
{
	//Load interface from Renderer.dll
	MetaRenderer_Init();
	g_pMetaHookAPI->GetVideoMode(&gScreenInfo.iWidth, &gScreenInfo.iHeight, nullptr, nullptr);
	auto err = glewInit();
	if (GLEW_OK != err) {
		SYS_ERROR("glewInit failed, %s", glewGetErrorString(err));
		return;
	}
	GL_ShaderInit();
	if (MetaRenderer())
	{
		MetaRenderer()->RegisterRenderCallbacks(&g_MetaRendererCallbacks);
	}
}

void HUD_Init(void)
{
	MathLib_Init();
	//VGUI init
	gCVars.pShellEfx = CREATE_CVAR("abc_shellefx", "1", FCVAR_VALUE, nullptr);
	gCVars.pBloodEfx = CREATE_CVAR("abc_bloodefx", "1", FCVAR_VALUE, nullptr);
	gCVars.pBloodSpriteSpeed = CREATE_CVAR("abc_bloodsprite_speed", "128", FCVAR_VALUE, nullptr);
	gCVars.pBloodSpriteNumber = CREATE_CVAR("abc_bloodsprite_num", "32", FCVAR_VALUE, nullptr);
	gCVars.pGaussEfx = CREATE_CVAR("abc_gaussefx", "1", FCVAR_VALUE, nullptr);
	gCVars.pEgonEfx = CREATE_CVAR("abc_egonefx", "1", FCVAR_VALUE, nullptr);

	gCVars.pModelLag = CREATE_CVAR("cl_modellag", "1", FCVAR_VALUE, nullptr);
	gCVars.pModelLagAutoStop = CREATE_CVAR("cl_modellag_autostop", "1", FCVAR_VALUE, nullptr);
	gCVars.pModelLagValue = CREATE_CVAR("cl_modellag_value", "1.0", FCVAR_VALUE, nullptr);

	gCVars.pModelSlide = CREATE_CVAR("cl_modelslide", "0", FCVAR_VALUE, nullptr);
	gCVars.pModelSlideAngle = CREATE_CVAR("cl_modelslide_angle", "24", FCVAR_VALUE, nullptr);
	gCVars.pModelSlideLength = CREATE_CVAR("cl_modelslide_length", "12", FCVAR_VALUE, nullptr);
	gCVars.pModelSlideHeight = CREATE_CVAR("cl_modelslide_height", "12", FCVAR_VALUE, nullptr);

	gCVars.pCamIdealHeight = CREATE_CVAR("cam_idealheight", "0", FCVAR_VALUE, nullptr);
	gCVars.pCamIdealRight = CREATE_CVAR("cam_idealright", "0", FCVAR_VALUE, nullptr);

	CREATE_CVAR("abc_version", STR(PLUGIN_VERSION), FCVAR_EXTDLL | FCVAR_CLIENTDLL, [](cvar_t* cvar) {
		gEngfuncs.Cvar_SetValue("abc_version", PLUGIN_VERSION);
		});

	ADD_COMMAND("models", []() {
		if (gEngfuncs.Cmd_Argc() <= 1)
			return;
		std::string sz = gEngfuncs.Cmd_Argv(1);
		sz = "models/player/" + sz + "*.*";
		gEngfuncs.Con_Printf("Models\n==============\n");
		FileFindHandle_t walk;
		auto pszFileName = vgui::filesystem()->FindFirst(sz.c_str(), &walk);
		if (pszFileName) {
			do {
				if (vgui::filesystem()->FindIsDirectory(walk))
					gEngfuncs.Con_Printf("%s\n", pszFileName);
			} while ((pszFileName = vgui::filesystem()->FindNext(walk)) != nullptr);
		}
		else {
			gEngfuncs.Con_Printf("!!!Nothing!!!\n");
			gEngfuncs.Con_Printf("==============\n");
			return;
		}
		vgui::filesystem()->FindClose(walk);
		gEngfuncs.Con_Printf("==============\n");
		});

	ADD_COMMAND("find", []() {
		if (gEngfuncs.Cmd_Argc() <= 1)
			return;
		std::string sz = gEngfuncs.Cmd_Argv(1);
		gEngfuncs.Con_Printf("==============\n");
		cvar_t* cvar = gEngfuncs.GetFirstCvarPtr();
		while (cvar)
		{
			std::string cvarname = cvar->name;
			if (cvarname.find(sz) != std::string::npos)
				gEngfuncs.Con_Printf("  %s  [%s]\n", cvar->name, (cvar->flags & FCVAR_PROTECTED) != 0 ? "**" : cvar->string);
			cvar = cvar->next;
		}
		gEngfuncs.Con_Printf("==============\n");
		unsigned int cmd = gEngfuncs.GetFirstCmdFunctionHandle();
		while (cmd) {
			std::string cmdname = gEngfuncs.GetCmdFunctionName(cmd);
			if (cmdname.find(sz) != std::string::npos)
				gEngfuncs.Con_Printf("  %s\n", cmdname.c_str());
			cmd = gEngfuncs.GetNextCmdFunctionHandle(cmd);
		}
		gEngfuncs.Con_Printf("==============\n");
		});

	gExportfuncs.HUD_Init();
	RegisterNetworkMessageEventTrigger();
	RegisterCommandEvents();
	GetClientVoiceMgr()->HUD_Init();
	extern void GameUI_GetInterface();
	GameUI_GetInterface();
	abcconfig::LoadJson();
	AutoFunc::Init();
}

int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{
	memcpy(&gEngineStudio, pstudio, sizeof(gEngineStudio));
	return gExportfuncs.HUD_GetStudioModelInterface(version, ppinterface, pstudio);
}

extern void FMOD_Shutdown();

void HUD_Shutdown(void)
{
	AutoFunc::Exit();

	gExportfuncs.HUD_Shutdown();

	GL_FreeShaders();
	ClearExtraPrecache();

	GetTaskManager()->Shutdown();

	FMOD_Shutdown();
	FreeParticleMan();
	UninstallClientHook();
	GetHttpClient()->ShutDown();
	GetClientVoiceMgr()->HUD_Shutdown();
	abcconfig::SaveJson();
}


using VanilliaHud = struct {
	void* m_vtable;
	int m_x;
	int m_y;
	int   m_type;
	int	  m_iFlags;
};
static std::vector<VanilliaHud*> s_aryVanillianHud{};

int HUD_VidInit(void)
{
	//Search and destory vanillia HUDs
	using hudlist_t = struct hudlist_s {
		VanilliaHud* p;
		hudlist_s* pNext;
	};
	if (g_dwHUDListAddr) {
		s_aryVanillianHud.clear();
		hudlist_t* pHudList = reinterpret_cast<hudlist_t*>((*(DWORD*)(g_dwHUDListAddr)));
		for (size_t i = 0; i <= 0x4; i++) {
			switch (i) {
				case 0x4:
				case 0x2:
				case 0x1:
				case 0x0: {
					s_aryVanillianHud.push_back(pHudList->p);
					break;
				}
				default:break;
			}
			pHudList = pHudList->pNext;
		}
	}
	else
		SYS_ERROR("Can not find vanillin HUDs");

	//Fillup Default CVars
	gCVars.pCvarDefaultFOV = CVAR_GET_POINTER("default_fov");
	gCVars.pCVarDevOverview = CVAR_GET_POINTER("dev_overview");
	gCVars.pCVarDrawEntities = CVAR_GET_POINTER("r_drawentities");
	gCVars.pCVarDrawViewModel = CVAR_GET_POINTER("r_drawviewmodel");
	gCVars.pCVarDrawDynamic = CVAR_GET_POINTER("r_dynamic");
	gCVars.pCVarFXAA = CVAR_GET_POINTER("r_fxaa");
	gCVars.pCVarWater = CVAR_GET_POINTER("r_water");
	gCVars.pCVarShadow = CVAR_GET_POINTER("r_shadow");
	gCVars.pCVarDeferredLighting = CVAR_GET_POINTER("r_deferred_lighting");
	gCVars.pCVarGammaBlend = CVAR_GET_POINTER("r_gamma_blend");

	int result = gExportfuncs.HUD_VidInit();
	GetBaseViewPort()->VidInit();
	return result;
}

void HUD_VoiceStatus(int entindex, qboolean talking)
{
	GetClientVoiceMgr()->UpdateSpeakerStatus(entindex, talking);
	gExportfuncs.HUD_VoiceStatus(entindex, talking);
}

void HUD_Frame(double frametime)
{
	GetClientVoiceMgr()->HUD_Frame(frametime);
	gExportfuncs.HUD_Frame(frametime);
	//task
	GetTaskManager()->CheckAll();
	GetPlayerTrace()->Update();
	EFX_Frame();
	GetHttpClient()->RunFrame();
}

int HUD_Redraw(float time, int intermission)
{
	for (auto h : s_aryVanillianHud) {
		h->m_iFlags &= ~1;
	}
	GetBaseViewPort()->SetInterMission(intermission);
	return gExportfuncs.HUD_Redraw(time, intermission);
}

void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
	gClientData = client;
	gExportfuncs.HUD_TxferLocalOverrides(state, client);
}

int HUD_UpdateClientData(struct client_data_s* c, float f)
{
	s_flFov = c->fov;
	HudEvent::OnUpdateClientData(c, f);
	return gExportfuncs.HUD_UpdateClientData(c, f);
}

void HUD_ClientMove(struct playermove_s* ppmove, qboolean server)
{
	gExportfuncs.HUD_PlayerMove(ppmove, server);
	g_playerppmove.inwater = (ppmove->waterlevel > 1);
	g_playerppmove.onground = (ppmove->onground != -1);
	g_playerppmove.walking = (ppmove->movetype == MOVETYPE_WALK);
}

void HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd)
{
	HudEvent::OnTxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	gExportfuncs.HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
}

void V_CalcRefdef(struct ref_params_s* pparams)
{
	//pparams->nextView will be zeroed by client dll
	gExportfuncs.V_CalcRefdef(pparams);
	if (!gExportfuncs.CL_IsThirdPerson()) {
		// fudge position around to keep amount of weapon visible
		// roughly equal with different FOV
		cl_entity_t* view = gEngfuncs.GetViewModel();
		if (pparams->viewsize == 110)
			view->origin[2] += 1;
		else if (pparams->viewsize == 100)
			view->origin[2] += 2;
		else if (pparams->viewsize == 90)
			view->origin[2] += 1;
		else if (pparams->viewsize == 80)
			view->origin[2] += 0.5;
		Vector viewOrigin = view->origin;
		Vector viewAngles = view->angles;
		V_CalcViewModelLag(pparams, viewOrigin, viewAngles, pparams->cl_viewangles);
		CMathlib::VectorCopy(viewOrigin, view->origin);
		CMathlib::VectorCopy(viewAngles, view->angles);
		V_CalcModelSlide(pparams);
	}
	else {
		vec3_t vecRight;
		CMathlib::AngleVectors(pparams->cl_viewangles, nullptr, vecRight, nullptr);
		CMathlib::VectorMultipiler(vecRight, gCVars.pCamIdealRight->value);
		pparams->vieworg[0] += vecRight[0];
		pparams->vieworg[1] += vecRight[1];
		pparams->vieworg[2] += gCVars.pCamIdealHeight->value + vecRight[2];
	}
}

void IN_MouseEvent(int mstate)
{
	HudEvent::OnIN_MouseEvent(mstate);
	gExportfuncs.IN_MouseEvent(mstate);
}

void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active)
{
	gExportfuncs.CL_CreateMove(frametime, cmd, active);
	if (GetBaseViewPort()->IsInScore())
		cmd->buttons |= IN_SCORE;
	AutoFunc::AutoJump(cmd);
	AutoFunc::DuckTap(cmd);
}

int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname)
{
	GetBaseViewPort()->AddEntity(type, ent, modelname);
	//hook for engon beam
	if (gCVars.pEgonEfx->value > 0) {
		if (type == ET_BEAM) {
			int beamtype = ent->curstate.rendermode & 0x0F;
			if (beamtype == 1) {
				int startent = ent->curstate.sequence;
				int endent = ent->curstate.skin;
				int etype = ent->curstate.entityType;
				if (startent == 0 && endent > 4096 && etype == 2 &&
					!strncmp(gEngfuncs.hudGetModelByIndex(ent->curstate.modelindex)->name, "sprites/xbeam1.spr", 19)) {
					int beamflags = (ent->curstate.rendermode & 0xF0);
					int noise = ent->curstate.body;
					float scrollrate = ent->curstate.animtime;
					if ((beamflags == 16 && scrollrate == 5.0f && noise == 20) ||
						(beamflags == 0 && scrollrate == 2.5f && noise == 8)) {
						extern void StartEgonParticle(unsigned char r, unsigned char g, unsigned char b, struct cl_entity_s* ent);
						int index = endent & 0xFFF;
						if (index <= 33 && index >= 1) {
							cl_entity_t* end = gEngfuncs.GetEntityByIndex(index);
							if (end && end == gEngfuncs.GetLocalPlayer()) {
								StartEgonParticle(ent->curstate.rendercolor.r, ent->curstate.rendercolor.g, ent->curstate.rendercolor.b, ent);
								ent->curstate.rendercolor = { 0,0,0 };
							}
						}
					}
				}
			}
		}
		else if (type == ET_NORMAL) {
			if (ent->curstate.modelindex > 0 &&
				!strncmp(gEngfuncs.hudGetModelByIndex(ent->curstate.modelindex)->name, "sprites/XSpark1.spr", 19) &&
				ent->curstate.entityType == 1 && ent->curstate.scale == 1.0f && ent->curstate.rendermode == 3 && ent->curstate.renderamt == 255 &&
				ent->curstate.rendercolor.r == 255 && ent->curstate.rendercolor.g == 255 && ent->curstate.rendercolor.b == 255 &&
				ent->curstate.renderfx == 14 && ent->curstate.movetype == 8) {
				return 0;
			}
		}
	}
	return gExportfuncs.HUD_AddEntity(type, ent, modelname);
}

int HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding)
{
	return GetBaseViewPort()->KeyInput(eventcode, keynum, pszCurrentBinding) ?
		gExportfuncs.HUD_Key_Event(eventcode, keynum, pszCurrentBinding) : 0;
}

void HUD_TempEntUpdate(
	double frametime,   // Simulation time
	double client_time, // Absolute time on client
	double cl_gravity,  // True gravity on client
	TEMPENTITY** ppTempEntFree,   // List of freed temporary ents
	TEMPENTITY** ppTempEntActive, // List 
	int		(*Callback_AddVisibleEntity)(cl_entity_t* pEntity),
	void	(*Callback_TempEntPlaySound)(TEMPENTITY* pTemp, float damp))
{
	Vector		vAngles;
	gEngfuncs.GetViewAngles((float*)vAngles);
	if (g_pParticleMan)
		g_pParticleMan->SetVariables(cl_gravity, vAngles);
	gExportfuncs.HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
}

void HUD_DrawTransparentTriangles()
{
	if (g_pParticleMan)
		g_pParticleMan->Update();
	gExportfuncs.HUD_DrawTransparentTriangles();
}

#pragma endregion

#pragma region Extern funcs
float GetCurrentFOV() {
	return s_flFov;
}
#pragma endregion