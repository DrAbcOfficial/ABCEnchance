#include <metahook.h>

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
	virtual void Activate(void);
	virtual void Initialize(void);
	virtual void Hide(void);
	virtual void Clear(void);
	virtual bool IsConsoleVisible(void);
	virtual void Printf(const char* format, ...);
	virtual void DPrintf(const char* format, ...);
	//virtual void ColorPrintf(Color &clr) = 0;	// Doesn't exist on GS
	virtual void SetParent(int parent);
};

void CGameConsole::Activate(void){
	g_ConsoleDialog->Activate();
}
void CGameConsole::Initialize(void){
	g_ConsoleDialog = new vgui::CConsoleDialog(nullptr, "Console", false);
	g_ConsoleDialog->ColorPrint(Color(0, 255, 0, 200), "[ABCEnchance] ABCEnchance has run successfully!\n");
}
void CGameConsole::Hide(void){
	g_ConsoleDialog->Hide();
}
void CGameConsole::Clear(void){
	g_ConsoleDialog->Clear();
}
bool CGameConsole::IsConsoleVisible(void){
	return g_ConsoleDialog->IsVisible();
}
void CGameConsole::Printf(const char* format, ...){
	va_list args;
	va_start(args, format);
	static char buf[1024];
	Q_vsnprintf(buf, sizeof(buf), format, args);
	g_ConsoleDialog->Print(buf);
	va_end(args);
}
void CGameConsole::DPrintf(const char* format, ...){
	va_list args;
	va_start(args, format);
	static char buf[1024];
	Q_vsnprintf(buf, sizeof(buf), format, args);
	g_ConsoleDialog->DPrint(buf);
	va_end(args);
}
void CGameConsole::SetParent(int parent){
	g_ConsoleDialog->SetParent(parent);
}

static CGameConsole s_GameConsole;

IGameConsole* gameconsolefuncs;

void GameConsole_InstallHook(void)
{
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (hGameUI) {
		CreateInterfaceFn fnCreateInterface = Sys_GetFactory(hGameUI);
		gameconsolefuncs = (IGameConsole*)fnCreateInterface(GAMECONSOLE_INTERFACE_VERSION_GS, NULL);
		DWORD* pVFTable = *(DWORD**)&s_GameConsole;
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 1, (void*)pVFTable[1], (void**)&m_pfnCGameConsole_Activate);
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 2, (void*)pVFTable[2], (void**)&m_pfnCGameConsole_Initialize);
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 3, (void*)pVFTable[3], (void**)&m_pfnCGameConsole_Hide);
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 4, (void*)pVFTable[4], (void**)&m_pfnCGameConsole_Clear);
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 5, (void*)pVFTable[5], (void**)&m_pfnCGameConsole_IsConsoleVisible);
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 6, (void*)pVFTable[6], (void**)&m_pfnCGameConsole_Printf);
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 7, (void*)pVFTable[7], (void**)&m_pfnCGameConsole_DPrintf);
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 8, (void*)pVFTable[8], (void**)&m_pfnCGameConsole_SetParent);
	}
	else
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not create interface of gameconsole.");
}