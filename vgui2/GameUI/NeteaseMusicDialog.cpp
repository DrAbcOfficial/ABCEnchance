#include <vector>
#include <atomic>

#include "plugins.h"

#include "vgui_controls/MemoryBitmap.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImageList.h"
#include "vgui_controls/ImageButton.h"
#include "vgui_controls/ListViewPanel.h"

#include "BasePanel.h"

#include "Task.h"
#include "httpclient.h"
#include "FreeImage.h"
#include "NeteaseApi.h"

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
	void SetUserInfo(std::shared_ptr<netease::CMy> myinfo) {
		if (myinfo == nullptr) {
			m_pName->SetText("#GaemUI_ABC_Netease_NotLogin");
			m_pSignature->SetText("");
			m_pIsVip->SetText("");
			m_pLevelNum->SetText("");
			return;
		}
		static auto decodecallback = [](IUtilHTTPResponse* rep, ImagePanel* target, CNeteaseUserInfo* info) {
			const unsigned char* data = reinterpret_cast<const unsigned char*>(rep->GetPayload()->GetBytes());
			size_t size = rep->GetPayload()->GetLength();
			int ar = 0, ag = 0, ab = 0;
			FIMEMORY* mem = FreeImage_OpenMemory(const_cast<unsigned char*>(data), size);
			FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(mem);
			FIBITMAP* bitmap = FreeImage_LoadFromMemory(format, mem);
			byte* pixels = (byte*)FreeImage_GetBits(bitmap);
			size_t width = FreeImage_GetWidth(bitmap);
			size_t height = FreeImage_GetHeight(bitmap);
			size_t pitch = FreeImage_GetPitch(bitmap);
			size_t bpp = FreeImage_GetBPP(bitmap);
			size_t bitnum = bpp / 8;
			static byte* s_pBuf;
			static size_t s_Size;
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
		};
		GetHttpClient()->Fetch((myinfo->avatarurl + "?param=130y130").c_str(), UtilHTTPMethod::Get)->
			OnRespond(decodecallback, m_pAvatar, this)->
			Create(true)->
			Start();
		
		int w, h;
		GetSize(w, h);
		GetHttpClient()->Fetch((myinfo->backgroundurl + "?param=" + std::to_string(w) + "y" + std::to_string(h)).c_str(), UtilHTTPMethod::Get)->
			OnRespond(decodecallback, m_pBackgroud, this)->
			Create(true)->
			Start();

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
		GetTaskManager()->Add<std::shared_ptr<netease::CMy>>([](){
			return GetNeteaseApi().load()->GetMyself();
		})->ContinueWith([](std::shared_ptr<netease::CMy> info, CNeteaseUserInfo* pInfoPanel, CNeteaseUserButton* pButton) {
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
	SetScheme("OptionsAdvanceDialogScheme");
	m_pIcon = new ImagePanel(this, "TitleIcon");
	m_pTitle = new Label(this, "Title", "#GaemUI_ABC_Netease_Title");
	m_pUserInfo = new CNeteaseUserButton(this, "UserInfoButton");
	m_pDivideLine = new Panel(this, "Divide");
	m_pAlubmImage = new ImagePanel(this, "AlbumImage");
	m_pSongName = new Label(this, "SongName", "(song)");
	m_pSongTime = new Label(this, "SongTime", "(time)");
	m_pLyric = new Label(this, "LyricPanel", "(lyric)");
	m_pVolumn = new ImageButton(this, "VolumnButton", "abcenchance/tga/netease_volumn", this, "OpenVolumn");
	m_pPlay = new ImageButton(this, "PlayButton", "abcenchance/tga/netease_play", this, "PlayButton");
	m_pNext = new ImageButton(this, "NextButton", "abcenchance/tga/netease_next", this, "NextSong");
	m_pLast = new ImageButton(this, "LastButton", "abcenchance/tga/netease_last", this, "LastSong");
	m_pClose = new ImageButton(this, "CloseButton", "abcenchance/tga/button_close", this, "Close");
	m_pSideBar = new ListViewPanel(this, "SideBar");
	m_pSearchPage = new Panel(this, "SearchPage");
	LoadControlSettings("abcenchance/res/gameui/NeteaseMusicDialog.res");

	ImageList* img = new ImageList(true);
	KeyValues* kv = new KeyValues("search", "text", "#GaemUI_ABC_Netease_Search");
	kv->SetInt("image", img->AddImage(scheme()->GetImage("abcenchance/tga/netease_search", true)));
	m_pSideBar->AddItem(kv, false, false);
	m_pSideBar->SetImageList(img, true);

	SetTitleBarVisible(false);
	SetCloseButtonVisible(false);
	SetMenuButtonVisible(false);
}

vgui::CNeteaseMusicDialog::~CNeteaseMusicDialog(){
}

static CNeteaseMusicDialog* s_pNeteaseDialog;
void CreateNeteaseMusicDialogCmd() {
	gEngfuncs.pfnAddCommand("open_neteasedialog", []() {
		if (!s_pNeteaseDialog)
			s_pNeteaseDialog = new CNeteaseMusicDialog(reinterpret_cast<Panel*>(BasePanel()));
		int w, h;
		extern size_t ScreenWidth();
		extern size_t ScreenHeight();
		s_pNeteaseDialog->GetSize(w, h);
		s_pNeteaseDialog->SetPos((ScreenWidth() - w) / 2, (ScreenHeight() - h) / 2);
		s_pNeteaseDialog->MakePopup();
		s_pNeteaseDialog->Activate();
	});
}