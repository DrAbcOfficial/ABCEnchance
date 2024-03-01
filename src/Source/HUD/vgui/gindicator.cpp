#pragma once
#include <algorithm>

#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"

#include "triangleapi.h"
#include "mymathlib.h"

#include "gindicator.h"

#define GRENADE_MODEL_PATH "models/w_grenade.mdl"

void CGIndicatorItem::SetExpire(float fl) {
	m_flExpire = fl;
}
bool CGIndicatorItem::IsExpire() {
	float flTime = ClientTime();
	return m_flExpire <= flTime;
}
cl_entity_t* CGIndicatorItem::GetEnt(){
	return m_pEnt;
}
void CGIndicatorItem::SetEnt(cl_entity_t* ent){
	m_pEnt = ent;
}

#define VIEWPORT_GINDICATOR_NAME "GrenadeIndicatorPanel"
extern vgui::HScheme GetViewPortBaseScheme();
CGenadeIndicatorPanel::CGenadeIndicatorPanel()
	: BaseClass(nullptr, VIEWPORT_GINDICATOR_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());

	gCVars.pGrenadeIndicator = CREATE_CVAR("cl_grenadeindicator", "1", FCVAR_VALUE, nullptr);
	gCVars.pGrenadeIndicatorRange = CREATE_CVAR("cl_grenadeindicator_range", "256", FCVAR_VALUE, nullptr);
	gCVars.pGrenadeInicatorTime = CREATE_CVAR("cl_grenadeindicator_time", "4", FCVAR_VALUE, nullptr);
	
	LoadControlSettings(VGUI2_ROOT_DIR "GrenadeIndicatorPanel.res");
}
const char* CGenadeIndicatorPanel::GetName() {
	return VIEWPORT_GINDICATOR_NAME;
}
void CGenadeIndicatorPanel::Reset() {
	ShowPanel(true);
	m_aryImages.clear();
}
void CGenadeIndicatorPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("GIndicator.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_cImageDrawColor = GetSchemeColor("GIndicator.DrawColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
}
void CGenadeIndicatorPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	const char* img = inResourceData->GetString("image", nullptr);
	if (img) {
		VGUI_CREATE_NEWTGA_TEXTURE(m_iImageTexture, img);
	}
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
		if (!img || !img->GetEnt() || img->IsExpire()) {
			delete img;
			it = m_aryImages.erase(it);
			continue;
		}
		it++;
	}
}
void CGenadeIndicatorPanel::PaintBackground(){
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if (!local)
		return;
	//视角角度
	Vector vecView;
	gEngfuncs.GetViewAngles(vecView);
	CMathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
	for (auto it = m_aryImages.begin(); it != m_aryImages.end(); it++) {
		CGIndicatorItem* img = (*it);
		//计算我和目标的相对偏移
		Vector vecLength;
		CMathlib::VectorSubtract(img->GetEnt()->curstate.origin, local->curstate.origin, vecLength);
		vecLength = vecLength.Normalize();
		float angledotResult = CMathlib::DotProduct(vecLength, vecView);
		//cos 60
		if (angledotResult > 0.5) {
			Vector vecHUD;
			VEC_WorldToScreen(img->GetEnt()->curstate.origin, vecHUD);
			int x1 = vecHUD.x - m_iImageWide / 2;
			int y1 = vecHUD.y - m_iImageTall / 2;
			int x2 = vecHUD.x + m_iImageWide / 2;
			int y2 = vecHUD.y + m_iImageTall / 2;
			vgui::surface()->DrawSetTexture(m_iImageTexture);
			vgui::surface()->DrawSetColor(m_cImageDrawColor);
			vgui::surface()->DrawTexturedRect(x1, y1, x2, y2);
		}
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
	Vector len;
	CMathlib::VectorSubtract(local->curstate.origin, ent->curstate.origin, len);
	if (len.Length() > gCVars.pGrenadeIndicatorRange->value)
		return;
	if (std::none_of(m_aryImages.begin(), m_aryImages.end(), [ent](CGIndicatorItem* v) {
		if (v->GetEnt() == ent)
			return true;
		return  false;
		})) {
		CGIndicatorItem* img = new CGIndicatorItem();
		img->SetExpire(ClientTime() + gCVars.pGrenadeInicatorTime->value);
		img->SetEnt(ent);
		m_aryImages.push_back(img);
	}
}
