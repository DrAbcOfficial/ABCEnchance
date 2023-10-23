#pragma once
#include <metahook.h>

#include "event_api.h"
#include "triangleapi.h"
#include <pm_defs.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui2/spr_image.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/AnimationController.h>
#include "ammobar.h"

#include "hud.h"
#include "local.h"
#include "vguilocal.h"
#include <CVector.h>

#include "mymathlib.h"
#include <weapon.h>
#include "Viewport.h"

#include "crosshair.h"
#include <exportfuncs.h>

#define VIEWPORT_CROSSHAIR_NAME "CrosshairPanel"
CCrosshairPanel::CCrosshairPanel()
	: BaseClass(nullptr, VIEWPORT_CROSSHAIR_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);

	gCVars.pDynamicCrossHair = CREATE_CVAR("cl_crosshair", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		g_pViewPort->ShowCrossHair(cvar->value > 0);
	});
	pDynamicCrossHairAH = CREATE_CVAR("cl_crosshairautohide", "1", FCVAR_VALUE, NULL);
	pDynamicCrossHairL = CREATE_CVAR("cl_crosshairsize", "24", FCVAR_VALUE, NULL);
	pDynamicCrossHairW = CREATE_CVAR("cl_crosshairthickness", "2", FCVAR_VALUE, NULL);
	pDynamicCrossHairO = CREATE_CVAR("cl_crosshairgap", "16", FCVAR_VALUE, NULL);
	pDynamicCrossHairM = CREATE_CVAR("cl_crosshairmultiple", "3", FCVAR_VALUE, NULL);
	pDynamicCrossHairA = CREATE_CVAR("cl_crosshairalpha", "200", FCVAR_VALUE, NULL);
	pDynamicCrossHairCR = CREATE_CVAR("cl_crosshaircolor_r", "50", FCVAR_VALUE, NULL);
	pDynamicCrossHairCG = CREATE_CVAR("cl_crosshaircolor_g", "250", FCVAR_VALUE, NULL);
	pDynamicCrossHairCB = CREATE_CVAR("cl_crosshaircolor_b", "50", FCVAR_VALUE, NULL);
	pDynamicCrossHairOA = CREATE_CVAR("cl_crosshair_outline_alpha", "255", FCVAR_VALUE, NULL);
	pDynamicCrossHairOR = CREATE_CVAR("cl_crosshaircolor_outline_r", "0", FCVAR_VALUE, NULL);
	pDynamicCrossHairOG = CREATE_CVAR("cl_crosshaircolor_outline_g", "0", FCVAR_VALUE, NULL);
	pDynamicCrossHairOB = CREATE_CVAR("cl_crosshaircolor_outline_b", "0", FCVAR_VALUE, NULL);
	pDynamicCrossHairOTD = CREATE_CVAR("cl_crosshair_outline_draw", "1", FCVAR_VALUE, NULL);
	pDynamicCrossHairOTDW = CREATE_CVAR("cl_crosshair_outline", "1", FCVAR_VALUE, NULL);
	pDynamicCrossHairT = CREATE_CVAR("cl_crosshair_t", "0", FCVAR_VALUE, NULL);
	pDynamicCrossHairD = CREATE_CVAR("cl_crosshairdot", "0", FCVAR_VALUE, NULL);

	pCvarDefaultCrosshair = CVAR_GET_POINTER("crosshair");

	SetSize(ScreenWidth, ScreenHeight);

	m_aryCrosshairBorder = {
		new vgui::Panel(this, "center"),
		new vgui::Panel(this, "top"),
		new vgui::Panel(this, "bottom"),
		new vgui::Panel(this, "left"),
		new vgui::Panel(this, "right")
	};
	m_aryCrosshair = {
		new vgui::Panel(this, "center"),
		new vgui::Panel(this, "top"),
		new vgui::Panel(this, "bottom"),
		new vgui::Panel(this, "left"),
		new vgui::Panel(this, "right")
	};
	m_pSprImage = new vgui::CSPRImage();
	m_pSprImage->SetRenderMode(kRenderTransTexture);
	m_pSprImage->SetColor(Color(255, 255, 255, 255));
}
void CCrosshairPanel::PaintBackground() {
	if (g_pViewPort->IsInSpectate())
		return;
	if (g_pViewPort->IsHudHide(HUD_HIDEALL | HUD_HIDEWEAPONS))
		return;
	if (!g_pViewPort->HasSuit())
		return;
	if (!gCVars.pDynamicCrossHair->value)
		return;
	//默认准心
	if (pCvarDefaultCrosshair->value > 0)
		DrawDefaultCrosshair(m_iCenterX, m_iCenterY);
	if (gClientData->health <= 0)
		return;
	if (pDynamicCrossHairAH->value > 0 && gCVars.pCvarDefaultFOV->value != m_hfov)
		return;

}
void CCrosshairPanel::OnThink() {
	if (!gClientData)
		return;
	if (gExportfuncs.CL_IsThirdPerson()) {
		pmtrace_t tr;
		CVector vViewAngleForward;
		gEngfuncs.GetViewAngles(vViewAngleForward);
		cl_entity_s* local = gEngfuncs.GetLocalPlayer();
		mathlib::AngleVectors(vViewAngleForward, vViewAngleForward, nullptr, nullptr);
		CVector vecSrc = local->curstate.origin;
		CVector viewOfs;
		gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(viewOfs);
		vecSrc += viewOfs;
		vViewAngleForward *= 8192;
		CVector vecEnd = vecSrc + vViewAngleForward;
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_NORMAL, local->index, &tr);
		CVector vecHUD;
		gEngfuncs.pTriAPI->WorldToScreen(tr.endpos, vecHUD);
		m_iCenterX = (1.0f + vecHUD[0]) * ScreenWidth / 2;
		m_iCenterY = (1.0f - vecHUD[1]) * ScreenHeight / 2;
	}
	else {
		m_iCenterX = ScreenWidth / 2;
		m_iCenterY = ScreenHeight / 2;
	}
	int iOffset = pDynamicCrossHairO->value;
	int iDrift = fabs(g_pViewPort->m_vecClientEVPunch[0]) + fabs(g_pViewPort->m_vecClientEVPunch[1]);
	if (iDrift <= 0)
		iDrift = fabs(gClientData->punchangle[0]) + fabs(gClientData->punchangle[1]);
	iDrift = iDrift * 5 * pDynamicCrossHairM->value;
	int r = pDynamicCrossHairCR->value,
		g = pDynamicCrossHairCG->value,
		b = pDynamicCrossHairCB->value,
		a = pDynamicCrossHairA->value;
	int otr = pDynamicCrossHairOR->value,
		otg = pDynamicCrossHairOG->value,
		otb = pDynamicCrossHairOB->value,
		ota = pDynamicCrossHairOA->value;
	int iWidth = pDynamicCrossHairW->value;
	int iLength = pDynamicCrossHairL->value;
	int iOutLineWidth = pDynamicCrossHairOTDW->value;
	int iWidthOffset = iWidth / 2;
	int iFinalOffset = iDrift + iOffset + iWidthOffset;
	//上色
	Color c = Color(r, g, b, a);
	for (auto iter = m_aryCrosshair.begin() + 2; iter != m_aryCrosshair.end(); iter++){
		(*iter)->SetBgColor(c);
	}
	m_aryCrosshair[0]->SetBgColor(pDynamicCrossHairD->value > 0 ? c : Color(0, 0, 0, 0));
	m_aryCrosshair[1]->SetBgColor(pDynamicCrossHairT->value <= 0 ? c : Color(0, 0, 0, 0));
	//位置大小
	//顺序中心上下左右
	m_aryCrosshair[0]->SetBounds(m_iCenterX - iWidthOffset, m_iCenterY - iWidthOffset, iWidth, iWidth);
	m_aryCrosshair[1]->SetBounds(m_iCenterX - iWidthOffset, m_iCenterY - iFinalOffset - iLength, iWidth, iLength);
	m_aryCrosshair[2]->SetBounds(m_iCenterX - iWidthOffset, m_iCenterY + iFinalOffset, iWidth, iLength);
	m_aryCrosshair[3]->SetBounds(m_iCenterX - iFinalOffset - iLength, m_iCenterY - iWidthOffset, iLength, iWidth);
	m_aryCrosshair[4]->SetBounds(m_iCenterX + iFinalOffset, m_iCenterY - iWidthOffset, iLength, iWidth);
	//描边
	if (pDynamicCrossHairOTD->value) {
		Color oc = Color(otr, otg, otb, ota);
		for (auto iter = m_aryCrosshairBorder.begin(); iter != m_aryCrosshairBorder.end(); iter++) {
			(*iter)->SetBgColor(oc);
		}
		m_aryCrosshairBorder[0]->SetAlpha(pDynamicCrossHairD->value > 0 ? ota : 0);
		m_aryCrosshairBorder[1]->SetAlpha(pDynamicCrossHairT->value <= 0 ? ota : 0);
		m_aryCrosshairBorder[0]->SetBounds(
			m_iCenterX - iWidthOffset - iOutLineWidth,
			m_iCenterY - iWidthOffset - iOutLineWidth,
			2 * iOutLineWidth + iWidth,
			2 * iOutLineWidth + iWidth
		);
		m_aryCrosshairBorder[1]->SetBounds(
			m_iCenterX - iWidthOffset - iOutLineWidth,
			m_iCenterY - iFinalOffset - iLength - iOutLineWidth,
			2 * iOutLineWidth + iWidth,
			2 * iOutLineWidth + iLength
		);
		m_aryCrosshairBorder[2]->SetBounds(
			m_iCenterX - iWidthOffset - iOutLineWidth,
			m_iCenterY + iFinalOffset - iOutLineWidth,
			2 * iOutLineWidth + iWidth,
			2 * iOutLineWidth + iLength
		);
		m_aryCrosshairBorder[3]->SetBounds(
			m_iCenterX - iFinalOffset - iLength - iOutLineWidth,
			m_iCenterY - iWidthOffset - iOutLineWidth,
			2 * iOutLineWidth + iLength,
			2 * iOutLineWidth + iWidth
		);
		m_aryCrosshairBorder[4]->SetBounds(
			m_iCenterX + iFinalOffset - iOutLineWidth,
			m_iCenterY - iWidthOffset - iOutLineWidth,
			2 * iOutLineWidth + iLength,
			2 * iOutLineWidth + iWidth
		);
	}
}
const char* CCrosshairPanel::GetName() {
	return VIEWPORT_CROSSHAIR_NAME;
}
void CCrosshairPanel::Reset() {
	m_pSprImage->Reset();
	m_pHandledWeapon = nullptr;
}

void CCrosshairPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CCrosshairPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CCrosshairPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CCrosshairPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

void CCrosshairPanel::SetCrosshairSPR(int x, int y, int hPic, wrect_t* hRc) {
	if (hPic <= 0) {
		m_pSprImage->SetTextureID(-1);
		return;
	}
	int w = hRc->right - hRc->left;
	int h = hRc->bottom - hRc->top;
	m_pSprImage->SetTextureID(hPic);
	m_pSprImage->SetPos(x - w / 2, y - h / 2);
	m_pSprImage->SetSize(w, h);
	m_pSprImage->SetRect(hRc->left, hRc->right, hRc->top, hRc->bottom);
}
void CCrosshairPanel::SetWeapon(WEAPON* weapon) {
	m_pHandledWeapon = weapon;
	if (!m_pHandledWeapon || m_pHandledWeapon->iId <= 0) {
		ShowPanel(false);
		return;
	}
	else if (!(m_pHandledWeapon->iState & CAmmoPanel::WEAPONSTATE::VALID))
		return;
	ShowPanel(true);
}
void CCrosshairPanel::DrawDefaultCrosshair(int x, int y) {
	if (!m_pHandledWeapon)
		return;
	bool bOnTarget = m_pHandledWeapon->iState & CAmmoPanel::WEAPONSTATE::ONTARGET;
	if (m_hfov >= gCVars.pCvarDefaultFOV->value) {
		if (bOnTarget && m_pHandledWeapon->hAutoaim)
			SetCrosshairSPR(x, y, m_pHandledWeapon->hAutoaim, &m_pHandledWeapon->rcAutoaim);
		else
			SetCrosshairSPR(x, y, m_pHandledWeapon->hCrosshair, &m_pHandledWeapon->rcCrosshair);
	}
	else {
		if (bOnTarget && m_pHandledWeapon->hZoomedAutoaim)
			SetCrosshairSPR(x, y, m_pHandledWeapon->hZoomedAutoaim, &m_pHandledWeapon->rcZoomedAutoaim);
		else
			SetCrosshairSPR(x, y, m_pHandledWeapon->hZoomedCrosshair, &m_pHandledWeapon->rcZoomedCrosshair);
	}
	m_pSprImage->Paint();
}