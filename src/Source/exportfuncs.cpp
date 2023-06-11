#include "plugins.h"
#include <vector>
#include <filesystem>
//Lib
#include "cmath"
#include "malloc.h"
#include "cl_entity.h"
#include "mymathlib.h"
#include "com_model.h"
#include "triangleapi.h"
#include "pm_movevars.h"
#include "cvar_hook.h"
#include <capstone.h>
#include "CVector.h"
//Def
#include "hud.h"
#include "vguilocal.h"
#include "exportfuncs.h"
#include "Color.h"
#include "weapon.h"
#include "usercmd.h"
#include "extraprecache.h"
#include "regquery.h"
#include "pm_defs.h"
#include "usercmd.h"
#include "vgui_controls/Controls.h"
//GL
#include "glew.h"
#include "gl_def.h"
#include "gl_shader.h"
#include "gl_utility.h"
#include "gl_draw.h"
//Base HUD
#include "CCustomHud.h"
#include "local.h"
#include <string>
#include "steam_api.h"
#include "player_info.h"
//HUD
#include "ammo.h"
#include "healthhud.h"
#include "scoreboard.h"
#include "Viewport.h"
//efx
#include "efxenchance.h"
#include "viewmodellag.h"
#include <voice_status.h>
#include <CVector.h>
#include <ClientParticleMan.h>

cl_enginefunc_t gEngfuncs;
cl_exportfuncs_t gExportfuncs;
metaplugins_t g_metaplugins;
engine_studio_api_t gEngineStudio;
DWORD g_dwHUDListAddr;

const clientdata_t* gClientData;
float m_hfov;

overviewInfo_t* gDevOverview;
uint* g_arySlotPuVar = nullptr;
refdef_t* g_refdef = nullptr;
netadr_s* g_pConnectingServer = nullptr;

struct playerppmoveinfo {
	bool onground;
	bool inwater;
	bool walking;
} g_playerppmove;

//VGUI2
HWND g_MainWnd = nullptr;
WNDPROC g_MainWndProc = nullptr;
bool g_bIMEComposing = false;
double g_flImeComposingTime = 0;
ICommandLine* CommandLine(void){
	return g_pInterface->CommandLine;
}
double GetAbsoluteTime(){
	return gEngfuncs.GetAbsoluteTime();
}
LRESULT WINAPI VID_MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	/*
	static HWND s_hLastHWnd;
	if (hWnd != s_hLastHWnd)
	{
		s_hLastHWnd = hWnd;
		vgui::input()->SetIMEWindow(hWnd);
	}

	switch (uMsg)
	{
	case WM_SYSCHAR:
	case WM_CHAR:
	{
		if (g_bIMEComposing)
			return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);

		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		if (wParam == VK_BACK)
		{
			if (g_bIMEComposing)
				return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
		}

		break;
	}
	case WM_INPUTLANGCHANGE:
	{
		vgui::input()->OnInputLanguageChanged();
		//break;
		return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_IME_STARTCOMPOSITION:
	{
		g_bIMEComposing = true;
		g_flImeComposingTime = GetAbsoluteTime();
		vgui::input()->OnIMEStartComposition();
		return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_IME_COMPOSITION:
	{
		int flags = (int)lParam;
		vgui::input()->OnIMEComposition(flags);
		return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_IME_ENDCOMPOSITION:
	{
		g_bIMEComposing = false;
		g_flImeComposingTime = GetAbsoluteTime();
		vgui::input()->OnIMEEndComposition();
		return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
	}

	case WM_IME_NOTIFY:
	{
		switch (wParam)
		{
		case IMN_OPENCANDIDATE:
		{
			vgui::input()->OnIMEShowCandidates();
			return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
		}

		case IMN_CHANGECANDIDATE:
		{
			vgui::input()->OnIMEChangeCandidates();
			return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
		}

		case IMN_CLOSECANDIDATE:
		{
			vgui::input()->OnIMECloseCandidates();
			//break;
			return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
		}

		case IMN_SETCONVERSIONMODE:
		case IMN_SETSENTENCEMODE:
		case IMN_SETOPENSTATUS:
		{
			vgui::input()->OnIMERecomputeModes();
			break;
		}

		case IMN_CLOSESTATUSWINDOW:
		case IMN_GUIDELINE:
		case IMN_OPENSTATUSWINDOW:
		case IMN_SETCANDIDATEPOS:
		case IMN_SETCOMPOSITIONFONT:
		case IMN_SETCOMPOSITIONWINDOW:
		case IMN_SETSTATUSWINDOWPOS:
		{
			break;
		}
		}

		break;
	}

	case WM_IME_SETCONTEXT:
	{
		lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
		lParam &= ~ISC_SHOWUIGUIDELINE;
		lParam &= ~ISC_SHOWUIALLCANDIDATEWINDOW;
		break;
	}
	}
	*/
	return CallWindowProc(g_MainWndProc, hWnd, uMsg, wParam, lParam);
}

