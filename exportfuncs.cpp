#include "plugins.h"
//Lib
#include "cmath"
#include "malloc.h"
#include "cl_entity.h"
#include "mathlib.h"
#include "com_model.h"
#include "triangleapi.h"
#include "pm_movevars.h"
#include "glew.h"
//Def
#include "hud.h"
#include "vguilocal.h"
#include "exportfuncs.h"
#include "CColor.h"
#include "weapon.h"
//HUD
#include "ammo.h"
#include "healthhud.h"
#include "basehealth.h"
#include "drawElement.h"
#include "CHudDelegate.h"
#include "local.h"
//efx
#include "efxenchance.h"
#include "viewmodellag.h"

cl_enginefunc_t gEngfuncs;
cl_exportfuncs_t gExportfuncs;
metaplugins_t g_metaplugins;

const clientdata_t* gClientData;
float m_hfov;

overviewInfo_t* gDevOverview;
baseweapon_t* (*g_rgBaseSlots)[10][26] = NULL;
int* g_iVisibleMouse = NULL;
refdef_t* g_refdef = NULL;

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
		//Ë®Æ½¼Ð½Ç
		float levelAngle = 0;
		//¸©Ñö½Ç
		float elevation = 0;
		//½ÇÉ«¼Ð½Ç
		float fPlayerAngle_xy = 0;
		float fPlayerAngle_z = 0;
		//ÆÁÄ»¼Ð½Ç
		float fPaintAngle_xy = 0;
		float fPaintAngle_z = 0;
		//ÊÓ½Ç½Ç¶È
		vec3_t vecView;
		gEngfuncs.GetViewAngles(vecView);

		for (int i = 1; i <= 32; i++)
		{
			cl_entity_t* entity = gEngfuncs.GetEntityByIndex(i);

			if (!entity || entity->curstate.messagenum != local->curstate.messagenum || !entity->player || !entity->model || entity == local)
				continue;
			//¼ÆËãÎÒºÍÄ¿±êµÄÏà¶ÔÆ«ÒÆ
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
			//½Ç¶È²¹³¥  -180 180 =¡·0 360
			if (levelAngle < 0)
				levelAngle = levelAngle + PERIGON_ANGLE;
			elevation = -elevation;
			//¼ÆËã½ÇÉ«¼Ð½Ç
			if (dx > 0)
			{
				if (dy == 0)
					fPlayerAngle_xy = 0.0;
				else if (dy > 0)
					fPlayerAngle_xy = asin(dy / fDistance) * RADIAN_PER_DEGREE;
				else if (dy < 0)
					fPlayerAngle_xy = PERIGON_ANGLE + (asin(dy / fDistance) * RADIAN_PER_DEGREE);
			}
			//ÅÐ¶ÏÌØÊâ½Ç¶È
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
			//ÊÓ½ÇË®Æ½¼Ð½Ç
			fPaintAngle_xy = levelAngle - fPlayerAngle_xy;
			if (fPaintAngle_xy == FLAT_ANGLE || fPaintAngle_xy == -FLAT_ANGLE)
				fPaintAngle_xy = FLAT_ANGLE;
			else if (fPaintAngle_xy > FLAT_ANGLE)
				fPaintAngle_xy = fPaintAngle_xy - PERIGON_ANGLE;
			else if (fPaintAngle_xy < -FLAT_ANGLE)
				fPaintAngle_xy = fPaintAngle_xy + PERIGON_ANGLE;
			//ÊÓ½Ç¸©Ñö¼Ð½Ç
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
//FINAL SHIT
void R_NewMap(void)
{
	gHudDelegate->HUD_Reset();
	gHookFuncs.R_NewMap();
}
int __fastcall R_CrossHair_ReDraw(void* pthis, int dummy, int param_1)
{
	if (gCVars.pDynamicCrossHair->value > 0)
		return 0;
	return gHookFuncs.R_CrossHair_ReDraw(pthis, dummy, param_1);
}
void EVVectorScale(float* punchangle1, float scale, float* punchangle2)
{
	gHookFuncs.EVVectorScale(punchangle1, scale, punchangle2);
	VectorCopy(punchangle1, gHudDelegate->m_vecClientEVPunch);
}
int CL_IsDevOverview(void)
{
	return gHudDelegate->m_iIsOverView ? 1 : gHookFuncs.CL_IsDevOverview();
}
void CL_SetDevOverView(int param1)
{
	gHookFuncs.CL_SetDevOverView(param1);
	if (gHudDelegate->m_iIsOverView)
	{
		(*(vec3_t*)(param1 + 0x1C))[YAW] = gHudDelegate->m_flOverViewYaw;
		*(float *)(param1 + 0x10) = gHudDelegate->m_vecOverViewOrg[0];
		*(float *)(param1 + 0x14) = gHudDelegate->m_vecOverViewOrg[1];
		gDevOverview->z_max = gHudDelegate->m_flOverViewZmax;
		gDevOverview->z_min = gHudDelegate->m_flOverViewZmin;
		gDevOverview->zoom = gHudDelegate->m_flOverViewScale;
	}	
}
void R_RenderView(int a1)
{
	gHudDelegate->HUD_PreRenderView(a1);
	gHookFuncs.R_RenderView(a1);
}
void R_ForceCVars(qboolean mp)
{
	if (CL_IsDevOverview())
		return;
	gHookFuncs.R_ForceCVars(mp);
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
void CheckOtherPlugin()
{
	g_metaplugins.renderer = (HINTERFACEMODULE)GetModuleHandle("Renderer.dll");
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
		DWORD addr;
#define R_NEWMAP_SIG "\x55\x8B\xEC\x51\xC7\x45\xFC\x00\x00\x00\x00\xEB\x2A\x8B\x45\xFC\x83\xC0\x01\x89\x45\xFC\x81\x7D\xFC\x00\x01\x00\x00"
		{
			gHookFuncs.R_NewMap = (decltype(gHookFuncs.R_NewMap))
				g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, R_NEWMAP_SIG, Sig_Length(R_NEWMAP_SIG));
			Sig_FuncNotFound(R_NewMap);
		}
#define R_ISCLOVERVIEW_SIG "\xD9\x05\x2A\x2A\x2A\x2A\xD9\xEE\xDA\xE9\xDF\xE0\xF6\xC4\x44\x2A\x2A\x83\x3D\x2A\x2A\x2A\x2A\x00\x2A\x2A\xB8\x01\x00\x00\x00\xC3\x2A\x2A"
		{
			gHookFuncs.CL_IsDevOverview = (decltype(gHookFuncs.CL_IsDevOverview))
				g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, R_ISCLOVERVIEW_SIG, Sig_Length(R_ISCLOVERVIEW_SIG));
			Sig_FuncNotFound(CL_IsDevOverview);
		}
#define R_VIEWREFDEF_SIG "\x68\x2A\x2A\x2A\x2A\xD9\x1D\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68"
		{
			addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, R_VIEWREFDEF_SIG, Sig_Length(R_VIEWREFDEF_SIG));
			Sig_AddrNotFound(g_refdef);
			auto r_refdef_viewangles = (vec_t*)(*(DWORD*)(addr + 28));
			g_refdef = (refdef_t*)((char*)r_refdef_viewangles - offsetof(refdef_t, viewangles));
		}
