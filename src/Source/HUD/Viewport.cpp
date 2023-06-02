#pragma once
#include <metahook.h>
#include <vector>
#include <vgui/VGUI.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/IEngineVGui.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Label.h>
#include "BaseUI.h"
#include "mymathlib.h"
#include "local.h"
#include "vguilocal.h"
#include <string>
#include "steam_api.h"
#include "player_info.h"

#include "playerboard.h"
#include "scoreboard.h"
#include "vote.h"

#include "Viewport.h"
#include "exportfuncs.h"
#include "keydefs.h"

using namespace vgui;

CViewport *g_pViewPort = nullptr;

CViewport::CViewport(void) : Panel(nullptr, "ABCEnchanceViewport")
{
	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	MakePopup(false, true);

	SetScheme("ABCEnchanceScheme");
	SetBounds(0, 0, swide, stall);
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	SetProportional(true);
}

CViewport::~CViewport(void)
{
	
}

void CViewport::Start(void)
{
	AddNewPanel(m_pScorePanel = new CScorePanel());
	for (size_t i = 0; i < 32; i++) {
		AddNewPanel(m_pPlayerInfoPanels[i] = new CPlayerInfoPanel());
		m_pPlayerInfoPanels[i]->SetId(i);
	}
	SetVisible(false);
}

void CViewport::SetParent(VPANEL vPanel)
{
	BaseClass::SetParent(vPanel);
	m_pScorePanel->SetParent(GetVPanel());
	for (size_t i = 0; i < 32; i++) {
		m_pPlayerInfoPanels[i]->SetParent(GetVPanel());
	}
}

void CViewport::AddNewPanel(IViewportPanel* panel)
{
	m_Panels.push_back(panel);
	panel->SetParent(GetVPanel());
	dynamic_cast<vgui::Panel*>(panel)->MakeReadyForUse();
}

void CViewport::Think(void)
{
	for (size_t i = 0; i < 32; i++) {
		m_pPlayerInfoPanels[i]->Think();
	}
}

void CViewport::VidInit(void)
{
	Reset();
}

void CViewport::Reset() {
	for (IViewportPanel* pPanel : m_Panels)
		pPanel->Reset();
	GetThisPlayerInfo()->ResetAll();
}

void CViewport::Init(void)
{
	GetThisPlayerInfo()->InitPlayerInfos();
	GetTeamInfo(0)->InitTeamInfos();
}

void CViewport::ActivateClientUI(void)
{
	SetVisible(true);
}

void CViewport::HideClientUI(void)
{
	SetVisible(false);
	for (IViewportPanel* pPanel : m_Panels)
	{
		if (pPanel->IsVisible())
			pPanel->ShowPanel(false);
	}
}

bool CViewport::KeyInput(int down, int keynum, const char* pszCurrentBinding){
	if (down){
		if (IsScoreBoardVisible()){
			if (!m_pScorePanel->IsMouseInputEnabled()){
				if (keynum == K_MOUSE2){
					m_pScorePanel->EnableMousePointer(true);
					return false;
				}
			}
		}
	}
	return true;
}
bool CViewport::IsScoreBoardVisible()
{
	return m_pScorePanel->IsVisible();
}
void CViewport::ShowScoreBoard()
{
	m_pScorePanel->ShowPanel(true);
}
void CViewport::HideScoreBoard()
{
	m_pScorePanel->ShowPanel(false);
}
long CViewport::GetTimeEnd() {
	return m_iTimeEnd;
}
char* CViewport::GetServerName(){
	return m_szServerName;
}
char* CViewport::GetNextMap() {
	return m_szNextMapName;
}
void CViewport::Paint(void){
	BaseClass::Paint();
}

CScorePanel* CViewport::GetScoreBoard() {
	return m_pScorePanel;
}

CVotePanel* CViewport::GetVotePanel(){
	return m_pVotePanel;
}

Color CViewport::GetPlayerColor(int index){
	vec3_t color;
	mathlib::VectorCopy(gHookFuncs.GetClientColor(index), color);
	return Color(color[0] * 255, color[1] * 255, color[2] * 255, 255);
}
