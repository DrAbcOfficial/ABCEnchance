#include <vector>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/ImagePanel.h>

#include "local.h"
#include "vguilocal.h"
#include <mymathlib.h>
#include <CVector.h>

#include "Viewport.h"

#include "plugins.h"
#include "cfefx.h"


#define VIEWPORT_KILLMARK_NAME "KillMarkPanel"
#define VIEWPORT_DMGMARK_NAME "DmgMarkPanel"
//#define KILLMARK_POINT "KillMark_Point"
//#define KiLLMARK_OKILL "KillMark_OKill"
//#define KILLMARK_DKILL "KillMark_DKill"
//#define KILLMARK_TKILL "KillMark_TKill"
//#define KILLMARK_MKILL "KIllMark_MKill"



CKillMarkPanel::CKillMarkPanel() : BaseClass(nullptr, VIEWPORT_KILLMARK_NAME)
{
	//vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "KillMarkScheme.res", "KillMarkScheme");
	SetScheme("KillMarkScheme");
	m_pKillMark = new vgui::ImagePanel(this, "KillMarkPoint");
	m_pKillMark->SetImage("abcenchance/tga/cfefx/BossDamage");
	m_pKillMark->SetShouldScaleImage(true);
	SetPos(mathlib::GetScreenPixel(ScreenWidth, 0.464), mathlib::GetScreenPixel(ScreenHeight, 0.768));
	SetSize(mathlib::GetScreenPixel(ScreenWidth, 0.087), mathlib::GetScreenPixel(ScreenHeight, 0.112));
	//LoadControlSettings(VGUI2_ROOT_DIR "KillMark.res");
	SetVisible(false);
}

CKillMarkPanel::~CKillMarkPanel()
{
	m_pKillMark->DeletePanel();
}

void CKillMarkPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(Color(0, 0, 0, 0));

	//m_pKillMark->SetBgColor(GetSchemeColor("KillMark.BackGoundColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}



bool CKillMarkPanel::IsVisible() {
	return BaseClass::IsVisible();
}

void CKillMarkPanel::ShowPanel(bool state)
{
	if (state == IsVisible())
		return;
	SetVisible(state);

}

void CKillMarkPanel::SetSize(int w, int t) {
	BaseClass::SetSize(w, t);
	m_pKillMark->SetSize(w, t);
}
vgui::VPANEL CKillMarkPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}

void CKillMarkPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

void CKillMarkPanel::OnThink()
{
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if (!local)
		return;
	ShowPanel(true);
	float a = m_pKillMark->GetAlpha();
	a -= 0.2;
	if (a <= 0)
		DeletePanel();
	else
		m_pKillMark->SetAlpha(a);
}

const char* CKillMarkPanel::GetName() {
	return VIEWPORT_KILLMARK_NAME;
}

void CKillMarkPanel::Reset() {
	ShowPanel(false);
}



//TODO:屏幕中心移动到右边

CDmgMarkPanel::CDmgMarkPanel() : BaseClass(nullptr, VIEWPORT_KILLMARK_NAME)
{
	//vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "KillMarkScheme.res", "KillMarkScheme");
	SetScheme("KillMarkScheme");
	m_pDmgMark = new vgui::ImagePanel(this, "DmgMark");
	m_pDmgMark->SetImage("abcenchance/tga/cfefx/NoBossSide100");
	m_pDmgMark->SetShouldScaleImage(true);

	SetPos(ScreenWidth / 2, ScreenHeight / 2);
	SetSize(ScreenWidth * 0.085, ScreenHeight * 0.112);
	//LoadControlSettings(VGUI2_ROOT_DIR "KillMark.res");
	SetVisible(true);
}

CDmgMarkPanel::~CDmgMarkPanel()
{
	m_pDmgMark->DeletePanel();
}

void CDmgMarkPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(Color(0, 0, 0, 0));

	//m_pKillMark->SetBgColor(GetSchemeColor("KillMark.BackGoundColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}



bool CDmgMarkPanel::IsVisible() {
	return BaseClass::IsVisible();
}

void CDmgMarkPanel::ShowPanel(bool state)
{
	if (state == IsVisible())
		return;
	SetVisible(state);

}

void CDmgMarkPanel::SetSize(int w, int t) {
	BaseClass::SetSize(w, t);
	m_pDmgMark->SetSize(w, t);
}
vgui::VPANEL CDmgMarkPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}

void CDmgMarkPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

void CDmgMarkPanel::OnThink()
{
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if (!local)
		return;
	ShowPanel(true);
	g_pViewPort;
}

const char* CDmgMarkPanel::GetName() {
	return VIEWPORT_KILLMARK_NAME;
}

void CDmgMarkPanel::Reset() {
	ShowPanel(false);
}




