#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/TextImageEx.h>
#include <vgui_controls/LabelEx.h>
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"

#include "textmenu.h"
#include "Viewport.h"

#include "plugins.h"
#include <parsemsg.h>

#define VIEWPORT_TEXTMENU_NAME "TextMenu"
CTextMenu::CTextMenu()
	: BaseClass(nullptr, VIEWPORT_TEXTMENU_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(g_pViewPort->GetBaseScheme());
	// Header labels
	m_pMenu = new vgui::LabelEx(this, "Menu", "");

	LoadControlSettings(VGUI2_ROOT_DIR "TextMenuPanel.res");
}
const char* CTextMenu::GetName() {
	return VIEWPORT_TEXTMENU_NAME;
}
void CTextMenu::Reset() {
	m_pMenu->SetText("");
	m_szMenuString.clear();
	m_flShutoffTime = -1;
	m_bWaitingForMore = false;
	m_bIsASMenu = false;
	SetAlpha(255);
	SetVisible(false);
}
void CTextMenu::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("TextMenu.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMenu->SetFgColor(GetSchemeColor("TextMenu.TextColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
}
void CTextMenu::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_flFadeAnimateTime = inResourceData->GetFloat("fade_time");
}
void CTextMenu::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CTextMenu::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CTextMenu::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CTextMenu::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

void CTextMenu::SetContent(const char* szMenu){
	m_pMenu->SetText(szMenu);
	int w, h;
	m_pMenu->GetTextImage()->GetContentSize(w, h);
	SetWide(w * 1.1f);
	SetTall(h * 1.1f);
	m_pMenu->SetPos(GetWide() * 0.05, GetTall() * 0.05);
}


void CTextMenu::StartFade(bool state){
	SetAlpha(state ? 0 : 255);
	ShowPanel(true);
	vgui::GetAnimationController()->RunAnimationCommand(this, "alpha", state ? 255 : 0, 0.0f, m_flFadeAnimateTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
}

void CTextMenu::OnThink(){
	if (GetAlpha() <= 0) {
		ShowPanel(false);
		return;
	}
	if (m_flShutoffTime >= 0 && gEngfuncs.GetClientTime() >= m_flShutoffTime) {
		StartFade(false);
		m_flShutoffTime = -1;
	}
}

void CTextMenu::SelectMenuItem(int slot){
	char szbuf[32];
	if ((slot > 0) && (m_bitsValidSlots & (1 << (slot - 1)))){
		sprintf(szbuf, m_bIsASMenu ? "as_menuselect %d\n" : "menuselect %d\n", slot);
		EngineClientCmd(szbuf);
		StartFade(false);
	}
}

bool CTextMenu::MsgShowMenu(const char* pszName, int iSize, void* pbuf){
	if (!m_bWaitingForMore)
		m_szMenuString.clear();

	BEGIN_READ(pbuf, iSize);
	m_bitsValidSlots = READ_SHORT();
	int iDisplayTime = READ_CHAR();
	int iNeedMoreBits = READ_BYTE();
	int iNeedMore = iNeedMoreBits & 0x7F;
	m_bIsASMenu = (iNeedMoreBits >> 7) & 1;
	m_flShutoffTime = iDisplayTime > 0 ? gEngfuncs.GetClientTime() + iDisplayTime : -1;
	if (m_bitsValidSlots){
		m_szMenuString += READ_STRING();
		if (!iNeedMore) {

			//Remove all \n from begin and end
			//someone will send a bunch of \n\n\n\n\n\n\n\n\n\n\n\n\n\n in the beginning, wtf?????
			auto searchFunc = [](char ch) {return ch != '\n';};
			m_szMenuString.erase(m_szMenuString.begin(), std::find_if(m_szMenuString.begin(), m_szMenuString.end(), searchFunc));
			m_szMenuString.erase(m_szMenuString.rbegin().base(), std::find_if(m_szMenuString.rbegin(), m_szMenuString.rend(), searchFunc).base());
			SetContent(m_szMenuString.c_str());
			StartFade(true);
		}
	}
	else
		StartFade(false);
	m_bWaitingForMore = iNeedMore;
	return true;
}
