#pragma once

#include <IVGUI2Extension.h>
#include <IDpiManager.h>

extern void VGUI2Extension_Init();
extern void VGUI2Extension_Shutdown();

extern void BaseUI_InstallHooks(void);
extern void BaseUI_UninstallHooks(void);

extern void GameUIBasePanel_InstallHooks();
extern void GameUIBasePanel_UninstallHooks();

extern void GameUI_InstallHooks(void);
extern void GameUI_UninstallHooks(void);

extern void ClientVGUI_InstallHooks(void);
extern void ClientVGUI_UninstallHooks(void);

extern void GameConsole_InstallHook();
extern void GameConsole_UninstallHook();
