#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"

#include <CVector.h>
#include "triangleapi.h"
#include "mymathlib.h"

#include "gindicator.h"
#include "Viewport.h"
#include <algorithm>

#define GRENADE_MODEL_PATH "models/w_grenade.mdl"

CGIndicatorItem::CGIndicatorItem(vgui::Panel* parent, const char* name) : BaseClass(parent, name){
}
void CGIndicatorItem::SetExpire(float fl) {
	m_flExpire = fl;
}
bool CGIndicatorItem::IsExpire() {
	float flTime = ClientTime();
	return m_flExpire <= flTime;
}
bool CGIndicatorItem::IsValid() {
	return GetAlpha() > 0 && m_pEnt;
}
void CGIndicatorItem::Update() {
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if (!local)
		return;
	//视角角度
	CVector vecView;
	gEngfuncs.GetViewAngles(vecView);
	mathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
	//计算我和目标的相对偏移
	CVector vecLength;
	mathlib::VectorSubtract(m_pEnt->curstate.origin, local->curstate.origin, vecLength);
	vecLength = vecLength.Normalize();
	float angledotResult = mathlib::DotProduct(vecLength, vecView);
	//cos 60
	if (angledotResult > 0.5) {
		CVector vecHUD;
		VEC_WorldToScreen(m_pEnt->curstate.origin, vecHUD);
		SetPos(vecHUD.x - GetWide() / 2, vecHUD.y - GetTall());
	}
}
cl_entity_t* CGIndicatorItem::GetEnt(){
	return m_pEnt;
}
void CGIndicatorItem::SetEnt(cl_entity_t* ent){
	m_pEnt = ent;
}

#define VIEWPORT_GINDICATOR_NAME "GrenadeIndicatorPanel"
CGenadeIndicatorPanel::CGenadeIndicatorPanel()
	: BaseClass(nullptr, VIEWPORT_GINDICATOR_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "GIndicatorScheme.res", "GIndicatorScheme");
	SetScheme("GIndicatorScheme");

	gCVars.pGrenadeIndicator = CREATE_CVAR("cl_grenadeindicator", "1", FCVAR_VALUE, nullptr);
	gCVars.pGrenadeIndicatorRange = CREATE_CVAR("cl_grenadeindicator_range", "256", FCVAR_VALUE, nullptr);
	gCVars.pGrenadeInicatorTime = CREATE_CVAR("cl_grenadeindicator_time", "4", FCVAR_VALUE, nullptr);
	
	LoadControlSettings(VGUI2_ROOT_DIR "GrenadeIndicatorPanel.res");
}
const char* CGenadeIndicatorPanel::GetName() {
	return VIEWPORT_GINDICATOR_NAME;
}
void CGenadeIndicatorPanel::Reset() {
	for (auto it = m_aryImages.begin(); it != m_aryImages.end();) {
		(*it)->DeletePanel();
		it = m_aryImages.erase(it);
	}
}
void CGenadeIndicatorPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("GIndicator.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_cImageDrawColor = GetSchemeColor("GIndicator.DrawColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
}
void CGenadeIndicatorPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	const char* img = inResourceData->GetString("image", nullptr);
	if (img)
		Q_strcpy(m_szImageName, img);
	m_iImageWide = inResourceData->GetInt("imagewide", 24);
	m_iImageTall = inResourceData->GetInt("imagetall", 24);
}
void CGenadeIndicatorPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CGenadeIndicatorPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CGenadeIndicatorPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CGenadeIndicatorPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}
void CGenadeIndicatorPanel::OnThink() {
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	for (auto it = m_aryImages.begin(); it != m_aryImages.end(); ) {
		CGIndicatorItem* img = (*it);
		if (!img || !img->IsValid()) {
			img->DeletePanel();
			it = m_aryImages.erase(it);
			continue;
		}
		if(img->IsExpire())
			vgui::GetAnimationController()->RunAnimationCommand(img, "alpha", 0, 0.0f, 0.15f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		else
			img->Update();
		it++;
	}
}
void CGenadeIndicatorPanel::AddEntity(cl_entity_s* ent, const char* modelname){
	if (gCVars.pGrenadeIndicator->value <= 0)
		return;
	if (Q_strcmp(modelname, GRENADE_MODEL_PATH) || ent->curstate.sequence == 0 || ent->model->type != mod_studio)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if (ent->curstate.messagenum != local->curstate.messagenum)
		return;
	CVector len;
	mathlib::VectorSubtract(local->curstate.origin, ent->curstate.origin, len);
	if (len.FLength() > gCVars.pGrenadeIndicatorRange->value)
		return;
	if (std::none_of(m_aryImages.begin(), m_aryImages.end(), [ent](CGIndicatorItem* v) {
		if (v->GetEnt() == ent)
			return true;
		return  false;
		})) {
		CGIndicatorItem* img = new CGIndicatorItem(this, "grenade");
		img->SetImage(m_szImageName);
		img->SetDrawColor(m_cImageDrawColor);
		img->SetSize(m_iImageWide, m_iImageTall);
		img->SetExpire(ClientTime() + gCVars.pGrenadeInicatorTime->value);
		img->SetEnt(ent);
		m_aryImages.push_back(img);
	}
}
