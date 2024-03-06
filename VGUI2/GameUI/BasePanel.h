#pragma once

enum EBackgroundState{
	BACKGROUND_INITIAL,
	BACKGROUND_LOADING,
	BACKGROUND_MAINMENU,
	BACKGROUND_LEVEL,
	BACKGROUND_DISCONNECTED,
	BACKGROUND_EXITING,
};

void BasePanel_InstallHook();
void BackGroundOnCommand(void*& pPanel, const char*& cmd);
void BasePanelInit();
void BasePanelPostInit();
void BasePanelConnectServer();
void BasePanelDiconnectServer();

namespace vgui {
	class IClientPanel;
}
vgui::IClientPanel* BasePanel();