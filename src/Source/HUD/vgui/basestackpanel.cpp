#include <metahook.h>

#include "vgui_controls/ImageSprPanel.h"
#include "vgui_controls/spr_image.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/AnimationController.h"

#include "basestackpanel.h"

using namespace vgui;

CBaseSprStackItem::CBaseSprStackItem(Panel* parent, int iSpridx, int l, int r, int t, int b, float expire, float fi, float fo, float is)
	: BaseClass(parent, "item") {
	m_pPanel = new vgui::ImageSprPanel(this, "Panel");
	m_pPanel->SetShouldScaleImage(true);
	m_pPanel->SetImage(iSpridx);
	m_pPanel->SetRect(l, r, t, b);
	m_pPanel->SetRenderMode(kRenderTransAdd);
	m_pPanel->GetImage()->SetAlphaParent(this);
	iSprIdx = iSpridx;
	fIn = fi;
	fOut = fo;
	Show(expire);
}
extern float ClientTime();
void CBaseSprStackItem::Show(float flTime) {
	float current = ClientTime();
	SetExpire(current + flTime);
	SetVisible(true);
	SetAlpha(1);
	vgui::GetAnimationController()->RunAnimationCommand(this, "alpha", 255, 0.0f, fIn, vgui::AnimationController::INTERPOLATOR_LINEAR);
}
void CBaseSprStackItem::SetExpire(float f) {
	fKeepTime = f;
}
void CBaseSprStackItem::CheckExpire() {
	if (fKeepTime <= 0)
		return;
	float flTime = ClientTime();
	if (fKeepTime <= flTime) {
		fKeepTime = 0;
		vgui::GetAnimationController()->RunAnimationCommand(this, "alpha", 0, 0.0f, fOut, vgui::AnimationController::INTERPOLATOR_LINEAR);
	}
}

CBaseStackPanel::CBaseStackPanel(vgui::Panel* parent, const char* name) : BaseClass(parent, name){
}

void CBaseStackPanel::Reset() {
	if (!IsVisible())
		ShowPanel(true);
	for (auto iter = m_aryPanels.begin(); iter != m_aryPanels.end(); iter++) {
		(*iter)->DeletePanel();
	}
	m_aryPanels.clear();
}
void CBaseStackPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_flKeepTime = inResourceData->GetFloat("keep_time", 2.0f);
	m_flFadeinTime = inResourceData->GetFloat("fadein_time", 0.1f);
	m_flFadeoutTime = inResourceData->GetFloat("fadeout_time", 0.3f);
	m_flIconSize = inResourceData->GetFloat("icon_scale", 1.0f);
	m_iStackGap = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("stack_gap", 0));
}
void CBaseStackPanel::OnThink() {
	for (auto iter = m_aryPanels.begin(); iter != m_aryPanels.end();) {
		auto item = (*iter);
		item->CheckExpire();
		if (item->GetAlpha() <= 0) {
			item->DeletePanel();
			iter = m_aryPanels.erase(iter);
		}
		else
			iter++;
	}
}
void CBaseStackPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CBaseStackPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CBaseStackPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CBaseStackPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}