#pragma once
#include <metahook.h>

#include <future>
#include <chrono>
#include <atomic>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/ImageClipPanel.h"
#include "vgui_controls/Button.h"
#include "VGUI2/avatar_image.h"

#include "local.h"
#include "vguilocal.h"

#include "Viewport.h"
#include "BaseUI.h"
#include "neteasemusic.h"
#include "FreeImage/FreeImage.h"
#include "curl.h"
#include "qrcodegen.h"

#pragma comment(lib,"FreeImage/FreeImage.lib")

#define VIEWPORT_NETEASEMUSIC_NAME "NeteasePanel"
#define VIEWPORT_NETEASEMUSICQR_NAME "NeteaseQRPanel"

class CAlbumImage : public vgui::IImage {
public:
	CAlbumImage() {
		m_nX = 0;
		m_nY = 0;
		m_wide = m_tall = 0;
		m_Color = Color(255, 255, 255, 255);
		m_iTextureID = -1;
	}
	// Call to Paint the image
	// Image will draw within the current panel context at the specified position
	virtual void Paint(void) {
		int posX = m_nX + m_offX;
		int posY = m_nY + m_offY;
		if (m_bValid)
		{
			vgui::surface()->DrawSetTexture(m_iTextureID);
			vgui::surface()->DrawSetColor(m_Color);
			vgui::surface()->DrawTexturedRect(posX, posY, posX + m_wide, posY + m_tall);
		}
	}
	// Set the position of the image
	virtual void SetPos(int x, int y)
	{
		m_nX = x;
		m_nY = y;
	}
	virtual void SetOffset(int x, int y)
	{
		m_offX = x;
		m_offY = y;
	}
	// Gets the size of the content
	virtual void GetContentSize(int& wide, int& tall)
	{
		wide = m_wide;
		tall = m_tall;
	}
	// Get the size the image will actually draw in (usually defaults to the content size)
	virtual void GetSize(int& wide, int& tall)
	{
		GetContentSize(wide, tall);
	}
	// Sets the size of the image
	virtual void SetSize(int wide, int tall) {
		m_wide = wide;
		m_tall = tall;
	}
	// Set the draw color
	virtual void SetColor(Color col)
	{
		m_Color = col;
	}
	bool IsValid() { return m_bValid; }
	int GetWide() { return m_wide; }
	int GetTall() { return m_tall; }
	void InitFromRGBA(const byte* rgba, int width, int height) {
		if(m_iTextureID <= 0)
			m_iTextureID = vgui::surface()->CreateNewTextureID(true);
		vgui::surface()->DrawSetTextureRGBA(m_iTextureID, rgba, width, height, true, false);
		m_wide = width;
		m_tall = height;
		m_bValid = true;
	}
private:
	Color m_Color;
	int m_iTextureID;
	int m_wide, m_tall;
	bool m_bValid;
	int m_nX, m_nY;
	int m_offX = 0, m_offY = 0;
};
static CAlbumImage* s_pAlbumImage;
static std::atomic<netease::CNeteaseMusicAPI*> s_pNeteaseApi;

