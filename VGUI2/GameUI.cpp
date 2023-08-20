#include <metahook.h>

#include <VGUI/VGUI.h>
#include <IVanilliaPanel.h>
#include <LoadingDialog.h>
#include <CGameLoading.h>
#include <GameConsole.h>
#include <BasePanel.h>

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

class CGameUI : public IGameUI{
public:
	virtual void Initialize(CreateInterfaceFn* factories, int count);
	virtual void Start(struct cl_enginefuncs_s* engineFuncs, int interfaceVersion, void* system);
	virtual void Shutdown();
	virtual int ActivateGameUI();
	virtual int ActivateDemoUI();
	virtual int HasExclusiveInput();
	virtual void RunFrame();
	virtual void ConnectToServer(const char* game, int IP, int port);
	virtual void DisconnectFromServer();
	virtual void HideGameUI();
	virtual bool IsGameUIActive();
	virtual void LoadingStarted(const char* resourceType, const char* resourceName);
	virtual void LoadingFinished(const char* resourceType, const char* resourceName);
	virtual void StartProgressBar(const char* progressType, int progressSteps);
	virtual int ContinueProgressBar(int progressPoint, float progressFraction);
	virtual void StopProgressBar(bool bError, const char* failureReason, const char* extendedReason = nullptr);
	virtual int SetProgressBarStatusText(const char* statusText);
	virtual void SetSecondaryProgressBar(float progress);
	virtual void SetSecondaryProgressBarText(const char* statusText);
	virtual void ValidateCDKey(bool force, bool inConnect);
	virtual void OnDisconnectFromServer(int maybeport, char* maybeip);
	virtual void ShowPasswordPromptAndRetry(char* passwd, bool correct);
	virtual void OnExitToDesktop();

	bool m_bLoadlingLevel;
	char m_szPreviousStatusText[128];
};
static CGameUI s_GameUI;
IGameUI* gameui;

vgui::DHANDLE<CLoadingDialog> g_hLoadingDialog;
vgui::DHANDLE<CGameLoading>g_hGameLoading;

void CGameUI::Initialize(CreateInterfaceFn* factories, int count) {
	g_pfnInitialize(this, 0, factories, count);
}
void CGameUI::Start(struct cl_enginefuncs_s* engineFuncs, int interfaceVersion, void* system) {
	g_pfnStart(this, 0, engineFuncs, interfaceVersion, system);
	BackGroundInitMusic();
}
void CGameUI::Shutdown(void) {
	g_pfnShutdown(this, 0);
}
int  CGameUI::ActivateGameUI(void) {
	return g_pfnActivateGameUI(this, 0);
}
int  CGameUI::ActivateDemoUI(void) {
	return g_pfnActivateDemoUI(this, 0);
}
int  CGameUI::HasExclusiveInput(void) {
	return g_pfnHasExclusiveInput(this, 0);
}
void CGameUI::RunFrame(void) {
	g_pfnRunFrame(this, 0);
}
void CGameUI::ConnectToServer(const char* game, int IP, int port) {
	BackGroundSetDecodeState(false);
	g_pfnConnectToServer(this, 0, game, IP, port);
}
void CGameUI::DisconnectFromServer(void) {
	BackGroundSetDecodeState(true);
	g_pfnDisconnectFromServer(this, 0);
}
void CGameUI::HideGameUI(void) {
	g_pfnHideGameUI(this, 0);
}
bool CGameUI::IsGameUIActive(void) {
	return g_pfnIsGameUIActive(this, 0);
}
void CGameUI::LoadingStarted(const char* resourceType, const char* resourceName) {
	/*m_bLoadlingLevel = true;
	if (!g_hGameLoading.Get())
		g_hGameLoading = new CGameLoading((vgui::Panel*)(BasePanel()), "GameLoading");
	g_hGameLoading->Activate();
	GameConsole()->Hide();*/
	g_pfnLoadingStarted(this, 0, resourceType, resourceName);
}
void CGameUI::LoadingFinished(const char* resourceType, const char* resourceName) {
	/*m_bLoadlingLevel = false;
	if (g_hGameLoading.Get()){
		g_hGameLoading->SetVisible(false);
		g_hGameLoading->SetAutoDelete(true);
	}
	StopProgressBar(false, "");
	HideGameUI();*/
	g_pfnLoadingFinished(this, 0, resourceType, resourceName);
}
void CGameUI::StartProgressBar(const char* progressType, int progressSteps) {
	/*if (g_hGameLoading.Get()){
		if (g_hGameLoading->IsVisible()){
			g_hGameLoading->SetProgressRange(0, progressSteps);
			g_hGameLoading->SetProgressPoint(0);
			if (!stricmp(progressType, "Server"))
				g_hGameLoading->SetProgressVisible(false);
			else if (!stricmp(progressType, "Connecting"))
				g_hGameLoading->SetProgressVisible(true);
			return;
		}
	}
	if (!g_hLoadingDialog.Get())
		g_hLoadingDialog = new CLoadingDialog((vgui::Panel*)(BasePanel()));
	m_szPreviousStatusText[0] = 0;
	g_hLoadingDialog->SetProgressRange(0, progressSteps);
	g_hLoadingDialog->SetProgressPoint(0);
	g_hLoadingDialog->Open();*/
	g_pfnStartProgressBar(this, 0, progressType, progressSteps);
}
int  CGameUI::ContinueProgressBar(int progressPoint, float progressFraction) {
	/*if (g_hGameLoading.Get()){
		if (g_hGameLoading->IsVisible()) {
			return g_hGameLoading->SetProgressPoint(progressPoint);
		}
	}
	if (!g_hLoadingDialog.Get()){
		g_hLoadingDialog = new CLoadingDialog((vgui::Panel*)(BasePanel()));
		g_hLoadingDialog->SetProgressRange(0, 24);
		g_hLoadingDialog->SetProgressPoint(0);
		g_hLoadingDialog->Open();
	}
	g_hLoadingDialog->Activate();
	return g_hLoadingDialog->SetProgressPoint(progressPoint);*/
	return g_pfnContinueProgressBar(this, 0, progressPoint, progressFraction);
}
void CGameUI::StopProgressBar(bool bError, const char* failureReason, const char* extendedReason) {
	//int* piVar3;
	//if (!g_hLoadingDialog.Get() && bError)
	//	g_hLoadingDialog = new CLoadingDialog((vgui::Panel*)(BasePanel()));
	//if (g_hLoadingDialog.Get()) {
	//	if (bError) 
	//		g_hLoadingDialog->DisplayGenericError(failureReason, extendedReason);
	//	else {
	//		g_hLoadingDialog->Close();
	//		g_hLoadingDialog = nullptr;
	//	}
	//	//CBasePanel::SetBackgroundRenderState((CBasePanel*)staticPanel, 2);
	//}
	g_pfnStopProgressBar(this, 0, bError, failureReason, extendedReason);
}
int  CGameUI::SetProgressBarStatusText(const char* statusText) {
	/*
	if (g_hGameLoading.Get()){
		if (g_hGameLoading->IsVisible()){
			g_hGameLoading->SetStatusText(statusText);
			return false;
		}
	}
	if (!g_hLoadingDialog.Get())
		return false;
	if (!statusText)
		return false;
	if (!stricmp(statusText, m_szPreviousStatusText))
		return false;
	g_hLoadingDialog->SetStatusText(statusText);
	Q_strncpy(m_szPreviousStatusText, statusText, sizeof(m_szPreviousStatusText));
	return true;
	*/
	return g_pfnSetProgressBarStatusText(this, 0, statusText);
}
void CGameUI::SetSecondaryProgressBar(float progress) {
	/*if (g_hGameLoading.Get()){
		if (g_hGameLoading->IsVisible())
			return;
	}
	if (!g_hLoadingDialog.Get())
		return;
	g_hLoadingDialog->SetSecondaryProgress(progress);*/
	g_pfnSetSecondaryProgressBar(this, 0, progress);
}
void CGameUI::SetSecondaryProgressBarText(const char* statusText) {
	/*if (g_hLoadingDialog.Get()){
		if (g_hLoadingDialog->IsVisible())
			return;
	}
	if (!g_hLoadingDialog.Get())
		return;
	g_hLoadingDialog->SetSecondaryProgressText(statusText);*/
	g_pfnSetSecondaryProgressBarText(this, 0, statusText);
}
void CGameUI::ValidateCDKey(bool force, bool inConnect) {
	g_pfnValidateCDKey(this, 0, force, inConnect);
}
void CGameUI::OnDisconnectFromServer(int maybeport, char* maybeip) {
	g_pfnOnDisconnectFromServer(this, 0, maybeport, maybeip);
}
void CGameUI::ShowPasswordPromptAndRetry(char* passwd, bool correct) {
	g_pfnShowPasswordPromptAndRetry(this, 0, passwd, correct);
}
void CGameUI::OnExitToDesktop() {
	g_pfnOnExitToDesktop(this, 0);
}

