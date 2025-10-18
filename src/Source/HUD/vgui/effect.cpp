#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>

#include <vgui_controls/spr_image.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/ImageSprPanel.h>
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"

#include "steamclientpublic.h"
#include "Viewport.h"
#include <exportfuncs.h>

#include "effect.h"

#define VIEWPORT_EFFECT_NAME "EffectPanel"

extern vgui::HScheme GetViewPortBaseScheme();
CEffectPanel::CEffectPanel()
	: BaseClass(nullptr, VIEWPORT_EFFECT_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);

	SetScheme(GetViewPortBaseScheme());
	// Header labels
	m_pRightWater = new vgui::ImageSprPanel(this, "RightWater");
	m_pLeftWater = new vgui::ImageSprPanel(this, "LeftWater");
	m_pUnderWater = new vgui::ImagePanel(this, "UnderWater");
	m_pDangerPanel = new vgui::ImagePanel(this, "Danger");

	gCVars.pHudEfx = CREATE_CVAR("hud_efx", "1", FCVAR_VALUE, nullptr);
	LoadControlSettings(VGUI2_ROOT_DIR "EffectPanel.res");
}

const char* CEffectPanel::GetName() {
	return VIEWPORT_EFFECT_NAME;
}
void CEffectPanel::Reset() {
	ShowPanel(true);
	m_pRightWater->Reset();
	m_pLeftWater->Reset();
	m_iOldWaterStatue = 0;
	m_iNowWaterType = WaterType::NONE;
	m_pLeftWater->SetVisible(false);
	m_pRightWater->SetVisible(false);
	m_pUnderWater->SetVisible(false);
	m_pDangerPanel->SetVisible(false);
}

void CEffectPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CEffectPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CEffectPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CEffectPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

void CEffectPanel::OnThink(){
	if (!GetBaseViewPort()->LoacalPlayerAvilable())
		return;
	if (GetBaseViewPort()->IsInSpectate() || gCVars.pHudEfx->value < 1) {
		ShowPanel(false);
		return;
	}
	ShowPanel(true);
	if (m_iOldWaterStatue == WATERLEVEL_HEAD)
		SetWater(gClientData->waterlevel == WATERLEVEL_HEAD ? WaterType::UNDER_WATER : WaterType::OUT_WATER);
	else if(m_pLeftWater->GetAlpha() <= 0 && m_pRightWater->GetAlpha() <= 0)
		SetWater(WaterType::NONE);

	if(m_pLeftWater->IsVisible())
		m_pLeftWater->Animate();
	if(m_pRightWater->IsVisible())
		m_pRightWater->Animate();

	m_iOldWaterStatue = gClientData->waterlevel;
}

void CEffectPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_flAlphaAinmeTime = inResourceData->GetFloat("fade_time", 0.2f);
	m_flWaterKeepTime = inResourceData->GetFloat("water_keeptime", 1.5f);
}

void CEffectPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetBgColor(GetSchemeColor("EffectPanel.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pUnderWater->SetDrawColor(GetSchemeColor("EffectPanel.UnderWaterColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	m_pDangerPanel->SetDrawColor(GetSchemeColor("EffectPanel.DangerColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
}

void CEffectPanel::SetHealth(int health){
	if (health < gCVars.pDangerHealth->value) {
		float flTarget = (1.0f - (static_cast<float>(health) / gCVars.pDangerHealth->value)) * 255.0f;
		m_pDangerPanel->SetVisible(true);
		vgui::GetAnimationController()->RunAnimationCommand(m_pDangerPanel, "alpha", flTarget, 0.0f, m_flAlphaAinmeTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
	}
	else {
		m_pDangerPanel->SetAlpha(0);
		m_pDangerPanel->SetVisible(false);
	}
}

void CEffectPanel::SetWater(WaterType type){
	if (m_iNowWaterType == type)
		return;
	switch (type)
	{
	case CEffectPanel::WaterType::NONE: {
		m_pLeftWater->SetVisible(false);
		m_pRightWater->SetVisible(false);
		m_pUnderWater->SetVisible(false);
		break;
	}
	case CEffectPanel::WaterType::UNDER_WATER: {
		m_pLeftWater->SetVisible(false);
		m_pRightWater->SetVisible(false);
		m_pUnderWater->SetVisible(true);
		m_pUnderWater->SetAlpha(0);
		vgui::GetAnimationController()->RunAnimationCommand(m_pUnderWater, "alpha", 255, 0.0f, m_flAlphaAinmeTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
		break;
	}
	case CEffectPanel::WaterType::OUT_WATER: {
		m_pLeftWater->SetVisible(true);
		m_pLeftWater->SetFrame(0);
		m_pLeftWater->SetAlpha(255);
		vgui::GetAnimationController()->RunAnimationCommand(m_pLeftWater, "alpha", 0, 0.0f, m_flWaterKeepTime, vgui::AnimationController::INTERPOLATOR_LINEAR);

		m_pRightWater->SetAlpha(255);
		m_pRightWater->SetVisible(true);
		m_pRightWater->SetFrame(0);
		vgui::GetAnimationController()->RunAnimationCommand(m_pRightWater, "alpha", 0, 0.0f, m_flWaterKeepTime, vgui::AnimationController::INTERPOLATOR_LINEAR);

		m_pUnderWater->SetVisible(true);
		m_pUnderWater->SetAlpha(255);
		vgui::GetAnimationController()->RunAnimationCommand(m_pUnderWater, "alpha", 0, 0.0f, m_flAlphaAinmeTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
		break;
	}
	default:
		break;
	}
	m_iNowWaterType = type;
}