//FINAL SHIT
void R_NewMap(void){
	ClearExtraPrecache();

	gCustomHud.HUD_Reset();
	EfxReset();
	gHookFuncs.R_NewMap();
}
int __fastcall R_CrossHair_ReDraw(void* pthis, int dummy, int param_1){
	if (gCVars.pDynamicCrossHair->value > 0)
		return 0;
	return gHookFuncs.R_CrossHair_ReDraw(pthis, dummy, param_1);
}
void __fastcall TFV_ShowScoreBoard(void* pthis) {
	if (gCVars.pDynamicHUD->value > 0)
		return;
	gHookFuncs.TFV_ShowScoreBoard(pthis);
}
void __fastcall TFV_ShowVGUIMenu(void* pthis, int dummy, int iVguiMenu) {
	switch (iVguiMenu){
		//MissionBrief
		case 4: return;
		//MOTD
		case 5: {
			g_pViewPort->ShowMOTD();
			return;
		}
		//TeamMenu
		case 2:
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
void EVVectorScale(float* punchangle1, float scale, float* punchangle2){
	gHookFuncs.EVVectorScale(punchangle1, scale, punchangle2);
	mathlib::VectorCopy(punchangle1, gCustomHud.m_vecClientEVPunch);
}
int CL_IsDevOverview(void){
	return gCustomHud.m_iIsOverView ? 1 : gHookFuncs.CL_IsDevOverview();
}
void CL_SetDevOverView(int param1){
	gHookFuncs.CL_SetDevOverView(param1);
	if (gCustomHud.m_iIsOverView){
		(*(vec3_t*)(param1 + 0x1C))[Q_YAW] = gCustomHud.m_flOverViewYaw;
		*(float *)(param1 + 0x10) = gCustomHud.m_vecOverViewOrg[0];
		*(float *)(param1 + 0x14) = gCustomHud.m_vecOverViewOrg[1];
		gDevOverview->z_max = gCustomHud.m_flOverViewZmax;
		gDevOverview->z_min = gCustomHud.m_flOverViewZmin;
		gDevOverview->zoom = gCustomHud.m_flOverViewScale;
	}	
}
void R_ForceCVars(qboolean mp){
	if (CL_IsDevOverview())
		return;
	gHookFuncs.R_ForceCVars(mp);
}
model_t* CL_GetModelByIndex (int index){
	if (index >= EXTRPRECACHE_INDEX_BASE && index < EXTRPRECACHE_INDEX_BASE + MAX_EXTRA_PRECACHENUM)
		return g_ExtraPreacheModel[index - EXTRPRECACHE_INDEX_BASE];
	return gHookFuncs.CL_GetModelByIndex(index);
}
void Cvar_DirectSet(cvar_t* var, char* value) {
	gHookFuncs.Cvar_DirectSet(var, value);
	if (gCVarsHookMap.find(var) != gCVarsHookMap.end())
		gCVarsHookMap[var](var);
}
void* NewClientFactory(void){
	return Sys_GetFactoryThis();
}
char* NewV_strncpy(char* a1, const char* a2, size_t a3){
	char language[128] = { 0 };
	const char* lang = NULL;
	auto gamedir = gEngfuncs.pfnGetGameDirectory();
	if (CommandLine()->CheckParm("-forcelang", &lang) && lang && lang[0])
		a2 = lang;
	else if ((gamedir && !strcmp(gamedir, "svencoop")) || CommandLine()->CheckParm("-steamlang")){
		Sys_GetRegKeyValue("Software\\Valve\\Steam", "Language", language, sizeof(language), "");
		if ((Q_strlen(language) > 0) && (Q_stricmp(language, "english")))
			a2 = language;
	}
	gHookFuncs.V_strncpy(m_szCurrentLanguage, a2, sizeof(m_szCurrentLanguage) - 1);
	m_szCurrentLanguage[sizeof(m_szCurrentLanguage) - 1] = 0;
	return gHookFuncs.V_strncpy(a1, a2, a3);
}
bool NET_StringToAdr(char* param_1, netadr_s* param_2) {
	bool result = gHookFuncs.NET_StringToAdr(param_1, param_2);
	g_pConnectingServer = param_2;
	return result;
}
void CheckOtherPlugin(){
	mh_plugininfo_t info;
	g_metaplugins.renderer = g_pMetaHookAPI->GetPluginInfo("Renderer.dll", &info);
	if(!g_metaplugins.renderer)
		g_metaplugins.renderer = g_pMetaHookAPI->GetPluginInfo("Renderer_AVX2.dll", &info);
	g_metaplugins.captionmod = g_pMetaHookAPI->GetPluginInfo("CaptionMod.dll", &info);
}
void FillEfxAddress(){

}
IBaseInterface* NewCreateInterface(const char* pName, int* pReturnCode){
	auto fnCreateInterface = (decltype(NewCreateInterface)*)Sys_GetFactoryThis();
	auto fn = fnCreateInterface(pName, pReturnCode);
	if (fn)
		return fn;
	fnCreateInterface = (decltype(NewCreateInterface)*)GetProcAddress(g_hClientDll, CREATEINTERFACE_PROCNAME);
	fn = fnCreateInterface(pName, pReturnCode);
	if (fn)
		return fn;
	return nullptr;
}
PVOID VGUIClient001_CreateInterface(HINTERFACEMODULE hModule){
	if (hModule == (HINTERFACEMODULE)g_hClientDll && !g_IsClientVGUI2)
		return NewCreateInterface;
	return Sys_GetFactory(hModule);
}
void FillEngineAddress() {
	auto engineFactory = Sys_GetFactory((HINTERFACEMODULE)g_dwEngineBase);
	if (engineFactory && engineFactory("EngineSurface007", nullptr)) {
#define R_NEWMAP_SIG "\x55\x8B\xEC\x51\xC7\x45\xFC\x00\x00\x00\x00\xEB\x2A\x8B\x45\xFC\x83\xC0\x01\x89\x45\xFC\x81\x7D\xFC\x00\x01\x00\x00"
		Fill_Sig(R_NEWMAP_SIG, g_dwEngineBase, g_dwEngineSize, R_NewMap);
#define R_ISCLOVERVIEW_SIG "\xD9\x05\x2A\x2A\x2A\x2A\xD9\xEE\xDA\xE9\xDF\xE0\xF6\xC4\x44\x2A\x2A\x83\x3D\x2A\x2A\x2A\x2A\x00\x2A\x2A\xB8\x01\x00\x00\x00\xC3\x2A\x2A"
		Fill_Sig(R_ISCLOVERVIEW_SIG, g_dwEngineBase, g_dwEngineSize, CL_IsDevOverview);
#define GL_BIND_SIG "\x8B\x44\x24\x04\x39\x05\x2A\x2A\x2A\x2A\x2A\x2A\x50\x68\xE1\x0D\x00\x00\xA3\x2A\x2A\x2A\x2A\xFF\x15\x2A\x2A\x2A\x2A\xC3"
		Fill_Sig(GL_BIND_SIG, g_dwEngineBase, g_dwEngineSize, GL_Bind);
#define CL_SETDEVOVERVIEW "\xD9\x05\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\xDC\xC9\xD9\x05\x2A\x2A\x2A\x2A\xDE\xE2\xD9\xC9\xD9\x1D\x2A\x2A\x2A\x2A\xD8\x0D\x2A\x2A\x2A\x2A\xD8\x2D\x2A\x2A\x2A\x2A\xD9\x1D\x2A\x2A\x2A\x2A\xD9\xEE\xD9\x05\x2A\x2A\x2A\x2A\xD8\xD1\xDF\xE0\xD9\xE8\xD9\x05\x2A\x2A\x2A\x2A\xF6\xC4\x41\x2A\x2A\xD8\xC1\xD9\x15"
		Fill_Sig(CL_SETDEVOVERVIEW, g_dwEngineBase, g_dwEngineSize, CL_SetDevOverView);
#define R_FORCECVAR_SIG "\x83\x7C\x24\x2A\x00\x2A\x2A\x2A\x2A\x00\x00\x81\x3D\x2A\x2A\x2A\x2A\xFF\x00\x00\x00"
		Fill_Sig(R_FORCECVAR_SIG, g_dwEngineBase, g_dwEngineSize, R_ForceCVars);
#define CL_FINDMODELBYINDEX_SIG "\x83\xEC\x08\x56\x57\x8B\x7C\x24\x14\x8B\x34\xBD\x2A\x2A\x2A\x2A\x85\xF6\x75\x08\x5F\x33\xC0\x5E\x83\xC4\x08\xC3"
		Fill_Sig(CL_FINDMODELBYINDEX_SIG, g_dwEngineBase, g_dwEngineSize, CL_GetModelByIndex);
#define NET_STRINGTOADR_SIG "\x56\x57\x8B\x7C\x24\x0C\x68\x2A\x2A\x2A\x2A\x57\xE8\xDF\x5C\xFC\xFF\x83\xC4\x08\x85\xC0"
		Fill_Sig(NET_STRINGTOADR_SIG, g_dwEngineBase, g_dwEngineSize, NET_StringToAdr);

		DWORD addr;
#define R_VIEWREFDEF_SIG "\x68\x2A\x2A\x2A\x2A\xD9\x1D\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68"
		{
			addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, R_VIEWREFDEF_SIG, Sig_Length(R_VIEWREFDEF_SIG));
			Sig_AddrNotFound(g_refdef);
			auto r_refdef_viewangles = (vec_t*)(*(DWORD*)(addr + 28));
			g_refdef = (refdef_t*)((char*)r_refdef_viewangles - offsetof(refdef_t, viewangles));
		}
#define R_RENDERSCENE_SIG_SVENGINE "\xDD\xD8\xDD\xD8\xE8"
#define R_RENDERVIEW_SIG_SVENGINE "\x55\x8B\xEC\x83\xE4\xC0\x83\xEC\x34\x53\x56\x57\x8B\x7D\x08\x85\xFF"
		{
			Fill_Sig(R_RENDERVIEW_SIG_SVENGINE, g_dwEngineBase, g_dwEngineSize, R_RenderView);
			addr = (DWORD)Search_Pattern_From(gHookFuncs.R_RenderView, R_RENDERSCENE_SIG_SVENGINE);
			Sig_AddrNotFound(R_RenderScene);
			gHookFuncs.R_RenderScene = (void(*)(void))(addr + 5 + 4 + *(int*)(addr + 5));
		}
#define DEVOVERVIEW_SIG "\x83\xEC\x30\xDD\x5C\x24\x2A\xD9\x05"
		{
			addr = (DWORD)Search_Pattern(DEVOVERVIEW_SIG);
			Sig_AddrNotFound(gDevOverview);
			gDevOverview = (decltype(gDevOverview))(*(DWORD*)(addr + 9) - 0xC);
		}
		if (1){
			const char sigs1[] = "***PROTECTED***";
			auto Cvar_DirectSet_String = Search_Pattern_Data(sigs1);
			if (!Cvar_DirectSet_String)
				Cvar_DirectSet_String = Search_Pattern_Rdata(sigs1);
			Sig_VarNotFound(Cvar_DirectSet_String);
			char pattern[] = "\x68\x2A\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8";
			*(DWORD*)(pattern + 1) = (DWORD)Cvar_DirectSet_String;
			auto Cvar_DirectSet_Call = Search_Pattern(pattern);
			Sig_VarNotFound(Cvar_DirectSet_Call);
			gHookFuncs.Cvar_DirectSet = (decltype(gHookFuncs.Cvar_DirectSet))g_pMetaHookAPI->ReverseSearchFunctionBegin(Cvar_DirectSet_Call, 0x500);
			Sig_FuncNotFound(Cvar_DirectSet);
		}
		if (1)
		{
			PUCHAR SearchBegin = (PUCHAR)g_dwEngineTextBase;
			PUCHAR SearchEnd = SearchBegin + g_dwEngineTextSize;
			while (1)
			{
#define LANGUAGESTRNCPY_SIG "\x68\x80\x00\x00\x00\x50\x8D"
				PUCHAR LanguageStrncpy = (PUCHAR)g_pMetaHookAPI->SearchPattern(SearchBegin, SearchEnd - SearchBegin, LANGUAGESTRNCPY_SIG, sizeof(LANGUAGESTRNCPY_SIG) - 1);
				if (LanguageStrncpy)
				{
					typedef struct
					{
						bool bHasPushEax;
					}LanguageStrncpy_ctx;

					LanguageStrncpy_ctx ctx = { 0 };

					g_pMetaHookAPI->DisasmRanges(LanguageStrncpy, 0x30, [](void* inst, PUCHAR address, size_t instLen, int instCount, int depth, PVOID context)
						{
							auto ctx = (LanguageStrncpy_ctx*)context;
							auto pinst = (cs_insn*)inst;

							if (pinst->id == X86_INS_PUSH &&
								pinst->detail->x86.op_count == 1 &&
								pinst->detail->x86.operands[0].type == X86_OP_REG &&
								pinst->detail->x86.operands[0].reg == X86_REG_EAX)
							{
								ctx->bHasPushEax = true;
							}

							if (ctx->bHasPushEax)
							{
								if (address[0] == 0xE8)
								{
									gHookFuncs.V_strncpy = (decltype(gHookFuncs.V_strncpy))GetCallAddress(address);
									PUCHAR pfnNewV_strncpy = (PUCHAR)NewV_strncpy;
									int rva = pfnNewV_strncpy - (address + 5);
									g_pMetaHookAPI->WriteMemory(address + 1, (BYTE*)&rva, 4);
									return TRUE;
								}
								else if (address[0] == 0xEB)
								{
									char jmprva = *(char*)(address + 1);
									PUCHAR jmptarget = address + 2 + jmprva;

									if (jmptarget[0] == 0xE8)
									{
										gHookFuncs.V_strncpy = (decltype(gHookFuncs.V_strncpy))GetCallAddress(jmptarget);
										PUCHAR pfnNewV_strncpy = (PUCHAR)NewV_strncpy;
										int rva = pfnNewV_strncpy - (jmptarget + 5);
										g_pMetaHookAPI->WriteMemory(jmptarget + 1, (BYTE*)&rva, 4);
										return TRUE;
									}
								}
							}

							if (instCount > 5)
								return TRUE;

							if (address[0] == 0xCC)
								return TRUE;

							if (pinst->id == X86_INS_RET)
								return TRUE;

							return FALSE;
						}, 0, &ctx);

					SearchBegin = LanguageStrncpy + sizeof(LANGUAGESTRNCPY_SIG) - 1;
				}
				else
				{
					break;
				}
			}
		}
		if (!g_metaplugins.captionmod){
			const char sigs1[] = "VClientVGUI001";
			auto VClientVGUI001_String = Search_Pattern_Data(sigs1);
			if (!VClientVGUI001_String)
				VClientVGUI001_String = Search_Pattern_Rdata(sigs1);
			Sig_VarNotFound(VClientVGUI001_String);
			char pattern[] = "\x8B\x2A\x2A\x6A\x00\x68\x2A\x2A\x2A\x2A\x89";
			*(DWORD*)(pattern + 6) = (DWORD)VClientVGUI001_String;
			auto VClientVGUI001_PushString = Search_Pattern(pattern);
			Sig_VarNotFound(VClientVGUI001_PushString);

			const char sigs2[] = "\x83\x3D\x2A\x2A\x2A\x2A\x00\x2A\x2A\xFF\x35\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x85\xC0";
			auto Call_VClientVGUI001_CreateInterface = g_pMetaHookAPI->ReverseSearchPattern(VClientVGUI001_PushString, 0x50, sigs2, sizeof(sigs2) - 1);
			Sig_VarNotFound(Call_VClientVGUI001_CreateInterface);

			PUCHAR address = (PUCHAR)Call_VClientVGUI001_CreateInterface + 15;

			gHookFuncs.VGUIClient001_CreateInterface = (decltype(gHookFuncs.VGUIClient001_CreateInterface))GetCallAddress(address);

			PUCHAR pfnVGUIClient001_CreateInterface = (PUCHAR)VGUIClient001_CreateInterface;

			int rva = pfnVGUIClient001_CreateInterface - (address + 5);

			g_pMetaHookAPI->WriteMemory(address + 1, (BYTE*)&rva, 4);
		}
	}
}
void FillAddress(){
	auto pfnClientCreateInterface = Sys_GetFactory((HINTERFACEMODULE)g_dwClientBase);
	if (pfnClientCreateInterface && pfnClientCreateInterface("SCClientDLL001", 0)){
#define SC_GETCLIENTCOLOR_SIG "\x8B\x4C\x24\x04\x85\xC9\x2A\x2A\x6B\xC1\x58"
		Fill_Sig(SC_GETCLIENTCOLOR_SIG, g_dwClientBase, g_dwClientSize, GetClientColor);
#define R_EVVECTORSCALE_SIG "\x8B\x4C\x24\x04\xF3\x0F\x10\x4C\x24\x08\x8B\x44\x24\x0C\x0F\x28\xC1\xF3\x0F\x59\x01\xF3\x0F\x11\x00\x0F\x28\xC1\xF3\x0F\x59\x41\x04\xF3\x0F\x11\x40\x04\xF3\x0F\x59\x49\x08\xF3\x0F\x11\x48\x08"
		Fill_Sig(R_EVVECTORSCALE_SIG, g_dwClientBase, g_dwClientSize, EVVectorScale);
#define R_CROSSHAIR_REDRAW_SIG "\x8B\x51\x14\x85\xD2\x0F\x84\x8B\x00\x00\x00\xA1\x2A\x2A\x2A\x2A\xF3\x0F\x2C\x40\x0C\x85\xC0\x7E\x2A\x83\x3D\x2A\x2A\x2A\x2A\x00\x53\x56\x57\x74\x2A\x80\x3D\x2A\x2A\x2A\x2A\x00\x75\x2A\xF3\x0F\x2C\x79\x34\xF3\x0F\x2C\x59\x38\xEB\x2A"
		Fill_Sig(R_CROSSHAIR_REDRAW_SIG, g_dwClientBase, g_dwClientSize, R_CrossHair_ReDraw);
#define TFV_SHOWSCOREBOARD_SIG "\x56\x8B\xF1\x8B\x8E\x2C\x10\x00\x00\x85\xC9\x74\x0D\xE8\xDE\xC3\xFF\xFF\x8B\xCE\x5E\xE9\x06\xF9\xFF\xFF\x5E\xC3\xCC\xCC\xCC\xCC"
		Fill_Sig(TFV_SHOWSCOREBOARD_SIG, g_dwClientBase, g_dwClientSize, TFV_ShowScoreBoard);
#define TFV_SHOWVGUIMENU_SHIT_SIG "\xA1\x2A\x2A\x2A\x2A\x57\x8B\xF9\x8B\x40\x04\xFF\xD0\x85\xC0\x0F\x85\x2A\x2A\x2A\x2A\x55\x8B\x6C\x24\x0C\x39\x05"
		Fill_Sig(TFV_SHOWVGUIMENU_SHIT_SIG, g_dwClientBase, g_dwClientSize, TFV_ShowVGUIMenu);

		DWORD addr;
#define R_SETPUNCHANGLE_SIG "\x83\xC4\x04\xD9\x1C\x24\x6A\x00\xE8\x93\x56\x05\x00\x83\xC4\x08\xF3\x0F\x10\x74\x24\x34\xF3\x0F\x10\xAC\x24\x98\x00\x00\x00\xF3\x0F\x10\x25\x2A\x2A\x2A\x2A\xF3\x0F\x58\xEE\xF3\x0F\x10\x44\x24\x74"
		{
			addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, R_SETPUNCHANGLE_SIG, sizeof(R_SETPUNCHANGLE_SIG) - 1);
			Sig_AddrNotFound(SetPunchAngle);
			gHookFuncs.SetPunchAngle = (decltype(gHookFuncs.SetPunchAngle))GetCallAddress(addr + 8);
		}
#define SC_SLOTPOS_PUVAR_SIG "\x53\x8B\x5C\x24\x08\x55\x56\x57\x8B\xE9\x33\xFF\xBE"
		{
			addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, SC_SLOTPOS_PUVAR_SIG, Sig_Length(SC_SLOTPOS_PUVAR_SIG));
			Sig_AddrNotFound(g_arySlotPuVar);
			g_arySlotPuVar = *(decltype(g_arySlotPuVar)*)(addr + 13);
		}
		if (1)
		{
			addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_pMetaSave->pExportFuncs->HUD_VidInit, 0x10, "\xB9", 1);
			g_dwHUDListAddr = *(DWORD*)(addr + 0x1);
		}
	}
}
std::vector<hook_t*> aryEngineHook = {};
std::vector<hook_t*> aryClientHook = {};

