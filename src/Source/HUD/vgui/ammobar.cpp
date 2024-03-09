#pragma once
#include <map>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/spr_image.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ImagePanel.h>
#include "vgui_controls/ImageSprPanel.h"

#include "local.h"
#include "vguilocal.h"
#include "weapon.h"
#include "weaponbank.h"

#include "Viewport.h"

#include "ammobar.h"

#define VIEWPORT_AMMO_NAME "AmmoPanel"

using namespace vgui;

extern vgui::HScheme GetViewPortBaseScheme();
CAmmoPanel::CAmmoPanel()
	: BaseClass(nullptr, VIEWPORT_AMMO_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);

	SetScheme(GetViewPortBaseScheme());
	m_pBackground = new ImagePanel(this, "Background");

	m_Ammo1Icon = new ImageSprPanel(this, "Ammo1Icon");
	m_pAmmo1Label = new Label(this, "Ammo1", "0");
	m_pSpr1 = new CSPRImage();

	m_pSlashLabel = new Label(this, "Slash", "|");

	m_Ammo2Icon = new ImageSprPanel(this, "Ammo2Icon");
	m_pAmmo2Label = new Label(this, "Ammo2", "0");
	m_pSpr2 = new CSPRImage();
	LoadControlSettings(VGUI2_ROOT_DIR "AmmoPanel.res");
}
const char* CAmmoPanel::GetName(){
	return VIEWPORT_AMMO_NAME;
}
void CAmmoPanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
	m_pHandledWeapon = nullptr;
}
void CAmmoPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("AmmoBar.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pAmmo1Label->SetFgColor(GetSchemeColor("AmmoBar.Ammo1FgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_pAmmo2Label->SetFgColor(GetSchemeColor("AmmoBar.Ammo2FgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_pSlashLabel->SetFgColor(GetSchemeColor("AmmoBar.SlashFgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_Ammo1Icon->SetDrawColor(GetSchemeColor("AmmoBar.Ammo1IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	m_Ammo2Icon->SetDrawColor(GetSchemeColor("AmmoBar.Ammo2IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
}
void CAmmoPanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	if (!m_pHandledWeapon)
		state = false;
	SetVisible(state);
}
bool CAmmoPanel::IsVisible(){
	return BaseClass::IsVisible();
}
VPANEL CAmmoPanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CAmmoPanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}

void CAmmoPanel::SetWeapon(WEAPON* weapon){
	m_pHandledWeapon = weapon;
	RefreshAmmo();
}

void CAmmoPanel::RefreshAmmo(){
	if (!m_pHandledWeapon) {
		ShowPanel(false);
		return;
	}
	if (g_pViewPort->HasSuit())
		ShowPanel(true);
	if (!(m_pHandledWeapon->iState & VALID))
		return;
	char buf[64];
	if (m_pHandledWeapon->iAmmoType > 0) {
		m_Ammo1Icon->SetVisible(true);
		m_pAmmo1Label->SetVisible(true);
		m_pSpr1->SetTextureID(m_pHandledWeapon->hAmmo);
		m_pSpr1->SetRect(m_pHandledWeapon->rcAmmo.left, m_pHandledWeapon->rcAmmo.right, m_pHandledWeapon->rcAmmo.top, m_pHandledWeapon->rcAmmo.bottom);
		m_pSpr1->SetRenderMode(kRenderTransAdd);
		m_Ammo1Icon->SetImage(m_pSpr1);
		int iAmmoLeft = gWR.CountAmmo(m_pHandledWeapon->iAmmoType);
		if (m_pHandledWeapon->iClip >= 0) {
			if (m_pHandledWeapon->iState & UZI &&
				m_pHandledWeapon->iFlags & 32 &&
				m_pHandledWeapon->iClip2 >= 0)
				Q_snprintf(buf, "%dx%d/%d", m_pHandledWeapon->iClip2, m_pHandledWeapon->iClip, iAmmoLeft);
			else
				Q_snprintf(buf, "%d/%d", m_pHandledWeapon->iClip, iAmmoLeft);
		}
		else
			Q_snprintf(buf, "%d", iAmmoLeft);
		m_pAmmo1Label->SetText(buf);
	}
	else {
		m_Ammo1Icon->SetVisible(false);
		m_pAmmo1Label->SetVisible(false);
	}
	if (m_pHandledWeapon->iAmmo2Type > 0 && m_pHandledWeapon->iState & VALID) {
		m_Ammo2Icon->SetVisible(true);
		m_pAmmo2Label->SetVisible(true);
		m_pSlashLabel->SetVisible(true);
		m_pSpr2->SetTextureID(m_pHandledWeapon->hAmmo2);
		m_pSpr2->SetRect(m_pHandledWeapon->rcAmmo2.left, m_pHandledWeapon->rcAmmo2.right, m_pHandledWeapon->rcAmmo2.top, m_pHandledWeapon->rcAmmo2.bottom);
		m_pSpr2->SetRenderMode(kRenderTransAdd);
		m_Ammo2Icon->SetImage(m_pSpr2);
		int iAmmoLeft2 = gWR.CountAmmo(m_pHandledWeapon->iAmmo2Type);
		if (m_pHandledWeapon->iClip2 >= 0)
			Q_snprintf(buf, "%d/%d", m_pHandledWeapon->iClip2, iAmmoLeft2);
		else
			Q_snprintf(buf, "%d", iAmmoLeft2);
		m_pAmmo2Label->SetText(buf);
	}
	else {
		m_Ammo2Icon->SetVisible(false);
		m_pAmmo2Label->SetVisible(false);
		m_pSlashLabel->SetVisible(false);
	}
	if (m_pHandledWeapon->iAmmoType <= 0 && m_pHandledWeapon->iAmmo2Type <= 0)
		ShowPanel(false);
}
