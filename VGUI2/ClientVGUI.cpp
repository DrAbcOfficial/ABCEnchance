#include <vector>
#include <string>

#include <VGUI_controls/Controls.h>
#include <VGUI_controls/Panel.h>
#include <VGUI_controls/Frame.h>

#include <IClientVGUI.h>
#include "plugins.h"
#include "vguilocal.h"
#include "steam_api.h"
#include "player_info.h"
#include "scoreboard.h"
#include "Viewport.h"
#include "exportfuncs.h"

#include <vgui.h>
#include "ClientVGUI.h"

namespace vgui{
	bool VGui_InitInterfacesList(const char* moduleName, CreateInterfaceFn* factoryList, int numFactories);
}

extern void SysError(const char* message ...);
void ClientVGUIInit(CreateInterfaceFn* factories, int count) {
	vgui::VGui_InitInterfacesList("ABCEnchance", factories, count);
	vgui::HScheme iScheme = vgui::scheme()->LoadSchemeFromFile("abcenchance/ABCEnchance.res", "ABCEnchance");
	int iPluginVersion = 0;
	if (iScheme > 0) {
		pSchemeData = vgui::scheme()->GetIScheme(vgui::scheme()->GetScheme("ABCEnchance"));
		iPluginVersion = atoi(pSchemeData->GetResourceString("Version"));
	}
	else {
		SysError("Ooops! Can not load resource file!\nHave you installed it correctly?\n");
		return;
	}
	if (iPluginVersion < PLUGIN_VERSION)
		SysError("Mismatched Resource file: abcenchance/ABCEnchance.res\nRequire Version: %d\nYour Version: %d\n",
			PLUGIN_VERSION, iPluginVersion);
	char localizePath[260];
	snprintf(localizePath, sizeof(localizePath), "abcenchance/localize/%s.txt",
		(!strlen(pSchemeData->GetResourceString("Language"))) ? "%language%" : pSchemeData->GetResourceString("Language"));
	if (!vgui::localize()->AddFile(g_pFileSystem, localizePath))
		SysError("Missing Localize file: %s\n", localizePath);
}
void ClientVGUIShutdown(void) {
	if (g_pViewPort) {
		delete g_pViewPort;
		g_pViewPort = nullptr;
	}
}

static void(__fastcall* m_pfnCClientVGUI_Initialize)(void* pthis, int, CreateInterfaceFn* factories, int count) = NULL;
static void(__fastcall* m_pfnCClientVGUI_Start)(void* pthis, int) = NULL;
static void(__fastcall* m_pfnCClientVGUI_SetParent)(void* pthis, int, vgui::VPANEL parent) = NULL;
static bool(__fastcall* m_pfnCClientVGUI_UseVGUI1)(void* pthis, int) = NULL;
static void(__fastcall* m_pfnCClientVGUI_HideScoreBoard)(void* pthis, int) = NULL;
static void(__fastcall* m_pfnCClientVGUI_HideAllVGUIMenu)(void* pthis, int) = NULL;
static void(__fastcall* m_pfnCClientVGUI_ActivateClientUI)(void* pthis, int) = NULL;
static void(__fastcall* m_pfnCClientVGUI_HideClientUI)(void* pthis, int) = NULL;

class CClientVGUI : public IClientVGUI{
public:
	virtual void Initialize(CreateInterfaceFn* factories, int count) {
		m_pfnCClientVGUI_Initialize(this, 0, factories, count);
		ClientVGUIInit(factories, count);
	}

	virtual void Start(void) {
		m_pfnCClientVGUI_Start(this, 0);

		g_pViewPort = new CViewport();
		g_pViewPort->Start();
	}
	virtual void SetParent(vgui::VPANEL parent) {
		m_pfnCClientVGUI_SetParent(this, 0, parent);

		g_pViewPort->SetParent(parent);
	}
	virtual bool UseVGUI1(void) {
		return m_pfnCClientVGUI_UseVGUI1(this, 0);
	}
	virtual void HideScoreBoard(void) {
		m_pfnCClientVGUI_HideScoreBoard(this, 0);
	}
	virtual void HideAllVGUIMenu(void) {
		m_pfnCClientVGUI_HideAllVGUIMenu(this, 0);
	}
	virtual void ActivateClientUI(void) {
		m_pfnCClientVGUI_ActivateClientUI(this, 0);

		g_pViewPort->ActivateClientUI();
	}
	virtual void HideClientUI(void) {
		m_pfnCClientVGUI_HideClientUI(this, 0);

		g_pViewPort->HideClientUI();
	}
	virtual void unknown(void) {

	}
	virtual void Shutdown(void) {

	}
};
IClientVGUI* g_pClientVGUI = nullptr;
static CClientVGUI s_ClientVGUI;
extern void AddHook(hook_t* hook);

vgui::ISchemeManager2* g_pVGuiSchemeManager2;
vgui::ISurface2* g_pVGuiSurface;

void ClientVGUIInstallHook(void){
	CreateInterfaceFn ClientVGUICreateInterface = (CreateInterfaceFn)Sys_GetFactory((HINTERFACEMODULE)g_metaplugins.captionmod.info.PluginModuleBase);
	if (ClientVGUICreateInterface) {
		g_pClientVGUI = (IClientVGUI*)ClientVGUICreateInterface(CLIENTVGUI_INTERFACE_VERSION, NULL);
		if (g_pClientVGUI) {
			DWORD* pVFTable = *(DWORD**)&s_ClientVGUI;
			AddHook(g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 1, (void*)pVFTable[1], (void**)&m_pfnCClientVGUI_Initialize));
			AddHook(g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 2, (void*)pVFTable[2], (void**)&m_pfnCClientVGUI_Start));
			AddHook(g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 3, (void*)pVFTable[3], (void**)&m_pfnCClientVGUI_SetParent));
			AddHook(g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 7, (void*)pVFTable[7], (void**)&m_pfnCClientVGUI_ActivateClientUI));
			AddHook(g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 8, (void*)pVFTable[8], (void**)&m_pfnCClientVGUI_HideClientUI));
			g_IsClientVGUI2 = true;
		}
		g_pVGuiSurface = (vgui::ISurface2*)ClientVGUICreateInterface(VGUI_SURFACE2_INTERFACE_VERSION, NULL);
		g_pVGuiSchemeManager2 = (vgui::ISchemeManager2*)ClientVGUICreateInterface(VGUI_SCHEME2_INTERFACE_VERSION, NULL);
		if (!g_pVGuiSurface || !g_pVGuiSchemeManager2)
			SysError("Your CaptionMod version are outdate, please update it and run again.");
	}
}

//vgui2

bool g_bIMEComposing = false;
double g_flImeComposingTime = 0;

//fuck me
bool IsOSX(){
	return false;
}

double GetAbsoluteTime() {
	return gEngfuncs.GetAbsoluteTime();
}

ICommandLine* CommandLine(void) {
	return g_pInterface->CommandLine;
}
