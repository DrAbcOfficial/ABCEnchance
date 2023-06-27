#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/TextImageEx.h>
#include <vgui_controls/LabelEx.h>

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
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "TextMenuScheme.res", "TextMenuScheme");
	SetScheme("TextMenuScheme");
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
}
void CTextMenu::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("TextMenu.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
void CTextMenu::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
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
	m_pMenu->GetTextImage()->GetSize(w, h);
	SetWide(w + 4);
	SetTall(h + 4);
}

bool CTextMenu::MsgShowMenu(const char* pszName, int iSize, void* pbuf){
	m_szMenuString.clear();

	BEGIN_READ(pbuf, iSize);
	m_bitsValidSlots = READ_SHORT();
	int iDisplayTime = READ_CHAR();
	int iNeedMoreBits = READ_BYTE();
	int iNeedMore = iNeedMoreBits & 0x7F;
	m_bIsASMenu = ((iNeedMoreBits >> 7) & 1) ? true : false;

	if (iDisplayTime > 0)
		m_flShutoffTime = gEngfuncs.GetClientTime() + iDisplayTime;
	else
		m_flShutoffTime = -1;

	if (m_bitsValidSlots){
		m_szMenuString += READ_STRING();
		if (!iNeedMore) {
			SetContent(m_szMenuString.c_str());
			ShowPanel(true);
		}
	}
	else
		ShowPanel(false);
	return true;
}
