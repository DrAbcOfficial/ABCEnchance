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

class TransWarpper : public vgui::Panel {
	using Panel::Panel;
public:
	virtual void PaintBackground() override {}
};
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
	virtual bool OnStartRequest(const char* url, const char* target, const char* pchPostData, bool bIsRedirect) {
		m_bInteractive = false;
		return HTML::OnStartRequest(url, target, pchPostData, bIsRedirect);
	}
	virtual void OnFinishRequest(const char* url, const char* pageTitle, const CUtlMap < CUtlString, CUtlString >& headers) override {
		HTML::OnFinishRequest(url, pageTitle, headers);
		if (!m_bInteractive) {
			RequestFocus();
			OnKeyCodeTyped(vgui::KEY_NONE);
			m_bInteractive = true;
		}
		m_bLoaded = strncmp("about:blank", url, 11);
	}
	bool m_bLoaded = false;
private:
	bool m_bInteractive = false;
};

static vgui::IClientPanel* s_pBasePanel;
static vgui::DHANDLE<vgui::COptionsAdvanceDialog>s_hAdvanceOptPanel;

static vgui::PHandle s_hBasePanelWarpper;
static vgui::DHANDLE<BackGroundHTML>s_hHTMLBackground;

static bool s_bInited = false;
static void HTMLReopenBackground() {
	char local[MAX_PATH] = "file:///";
	const size_t uiURLLength = 8;
	vgui::filesystem()->GetLocalPath("abcenchance/scence/background.html", local + uiURLLength, 252);
	s_hHTMLBackground->OpenURL(local, nullptr, true);
}
static void SetBasePanelState(bool state) {
	if (state) {
		if (!s_bInited) {
			int w = ScreenWidth();
			int h = ScreenHeight();
			if (!s_hBasePanelWarpper) {
				s_hBasePanelWarpper = new TransWarpper(s_pBasePanel->GetPanel(), "Warpper");
				s_hBasePanelWarpper->SetSize(w, h);
			}
			if (!s_hHTMLBackground) {
				s_hHTMLBackground = new BackGroundHTML(s_hBasePanelWarpper);
				s_hHTMLBackground->SetBgColor(Color(0, 0, 0, 0));
				HTMLReopenBackground();
			}
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
void BasePanelSetHandle(void* ptr){
	s_pBasePanel = reinterpret_cast<vgui::IClientPanel*>(ptr);
}
void BasePanelInit() {
	gCVars.pDynamicBackground = CREATE_CVAR("hud_dynamic_background", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		SetBasePanelState(cvar->value > 0);
	});
}
void BasePanelPostInit() {
	SetBasePanelState(gCVars.pDynamicBackground->value > 0);
}
static void BasePanelSendJSEvent(const char* cmd) {
	if (s_hHTMLBackground) {
		int x, y;
		vgui::input()->GetCursorPos(x, y);
		std::string js = std::format("window.dispatchEvent(new CustomEvent(\"vgui-command\", {{command:\"{}\",cursor:{{x: {}, y:{}}}}}));", cmd, x, y);
		s_hHTMLBackground.Get()->RunJavascript(js.c_str());
	}
}
void BackGroundOnCommand(void*& pPanel, const char*& cmd) {
	if (!std::strcmp(cmd, "OpenOptionsABCEnchanceDialog")) {
		if (s_hAdvanceOptPanel == nullptr) {
			s_hAdvanceOptPanel = new vgui::COptionsAdvanceDialog(reinterpret_cast<vgui::Panel*>(pPanel));
		}
		s_hAdvanceOptPanel->Activate();
	}
	BasePanelSendJSEvent(cmd);
}
void BasePanelConnectServer() {
	if (s_hHTMLBackground && s_hHTMLBackground.Get()->m_bLoaded) {
		BasePanelSendJSEvent("ConnectedServer");
		s_hHTMLBackground.Get()->OpenURL("about:blank", nullptr, true);
		s_hHTMLBackground.Get()->SetVisible(false);
	}
		
}
void BasePanelDiconnectServer() {
	if (s_hHTMLBackground && !s_hHTMLBackground.Get()->m_bLoaded) {
		HTMLReopenBackground();
		BasePanelSendJSEvent("DisconnectedServer");
		s_hHTMLBackground.Get()->SetVisible(true);
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