#include <metahook.h>
#include "hud.h"
#include "local.h"
#include "vguilocal.h"
#include <cmath>
#include "pm_defs.h"
#include "event_api.h"
#include "triangleapi.h"
#include <mathlib.h>
#include "weapon.h"
#include "CHudDelegate.h"
#include "exportfuncs.h"

#include "crosshair.h"
#include <ammo.h>


CHudCustomCrosshair m_HudCrosshair;

int CHudCustomCrosshair::Init(void)
{
	gCVars.pDynamicCrossHair = gEngfuncs.pfnRegisterVariable("cl_crosshair", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairAH = gEngfuncs.pfnRegisterVariable("cl_crosshairautohide", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairL = gEngfuncs.pfnRegisterVariable("cl_crosshairsize", "24", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairW = gEngfuncs.pfnRegisterVariable("cl_crosshairthickness", "2", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairO = gEngfuncs.pfnRegisterVariable("cl_crosshairgap", "16", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairM = gEngfuncs.pfnRegisterVariable("cl_crosshairmultiple", "3", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairA = gEngfuncs.pfnRegisterVariable("cl_crosshairalpha", "200", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairCR = gEngfuncs.pfnRegisterVariable("cl_crosshaircolor_r", "50", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairCG = gEngfuncs.pfnRegisterVariable("cl_crosshaircolor_g", "250", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairCB = gEngfuncs.pfnRegisterVariable("cl_crosshaircolor_b", "50", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairOTD = gEngfuncs.pfnRegisterVariable("cl_crosshair_outline_draw", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairOTDW = gEngfuncs.pfnRegisterVariable("cl_crosshair_outline", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairT = gEngfuncs.pfnRegisterVariable("cl_crosshair_t", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairD = gEngfuncs.pfnRegisterVariable("cl_crosshairdot", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

	pCvarDefaultCrosshair = gEngfuncs.pfnGetCvarPointer("crosshair");

	pCvarDefaultFOV = gEngfuncs.pfnGetCvarPointer("default_fov");
	return 1;
}
int CHudCustomCrosshair::Draw(float flTime)
{
	if (gHudDelegate->IsInSpectate())
		return 1;
	int iCenterX;
	int iCenterY;
	if (gExportfuncs.CL_IsThirdPerson())
	{
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
		vForward[0] *= 8192;
		vForward[1] *= 8192;
		vForward[2] *= 8192;
		VectorAdd(vecSrc, vForward, vecEnd);
		physent_t* pPhyplayer = gEngfuncs.pEventAPI->EV_GetPhysent(local->index);
		if (!pPhyplayer)
			return NULL;
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_NORMAL, pPhyplayer->info, &tr);

		vec3_t vecHUD;
		gEngfuncs.pTriAPI->WorldToScreen(tr.endpos, vecHUD);
		iCenterX = (1.0f + vecHUD[0]) * gScreenInfo.iWidth / 2;
		iCenterY = (1.0f - vecHUD[1]) * gScreenInfo.iHeight / 2;
	}
	else
	{
		iCenterX = gScreenInfo.iWidth / 2;
		iCenterY = gScreenInfo.iHeight / 2;
	}
	//默认准心
	if (pCvarDefaultCrosshair->value > 0)
		DrawDefaultCrosshair(flTime, iCenterX, iCenterY);
	if (gCVars.pDynamicCrossHair->value > 0)
	{
		if (gClientData->health <= 0)
			return 0;

		if (gCVars.pDynamicCrossHairAH->value > 0 && pCvarDefaultFOV->value != m_hfov)
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
		if (gCVars.pDynamicCrossHairOTD->value)
		{
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
void CHudCustomCrosshair::DrawCrosshairSPR(int x, int y, int hPic, wrect_t hRc)
{
	if (hPic <= 0)
		return;
	int rx = x + 1 - (hRc.right - hRc.left) / 2;
	int ry = y + 1 - (hRc.bottom - hRc.top) / 2;
	SPR_Set(hPic, 255, 255, 255);
	SPR_DrawHoles(0, rx, ry, &hRc);
}
void CHudCustomCrosshair::DrawDefaultCrosshair(float flTime, int x, int y)
{
	WEAPON* pWeapon = m_HudCustomAmmo.m_pWeapon;
	if (!pWeapon)
		return;
	if (pWeapon->iId <= 0)
		return;
	bool bOnTarget = m_HudCustomAmmo.m_bIsOnTarget;
	if (m_hfov >= pCvarDefaultFOV->value)
	{
		if (bOnTarget && pWeapon->hAutoaim)
			DrawCrosshairSPR(x, y, pWeapon->hAutoaim, pWeapon->rcAutoaim);
		else
			DrawCrosshairSPR(x, y, pWeapon->hCrosshair, pWeapon->rcCrosshair);
	}
	else
	{
		if (bOnTarget && m_HudCustomAmmo.m_pWeapon->hZoomedAutoaim)
			DrawCrosshairSPR(x, y, pWeapon->hZoomedAutoaim, pWeapon->rcZoomedAutoaim);
		else
			DrawCrosshairSPR(x, y, pWeapon->hZoomedCrosshair, pWeapon->rcZoomedCrosshair);
	}
}
