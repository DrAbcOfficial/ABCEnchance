#include <metahook.h>

#include <VGUI/VGUI.h>
#include <IVanilliaPanel.h>
#include <GameUI/GameConsole.h>
#include <GameUI/BasePanel.h>

#include "GameUI.h"

void (__fastcall *g_pfnInitialize)(void* pThis, int, CreateInterfaceFn* factories, int count);
void (__fastcall *g_pfnStart)(void* pThis, int, struct cl_enginefuncs_s* engineFuncs, int interfaceVersion, void* system);
void (__fastcall *g_pfnShutdown)(void* pThis, int);
int  (__fastcall *g_pfnActivateGameUI)(void* pThis, int);
int  (__fastcall *g_pfnActivateDemoUI)(void* pThis, int);
int  (__fastcall *g_pfnHasExclusiveInput)(void* pThis, int);
void (__fastcall *g_pfnRunFrame)(void* pThis, int);
void (__fastcall *g_pfnConnectToServer)(void* pThis, int, const char* game, int IP, int port);
void (__fastcall *g_pfnDisconnectFromServer)(void* pThis, int);
void (__fastcall *g_pfnHideGameUI)(void* pThis, int);
bool (__fastcall *g_pfnIsGameUIActive)(void* pThis, int);
void (__fastcall *g_pfnLoadingStarted)(void* pThis, int, const char* resourceType, const char* resourceName);
void (__fastcall *g_pfnLoadingFinished)(void* pThis, int, const char* resourceType, const char* resourceName);
void (__fastcall *g_pfnStartProgressBar)(void* pThis, int, const char* progressType, int progressSteps);
int  (__fastcall *g_pfnContinueProgressBar)(void* pThis, int, int progressPoint, float progressFraction);
void (__fastcall *g_pfnStopProgressBar)(void* pThis, int, bool bError, const char* failureReason, const char* extendedReason);
int  (__fastcall *g_pfnSetProgressBarStatusText)(void* pThis, int, const char* statusText);
void (__fastcall *g_pfnSetSecondaryProgressBar)(void* pThis, int, float progress);
void (__fastcall *g_pfnSetSecondaryProgressBarText)(void* pThis, int, const char* statusText);
void (__fastcall *g_pfnValidateCDKey)(void* pThis, int, bool force, bool inConnect);
void (__fastcall *g_pfnOnDisconnectFromServer)(void* pThis, int, int maybeport, char* maybeip);
void (__fastcall *g_pfnShowPasswordPromptAndRetry)(void* pThis, int, char* passwd, bool correct);
void (__fastcall *g_pfnOnExitToDesktop)(void* pThis, int);