void AddHook(hook_t* h) {
	aryClientHook.push_back(h);
}
void AddEngineHook(hook_t* h) {
	aryEngineHook.push_back(h);
}
void InstallEngineHook() {
	Fill_InlineEfxHook(R_BloodSprite);

	Fill_EngFunc(pfnPlaybackEvent);
	Install_InlineEngHook(pfnPlaybackEvent);

	Install_InlineEngHook(R_NewMap);
	Install_InlineEngHook(R_ForceCVars);
	Install_InlineEngHook(CL_IsDevOverview);
	Install_InlineEngHook(CL_SetDevOverView);
	Install_InlineEngHook(Cvar_DirectSet);
	Install_InlineEngHook(CL_GetModelByIndex);
	Install_InlineEngHook(NET_StringToAdr);
}
void InstallClientHook(){
	Install_InlineHook(EVVectorScale);
	Install_InlineHook(R_CrossHair_ReDraw);
	Install_InlineHook(TFV_ShowScoreBoard);
	Install_InlineHook(TFV_ShowVGUIMenu);
}
void UninstallEngineHook() {
	for (hook_t* h : aryEngineHook) {
		Uninstall_Hook(h);
	}
	aryEngineHook.clear();
}
void UninstallClientHook() {
	for (hook_t* h : aryClientHook) {
		Uninstall_Hook(h);
	}
	aryClientHook.clear();
}

