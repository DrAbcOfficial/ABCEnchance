#include <Panel.h>
#include <KeyValues.h>
#include "BasePanel.h"
#include <IVGUI2Extension.h>

extern void SetAdvanceOptPanelVisible(bool state);

class CVGUI2Extension_GameUICallbacks : public IVGUI2Extension_GameUICallbacks
{
public:
	int GetAltitude() const override
	{
		return 0;
	}
	const char* GetControlModuleName() const
	{
		return "ABCEnchance";
	}
	void Initialize(CreateInterfaceFn* factories, int count) override{
	}
	void Start(struct cl_enginefuncs_s* engineFuncs, int interfaceVersion, void* system) override
	{
		BackGroundInitMusic();
	}

	void Shutdown(void) override
	{

	}

	void ActivateGameUI(VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void ActivateDemoUI(VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void HasExclusiveInput(VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void RunFrame(VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void ConnectToServer(const char*& game, int& IP, int& port, VGUI2Extension_CallbackContext* CallbackContext) override
	{
		BackGroundSetDecodeState(false);
	}

	void DisconnectFromServer(VGUI2Extension_CallbackContext* CallbackContext) override
	{
		BackGroundSetDecodeState(true);
	}

	void HideGameUI(VGUI2Extension_CallbackContext* CallbackContext) override
	{
		SetAdvanceOptPanelVisible(false);
	}

	void IsGameUIActive(VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void LoadingStarted(const char*& resourceType, const char*& resourceName, VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void LoadingFinished(const char*& resourceType, const char*& resourceName, VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void StartProgressBar(const char*& progressType, int& progressSteps, VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void ContinueProgressBar(int& progressPoint, float& progressFraction, VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void StopProgressBar(bool& bError, const char*& failureReason, const char*& extendedReason, VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void SetProgressBarStatusText(const char*& statusText, VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void SetSecondaryProgressBar(float& progress, VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

	void SetSecondaryProgressBarText(const char*& statusText, VGUI2Extension_CallbackContext* CallbackContext) override
	{

	}

};
static CVGUI2Extension_GameUICallbacks s_GameUICallbacks;

class CVGUI2Extension_GameUIKeyValuesCallbacks : public IVGUI2Extension_GameUIKeyValuesCallbacks{
public:
	int GetAltitude() const override{
		return 0;
	}
	void KeyValues_LoadFromFile(void*& pthis, IFileSystem*& pFileSystem, const char*& resourceName, const char*& pathId, const char* sourceModule, VGUI2Extension_CallbackContext* CallbackContext){
		if (CallbackContext->IsPost && !strcmp(resourceName, "resource/GameMenu.res")){
			bool* pRealReturnValue = (bool*)CallbackContext->pRealReturnValue;
			if ((*pRealReturnValue) == true){
				KeyValues* pKeyValues = (KeyValues*)pthis;
				auto name = pKeyValues->GetName();
				KeyValues* SectionQuit = nullptr;
				for (auto p = pKeyValues->GetFirstSubKey(); p; p = p->GetNextKey()){
					auto command = p->GetString("command");
					if (!stricmp(command, "OpenOptionsDialog"))
						SectionQuit = p;
				}
				if (SectionQuit){
					auto NameSectionQuit = SectionQuit->GetName();
					int iNameSectionQuit = atoi(NameSectionQuit);
					if (iNameSectionQuit > 0){
						char szNewNameSectionQuit[32];
						snprintf(szNewNameSectionQuit, sizeof(szNewNameSectionQuit), "%d", iNameSectionQuit + 1);
						SectionQuit->SetName(szNewNameSectionQuit);
						char szNewNameTestButton[32];
						snprintf(szNewNameTestButton, sizeof(szNewNameTestButton), "%d", iNameSectionQuit);
						auto SectionTestButton = new KeyValues(szNewNameTestButton);
						SectionTestButton->SetString("label", "#GameUI_ABC_Options");
						SectionTestButton->SetString("command", "OpenOptionsABCEnchanceDialog");
						pKeyValues->AddSubKeyAfter(SectionTestButton, SectionQuit);
					}
				}
			}
		}
	}
};

static CVGUI2Extension_GameUIKeyValuesCallbacks s_GameUIKeyValuesCallbacks;

extern void OpenAdvanceOptPanel(vgui::Panel* pthis);
class CVGUI2Extension_TaskBarCallbacks : public IVGUI2Extension_GameUITaskBarCallbacks {
	int GetAltitude() const override{
		return 0;
	}
	virtual void CTaskBar_ctor(IGameUITaskBarCtorCallbackContext* CallbackContext) override {

	}
	virtual void CTaskBar_OnCommand(void*& pPanel, const char*& command, VGUI2Extension_CallbackContext* CallbackContext) override {
		if (!strcmp(command, "OpenOptionsABCEnchanceDialog")) {
			OpenAdvanceOptPanel(reinterpret_cast<vgui::Panel*>(pPanel));
		}
	}
};
static CVGUI2Extension_TaskBarCallbacks s_TaskBarCallbacks;
/*
=================================================================================================================
GameUI init & shutdown
=================================================================================================================
*/

void GameUI_InstallHooks(void){
	VGUI2Extension()->RegisterGameUICallbacks(&s_GameUICallbacks);
	VGUI2Extension()->RegisterGameUIKeyValuesCallbacks(&s_GameUIKeyValuesCallbacks);
	VGUI2Extension()->RegisterGameUITaskBarCallbacks(&s_TaskBarCallbacks);
}

void GameUI_UninstallHooks(void){
	VGUI2Extension()->UnregisterGameUICallbacks(&s_GameUICallbacks);
	VGUI2Extension()->UnregisterGameUIKeyValuesCallbacks(&s_GameUIKeyValuesCallbacks);
	VGUI2Extension()->UnregisterGameUITaskBarCallbacks(&s_TaskBarCallbacks);
}