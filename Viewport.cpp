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
#include "scoreboard.h"
#include "Viewport.h"
#include "exportfuncs.h"

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
	m_szLevelName[0] = 0;
}

CViewport::~CViewport(void)
{
	
}

void CViewport::Start(void)
{
	AddNewPanel(m_pScorePanel = new CScorePanel());
	SetVisible(false);
}

void CViewport::SetParent(VPANEL vPanel)
{
	BaseClass::SetParent(vPanel);
	m_pScorePanel->SetParent(GetVPanel());
}

void CViewport::AddNewPanel(IViewportPanel* panel)
{
	m_Panels.push_back(panel);
	panel->SetParent(GetVPanel());
	dynamic_cast<vgui::Panel*>(panel)->MakeReadyForUse();
}

void CViewport::Think(void)
{
	auto levelname = gEngfuncs.pfnGetLevelName();
	if (!levelname || !levelname[0])
		return;

}

void CViewport::VidInit(void)
{
	Reset();
}

void CViewport::Reset() {
	m_szLevelName[0] = 0;
	for (IViewportPanel* pPanel : m_Panels)
		pPanel->Reset();
	GetThisPlayerInfo()->ResetAll();
}

void CViewport::Init(void)
{

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

void CViewport::OnMousePressed(vgui::MouseCode code){
	BaseClass::OnMousePressed(code);
	m_pScorePanel->OnMousePressed(code);
}

void CViewport::OnMouseDoublePressed(vgui::MouseCode code) {
	BaseClass::OnMouseDoublePressed(code);
	m_pScorePanel->OnMouseDoublePressed(code);
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

char* CViewport::GetServerName()
{
	return m_szServerName;
}

void CViewport::Paint(void){
	BaseClass::Paint();
}

CScorePanel* CViewport::GetScoreBoard() {
	return m_pScorePanel;
}

Color CViewport::GetPlayerColor(int index){
	vec3_t color;
	mathlib::VectorCopy(gHookFuncs.GetClientColor(index), color);
	return Color(color[0] * 255, color[1] * 255, color[2] * 255, 255);
}
