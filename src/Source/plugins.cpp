#include <metahook.h>
#include "exportfuncs.h"
#include "ClientParticleMan.h"
#include "curl.h"
#include "soundengine.h"
#include <vgui_controls/Controls.h>
#include <svc_hook.h>

cl_enginefunc_t gEnginefuncs;
mh_interface_t *g_pInterface;
metahook_api_t *g_pMetaHookAPI;
mh_enginesave_t *g_pMetaSave;
IFileSystem *g_pFileSystem;

HMODULE g_hClientDll;
HINSTANCE g_hThisModule;
DWORD g_dwEngineBuildnum;
HINSTANCE g_hEngineModule;
PVOID g_dwClientBase;
DWORD g_dwClientSize;
PVOID g_dwEngineBase;
DWORD g_dwEngineSize;
PVOID g_dwEngineTextBase;
DWORD g_dwEngineTextSize;
PVOID g_dwEngineDataBase;
DWORD g_dwEngineDataSize;
PVOID g_dwEngineRdataBase;
DWORD g_dwEngineRdataSize;
int g_iEngineType;
bool g_IsClientVGUI2 = false;

void IPluginsV4::Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave){
	g_pInterface = pInterface;
	g_pMetaHookAPI = pAPI;
	g_pMetaSave = pSave;
}

void IPluginsV4::LoadEngine(cl_enginefunc_t *pEngfuncs){
	g_pFileSystem = g_pInterface->FileSystem;
	g_pFullFileSystem = reinterpret_cast<IFileSystem*>(g_pFileSystem);
	g_iEngineType = g_pMetaHookAPI->GetEngineType();
	g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();
	g_hEngineModule = g_pMetaHookAPI->GetEngineModule();
	g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
	g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();
	g_dwEngineTextBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".text\x0\x0\x0", &g_dwEngineTextSize);
	g_dwEngineDataBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".data\x0\x0\x0", &g_dwEngineDataSize);
	g_dwEngineRdataBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".rdata\x0\x0", &g_dwEngineRdataSize);

	memcpy(&gEngfuncs, pEngfuncs, sizeof(gEngfuncs));
	if (g_iEngineType != ENGINE_SVENGINE)
		g_pMetaHookAPI->SysError("%s can only run in SvenEngine!\nEngine type: %d\nEngine buildnum: %d", 
			"ABCEnchance.dll", g_iEngineType, g_dwEngineBuildnum);

	CheckOtherPlugin();
	FillEngineAddress();
	//SVC_FillAddress();
	InstallEngineHook();
	BaseUI_InstallHook();
}
void IPluginsV4::LoadClient(cl_exportfuncs_t *pExportFunc){
	memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));

	g_hClientDll = g_pMetaHookAPI->GetClientModule();
	g_dwClientBase = g_pMetaHookAPI->GetClientBase();
	g_dwClientSize = g_pMetaHookAPI->GetClientSize();

	CheckAsset();

	GL_Init();

	pExportFunc->HUD_Init = HUD_Init;
	pExportFunc->HUD_GetStudioModelInterface = HUD_GetStudioModelInterface;
	pExportFunc->HUD_VidInit = HUD_VidInit;
	pExportFunc->HUD_Redraw = HUD_Redraw;
	pExportFunc->IN_MouseEvent = IN_MouseEvent;
	pExportFunc->HUD_Key_Event = HUD_KeyEvent;
	pExportFunc->IN_Accumulate = IN_Accumulate;
	pExportFunc->CL_CreateMove = CL_CreateMove;
	pExportFunc->HUD_UpdateClientData = HUD_UpdateClientData;
	//pExportFunc->HUD_Reset = HUD_Reset;
	pExportFunc->HUD_TxferLocalOverrides = HUD_TxferLocalOverrides;
	pExportFunc->HUD_PlayerMove = HUD_ClientMove;
	pExportFunc->HUD_AddEntity = HUD_AddEntity;
	pExportFunc->V_CalcRefdef = V_CalcRefdef;
	pExportFunc->HUD_VoiceStatus = HUD_VoiceStatus;
	pExportFunc->HUD_Frame = HUD_Frame;
	pExportFunc->HUD_TxferPredictionData = HUD_TxferPredictionData;

	FillAddress();
	InstallClientHook();
	ClientVGUIInstallHook();
	InitCreateParticleMan();
	LoadLibcurl();
	FModEngine::InitFModLibrary();

	EnumWindows([](HWND hwnd, LPARAM lParam
		)
		{
			DWORD pid = 0;
			if (GetWindowThreadProcessId(hwnd, &pid) && pid == GetCurrentProcessId())
			{
				char windowClass[256] = { 0 };
				RealGetWindowClassA(hwnd, windowClass, sizeof(windowClass));
				if (!strcmp(windowClass, "Valve001") || !strcmp(windowClass, "SDL_app"))
				{
					g_MainWnd = hwnd;
					return FALSE;
				}
			}
			return TRUE;
		}, NULL);

	g_MainWndProc = (WNDPROC)GetWindowLong(g_MainWnd, GWL_WNDPROC);
	SetWindowLong(g_MainWnd, GWL_WNDPROC, (LONG)VID_MainWndProc);
}
void IPluginsV4::Shutdown(void){
	HUD_Clear();
}
void IPluginsV4::ExitGame(int iResult){
	UninstallClientHook();
	UninstallEngineHook();
	FreeParticleMan();
	CloseLibcurl();
	FModEngine::FreeFModLibrary();
}

#define STR1(R) #R
#define STR2(R) STR1(R)

const char* IPluginsV4::GetVersion(void){
	return "Dr.Abc|Version:" STR2(PLUGIN_VERSION);
}
EXPOSE_SINGLE_INTERFACE(IPluginsV4, IPluginsV4, METAHOOK_PLUGIN_API_VERSION_V4);