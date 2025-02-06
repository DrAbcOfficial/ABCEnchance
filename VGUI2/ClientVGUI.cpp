#include <vector>
#include <string>

#include "plugins.h"

#include <VGUI_controls/Controls.h>
#include <VGUI_controls/Panel.h>
#include <VGUI_controls/Frame.h>

#include <IClientVGUI.h>
#include "vguilocal.h"
#include "steam_api.h"
#include "player_info.h"
#include "scoreboard.h"
#include "Viewport.h"
#include "exportfuncs.h"

#include <vgui.h>
#include "ClientVGUI.h"
#include <IVGUI2Extension.h>

namespace vgui{
	extern bool VGui_InitInterfacesList(const char* moduleName, CreateInterfaceFn* factoryList, int numFactories);
}

class CVGUI2Extension_ClientVGUICallbacks : public IVGUI2Extension_ClientVGUICallbacks {
public:
	virtual void Initialize(CreateInterfaceFn* factories, int count) override {
		vgui::VGui_InitInterfacesList("ABCEnchance", factories, count);
		vgui::HScheme iScheme = vgui::scheme()->LoadSchemeFromFile("abcenchance\\ABCEnchance.res", "ABCEnchance");
		int iPluginVersion = 0;
		if (iScheme != 0) {
			pSchemeData = vgui::scheme()->GetIScheme(vgui::scheme()->GetScheme("ABCEnchance"));
			iPluginVersion = atoi(pSchemeData->GetResourceString("Version"));
		}
		else{
			SYS_ERROR("Could not load \"abcenchance\\ABCEnchance.res\"!\nHave you installed it correctly?\n");
			return;
		}

		if (iPluginVersion < PLUGIN_VERSION){
			SYS_ERROR("Resource version mismatch: \"abcenchance\\ABCEnchance.res\" too old.\nRequired Version: %d\nYour Version: %d\n",
				PLUGIN_VERSION, iPluginVersion);
			return;
		}
	}
	virtual void Start(void) override {
		extern vgui::CViewport* g_pViewPort;
		g_pViewPort = new vgui::CViewport();
		g_pViewPort->Start();
	}
	virtual void SetParent(vgui::VPANEL parent) override {
		GetBaseViewPort()->SetParent(parent);
	}
	virtual void UseVGUI1(VGUI2Extension_CallbackContext* CallbackContext) override {
	}
	virtual void HideScoreBoard(VGUI2Extension_CallbackContext* CallbackContext) override {

	}
	virtual void HideAllVGUIMenu(VGUI2Extension_CallbackContext* CallbackContext) override {
	}
	virtual void ActivateClientUI(VGUI2Extension_CallbackContext* CallbackContext) override {
		GetBaseViewPort()->ActivateClientUI();
	}
	virtual void HideClientUI(VGUI2Extension_CallbackContext* CallbackContext) override {
		GetBaseViewPort()->HideClientUI();
	}
	virtual void Shutdown() override {
		extern vgui::CViewport* g_pViewPort;
		if (g_pViewPort) {
			delete g_pViewPort;
			g_pViewPort = nullptr;
		}
	}
	virtual int GetAltitude() const override{
		return 0;
	}
};
static CVGUI2Extension_ClientVGUICallbacks s_ClientVGUICallbacks;

extern IVGUI2Extension* VGUI2Extension();

void ClientVGUI_InstallHooks(void) {
	VGUI2Extension()->RegisterClientVGUICallbacks(&s_ClientVGUICallbacks);
}
void ClientVGUI_UninstallHooks(void){
	VGUI2Extension()->UnregisterClientVGUICallbacks(&s_ClientVGUICallbacks);
}
void ClientVGUI_InitLocalize() {
	if (g_pFileSystem) {
		if (!vgui::localize()->AddFile(g_pFileSystem, "abcenchance/localize/%language%.txt")) {
			if (!vgui::localize()->AddFile(g_pFileSystem, "abcenchance/localize/english.txt"))
				SYS_ERROR("Failed to load abcenchance/localize/english.txt");
		}
	}
}

//vgui2
bool g_bIMEComposing = false;
double g_flImeComposingTime = 0;
//fuck me
bool IsOSX(){
	return false;
}
double GetAbsoluteTime() {
	return vgui::surface()->GetAbsoluteTime();
}
ICommandLine* CommandLine(void) {
	return g_pInterface->CommandLine;
}
