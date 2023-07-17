#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"

#include "mymathlib.h"

#include "cfefx.h"
#include "Viewport.h"


#define VIEWPORT_KILLMARK_NAME "KillMarkPanel"
#define VIEWPORT_DMGMARK_NAME "DmgMarkPanel"



CKillMarkPanel::CKillMarkPanel() : BaseClass(nullptr, VIEWPORT_KILLMARK_NAME)
{
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);

	//评价是没必要用Scheme
	//vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "CfefxScheme.res", "CfefxScheme");
	//SetScheme("CfefxScheme");

	m_pKillMarkPoint = CREATE_PANEL(vgui::ImagePanel, this, "KillMarkPoint");
	m_pDmgMarkOne = CREATE_PANEL(vgui::ImagePanel, this, "DmgMarkOne");
	m_pDmgMarkTwo = CREATE_PANEL(vgui::ImagePanel, this, "DmgMarkTwo");
	m_pDmgMarkThree = CREATE_PANEL(vgui::ImagePanel, this, "DmgMarkThree");
	m_pDmgMarkFour = CREATE_PANEL(vgui::ImagePanel, this, "DmgMarkFour");
	m_pDmgMarkFive = CREATE_PANEL(vgui::ImagePanel, this, "DmgMarkFive");
	m_pDmgEffect = CREATE_PANEL(vgui::ImagePanel, this, "DmgEffect");
	m_pDmgStar = CREATE_PANEL(vgui::ImagePanel, this, "DmgStar");

	gCVars.pCfefx = CREATE_CVAR("cl_cfefx", "1", FCVAR_VALUE, nullptr);
	gCVars.pCfefxDmgMax = CREATE_CVAR("cl_cfefx_max", "1000", FCVAR_VALUE, nullptr);

	MakeReadyForUse();
	LoadControlSettings(VGUI2_ROOT_DIR "Cfefx.res");

	//SetPos(mathlib::GetScreenPixel(ScreenWidth, 0.464), mathlib::GetScreenPixel(ScreenHeight, 0.768));
	//SetSize(mathlib::GetScreenPixel(ScreenWidth, 0.087), mathlib::GetScreenPixel(ScreenHeight, 0.112));
}

void CKillMarkPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(Color(0, 0, 0, 0));
}

void CKillMarkPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_ioffestYPos = inResourceData->GetInt("offest_ypos");
}

bool CKillMarkPanel::IsVisible() {
	return BaseClass::IsVisible();
}

void CKillMarkPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}

void CKillMarkPanel::StartFade(bool state, vgui::ImagePanel* panel, float delaytime, float fadetime) 
{
	panel->SetAlpha(state ? 1 : 255);
	panel->SetVisible(true);
	vgui::GetAnimationController()->RunAnimationCommand(panel, "alpha", state ? 255 : 0, delaytime, fadetime, vgui::AnimationController::INTERPOLATOR_LINEAR);
	panel->SetVisible(state);
}

void CKillMarkPanel::ShowDmgMark(vgui::ImagePanel* panel, bool state)
{
	//偏移YPos重复利用Panel
	if (state == 1)
		panel->SetPos(GetXPos(), GetYPos() + m_ioffestYPos);

	//DmgStar从屏幕中间淡入向对应伤害图标移动并放大，经过一定距离最大后缩小并移动到当前伤害图标
	//两个参数咋传
	//StartFade(true, m_pDmgStar, 0, 1);
	vgui::GetAnimationController()->RunAnimationCommand(m_pDmgStar, "position", 0, 0, 1, vgui::AnimationController::INTERPOLATOR_LINEAR);

	StartFade(true, panel, 0, 1);
}

void CKillMarkPanel::ShowKillMark(int* iDmg)
{
	int dmg = *iDmg;
	int i = gCVars.pCfefxDmgMax->value / 10;
	if (dmg >= i && dmg / i != 0)
	{
		switch (dmg / i) {
		case 1:
			ShowDmgMark(m_pDmgMarkOne, 0);
			break;
		case 2: 
			ShowDmgMark(m_pDmgMarkTwo, 0);
			break;
		case 3: 
			ShowDmgMark(m_pDmgMarkThree, 0);
			break;
		case 4: 
			ShowDmgMark(m_pDmgMarkFour, 0);
			break;
		case 5: {
			m_pDmgMarkOne->SetVisible(false);
			//TODO:达到500伤害其它图标向上移动到第一个图标“合并”成一个500伤害图标
			ShowDmgMark(m_pDmgMarkFive, 0);
			break;
		}
		case 6:
			ShowDmgMark(m_pDmgMarkOne, 1);
			break;
		case 7: 
			ShowDmgMark(m_pDmgMarkTwo, 1);
			break;
		case 8:
			ShowDmgMark(m_pDmgMarkThree, 1);
			break;
		case 9:
			ShowDmgMark(m_pDmgMarkFour, 1);
			break;
		default: {
			//TODO:清空伤害图标,达到一千伤害后显示DmgEffect从第一个伤害图标的上面移动到击杀图标
			m_pDmgMarkOne->SetVisible(false);
			m_pDmgMarkTwo->SetVisible(false);
			m_pDmgMarkThree->SetVisible(false);
			m_pDmgMarkFour->SetVisible(false);
			m_pDmgMarkFive->SetVisible(false);
			*iDmg = 0;
			PlaySoundByName("misc/UI_SPECIALKILL2.wav", 1);
			gEngfuncs.Con_Printf("sound played\n");
			StartFade(false, m_pKillMarkPoint, 1.5, 1);
		}
		}
	}
}

vgui::VPANEL CKillMarkPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}

void CKillMarkPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

const char* CKillMarkPanel::GetName() {
	return VIEWPORT_KILLMARK_NAME;
}

void CKillMarkPanel::Reset() {
	ShowPanel(false);
}

//void CKillMarkPanel::OnThink()
//{
//	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
//	if (!local)
//		return;
//	ShowPanel(true);
//	float a = m_pKillMark->GetAlpha();
//	a -= 0.2;
//	if (a <= 0)
//		ShowPanel(false);
//	else
//		m_pKillMark->SetAlpha(a);
//}