void CheckAsset() {
	auto c = gEngfuncs.COM_LoadFile((char*)"abcenchance/ABCEnchance.res", 5, 0);
	if(!c)
		g_pMetaHookAPI->SysError("[ABCEnchance]:\nMissing resource files!\nPlease make sure the \"abcenchance/\" folder is placed correctly!");
	gEngfuncs.COM_FreeFile(c);
}
void GL_Init(void){
	g_pMetaHookAPI->GetVideoMode(&gScreenInfo.iWidth, &gScreenInfo.iHeight, nullptr, nullptr);
	auto err = glewInit();
	if (GLEW_OK != err){
		g_pMetaHookAPI->SysError("glewInit failed, %s", glewGetErrorString(err));
		return;
	}
	GL_ShaderInit();
	g_pViewPort->Init();
	gCustomHud.GL_Init();
}
void HUD_Init(void){
	//VGUI init
	gCVars.pBloodEfx = CREATE_CVAR("abc_bloodefx", "1", FCVAR_VALUE, nullptr);
	gCVars.pBloodSpriteSpeed = CREATE_CVAR("abc_bloodsprite_speed", "128", FCVAR_VALUE, nullptr);
	gCVars.pBloodSpriteNumber = CREATE_CVAR("abc_bloodsprite_num", "32", FCVAR_VALUE, nullptr);
	gCVars.pGaussEfx = CREATE_CVAR("abc_gaussefx", "1", FCVAR_VALUE, nullptr);

	gCVars.pModelLag = CREATE_CVAR("cl_modellag", "1", FCVAR_VALUE, nullptr);
	gCVars.pModelLagAutoStop = CREATE_CVAR("cl_modellag_autostop", "1", FCVAR_VALUE, nullptr);
	gCVars.pModelLagValue = CREATE_CVAR("cl_modellag_value", "1.0", FCVAR_VALUE, nullptr);

	gCVars.pModelSlide = CREATE_CVAR("cl_modelslide", "0", FCVAR_VALUE, nullptr);
	gCVars.pModelSlideAngle = CREATE_CVAR("cl_modelslide_angle", "24", FCVAR_VALUE, nullptr);
	gCVars.pModelSlideLength = CREATE_CVAR("cl_modelslide_length", "12", FCVAR_VALUE, nullptr);
	gCVars.pModelSlideHeight = CREATE_CVAR("cl_modelslide_height", "12", FCVAR_VALUE, nullptr);

	gCVars.pCamIdealHeight = CREATE_CVAR("cam_idealheight", "0", FCVAR_VALUE, nullptr);
	gCVars.pCamIdealRight = CREATE_CVAR("cam_idealright", "0", FCVAR_VALUE, nullptr);

	gCVars.pCVarAutoBunnyJump = CREATE_CVAR("cl_autojump", "0", FCVAR_VALUE, nullptr);

	CREATE_CVAR("models", "", 0, [](cvar_t* cvar) {
		static auto toLower = [](std::string& sz) {
			std::transform(sz.begin(), sz.end(), sz.begin(),
				[](unsigned char c) { return std::tolower(c); });
		};
		static auto searchAndprint = [&](const char* subpath) {
			std::string szPath = gEngfuncs.pfnGetGameDirectory();
			szPath += subpath;
			if (std::filesystem::exists(szPath)) {
				std::filesystem::directory_iterator iter(szPath);
				for (const auto& entry : iter) {
					if (std::filesystem::is_directory(entry)) {
						std::string szName = entry.path().filename().string();
						toLower(szName);
						if (szName.find(cvar->string) != std::string::npos)
							gEngfuncs.Con_Printf("%s\n", szName.c_str());
					}
				}
			}
		};
		gEngfuncs.Con_Printf("Models\n==============\n");
		searchAndprint("/models/player");
		searchAndprint("_addon/models/player");
		searchAndprint("_downloads/models/player");
		searchAndprint("_hd/models/player");
		gEngfuncs.Con_Printf("==============\n");
		cvar->string[0] = '\0';
		cvar->value = 0;
	});

	gExportfuncs.HUD_Init();
	gCustomHud.HUD_Init();
	GetClientVoiceMgr()->Init();
	if (g_pParticleMan)
		g_pParticleMan->ResetParticles();
}
int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio){
	memcpy(&gEngineStudio, pstudio, sizeof(gEngineStudio));
	return gExportfuncs.HUD_GetStudioModelInterface(version, ppinterface, pstudio);
}
int HUD_VidInit(void){
	//Search and destory vanillia HUDs
	if (g_dwHUDListAddr && gCVars.pDynamicHUD->value > 0) {
		HUDLIST* pHudList = (HUDLIST*)(*(DWORD*)(g_dwHUDListAddr + 0x0));
		while (pHudList) {
			if (dynamic_cast<CHudBattery*>(pHudList->p) != nullptr)
				gHookHud.m_Battery = (dynamic_cast<CHudBattery*>(pHudList->p));
			else if (dynamic_cast<CHudHealth*>(pHudList->p) != nullptr)
				gHookHud.m_Health = (dynamic_cast<CHudHealth*>(pHudList->p));
			else if (dynamic_cast<CHudAmmo*>(pHudList->p) != nullptr)
				gHookHud.m_Ammo = (dynamic_cast<CHudAmmo*>(pHudList->p));
			pHudList = pHudList->pNext;
		}
	}
	if (g_pViewPort)
		g_pViewPort->VidInit();
	//Fillup Default CVars
	gCVars.pCvarDefaultFOV = CVAR_GET_POINTER("default_fov");

	int result = gExportfuncs.HUD_VidInit();
	gCustomHud.HUD_VidInit();
	return result;
}
void HUD_VoiceStatus(int entindex, qboolean talking) {
	GetClientVoiceMgr()->UpdateSpeakerStatus(entindex, talking);
	gExportfuncs.HUD_VoiceStatus(entindex, talking);
}
void HUD_Frame(double frametime) {
	GetClientVoiceMgr()->Frame(frametime);
	gExportfuncs.HUD_Frame(frametime);
}
int HUD_Redraw(float time, int intermission){
	gCustomHud.HUD_Draw(time);
	g_pViewPort->SetInterMission(intermission);
	return gExportfuncs.HUD_Redraw(time, intermission);
}
void HUD_Reset(void){
	gExportfuncs.HUD_Reset();
}
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client){
	gClientData = client;
	gExportfuncs.HUD_TxferLocalOverrides(state, client);
}
int HUD_UpdateClientData (struct client_data_s* c, float f){
	m_hfov = c->fov;
	gCustomHud.HUD_UpdateClientData(c,f);
	return gExportfuncs.HUD_UpdateClientData(c, f);
}
void HUD_ClientMove(struct playermove_s* ppmove, qboolean server){
	g_playerppmove.inwater = ppmove->waterlevel > 1;
	g_playerppmove.onground = ppmove->onground != -1;
	g_playerppmove.walking = ppmove->movetype = MOVETYPE_WALK;
	gCustomHud.HUD_ClientMove(ppmove, server);
	return gExportfuncs.HUD_PlayerMove(ppmove, server);
}
void HUD_TxferPredictionData (struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd) {
	gCustomHud.HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	gExportfuncs.HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
}
void V_CalcRefdef(struct ref_params_s* pparams){
	
	//pparams->nextView will be zeroed by client dll
	gExportfuncs.V_CalcRefdef(pparams);

	if (gCVars.pRadar->value)
	{
		if (!gCustomHud.m_bRenderRadarView)
		{
			//Tell engine to render twice
			pparams->nextView = 1;

			gCustomHud.m_bRenderRadarView = true;

			//设置到玩家脑袋上朝下看
			gCustomHud.m_flOverViewScale = gCVars.pRadarZoom->value;
			cl_entity_t* local = gEngfuncs.GetLocalPlayer();
			gCustomHud.m_vecOverViewOrg[0] = local->curstate.origin[0];
			gCustomHud.m_vecOverViewOrg[1] = local->curstate.origin[1];
			gCustomHud.m_flOverViewYaw = local->curstate.angles[Q_YAW];

			gCustomHud.m_iIsOverView = 1;

			gCustomHud.m_flSavedCvars[0] = gCVars.pCVarDevOverview->value;
			gCustomHud.m_flSavedCvars[1] = gCVars.pCVarDrawEntities->value;
			gCustomHud.m_flSavedCvars[2] = gCVars.pCVarDrawViewModel->value;
			gCustomHud.m_flSavedCvars[3] = gCVars.pCVarDrawDynamic->value;

			if (gCVars.pCVarFXAA)
				gCustomHud.m_flSavedCvars[4] = gCVars.pCVarFXAA->value;
			if (gCVars.pCVarWater)
				gCustomHud.m_flSavedCvars[5] = gCVars.pCVarWater->value;
			if (gCVars.pCVarShadow)
				gCustomHud.m_flSavedCvars[6] = gCVars.pCVarShadow->value;

			gCVars.pCVarDevOverview->value = 2;
			gCVars.pCVarDrawEntities->value = 0;
			gCVars.pCVarDrawViewModel->value = 0;
			gCVars.pCVarDrawDynamic->value = 0;
			if (gCVars.pCVarFXAA)
				gCVars.pCVarFXAA->value = 0;
			if (gCVars.pCVarWater)
				gCVars.pCVarWater->value = 0;
			if (gCVars.pCVarShadow)
				gCVars.pCVarShadow->value = 0;
		}
		else
		{
			//The first render pass is done
			//Now blit the radar overview from final buffer into radar texture

			gCustomHud.HUD_BlitRadarFramebuffer();

			gCustomHud.m_bRenderRadarView = false;

			gCustomHud.m_iIsOverView = 0;

			gCVars.pCVarDevOverview->value = gCustomHud.m_flSavedCvars[0];
			gCVars.pCVarDrawEntities->value = gCustomHud.m_flSavedCvars[1];
			gCVars.pCVarDrawViewModel->value = gCustomHud.m_flSavedCvars[2];
			gCVars.pCVarDrawDynamic->value = gCustomHud.m_flSavedCvars[3];
			if (gCVars.pCVarFXAA)
				gCVars.pCVarFXAA->value = gCustomHud.m_flSavedCvars[4];
			if (gCVars.pCVarWater)
				gCVars.pCVarWater->value = gCustomHud.m_flSavedCvars[5];
			if (gCVars.pCVarShadow)
				gCVars.pCVarShadow->value = gCustomHud.m_flSavedCvars[6];
		}
	}

	if (!gCustomHud.m_bRenderRadarView)
	{
		if (!gExportfuncs.CL_IsThirdPerson())
		{
			// fudge position around to keep amount of weapon visible
			// roughly equal with different FOV
			cl_entity_t* view = gEngfuncs.GetViewModel();
			if (pparams->viewsize == 110)
			{
				view->origin[2] += 1;
			}
			else if (pparams->viewsize == 100)
			{
				view->origin[2] += 2;
			}
			else if (pparams->viewsize == 90)
			{
				view->origin[2] += 1;
			}
			else if (pparams->viewsize == 80)
			{
				view->origin[2] += 0.5;
			}
			CVector viewOrigin = view->origin;
			CVector viewAngles = view->angles;
			V_CalcViewModelLag(pparams, viewOrigin, viewAngles, pparams->cl_viewangles);
			mathlib::VectorCopy(viewOrigin, view->origin);
			mathlib::VectorCopy(viewAngles, view->angles);
			V_CalcModelSlide(pparams);
		}
		else
		{
			vec3_t vecRight;
			mathlib::AngleVectors(pparams->cl_viewangles, nullptr, vecRight, nullptr);
			mathlib::VectorMultipiler(vecRight, gCVars.pCamIdealRight->value);
			pparams->vieworg[0] += vecRight[0];
			pparams->vieworg[1] += vecRight[1];
			pparams->vieworg[2] += gCVars.pCamIdealHeight->value + vecRight[2];
		}
	}
}

