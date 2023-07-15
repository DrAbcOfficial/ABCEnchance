#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui2/spr_image.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"

#include "steamclientpublic.h"
#include "player_info.h"
#include "Viewport.h"
#include <exportfuncs.h>

#include "effect.h"

#define VIEWPORT_EFFECT_NAME "EffectPanel"

CEffectPanel::CEffectPanel()
	: BaseClass(nullptr, VIEWPORT_EFFECT_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);

	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "EffectPanelScheme.res", "EffectPanelScheme");
	SetScheme("EffectPanelScheme");
	// Header labels
	m_pLeftWater = new vgui::ImagePanel(this, "LeftWater");
	m_pLeftWaterSpr = new vgui::CSPRImage(m_pLeftWater->GetImageName());
	
	m_pRightWater = new vgui::ImagePanel(this, "RightWater");
	m_pRightWaterSpr = new vgui::CSPRImage(m_pRightWater->GetImageName());
	
	m_pUnderWater = new vgui::ImagePanel(this, "UnderWater");
	m_pDangerPanel = new vgui::ImagePanel(this, "Danger");

	gCVars.pHudEfx = CREATE_CVAR("cl_efxhud", "1", FCVAR_VALUE, NULL);

	LoadControlSettings(VGUI2_ROOT_DIR "EffectPanel.res");
}

const char* CEffectPanel::GetName() {
	return VIEWPORT_EFFECT_NAME;
}
void CEffectPanel::Reset() {
	ShowPanel(true);
	m_pLeftWaterSpr->Reset();
	m_pRightWaterSpr->Reset();
	m_iOldWaterStatue = 0;
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

void CEffectPanel::OnTick(){
	m_pLeftWaterSpr->Animate();
	m_pRightWaterSpr->Animate();

	if (g_pViewPort->IsInSpectate() || gCVars.pHudEfx->value < 1) {
		ShowPanel(false);
		return;
	}
	ShowPanel(true);
	if (m_iOldWaterStatue == WATERLEVEL_HEAD)
		SetWater(gClientData->waterlevel == WATERLEVEL_HEAD ? WaterType::UNDER_WATER : WaterType::OUT_WATER);
	else
		SetWater(WaterType::NONE);
	m_iOldWaterStatue = gClientData->waterlevel;
}

void CEffectPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_flAlphaAinmeTime = inResourceData->GetFloat("fade_time", 0.2f);
	m_flInterve = inResourceData->GetFloat("spr_interval", 0.5f);
	m_flWaterKeepTime = inResourceData->GetFloat("water_keeptime", 1.5f);

	m_pLeftWaterSpr->SetFramerate(m_flInterve);
	m_pRightWaterSpr->SetFramerate(m_flInterve);
	m_pRightWater->SetImage(m_pRightWaterSpr);
	m_pLeftWater->SetImage(m_pLeftWaterSpr);
}

void CEffectPanel::SetHealth(int health){
	if (health < gCVars.pDangerHealth->value) {
		float flTarget = health / 100.0f * 255.0f;
		vgui::GetAnimationController()->RunAnimationCommand(m_pDangerPanel, "alpha", flTarget, 0.0f, m_flAlphaAinmeTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
	}
	else
		m_pDangerPanel->SetAlpha(0);
}

void CEffectPanel::SetWater(WaterType type){
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
		break;
	}
	case CEffectPanel::WaterType::OUT_WATER: {
		m_pLeftWater->SetVisible(true);
		m_pRightWater->SetVisible(true);
		m_pLeftWater->SetAlpha(255);
		m_pRightWater->SetAlpha(255);
		vgui::GetAnimationController()->RunAnimationCommand(m_pLeftWater, "alpha", 0, m_flWaterKeepTime, m_flAlphaAinmeTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
		vgui::GetAnimationController()->RunAnimationCommand(m_pRightWater, "alpha", 0, m_flWaterKeepTime, m_flAlphaAinmeTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
		m_pUnderWater->SetVisible(false);
		break;
	}
	default:
		break;
	}
}
