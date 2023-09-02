#pragma once
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/ImageClipPanel.h"

#include "local.h"
#include "vguilocal.h"

#include "Viewport.h"
#include "BaseUI.h"

#include "neteasemusic.h"

#define VIEWPORT_NETEASEMUSIC_NAME "NeteasePanel"
CNeteasePanel::CNeteasePanel()
	: BaseClass(nullptr, VIEWPORT_NETEASEMUSIC_NAME){
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
	m_pProgressLable = new vgui::ImageClipPanel(this, "Progress");
	m_pProgressBackgroundPanel = new vgui::ImagePanel(this, "ProgressBackground");

	m_pAlbumPanel = new vgui::ImagePanel(this, "Album");

	LoadControlSettings(VGUI2_ROOT_DIR "NeteasePanel.res");
	SetVisible(false);
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
	SetBgColor(GetSchemeColor("Vote.BackGoundColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
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