#define R_RENDERSCENE_SIG_SVENGINE "\xDD\xD8\xDD\xD8\xE8"
#define R_RENDERVIEW_SIG_SVENGINE "\x55\x8B\xEC\x83\xE4\xC0\x83\xEC\x34\x53\x56\x57\x8B\x7D\x08\x85\xFF"
		{
			gHookFuncs.R_RenderView = (void(*)(int))Search_Pattern(R_RENDERVIEW_SIG_SVENGINE);
			Sig_FuncNotFound(R_RenderView);

			addr = (DWORD)Search_Pattern_From(gHookFuncs.R_RenderView, R_RENDERSCENE_SIG_SVENGINE);
			Sig_AddrNotFound(R_RenderScene);
			gHookFuncs.R_RenderScene = (void(*)(void))(addr + 5 + 4 + *(int*)(addr + 5));
		}
#define GL_BIND_SIG "\x8B\x44\x24\x04\x39\x05\x2A\x2A\x2A\x2A\x2A\x2A\x50\x68\xE1\x0D\x00\x00\xA3\x2A\x2A\x2A\x2A\xFF\x15\x2A\x2A\x2A\x2A\xC3"
		{
			gHookFuncs.GL_Bind = (decltype(gHookFuncs.GL_Bind))
				g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, GL_BIND_SIG, Sig_Length(GL_BIND_SIG));
			Sig_FuncNotFound(GL_Bind);
		}
