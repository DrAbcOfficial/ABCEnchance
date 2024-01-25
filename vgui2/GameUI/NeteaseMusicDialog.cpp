#include <vector>
#include <future>
#include <atomic>

#include "plugins.h"

#include "vgui_controls/MemoryBitmap.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Button.h"

#include "BasePanel.h"

#include "Task.h"
#include "FreeImage.h"
#include "NeteaseApi.h"
#include "curl.h"

#include "NeteaseMusicDialog.h"

extern std::atomic<netease::CNeteaseMusicAPI*> GetNeteaseApi();

using namespace vgui;

class CNeteaseUserInfo : public Frame {
public:
	CNeteaseUserInfo(Panel* parent, const char* name) : Frame(parent, name) {
		SetTitleBarVisible(false);
		SetMouseInputEnabled(false);
		SetKeyBoardInputEnabled(false);
		SetAutoResize(PIN_BOTTOMRIGHT, AUTORESIZE_DOWNANDRIGHT, 0, 0, 0, 0);
		SetSizeable(false);
		SetMoveable(false);
		m_pBackgroud = new ImagePanel(this, "Backgroud");
		m_pAvatar = new ImagePanel(this, "Avatar");
		m_pName = new Label(this, "Name", "%%name%%");
		m_pSignature = new Label(this, "Signature", "%%sig%%");
		m_pIsVip = new Label(this, "VIP", "%%vip%%");
		m_pLevel = new Label(this, "Level", "#GameUI_ABC_Level");
		m_pLevelNum = new Label(this, "LevelNumber", "%%level%%");

		m_pAvatar->SetShouldScaleImage(true);
		m_pBackgroud->SetShouldScaleImage(true);
	}
	void SetTextColor(Color c) {
		m_pName->SetFgColor(c);
		m_pSignature->SetFgColor(c);
		m_pIsVip->SetFgColor(c);
		m_pLevel->SetFgColor(c);
		m_pLevelNum->SetFgColor(c);
	}
	virtual void ApplySchemeSettings(IScheme* pScheme) override {
		Frame::ApplySchemeSettings(pScheme);
		m_pBackgroud->SetDrawColor(Color(125, 125, 125, 125));
		auto empty = Color(0, 0, 0, 0);
		m_pName->SetBgColor(empty);
		m_pSignature->SetBgColor(empty);
		m_pIsVip->SetBgColor(empty);
		m_pLevel->SetBgColor(empty);
		m_pLevelNum->SetBgColor(empty);
	}
	virtual void PerformLayout() {
		Panel::PerformLayout();
		int w, h;
		GetSize(w, h);
		int startX = 0;
		int startY = 0;
		int contentW = w;
		int avatarSize = h / 2;
		m_pBackgroud->SetBounds(0, 0, w, h);
		m_pAvatar->SetBounds(startX, startY, avatarSize, avatarSize);
		m_pName->SetBounds(startX + avatarSize + 2, startY, contentW - 2 - avatarSize, avatarSize / 3);
		m_pLevel->SetBounds(startX + avatarSize + 2, startY + avatarSize / 3, (contentW - 2 - avatarSize) / 2, avatarSize / 3);
		m_pLevelNum->SetBounds((startX + avatarSize + 2) + (contentW - 2 - avatarSize) / 2, startY + avatarSize / 3, (contentW - 2 - avatarSize) / 2, avatarSize / 3);
		m_pIsVip->SetBounds(startX + avatarSize + 2, startY + avatarSize / 3 * 2, contentW - 2 - avatarSize, avatarSize / 3);
		m_pSignature->SetBounds(startX, startY + avatarSize, contentW, h / 2 - startY);
	}
	static size_t append(void* ptr, size_t size, size_t nmemb, void* user) {
		std::vector<byte>* p = (std::vector<byte>*)user;
		auto cs = p->size();
		p->resize(cs + size * nmemb);
		memcpy(p->data() + cs, ptr, size * nmemb);
		return size * nmemb;
	}
	static std::vector<byte> DownLoadImg(std::string url) {
		std::vector<byte> retdata;
		void* curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_HEADER, 0);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &append);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retdata);
			curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
			curl_easy_setopt(curl, CURLOPT_COOKIEFILE, netease::CNeteaseMusicAPI::CokkieInPath().c_str());
			curl_easy_setopt(curl, CURLOPT_COOKIEJAR, netease::CNeteaseMusicAPI::CookieOutPath().c_str());
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);
			curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
		return retdata;
	}
	static void DecodeImage(std::vector<byte> raw, ImagePanel* target, CNeteaseUserInfo* info) {
		int ar = 0, ag = 0, ab = 0;
		FIMEMORY* mem = FreeImage_OpenMemory(raw.data(), raw.size());
		FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(mem);
		FIBITMAP* bitmap = FreeImage_LoadFromMemory(format, mem);
		byte* pixels = (byte*)FreeImage_GetBits(bitmap);
		auto width = FreeImage_GetWidth(bitmap);
		auto height = FreeImage_GetHeight(bitmap);
		auto pitch = FreeImage_GetPitch(bitmap);
		auto bpp = FreeImage_GetBPP(bitmap);
		auto bitnum = bpp / 8;
		static byte* s_pBuf;
		static int s_Size;
		if (s_Size < width * height) {
			if (s_pBuf)
				delete[] s_pBuf;
			s_Size = width * height;
			s_pBuf = new byte[width * height * 4];
		}
		size_t c = 0;
		pixels += pitch * (height - 1);
		for (size_t y = 0; y < height; y++) {
			BYTE* pixel = (BYTE*)pixels;
			for (size_t x = 0; x < width; x++) {
				switch (bitnum) {
					//8bpp
				case 1: {
					BYTE grey = pixel[0];
					s_pBuf[c * 4 + 0] = grey;
					s_pBuf[c * 4 + 1] = grey;
					s_pBuf[c * 4 + 2] = grey;
					s_pBuf[c * 4 + 3] = 255;
					ar = (ar + grey) / 2;
					ag = (ag + grey) / 2;
					ab = (ab + grey) / 2;
					break;
				}
					  //16bpp
				case 2: {
					int code = (pixel[1] << 8) + pixel[0];
					s_pBuf[c * 4 + 0] = code & 0x1F;
					s_pBuf[c * 4 + 1] = (code & 0x7E0) >> 5;
					s_pBuf[c * 4 + 2] = (code & 0xF800) >> 11;
					s_pBuf[c * 4 + 3] = 255;
					ar = (ar + s_pBuf[c * 4 + 0]) / 2;
					ag = (ag + s_pBuf[c * 4 + 1]) / 2;
					ab = (ab + s_pBuf[c * 4 + 2]) / 2;
					break;
				}
					  //24bpp
				case 3: {
					s_pBuf[c * 4 + 0] = pixel[FI_RGBA_RED];
					s_pBuf[c * 4 + 1] = pixel[FI_RGBA_GREEN];
					s_pBuf[c * 4 + 2] = pixel[FI_RGBA_BLUE];
					s_pBuf[c * 4 + 3] = 255;
					ar = (ar + pixel[FI_RGBA_RED]) / 2;
					ag = (ag + pixel[FI_RGBA_GREEN]) / 2;
					ab = (ab + pixel[FI_RGBA_BLUE]) / 2;
					break;
				}
					  //32bpp
				case 4: {
					s_pBuf[c * 4 + 0] = pixel[FI_RGBA_RED];
					s_pBuf[c * 4 + 1] = pixel[FI_RGBA_GREEN];
					s_pBuf[c * 4 + 2] = pixel[FI_RGBA_BLUE];
					s_pBuf[c * 4 + 3] = pixel[FI_RGBA_ALPHA];
					ar = (ar + pixel[FI_RGBA_RED]) / 2;
					ag = (ag + pixel[FI_RGBA_GREEN]) / 2;
					ab = (ab + pixel[FI_RGBA_BLUE]) / 2;
					break;
				}
					  //cnmdwy
				default: break;
				}
				pixel += bitnum;
				c++;
			}
			pixels -= pitch;
		}
		FreeImage_Unload(bitmap);
		FreeImage_CloseMemory(mem);

		Vector crgb = Vector(ar, ag, ab);
		Vector chsv;
		RGBtoHSV(crgb, chsv);
		chsv.x = fmodf((chsv.x + 120), 360.0f);
		HSVtoRGB(chsv, crgb);
		info->SetTextColor(Color(crgb.x, crgb.y, crgb.z));
		target->SetImage(new MemoryBitmap(s_pBuf, width, height));
	}
	void SetUserInfo(std::shared_ptr<netease::CMy> myinfo) {
		if (myinfo == nullptr) {
			m_pName->SetText("#GaemUI_ABC_Netease_NotLogin");
			m_pSignature->SetText("");
			m_pIsVip->SetText("");
			m_pLevelNum->SetText("");
			return;
		}
		GetTaskManager()->Add(
			std::async([](std::string url) {
				return DownLoadImg(url);
				}, myinfo->avatarurl + "?param=130y130")
		)->ContinueWith(
			[](std::vector<byte> retdata, ImagePanel* target, CNeteaseUserInfo* info) {
				DecodeImage(retdata, target, info);
			}, m_pAvatar, this
		)->Start();

		int w, h;
		GetSize(w, h);
		GetTaskManager()->Add(
			std::async([](std::string url) {
				return DownLoadImg(url);
				}, myinfo->backgroundurl + "?param=" + std::to_string(w) + "y" + std::to_string(h))
		)->ContinueWith(
			[](std::vector<byte> retdata, ImagePanel* target, CNeteaseUserInfo* info) {
				DecodeImage(retdata, target, info);
			}, m_pBackgroud, this
		)->Start();

		m_pName->SetText(myinfo->name.c_str());
		m_pSignature->SetText(myinfo->signature.c_str());
		m_pLevelNum->SetText(std::to_string(myinfo->level).c_str());
		m_pIsVip->SetText(myinfo->vip ? "#GameUI_ABC_VIP" : "#GameUI_ABC_Not_VIP");
	};
