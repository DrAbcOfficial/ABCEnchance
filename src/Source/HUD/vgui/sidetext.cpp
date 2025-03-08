#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ImagePanel.h>

#include "local.h"
#include "vguilocal.h"

#include "sidetext.h"
#include "Viewport.h"

#include "plugins.h"
#include <exportfuncs.h>

#include "steamclientpublic.h"
#include <player_info.h>

extern vgui::HScheme GetViewPortBaseScheme();

#define VIEWPORT_SIDETEXT_NAME "SidePanel"
CSidePanel::CSidePanel()
	: BaseClass(nullptr, VIEWPORT_SIDETEXT_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());
	// Header labels
	m_pImage = new vgui::ImagePanel(this, "Image");
	m_pMessage = new vgui::Label(this, "Message", "");
	
	gCVars.pEccoEnable = CREATE_CVAR("hud_sideinfo", "1", FCVAR_VALUE, [](cvar_t* cvar){
		GetBaseViewPort()->ShowSideText(cvar->value > 0);
	});

	LoadControlSettings(VGUI2_ROOT_DIR "SideTextPanel.res");
}
const char* CSidePanel::GetName() {
	return VIEWPORT_SIDETEXT_NAME;
}
void CSidePanel::Reset() {
	if (IsVisible())
		ShowPanel(false);
	m_flMaxSpeed = 0;
}
void CSidePanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("SideText.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMessage->SetBgColor(GetSchemeColor("SideText.TextBgColor", GetSchemeColor("Label.BgColor", pScheme), pScheme));
	m_pMessage->SetFgColor(GetSchemeColor("SideText.TextFgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
}
void CSidePanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_szTeplate = inResourceData->GetString("template_text", NULL);
}
void CSidePanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CSidePanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CSidePanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CSidePanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}
void CSidePanel::OnThink() {
	if (!gClientData || !gEngfuncs.GetLocalPlayer())
		return;
	Vector vecSpeed = { gClientData->velocity[0] , gClientData->velocity[1] , 0 };
	float flPlayerSpeed = vecSpeed.Length();
	if (m_flMaxSpeed < flPlayerSpeed)
		m_flMaxSpeed = flPlayerSpeed;
	const auto ReplaceAll = [](std::string subject, const char* search,
		const std::string& replace) -> std::string {
			size_t pos = 0;
			while ((pos = subject.find(search, pos)) != std::string::npos) {
				subject.replace(pos, strlen(search), replace);
				pos += replace.length();
			}
			return subject;
	};
	std::string szTemp = m_szTeplate;
	szTemp = ReplaceAll(szTemp, "{SCORE}", std::to_string(CPlayerInfo::GetPlayerInfo(gEngfuncs.GetLocalPlayer()->index)->GetFrags()));
	szTemp = ReplaceAll(szTemp, "{MAXSPEED}", std::to_string(static_cast<int>(m_flMaxSpeed)));
	szTemp = ReplaceAll(szTemp, "{SPEED}", std::to_string(static_cast<int>(flPlayerSpeed)));

	m_pMessage->SetText(szTemp.c_str());
}