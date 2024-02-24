#include <metahook.h>

#include <vguilocal.h>
#include <wrect.h>

#include <map>
#include <string>
#include "cvardef.h"
#include "weaponinfo.h"
class WEAPON;
typedef int HSPRITE;
#include "weaponbank.h"

#include "vgui_controls/ImageSprPanel.h"
#include "vgui_controls/spr_image.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/AnimationController.h"

#include "ammostack.h"

#define VIEWPORT_AMMOSTACK_NAME "AmmoStackPanel"

CAmmoStackItem::CAmmoStackItem(Panel* parent, int iSpridx, int value, int l, int r, int t, int b, float expire, float fi, float fo, float is)
	: BaseClass(parent, "item") {
	m_pPanel = new vgui::ImageSprPanel(this, "Panel");
	m_pPanel->SetShouldScaleImage(true);
	m_pPanel->SetImage(iSpridx);
	m_pPanel->SetRect(l, r, t, b);
	m_pPanel->SetRenderMode(kRenderTransAdd);
	m_pPanel->GetImage()->SetAlphaParent(this);
	m_pText = new vgui::Label(this, "Text", std::to_string(value).c_str());

	iSprIdx = iSpridx;
	iValue = value;
	fIn = fi;
	fOut = fo;
	SetTall(vgui::scheme()->GetProportionalScaledValue(b - t) * is);
	Show(expire);
}
void CAmmoStackItem::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("AmmoStack.ListViewBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pPanel->SetDrawColor(GetSchemeColor("AmmoStack.IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	m_pText->SetFgColor(GetSchemeColor("AmmoStack.TextColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
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
int CAmmoStackItem::GetValue() {
	return iValue;
}
void CAmmoStackItem::SetImage(const char* image) {
	m_pPanel->SetImage(image);
}
extern float ClientTime();
void CAmmoStackItem::Show(float flTime) {
	float current = ClientTime();
	SetExpire(current + flTime);
	SetVisible(true);
	SetAlpha(1);
	vgui::GetAnimationController()->RunAnimationCommand(this, "alpha", 255, 0.0f, fIn, vgui::AnimationController::INTERPOLATOR_LINEAR);
}
void CAmmoStackItem::SetExpire(float f) {
	fKeepTime = f;
}
void CAmmoStackItem::CheckExpire() {
	if (fKeepTime <= 0)
		return;
	float flTime = ClientTime();
	if (fKeepTime <= flTime) {
		fKeepTime = 0;
		vgui::GetAnimationController()->RunAnimationCommand(this, "alpha", 0, 0.0f, fOut, vgui::AnimationController::INTERPOLATOR_LINEAR);
	}
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
		(*iter)->DeletePanel();
	}
	m_aryAmmos.clear();
}
void CAmmoStackPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("AmmoStack.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
void CAmmoStackPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_flKeepTime = inResourceData->GetFloat("keep_time", 2.0f);
	m_flFadeinTime = inResourceData->GetFloat("fadein_time", 0.1f);
	m_flFadeoutTime = inResourceData->GetFloat("fadeout_time", 0.3f);
	m_flIconSize = inResourceData->GetFloat("icon_scale", 1.0f);
}
void CAmmoStackPanel::OnThink() {
	for (auto iter = m_aryAmmos.begin(); iter != m_aryAmmos.end();) {
		auto item = (*iter);
		item->CheckExpire();
		if (item->GetAlpha() <= 0) {
			item->DeletePanel();
			iter = m_aryAmmos.erase(iter);
		}
		else
			iter++;
	}
}
void CAmmoStackPanel::PaintBackground() {
	if (m_aryAmmos.size() == 0)
		return;
	int w = GetWide();
	int y = GetTall();
	for (auto iter = m_aryAmmos.rbegin(); iter != m_aryAmmos.rend(); iter++) {
		auto item = *iter;
		if (item->IsVisible()) {
			y -= item->GetTall();
			item->SetWide(w);
			item->SetPos(0, y);
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

void CAmmoStackPanel::AddAmmoPickup(int id, int count){
	wrect_t rcPic;
	HSPRITE* spr = gWR.GetAmmoPicFromWeapon(id, rcPic);
	CAmmoStackItem* item = new CAmmoStackItem(this, *spr, count, rcPic.left, rcPic.right, rcPic.top, rcPic.bottom, m_flKeepTime, m_flFadeinTime, m_flFadeoutTime, m_flIconSize);
	m_aryAmmos.push_back(item);
}
