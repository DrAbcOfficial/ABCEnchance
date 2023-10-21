﻿#pragma once
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

	m_pCrosshair = {
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
	int iCenterX;
	int iCenterY;
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
		iCenterX = (1.0f + vecHUD[0]) * ScreenWidth / 2;
		iCenterY = (1.0f - vecHUD[1]) * ScreenHeight / 2;
	}
	else {
		iCenterX = ScreenWidth / 2;
		iCenterY = ScreenHeight / 2;
	}
	//默认准心
	if (pCvarDefaultCrosshair->value > 0)
		DrawDefaultCrosshair(iCenterX, iCenterY);

	if (gClientData->health <= 0)
		return;
	if (pDynamicCrossHairAH->value > 0 && gCVars.pCvarDefaultFOV->value != m_hfov)
		return;
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
	for (auto iter = m_pCrosshair.begin(); iter != m_pCrosshair.end(); iter++)
	{
		(*iter)->SetBgColor(Color(r, g, b, a));
	}
	if (pDynamicCrossHairD)
		m_pCrosshair[0]->SetAlpha(0);
	else
		m_pCrosshair[0]->SetBgColor(Color(r, g, b, a));
	if (pDynamicCrossHairT)
		m_pCrosshair[1]->SetAlpha(0);
	else
		m_pCrosshair[1]->SetBgColor(Color(r, g, b, a));

	//位置
	//顺序上下左右
	m_pCrosshair[1]->SetPos(iCenterX - iWidthOffset, iCenterY - iFinalOffset - iLength);
	m_pCrosshair[2]->SetPos(iCenterX - iWidthOffset, iCenterY + iFinalOffset);
	m_pCrosshair[3]->SetPos(iCenterX - iFinalOffset - iLength, iCenterY - iWidthOffset);
	m_pCrosshair[4]->SetPos(iCenterX + iFinalOffset, iCenterY - iWidthOffset);
	//中心
	m_pCrosshair[0]->SetBounds(
		iCenterX - iWidthOffset,
		iCenterY - iWidthOffset,
		iWidth,
		iWidth
	);
	//大小
	//顺序上下左右
	m_pCrosshair[1]->SetSize(iWidth, iLength);
	m_pCrosshair[2]->SetSize(iWidth, iLength);
	m_pCrosshair[3]->SetSize(iLength, iWidth);
	m_pCrosshair[4]->SetSize(iLength, iWidth);
	//动画
	vgui::GetAnimationController()->RunAnimationCommand(m_pCrosshair[1], "ypos", iCenterY - iOffset + iWidthOffset - iLength + 5 * pDynamicCrossHairM->value, 0, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	vgui::GetAnimationController()->RunAnimationCommand(m_pCrosshair[2], "ypos", iCenterY + iOffset + iWidthOffset + 5 * pDynamicCrossHairM->value, 0, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	vgui::GetAnimationController()->RunAnimationCommand(m_pCrosshair[3], "xpos", iCenterX - iOffset + iWidthOffset - iLength + 5 * pDynamicCrossHairM->value, 0, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	vgui::GetAnimationController()->RunAnimationCommand(m_pCrosshair[4], "xpos", iCenterX + iOffset + iWidthOffset + 5 * pDynamicCrossHairM->value, 0, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);


	////描边
	//if (pDynamicCrossHairOTD->value) {
	//	vgui::IntRect outlinerects[4];
	//	//左
	//	vgui::surface()->DrawSetColor(otr, otg, otb, ota);
	//	outlinerects[0] = {
	//		iCenterX - iFinalOffset - iLength - iOutLineWidth,
	//		iCenterY - iWidthOffset - iOutLineWidth,
	//		iCenterX - iFinalOffset + iOutLineWidth,
	//		iCenterY - iWidthOffset + iWidth + iOutLineWidth
	//	};
	//	//右
	//	outlinerects[1] = {
	//		iCenterX + iFinalOffset - iOutLineWidth,
	//		iCenterY - iWidthOffset - iOutLineWidth,
	//		iCenterX + iFinalOffset + iOutLineWidth + iLength,
	//		iCenterY - iWidthOffset + iOutLineWidth + iWidth
	//	};
	//	//下
	//	outlinerects[2] = {
	//		iCenterX - iWidthOffset - iOutLineWidth,
	//		iCenterY + iFinalOffset - iOutLineWidth,
	//		iCenterX - iWidthOffset + iOutLineWidth + iWidth,
	//		iCenterY + iFinalOffset + iOutLineWidth + iLength
	//	};
	//	//上
	//	if (!pDynamicCrossHairT->value) {
	//		outlinerects[3] = {
	//			iCenterX - iWidthOffset - iOutLineWidth,
	//			iCenterY - iFinalOffset - iLength - iOutLineWidth,
	//			iCenterX - iWidthOffset + iOutLineWidth + iWidth,
	//			iCenterY - iFinalOffset + iOutLineWidth
	//		};
	//	}
	//	vgui::surface()->DrawFilledRectArray(outlinerects, !pDynamicCrossHairT->value ? 4 : 3);
	//}
	////中心
	//if (pDynamicCrossHairD->value) {
	//	vgui::surface()->DrawSetColor(otr, otg, otb, ota);
	//	vgui::surface()->DrawFilledRect(
	//		iCenterX - iWidthOffset - iOutLineWidth,
	//		iCenterY - iWidthOffset - iOutLineWidth,
	//		iCenterX - iWidthOffset + iWidth + iOutLineWidth,
	//		iCenterY - iWidthOffset + iWidth + iOutLineWidth);
	//	vgui::surface()->DrawSetColor(r, g, b, a);
	//	vgui::surface()->DrawFilledRect(
	//		iCenterX - iWidthOffset,
	//		iCenterY - iWidthOffset,
	//		iCenterX - iWidthOffset + iWidth,
	//		iCenterY - iWidthOffset + iWidth);
	//}
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