#include <metahook.h>
#include "exportfuncs.h"
#include "command.h"

cl_enginefunc_t gEnginefuncs;
mh_interface_t *g_pInterface;
metahook_api_t *g_pMetaHookAPI;
mh_enginesave_t *g_pMetaSave;
IFileSystem *g_pFileSystem;

HINSTANCE g_hInstance, g_hThisModule;

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

void IPluginsV3::Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave){
	g_pInterface = pInterface;
	g_pMetaHookAPI = pAPI;
	g_pMetaSave = pSave;
	g_hInstance = GetModuleHandle(nullptr);
}

void IPluginsV3::LoadEngine(cl_enginefunc_t *pEngfuncs){
	g_pFileSystem = g_pInterface->FileSystem;
	g_iEngineType = g_pMetaHookAPI->GetEngineType();
	g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();
	g_hEngineModule = g_pMetaHookAPI->GetEngineModule();
	g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
	g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();
	g_dwEngineTextBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".text\x0\x0\x0", &g_dwEngineTextSize);
	g_dwEngineDataBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".data\x0\x0\x0", &g_dwEngineDataSize);
	g_dwEngineRdataBase = g_pMetaHookAPI->GetSectionByName(g_dwEngineBase, ".rdata\x0\x0", &g_dwEngineRdataSize);

	memcpy(&gEngfuncs, pEngfuncs, sizeof(gEngfuncs));
	Cmd_GetCmdBase = *(cmd_function_t * (**)(void))((DWORD)pEngfuncs + 0x198);

	if (g_iEngineType != ENGINE_SVENGINE)
		Sys_ErrorEx("%s can only run in SvenEngine!\nEngine type: %d\nEngine buildnum: %d", 
			"ABCEnchance.dll", g_iEngineType, g_dwEngineBuildnum);
	CheckOtherPlugin();
}

void IPluginsV3::LoadClient(cl_exportfuncs_t *pExportFunc){
	memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));

	g_dwClientBase = (PVOID)GetModuleHandleA("client.dll");
	g_dwClientSize = g_pMetaHookAPI->GetModuleSize((HMODULE)g_dwClientBase);

	GL_Init();

	pExportFunc->HUD_Init = HUD_Init;
	pExportFunc->HUD_GetStudioModelInterface = HUD_GetStudioModelInterface;
	pExportFunc->HUD_VidInit = HUD_VidInit;
	pExportFunc->HUD_Redraw = HUD_Redraw;
	pExportFunc->IN_MouseEvent = IN_MouseEvent;
	pExportFunc->HUD_Key_Event = HUD_KeyEvent;
	pExportFunc->IN_Accumulate = IN_Accumulate;
	pExportFunc->HUD_UpdateClientData = HUD_UpdateClientData;
	pExportFunc->HUD_Reset = HUD_Reset;
	pExportFunc->HUD_TxferLocalOverrides = HUD_TxferLocalOverrides;
	pExportFunc->HUD_PlayerMove = HUD_ClientMove;
	pExportFunc->HUD_AddEntity = HUD_AddEntity;
	pExportFunc->V_CalcRefdef = V_CalcRefdef;

	FillEfxAddress();
	FillAddress();
	InstallHook();

	MSG_Init();
}

void IPluginsV3::Shutdown(void){
	HUD_Clear();
}

void IPluginsV3::ExitGame(int iResult){
}
EXPOSE_SINGLE_INTERFACE(IPluginsV3, IPluginsV3, METAHOOK_PLUGIN_API_VERSION_V3);