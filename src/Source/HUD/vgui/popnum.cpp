#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>

#include "local.h"
#include "vguilocal.h"

#include "popnum.h"
#include "Viewport.h"

#include "plugins.h"

#define VIEWPORT_POPNUMBER_NAME "PopNumberPanel"

#define POPNUMBER_O_TOKEN "PopNumber_O"
#define POPNUMBER_K_TOKEN "PopNumber_K"
#define POPNUMBER_K 1000
#define POPNUMBER_M_TOKEN "PopNumber_M"
#define POPNUMBER_M 1000000
#define POPNUMBER_B_TOKEN "PopNumber_B"
#define POPNUMBER_B 100000000

CPopNumberPanel::CPopNumberPanel(int x, int y, Color& pColor, int value)
	: BaseClass(nullptr, VIEWPORT_POPNUMBER_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "PopNumerScheme.res", "PopNumerScheme");
	SetScheme("PopNumerScheme");
	// Header labels
	m_pNumberLable = new vgui::Label(this, "PopNumber", "");
	m_pNumberLable->SetFgColor(pColor);
	m_iValue = value;
	std::string token;
	if (m_iValue > POPNUMBER_B) {
		m_iValue /= POPNUMBER_B;
		token = POPNUMBER_B_TOKEN;
	}
	else if (m_iValue > POPNUMBER_M) {
		m_iValue /= POPNUMBER_M;
		token = POPNUMBER_M_TOKEN;
	}
	else if (m_iValue > POPNUMBER_K) {
		m_iValue /= POPNUMBER_K;
		token = POPNUMBER_K_TOKEN;
	}
	else
		token = POPNUMBER_O_TOKEN;
	wchar_t wszContent[32] = { 0 };
	vgui::localize()->ConstructString(wszContent, sizeof(wszContent), vgui::localize()->Find(token.c_str()), 1, std::to_wstring(m_iValue).c_str());
	m_pNumberLable->SetText(wszContent);

	LoadControlSettings(VGUI2_ROOT_DIR "PopNumber.res");

	m_iOriginalY = y;
	m_iOriginalTall = GetTall() / 2;
	SetPos(x - GetWide() / 2, y - m_iOriginalTall);
	SetVisible(false);
	m_flCreateTime = gEngfuncs.GetClientTime();
}
const char* CPopNumberPanel::GetName(){
	return VIEWPORT_POPNUMBER_NAME;
}
void CPopNumberPanel::SetSize(int w, int t){
	BaseClass::SetSize(w, t);
	m_pNumberLable->SetSize(w, t);
}
void CPopNumberPanel::Reset(){
	Destory();
}
void CPopNumberPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("PopNumber.BackGoundColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
void CPopNumberPanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CPopNumberPanel::IsVisible(){
	return BaseClass::IsVisible();
}
vgui::VPANEL CPopNumberPanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CPopNumberPanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}

void CPopNumberPanel::OnThink(){
	float ratio = 1 + sin(gEngfuncs.GetClientTime() - m_flCreateTime);
	int x, y;
	GetPos(x, y);
	SetPos(x, m_iOriginalY + m_iOriginalTall * ratio);
	int a = m_pNumberLable->GetAlpha();
	a-=2;
	if (a <= 0)
		Destory();
	else
		m_pNumberLable->SetAlpha(a);
}

void CPopNumberPanel::Destory(){
	m_pNumberLable->DeletePanel();
	DeletePanel();
}
