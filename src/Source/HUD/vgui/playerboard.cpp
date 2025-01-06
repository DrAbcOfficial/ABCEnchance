#pragma once
#include <vector>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>

#include "local.h"
#include "vguilocal.h"
#include "mymathlib.h"
#include "steamclientpublic.h"
#include "player_info.h"

#include "vgui_controls/ImagePanel.h"
#include "playerboard.h"
#include "Viewport.h"

#include "plugins.h"

//thank you microsoft
#undef clamp

#define VIEWPORT_PLAYERBOARD_NAME "PlayerBoardPanel"
extern vgui::HScheme GetViewPortBaseScheme();
CPlayerInfoPanel::CPlayerInfoPanel()
	: BaseClass(nullptr, VIEWPORT_PLAYERBOARD_NAME)
{
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());

	// Header labels
	m_pNameLabel = new vgui::Label(this, "PlayerName", "(Anon)");
	m_pBackgroundImagePanel = new vgui::ImagePanel(this, "BackGroundImage");
	m_pHealthImagePanel = new vgui::ImagePanel(this, "HealthImage");
	m_pArmorImagePanel = new vgui::ImagePanel(this, "ArmorImage");
	m_pCourchIconImagePanel = new vgui::ImagePanel(this, "CourchIconImage");
	m_pMedikIconImagePanel = new vgui::ImagePanel(this, "MedikIconImage");
	m_pDeadIconImagePanel = new vgui::ImagePanel(this, "DeadIconImage");

	LoadControlSettings(VGUI2_ROOT_DIR "PlayerBoardPanel.res");
	SetVisible(false);
}

void CPlayerInfoPanel::SetId(int index){
	m_iPlayerIndex = index + 1;
}

void CPlayerInfoPanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("PlayerBoardPanel.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}

void CPlayerInfoPanel::Think(){
	if (!GetBaseViewPort()->IsPlayerTileEnable()) {
		SetVisible(false);
		return;
	}
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if(!local)
		return;
	if (local->index == m_iPlayerIndex)
		return;
	//视角角度
	vec3_t vecView;
	Vector vecLength;
	float angledotResult;
	Vector vecEntityOrigin;
	Vector vecHUD;
	gEngfuncs.GetViewAngles(vecView);
	CMathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
	cl_entity_t* entity = gEngfuncs.GetEntityByIndex(m_iPlayerIndex);
	if (!entity ||
		entity->curstate.messagenum != local->curstate.messagenum ||
		!entity->player || !entity->model) {
		SetVisible(false);
		return;
	}
	//计算我和目标的相对偏移
	vecLength = entity->curstate.origin;
	vecLength -= local->curstate.origin;
	if (vecLength.Length() >= 1024) {
		SetVisible(false);
		return;
	}
	vecLength = vecLength.Normalize();
	angledotResult = vecLength.Dot(vecView);;
	//cos 60
	if (angledotResult > 0.5) {
		vecEntityOrigin = entity->curstate.origin;
		vecEntityOrigin.z += 45;
		VEC_WorldToScreen(vecEntityOrigin, vecHUD);
		this->SetPos(vecHUD.x - this->GetWide() / 2, vecHUD.y);
	}
	else{
		SetVisible(false);
		return;
	}
	UpdateClientInfo();
}

const char* CPlayerInfoPanel::GetName()
{
	return VIEWPORT_PLAYERBOARD_NAME;
}

void CPlayerInfoPanel::Reset()
{
	if (IsVisible())
	{
		ShowPanel(false);
	}
}

void CPlayerInfoPanel::ShowPanel(bool state)
{
	if (state == IsVisible())
		return;
	SetVisible(state);
}

vgui::VPANEL CPlayerInfoPanel::GetVPanel()
{
	return BaseClass::GetVPanel();
}

bool CPlayerInfoPanel::IsVisible()
{
	return BaseClass::IsVisible();
}

void CPlayerInfoPanel::SetParent(vgui::VPANEL parent)
{
	BaseClass::SetParent(parent);
}

void CPlayerInfoPanel::UpdateClientInfo(){
	CPlayerInfo* pi = CPlayerInfo::GetPlayerInfo(m_iPlayerIndex);
	if (pi->IsValid()) {
		if (pi->IsSpectator() || pi->GetTeamNumber() != CPlayerInfo::GetThisPlayerInfo()->GetTeamNumber()) {
			ShowPanel(false);
			return;
		}	
		int iHealth = pi->GetHealth();
		float flArmorRatio = clamp<float>((float)pi->GetArmor() / 100.0f, 0.0f, 1.0f);
		float flHealthRatio = clamp((float)iHealth / 100.0f, 0.0f, 1.0f);
		m_pNameLabel->SetText(pi->GetName());
		m_pNameLabel->SetFgColor(GetBaseViewPort()->GetPlayerColor(m_iPlayerIndex));
		m_pArmorImagePanel->SetWide(m_pBackgroundImagePanel->GetWide() * flArmorRatio);
		m_pHealthImagePanel->SetWide(m_pBackgroundImagePanel->GetWide() * flHealthRatio);
		cl_entity_t* ent = gEngfuncs.GetEntityByIndex(m_iPlayerIndex);
		if (iHealth < GetBaseViewPort()->m_pPlayerTitleDanger->value) {
			if (iHealth <= 0){
				m_pCourchIconImagePanel->SetVisible(false);
				m_pMedikIconImagePanel->SetVisible(false);
				m_pDeadIconImagePanel->SetVisible(true);
			}
			else {
				m_pCourchIconImagePanel->SetVisible(false);
				m_pMedikIconImagePanel->SetVisible(true);
				m_pDeadIconImagePanel->SetVisible(false);
			}
		}
		else if (fabs(ent->curstate.maxs[2] - ent->curstate.mins[2]) < 64) {
			m_pCourchIconImagePanel->SetVisible(true);
			m_pMedikIconImagePanel->SetVisible(false);
			m_pDeadIconImagePanel->SetVisible(false);
		}
		else {
			m_pCourchIconImagePanel->SetVisible(false);
			m_pMedikIconImagePanel->SetVisible(false);
			m_pDeadIconImagePanel->SetVisible(false);
		}
		ShowPanel(true);
	}
	else {
		ShowPanel(false);
	}
	
}