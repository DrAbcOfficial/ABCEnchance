#include "plugins.h"
#include <string>
#include <vector>
//Lib
#include "cl_entity.h"
#include "mymathlib.h"
#include "com_model.h"
#include "triangleapi.h"
#include "pm_movevars.h"
#include "cvardef.h"
#include "httpclient.h"

#include "Task.h"
//Def
#include "hud.h"
#include "vguilocal.h"
#include "exportfuncs.h"
#include "Color.h"
#include "weapon.h"
#include "usercmd.h"
#include "extraprecache.h"
#include "pm_defs.h"
#include "entity_types.h"
#include "StudioModelRenderer.h"
#include "autofunc.h"

#include "vgui_controls/Controls.h"
#include "config.h"
#include "playertrace.h"
//GL
#include "glew.h"
#include "gl_def.h"
#include "gl_shader.h"
#include "gl_utility.h"
#include "gl_draw.h"
//Base HUD
#include "CCustomHud.h"
#include "local.h"
#include "steam_api.h"
#include "player_info.h"
//HUD
#include "neteasemusic.h"
#include "Viewport.h"
//efx
#include "efxenchance.h"
#include "viewmodellag.h"
#include <voice_status.h>
#include <ClientParticleMan.h>

#define _STR(s) #s
#define STR(s) _STR(s)

#pragma region External program management variables
overviewInfo_t* gDevOverview;
void* g_pClientSoundEngine;
const clientdata_t* gClientData;
CGameStudioModelRenderer* g_StudioRenderer;
ref_params_t* g_clientrefparams = nullptr;
DWORD g_dwHUDListAddr;
#pragma endregion

