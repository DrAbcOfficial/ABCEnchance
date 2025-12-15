#include <metahook.h>

#include <format>
#include <span>

#include "plugins.h"

#include "utility/util.h"
#include "utility/vgui_util.h"

#include "Controls.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/HTML.h"

#include "core/resource/playerresource.h"

#include "GameUI/BasePanel.h"

#include "exportfuncs.h"
#include "OptionAdvancedDlg.h"

constexpr const char ENCODING_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static std::string encodeBase64(std::span<const unsigned char> data) {
	std::string result;
	result.reserve(((data.size() + 2) / 3) * 4);

	for (size_t i = 0; i < data.size(); i += 3) {
		uint32_t triad = 0;
		size_t bytes_used = 0;

		for (size_t j = 0; j < 3 && (i + j) < data.size(); ++j) {
			triad |= (static_cast<uint8_t>(data[i + j]) << (16 - 8 * j));
			bytes_used++;
		}

		result += ENCODING_TABLE[(triad >> 18) & 0x3F];
		result += ENCODING_TABLE[(triad >> 12) & 0x3F];
		result += (bytes_used >= 2) ? ENCODING_TABLE[(triad >> 6) & 0x3F] : '=';
		result += (bytes_used >= 3) ? ENCODING_TABLE[triad & 0x3F] : '=';
	}
	return result;
}

class TransWarpper : public vgui::Panel {
	using Panel::Panel;
public:
	virtual void PaintBackground() override {}
};
class BackGroundHTML : public vgui::HTML {
private:
	std::string m_playerMessage{};
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
			if (m_playerMessage.size() == 0) {
				auto steamid = SteamUser()->GetSteamID();
				if (!steamid.IsValid())
					return;
				std::string name{};
				std::string avatar{};
				uint32 wide = 0, tall = 0;
				if (!SteamFriends()->RequestUserInformation(steamid, false))
				{
					int iAvatar = 0;
					name = SteamFriends()->GetPersonaName();
					iAvatar = SteamFriends()->GetLargeFriendAvatar(steamid);
					if (iAvatar > 0)
					{
						if (SteamUtils()->GetImageSize(iAvatar, &wide, &tall) && wide > 0 && tall > 0)
						{
							int destBufferSize = wide * tall * 4;
							byte* rgbDest = new byte[destBufferSize];
							if (SteamUtils()->GetImageRGBA(iAvatar, rgbDest, destBufferSize))
								avatar = encodeBase64(std::span<const byte>(rgbDest, destBufferSize));
							delete[] rgbDest;
						}
					}
				}
				m_playerMessage = std::format("{{playername:\"{}\", steamid:{}, avatar:{{width:{}, height:{}, data:\"{}\"}}}}", name, steamid.ConvertToUint64(), wide, tall, avatar);
			}
			//send player info and other info
			BasePanelSendJSEvent("player_info", m_playerMessage.c_str());
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
			int screen_w, screen_h;
			vgui::surface()->GetScreenSize(screen_w, screen_h);
			if (!s_hBasePanelWarpper) {
				s_hBasePanelWarpper = new TransWarpper(s_pBasePanel->GetPanel(), "Warpper");
				s_hBasePanelWarpper->SetSize(screen_w, screen_h);
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
void BasePanelSendJSEvent(const char* cmd, const char* other) {
	if (s_hHTMLBackground) {
		int x, y;
		vgui::input()->GetCursorPos(x, y);
		std::string js = std::format("window.dispatchEvent(new CustomEvent(\"vgui-command\", {{detail:{{command:\"{}\",cursor:{{x: {}, y:{}}}, message:{}}}}}));", cmd, x, y, other ? other : "{}");
		s_hHTMLBackground.Get()->RunJavascript(js.c_str());
	}
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