private:
	ImagePanel* m_pBackgroud;
	ImagePanel* m_pAvatar;
	Label* m_pName;
	Label* m_pLevel;
	Label* m_pLevelNum;
	Label* m_pIsVip;
	Label* m_pSignature;
};

class CNeteaseUserButton : public Label {
public:
	CNeteaseUserButton(Panel* parent, const char* name) : Label(parent, name, "#GaemUI_ABC_Netease_Logining") {
		m_pUserInfo = new CNeteaseUserInfo(this, "UserInfo");
		m_pUserInfo->SetVisible(false);
		m_pUserInfo->SetFadeEffectDisableOverride(false);
		GetNeteaseUserInfo();
	}
	void GetNeteaseUserInfo() {
		GetTaskManager()->Add(std::async([]() {
			return GetNeteaseApi().load()->GetMyself();
		}))->ContinueWith([](std::shared_ptr<netease::CMy> info, CNeteaseUserInfo* pInfoPanel, CNeteaseUserButton* pButton) {
			if (info)
				pButton->SetText(info->name.c_str());
			else
				pButton->SetText("#GaemUI_ABC_Netease_NotLogin");
			pInfoPanel->SetUserInfo(info);
		}, m_pUserInfo, this)->Start();
	}
	void PopupInfo() {
		int x, y;
		ipanel()->GetAbsPos(GetVPanel(), x, y);
		int w, h;
		GetSize(w, h);
		int pw, ph;
		GetParent()->GetSize(pw, ph);
		m_pUserInfo->SetBounds(x, y + h, pw / 2, ph / 2);
		m_pUserInfo->MakePopup();
		m_pUserInfo->Activate();
	}
protected:
	virtual void OnCursorEntered() override {
		PopupInfo();
	}
	virtual void OnCursorExited() override {
		m_pUserInfo->SetVisible(false);
	}

private:
	CNeteaseUserInfo* m_pUserInfo;
};

vgui::CNeteaseMusicDialog::CNeteaseMusicDialog(Panel* parent) : BaseClass(parent, "NeteaseMusicDialog", false, true){
	SetProportional(true);
	SetSizeable(false);
	SetTitle("#GaemUI_ABC_Netease_Title", false);
	SetScheme("OptionsAdvanceDialogScheme");
	m_pUserInfo = new CNeteaseUserButton(this, "UserInfoButton");
	m_pDivideLine = new Panel(this, "Divide");
	LoadControlSettings("abcenchance/res/gameui/NeteaseMusicDialog.res");
}

vgui::CNeteaseMusicDialog::~CNeteaseMusicDialog(){
}

static CNeteaseMusicDialog* s_pNeteaseDialog;
void CreateNeteaseMusicDialogCmd() {
	gEngfuncs.pfnAddCommand("open_neteasedialog", []() {
		if (!s_pNeteaseDialog)
			s_pNeteaseDialog = new CNeteaseMusicDialog(reinterpret_cast<Panel*>(BasePanel()));
		int x, y;
		input()->GetCursorPos(x, y);
		s_pNeteaseDialog->SetPos(x, y);
		s_pNeteaseDialog->MakePopup();
		s_pNeteaseDialog->Activate();
	});
}