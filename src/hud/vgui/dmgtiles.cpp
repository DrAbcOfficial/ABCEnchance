#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>

#include "vgui_controls/Panel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImagePanel.h"
#include <vgui_controls/AnimationController.h>

#include "utility/util.h"
#include "utility/vgui_util.h"

#include "dmgtiles.h"

#define VIEWPORT_DMGTILES_NAME "DMGTilesPanel"

using namespace vgui;
extern vgui::HScheme GetViewPortBaseScheme();

CTileIconItem::CTileIconItem(Panel* parent, const char* text, const char* icon, int dmg)
	: BaseClass(parent, text) {
	m_pPanel = new ImagePanel(this, "Panel");
	m_pPanel->SetShouldScaleImage(true);
	m_pText = new Label(this, "Text", text);

	Q_strcpy(szIconKey, icon);
	iDmg = dmg;
}

void CTileIconItem::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("DMGTiles.ListViewBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	SetIconColor(GetSchemeColor("DMGTiles.IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	SetTextColor(GetSchemeColor("DMGTiles.TextColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_pText->SetFont(pScheme->GetFont(pScheme->GetResourceString("DMGTiles.Font"), IsProportional()));
}

void CTileIconItem::PerformLayout(){
	BaseClass::PerformLayout();
	int w, h;
	GetSize(w, h);
	int img = min(w * 0.25, h);
	m_pPanel->SetSize(img, img);
	m_pText->SetPos(w * 0.25, 0);
	m_pText->SetSize(w * 0.75, h);
}

const char* CTileIconItem::GetIconKey(){
	return szIconKey;
}

int CTileIconItem::GetDMG(){
	return iDmg;
}

void CTileIconItem::SetIconColor(Color in){
	m_pPanel->SetDrawColor(in);
}

void CTileIconItem::SetTextColor(Color in){
	m_pText->SetFgColor(in);
}

void CTileIconItem::SetImage(const char* image){
	m_pPanel->SetImage(image);
}

void CTileIconItem::Show(){
	SetVisible(true);
	GetAnimationController()->CancelAnimationsForPanel(GetParent());
	GetAnimationController()->StartAnimationSequence(GetParent(), GetName());
}

void CTileIconItem::Reset(){
	SetVisible(false);
}

CDmgTilesPanel::CDmgTilesPanel()
	: BaseClass(nullptr, VIEWPORT_DMGTILES_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());
	m_aryDmg = {
		new CTileIconItem(this, "#DMGTiles_Poison", "icon_poison", DMG_POISON),
		new CTileIconItem(this, "#DMGTiles_Acid", "icon_acid", DMG_ACID),
		new CTileIconItem(this, "#DMGTiles_Freeze","icon_freeze", DMG_FREEZE | DMG_SLOWFREEZE),
		new CTileIconItem(this, "#DMGTiles_Drown", "icon_drown", DMG_DROWN),
		new CTileIconItem(this, "#DMGTiles_Burn", "icon_burn", DMG_BURN | DMG_SLOWBURN),
		new CTileIconItem(this, "#DMGTiles_Gas", "icon_gas", DMG_NERVEGAS),
		new CTileIconItem(this, "#DMGTiles_Radiation", "icon_radiation", DMG_RADIATION),
		new CTileIconItem(this, "#DMGTiles_Shock", "icon_shock", DMG_SHOCK | DMG_SHOCK_GLOW),
		new CTileIconItem(this, "#DMGTiles_Energybeam", "icon_beam", DMG_ENERGYBEAM),
		new CTileIconItem(this, "#DMGTiles_Sonic", "icon_sonic", DMG_SONIC)
	};
	LoadControlSettings(VGUI2_ROOT_DIR "DMGTilesPanel.res");
	SetVisible(false);
}
const char* CDmgTilesPanel::GetName(){
	return VIEWPORT_DMGTILES_NAME;
}
void CDmgTilesPanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
	for (auto iter = m_aryDmg.begin(); iter != m_aryDmg.end(); iter++) {
		(*iter)->Reset();
	}
}
void CDmgTilesPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("DMGTiles.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
void CDmgTilesPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	for (auto iter = m_aryDmg.begin(); iter != m_aryDmg.end(); iter++) {
		(*iter)->Reset();
		const char* icon = inResourceData->GetString((*iter)->GetIconKey(), nullptr);
		if(icon)
			(*iter)->SetImage(icon);
	}
}
void CDmgTilesPanel::PaintBackground(){
	if (m_aryDmg.size() == 0)
		return;
	int w, h;
	GetSize(w, h);
	int itemh = h / m_aryDmg.size();
	int y = h - itemh;
	for (auto iter = m_aryDmg.rbegin(); iter != m_aryDmg.rend(); iter++) {
		if ((*iter)->IsVisible()) {
			(*iter)->SetSize(w, itemh);
			(*iter)->SetPos(0, y);
			y -= itemh;
		}
	}
	BaseClass::PaintBackground();
}
void CDmgTilesPanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CDmgTilesPanel::IsVisible(){
	return BaseClass::IsVisible();
}
vgui::VPANEL CDmgTilesPanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CDmgTilesPanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}
void CDmgTilesPanel::UpdateTiles(long bitsDamage) {
	for (auto iter = m_aryDmg.begin(); iter != m_aryDmg.end(); iter++) {
		if ((*iter)->GetDMG() & bitsDamage)
			(*iter)->Show();
	}
}