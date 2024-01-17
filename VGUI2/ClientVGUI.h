#pragma once

void BaseUI_InstallHook(void);
void ClientVGUIInstallHook(void);
void ClientVGUIShutdown(void);

bool IsOSX();
double GetAbsoluteTime();