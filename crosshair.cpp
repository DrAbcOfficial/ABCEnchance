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

	pCvarDefaultFOV = gEngfuncs.pfnGetCvarPointer("default_fov");
	return 1;
}

int CHudCustomCrosshair::Draw(float flTime)
{
	if (gCVars.pDynamicCrossHair->value > 0)
	{
		if (gClientData->health <= 0)
			return 0;

		if (gCVars.pDynamicCrossHairAH->value > 0 && pCvarDefaultFOV->value != m_hfov)
			return 0;
		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
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
			vForward[0] *= 4096;
			vForward[1] *= 4096;
			vForward[2] *= 4096;
			VectorAdd(vecSrc, vForward, vecEnd);
			physent_t* pPhyplayer = gEngfuncs.pEventAPI->EV_GetPhysent(local->index);
			if (!pPhyplayer)
				return NULL;
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, pPhyplayer->info, &tr);

			vec3_t vecHUD;
			gEngfuncs.pTriAPI->WorldToScreen(tr.endpos, vecHUD);
			iCenterX = (1.0f + vecHUD[0]) * gScreenInfo.iWidth / 2;
			iCenterY = (1.0f - vecHUD[1]) * gScreenInfo.iHeight / 2;
		}
		else
		{
			iCenterX = gScreenInfo.iWidth / 2 - 1;
			iCenterY = gScreenInfo.iHeight / 2 - 1;
		}
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

void CHudCustomCrosshair::Reset(void)
{
}
