#include "plugins.h"

#include "math.h"
#include "malloc.h"
#include "cl_entity.h"
#include "mathlib.h"
#include "com_model.h"
#include "triangleapi.h"

#include "hud.h"
#include "exportfuncs.h"
//HUD
#include "CColor.h"
#include "weapon.h"

#include "ammo.h"
#include "healthhud.h"
#include "basehealth.h"

#include "drawElement.h"

#include "CHudDelegate.h"

#include "local.h"
//efx
#include "efxenchance.h"

cl_enginefunc_t gEngfuncs;
cl_exportfuncs_t gExportfuncs;

const clientdata_t* gClientData;
float m_hfov;
float* pClientEVPunchAngles;

//PLAYER TITLE
void DrawPlayerTitle()
{
	if (gCVars.pPlayerTitle->value > 0)
	{
		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		float* localColor = gHookFuncs.GetClientColor(local->index);

		float dx, dy, dz;
		vec3_t vecHUD;
		vec3_t vecOrg;
		//水平夹角
		float levelAngle = 0;
		//俯仰角
		float elevation = 0;
		//角色夹角
		float fPlayerAngle_xy = 0;
		float fPlayerAngle_z = 0;
		//屏幕夹角
		float fPaintAngle_xy = 0;
		float fPaintAngle_z = 0;
		//视角角度
		vec3_t vecView;
		gEngfuncs.GetViewAngles(vecView);

		for (int i = 1; i <= 32; i++)
		{
			cl_entity_t* entity = gEngfuncs.GetEntityByIndex(i);

			if (!entity || entity->curstate.messagenum != local->curstate.messagenum || !entity->player || !entity->model || entity == local)
				continue;
			//计算我和目标的相对偏移
			dx = entity->curstate.origin[0] - local->curstate.origin[0];
			dy = entity->curstate.origin[1] - local->curstate.origin[1];
			dz = entity->curstate.origin[2] - local->curstate.origin[2];
			float fDistance = sqrt(pow(dx, 2) + pow(dy, 2));
			float* color = gHookFuncs.GetClientColor(i);
			if (fDistance >= 2048 || color != localColor)
				continue;

			vecOrg[0] = entity->curstate.origin[0];
			vecOrg[1] = entity->curstate.origin[1];
			vecOrg[2] = entity->curstate.origin[2] + 56;

			levelAngle = vecView[1];
			elevation = vecView[0];
			//角度补偿  -180 180 =》0 360
			if (levelAngle < 0)
				levelAngle = levelAngle + PERIGON_ANGLE;
			elevation = -elevation;
			//计算角色夹角
			if (dx > 0)
			{
				if (dy == 0)
					fPlayerAngle_xy = 0.0;
				else if (dy > 0)
					fPlayerAngle_xy = asin(dy / fDistance) * RADIAN_PER_DEGREE;
				else if (dy < 0)
					fPlayerAngle_xy = PERIGON_ANGLE + (asin(dy / fDistance) * RADIAN_PER_DEGREE);
			}
			//判断特殊角度
			else if (dx == 0)
			{
				if (dy > 0)
					fPlayerAngle_xy = 90.0;
				else if (dy < 0)
					fPlayerAngle_xy = 270.0;
			}
			else if (dx < 0)
			{
				if (dy == 0)
					fPlayerAngle_xy = FLAT_ANGLE;
				else if (dy > 0)
					fPlayerAngle_xy = FLAT_ANGLE - asin(dy / (fDistance)) * RADIAN_PER_DEGREE;
				else if (dy < 0)
					fPlayerAngle_xy = FLAT_ANGLE - asin(dy / (fDistance)) * RADIAN_PER_DEGREE;
			}
			//视角水平夹角
			fPaintAngle_xy = levelAngle - fPlayerAngle_xy;
			if (fPaintAngle_xy == FLAT_ANGLE || fPaintAngle_xy == -FLAT_ANGLE)
				fPaintAngle_xy = FLAT_ANGLE;
			else if (fPaintAngle_xy > FLAT_ANGLE)
				fPaintAngle_xy = fPaintAngle_xy - PERIGON_ANGLE;
			else if (fPaintAngle_xy < -FLAT_ANGLE)
				fPaintAngle_xy = fPaintAngle_xy + PERIGON_ANGLE;
			//视角俯仰夹角
			fPlayerAngle_z = asin(dz / (sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2)))) * RADIAN_PER_DEGREE;
			fPaintAngle_z = elevation - fPlayerAngle_z;

			gEngfuncs.pTriAPI->WorldToScreen(vecOrg, vecHUD);
			vecHUD[0] = (1.0f + vecHUD[0]) * gScreenInfo.iWidth / 2;
			vecHUD[1] = (1.0f - vecHUD[1]) * gScreenInfo.iHeight / 2;
			if (fPaintAngle_xy > -45 && fPaintAngle_xy < 45 && fPaintAngle_z > -45 && fPaintAngle_z < 45)
			{
				hud_player_info_t playerinfo = { 0 };
				gEngfuncs.pfnGetPlayerInfo(i, &playerinfo);
				if (playerinfo.name)
				{
					wchar_t wideName[MAX_PLAYER_NAME_LENGTH];
					pLocalize->ConvertANSIToUnicode(playerinfo.name, wideName, sizeof(wideName));
					int iSzWidth = 0;
					vgui::HFont hFont = pScheme->GetFont("MainShitFont", true);
					GetStringSize(wideName, &iSzWidth, NULL, hFont);
					DrawVGUI2String(wideName, vecHUD[0] - iSzWidth / 2, vecHUD[1], color[0], color[1], color[2], hFont);
				}
			}
		}
	}
}
//CROSSHAIR
void R_VectorScale(float* pucnangle1, float scale, float* pucnangle2)
{
	gHookFuncs.VectorScale(pucnangle1, scale, pucnangle2);
	pClientEVPunchAngles = pucnangle1;
}
void RedrawCorssHair()
{
	if (gCVars.pDynamicCrossHair->value > 0)
	{
		if (gClientData->health <= 0)
			return;

		if (gCVars.pDynamicCrossHairAH->value > 0)
		{
			float def_fov = gEngfuncs.pfnGetCvarFloat("default_fov");
			if (def_fov != m_hfov)
				return;
		}
		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		int iCenterX = gScreenInfo.iWidth / 2 - 1;
		int iCenterY = gScreenInfo.iHeight / 2 - 1;
		int iOffset = gCVars.pDynamicCrossHairO->value;
		int iDrift = fabs(pClientEVPunchAngles[0]) + fabs(pClientEVPunchAngles[1]);
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
		if(!gCVars.pDynamicCrossHairT->value)
			gEngfuncs.pfnFillRGBA(iCenterX - iWidthOffset, iCenterY - iFinalOffset - iLength, iWidth, iLength, r, g, b, a);
		//右
		gEngfuncs.pfnFillRGBA(iCenterX + iFinalOffset, iCenterY - iWidthOffset, iLength, iWidth, r, g, b, a);
		//下
		gEngfuncs.pfnFillRGBA(iCenterX - iWidthOffset, iCenterY + iFinalOffset, iWidth, iLength, r, g, b, a);
	}
}