extern void SetAdvanceOptPanelVisible(bool state);
class CGameUI : public IGameUI{
public:
	virtual void Initialize(CreateInterfaceFn* factories, int count) {
		g_pfnInitialize(this, 0, factories, count);
	}
	virtual void Start(struct cl_enginefuncs_s* engineFuncs, int interfaceVersion, void* system) {
		g_pfnStart(this, 0, engineFuncs, interfaceVersion, system);
		BackGroundInitMusic();
	}
	virtual void Shutdown() {
		g_pfnShutdown(this, 0);
	}
	virtual int ActivateGameUI() {
		return g_pfnActivateGameUI(this, 0);
	}
	virtual int ActivateDemoUI() {
		return g_pfnActivateDemoUI(this, 0);
	}
	virtual int HasExclusiveInput() {
		return g_pfnHasExclusiveInput(this, 0);
	}
	virtual void RunFrame() {
		g_pfnRunFrame(this, 0);
	}
	virtual void ConnectToServer(const char* game, int IP, int port) {
		BackGroundSetDecodeState(false);
		g_pfnConnectToServer(this, 0, game, IP, port);
	}
	virtual void DisconnectFromServer() {
		BackGroundSetDecodeState(true);
		g_pfnDisconnectFromServer(this, 0);
	}
	virtual void HideGameUI() {
		g_pfnHideGameUI(this, 0);
		SetAdvanceOptPanelVisible(false);
	}
	virtual bool IsGameUIActive() {
		return g_pfnIsGameUIActive(this, 0);
	}
	virtual void LoadingStarted(const char* resourceType, const char* resourceName) {
		g_pfnLoadingStarted(this, 0, resourceType, resourceName);
	}
	virtual void LoadingFinished(const char* resourceType, const char* resourceName) {
		g_pfnLoadingFinished(this, 0, resourceType, resourceName);
	}
	virtual void StartProgressBar(const char* progressType, int progressSteps) {
		g_pfnStartProgressBar(this, 0, progressType, progressSteps);
	}
	virtual int ContinueProgressBar(int progressPoint, float progressFraction) {
		return g_pfnContinueProgressBar(this, 0, progressPoint, progressFraction);
	}
	virtual void StopProgressBar(bool bError, const char* failureReason, const char* extendedReason = nullptr) {
		g_pfnStopProgressBar(this, 0, bError, failureReason, extendedReason);
	}
	virtual int SetProgressBarStatusText(const char* statusText) {
		return g_pfnSetProgressBarStatusText(this, 0, statusText);
	}
	virtual void SetSecondaryProgressBar(float progress) {
		g_pfnSetSecondaryProgressBar(this, 0, progress);
	}
	virtual void SetSecondaryProgressBarText(const char* statusText) {
		g_pfnSetSecondaryProgressBarText(this, 0, statusText);
	}
	virtual void ValidateCDKey(bool force, bool inConnect) {
		g_pfnValidateCDKey(this, 0, force, inConnect);
	}
	virtual void OnDisconnectFromServer(int maybeport, char* maybeip) {
		g_pfnOnDisconnectFromServer(this, 0, maybeport, maybeip);
	}
	virtual void ShowPasswordPromptAndRetry(char* passwd, bool correct) {
		g_pfnShowPasswordPromptAndRetry(this, 0, passwd, correct);
	}
	virtual void OnExitToDesktop() {
		g_pfnOnExitToDesktop(this, 0);
	}

	bool m_bLoadlingLevel;
	char m_szPreviousStatusText[128];
};
static CGameUI s_GameUI;
IGameUI* gameui;

extern void AddHook(hook_t* hook);

void GameUI_InstallHook(){
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (hGameUI) {
		CreateInterfaceFn fnCreateInterface = Sys_GetFactory(hGameUI);
		gameui = static_cast<IGameUI*>(fnCreateInterface(GAMEUI_INTERFACE_VERSION, nullptr));
		DWORD* pVFTable = *(DWORD**)&s_GameUI;
#define GAMEUI_VFTHOOK(index, name) AddHook(g_pMetaHookAPI->VFTHook(gameui, 0, index, (void *)pVFTable[index], (void **)&g_pfn##name))
		//GAMEUI_VFTHOOK(1, Initialize);
		GAMEUI_VFTHOOK(2, Start);
		//GAMEUI_VFTHOOK(3, Shutdown);
		//GAMEUI_VFTHOOK(4, ActivateGameUI);
		//GAMEUI_VFTHOOK(5, ActivateDemoUI);
		//GAMEUI_VFTHOOK(6, HasExclusiveInput);
		//GAMEUI_VFTHOOK(7, RunFrame);
		GAMEUI_VFTHOOK(8, ConnectToServer);
		GAMEUI_VFTHOOK(9, DisconnectFromServer);
		GAMEUI_VFTHOOK(10, HideGameUI);
		//GAMEUI_VFTHOOK(11, IsGameUIActive);
		//GAMEUI_VFTHOOK(12, LoadingStarted);
		//GAMEUI_VFTHOOK(13, LoadingFinished);
		//GAMEUI_VFTHOOK(14, StartProgressBar);
		//GAMEUI_VFTHOOK(15, ContinueProgressBar);
		//GAMEUI_VFTHOOK(16, StopProgressBar);
		//GAMEUI_VFTHOOK(17, SetProgressBarStatusText);
		//GAMEUI_VFTHOOK(18, SetSecondaryProgressBar);
		//GAMEUI_VFTHOOK(19, SetSecondaryProgressBarText);
		//GAMEUI_VFTHOOK(20, ValidateCDKey);
		//GAMEUI_VFTHOOK(21, OnDisconnectFromServer);
		//GAMEUI_VFTHOOK(22, ShowPasswordPromptAndRetry);
		//GAMEUI_VFTHOOK(23, OnExitToDesktop);
	}
#undef GAMEUI_VFTHOOK
}