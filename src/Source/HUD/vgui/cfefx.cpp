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

	m_pKillMarkPoint = new vgui::ImagePanel(this, "KillMarkPoint");
	m_pDmgMarkOne = new vgui::ImagePanel(this, "DmgMarkOne");
	m_pDmgMarkTwo = new vgui::ImagePanel(this, "DmgMarkTwo");
	m_pDmgMarkThree = new vgui::ImagePanel(this, "DmgMarkThree");
	m_pDmgMarkFour = new vgui::ImagePanel(this, "DmgMarkFour");
	m_pDmgMarkFive = new vgui::ImagePanel(this, "DmgMarkFive");
	m_pDmgStar = new vgui::ImagePanel(this, "DmgStar");

	LoadControlSettings(VGUI2_ROOT_DIR "Cfefx.res");

	//SetPos(mathlib::GetScreenPixel(ScreenWidth, 0.464), mathlib::GetScreenPixel(ScreenHeight, 0.768));
	//SetSize(mathlib::GetScreenPixel(ScreenWidth, 0.087), mathlib::GetScreenPixel(ScreenHeight, 0.112));
	//m_pKillMark->SetShouldScaleImage(true);
	//m_pKillMark->SetImage("abcenchance/tga/cfefx/BossDamage");
	//m_pKillMark->SetPos(mathlib::GetScreenPixel(ScreenWidth, 0.464), mathlib::GetScreenPixel(ScreenHeight, 0.768));
	//m_pKillMark->SetSize(mathlib::GetScreenPixel(ScreenWidth, 0.087), mathlib::GetScreenPixel(ScreenHeight, 0.112));
	//m_pKillMark->SetVisible(true);
	//SetVisible(true);
}

void CKillMarkPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	//SetBgColor(GetSchemeColor("KillMark.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	SetBgColor(Color(0, 0, 0, 0));
}

bool CKillMarkPanel::IsVisible() {
	return BaseClass::IsVisible();
}

void CKillMarkPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}

void CKillMarkPanel::ShowKillMark(int* iDmg, int iDmgMax)
{
	int i = iDmgMax / 10;

	if (*iDmg >= i)
	{
		/*
		TODO:
		每100伤害触发
		星号向左对应伤害图标移动并放大，经过一定距离最大后缩小并移动到当前伤害图标
		伤害图标直接在对应位置淡入
		达到500伤害其它图标向上移动到第一个图标“合并”成一个500伤害图标


		面板得统一放在个数组什么之类的里面 还没学不会写
		


		*/	
		
		switch (*iDmg / i)
		{
		case 0:
			break;
		case 1:
			m_pDmgMarkOne->SetVisible(true);
			m_pDmgMarkOne->SetAlpha(0);
			vgui::GetAnimationController()->RunAnimationCommand(m_pDmgMarkOne, "alpha", 255, 0, 1.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
			break;
		case 2:
			m_pDmgMarkTwo->SetVisible(true);
			break;
		case 3:
			m_pDmgMarkThree->SetVisible(true);
			break;
		case 4:
			m_pDmgMarkFour->SetVisible(true);
			break;
		case 5:
			m_pDmgMarkFive->SetVisible(true);
			break;
		}
	}
	if (*iDmg >= iDmgMax)
	{
		//TODO:清空伤害图标
		*iDmg = 0;
		PlaySoundByName("misc/UI_SPECIALKILL2.wav", 1);
		ShowPanel(true);
		SetAlpha(255);
		vgui::GetAnimationController()->RunAnimationCommand(m_pKillMarkPoint, "alpha", 0, 1.5f, 1.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		ShowPanel(false);
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