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

#include "hud.h"
#include "local.h"
#include "vguilocal.h"
#include <CVector.h>

#include "mymathlib.h"

#include "crosshair.h"
#include "Viewport.h"
#include <weapon.h>
#include <ammo.h>
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
	//描边
	if (pDynamicCrossHairOTD->value) {
		vgui::IntRect outlinerects[4];
		//左
		vgui::surface()->DrawSetColor(otr, otg, otb, ota);
		outlinerects[0] = {
			iCenterX - iFinalOffset - iLength - iOutLineWidth,
			iCenterY - iWidthOffset - iOutLineWidth,
			iCenterX - iFinalOffset + iOutLineWidth,
			iCenterY - iWidthOffset + iWidth + iOutLineWidth
		};
		//右
		outlinerects[1] = {
			iCenterX + iFinalOffset - iOutLineWidth,
			iCenterY - iWidthOffset - iOutLineWidth,
			iCenterX + iFinalOffset + iOutLineWidth + iLength,
			iCenterY - iWidthOffset + iOutLineWidth + iWidth
		};
		//下
		outlinerects[2] = {
			iCenterX - iWidthOffset - iOutLineWidth,
			iCenterY + iFinalOffset - iOutLineWidth,
			iCenterX - iWidthOffset + iOutLineWidth + iWidth,
			iCenterY + iFinalOffset + iOutLineWidth + iLength
		};
		//上
		if (!pDynamicCrossHairT->value) {
			outlinerects[3] = {
				iCenterX - iWidthOffset - iOutLineWidth,
				iCenterY - iFinalOffset - iLength - iOutLineWidth,
				iCenterX - iWidthOffset + iOutLineWidth + iWidth,
				iCenterY - iFinalOffset + iOutLineWidth
			};
		}
		vgui::surface()->DrawFilledRectArray(outlinerects, !pDynamicCrossHairT->value ? 4 : 3);
	}
	//中心
	if (pDynamicCrossHairD->value) {
		vgui::surface()->DrawSetColor(otr, otg, otb, ota);
		vgui::surface()->DrawFilledRect(
			iCenterX - iWidthOffset - iOutLineWidth,
			iCenterY - iWidthOffset - iOutLineWidth,
			iCenterX - iWidthOffset + iWidth + iOutLineWidth,
			iCenterY - iWidthOffset + iWidth + iOutLineWidth);
		vgui::surface()->DrawSetColor(r, g, b, a);
		vgui::surface()->DrawFilledRect(
			iCenterX - iWidthOffset,
			iCenterY - iWidthOffset,
			iCenterX - iWidthOffset + iWidth,
			iCenterY - iWidthOffset + iWidth);
	}
	//左
	vgui::surface()->DrawSetColor(r, g, b, a);
	vgui::IntRect rects[4];
	rects[0] = {
		iCenterX - iFinalOffset - iLength,
		iCenterY - iWidthOffset,
		iCenterX - iFinalOffset,
		iCenterY - iWidthOffset + iWidth
	};

	//右
	rects[1] = {
		iCenterX + iFinalOffset,
		iCenterY - iWidthOffset,
		iCenterX + iFinalOffset + iLength,
		iCenterY - iWidthOffset + iWidth
	};
	//下
	rects[2] = {
		iCenterX - iWidthOffset,
		iCenterY + iFinalOffset,
		iCenterX - iWidthOffset + iWidth,
		iCenterY + iFinalOffset + iLength
	};
	//上
	if (!pDynamicCrossHairT->value) {
		rects[3] = {
			iCenterX - iWidthOffset,
			iCenterY - iFinalOffset - iLength,
			iCenterX - iWidthOffset + iWidth,
			iCenterY - iFinalOffset
		};
	}
	vgui::surface()->DrawFilledRectArray(rects, !pDynamicCrossHairT->value ? 4 : 3);
}
const char* CCrosshairPanel::GetName() {
	return VIEWPORT_CROSSHAIR_NAME;
}
void CCrosshairPanel::Reset() {
	ShowPanel(true);
	m_pSprImage->Reset();
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
void CCrosshairPanel::DrawDefaultCrosshair(int x, int y) {
	WEAPON* pWeapon = m_HudCustomAmmo.m_pWeapon;
	if (!pWeapon || pWeapon->iId <= 0) {
		m_pSprImage->SetTextureID(-1);
		return;
	}
	bool bOnTarget = m_HudCustomAmmo.m_bIsOnTarget;
	if (m_hfov >= gCVars.pCvarDefaultFOV->value) {
		if (bOnTarget && pWeapon->hAutoaim)
			SetCrosshairSPR(x, y, pWeapon->hAutoaim, &pWeapon->rcAutoaim);
		else
			SetCrosshairSPR(x, y, pWeapon->hCrosshair, &pWeapon->rcCrosshair);
	}
	else {
		if (bOnTarget && m_HudCustomAmmo.m_pWeapon->hZoomedAutoaim)
			SetCrosshairSPR(x, y, pWeapon->hZoomedAutoaim, &pWeapon->rcZoomedAutoaim);
		else
			SetCrosshairSPR(x, y, pWeapon->hZoomedCrosshair, &pWeapon->rcZoomedCrosshair);
	}
	m_pSprImage->Paint();
}