#include <metahook.h>
#include <cmath>
#include "pm_defs.h"
#include "event_api.h"
#include "triangleapi.h"

#include "cvar_hook.h"
#include "mathlib.h"

#include "hud.h"
#include "local.h"
#include "vguilocal.h"
#include "myconst.h"

#include "glew.h"
#include "weapon.h"
#include "ammo.h"

#include "CHudDelegate.h"
#include "exportfuncs.h"

#include "crosshair.h"

CHudCustomCrosshair m_HudCrosshair;
using namespace mathlib;

int CHudCustomCrosshair::Init(void){
	gCVars.pDynamicCrossHair = CREATE_CVAR("cl_crosshair", "1", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairAH = CREATE_CVAR("cl_crosshairautohide", "1", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairL = CREATE_CVAR("cl_crosshairsize", "24", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairW = CREATE_CVAR("cl_crosshairthickness", "2", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairO = CREATE_CVAR("cl_crosshairgap", "16", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairM = CREATE_CVAR("cl_crosshairmultiple", "3", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairA = CREATE_CVAR("cl_crosshairalpha", "200", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairCR = CREATE_CVAR("cl_crosshaircolor_r", "50", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairCG = CREATE_CVAR("cl_crosshaircolor_g", "250", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairCB = CREATE_CVAR("cl_crosshaircolor_b", "50", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairOTD = CREATE_CVAR("cl_crosshair_outline_draw", "1", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairOTDW = CREATE_CVAR("cl_crosshair_outline", "1", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairT = CREATE_CVAR("cl_crosshair_t", "0", FCVAR_VALUE, NULL);
	gCVars.pDynamicCrossHairD = CREATE_CVAR("cl_crosshairdot", "0", FCVAR_VALUE, NULL);

	pCvarDefaultCrosshair = gEngfuncs.pfnGetCvarPointer("crosshair");
	return 1;
}
int CHudCustomCrosshair::Draw(float flTime){
	if (gHudDelegate->IsInSpectate())
		return 1;
	int iCenterX;
	int iCenterY;
	if (gExportfuncs.CL_IsThirdPerson()){
		pmtrace_t tr;
		vec3_t vForward;
		vec3_t vViewAngle;
		gEngfuncs.GetViewAngles(vViewAngle);
		cl_entity_s* local = gEngfuncs.GetLocalPlayer();
		AngleVectors(vViewAngle, vForward, NULL, NULL);
		vec3_t vecSrc, viewOfs, vecEnd;
		VectorCopy(local->curstate.origin, vecSrc);
		gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(viewOfs);
		VectorAdd(vecSrc, viewOfs, vecSrc);
		VectorMultipiler(vForward, 8192);
		VectorAdd(vecSrc, vForward, vecEnd);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_NORMAL, local->index, &tr);

		vec3_t vecHUD;
		gEngfuncs.pTriAPI->WorldToScreen(tr.endpos, vecHUD);
		iCenterX = (1.0f + vecHUD[0]) * gScreenInfo.iWidth / 2;
		iCenterY = (1.0f - vecHUD[1]) * gScreenInfo.iHeight / 2;
	}
	else{
		iCenterX = gScreenInfo.iWidth / 2;
		iCenterY = gScreenInfo.iHeight / 2;
	}
	//默认准心
	if (pCvarDefaultCrosshair->value > 0)
		DrawDefaultCrosshair(flTime, iCenterX, iCenterY);
	if (gCVars.pDynamicCrossHair->value > 0){
		if (gClientData->health <= 0)
			return 0;
		if (gCVars.pDynamicCrossHairAH->value > 0 && gCVars.pCvarDefaultFOV->value != m_hfov)
			return 0;
		int iOffset = gCVars.pDynamicCrossHairO->value;
		int iDrift = fabs(gHudDelegate->m_vecClientEVPunch[0]) + fabs(gHudDelegate->m_vecClientEVPunch[1]);
		if (iDrift <= 0)
			iDrift = fabs(gClientData->punchangle[0]) + fabs(gClientData->punchangle[1]);
		iDrift = iDrift * 5 * gCVars.pDynamicCrossHairM->value;
		int r = gCVars.pDynamicCrossHairCR->value, g = gCVars.pDynamicCrossHairCG->value, b = gCVars.pDynamicCrossHairCB->value, a = gCVars.pDynamicCrossHairA->value;
		int iWidth = gCVars.pDynamicCrossHairW->value;
		int iLength = gCVars.pDynamicCrossHairL->value;
		int iWidthOffset = iWidth / 2;
		int iFinalOffset = iDrift + iOffset + iWidthOffset;
		//描边
		if (gCVars.pDynamicCrossHairOTD->value){
			int iOutLineWidth = gCVars.pDynamicCrossHairOTDW->value;
			//左
			gEngfuncs.pfnFillRGBABlend(iCenterX - iFinalOffset - iLength - iOutLineWidth, iCenterY - iWidthOffset - iOutLineWidth, iOutLineWidth * 2 + iLength, iOutLineWidth * 2 + iWidth, 0, 0, 0, a);
			//上
			if (!gCVars.pDynamicCrossHairT->value)
				gEngfuncs.pfnFillRGBABlend(iCenterX - iWidthOffset - iOutLineWidth, iCenterY - iFinalOffset - iLength - iOutLineWidth, iOutLineWidth * 2 + iWidth, iOutLineWidth * 2 + iLength, 0, 0, 0, a);
			//右
			gEngfuncs.pfnFillRGBABlend(iCenterX + iFinalOffset - iOutLineWidth, iCenterY - iWidthOffset - iOutLineWidth, iOutLineWidth * 2 + iLength, iOutLineWidth * 2 + iWidth, 0, 0, 0, a);
			//下
			gEngfuncs.pfnFillRGBABlend(iCenterX - iWidthOffset - iOutLineWidth, iCenterY + iFinalOffset - iOutLineWidth, iOutLineWidth * 2 + iWidth, iOutLineWidth * 2 + iLength, 0, 0, 0, a);
		}
		//中心
		if (gCVars.pDynamicCrossHairD->value)
			gEngfuncs.pfnFillRGBA(iCenterX - iWidthOffset, iCenterY - iWidthOffset, iWidth, iWidth, r, g, b, a);
		//左
		gEngfuncs.pfnFillRGBA(iCenterX - iFinalOffset - iLength, iCenterY - iWidthOffset, iLength, iWidth, r, g, b, a);
		//上
		if (!gCVars.pDynamicCrossHairT->value)
			gEngfuncs.pfnFillRGBA(iCenterX - iWidthOffset, iCenterY - iFinalOffset - iLength, iWidth, iLength, r, g, b, a);
		//右
		gEngfuncs.pfnFillRGBA(iCenterX + iFinalOffset, iCenterY - iWidthOffset, iLength, iWidth, r, g, b, a);
		//下
		gEngfuncs.pfnFillRGBA(iCenterX - iWidthOffset, iCenterY + iFinalOffset, iWidth, iLength, r, g, b, a);
	}
	return 1;
}
void CHudCustomCrosshair::DrawCrosshairSPR(int x, int y, int hPic, wrect_t hRc){
	if (hPic <= 0)
		return;
	int rx = x + 1 - (hRc.right - hRc.left) / 2;
	int ry = y + 1 - (hRc.bottom - hRc.top) / 2;
	SPR_Set(hPic, 255, 255, 255);
	SPR_DrawHoles(0, rx, ry, &hRc);
}
void CHudCustomCrosshair::DrawDefaultCrosshair(float flTime, int x, int y){
	WEAPON* pWeapon = m_HudCustomAmmo.m_pWeapon;
	if (!pWeapon)
		return;
	if (pWeapon->iId <= 0)
		return;
	bool bOnTarget = m_HudCustomAmmo.m_bIsOnTarget;
	if (m_hfov >= gCVars.pCvarDefaultFOV->value){
		if (bOnTarget && pWeapon->hAutoaim)
			DrawCrosshairSPR(x, y, pWeapon->hAutoaim, pWeapon->rcAutoaim);
		else
			DrawCrosshairSPR(x, y, pWeapon->hCrosshair, pWeapon->rcCrosshair);
	}
	else{
		if (bOnTarget && m_HudCustomAmmo.m_pWeapon->hZoomedAutoaim)
			DrawCrosshairSPR(x, y, pWeapon->hZoomedAutoaim, pWeapon->rcZoomedAutoaim);
		else
			DrawCrosshairSPR(x, y, pWeapon->hZoomedCrosshair, pWeapon->rcZoomedCrosshair);
	}
}
