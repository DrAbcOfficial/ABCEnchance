#ifndef CNETEASEMUSIC_H
#define CNETEASEMUSIC_H

#include <string>
#include <list>
#include <memory>

#include "NeteaseApi.h"
#include "soundengine.h"
#include "liblrc/lrc_parser.h"

#include <vgui_controls/Frame.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
	class ImageClipPanel;
	class Button;
}
class CNeteasePanel;

class CQRLoginPanel : public vgui::Frame {
public:
	DECLARE_CLASS_SIMPLE(CQRLoginPanel, vgui::Frame);
	CQRLoginPanel(vgui::Panel* parent, char* name);
	void Login();
	void ResetText();
	void SendMyInfo();
private:
	vgui::ImagePanel* m_pQRImagePanel = nullptr;
	vgui::Label* m_pNotice = nullptr;
	CNeteasePanel* m_pMusicPanel = nullptr;
};

class CNeteasePanel : public vgui::EditablePanel, public IViewportPanel{
public:
	DECLARE_CLASS_SIMPLE(CNeteasePanel, vgui::EditablePanel);

	CNeteasePanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	//force think
	void Think();

	//Play Method
	void Search(const char* keyword, netease::SearchType type);
	void PlayMusic(netease::neteaseid_t id);
	void PlayList(netease::neteaseid_t id);
	void PlayDj(netease::neteaseid_t id);
	void PlayRadio(netease::neteaseid_t id);
	void PlayRecommendMusic();
	void PlayFM();
	
	//Music control
	void StopMusic();
	void NextMusic();
	void PauseMusic();
	bool IsPaused();
	void SetVolume(float vol);

	//User info
	void QRLogin();
	void EmailLogin(const char* mail, const char* passwd);
	void SendSMS(const char* phone, int country);
	void SMSLogin(const char* phone, const char* captcha, int country);
	void PhoneLogin(const char* phone, const char* passwd, int country);
	void GetMyInfo(bool silence = true);
	void SetMyInfo(std::shared_ptr<netease::CMy> info);

	template<typename... Args>
	static void PrintF(const char* str, bool dev, const Args&& ...args);

	//NEVER KEEP THEM IN A VARLUE!
	netease::CMusic* GetNowPlaying();
	netease::CMy* GetNowUser();

	enum PLAYTYPE {
		MUSIC,
		DJ
	};
private:
	enum class PLAYSTATE {
		NORMAL,
		FM,
		DJ
	};
	void SetPlayerState(PLAYSTATE state);
	void PlayListMusic();
	void RenewFM(bool play);

	void AddToList(netease::neteaseid_t id, CNeteasePanel::PLAYTYPE type = CNeteasePanel::PLAYTYPE::MUSIC);

	cvar_t* m_pQuality = nullptr;
	cvar_t* m_pVolume = nullptr;
	cvar_t* m_pSearchCount = nullptr;

	vgui::Label* m_pMusicNameLable = nullptr;
	vgui::Label* m_pArtistNameLable = nullptr;
	vgui::Label* m_pLyricLable = nullptr;
	vgui::Label* m_pTranslatedLyricLable = nullptr;
	vgui::Label* m_pLyricLableHighlight = nullptr;
	vgui::Label* m_pTranslatedLyricLableHighlight = nullptr;

	vgui::Label* m_pTimeLable = nullptr;
	vgui::Label* m_pMaxTimeLable = nullptr;
	vgui::ImageClipPanel* m_pProgressLable = nullptr;
	vgui::ImagePanel* m_pProgressBackgroundPanel = nullptr;

	vgui::ImagePanel* m_pAlbumPanel = nullptr;

	CQRLoginPanel* m_pLoginPanel = nullptr;

	FMOD_SOUND* m_pSound = nullptr;
	FMOD_CHANNEL* m_pChannel = nullptr;

	bool m_bRenewingFM = false;
	bool m_bPendingMusic = false;
	bool m_bPaused = false;

	std::shared_ptr<netease::CMusic> m_pPlaying = nullptr;
	std::unique_ptr<lrc::Lyrics> m_pLyric = nullptr;
	std::unique_ptr<lrc::Lyrics> m_pTransLyric = nullptr;
	std::shared_ptr<netease::CMy> m_pLogined = nullptr;

	PLAYSTATE m_pNowState;
	struct PlayItem {
		netease::neteaseid_t id;
		PLAYTYPE type;
	};
	std::list<std::shared_ptr<PlayItem>> m_aryPlayList;
};
#endif