//FINAL SHIT
void R_NewMap(void)
{
	gHudDelegate->HUD_Reset();
	gHookFuncs.R_NewMap();
}
void Sys_ErrorEx(const char* fmt, ...)
{
	char msg[4096] = { 0 };

	va_list argptr;

	va_start(argptr, fmt);
	_vsnprintf(msg, sizeof(msg), fmt, argptr);
	va_end(argptr);

	if (gEngfuncs.pfnClientCmd)
		gEngfuncs.pfnClientCmd("escape\n");

	MessageBox(NULL, msg, "Fatal Error", MB_ICONERROR);
	TerminateProcess((HANDLE)(-1), 0);
}
void FillDelegate()
{
	Fill_DelegateFunc(CL_TempEntAllocHigh);
	Fill_DelegateFunc(CL_TempEntAlloc);
	Fill_DelegateFunc(R_SparkEffect);
	Fill_DelegateFunc(R_BloodStream);
}
void FillAddress()
{
	auto engineFactory = Sys_GetFactory((HINTERFACEMODULE)g_dwEngineBase);
	if (engineFactory && engineFactory("EngineSurface007", NULL))
	{
#define R_NEWMAP_SIG "\x55\x8B\xEC\x51\xC7\x45\xFC\x00\x00\x00\x00\xEB\x2A\x8B\x45\xFC\x83\xC0\x01\x89\x45\xFC\x81\x7D\xFC\x00\x01\x00\x00"
		{
			gHookFuncs.R_NewMap = (decltype(gHookFuncs.R_NewMap))
				g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, R_NEWMAP_SIG, Sig_Length(R_NEWMAP_SIG));
			Sig_FuncNotFound(R_NewMap);
		}
	}
	auto pfnClientCreateInterface = Sys_GetFactory((HINTERFACEMODULE)g_dwClientBase);
	if (pfnClientCreateInterface && pfnClientCreateInterface("SCClientDLL001", 0))
	{
#define SC_GETCLIENTCOLOR_SIG "\x8B\x4C\x24\x04\x85\xC9\x2A\x2A\x6B\xC1\x58"
		{
			gHookFuncs.GetClientColor = (decltype(gHookFuncs.GetClientColor))
				g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, SC_GETCLIENTCOLOR_SIG, Sig_Length(SC_GETCLIENTCOLOR_SIG));
			Sig_FuncNotFound(GetClientColor);
		}
