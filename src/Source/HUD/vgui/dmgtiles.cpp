#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <VGUI2/tga_image.h>
#include "vgui_controls/ImageList.h"
#include "vgui_controls/Panel.h"
#include "vgui_controls/ListViewPanel.h"
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include <vguilocal.h>
#include "Viewport.h"

#include "dmgtiles.h"

#define VIEWPORT_DMGTILES_NAME "DMGTilesPanel"

using namespace vgui;

CDmgTilesPanel::CDmgTilesPanel()
	: BaseClass(nullptr, VIEWPORT_DMGTILES_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme("HealthScheme");
	m_pDmgImages = new ListViewPanel(this, "DMGImages");

	dmgimageitem_t some[] = {
		{ "Poison", "icon_poison", DMG_POISON, 0.0f},
		{ "Acid", "icon_acid", DMG_ACID, 0.0f},
		{ "Freeze","icon_freeze", DMG_FREEZE | DMG_SLOWFREEZE, 0.0f},
		{ "Drown", "icon_drown", DMG_DROWN, 0.0f},
		{ "Burn", "icon_burn", DMG_BURN | DMG_SLOWBURN, 0.0f},
		{ "Gas", "icon_gas", DMG_NERVEGAS, 0.0f},
		{ "Radiation", "icon_radiation", DMG_RADIATION, 0.0f},
		{ "Shock", "icon_shock", DMG_SHOCK, 0.0f}
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
	m_pDmgImages->DeleteAllItems();
}
void CDmgTilesPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("HealthBar.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pDmgImages->SetBgColor(GetSchemeColor("HealthBar.ListViewBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
void CDmgTilesPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	ImageList* list = new ImageList(true);
	for (size_t i = 0; i < m_aryDmgImageList.size(); i++) {
		auto iter = m_aryDmgImageList[i];
		const char* icon = inResourceData->GetString(iter.szIconKey, nullptr);
		if (icon) {
			CTGAImage* img = new CTGAImage(icon);
			img->SetColor(Color(255, 255, 255, 255));
			list->AddImage(img);
		}
	}
	m_pDmgImages->SetImageList(list, true);
}
void CDmgTilesPanel::OnThink(){
	float flTime = ClientTime();
	for (auto iter = m_aryDmgImageList.begin(); iter != m_aryDmgImageList.end(); iter++) {
		if (iter->fExpire < flTime) {
			m_pDmgImages->RemoveItem(iter->iIndex);
		}
	}
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
	bitsDamage = -1;
	float flTime = ClientTime();
	for (auto iter = m_aryDmgImageList.begin(); iter != m_aryDmgImageList.end(); iter++) {
		if (iter->iDmg & bitsDamage) {
			iter->fExpire = flTime + 15.0f;
			if (!m_pDmgImages->GetItem(iter->iIndex)) {
				KeyValues* pkv = new KeyValues(iter->szName);
				pkv->SetString("text", iter->szName);
				pkv->SetInt("image", iter->iIndex);
				m_pDmgImages->AddItem(pkv, false, true);
				delete pkv;
			}
		}
	}
}