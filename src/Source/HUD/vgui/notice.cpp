#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>

#include "local.h"
#include "vguilocal.h"

#include "notice.h"
#include "Viewport.h"

#define VIEWPORT_NOTICETEXT_NAME "NoticePanel"
CNoticePanel::CNoticePanel(const char* szControlName)
	: BaseClass(nullptr, VIEWPORT_NOTICETEXT_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "NoticeScheme.res", "NoticeScheme");
	SetScheme("NoticeScheme");

	m_pMessage = new vgui::Label(this, "Message", "");

	char szControl[256];
	sprintf(szControl, VGUI2_ROOT_DIR "%s.res", szControlName);
	LoadControlSettings(szControl);
}
void CNoticePanel::ShowMessage(const char* message){
	m_pMessage->SetText(message);
	int w, h;
	m_pMessage->GetContentSize(w, h);
	if (m_bKeepCenter) {
		int x, y;
		GetPos(x, y);
		SetPos((ScreenWidth - w) / 2, y);
	}
	SetTall(h);
	m_pMessage->SetTall(h);
	m_flCloseTime = gEngfuncs.GetClientTime() + m_flKeepTime;
	ShowPanel(true);
}
const char* CNoticePanel::GetName() {
	return VIEWPORT_NOTICETEXT_NAME;
}
void CNoticePanel::Reset() {
	m_flCloseTime = 0;
	ShowPanel(false);
}
void CNoticePanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("Notice.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMessage->SetBgColor(GetSchemeColor("Notice.TextBgColor", GetSchemeColor("Label.BgColor", pScheme), pScheme));
	m_pMessage->SetFgColor(GetSchemeColor("Notice.TextFgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
}
void CNoticePanel::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
	m_flKeepTime = inResourceData->GetFloat("keep_time", 3.0f);
	m_bKeepCenter = inResourceData->GetBool("keep_center", false);
}
void CNoticePanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CNoticePanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CNoticePanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CNoticePanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

void CNoticePanel::OnThink(){
	float clTime = gEngfuncs.GetClientTime();
	if (m_flCloseTime > 0 && clTime > m_flCloseTime) {
		m_flCloseTime = 0;
		ShowPanel(false);
	}
}