#define R_VECTORSCALE_SIG "\x8B\x4C\x24\x04\xF3\x0F\x10\x4C\x24\x08\x8B\x44\x24\x0C\xF3\x0F\x10\x01\xF3\x0F\x59\xC1\xF3\x0F\x11\x00\xF3\x0F\x10\x41\x04"
		{
			gHookFuncs.VectorScale = (decltype(gHookFuncs.VectorScale))
				g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, R_VECTORSCALE_SIG, Sig_Length(R_VECTORSCALE_SIG));
			Sig_FuncNotFound(VectorScale);
		}
	}
}
void InstallHook()
{
	Fill_InlineEfxHook(R_Blood);
	Fill_InlineEfxHook(R_BloodSprite);
	Fill_InlineEfxHook(R_Explosion);
	Fill_InlineEfxHook(R_RicochetSprite);

	g_pMetaHookAPI->InlineHook((void*)gHookFuncs.VectorScale, R_VectorScale, (void**)&gHookFuncs.VectorScale);
	g_pMetaHookAPI->InlineHook((void*)gHookFuncs.R_NewMap, R_NewMap, (void**)&gHookFuncs.R_NewMap);
}
void HUD_Init(void)
{
	gScreenInfo.iSize = sizeof(SCREENINFO_s);
	gEngfuncs.pfnGetScreenInfo(&gScreenInfo);

	HINTERFACEMODULE hVGUI2 = (HINTERFACEMODULE)GetModuleHandle("vgui2.dll");
	if (hVGUI2)
	{
		CreateInterfaceFn fnVGUI2CreateInterface = Sys_GetFactory(hVGUI2);
		g_pScheme = (vgui::ISchemeManager*)fnVGUI2CreateInterface(VGUI_SCHEME_INTERFACE_VERSION, NULL);
		pLocalize = (vgui::ILocalize*)fnVGUI2CreateInterface(VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
	}
	pSurface = (vgui::ISurface*)Sys_GetFactory((HINTERFACEMODULE)g_hEngineModule)(VGUI_SURFACE_INTERFACE_VERSION, NULL);

	g_pScheme->LoadSchemeFromFile("abcenchance/ABCEnchance.res", "ABCEnchance");
	pScheme = g_pScheme->GetIScheme(g_pScheme->GetScheme("ABCEnchance"));


	gCVars.pPlayerTitle = gEngfuncs.pfnRegisterVariable("cl_playertitle", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

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

	gCVars.pDynamicHUD = gEngfuncs.pfnRegisterVariable("cl_hud_csgo", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

	gCVars.pBloodSpriteSpeed = gEngfuncs.pfnRegisterVariable("abc_bloodsprite_speed", "128", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pBloodSpriteNumber = gEngfuncs.pfnRegisterVariable("abc_bloodsprite_num", "32", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pExpSmokeNumber = gEngfuncs.pfnRegisterVariable("abc_explosion_smokenumr", "32", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pExpSmokeSpeed = gEngfuncs.pfnRegisterVariable("abc_explosion_smokespeed", "256", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pRicochetNumber = gEngfuncs.pfnRegisterVariable("abc_ricochet_sparknum", "24", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

	gHudDelegate = new CHudDelegate();
	gExportfuncs.HUD_Init();
	gHudDelegate->HUD_Init();
}
int HUD_VidInit(void)
{
	int result = gExportfuncs.HUD_VidInit();
	gHudDelegate->HUD_VidInit();
	return result;
}
int HUD_Redraw(float time, int intermission)
{
	DWORD dwAddress = (DWORD)g_pMetaHookAPI->SearchPattern(g_pMetaSave->pExportFuncs->HUD_VidInit, 0x10, "\xB9", 1);
	if (dwAddress)
	{
		static DWORD pHUD = *(DWORD*)(dwAddress + 0x1);
		HUDLIST* pHudList = (HUDLIST*)(*(DWORD*)(pHUD + 0x0));
		while (pHudList)
		{
			if (dynamic_cast<CHudBattery*>(pHudList->p) != NULL)
				pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			else if (dynamic_cast<CHudHealth*>(pHudList->p) != NULL)
				pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			else if (dynamic_cast<CHudAmmo*>(pHudList->p) != NULL)
			{
				gHookHud.m_Ammo = (dynamic_cast<CHudAmmo*>(pHudList->p));
				pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			else if (dynamic_cast<CHudAmmoSecondary*>(pHudList->p) != NULL)
			{
				//pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			else if (dynamic_cast<CHudMenu*>(pHudList->p) != NULL)
				gHookHud.m_Menu = (dynamic_cast<CHudMenu*>(pHudList->p));
			pHudList = pHudList->pNext;
		}
	}
	DrawPlayerTitle();
	RedrawCorssHair();

	gHudDelegate->HUD_Draw(time);
	return gExportfuncs.HUD_Redraw(time, intermission);
}
void HUD_Reset(void)
{
	gExportfuncs.HUD_Reset();
}
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
	gClientData = client;
	gExportfuncs.HUD_TxferLocalOverrides(state, client);
}
int HUD_UpdateClientData (struct client_data_s* c, float f)
{
	m_hfov = c->fov;
	gHudDelegate->HUD_UpdateClientData(c,f);
	return gExportfuncs.HUD_UpdateClientData(c, f);
}