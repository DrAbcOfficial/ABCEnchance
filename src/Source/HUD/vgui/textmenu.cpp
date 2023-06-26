#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui/IBorder.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ListViewPanel.h>

#include "local.h"
#include "vguilocal.h"

#include "textmenu.h"
#include "Viewport.h"

#include "plugins.h"

#define VIEWPORT_TEXTMENU_NAME "TextMenu"
CTextMenu::CTextMenu()
	: BaseClass(nullptr, VIEWPORT_TEXTMENU_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "TextMenuScheme.res", "TextMenuScheme");
	SetScheme("TextMenuScheme");
	// Header labels
	m_pTitle = new vgui::Label(this, "Title", "");
	m_pMenu = new vgui::ListViewPanel(this, "Menu");

	LoadControlSettings(VGUI2_ROOT_DIR "TextMenuPanel.res");
}
const char* CTextMenu::GetName() {
	return VIEWPORT_TEXTMENU_NAME;
}
void CTextMenu::Reset() {
	m_pTitle->SetText("");
	m_pMenu->DeleteAllItems();
	m_flDisplayTime = 0;
}
void CTextMenu::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("TextMenu.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMenu->SetBgColor(GetSchemeColor("TextMenu.MenuBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMenu->SetBorder(pScheme->GetBorder("TextMenu.MenuBorder"));
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

void CTextMenu::AddMenu(char* szMenu){
	KeyValues* kv = new KeyValues("item");
	kv->SetString("text", szMenu);
	m_pMenu->AddItem(kv, false, false);
	kv->deleteThis();
}

void CTextMenu::SetTitle(char* szTitle){
	m_pTitle->SetText(szTitle);
}
