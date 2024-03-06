#include <metahook.h>

#include <format>

#include "plugins.h"

#include "local.h"
#include "vguilocal.h"

#include "Controls.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/HTML.h"

#include "GameUI/BasePanel.h"

#include "exportfuncs.h"
#include "OptionAdvancedDlg.h"

static vgui::IClientPanel* s_pBasePanel;
static vgui::DHANDLE<vgui::COptionsAdvanceDialog>s_hAdvanceOptPanel;

static vgui::PHandle s_hBasePanelWarpper;
static vgui::DHANDLE<vgui::HTML>s_hHTMLBackground;

class BackGroundHTML : public vgui::HTML {
public:
	BackGroundHTML(vgui::Panel* parent) : HTML(parent, "HTML", true, false) {
		int w, h;
		parent->GetSize(w, h);
		SetSize(w, h);
		SetScrollbarsEnabled(false);
		SetContextMenuEnabled(false);
		NewWindowsOnly(false);
		SetViewSourceEnabled(false);
		SetKeyBoardInputEnabled(false);
		SetMouseInputEnabled(false);
	}
	virtual void OnFinishRequest(const char* url, const char* pageTitle, const CUtlMap < CUtlString, CUtlString >& headers) override {
		HTML::OnFinishRequest(url, pageTitle, headers);
		//steam初始化时默认打开一个空白页面，需要在该页面之后再打开其他页面以避免竞争者错误
		if (!m_bInit) {
			m_bInit = true;
			char local[MAX_PATH] = "file:///";
			const size_t uiURLLength = 8;
			vgui::filesystem()->GetLocalPath("abcenchance/scence/background.html", local + uiURLLength, 252);
			OpenURL(local, nullptr, true);
		}
		RequestFocus();
		OnKeyCodeTyped(vgui::KEY_ENTER);
	}
private:
	bool m_bInit = false;
};

static bool s_bInited = false;
static void SetBasePanelState(bool state) {
	if (state) {
		if (!s_bInited) {
			int w = ScreenWidth();
			int h = ScreenHeight();
			if (!s_hBasePanelWarpper) {
				s_hBasePanelWarpper = new vgui::Panel(s_pBasePanel->GetPanel(), "Warpper");
				s_hBasePanelWarpper->SetSize(w, h);
			}
			if (!s_hHTMLBackground)
				s_hHTMLBackground = new BackGroundHTML(s_hBasePanelWarpper);
			s_bInited = true;
		}
		EngineClientCmd("mp3 stop");
	}
	else {
		if (s_bInited) {
			if (s_hBasePanelWarpper)
				s_hBasePanelWarpper.Get()->DeletePanel();
			if (s_hHTMLBackground)
				s_hHTMLBackground.Get()->DeletePanel();
			s_bInited = false;
		}
		EngineClientCmd("mp3 loop media/gamestartup.mp3 ui");
	}
}
void BasePanelInit() {
	gCVars.pDynamicBackground = CREATE_CVAR("hud_dynamic_background", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		SetBasePanelState(cvar->value > 0);
	});
}
void BasePanelPostInit() {
	SetBasePanelState(gCVars.pDynamicBackground->value > 0);
}
void BackGroundOnCommand(void*& pPanel, const char*& cmd) {
	if (!std::strcmp(cmd, "OpenOptionsABCEnchanceDialog")) {
		if (s_hAdvanceOptPanel == nullptr) {
			s_hAdvanceOptPanel = new vgui::COptionsAdvanceDialog(reinterpret_cast<vgui::Panel*>(pPanel));
		}
		s_hAdvanceOptPanel->Activate();
	}
	if (s_hHTMLBackground) {
		int x, y;
		vgui::input()->GetCursorPos(x, y);
		std::string js = std::format("OnVGUICommand(\"{}\", {}, {});", cmd, x, y);
		s_hHTMLBackground.Get()->RunJavascript(js.c_str());
	}
}

vgui::IClientPanel* BasePanel() {
	return s_pBasePanel;
}
void SetAdvanceOptPanelVisible(bool state) {
	if (s_hAdvanceOptPanel) {
		if (state)
			s_hAdvanceOptPanel->Activate();
		else
			s_hAdvanceOptPanel->Close();
	}
}
void* __fastcall CBasePanel_ctor(void* pthis, int dummy) {
	s_pBasePanel = static_cast<vgui::IClientPanel*>(gHookFuncs.CBasePanel_ctor(pthis, dummy));
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "gameui/OptionsAdvanceDialogScheme.res", "OptionsAdvanceDialogScheme");
	return s_pBasePanel;
}
void BasePanel_InstallHook(void){
	HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandle("GameUI.dll");
	if (!hGameUI) {
		SYS_ERROR("Failed to locate GameUI.dll");
		return;
	}
	auto GameUIBase = g_pMetaHookAPI->GetModuleBase(hGameUI);
	auto GameUISize = g_pMetaHookAPI->GetModuleSize(hGameUI);

#define SC_CBASEPANEL_CTOR_SIG "\x55\x8B\xEC\x51\x56\x68\x2A\x2A\x2A\x2A\x8B\xF1\x6A\x00\x89\x75\xFC\xE8\x2A\x2A\x2A\x2A\xC7"
	Fill_Sig(SC_CBASEPANEL_CTOR_SIG, GameUIBase, GameUISize, CBasePanel_ctor);
	Install_InlineHook(CBasePanel_ctor);
#define SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG "\x55\x8B\xEC\x83\xEC\x38\x53\x8D\x45\xCC\x8B\xD9\x50\x8D\x45\xC8\x89\x5D\xD0\x50\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8D\x4D\xD4\x51"
	//Fill_Sig(SC_CBASEPANEL_PAINTBACKGROUNDIMAGE_SIG, GameUIBase, GameUISize, CBasePanel_PaintBackground);
	//Install_InlineHook(CBasePanel_PaintBackground);
}