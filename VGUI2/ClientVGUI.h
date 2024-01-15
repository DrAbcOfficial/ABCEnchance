#pragma once

void BaseUI_InstallHook(void);
void ClientVGUIInstallHook(void);
void ClientVGUIShutdown(void);
void Surface_InstallHooks(void);

bool IsOSX();
double GetAbsoluteTime();