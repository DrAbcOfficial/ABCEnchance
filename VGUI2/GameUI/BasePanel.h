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
void BackGroundVideoInit();
void BackGroundVideoClose();
void BackGroundSetDecodeState(bool state);
void BackGroundInitMusic();

class IVanilliaPanel;
IVanilliaPanel* BasePanel();