#define CL_SETDEVOVERVIEW "\xD9\x05\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\xDC\xC9\xD9\x05\x2A\x2A\x2A\x2A\xDE\xE2\xD9\xC9\xD9\x1D\x2A\x2A\x2A\x2A\xD8\x0D\x2A\x2A\x2A\x2A\xD8\x2D\x2A\x2A\x2A\x2A\xD9\x1D\x2A\x2A\x2A\x2A\xD9\xEE\xD9\x05\x2A\x2A\x2A\x2A\xD8\xD1\xDF\xE0\xD9\xE8\xD9\x05\x2A\x2A\x2A\x2A\xF6\xC4\x41\x2A\x2A\xD8\xC1\xD9\x15\x2A\x2A\x2A\x2A"
		{
			gHookFuncs.CL_SetDevOverView = (decltype(gHookFuncs.CL_SetDevOverView))
				g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, CL_SETDEVOVERVIEW, Sig_Length(CL_SETDEVOVERVIEW));
			Sig_FuncNotFound(CL_SetDevOverView);
		}
#define DEVOVERVIEW_SIG "\x83\xEC\x30\xDD\x5C\x24\x2A\xD9\x05"
		{
			addr = (DWORD)Search_Pattern(DEVOVERVIEW_SIG);
			Sig_AddrNotFound(gDevOverview);
			gDevOverview = (decltype(gDevOverview))(*(DWORD*)(addr + 9) - 0xC);
		}
#define R_FORCECVAR_SIG "\x83\x7C\x24\x2A\x00\x2A\x2A\x2A\x2A\x00\x00\x81\x3D\x2A\x2A\x2A\x2A\xFF\x00\x00\x00"
		{
			gHookFuncs.R_ForceCVars = (decltype(gHookFuncs.R_ForceCVars))
				g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, R_FORCECVAR_SIG, Sig_Length(R_FORCECVAR_SIG));
			Sig_FuncNotFound(R_ForceCVars);
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
#define R_EVVECTORSCALE_SIG "\x8B\x4C\x24\x04\xF3\x0F\x10\x4C\x24\x08\x8B\x44\x24\x0C\xF3\x0F\x10\x01\xF3\x0F\x59\xC1\xF3\x0F\x11\x00\xF3\x0F\x10\x41\x04"
		{
			gHookFuncs.EVVectorScale = (decltype(gHookFuncs.EVVectorScale))
				g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, R_EVVECTORSCALE_SIG, Sig_Length(R_EVVECTORSCALE_SIG));
			Sig_FuncNotFound(EVVectorScale);
		}
#define R_CROSSHAIR_REDRAW_SIG "\x8B\x51\x14\x85\xD2\x0F\x84\x8B\x00\x00\x00\xA1\x2A\x2A\x2A\x2A\xF3\x0F\x2C\x40\x0C\x85\xC0\x7E\x2A\x83\x3D\x2A\x2A\x2A\x2A\x00\x53\x56\x57\x74\x2A\x80\x3D\x2A\x2A\x2A\x2A\x00\x75\x2A\xF3\x0F\x2C\x79\x34\xF3\x0F\x2C\x59\x38\xEB\x2A"
		{
			gHookFuncs.R_CrossHair_ReDraw = (decltype(gHookFuncs.R_CrossHair_ReDraw))
				g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, R_CROSSHAIR_REDRAW_SIG, Sig_Length(R_CROSSHAIR_REDRAW_SIG));
			Sig_FuncNotFound(R_CrossHair_ReDraw);
		}
#define SC_UPDATECURSORSTATE_SIG "\x8B\x40\x28\xFF\xD0\x84\xC0\x2A\x2A\xC7\x05\x2A\x2A\x2A\x2A\x01\x00\x00\x00"
		{
			DWORD addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, SC_UPDATECURSORSTATE_SIG, Sig_Length(SC_UPDATECURSORSTATE_SIG));
			Sig_AddrNotFound(g_iVisibleMouse);
			g_iVisibleMouse = *(decltype(g_iVisibleMouse)*)(addr + 11);
		}
