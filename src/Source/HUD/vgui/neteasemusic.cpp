#pragma once
#include <metahook.h>

#include <thread>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/ImageClipPanel.h"
#include "VGUI2/avatar_image.h"

#include "local.h"
#include "vguilocal.h"

#include "Viewport.h"
#include "BaseUI.h"
#include "neteasemusic.h"
#include "FreeImage/FreeImage.h"
#include "curl.h"

#pragma comment(lib,"FreeImage/FreeImage.lib")

#define VIEWPORT_NETEASEMUSIC_NAME "NeteasePanel"

class CAlbumImage : public vgui::IImage
{
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

void CloudMusic() {
	int argcount = gEngfuncs.Cmd_Argc();
	switch (argcount)
	{
	case 0:
	case 1: {
		break;
	}
	case 2: {
		int id = std::atoi(gEngfuncs.Cmd_Argv(1));
		g_pViewPort->GetMusicPanel()->PlayMusic(id);
		break;
	}
	default:
		break;
	}
}
CNeteasePanel::CNeteasePanel()
	: BaseClass(nullptr, VIEWPORT_NETEASEMUSIC_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "NeteaseScheme.res", "NeteaseScheme");
	SetScheme("NeteaseScheme");

	m_pMusicNameLable = new vgui::Label(this, "MusicName", "Music");
	m_pArtistNameLable = new vgui::Label(this, "ArtistName", "Artist");
	m_pLyricLable = new vgui::Label(this, "Lyric", "Lyric");
	m_pTranslatedLyricLable = new vgui::Label(this, "TransLyric", "TransLyric");

	m_pTimeLable = new vgui::Label(this, "Timer", "0:00");
	m_pMaxTimeLable = new vgui::Label(this, "MaxTime", "99:99");
	m_pProgressBackgroundPanel = new vgui::ImagePanel(this, "ProgressBackground");
	m_pProgressLable = new vgui::ImageClipPanel(this, "Progress");

	m_pAlbumPanel = new vgui::ImagePanel(this, "Album");
	s_pAlbumImage = new CAlbumImage();

