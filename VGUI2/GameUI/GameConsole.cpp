#include <metahook.h>
#include "plugins.h"

#include "GameConsole.h"
#include <IGameConsole.h>

#include <vgui_controls/consoledialog.h>

vgui::CConsoleDialog* g_ConsoleDialog;

void(__fastcall* m_pfnCGameConsole_Activate)(void* pthis, int);
void(__fastcall* m_pfnCGameConsole_Initialize)(void* pthis, int);
void(__fastcall* m_pfnCGameConsole_Hide)(void* pthis, int);
void(__fastcall* m_pfnCGameConsole_Clear)(void* pthis, int);
bool(__fastcall* m_pfnCGameConsole_IsConsoleVisible)(void* pthis, int);
void(__fastcall* m_pfnCGameConsole_Printf)(void* pthis, int, const char* format, ...);
void(__fastcall* m_pfnCGameConsole_DPrintf)(void* pthis, int, const char* format, ...);
void(__fastcall* m_pfnCGameConsole_SetParent)(void* pthis, int, int parent);

class CGameConsole : public IGameConsole{
public:
	virtual void Activate(void) {
		g_ConsoleDialog->Activate();
	}
	virtual void Initialize(void) {
		g_ConsoleDialog = new vgui::CConsoleDialog(nullptr, "Console", false);
		g_ConsoleDialog->ColorPrint(Color(0, 255, 0, 200), "[ABCEnchance] ABCEnchance has run successfully!\n");
	}
	virtual void Hide(void) {
		g_ConsoleDialog->Close();
	}
	virtual void Clear(void) {
		g_ConsoleDialog->Clear();
	}
	virtual bool IsConsoleVisible(void) {
		return g_ConsoleDialog->IsVisible();
	}
	virtual void Printf(const char* format, ...) {
		va_list args;
		va_start(args, format);
		static char buf[1024];
		Q_vsnprintf(buf, sizeof(buf), format, args);
		g_ConsoleDialog->Print(buf);
		va_end(args);
	}
	virtual void DPrintf(const char* format, ...) {
		va_list args;
		va_start(args, format);
		static char buf[1024];
		Q_vsnprintf(buf, sizeof(buf), format, args);
		g_ConsoleDialog->DPrint(buf);
		va_end(args);
	}
	//virtual void ColorPrintf(Color &clr) = 0;	// Doesn't exist on GS
	virtual void SetParent(int parent) {
		g_ConsoleDialog->SetParent(parent);
	}
};

extern void AddHook(hook_t* hook);
static CGameConsole s_GameConsole;
IGameConsole* gameconsolefuncs;
void GameConsole_InstallHook(void){
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (!hGameUI) {
		SYS_ERROR("Failed to locate GameUI.dll");
		return;
	}
	CreateInterfaceFn fnCreateInterface = Sys_GetFactory(hGameUI);
	if (!fnCreateInterface)
	{
		SYS_ERROR("Failed to get factory from GameUI.dll");
		return;
	}
	gameconsolefuncs = (IGameConsole*)fnCreateInterface(GAMECONSOLE_INTERFACE_VERSION_GS, NULL);
	if (!gameconsolefuncs)
	{
		SYS_ERROR("Failed to get interface " GAMECONSOLE_INTERFACE_VERSION_GS " from GameUI.dll");
		return;
	}
	DWORD* pVFTable = *(DWORD**)&s_GameConsole;
	AddHook(g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 1, (void*)pVFTable[1], (void**)&m_pfnCGameConsole_Activate));
	AddHook(g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 2, (void*)pVFTable[2], (void**)&m_pfnCGameConsole_Initialize));
	AddHook(g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 3, (void*)pVFTable[3], (void**)&m_pfnCGameConsole_Hide));
	AddHook(g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 4, (void*)pVFTable[4], (void**)&m_pfnCGameConsole_Clear));
	AddHook(g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 5, (void*)pVFTable[5], (void**)&m_pfnCGameConsole_IsConsoleVisible));
	AddHook(g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 6, (void*)pVFTable[6], (void**)&m_pfnCGameConsole_Printf));
	AddHook(g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 7, (void*)pVFTable[7], (void**)&m_pfnCGameConsole_DPrintf));
	AddHook(g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 8, (void*)pVFTable[8], (void**)&m_pfnCGameConsole_SetParent));
}

IGameConsole* GameConsole() {
	return &s_GameConsole;
}