#include <metahook.h>
#include <IScheme2.h>
#include <ISurface2.h>
#include "plugins.h"
#include "VGUI2ExtensionImport.h"
#include <IPanel.h>
#include <IInput2.h>

IVGUI2Extension* g_pVGUI2Extension = nullptr;
IDpiManager* g_pDpiManager = nullptr;
vgui::ISchemeManager2* g_pVGuiSchemeManager = nullptr;
vgui::ISurface2* g_pVGuiSurface = nullptr;
vgui::IPanel2* g_pVGuiPanel2 = nullptr;
vgui::IInput2* g_pVGuiInput2 = nullptr;

void VGUI2Extension_Init()
{
	auto hVGUI2Extension = GetModuleHandleA("VGUI2Extension.dll");

	if (!hVGUI2Extension){
		SYS_ERROR("VGUI2Extension.dll is required!");
		return;
	}

	auto factory = Sys_GetFactory((HINTERFACEMODULE)hVGUI2Extension);

	if (!factory){
		SYS_ERROR("Could not get factory from VGUI2Extension.dll");
		return;
	}

#define LOAD_FROM_EXTENSION(dst, itf) dst = (decltype(dst))factory(itf, nullptr);if(!dst){SYS_ERROR("Could not get interface \"" itf "\" from VGUI2Extension.dll");return;}
	LOAD_FROM_EXTENSION(g_pVGUI2Extension, VGUI2_EXTENSION_INTERFACE_VERSION);
	LOAD_FROM_EXTENSION(g_pDpiManager, DPI_MANAGER_INTERFACE_VERSION);
	LOAD_FROM_EXTENSION(g_pVGuiSurface, VGUI_SURFACE2_INTERFACE_VERSION);
	LOAD_FROM_EXTENSION(g_pVGuiSchemeManager, VGUI_SCHEME2_INTERFACE_VERSION);
	LOAD_FROM_EXTENSION(g_pVGuiInput2, VGUI_INPUT2_INTERFACE_VERSION);
#undef LOAD_FROM_EXTENSION
}

void VGUI2Extension_Shutdown()
{

}

IVGUI2Extension* VGUI2Extension()
{
	return g_pVGUI2Extension;
}

IDpiManager* DpiManager()
{
	return g_pDpiManager;
}