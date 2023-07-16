#include <metahook.h>

#include "GameConsole.h"
#include <IGameConsole.h>
#include <strtools.h>

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
	m_pfnCGameConsole_Activate(this, 0);
}
void CGameConsole::Initialize(void){
	m_pfnCGameConsole_Initialize(this, 0);
}
void CGameConsole::Hide(void){
	m_pfnCGameConsole_Hide(this, 0);
}
void CGameConsole::Clear(void){
	m_pfnCGameConsole_Clear(this, 0);
}
bool CGameConsole::IsConsoleVisible(void){
	return m_pfnCGameConsole_IsConsoleVisible(this, 0);
}
void CGameConsole::Printf(const char* format, ...){
	va_list args;
	va_start(args, format);
	static char buf[1024];
	Q_vsnprintf(buf, sizeof(buf), format, args);
	m_pfnCGameConsole_Printf(this, 0, "%s", buf);
	va_end(args);
}
void CGameConsole::DPrintf(const char* format, ...){
	va_list args;
	va_start(args, format);
	static char buf[1024];
	Q_vsnprintf(buf, sizeof(buf), format, args);
	m_pfnCGameConsole_DPrintf(this, 0, "%s", buf);
	va_end(args);
}
void CGameConsole::SetParent(int parent){
	m_pfnCGameConsole_SetParent(this, 0, parent);
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
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 6, (void*)pVFTable[6], (void**)&m_pfnCGameConsole_Printf);
		g_pMetaHookAPI->VFTHook(gameconsolefuncs, 0, 7, (void*)pVFTable[7], (void**)&m_pfnCGameConsole_DPrintf);
	}
	else
		g_pMetaHookAPI->SysError("[ABCEnchace] Can not create interface of gameconsole.");
}