#define SC_HUDAMMO_RESET_SIG "\x83\x49\x10\x01\x68\x78\x05\x00\x00\xC7\x41\x14\x00\x00\x00\x00\xC7\x41\x2C\x00\x00\x00\x00\xC7\x41\x18\x00\x00\x00\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\xC7\x05\x2A\x2A\x2A\x2A\x00\x00\x00\x00\xC7\x05\x2A\x2A\x2A\x2A\x00\x00\x00\x00\xE8\x2A\x2A\x2A\x2A\x68\xC0\x00\x00\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x18\xC7\x05\x2A\x2A\x2A\x2A\x00\x00\x00\x00\xC3"
		{
			DWORD addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, SC_HUDAMMO_RESET_SIG, Sig_Length(SC_HUDAMMO_RESET_SIG));
			Sig_AddrNotFound(g_rgBaseSlots);
			g_rgBaseSlots = *(decltype(g_rgBaseSlots)*)(addr + 33);
		}
	}
}
void InstallHook()
{
	Fill_InlineEfxHook(R_Blood);
	Fill_InlineEfxHook(R_BloodSprite);
	Fill_InlineEfxHook(R_Explosion);
	Fill_InlineEfxHook(R_RicochetSprite);

	g_pMetaHookAPI->InlineHook((void*)gHookFuncs.R_NewMap, R_NewMap, (void**)&gHookFuncs.R_NewMap);
	g_pMetaHookAPI->InlineHook((void*)gHookFuncs.R_RenderView, R_RenderView, (void**)&gHookFuncs.R_RenderView);
	g_pMetaHookAPI->InlineHook((void*)gHookFuncs.CL_IsDevOverview, CL_IsDevOverview, (void**)&gHookFuncs.CL_IsDevOverview);
	g_pMetaHookAPI->InlineHook((void*)gHookFuncs.CL_SetDevOverView, CL_SetDevOverView, (void**)&gHookFuncs.CL_SetDevOverView);
	g_pMetaHookAPI->InlineHook((void*)gHookFuncs.EVVectorScale, EVVectorScale, (void**)&gHookFuncs.EVVectorScale);
	g_pMetaHookAPI->InlineHook((void*)gHookFuncs.R_CrossHair_ReDraw, R_CrossHair_ReDraw, (void**)&gHookFuncs.R_CrossHair_ReDraw);
}
void GL_Init(void)
{
	g_pMetaHookAPI->GetVideoMode(&gScreenInfo.iWidth, &gScreenInfo.iHeight, NULL, NULL);
	auto err = glewInit();
	if (GLEW_OK != err)
	{
		Sys_ErrorEx("glewInit failed, %s", glewGetErrorString(err));
		return;
	}
	gHudDelegate->GL_Init();
}
void HUD_Init(void)
{
	HINTERFACEMODULE hVGUI2 = (HINTERFACEMODULE)GetModuleHandle("vgui2.dll");
	if (hVGUI2)
	{
		CreateInterfaceFn fnVGUI2CreateInterface = Sys_GetFactory(hVGUI2);
		g_pScheme = (vgui::ISchemeManager*)fnVGUI2CreateInterface(VGUI_SCHEME_INTERFACE_VERSION, NULL);
		pLocalize = (vgui::ILocalize*)fnVGUI2CreateInterface(VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
	}
	g_pSurface = (vgui::ISurface*)Sys_GetFactory((HINTERFACEMODULE)g_hEngineModule)(VGUI_SURFACE_INTERFACE_VERSION, NULL);
	g_pScheme->LoadSchemeFromFile("abcenchance/ABCEnchance.res", "ABCEnchance");
	pScheme = g_pScheme->GetIScheme(g_pScheme->GetScheme("ABCEnchance"));
	

	gCVars.pPlayerTitle = gEngfuncs.pfnRegisterVariable("cl_playertitle", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

	gCVars.pDynamicHUD = gEngfuncs.pfnRegisterVariable("cl_hud_csgo", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

	gCVars.pBloodSpriteSpeed = gEngfuncs.pfnRegisterVariable("abc_bloodsprite_speed", "128", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pBloodSpriteNumber = gEngfuncs.pfnRegisterVariable("abc_bloodsprite_num", "32", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pExpSmokeNumber = gEngfuncs.pfnRegisterVariable("abc_explosion_smokenumr", "32", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pExpSmokeSpeed = gEngfuncs.pfnRegisterVariable("abc_explosion_smokespeed", "256", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pRicochetNumber = gEngfuncs.pfnRegisterVariable("abc_ricochet_sparknum", "24", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	
	gCVars.pModelLag = gEngfuncs.pfnRegisterVariable("cl_modellag", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pModelLagValue = gEngfuncs.pfnRegisterVariable("cl_modellagvalue", "1.0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

	gCVars.pCurDebug = gEngfuncs.pfnRegisterVariable("cl_curdebug", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

	gCVars.pCamIdealHeight = gEngfuncs.pfnRegisterVariable("cam_idealheight", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

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
			{
				if (gCVars.pDynamicHUD->value <= 0)
					pHudList->p->m_iFlags &= HUD_ACTIVE;
				else
					pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			else if (dynamic_cast<CHudHealth*>(pHudList->p) != NULL)
			{
				if (gCVars.pDynamicHUD->value <= 0)
					pHudList->p->m_iFlags &= HUD_ACTIVE;
				else
					pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			else if (dynamic_cast<CHudAmmo*>(pHudList->p) != NULL)
			{
				gHookHud.m_Ammo = (dynamic_cast<CHudAmmo*>(pHudList->p));
				if (gCVars.pDynamicHUD->value <= 0)
					pHudList->p->m_iFlags &= HUD_ACTIVE;
				else
					pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			else if (dynamic_cast<CHudMenu*>(pHudList->p) != NULL)
				gHookHud.m_Menu = (dynamic_cast<CHudMenu*>(pHudList->p));
			pHudList = pHudList->pNext;
		}
	}
	DrawPlayerTitle();
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
void HUD_ClientMove(struct playermove_s* ppmove, qboolean server)
{
	gHudDelegate->HUD_ClientMove(ppmove, server);
	return gExportfuncs.HUD_PlayerMove(ppmove, server);
}
void V_CalcRefdef(struct ref_params_s* pparams)
{
	gExportfuncs.V_CalcRefdef(pparams);
	if(!gExportfuncs.CL_IsThirdPerson())
		V_CalcViewModelLag(pparams);
	else
		pparams->vieworg[2] -= gCVars.pCamIdealHeight->value;
}
void IN_MouseEvent(int mstate)
{
	if (g_iVisibleMouse && gHudDelegate->m_iVisibleMouse)
	{
		int iVisibleMouse = *g_iVisibleMouse;
		*g_iVisibleMouse = 1;
		gExportfuncs.IN_MouseEvent(mstate);
		gHudDelegate->IN_MouseEvent(mstate);
		*g_iVisibleMouse = iVisibleMouse;
	}
	else
		gExportfuncs.IN_MouseEvent(mstate);
}
void IN_Accumulate(void)
{
	if (g_iVisibleMouse && gHudDelegate->m_iVisibleMouse)
	{
		int iVisibleMouse = *g_iVisibleMouse;
		*g_iVisibleMouse = 1;
		gExportfuncs.IN_Accumulate();
		gHudDelegate->IN_Accumulate();
		*g_iVisibleMouse = iVisibleMouse;
	}
	else
		gExportfuncs.IN_Accumulate();
}
void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active)
{
	if (g_iVisibleMouse && gHudDelegate->m_iVisibleMouse)
	{
		int iVisibleMouse = *g_iVisibleMouse;
		*g_iVisibleMouse = 1;
		gExportfuncs.CL_CreateMove(frametime, cmd, active);
		gHudDelegate->CL_CreateMove(frametime, cmd, active);
		*g_iVisibleMouse = iVisibleMouse;
	}
	else
		gExportfuncs.CL_CreateMove(frametime, cmd, active);
}
void HUD_Clear(void)
{
	gHudDelegate->HUD_Clear();
}