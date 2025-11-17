#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/HTML.h>

#include "local.h"
#include "vguilocal.h"

#include "motd.h"
#include "Viewport.h"

#include "plugins.h"

#define VIEWPORT_MOTD_NAME "MotdPanel"
extern vgui::HScheme GetViewPortBaseScheme();
CMotdPanel::CMotdPanel()
	: BaseClass(nullptr, VIEWPORT_MOTD_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());

	// Header labels
	m_pMessage = new vgui::Label(this, "Message", "");
	m_pProgressBar = new vgui::Panel(this, "Progress");
	m_pHTML = new vgui::HTML(this, "HTMLMessage", false, false);

	gCVars.pMotd = CREATE_CVAR("hud_motd", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		switch (static_cast<int>(cvar->value)) {
		case 1:
		case -1: GetBaseViewPort()->GetMotdPanel()->ShowPanel(false); break;
		default: break;
		}
		});

	LoadControlSettings(VGUI2_ROOT_DIR "MotdPanel.res");
	SetVisible(false);
}
CMotdPanel::~CMotdPanel() {
	RemoveTempHTML();
}
const char* CMotdPanel::GetName() {
	return VIEWPORT_MOTD_NAME;
}
void CMotdPanel::Reset() {
	if (IsVisible())
		ShowPanel(false);
	RemoveTempHTML();
	//m_pHTML->OpenURL("", nullptr);
	m_aryClipedPage.clear();
	m_szBuffer.clear();
	m_bLoadedMissionBrief = false;
	m_bInHTML = false;
	m_bBuiledPages = false;
	m_iNowPage = 0;
}
void CMotdPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("Motd.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMessage->SetBgColor(GetSchemeColor("Motd.MessageBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pProgressBar->SetBgColor(GetSchemeColor("Motd.ProgressBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
void CMotdPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	const char* labelText = inResourceData->GetString("keep_time", NULL);
	if (labelText)
		m_flKeepTime = std::stof(labelText);
	labelText = inResourceData->GetString("fade_time", NULL);
	if (labelText)
		m_flFadeTime = std::stof(labelText);
}
void CMotdPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	if (state)
		SetAlpha(255);
	SetVisible(state);
}
bool CMotdPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CMotdPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CMotdPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}
void CMotdPanel::OnThink() {
	float flTime = m_flShowTime - gEngfuncs.GetClientTime();
	if (flTime > 0)
		m_pProgressBar->SetWide(GetWide() * (flTime / m_flKeepTime));
	else if (m_iNowPage + 1 < m_aryClipedPage.size())
		ChangePage(m_iNowPage + 1);
	else {
		int a = GetAlpha();
		float flRatio = (flTime + m_flFadeTime) / m_flFadeTime;
		a *= flRatio;
		if (a <= 0)
			ShowPanel(false);
		else
			SetAlpha(a);
	}
}
void CMotdPanel::BuildPageInternal() {
	if (m_szBuffer.size() == 0)
		return;
	int w, h;
	m_pMessage->GetSize(w, h);
	int fh = vgui::surface()->GetFontTall(m_pMessage->GetFont());
	int iMaxClipSize = (w / (fh / 1.66f)) * (h / fh);
	if ((int)m_szBuffer.size() > iMaxClipSize) {
		for (size_t i = 0; i < m_szBuffer.size(); i += iMaxClipSize) {
			m_aryClipedPage.push_back(m_szBuffer.substr(i, iMaxClipSize));
		}
	}
	else
		m_aryClipedPage.push_back(m_szBuffer);
}
void CMotdPanel::BuildPage() {
	if (m_bBuiledPages)
		return;
	BuildPageInternal();
	m_szBuffer.clear();
	m_bBuiledPages = true;
}
#undef CLIP_MAX_SIZE
void CMotdPanel::AddBuffer(std::string szBuffer) {
	m_szBuffer += szBuffer;
}
void CMotdPanel::AppendMotd(const char* message) {
	AddBuffer(message);
}
void CMotdPanel::ChangePage(size_t iNewPage) {
	;
	m_flShowTime = gEngfuncs.GetClientTime() + m_flKeepTime;
	m_iNowPage = iNewPage;
	m_pMessage->SetText(m_aryClipedPage[m_iNowPage].c_str());
}
void CMotdPanel::ShowMotd() {
	if (gCVars.pMotd->value < 1)
		return;
	if (m_bInHTML) {
		m_flShowTime = gEngfuncs.GetClientTime() + m_flKeepTime;
		ShowPanel(true);
		return;
	}
	LoadMissionBrief();
	BuildPage();
	if (m_aryClipedPage.size() > 0) {
		ChangePage(0);
		ShowPanel(true);
	}
}
void CMotdPanel::FinishSendMOTD() {
	//check html
	if (m_szBuffer.starts_with("<!DOCTYPE html>")) {
		FileHandle_t file = vgui::filesystem()->Open("abcenchance/motd_cache.html", "w+");
		if (file)
			vgui::filesystem()->Write(m_szBuffer.c_str(), m_szBuffer.size(), file);
		vgui::filesystem()->Close(file);
		char local[MAX_PATH] = "file:///";
		const size_t uiURLLength = 8;
		vgui::filesystem()->GetLocalPath("abcenchance/motd_cache.html", local + uiURLLength, 252);
		m_pHTML->OpenURL(local, nullptr, true);
		m_pMessage->SetVisible(false);
		m_pHTML->SetVisible(true);
		m_bInHTML = true;
	}
	else if (m_szBuffer.starts_with("http://") || m_szBuffer.starts_with("https://")) {
		m_pHTML->OpenURL(m_szBuffer.c_str(), nullptr, true);
		m_pMessage->SetVisible(false);
		m_pHTML->SetVisible(true);
		m_bInHTML = true;
	}
	else {
		BuildPageInternal();
		m_pMessage->SetVisible(true);
		m_pHTML->SetVisible(false);
		m_bInHTML = false;
	}
	m_szBuffer.clear();
}
void CMotdPanel::RemoveTempHTML() {
	if (vgui::filesystem()->FileExists("abcenchance/motd_cache.html"))
		vgui::filesystem()->RemoveFile("abcenchance/motd_cache.html");
}
void CMotdPanel::LoadMissionBrief() {
	if (!m_bLoadedMissionBrief) {
		m_bLoadedMissionBrief = true;
		char buf[1024];
		std::string mappath = gEngfuncs.pfnGetLevelName();
		mappath += ".txt";
		if (!vgui::filesystem()->FileExists(mappath.c_str()))
			return;
		FileHandle_t file = vgui::filesystem()->Open(mappath.c_str(), "r");
		char* p = buf;
		std::string buffer;
		while (p && *p) {
			p = vgui::filesystem()->ReadLine(buf, 1023, file);
			buffer += buf;
		}
		AddBuffer(buffer);
		vgui::filesystem()->Close(file);
	}
}