	LoadControlSettings(VGUI2_ROOT_DIR "NeteasePanel.res");
	SetVisible(false);

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

std::thread g_pDownloadThread;
std::atomic_bool g_pDownloadOK = false;
struct threadshare_obj
{
	std::shared_ptr <netease::CMusic> music;
	std::shared_ptr<netease::CLyric> lyric;
	int id;
	byte* album;
	size_t album_h;
	size_t album_w;
};
std::atomic<threadshare_obj*> g_pThreadObj;

void CNeteasePanel::OnThink() {
	if (g_pDownloadOK) {
		PlayMusicFromBuffer();
		g_pDownloadOK = false;
	}
	if (m_bPlaying) {
		float gap = gEngfuncs.GetClientTime() - m_flStartMusicTime;
		if (gap <= m_uiMusicLen) {
			char buffer[MAX_PATH];
			V_snprintf(buffer, "%d:%d", static_cast<size_t>(gap) / 60, static_cast<size_t>(gap) % 60);
			m_pTimeLable->SetText(buffer);
			float flRatio = gap / static_cast<float>(m_uiMusicLen);
			m_pProgressLable->SetWide(static_cast<float>(m_pProgressBackgroundPanel->GetWide()) * flRatio);
			//lyric
			threadshare_obj* obj = g_pThreadObj.load();
			if (obj->lyric->lyric.size() > 0) {
				for (auto iter = obj->lyric->lyric.rbegin(); iter != obj->lyric->lyric.rend(); iter++) {
					if (gap * 1000 >= (*iter)->time.count()) {
						m_pLyricLable->SetText((*iter)->text.c_str());
						break;
					}
				}
			}
			if (obj->lyric->tlyric.size() > 0) {
				for (auto iter = obj->lyric->tlyric.rbegin(); iter != obj->lyric->tlyric.rend(); iter++) {
					if (gap * 1000 >= (*iter)->time.count()) {
						m_pTranslatedLyricLable->SetText((*iter)->text.c_str());
						break;
					}
				}
			}
		}
		else {
			m_bPlaying = false;
		}
	}
}

size_t writefile(void* buffer, size_t size, size_t nmemb, void* user_p) {
	FILE* fp = (FILE*)user_p;
	size_t return_size = fwrite(buffer, size, nmemb, fp);
	return return_size;
}
CURLcode DownLoad(const std::string& url, std::string& filename) {
	const char* file_name = filename.c_str();
	FILE* fp = fopen(file_name, "wb");
	void* curl = curl_easy_init();
	CURLcode res = CURLE_FAILED_INIT;
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writefile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);
		res = curl_easy_perform(curl);
	}
	curl_easy_cleanup(curl);
	fclose(fp);
	return res;
}
void DonwloadMusic() {
	threadshare_obj* obj = g_pThreadObj.load();
	netease::CNeteaseMusicAPI s_Api;
	int id = obj->id;
	std::shared_ptr <netease::CMusic> music = s_Api.GetSongDetail(id);
	std::shared_ptr<netease::CLyric> lyric = s_Api.GetLyric(id);
	char buffer[MAX_PATH];
	vgui::filesystem()->GetLocalPath("abcenchance/", buffer, MAX_PATH);
	std::string buf = buffer;
	buf += "musictemp.mp3";
	DownLoad("https://music.163.com/song/media/outer/url?id=" + std::to_string(id) + ".mp3", buf);
	//Load Album
	buf = buffer;
	buf += "musictemp.jpg";
	DownLoad(music->al->picUrl + "?param=130y130", buf);
	FIBITMAP* bitmap = FreeImage_Load(FIF_JPEG, buf.c_str(), JPEG_DEFAULT);
	byte* pixels = (byte*)FreeImage_GetBits(bitmap);
	int width = FreeImage_GetWidth(bitmap);
	int height = FreeImage_GetHeight(bitmap);
	int pitch = FreeImage_GetPitch(bitmap);
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
			pixel += 3;
			s_pBuf[c * 4 + 0] = r;
			s_pBuf[c * 4 + 1] = g;
			s_pBuf[c * 4 + 2] = b;
			s_pBuf[c * 4 + 3] = 255;
			c++;
		}
		pixels -= pitch;
	}
	FreeImage_Unload(bitmap);

	std::string albump = buf;
	obj->music = music;
	obj->lyric = lyric;
	obj->album = s_pBuf;
	obj->album_h = height;
	obj->album_w = width;
	g_pDownloadOK = true;
}
void CNeteasePanel::PlayMusic(int id){
	threadshare_obj* obj = g_pThreadObj.load();
	if (obj == nullptr)
		g_pThreadObj = obj = new threadshare_obj();
	obj->id = id;
	g_pDownloadOK = false;
	g_pDownloadThread = std::thread(DonwloadMusic);
	g_pDownloadThread.detach();
	ShowPanel(true);
}
void CNeteasePanel::PlayMusicFromBuffer(){
	FModEngine::CFModSystem* soundSystem = FModEngine::GetSystem();
	if (m_pSound) {
		soundSystem->FreeSound(m_pSound);
		m_pSound = nullptr;
	}
	threadshare_obj* obj = g_pThreadObj.load();
	char buffer[MAX_PATH];
	vgui::filesystem()->GetLocalPath("abcenchance/musictemp.mp3", buffer, MAX_PATH);
	soundSystem->CreateSound(buffer, FMOD_DEFAULT, 0, &m_pSound);
	FMOD_CHANNEL* channel;
	soundSystem->PlaySound(FMOD_CHANNEL_FREE, m_pSound, 0, &channel);
	s_pAlbumImage->InitFromRGBA(obj->album, obj->album_w, obj->album_h);
	m_pAlbumPanel->SetImage(s_pAlbumImage);

	m_pMusicNameLable->SetText(obj->music->name.c_str());
	std::string buf = obj->music->ar[0]->name.c_str();
	buf += " - ";
	buf += obj->music->al->name;
	m_pArtistNameLable->SetText(buf.c_str());

	soundSystem->GetLength(m_pSound, &m_uiMusicLen, FMOD_TIMEUNIT_MS);
	m_uiMusicLen /= 1000;
	V_snprintf(buffer, "%d:%d", m_uiMusicLen / 60, m_uiMusicLen % 60);
	m_pMaxTimeLable->SetText(buffer);

	m_flStartMusicTime = gEngfuncs.GetClientTime();
	m_bPlaying = true;
}