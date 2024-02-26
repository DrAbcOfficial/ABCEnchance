#include <metahook.h>
#include "plugins.h"

#include <vgui_controls/consoledialog.h>
#include <IVGUI2Extension.h>

vgui::CConsoleDialog* g_ConsoleDialog;

void ConsoleGoodMessage(const char* format, ...) {
	va_list argptr;
	char data[2048];
	va_start(argptr, format);
	Q_vsnprintf(data, sizeof(data), format, argptr);
	va_end(argptr);
	char result[2048] ;
	std::snprintf(result, 2048, "[ABCEnchance] %s\n", data);
	g_ConsoleDialog->ColorPrint(Color(0, 255, 0, 255), result);
}
void ConsoleWarnMessage(const char* format, ...) {
	va_list argptr;
	char data[2048];
	va_start(argptr, format);
	Q_vsnprintf(data, sizeof(data), format, argptr);
	va_end(argptr);
	char result[2048];
	std::snprintf(result, 2048, "[ABCEnchance] %s\n", data);
	g_ConsoleDialog->ColorPrint(Color(255, 255, 0, 255), result);
}
void ConsoleErrorMessage(const char* format, ...) {
	va_list argptr;
	char data[2048];
	va_start(argptr, format);
	Q_vsnprintf(data, sizeof(data), format, argptr);
	va_end(argptr);
	char result[2048];
	std::snprintf(result, 2048, "[ABCEnchance] %s\n", data);
	g_ConsoleDialog->ColorPrint(Color(255, 0, 0, 255), result);
}

class CVGUI2Extension_GameConsoleCallbacks : public IVGUI2Extension_GameConsoleCallbacks{
public:
	virtual int GetAltitude() const {
		return 0;
	}
	virtual void Activate(VGUI2Extension_CallbackContext* CallbackContext) {
		if (!CallbackContext->IsPost) {
			g_ConsoleDialog->Activate();
			CallbackContext->Result = VGUI2Extension_Result::SUPERCEDE;
		}
	}
	virtual void Initialize(VGUI2Extension_CallbackContext* CallbackContext) {
		if (!CallbackContext->IsPost) {
			g_ConsoleDialog = new vgui::CConsoleDialog(nullptr, "Console", false);
			ConsoleGoodMessage("ABCEnchance has run successfully!");
			CallbackContext->Result = VGUI2Extension_Result::SUPERCEDE;
		}
	}
	virtual void Hide(VGUI2Extension_CallbackContext* CallbackContext) {
		if (!CallbackContext->IsPost) {
			g_ConsoleDialog->Close();
			CallbackContext->Result = VGUI2Extension_Result::SUPERCEDE;
		}
	}
	virtual void Clear(VGUI2Extension_CallbackContext* CallbackContext) {
		if (!CallbackContext->IsPost) {
			g_ConsoleDialog->Clear();
			CallbackContext->Result = VGUI2Extension_Result::SUPERCEDE;
		}
	}
	virtual void IsConsoleVisible(VGUI2Extension_CallbackContext* CallbackContext) {
		if (!CallbackContext->IsPost) {
			*((bool*)CallbackContext->pPluginReturnValue) = g_ConsoleDialog->IsVisible();
			CallbackContext->Result = VGUI2Extension_Result::SUPERCEDE;
		}
	}
	virtual void Printf(IVGUI2Extension_String* str, VGUI2Extension_CallbackContext* CallbackContext) {
		if (!CallbackContext->IsPost) {
			g_ConsoleDialog->Print(str->c_str());
			CallbackContext->Result = VGUI2Extension_Result::SUPERCEDE;
		}
		
	}
	virtual void DPrintf(IVGUI2Extension_String* str, VGUI2Extension_CallbackContext* CallbackContext) {
		if (!CallbackContext->IsPost) {
			g_ConsoleDialog->DPrint(str->c_str());
			CallbackContext->Result = VGUI2Extension_Result::SUPERCEDE;
		}
	}
	virtual void SetParent(vgui::VPANEL parent, VGUI2Extension_CallbackContext* CallbackContext) {
		if (!CallbackContext->IsPost) {
			g_ConsoleDialog->SetParent(parent);
			CallbackContext->Result = VGUI2Extension_Result::SUPERCEDE;
		}
	}
};

CVGUI2Extension_GameConsoleCallbacks s_GameConsolecallback;
void GameConsole_InstallHook(void){
	VGUI2Extension()->RegisterGameConsoleCallbacks(&s_GameConsolecallback);
}
void GameConsole_UninstallHook(void) {
	VGUI2Extension()->UnregisterGameConsoleCallbacks(&s_GameConsolecallback);
}