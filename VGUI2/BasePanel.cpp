#include <metahook.h>

#include "local.h"

#include "BasePanel.h"

#include "exportfuncs.h"
#include <plugins.h>

void __fastcall CBasePanel_PaintBackground(void* pthis, int dummy) {

}

void BasePanel_InstallHook(void)
{
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (hGameUI) {
		CreateInterfaceFn fnCreateInterface = Sys_GetFactory(hGameUI);
		if (fnCreateInterface) {
#define SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG "\x55\x8B\xEC\x83\xEC\x38\x53\x8D\x45\xCC\x8B\xD9\x50\x8D\x45\xC8\x89\x5D\xD0\x50\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8D\x4D\xD4\x51"
			Fill_Sig(SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG, hGameUI, g_pMetaHookAPI->GetModuleSize(hGameUI), CBasePanel_PaintBackground);
			Install_InlineHook(CBasePanel_PaintBackground);
		}
	}
	else
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not find game ui for dynamic background.");
}