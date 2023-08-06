enum EBackgroundState{
	BACKGROUND_INITIAL,
	BACKGROUND_LOADING,
	BACKGROUND_MAINMENU,
	BACKGROUND_LEVEL,
	BACKGROUND_DISCONNECTED,
	BACKGROUND_EXITING,
};

void BasePanel_InstallHook(void);
void BackGroundVideoInit();
void BackGroundVideoClose();