void GameUI_InstallHook(){
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (hGameUI) {
		CreateInterfaceFn fnCreateInterface = Sys_GetFactory(hGameUI);
		gameui = static_cast<IGameUI*>(fnCreateInterface(GAMEUI_INTERFACE_VERSION, nullptr));
		DWORD* pVFTable = *(DWORD**)&s_GameUI;
#define GAMEUI_VFTHOOK(index, name) g_pMetaHookAPI->VFTHook(gameui, 0, index, (void *)pVFTable[index], (void **)&g_pfn##name)
		GAMEUI_VFTHOOK(1, Initialize);
		GAMEUI_VFTHOOK(2, Start);
		GAMEUI_VFTHOOK(3, Shutdown);
		GAMEUI_VFTHOOK(4, ActivateGameUI);
		GAMEUI_VFTHOOK(5, ActivateDemoUI);
		GAMEUI_VFTHOOK(6, HasExclusiveInput);
		GAMEUI_VFTHOOK(7, RunFrame);
		GAMEUI_VFTHOOK(8, ConnectToServer);
		GAMEUI_VFTHOOK(9, DisconnectFromServer);
		GAMEUI_VFTHOOK(10, HideGameUI);
		GAMEUI_VFTHOOK(11, IsGameUIActive);
		GAMEUI_VFTHOOK(12, LoadingStarted);
		GAMEUI_VFTHOOK(13, LoadingFinished);
		GAMEUI_VFTHOOK(14, StartProgressBar);
		GAMEUI_VFTHOOK(15, ContinueProgressBar);
		GAMEUI_VFTHOOK(16, StopProgressBar);
		GAMEUI_VFTHOOK(17, SetProgressBarStatusText);
		GAMEUI_VFTHOOK(18, SetSecondaryProgressBar);
		GAMEUI_VFTHOOK(19, SetSecondaryProgressBarText);
		GAMEUI_VFTHOOK(20, ValidateCDKey);
		GAMEUI_VFTHOOK(21, OnDisconnectFromServer);
		GAMEUI_VFTHOOK(22, ShowPasswordPromptAndRetry);
		GAMEUI_VFTHOOK(23, OnExitToDesktop);
	}
#undef GAMEUI_VFTHOOK
}