const static netease::SearchType s_arySearchTypeTable[] = {
	netease::ST_SONG,
	netease::ST_ALBUM,
	netease::ST_ARTIST,
	netease::ST_PLAYLIST,
	netease::ST_USER,
};
void CloudMusic() {
	int argcount = gEngfuncs.Cmd_Argc();
	switch (argcount)
	{
	case 0: 
	case 1:{
		//TODO: Help Info here
		break;
	}
	case 2: {
		char* subcmd = gEngfuncs.Cmd_Argv(1);
		if (!V_strcmp(subcmd, "login")) {
			g_pViewPort->GetMusicPanel()->QRLogin();
			return;
		}
		else if (!V_strcmp(subcmd, "stop")) {
			g_pViewPort->GetMusicPanel()->StopMusic();
			return;
		}
		else if (!V_strcmp(subcmd, "next")) {
			g_pViewPort->GetMusicPanel()->NextMusic();
			return;
		}
		break;
	}
	case 3: {
		char* subcmd = gEngfuncs.Cmd_Argv(1);
		if (!V_strcmp(subcmd, "music")) {
			char* end;
			netease::neteaseid_t id = std::strtoull(gEngfuncs.Cmd_Argv(2), &end, 10);
			g_pViewPort->GetMusicPanel()->PlayMusic(id);
			return;
		}
		else if (!V_strcmp(subcmd, "playlist")) {
			char* end;
			netease::neteaseid_t id = std::strtoull(gEngfuncs.Cmd_Argv(2), &end, 10);
			g_pViewPort->GetMusicPanel()->PlayList(id);
			return;
		}
		break;
	}
	case 4: {
		char* subcmd = gEngfuncs.Cmd_Argv(1);
		if (!V_strcmp(subcmd, "search")) {
			char* type = gEngfuncs.Cmd_Argv(2);
			char* keyword = gEngfuncs.Cmd_Argv(3);
			g_pViewPort->GetMusicPanel()->Search(keyword, s_arySearchTypeTable[std::clamp(std::stoi(type), 0, 4)]);
			return;
		}
	}
	}
}
CNeteasePanel::CNeteasePanel()
	: BaseClass(nullptr, VIEWPORT_NETEASEMUSIC_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "NeteaseScheme.res", "NeteaseScheme");
	SetScheme("NeteaseScheme");

	m_pMusicNameLable = new vgui::Label(this, "MusicName", "");
	m_pArtistNameLable = new vgui::Label(this, "ArtistName", "");
	m_pLyricLable = new vgui::Label(this, "Lyric", "");
	m_pTranslatedLyricLable = new vgui::Label(this, "TransLyric", "");

	m_pTimeLable = new vgui::Label(this, "Timer", "");
	m_pMaxTimeLable = new vgui::Label(this, "MaxTime", "");
	m_pProgressBackgroundPanel = new vgui::ImagePanel(this, "ProgressBackground");
	m_pProgressLable = new vgui::ImageClipPanel(this, "Progress");

	m_pAlbumPanel = new vgui::ImagePanel(this, "Album");
	m_pLoginPanel = new CQRLoginPanel(this->GetParent(), VIEWPORT_NETEASEMUSICQR_NAME);
	s_pAlbumImage = new CAlbumImage();

	m_pQuality = CREATE_CVAR("cl_netease_quality", "0", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = std::clamp<int>(cvar->value, 0, 4);
	});
	m_pVolume = CREATE_CVAR("cl_netease_volume", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = std::clamp<float>(cvar->value, 0.0f, 1.0f);
		g_pViewPort->GetMusicPanel()->SetVolume(cvar->value);
	});
	m_pSearchCount = CREATE_CVAR("cl_netease_searchcount", "5", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = std::max<int>(cvar->value, 1);
	});

	s_pNeteaseApi = new netease::CNeteaseMusicAPI();
	char buf[MAX_PATH];
	vgui::filesystem()->GetLocalPath("abcenchance/", buf, MAX_PATH);
	V_snprintf(buf, "%sneteasecookie", buf);
	netease::CNeteaseMusicAPI::SetCookie(buf);

	LoadControlSettings(VGUI2_ROOT_DIR "NeteasePanel.res");
	SetVisible(false);

	GetMyInfo();

	ADD_COMMAND("cloudmusic", CloudMusic);
}
const char* CNeteasePanel::GetName(){
	return VIEWPORT_NETEASEMUSIC_NAME;
}
void CNeteasePanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
}
void CNeteasePanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("Music.BackGoundColor", GetSchemeColor("Frame.BgColor", pScheme), pScheme));
	m_pMusicNameLable->SetFgColor(GetSchemeColor("Music.MusicFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pArtistNameLable->SetFgColor(GetSchemeColor("Music.ArtistFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pTimeLable->SetFgColor(GetSchemeColor("Music.NumberFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pMaxTimeLable->SetFgColor(GetSchemeColor("Music.NumberFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pLyricLable->SetFgColor(GetSchemeColor("Music.LyricFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pTranslatedLyricLable->SetFgColor(GetSchemeColor("Music.TransLyricFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pLoginPanel->SetBgColor(GetSchemeColor("Music.BackGoundColor", GetSchemeColor("Frame.BgColor", pScheme), pScheme));
}
void CNeteasePanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	SetKeyBoardInputEnabled(state);
	SetVisible(state);
}
bool CNeteasePanel::IsVisible(){
	return BaseClass::IsVisible();
}
vgui::VPANEL CNeteasePanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CNeteasePanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}

struct musicthread_obj{
	std::shared_ptr <netease::CMusic> music;
	std::shared_ptr<netease::CLyric> lyric;
	int id;
	std::vector<byte> musicData;
	byte* album;
	size_t album_h;
	size_t album_w;
};
std::future<musicthread_obj*> g_pMusicAsync;

struct loginshare_obj {
	byte* qrimagebyte;
	size_t size;
	std::string qrkey;
};
std::future<loginshare_obj*> g_pLoginAsync;

std::future<netease::CMy*> g_pUserAsync;

void CNeteasePanel::Think() {
	if (g_pUserAsync.valid() && g_pUserAsync.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
		m_pLogined = g_pUserAsync.get();
		char buffer[512];
		if (m_pLogined != nullptr) 
			V_snprintf(buffer, "User %llu Logined.", m_pLogined->id);
		else
			V_snprintf(buffer, "User not login.");
		PrintF(buffer);
	}
	if (g_pMusicAsync.valid() && g_pMusicAsync.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		PlayMusicFromBuffer(g_pMusicAsync.get());
	if (m_pPlaying != nullptr) {
		ShowPanel(true);
		size_t pos;
		FModEngine::GetSystem()->GetPosition(m_pChannel, &pos, FMOD_TIMEUNIT_MS);
		if (pos < m_pPlaying->duration) {
			char buffer[MAX_PATH];
			V_snprintf(buffer, "%02d:%02d", pos / 60000, pos % 60000 / 1000);
			m_pTimeLable->SetText(buffer);
			float flRatio = static_cast<float>(pos) / static_cast<float>(m_pPlaying->duration);
			m_pProgressLable->SetWide(static_cast<float>(m_pProgressBackgroundPanel->GetWide()) * flRatio);
			//lyric
			if (m_pLyric->lyric.size() > 0) {
				for (auto iter = m_pLyric->lyric.rbegin(); iter != m_pLyric->lyric.rend(); iter++) {
					if (pos >= (*iter)->time.count()) {
						m_pLyricLable->SetText((*iter)->text.c_str());
						break;
					}
				}
			}
			else
				m_pLyricLable->SetText("");
			if (m_pLyric->tlyric.size() > 0) {
				for (auto iter = m_pLyric->tlyric.rbegin(); iter != m_pLyric->tlyric.rend(); iter++) {
					if (pos >= (*iter)->time.count()) {
						m_pTranslatedLyricLable->SetText((*iter)->text.c_str());
						break;
					}
				}
			}
			else
				m_pTranslatedLyricLable->SetText("");
		}
		else
			ChangeMusic();
	}
}
size_t append(void* ptr, size_t size, size_t nmemb, void* user) {
	std::vector<byte>* p = (std::vector<byte>*)user;
	auto cs = p->size();
	p->resize(cs + size * nmemb);
	memcpy(p->data() + cs, ptr, size * nmemb);
	return size * nmemb;
}
std::vector<byte> DownLoad(const std::string& url) {
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

const char* g_aryMusicQuality[] = {
	"standard",
	"higher",
	"exhigh",
	"lossless",
	"hires"
};
musicthread_obj* DonwloadMusic(netease::neteaseid_t id, size_t quality) {
	static musicthread_obj obj;
	//Load Music
	std::shared_ptr <netease::CMusic> music = s_pNeteaseApi.load()->GetSongDetail(id);
	if (music == nullptr)
		return nullptr;
	obj.musicData = DownLoad(music->GetPlayUrl(g_aryMusicQuality[quality], "flac"));
	//Load Album
	std::vector<byte> imageData = DownLoad(music->al->picUrl + "?param=130y130");
	FIMEMORY* mem = FreeImage_OpenMemory(imageData.data(), imageData.size());
	FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(mem);
	FIBITMAP* bitmap = FreeImage_LoadFromMemory(format, mem);
	byte* pixels = (byte*)FreeImage_GetBits(bitmap);
	int width = FreeImage_GetWidth(bitmap);
	int height = FreeImage_GetHeight(bitmap);
	int pitch = FreeImage_GetPitch(bitmap);
	int bpp = FreeImage_GetBPP(bitmap);
	static size_t s_iArea;
	static byte* s_pBuf;
	if (s_iArea < width * height) {
		s_iArea = width * height;
		delete[] s_pBuf;
		s_pBuf = new byte[s_iArea * 4];
	}
	size_t c = 0;
	pixels += pitch * (height - 1);
	for (int y = 0; y < height; y++) {
		BYTE* pixel = (BYTE*)pixels;
		for (int x = 0; x < width; x++) {
			BYTE r = pixel[FI_RGBA_RED];
			BYTE g = pixel[FI_RGBA_GREEN];
			BYTE b = pixel[FI_RGBA_BLUE];
			s_pBuf[c * 4 + 0] = r;
			s_pBuf[c * 4 + 1] = g;
			s_pBuf[c * 4 + 2] = b;
			//Jpeg with alpha? weird
			if (bpp == 32) {
				BYTE a = pixel[FI_RGBA_ALPHA];
				s_pBuf[c * 4 + 3] = a;
				pixel += 4;
			}
			else {
				s_pBuf[c * 4 + 3] = 255;
				pixel += 3;
			}
			c++;
		}
		pixels -= pitch;
	}
	FreeImage_Unload(bitmap);
	FreeImage_CloseMemory(mem);

	obj.music = music;
	obj.lyric = s_pNeteaseApi.load()->GetLyric(id);
	obj.album = s_pBuf;
	obj.album_h = height;
	obj.album_w = width;
	return &obj;
}
void CNeteasePanel::PlayMusic(netease::neteaseid_t  id){
	m_aryPlayList.clear();
	AddToList(id);
	PlayListMusic();
}
void CNeteasePanel::PlayList(netease::neteaseid_t id){
	m_aryPlayList.clear();
	std::shared_ptr<netease::CPlayList> list = s_pNeteaseApi.load()->GetPlayList(id);
	for (auto iter = list->mucics.begin(); iter != list->mucics.end(); iter++) {
		AddToList(*iter);
	}
	PlayListMusic();
}
void CNeteasePanel::StopMusic(){
	FModEngine::CFModSystem* soundSystem = FModEngine::GetSystem();
	if (m_pSound) {
		soundSystem->StopSound(m_pChannel);
		soundSystem->FreeSound(m_pSound);
		m_pSound = nullptr;
		m_pChannel = nullptr;
		m_pPlaying = nullptr;
	}
}
void CNeteasePanel::NextMusic(){
	ChangeMusic();
}
void CNeteasePanel::PrintF(char* str){
	char buffer[2048];
	V_snprintf(buffer, "[NeteaseApi] %s\n", str);
	gEngfuncs.Con_DPrintf(buffer);
}
void CNeteasePanel::PlayMusicFromBuffer(musicthread_obj* obj){
	if (obj == nullptr) {
		PrintF("Can not play this song!");
		return;
	}
	if (obj->music->copyright < 2 || (m_pLogined != nullptr && m_pLogined->vip > 0)) {
		//Music
		FModEngine::CFModSystem* soundSystem = FModEngine::GetSystem();
		FMOD_CREATESOUNDEXINFO extrainfo = {};
		extrainfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		extrainfo.length = obj->musicData.size();
		soundSystem->CreateStream(reinterpret_cast<const char*>(obj->musicData.data()), FMOD_HARDWARE | FMOD_OPENMEMORY, &extrainfo, &m_pSound);
		soundSystem->PlaySound(FMOD_CHANNEL_FREE, m_pSound, 0, &m_pChannel);
		soundSystem->SetVolume(m_pChannel, m_pVolume->value);
		//Album
		s_pAlbumImage->InitFromRGBA(obj->album, obj->album_w, obj->album_h);
		m_pAlbumPanel->SetImage(s_pAlbumImage);
		//Set Lyric
		m_pLyric = obj->lyric.get();
		//Text
		m_pMusicNameLable->SetText(obj->music->name.c_str());
		std::string buf = obj->music->ar[0]->name.c_str();
		buf += " - ";
		buf += obj->music->al->name;
		m_pArtistNameLable->SetText(buf.c_str());

		size_t len = obj->music->duration / 1000;
		static char buffer[256];
		V_snprintf(buffer, "%02d:%02d", len / 60, len % 60);
		m_pMaxTimeLable->SetText(buffer);
		m_pPlaying = obj->music.get();
	}
	else
		PrintF("VIP song, skipped.");
}
void CNeteasePanel::PlayListMusic(){
	if (m_aryPlayList.size() == 0)
		return;
	if (m_pPlaying != nullptr)
		StopMusic();
	netease::neteaseid_t id = *m_aryPlayList.begin();
	m_aryPlayList.pop_front();
	g_pMusicAsync = std::async(DonwloadMusic, id, static_cast<size_t>(m_pQuality->value));
	ShowPanel(true);
}
void CNeteasePanel::AddToList(netease::neteaseid_t id){
	m_aryPlayList.push_back(id);
}
void CNeteasePanel::ChangeMusic(){
	if (m_aryPlayList.size() == 0) {
		StopMusic();
		ShowPanel(false);
	}
	else
		PlayListMusic();
}
void CNeteasePanel::QRLogin() {
	m_pLoginPanel->ResetText();
	m_pLoginPanel->Login();
}
void CNeteasePanel::GetMyInfo(){
	g_pUserAsync = std::async([]() {
		return s_pNeteaseApi.load()->GetMyself().get();
	});
}

void CNeteasePanel::SetVolume(float vol){
	if (m_pChannel) {
		FModEngine::CFModSystem* soundSystem = FModEngine::GetSystem();
		soundSystem->SetVolume(m_pChannel, vol);
	}
}

void CNeteasePanel::Search(const char* keyword, netease::SearchType type){
	auto result = s_pNeteaseApi.load()->Search(keyword, type, m_pSearchCount->value, 0);
	char buffer[256];
	switch (type){
	case netease::ST_SONG: ConsoleWriteline("Song Result: \n");break;
	case netease::ST_ALBUM: ConsoleWriteline("Album Result: \n"); break;
	case netease::ST_ARTIST: ConsoleWriteline("Artist Result: \n"); break;
	case netease::ST_PLAYLIST: ConsoleWriteline("Playlist Result: \n"); break;
	case netease::ST_USER: ConsoleWriteline("User Result: \n"); break;
	}
	for (auto iter = result.begin(); iter != result.end(); iter++) {
		netease::CSearchResult* ret = (*iter).get();
		if(ret->extra.size() > 0)
			V_snprintf(buffer, "#[%llu] %s - %s\n", ret->id, ret->name.c_str(), ret->extra.c_str());
		else
			V_snprintf(buffer, "#[%llu] %s\n", ret->id, ret->name.c_str());
		ConsoleWriteline(buffer);
	}
}

/*
	QRLogin Panel
*/
#define CHECK_LOGIN_INVERTV 1.33f
static CAlbumImage* s_pQRCodeImage;
CQRLoginPanel::CQRLoginPanel(vgui::Panel* parent, char* name) 
	: BaseClass(parent, name) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetTitleBarVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetScheme("NeteaseScheme");

	m_pNotice = new vgui::Label(this, "QRNotice", "#Netease_QRNoticeText");
	m_pNotice->SetMouseInputEnabled(true);
	m_pQRImagePanel = new vgui::ImagePanel(this, "QRImage");
	m_pQRImagePanel->SetMouseInputEnabled(true);
	s_pQRCodeImage = new CAlbumImage();

	LoadControlSettings(VGUI2_ROOT_DIR "NeteaseQRPanel.res");
}

std::future<netease::CLocalUser::QRStatue> g_pGetCookieAsync;
void CQRLoginPanel::OnThink(){
	if (g_pLoginAsync.valid() && g_pLoginAsync.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
		loginshare_obj* obj = g_pLoginAsync.get();
		s_pQRCodeImage->InitFromRGBA(obj->qrimagebyte, obj->size, obj->size);
		m_pQRImagePanel->SetImage(s_pQRCodeImage);
		m_pLoginObj = obj;
		CheckLogin();
		SetMouseInputEnabled(true);
	}
	if (g_pGetCookieAsync.valid() && g_pGetCookieAsync.wait_for(std::chrono::seconds(0)) == std::future_status::ready && 
		gEngfuncs.GetClientTime() > m_flNextCheckTime) {
		netease::CLocalUser::QRStatue result = g_pGetCookieAsync.get();
		switch (result){
			case netease::CLocalUser::QRStatue::INVALID: {
				m_pMusicPanel->PrintF("Login Failed!");
				SetVisible(false);
				break;
			}
			case netease::CLocalUser::QRStatue::WAITINGSCAN:
			case netease::CLocalUser::QRStatue::AUTHORIZING: {
				CheckLogin();
				break;
			}
			case netease::CLocalUser::QRStatue::OK: {
				m_pLoginObj = nullptr;
				SetVisible(false);
				m_pMusicPanel->GetMyInfo();
				break;
			}
		}
		m_flNextCheckTime = gEngfuncs.GetClientTime() + CHECK_LOGIN_INVERTV;
	}
}
void CQRLoginPanel::Login(){
	g_pLoginAsync = std::async([]() -> loginshare_obj* {
		static loginshare_obj obj;
		obj.qrkey = s_pNeteaseApi.load()->GetUser()->RequestQRKey();
		std::string url = "https://music.163.com/login?codekey=" + obj.qrkey;
		std::vector<qrcodegen::QrSegment> segs =
			qrcodegen::QrSegment::makeSegments(url.c_str());
		qrcodegen::QrCode qrcode = qrcodegen::QrCode::encodeSegments(
			segs, qrcodegen::QrCode::Ecc::HIGH, 20, 40, 2, true);
		static byte* s_qrbyte;
		static int s_size;

		int qrsize = qrcode.getSize();
		if (qrsize > s_size) {
			s_size = qrsize;
			delete[] s_qrbyte;
			s_qrbyte = new byte[s_size * s_size * 4];
		}
		size_t c = 0;
		for (int x = 0; x < qrsize; x++) {
			for (int y = 0; y < qrsize; y++) {
				bool p = qrcode.getModule(x, y);
				s_qrbyte[c * 4 + 0] = p ? 0 : 255;
				s_qrbyte[c * 4 + 1] = p ? 0 : 255;
				s_qrbyte[c * 4 + 2] = p ? 0 : 255;
				s_qrbyte[c * 4 + 3] = 255;
				c++;
			}
		}
		obj.qrimagebyte = s_qrbyte;
		obj.size = qrsize;
		return &obj;
		});
	m_flNextCheckTime = gEngfuncs.GetClientTime() + CHECK_LOGIN_INVERTV;
	SetVisible(true);
}
void CQRLoginPanel::ResetText(){
	m_pNotice->SetText("#Netease_QRNoticeText");
}

void CQRLoginPanel::CheckLogin(){
	g_pGetCookieAsync = std::async([](std::string unikey)->netease::CLocalUser::QRStatue {
		return s_pNeteaseApi.load()->GetUser()->QRCheck(unikey);
		}, m_pLoginObj->qrkey);
}