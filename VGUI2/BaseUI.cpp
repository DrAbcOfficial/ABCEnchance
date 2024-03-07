#include <metahook.h>
#include <VGUI\IScheme.h>
#include <VGUI\ILocalize.h>
#include <VGUI\ISurface.h>
#include <VGUI\IInput.h>

#include <GameUI/BasePanel.h>

#include <IBaseUI.h>
#include <IEngineSurface.h>
#include <IKeyValuesSystem.h>
#include <IGameUIFuncs.h>
#include <IVGUI2Extension.h>
#include "plugins.h"

namespace vgui
{
	bool VGui_InitInterfacesList(const char* moduleName, CreateInterfaceFn* factoryList, int numFactories);
}

IGameUIFuncs* gameuifuncs = nullptr;
IBaseUI* baseuifuncs = nullptr;

extern IKeyValuesSystem* g_pKeyValuesSystem;
extern IEngineSurface* staticSurface;
//extern void CreateNeteaseMusicDialogCmd();

class CVGUI2Extension_BaseUICallbacks : public IVGUI2Extension_BaseUICallbacks
{
public:
	virtual int GetAltitude() const override{
		return 0;
	}

	virtual void Initialize(CreateInterfaceFn* factories, int count) override{
		HINTERFACEMODULE hVGUI2 = (HINTERFACEMODULE)GetModuleHandleA("vgui2.dll");

		if (hVGUI2){
			CreateInterfaceFn fnVGUI2CreateInterface = Sys_GetFactory(hVGUI2);
			g_pKeyValuesSystem = (IKeyValuesSystem*)fnVGUI2CreateInterface(KEYVALUESSYSTEM_INTERFACE_VERSION, NULL);
		}
		staticSurface = (IEngineSurface*)factories[0](ENGINE_SURFACE_VERSION, NULL);
		BasePanel_InstallHook();
		BasePanelInit();
		//CreateNeteaseMusicDialogCmd();
	}
	virtual void Start(struct cl_enginefuncs_s* engineFuncs, int interfaceVersion) override{
		
	}
	virtual void Shutdown(void){

	}
	virtual void Key_Event(int& down, int& keynum, const char*& pszCurrentBinding, VGUI2Extension_CallbackContext* CallbackContext) override{

	}
	virtual void CallEngineSurfaceAppProc(void*& pevent, void*& userData, VGUI2Extension_CallbackContext* CallbackContext) override {

	}
	virtual void CallEngineSurfaceWndProc(void*& hwnd, unsigned int& msg, unsigned int& wparam, long& lparam, VGUI2Extension_CallbackContext* CallbackContext) override {

	}
	virtual void Paint(int& x, int& y, int& right, int& bottom, VGUI2Extension_CallbackContext* CallbackContext) override{

	}
	virtual void HideGameUI(VGUI2Extension_CallbackContext* CallbackContext) override{

	}
	virtual void ActivateGameUI(VGUI2Extension_CallbackContext* CallbackContext) override{

	}
	virtual void HideConsole(VGUI2Extension_CallbackContext* CallbackContext) override{

	}
	virtual void ShowConsole(VGUI2Extension_CallbackContext* CallbackContext) override{

	}
};

static CVGUI2Extension_BaseUICallbacks s_BaseUICallbacks;

void BaseUI_InstallHooks(void){
	CreateInterfaceFn fnCreateInterface = g_pMetaHookAPI->GetEngineFactory();
	if (!fnCreateInterface)
	{
		SYS_ERROR("Failed to get engine factory.");
		return;
	}
	gameuifuncs = (IGameUIFuncs*)fnCreateInterface(VENGINE_GAMEUIFUNCS_VERSION, NULL);
	if (!fnCreateInterface){
		SYS_ERROR("Failed to get interface \"" VENGINE_GAMEUIFUNCS_VERSION "\" from engine.");
		return;
	}
	baseuifuncs = (IBaseUI*)fnCreateInterface(BASEUI_INTERFACE_VERSION, NULL);
	if (!baseuifuncs){
		SYS_ERROR("Failed to get interface \"" BASEUI_INTERFACE_VERSION "\" from engine.");
		return;
	}
	VGUI2Extension()->RegisterBaseUICallbacks(&s_BaseUICallbacks);
}
void BaseUI_UninstallHooks(void){
	VGUI2Extension()->UnregisterBaseUICallbacks(&s_BaseUICallbacks);
}

IGameUIFuncs* GameUIFuncs() {
	return gameuifuncs;
}
IBaseUI* BaseUI() {
	return baseuifuncs;
}