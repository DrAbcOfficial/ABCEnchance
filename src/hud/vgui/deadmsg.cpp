#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>

#include "vgui_controls/Panel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/TextImage.h"
#include <vgui_controls/AnimationController.h>

#include "utility/util.h"
#include "utility/vgui_util.h"
#include "hud/Viewport.h"

#include "deadmsg.h"

#define VIEWPORT_DEATHMSGPANEL_NAME "DeathMsgPanel"

using namespace vgui;
extern vgui::HScheme GetViewPortBaseScheme();

CDeathMsgItem::CDeathMsgItem(Panel* parent, const wchar_t* victim, const wchar_t* attacker, const wchar_t* inflictor, vgui::IScheme* scheme)
	: BaseClass(parent, "Item") {
	m_pBackGround = new Panel(this);
	m_pAttacker = new Label(this, "Attacker", attacker);
	std::wstring inf = L"[";
	inf += inflictor;
	inf += L"]";
	m_pInflictor = new Label(this, "Inflictor", inf.c_str());
	m_pVictim = new Label(this, "Victim", victim);

	m_pAttacker->SetContentAlignment(vgui::Label::a_east);
	m_pInflictor->SetContentAlignment(vgui::Label::a_center);
	m_pVictim->SetContentAlignment(vgui::Label::a_west);
	m_pBackGround->SetRoundedCorners(PANEL_ROUND_CORNER_ALL);
}
void CDeathMsgItem::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(Color(0, 0, 0, 0));
	m_pBackGround->SetBgColor(GetSchemeColor("DeathMsgPanel.ItemBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pAttacker->SetFgColor(GetSchemeColor("DeathMsgPanel.ItemAttackerColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_pInflictor->SetFgColor(GetSchemeColor("DeathMsgPanel.ItemInflictorColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_pVictim->SetFgColor(GetSchemeColor("DeathMsgPanel.ItemVictimColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));

	const char* font = pScheme->GetResourceString("DeathMsgPanel.Font");
	m_pAttacker->SetFont(pScheme->GetFont(font, IsProportional()));
	m_pInflictor->SetFont(pScheme->GetFont(font, IsProportional()));
	m_pVictim->SetFont(pScheme->GetFont(font, IsProportional()));
}
void CDeathMsgItem::PerformLayout() {
	BaseClass::PerformLayout();
	int wv, hv;
	m_pVictim->GetTextImage()->GetContentSize(wv, hv);
	m_pVictim->SetSize(wv, hv);
	int wi, hi;
	m_pInflictor->GetTextImage()->GetContentSize(wi, hi);
	m_pInflictor->SetSize(wi, hi);
	int wa, ha;
	m_pAttacker->GetTextImage()->GetContentSize(wa, ha);
	m_pAttacker->SetSize(wa, ha);
	int w, h;;
	GetSize(w, h);
	m_pVictim->SetPos(w - wv, 0);
	m_pInflictor->SetPos(w - wv - wi - 1, 0);
	m_pAttacker->SetPos(w - wv - wi - wa - 2, 0);

	m_pBackGround->SetSize(wv + wi + wa + 2, h);
	m_pBackGround->SetPos(w - wv - wi - wa - 2, 0);
}
CDeathMsgPanel::CDeathMsgPanel()
	: BaseClass(nullptr, VIEWPORT_DEATHMSGPANEL_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());

	LoadControlSettings(VGUI2_ROOT_DIR "DeathMsgPanel.res");
	SetVisible(false);

	gCVars.pDeathNotice = CREATE_CVAR("hud_deathnotice", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		GetBaseViewPort()->ShowDeathMsg(cvar->value > 0);
	});
	gCVars.pDeathNoticeTime = CREATE_CVAR("hud_deathnotice_time", "6", FCVAR_VALUE, nullptr);
	m_pDeathMsgCount = CREATE_CVAR("hud_deathnotice_count", "6", FCVAR_VALUE, nullptr);
}
const char* CDeathMsgPanel::GetName(){
	return VIEWPORT_DEATHMSGPANEL_NAME;
}
void CDeathMsgPanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
	for (auto iter = m_aryDeath.rbegin(); iter != m_aryDeath.rend(); iter++) {
		(*iter)->DeletePanel();
	}
	m_aryDeath.clear();
}
void CDeathMsgPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("DeathMsgPanel.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pScheme = pScheme;
}
void CDeathMsgPanel::PerformLayout(){
	BaseClass::PerformLayout();
	if (m_aryDeath.size() == 0)
		return;
	int w, h;
	GetSize(w, h);
	int itemh = h / m_pDeathMsgCount->value;
	int y = h - itemh;
	for (auto iter = m_aryDeath.rbegin(); iter != m_aryDeath.rend(); iter++) {
		if ((*iter)->IsVisible()) {
			(*iter)->SetSize(w, itemh);
			(*iter)->SetPos(0, y);
			y -= itemh + vgui::scheme()->GetProportionalScaledValue(1);
		}
	}
}
void CDeathMsgPanel::AddItem(const wchar_t* victim, const wchar_t* attacker, const wchar_t* inflictor){
	ShowPanel(true);
	CDeathMsgItem* item = new CDeathMsgItem(this, victim, attacker, inflictor, m_pScheme);
	if (m_aryDeath.size() >= m_pDeathMsgCount->value) {
		m_aryDeath.front()->DeletePanel();
		m_aryDeath.erase(m_aryDeath.begin());
	}
	GetAnimationController()->RunAnimationCommand(item, "alpha", 0, gCVars.pDeathNoticeTime->value, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	m_aryDeath.push_back(item);
	char cl[256], cv[256], ce[256], ck[256];
	Q_UnicodeToUTF8(vgui::localize()->Find("DeathMsg_ConsolePrint"), cl, sizeof(cl));
	Q_UnicodeToUTF8(victim, cv, sizeof(cv));
	Q_UnicodeToUTF8(inflictor, ce, sizeof(ce));
	Q_UnicodeToUTF8(attacker, ck, sizeof(ck));
	char buffer[256 * 4];
	Q_snprintf(buffer, cl, cv, ck[0] == '\0' ? "Something" : ck, ce);
	gEngfuncs.Con_Printf(buffer);
	InvalidateLayout();
}
void CDeathMsgPanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CDeathMsgPanel::IsVisible(){
	return BaseClass::IsVisible();
}
vgui::VPANEL CDeathMsgPanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CDeathMsgPanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}