#pragma region Memcpy or internal management variable
//memcpy
engine_studio_api_t IEngineStudio;
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

	gCustomHud.HUD_Reset();
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
				g_pViewPort->ShowMOTD();
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
	CMathlib::VectorCopy(punchangle1, g_pViewPort->m_vecClientEVPunch);
}
extern bool g_bInRenderRadar;
static int CL_IsDevOverview(void) {
	return g_bInRenderRadar ? 1 : gHookFuncs.CL_IsDevOverview();
}
static void CL_SetDevOverView(int param1) {
	gHookFuncs.CL_SetDevOverView(param1);
	if (g_bInRenderRadar) {
		(*(vec3_t*)(param1 + 0x1C))[CMathlib::Q_YAW] = gCustomHud.m_flOverViewYaw;
		*(float*)(param1 + 0x10) = gCustomHud.m_vecOverViewOrg[0];
		*(float*)(param1 + 0x14) = gCustomHud.m_vecOverViewOrg[1];
		gDevOverview->z_max = gCustomHud.m_flOverViewZmax;
		gDevOverview->z_min = gCustomHud.m_flOverViewZmin;
		gDevOverview->zoom = gCustomHud.m_flOverViewScale;
	}
}
static void R_ForceCVars(int mp) {
	if (CL_IsDevOverview())
		return;
	gHookFuncs.R_ForceCVars(mp);
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
	if (channel == 7 && g_pViewPort->GetMusicPanel()->IsSuppressBackGroudMusic()) {
		if (g_pViewPort->GetMusicPanel()->GetPlayListSize() > 0)
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
#define R_ISCLOVERVIEW_SIG "\xD9\x05\x2A\x2A\x2A\x2A\xD9\xEE\xDA\xE9\xDF\xE0\xF6\xC4\x44\x2A\x2A\x83\x3D\x2A\x2A\x2A\x2A\x00\x2A\x2A\xB8\x01\x00\x00\x00\xC3\x2A\x2A"
		Fill_Sig(R_ISCLOVERVIEW_SIG, g_dwEngineBase, g_dwEngineSize, CL_IsDevOverview);
#define GL_BIND_SIG "\x8B\x44\x24\x04\x39\x05\x2A\x2A\x2A\x2A\x2A\x2A\x50\x68\xE1\x0D\x00\x00\xA3\x2A\x2A\x2A\x2A\xFF\x15\x2A\x2A\x2A\x2A\xC3"
		Fill_Sig(GL_BIND_SIG, g_dwEngineBase, g_dwEngineSize, GL_Bind);
#define CL_SETDEVOVERVIEW "\xD9\x05\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\xDC\xC9"
		Fill_Sig(CL_SETDEVOVERVIEW, g_dwEngineBase, g_dwEngineSize, CL_SetDevOverView);
#define R_FORCECVAR_SIG "\x83\x7C\x24\x2A\x00\x2A\x2A\x2A\x2A\x00\x00\x81\x3D\x2A\x2A\x2A\x2A\xFF\x00\x00\x00"
		Fill_Sig(R_FORCECVAR_SIG, g_dwEngineBase, g_dwEngineSize, R_ForceCVars);
#define CL_FINDMODELBYINDEX_SIG "\x83\xEC\x08\x56\x57\x8B\x7C\x24\x14\x8B\x34\xBD\x2A\x2A\x2A\x2A\x85\xF6\x75\x08\x5F\x33\xC0\x5E\x83\xC4\x08\xC3"
		Fill_Sig(CL_FINDMODELBYINDEX_SIG, g_dwEngineBase, g_dwEngineSize, CL_GetModelByIndex);
#define CEngineClient_RenderView_SIG "\xFF\x74\x24\x04\x2A\x2A\x2A\x2A\x2A\x83\xC4\x04\x2A\x2A\x2A\x2A\x2A\x80\x7C\x24\x08\x00\xD9\xEE\x2A\x2A\x83\xEC\x10\xD9\x54\x24\x0C\xD9"
		Fill_Sig(CEngineClient_RenderView_SIG, g_dwEngineBase, g_dwEngineSize, CEngineClient_RenderView);
		DWORD addr;
#define DEVOVERVIEW_SIG "\x83\xEC\x30\xDD\x5C\x24\x2A\xD9\x05"
		{
			addr = (ULONG_PTR)Search_Pattern(DEVOVERVIEW_SIG);
			Sig_AddrNotFound(gDevOverview);
			gDevOverview = (decltype(gDevOverview))(*(ULONG_PTR*)(addr + 9) - 0xC);
		}
	}
}
extern void ClientDLLHook(void* interface_ptr);
void FillAddress() {
	auto pfnClientCreateInterface = g_pMetaHookAPI->GetClientFactory();
	auto SCClient001 = pfnClientCreateInterface("SCClientDLL001", 0);
	if (pfnClientCreateInterface && SCClient001) {
		ClientDLLHook(SCClient001);
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
		auto x = gHookFuncs.V_PunchAxis;
		PUCHAR addr;
		if (1)
		{
			addr = (PUCHAR)g_pMetaHookAPI->SearchPattern(g_pMetaSave->pExportFuncs->HUD_VidInit, 0x10, "\xB9", 1);
			g_dwHUDListAddr = *(DWORD*)(addr + 0x1);
		}
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
	Install_InlineEngHook(CL_IsDevOverview);
	Install_InlineEngHook(CL_SetDevOverView);
	Install_InlineEngHook(CL_GetModelByIndex);
	Install_InlineEngHook(R_ForceCVars);
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
void CheckOtherPlugin() {
	mh_plugininfo_t info;
	if (g_pMetaHookAPI->GetPluginInfo("Renderer.dll", &info)) {
		memcpy(&g_metaplugins.renderer.info, &info, sizeof(info));
		g_metaplugins.renderer.has = true;
	}
}
inline void CheckAsset() {
	if (!g_pFileSystem->FileExists("abcenchance/ABCEnchance.res"))
		SYS_ERROR("Missing resource files!\nPlease make sure the \"abcenchance/\" folder is placed correctly!");
}
#pragma endregion

#pragma region HUD_XXX Funcs
void GL_Init(void) {
	g_pMetaHookAPI->GetVideoMode(&gScreenInfo.iWidth, &gScreenInfo.iHeight, nullptr, nullptr);
	auto err = glewInit();
	if (GLEW_OK != err) {
		SYS_ERROR("glewInit failed, %s", glewGetErrorString(err));
		return;
	}
	GL_ShaderInit();
	gCustomHud.GL_Init();
}
void HUD_Init(void) {
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
		if (cvar->value != PLUGIN_VERSION)
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

	gExportfuncs.HUD_Init();
	gCustomHud.HUD_Init();
	g_pViewPort->Init();
	GetClientVoiceMgr()->HUD_Init();
	extern void GameUI_GetInterface();
	GameUI_GetInterface();
	abcconfig::LoadJson();
	AutoFunc::Init();
}
int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio) {
	memcpy(&IEngineStudio, pstudio, sizeof(IEngineStudio));
	return gExportfuncs.HUD_GetStudioModelInterface(version, ppinterface, pstudio);
}
void HUD_Shutdown(void) {
	AutoFunc::Exit();

	gExportfuncs.HUD_Shutdown();

	gCustomHud.HUD_Clear();
	GL_FreeShaders();
	ClearExtraPrecache();

	GetTaskManager()->Shutdown();

	extern void FMOD_Shutdown();
	extern void FreeLibcurl();
	FreeLibcurl();
	FMOD_Shutdown();
	FreeParticleMan();
	UninstallClientHook();
	CHttpClient::ShutDown();

	GetClientVoiceMgr()->HUD_Shutdown();
	abcconfig::SaveJson();
}
int HUD_VidInit(void) {
	//Search and destory vanillia HUDs
	if (g_dwHUDListAddr) {
		HUDLIST* pHudList = reinterpret_cast<HUDLIST*>((*(DWORD*)(g_dwHUDListAddr)));
		for (size_t i = 0; i <= 4; i++) {
			switch (i) {
			case 0x0:gHookHud.m_Health = reinterpret_cast<CHudHealth*>(pHudList->p); break;
			case 0x1:gHookHud.m_Battery = reinterpret_cast<CHudBattery*>(pHudList->p); break;
			case 0x2:gHookHud.m_Ammo = reinterpret_cast<CHudAmmo*>(pHudList->p); break;
			case 0x4:gHookHud.m_Flash = reinterpret_cast<CHudFlashlight*>(pHudList->p); break;
			default:break;
			}
			pHudList = pHudList->pNext;
		}

		//nah
		//while (pHudList) {
		//	// Use RTTI to know which HUD
		//	//if (dynamic_cast<CHudBattery*>(pHudList->p) != nullptr)
		//	//	gHookHud.m_Battery = (dynamic_cast<CHudBattery*>(pHudList->p));
		//	pHudList = pHudList->pNext;
		//}
	}
	else
		SYS_ERROR("Can not find vanillin HUDs");
	if (g_pViewPort)


		//Fillup Default CVars
		gCVars.pCvarDefaultFOV = CVAR_GET_POINTER("default_fov");
	gCVars.pCVarDevOverview = CVAR_GET_POINTER("dev_overview");
	gCVars.pCVarDrawEntities = CVAR_GET_POINTER("r_drawentities");
	gCVars.pCVarDrawViewModel = CVAR_GET_POINTER("r_drawviewmodel");
	gCVars.pCVarDrawDynamic = CVAR_GET_POINTER("r_dynamic");
	gCVars.pCVarFXAA = CVAR_GET_POINTER("r_fxaa");
	gCVars.pCVarWater = CVAR_GET_POINTER("r_water");
	gCVars.pCVarShadow = CVAR_GET_POINTER("r_shadow");

	int result = gExportfuncs.HUD_VidInit();
	gCustomHud.HUD_VidInit();
	g_pViewPort->VidInit();
	return result;
}
void HUD_VoiceStatus(int entindex, qboolean talking) {
	GetClientVoiceMgr()->UpdateSpeakerStatus(entindex, talking);
	gExportfuncs.HUD_VoiceStatus(entindex, talking);
}
void HUD_Frame(double frametime) {
	GetClientVoiceMgr()->HUD_Frame(frametime);
	gExportfuncs.HUD_Frame(frametime);
	//task
	GetTaskManager()->CheckAll();
	GetPlayerTrace()->Update();
	EFX_Frame();
	CHttpClient::RunFrame();
}
int HUD_Redraw(float time, int intermission) {
	CCustomHud::HideOriginalHud();
	gCustomHud.HUD_Draw(time);
	g_pViewPort->SetInterMission(intermission);
	return gExportfuncs.HUD_Redraw(time, intermission);
}
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client) {
	gClientData = client;
	gExportfuncs.HUD_TxferLocalOverrides(state, client);
}
int HUD_UpdateClientData(struct client_data_s* c, float f) {
	s_flFov = c->fov;
	gCustomHud.HUD_UpdateClientData(c, f);
	return gExportfuncs.HUD_UpdateClientData(c, f);
}
void HUD_ClientMove(struct playermove_s* ppmove, qboolean server) {
	gExportfuncs.HUD_PlayerMove(ppmove, server);
	g_playerppmove.inwater = (ppmove->waterlevel > 1);
	g_playerppmove.onground = (ppmove->onground != -1);
	g_playerppmove.walking = (ppmove->movetype == MOVETYPE_WALK);
}
void HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd) {
	gCustomHud.HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	gExportfuncs.HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
}
void V_CalcRefdef(struct ref_params_s* pparams) {
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
	g_clientrefparams = pparams;
}
void IN_MouseEvent(int mstate) {
	gCustomHud.IN_MouseEvent(mstate);
	gExportfuncs.IN_MouseEvent(mstate);
}
void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active) {
	gExportfuncs.CL_CreateMove(frametime, cmd, active);
	if (gCustomHud.IsInScore())
		cmd->buttons |= IN_SCORE;
	AutoFunc::AutoJump(cmd);
	AutoFunc::DuckTap(cmd);
}
int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname) {
	if (!gCustomHud.HUD_AddEntity(type, ent, modelname))
		return 0;
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
int HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding) {
	return g_pViewPort->KeyInput(eventcode, keynum, pszCurrentBinding) ?
		gExportfuncs.HUD_Key_Event(eventcode, keynum, pszCurrentBinding) : 0;
}
void HUD_TempEntUpdate(
	double frametime,   // Simulation time
	double client_time, // Absolute time on client
	double cl_gravity,  // True gravity on client
	TEMPENTITY** ppTempEntFree,   // List of freed temporary ents
	TEMPENTITY** ppTempEntActive, // List 
	int		(*Callback_AddVisibleEntity)(cl_entity_t* pEntity),
	void	(*Callback_TempEntPlaySound)(TEMPENTITY* pTemp, float damp)) {
	Vector		vAngles;
	gEngfuncs.GetViewAngles((float*)vAngles);
	if (g_pParticleMan)
		g_pParticleMan->SetVariables(cl_gravity, vAngles);
	gExportfuncs.HUD_TempEntUpdate(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
}
void HUD_DrawTransparentTriangles() {
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