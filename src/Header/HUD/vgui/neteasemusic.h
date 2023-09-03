#ifndef CNETEASEMUSIC_H
#define CNETEASEMUSIC_H

#include <string>
#include "NeteaseApi.h"
#include "soundengine.h"
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

	virtual void OnThink() override;

	void Login();
	void ResetText();
	void CheckLogin();
private:
	vgui::ImagePanel* m_pQRImagePanel = nullptr;
	vgui::Label* m_pNotice = nullptr;

	float m_flNextCheckTime;
	struct loginshare_obj* m_pLoginObj = nullptr;
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

	virtual void OnThink() override;
	void PlayMusic(int id);
	void StopMusic();
	void QRLogin();
	void GetMyInfo();

	void SetVolume(float vol);
	void Search(const char* keyword, netease::SearchType type);
private:
	void PrintF(char* str);
	void PlayMusicFromBuffer(struct musicthread_obj* obj);

	cvar_t* m_pQuality = nullptr;
	cvar_t* m_pVolume = nullptr;
	cvar_t* m_pSearchCount = nullptr;

	vgui::Label* m_pMusicNameLable = nullptr;
	vgui::Label* m_pArtistNameLable = nullptr;
	vgui::Label* m_pLyricLable = nullptr;
	vgui::Label* m_pTranslatedLyricLable = nullptr;

	vgui::Label* m_pTimeLable = nullptr;
	vgui::Label* m_pMaxTimeLable = nullptr;
	vgui::ImageClipPanel* m_pProgressLable = nullptr;
	vgui::ImagePanel* m_pProgressBackgroundPanel = nullptr;

	vgui::ImagePanel* m_pAlbumPanel = nullptr;

	CQRLoginPanel* m_pLoginPanel = nullptr;

	FMOD_SOUND* m_pSound = nullptr;
	FMOD_CHANNEL* m_pChannel = nullptr;

	size_t m_flStartMusicTime = 0;

	netease::CMusic* m_pPlaying = nullptr;
	netease::CLyric* m_pLyric = nullptr;
	netease::CMy* m_pLogined = nullptr;
};
#endif