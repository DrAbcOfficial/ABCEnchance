#pragma once

enum EBackgroundState{
	BACKGROUND_INITIAL,
	BACKGROUND_LOADING,
	BACKGROUND_MAINMENU,
	BACKGROUND_LEVEL,
	BACKGROUND_DISCONNECTED,
	BACKGROUND_EXITING,
};

extern void BackGroundOnCommand(void*& pPanel, const char*& cmd);
extern void BasePanelInit();
extern void BasePanelSetHandle(void* ptr);
extern void BasePanelPostInit();
extern void BasePanelConnectServer();
extern void BasePanelDiconnectServer();

namespace vgui {
	class IClientPanel;
}
vgui::IClientPanel* BasePanel();