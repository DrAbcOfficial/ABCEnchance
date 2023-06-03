#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>

#include "local.h"
#include "vguilocal.h"

#include "motd.h"
#include "Viewport.h"

#include "plugins.h"

#define VIEWPORT_MOTD_NAME "MotdPanel"
CMotdPanel::CMotdPanel()
	: BaseClass(nullptr, VIEWPORT_MOTD_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "MotdScheme.res", "MotdScheme");
	SetScheme("MotdScheme");

	// Header labels
	m_pMessage = new vgui::Label(this, "Message", "");
	m_pProgressBar = new vgui::Panel(this, "Progress");

	LoadControlSettings(VGUI2_ROOT_DIR "MotdPanel.res");
	SetVisible(false);
}
const char* CMotdPanel::GetName(){
	return VIEWPORT_MOTD_NAME;
}
void CMotdPanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
	m_aryClipedPage.clear();
	m_szBuffer.clear();
	m_bLoadedMissionBrief = false;
	m_bBuiledPages = false;
	m_iNowPage = 0;
}
void CMotdPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("Motd.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMessage->SetBgColor(GetSchemeColor("Motd.MessageBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pProgressBar->SetBgColor(GetSchemeColor("Motd.ProgressBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
void CMotdPanel::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
	const char* labelText = inResourceData->GetString("keep_time", NULL);
	if (labelText)
		m_flKeepTime = std::stof(labelText);
	labelText = inResourceData->GetString("fade_time", NULL);
	if (labelText)
		m_flFadeTime = std::stof(labelText);
}
void CMotdPanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	if (state)
		SetAlpha(255);
	SetVisible(state);
}
bool CMotdPanel::IsVisible(){
	return BaseClass::IsVisible();
}
vgui::VPANEL CMotdPanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CMotdPanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}
void CMotdPanel::OnThink(){
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
	if (m_szBuffer.size() > iMaxClipSize) {
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
void CMotdPanel::AppendMotd(char* message){
	AddBuffer(message);
}
void CMotdPanel::ChangePage(size_t iNewPage){;
	m_flShowTime = gEngfuncs.GetClientTime() + m_flKeepTime;
	m_iNowPage = iNewPage;
	m_pMessage->SetText(m_aryClipedPage[m_iNowPage].c_str());	
}
void CMotdPanel::ShowMotd(){
	LoadMissionBrief();
	BuildPage();
	if (m_aryClipedPage.size() > 0) {
		ChangePage(0);
		ShowPanel(true);
	}	
}
void CMotdPanel::ForceAddPage(){
	BuildPageInternal();
	m_szBuffer.clear();
}
void CMotdPanel::LoadMissionBrief(){
	if (!m_bLoadedMissionBrief) {
		m_bLoadedMissionBrief = true;
		char mappath[260];
		char buf[1024];
		sprintf(mappath, "%s", gEngfuncs.pfnGetLevelName());
		int len = strlen(mappath);
		mappath[len - 3] = 't';
		mappath[len - 2] = 'x';
		mappath[len - 1] = 't';
		if (!g_pFileSystem->FileExists(mappath))
			return;
		FileHandle_t file = g_pFileSystem->Open(mappath, "r");
		char* p = buf;
		std::string buffer;
		while (p && *p){
			p = g_pFileSystem->ReadLine(buf, 1023, file);
			buffer += buf;
		}
		AddBuffer(buffer);
		g_pFileSystem->Close(file);
	}
}