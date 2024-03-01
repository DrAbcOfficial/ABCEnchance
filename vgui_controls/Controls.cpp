#include "plugins.h"
#include "Controls.h"
#include <IEngineSurface.h>
#include <locale.h>
#include <VGUI/IEngineVGui.h>

vgui::ISystemABC* g_pVGuiSystemABC;
vgui::IVGui *g_pVGui;
vgui::IPanel *g_pVGuiPanel;
vgui::ILocalize *g_pVGuiLocalize;

IFileSystem *g_pFullFileSystem;
IKeyValuesSystem *g_pKeyValuesSystem;
vgui::IEngineVGui *g_pEngineVGui;

IEngineSurface *staticSurface;

namespace vgui
{
static char g_szControlsModuleName[256];

bool (__fastcall *g_pfnCWin32Input_PostKeyMessage)(void *pthis, int, KeyValues *message);

bool VGui_InitInterfacesList(const char *moduleName, CreateInterfaceFn *factoryList, int numFactories){
	strncpy_s(g_szControlsModuleName, moduleName, sizeof(g_szControlsModuleName));

	setlocale(LC_CTYPE, "");
	setlocale(LC_TIME, "");
	setlocale(LC_COLLATE, "");
	setlocale(LC_MONETARY, "");

	g_pVGuiSystemABC = (ISystemABC*)factoryList[1](VGUI_SYSTEM_INTERFACE_VERSION, NULL);
	g_pVGui = (IVGui *)factoryList[1](VGUI_IVGUI_INTERFACE_VERSION, NULL);
	g_pVGuiPanel = (IPanel *)factoryList[1](VGUI_PANEL_INTERFACE_VERSION, NULL);
	g_pVGuiLocalize = (ILocalize *)factoryList[1](VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
	g_pFullFileSystem = (IFileSystem*)factoryList[2](FILESYSTEM_INTERFACE_VERSION, NULL);
	g_pEngineVGui = (IEngineVGui *)factoryList[0](VENGINE_VGUI_VERSION, NULL);

	if (!g_pFullFileSystem || !g_pKeyValuesSystem || !g_pVGuiSystemABC || !g_pVGui || !g_pVGuiPanel || !g_pVGuiLocalize)
	{
		SYS_ERROR("vgui_controls is missing a required interface!\n");
		return false;
	}
	return true;
}

const char *GetControlsModuleName(void)
{
	return g_szControlsModuleName;
}

}