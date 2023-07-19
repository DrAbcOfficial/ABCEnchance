#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <VGUI2/spr_image.h>

#include "vgui_controls/Panel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImagePanel.h"
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include <vguilocal.h>
#include "Viewport.h"

#include "dmgtiles.h"

#define VIEWPORT_DMGTILES_NAME "DMGTilesPanel"

using namespace vgui;

CTileIconItem::CTileIconItem(Panel* parent, const char* controller, const char* text, const char* image) 
	: BaseClass(parent, controller) {
	m_pPanel = new ImagePanel(this, "Panel");
	m_pPanel->SetShouldScaleImage(true);
	m_pPanel->SetImage(image);
	m_pText = new Label(this, "Text", text);
}

void CTileIconItem::PaintBackground(){
	int w, h;
	GetSize(w, h);
	int img = min(w * 0.25, h);
	m_pPanel->SetSize(img, img);
	m_pText->SetPos(w * 0.25, 0);
	m_pText->SetSize(w * 0.75, h);
	BaseClass::PaintBackground();
}

void CTileIconItem::SetIconColor(Color in){
	m_pPanel->SetDrawColor(in);
}

void CTileIconItem::SetTextColor(Color in){
	m_pText->SetFgColor(in);
}

CDmgTilesPanel::CDmgTilesPanel()
	: BaseClass(nullptr, VIEWPORT_DMGTILES_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme("HealthScheme");
	dmgimageitem_t some[] = {
		{ "#DMGTiles_Poison", "icon_poison", DMG_POISON, 0.0f},
		{ "#DMGTiles_Acid", "icon_acid", DMG_ACID, 0.0f},
		{ "#DMGTiles_Freeze","icon_freeze", DMG_FREEZE | DMG_SLOWFREEZE, 0.0f},
		{ "#DMGTiles_Drown", "icon_drown", DMG_DROWN, 0.0f},
		{ "#DMGTiles_Burn", "icon_burn", DMG_BURN | DMG_SLOWBURN, 0.0f},
		{ "#DMGTiles_Gas", "icon_gas", DMG_NERVEGAS, 0.0f},
		{ "#DMGTiles_Radiation", "icon_radiation", DMG_RADIATION, 0.0f},
		{ "#DMGTiles_Shock", "icon_shock", DMG_SHOCK, 0.0f}
	};
	m_aryDmgImageList = std::vector<dmgimageitem_t>(some, some + sizeof(some) / sizeof(some[0]));
	for (size_t i = 0; i < m_aryDmgImageList.size(); i++)
		m_aryDmgImageList[i].iIndex = i;

	LoadControlSettings(VGUI2_ROOT_DIR "DMGTilesPanel.res");
	SetVisible(false);
}
const char* CDmgTilesPanel::GetName(){
	return VIEWPORT_DMGTILES_NAME;
}
void CDmgTilesPanel::Reset(){
	if (!IsVisible())
		ShowPanel(true);
	for (auto iter = m_aryDmgImageList.begin(); iter != m_aryDmgImageList.end(); iter++) {
		iter->fExpire = 0;
	}
	for (auto iter = m_aryDmg.begin(); iter != m_aryDmg.end(); iter++) {
		(*iter)->SetVisible(false);
		(*iter)->SetAlpha(255);
	}
}
void CDmgTilesPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("DMGTiles.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	Color itemBg = GetSchemeColor("DMGTiles.ListViewBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	Color itemIcon = GetSchemeColor("DMGTiles.IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	Color itemText = GetSchemeColor("DMGTiles.TextColor", GetSchemeColor("Label.FgColor", pScheme), pScheme);
	for (auto iter = m_aryDmg.rbegin(); iter != m_aryDmg.rend(); iter++) {
		(*iter)->SetBgColor(itemBg);
		(*iter)->SetIconColor(itemIcon);
		(*iter)->SetTextColor(itemText);
	}
}
void CDmgTilesPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	for (size_t i = 0; i < m_aryDmgImageList.size(); i++) {
		auto iter = m_aryDmgImageList[i];
		const char* icon = inResourceData->GetString(iter.szIconKey, nullptr);
		m_aryDmg.push_back(new CTileIconItem(this, iter.szIconKey, iter.szName, icon));
	}
	m_flKeepTime = inResourceData->GetFloat("keep_time");
}
void CDmgTilesPanel::OnThink(){
	float flTime = ClientTime();
	for (auto iter = m_aryDmgImageList.begin(); iter != m_aryDmgImageList.end(); iter++) {
		if (iter->fExpire < flTime) {
			GetAnimationController()->RunAnimationCommand(m_aryDmg[iter->iIndex], "alpha", 0, 0.0f, 0.3f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		}
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
	float flTime = ClientTime();
	for (auto iter = m_aryDmgImageList.begin(); iter != m_aryDmgImageList.end(); iter++) {
		if (iter->iDmg & bitsDamage) {
			iter->fExpire = flTime + m_flKeepTime;
			auto tile = m_aryDmg[iter->iIndex];
			tile->SetVisible(true);
			tile->SetAlpha(0);
			GetAnimationController()->RunAnimationCommand(tile, "alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		}
	}
}