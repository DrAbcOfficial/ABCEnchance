#include <vguilocal.h>

#include "vgui_controls/ImageSprPanel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/AnimationController.h"

#include "ammostack.h"

#define VIEWPORT_AMMOSTACK_NAME "AmmoStackPanel"

CAmmoStackItem::CAmmoStackItem(Panel* parent, const char* text, const char* icon, int value)
	: BaseClass(parent, text) {
	m_pPanel = new vgui::ImageSprPanel(this, "Panel");
	m_pPanel->SetShouldScaleImage(true);
	m_pText = new vgui::Label(this, "Text", text);

	Q_strcpy(szIconKey, icon);
	iValue = value;
}
void CAmmoStackItem::PerformLayout() {
	BaseClass::PerformLayout();
	int w, h;
	GetSize(w, h);
	int img = min(w * 0.25, h);
	m_pPanel->SetSize(img, img);
	m_pText->SetPos(w * 0.25, 0);
	m_pText->SetSize(w * 0.75, h);
}
const char* CAmmoStackItem::GetIconKey() {
	return szIconKey;
}
int CAmmoStackItem::GetValue() {
	return iValue;
}
void CAmmoStackItem::SetIconColor(Color in) {
	m_pPanel->SetDrawColor(in);
}
void CAmmoStackItem::SetTextColor(Color in) {
	m_pText->SetFgColor(in);
}
void CAmmoStackItem::SetImage(const char* image) {
	m_pPanel->SetImage(image);
}
extern float ClientTime();
void CAmmoStackItem::Show(float flTime) {
	float current = ClientTime();
	fExpire = current + flTime;
	SetVisible(true);
	SetAlpha(0);
	vgui::GetAnimationController()->RunAnimationCommand(this, "alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}
void CAmmoStackItem::Reset() {
	fExpire = 0;
	SetVisible(false);
	SetAlpha(255);
}
void CAmmoStackItem::SetExpire(float f) {
	fExpire = f;
}
void CAmmoStackItem::CheckExpire() {
	if (fExpire <= 0)
		return;
	float flTime = ClientTime();
	if (fExpire + 0.3f <= flTime) {
		fExpire = 0;
		return;
	}
	if (fExpire < flTime)
		vgui::GetAnimationController()->RunAnimationCommand(this, "alpha", 0, 0.0f, 0.3f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}

extern vgui::HScheme GetViewPortBaseScheme();
CAmmoStackPanel::CAmmoStackPanel()
	: BaseClass(nullptr, VIEWPORT_AMMOSTACK_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());
	LoadControlSettings(VGUI2_ROOT_DIR "AmmoStackPanel.res");
	SetVisible(true);
}
const char* CAmmoStackPanel::GetName() {
	return VIEWPORT_AMMOSTACK_NAME;
}
void CAmmoStackPanel::Reset() {
	if (!IsVisible())
		ShowPanel(true);
	for (auto iter = m_aryAmmos.begin(); iter != m_aryAmmos.end(); iter++) {
		(*iter)->Reset();
	}
}
void CAmmoStackPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("AmmoStack.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_cItemBgColor = GetSchemeColor("AmmoStack.ListViewBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	m_cItemIconColor = GetSchemeColor("AmmoStack.IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cItemTextColor = GetSchemeColor("AmmoStack.TextColor", GetSchemeColor("Label.FgColor", pScheme), pScheme);
}
void CAmmoStackPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_flKeepTime = inResourceData->GetFloat("keep_time");
}
void CAmmoStackPanel::OnThink() {
	for (auto iter = m_aryAmmos.begin(); iter != m_aryAmmos.end(); iter++) {
		(*iter)->CheckExpire();
	}
}
void CAmmoStackPanel::PaintBackground() {
	if (m_aryAmmos.size() == 0)
		return;
	int w, h;
	GetSize(w, h);
	int itemh = h / m_aryAmmos.size();
	int y = h - itemh;
	for (auto iter = m_aryAmmos.rbegin(); iter != m_aryAmmos.rend(); iter++) {
		if ((*iter)->IsVisible()) {
			(*iter)->SetSize(w, itemh);
			(*iter)->SetPos(0, y);
			y -= itemh;
		}
	}
	BaseClass::PaintBackground();
}
void CAmmoStackPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CAmmoStackPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CAmmoStackPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CAmmoStackPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}