void HUD_DrawTransparentTriangles(void){
	gExportfuncs.HUD_DrawTransparentTriangles();
}
void IN_MouseEvent(int mstate){
	gCustomHud.IN_MouseEvent(mstate);
	gExportfuncs.IN_MouseEvent(mstate);
}
void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active) {
	if (gCustomHud.IsMouseVisible()) {
		gExportfuncs.CL_CreateMove(frametime, cmd, active);
		gCustomHud.CL_CreateMove(frametime, cmd, active);
	}
	else
		gExportfuncs.CL_CreateMove(frametime, cmd, active);
	if (gCustomHud.IsInScore())
		cmd->buttons |= IN_SCORE;
	//Auto jump from openag
	static bool s_jump_was_down_last_frame = false;
	if (gCVars.pCVarAutoBunnyJump->value != 0.0f) {
		bool should_release_jump = (!g_playerppmove.onground && !g_playerppmove.inwater && g_playerppmove.walking);
		if (s_jump_was_down_last_frame && g_playerppmove.onground && !g_playerppmove.inwater && g_playerppmove.walking)
			should_release_jump = true;
		if (should_release_jump)
			cmd->buttons &= ~IN_JUMP;
	}
	s_jump_was_down_last_frame = ((cmd->buttons & IN_JUMP) != 0);
}
void IN_Accumulate(void){
	if (gCustomHud.IsMouseVisible()){
		gExportfuncs.IN_Accumulate();
		gCustomHud.IN_Accumulate();
	}
	else
		gExportfuncs.IN_Accumulate();
}
int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname) {
	if (ent->player)
		GetPlayerInfo(ent->index);
	if (!gCustomHud.HUD_AddEntity(type, ent, modelname))
		return 0;
	return gExportfuncs.HUD_AddEntity(type, ent, modelname);
}
int HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding){
	return gCustomHud.HUD_KeyEvent(eventcode, keynum, pszCurrentBinding) ? 
		gExportfuncs.HUD_Key_Event(eventcode, keynum, pszCurrentBinding) : 0;
}
void HUD_Clear(void){
	gCustomHud.HUD_Clear();
	GL_FreeShaders();
	ClearExtraPrecache();
}