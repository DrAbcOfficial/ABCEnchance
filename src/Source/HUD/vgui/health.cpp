#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>

#include <vgui_controls/tga_image.h>
#include <vgui_controls/Label.h>
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/ImageClipPanel.h"
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"

#include "health.h"

#define VIEWPORT_HEALTH_NAME "HealthPanel"

using namespace vgui;
extern vgui::HScheme GetViewPortBaseScheme();
CHealthPanel::CHealthPanel()
	: BaseClass(nullptr, VIEWPORT_HEALTH_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());

	m_pBackgroundImage = new ImagePanel(this, "Background");

	m_pHealthIcon = new ImagePanel(this, "HealthIcon");
	m_pHealthLabel = new Label(this, "Health", "0");
	m_pHealthImagePanel = new ImageClipPanel(this, "HealthBar");
	m_pHealthImageBackround = new ImagePanel(this, "HealthBackground");

	m_pArmorIcon = new ImagePanel(this, "ArmorIcon");
	m_pArmorLabel = new Label(this, "Armor", "0");
	m_pArmorImagePanel = new ImageClipPanel(this, "ArmorBar");
	m_pArmorImageBackround = new ImagePanel(this, "ArmorBackground");

	m_pLongJumpImagePanel = new ImagePanel(this, "Longjump");

	LoadControlSettings(VGUI2_ROOT_DIR "HealthPanel.res");
	SetVisible(false);

	m_iRestoredHealWide = m_pHealthImagePanel->GetWide();
	m_iRestoredArmorWide = m_pArmorImagePanel->GetWide();
}
const char* CHealthPanel::GetName(){
	return VIEWPORT_HEALTH_NAME;
}
void CHealthPanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
	SetLongJump(false);
}
void CHealthPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("HealthBar.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_cRestoredHealthLabel = GetSchemeColor("HealthBar.HealthFgColor", GetSchemeColor("Label.BgColor", pScheme), pScheme);
	m_pHealthLabel->SetFgColor(m_cRestoredHealthLabel);
	m_cRestoredArmorLabel = GetSchemeColor("HealthBar.ArmorFgColor", GetSchemeColor("Label.BgColor", pScheme), pScheme);
	m_pArmorLabel->SetFgColor(m_cRestoredArmorLabel);

	m_cHealthDanger = GetSchemeColor("HealthBar.HealthDangerColor", pScheme);
	m_cArmorDanger = GetSchemeColor("HealthBar.ArmorDangerColor", pScheme);

	m_pBackgroundImage->SetDrawColor(GetSchemeColor("HealthBar.ImageBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));

	m_cRestoredHealthIcon = GetSchemeColor("HealthBar.HealthIconFgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cRestoredHealth = GetSchemeColor("HealthBar.HealthBarFgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_pHealthImageBackround->SetDrawColor(GetSchemeColor("HealthBar.HealthBarBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pHealthIcon->SetDrawColor(m_cRestoredHealthIcon);
	m_pHealthImagePanel->SetDrawColor(m_cRestoredHealth);

	m_cRestoredArmorIcon = GetSchemeColor("HealthBar.ArmorIconFgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cRestoredArmor = GetSchemeColor("HealthBar.ArmorBarFgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_pArmorImageBackround->SetDrawColor(GetSchemeColor("HealthBar.ArmorBarBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pArmorIcon->SetDrawColor(m_cRestoredArmorIcon);
	m_pArmorImagePanel->SetDrawColor(m_cRestoredArmor);

	m_pLongJumpImagePanel->SetDrawColor(GetSchemeColor("HealthBar.LongJumpIconFgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
}
void CHealthPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
}
void CHealthPanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CHealthPanel::IsVisible(){
	return BaseClass::IsVisible();
}
vgui::VPANEL CHealthPanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CHealthPanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}

void CHealthPanel::SetHealth(int health){
	static char buf[32];
	Q_snprintf(buf, "%d", health);
	m_pHealthLabel->SetText(buf);

	if (health < gCVars.pDangerHealth->value) {
		float flRatio = (float)health / gCVars.pDangerHealth->value;
		m_pHealthLabel->SetFgColor(GetDifferColor(flRatio, m_pHealthLabel->GetFgColor(), m_cHealthDanger));
		m_pHealthImagePanel->SetDrawColor(GetDifferColor(flRatio, m_pHealthImagePanel->GetDrawColor(), m_cHealthDanger));
		m_pHealthIcon->SetDrawColor(GetDifferColor(flRatio, m_pHealthIcon->GetDrawColor(), m_cHealthDanger));
	}
	else {
		m_pHealthLabel->SetFgColor(m_cRestoredHealthLabel);
		m_pHealthImagePanel->SetDrawColor(m_cRestoredHealth);
		m_pHealthIcon->SetDrawColor(m_cRestoredHealthIcon);
	}	
	int newWide = clamp((float)health / 100.0f, 0.0f, 1.0f) * m_iRestoredHealWide;
	vgui::GetAnimationController()->RunAnimationCommand(m_pHealthImagePanel, "wide", newWide, 0.0f, 0.15f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}

void CHealthPanel::SetArmor(int armor){
	static char buf[32];
	Q_snprintf(buf, "%d", armor);
	m_pArmorLabel->SetText(buf);

	if (armor < gCVars.pDangerArmor->value) {
		float flRatio = (float)armor / gCVars.pDangerHealth->value;
		m_pArmorImagePanel->SetDrawColor(GetDifferColor(flRatio, m_pArmorImagePanel->GetDrawColor(), m_cArmorDanger));
		m_pArmorLabel->SetFgColor(GetDifferColor(flRatio, m_pArmorLabel->GetFgColor(), m_cArmorDanger));
		m_pArmorIcon->SetDrawColor(GetDifferColor(flRatio, m_pArmorIcon->GetDrawColor(), m_cArmorDanger));
	}
	else {
		m_pArmorLabel->SetFgColor(m_cRestoredArmorLabel);
		m_pArmorImagePanel->SetDrawColor(m_cRestoredArmor);
		m_pArmorIcon->SetDrawColor(m_cRestoredArmorIcon);
	}
	int newWide = clamp((float)armor / 100.0f, 0.0f, 1.0f) * m_iRestoredArmorWide;
	vgui::GetAnimationController()->RunAnimationCommand(m_pArmorImagePanel, "wide", newWide, 0.0f, 0.15f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}

void CHealthPanel::SetLongJump(bool lj){
	m_pLongJumpImagePanel->SetVisible(lj);
}
void CHealthPanel::SetArmorVisible(bool state) {
	m_pArmorImagePanel->SetVisible(state);
	m_pArmorLabel->SetVisible(state);
	m_pArmorImageBackround->SetVisible(state);
	m_pArmorIcon->SetVisible(state);
}
void CHealthPanel::SetHealthVisible(bool state) {
	m_pHealthImagePanel->SetVisible(state);
	m_pHealthLabel->SetVisible(state);
	m_pHealthImageBackround->SetVisible(state);
	m_pHealthIcon->SetVisible(state);
}

Color CHealthPanel::GetDifferColor(float flRatio, Color c1, Color c2){
	Vector v1 = { (float)c1.r() / 255.0f,(float)c1.g() / 255.0f ,(float)c1.b() / 255.0f };
	Vector v2 = { (float)c2.r() / 255.0f,(float)c2.g() / 255.0f ,(float)c2.b() / 255.0f };
	RGBtoHSV(v1, v1);
	RGBtoHSV(v2, v2);
	v1.x = fmodf(v2.x + (v1.x - v2.x) * flRatio, 360.0f);
	if (v1.x < 0)
		v1.x += 360.0f;
	HSVtoRGB(v1, v1);
	v1 *= 255;
	Color color = { (int)v1.x, (int)v1.y, (int)v1.z, c1.a() };
	return color;
}
