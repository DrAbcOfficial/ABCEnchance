#include <metahook.h>
#include "exportfuncs.h"

#include "cl_entity.h"
#include "mathlib.h"
#include "com_model.h"
#include "palette.h"

#include "math.h"
#include "malloc.h"

#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>

cl_enginefunc_t gEngfuncs;
cl_exportfuncs_t gExportfuncs;
cl_refHookfunc_t gHookFuncs;
cl_cvars_t gCVars;

PVOID g_dwEngineBase;
DWORD g_dwEngineSize;
DWORD g_dwEngineBuildnum;
HINSTANCE g_hEngineModule;
PVOID g_dwClientBase;
DWORD g_dwClientSize;

vgui::ISchemeManager* g_pScheme;
vgui::ISurface* pSurface;
vgui::ILocalize* pLocalize;

vgui::HFont m_hFont;
vgui::IScheme* pScheme;

SCREENINFO_s gScreenInfo;

const clientdata_t* pClientData;

//EFFECT
void R_RicochetSprite(float* pos, struct model_s* pmodel, float duration, float scale)
{
	//stack overflow
	//gHookFuncs.R_SparkEffect(pos, (int)gCVars.pRicochetNumber->value, 4, 32);
	gHookFuncs.R_RicochetSprite(pos, pmodel, duration, scale);
}
void R_Explosion(float* pos, int model, float scale, float framerate, int flags)
{
	int	i;
	vec3_t	offset, dir;
	vec3_t	forward, right, up;
	for (int i = 0; i < gCVars.pExpSmokeNumber->value; i++)
	{
		VectorCopy(pos, offset);
		VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * scale, right, offset);
		VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * scale, up, offset);

		TEMPENTITY* pTemp = gHookFuncs.CL_TempEntAllocHigh(pos, NULL);
		if (!pTemp)
			return;
		pTemp->flags = FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY | FTENT_SMOKETRAIL | FTENT_NOMODEL;
		pTemp->die = gEngfuncs.GetClientTime() + 5;
		pTemp->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 360);
		pTemp->bounceFactor = 0;

		srand((unsigned int)gEngfuncs.GetClientTime() * i - i);
		dir[0] = forward[0] + rand() / double(RAND_MAX) * 2 - 1;
		srand((unsigned int)gEngfuncs.GetClientTime() * i + i);
		dir[1] = forward[1] + rand() / double(RAND_MAX) * 2 - 1;
		dir[2] = forward[2];

		VectorScale(dir, gEngfuncs.pfnRandomFloat(8.0f * scale, 20.0f * scale), pTemp->entity.baseline.origin);
		pTemp->entity.baseline.origin[0] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pExpSmokeSpeed->value) * (scale);
		pTemp->entity.baseline.origin[1] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pExpSmokeSpeed->value) * (scale / 2);
		pTemp->entity.baseline.origin[2] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pExpSmokeSpeed->value) * (scale / 4);
	}
	gHookFuncs.R_Explosion(pos, model, scale, framerate, flags);
}
void R_Blood(float* org, float* dir, int pcolor, int speed)
{
	//Todo::´´½¨Ê±ì­Ñª
	gHookFuncs.R_Blood(org, dir, pcolor, speed);
}
void R_BloodSprite(float* org, int colorindex, int modelIndex, int modelIndex2, float size)
{
	model_t* pModel = gEngfuncs.hudGetModelByIndex(modelIndex);
	if (!pModel)
		return;
	
	TEMPENTITY* pTemp = gHookFuncs.CL_TempEntAllocHigh(org, pModel);
	// large, single blood sprite is a high-priority tent
	if (pTemp)
	{
		int	i;
		vec3_t	offset, dir;
		vec3_t	forward, right, up;

		colorindex = clamp(colorindex, 0, 256);

		pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
		pTemp->flags = FTENT_SPRANIMATE;

		pTemp->entity.curstate.rendercolor.r = base_palette1[colorindex].r;
		pTemp->entity.curstate.rendercolor.g = base_palette1[colorindex].g;
		pTemp->entity.curstate.rendercolor.b = base_palette1[colorindex].b;
		pTemp->entity.curstate.framerate = pModel->numframes * 10; //1s
		// play the whole thing once
		pTemp->die = gEngfuncs.GetClientTime() + (pModel->numframes / pTemp->entity.curstate.framerate);

		pTemp->entity.angles[2] = gEngfuncs.pfnRandomFloat(0, 360);
		pTemp->bounceFactor = 0;

		up[0] = right[0] = forward[0] = 0.0f;
		up[1] = right[1] = forward[1] = 0.0f;
		up[2] = right[2] = forward[2] = 1.0f;

		for (i = 0; i < gCVars.pBloodSpriteNumber->value; i++)
		{
			VectorCopy(org, offset);
			VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * size, right, offset);
			VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * size, up, offset);

			pTemp = gHookFuncs.CL_TempEntAllocHigh(org, gEngfuncs.hudGetModelByIndex(modelIndex2));
			if (!pTemp) 
				return;

			pTemp->flags = FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY;
			pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
			pTemp->entity.curstate.rendercolor.r = base_palette1[colorindex].r;
			pTemp->entity.curstate.rendercolor.g = base_palette1[colorindex].g;
			pTemp->entity.curstate.rendercolor.b = base_palette1[colorindex].b;
			pTemp->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, pModel->numframes - 1);
			pTemp->die = gEngfuncs.GetClientTime() + gEngfuncs.pfnRandomFloat(1.0f, 3.0f);

			pTemp->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 360);
			pTemp->bounceFactor = 0;

			srand((unsigned int)gEngfuncs.GetClientTime() * i - i);
			dir[0] = forward[0] + rand() / double(RAND_MAX) * 2 - 1;
			srand((unsigned int)gEngfuncs.GetClientTime() * i + i);
			dir[1] = forward[1] + rand() / double(RAND_MAX) * 2 - 1;
			dir[2] = forward[2];

			VectorScale(dir, gEngfuncs.pfnRandomFloat(8.0f * size, 20.0f * size), pTemp->entity.baseline.origin);

			pTemp->entity.baseline.origin[0] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pBloodSpriteSpeed->value) * (size);
			pTemp->entity.baseline.origin[1] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pBloodSpriteSpeed->value) * (size / 2);
			pTemp->entity.baseline.origin[2] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pBloodSpriteSpeed->value) * (size / 4);
		}
		vec3_t nOrg;
		nOrg[0] = org[0];
		nOrg[1] = org[1];
		nOrg[2] = org[2];
		gHookFuncs.R_BloodStream(org, dir, colorindex, gEngfuncs.pfnRandomLong(4, gCVars.pBloodSpriteSpeed->value));
	}
}


//PLAYER TITLE
int GetHudFontHeight(void)
{
	if (!m_hFont)
		return 0;

	return pSurface->GetFontTall(m_hFont);
}
void GetStringSize(const wchar_t* string, int* width, int* height)
{
	int i;
	int len;
	int a, b, c;

	if (width)
		*width = 0;

	if (height)
		*height = 0;

	if (!m_hFont)
		return;

	len = wcslen(string);

	if (width)
	{
		for (i = 0; i < len; i++)
		{
			pSurface->GetCharABCwide(m_hFont, string[i], a, b, c);
			*width += a + b + c;
		}
	}

	if (height)
	{
		*height = GetHudFontHeight();
	}
}
int DrawVGUI2String(wchar_t* msg, int x, int y, float r, float g, float b)
{
	int i;
	int iOriginalX;
	int iTotalLines;
	int iCurrentLine;
	int w1, w2, w3;
	bool bHorzCenter;
	int len;
	wchar_t* strTemp;
	int fontheight;

	if (!m_hFont)
		return 0;

	iCurrentLine = 0;
	iOriginalX = x;
	iTotalLines = 1;
	bHorzCenter = false;
	len = wcslen(msg);

	for (strTemp = msg; *strTemp; strTemp++)
	{
		if (*strTemp == '\r')
			iTotalLines++;
	}

	if (x == -1)
	{
		bHorzCenter = true;
	}

	if (y == -1)
	{
		fontheight = pSurface->GetFontTall(m_hFont);
		y = (gScreenInfo.iHeight - fontheight) / 2;
	}

	for (i = 0; i < iTotalLines; i++)
	{
		wchar_t line[1024];
		int iWidth;
		int iHeight;
		int iTempCount;
		int j;
		int shadow_x;

		iTempCount = 0;
		iWidth = 0;
		iHeight = 0;

		for (strTemp = &msg[iCurrentLine]; *strTemp; strTemp++, iCurrentLine++)
		{
			if (*strTemp == '\r')
				break;

			if (*strTemp != '\n')
				line[iTempCount++] = *strTemp;
		}

		line[iTempCount] = 0;

		GetStringSize(line, &iWidth, &iHeight);

		if (bHorzCenter)
			x = (gScreenInfo.iWidth - iWidth) / 2;
		else
			x = iOriginalX;

		gEngfuncs.pfnDrawSetTextColor(0, 0, 0);

		shadow_x = x;

		for (j = 0; j < iTempCount; j++)
		{
			gEngfuncs.pfnVGUI2DrawCharacter(shadow_x, y, line[j], m_hFont);
			pSurface->GetCharABCwide(m_hFont, line[j], w1, w2, w3);

			shadow_x += w1 + w2 + w3;
		}

		gEngfuncs.pfnDrawSetTextColor(r, g, b);

		for (j = 0; j < iTempCount; j++)
		{
			gEngfuncs.pfnVGUI2DrawCharacter(x, y, line[j], m_hFont);
			pSurface->GetCharABCwide(m_hFont, line[j], w1, w2, w3);

			x += w1 + w2 + w3;
		}

		y += iHeight;
		iCurrentLine++;
	}

	return x;
}

void DrawPlayerTitle()
{
	if (gCVars.pPlayerTitle->value > 0)
	{
		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		float* localColor = gHookFuncs.GetClientColor(local->index);

		float dx = 0;
		float dy = 0;
		float dz = 0;
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
			if (!entity || !entity->player || !entity->model || entity == local)
				continue;

			//¼ÆËãÎÒºÍÄ¿±êµÄÏà¶ÔÆ«ÒÆ
			dx = entity->curstate.origin[0] - local->curstate.origin[0];
			dy = entity->curstate.origin[1] - local->curstate.origin[1];
			dz = entity->curstate.origin[2] - local->curstate.origin[2] - 50;

			float fDistance = sqrt(pow(dx, 2) + pow(dy, 2));
			float* color = gHookFuncs.GetClientColor(i);

			if (fDistance >= 2048 || color != localColor)
				continue;

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
			if (fPaintAngle_xy > -45 && fPaintAngle_xy < 45 && fPaintAngle_z > -45 && fPaintAngle_z < 45)
			{
				DWORD dwLeft_x;
				DWORD dwLeft_y;
				float fPaintPoint = gScreenInfo.iWidth * (tan(fPaintAngle_xy * DEGREE_PER_RADIAN) + 1) / 2;
				dwLeft_x = fPaintPoint - (5000 / (100 + fDistance) + 5);
				//¸©Ñö
				float fPaintSx = gScreenInfo.iHeight * (tan(53.0 * DEGREE_PER_RADIAN) * tan(fPaintAngle_z * DEGREE_PER_RADIAN) * 10 / 11 + 1) / 2;
				dwLeft_y = fPaintSx + (30000 / fDistance) * (fPaintAngle_z / 100);

				hud_player_info_t playerinfo = { 0 };
				gEngfuncs.pfnGetPlayerInfo(i, &playerinfo);
				if (playerinfo.name)
				{
					wchar_t wideName[MAX_PLAYER_NAME_LENGTH];
					pLocalize->ConvertANSIToUnicode(playerinfo.name, wideName, sizeof(wideName));
					DrawVGUI2String(wideName, dwLeft_x, dwLeft_y, color[0], color[1], color[2]);
				}
			}
		}
	}
}

//CROSSHAIR
void RedrawCorssHair()
{
	if (gCVars.pDynamicCrossHair->value > 0)
	{
		int iCenterX = gScreenInfo.iWidth / 2;
		int iCenterY = gScreenInfo.iHeight / 2;
		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		int iOffset = gCVars.pDynamicCrossHairO->value;

		int iDrift = (fabs(pClientData->punchangle[0]) + fabs(pClientData->punchangle[1])) * 5 * gCVars.pDynamicCrossHairM->value;
		int r = 255, g = 255, b = 255, a = 200;
		//×ó
		gEngfuncs.pfnFillRGBA(iCenterX - iDrift - iOffset - gCVars.pDynamicCrossHairL->value, iCenterY, gCVars.pDynamicCrossHairL->value, gCVars.pDynamicCrossHairW->value,r,g,b,a);
		//ÉÏ
		gEngfuncs.pfnFillRGBA(iCenterX, iCenterY - iDrift - iOffset - gCVars.pDynamicCrossHairL->value, gCVars.pDynamicCrossHairW->value, gCVars.pDynamicCrossHairL->value, r, g, b, a);
		//ÓÒ
		gEngfuncs.pfnFillRGBA(iCenterX + iDrift + iOffset, iCenterY, gCVars.pDynamicCrossHairL->value, gCVars.pDynamicCrossHairW->value,r,g,b,a);
		//ÏÂ
		gEngfuncs.pfnFillRGBA(iCenterX, iCenterY + iDrift + iOffset, gCVars.pDynamicCrossHairW->value, gCVars.pDynamicCrossHairL->value, r, g, b, a);
	}
}

//FINAL SHIT
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
	//PlayerTitle
	auto pfnClientCreateInterface = Sys_GetFactory((HINTERFACEMODULE)g_dwClientBase);

	if (pfnClientCreateInterface && pfnClientCreateInterface("SCClientDLL001", 0))
	{
#define SC_GETCLIENTCOLOR_SIG "\x8B\x4C\x24\x04\x85\xC9\x2A\x2A\x6B\xC1\x58"
		{
			gHookFuncs.GetClientColor = (decltype(gHookFuncs.GetClientColor))
				g_pMetaHookAPI->SearchPattern(g_dwClientBase, g_dwClientSize, SC_GETCLIENTCOLOR_SIG, Sig_Length(SC_GETCLIENTCOLOR_SIG));
			Sig_FuncNotFound(GetClientColor);
		}
	}
	//DWORD addr = (DWORD)Search_Pattern("\x6A\x03\x6A\x01\xE8\x2A\x2A\x2A\x2A\x8B\x15");
	//Sig_AddrNotFound("Palatte Addr");
	//gHookFuncs.host_basepal = (word*)*(DWORD*)addr;
}

void InstallHook()
{
	Fill_InlineEfxHook(R_Blood);
	Fill_InlineEfxHook(R_BloodSprite);
	Fill_InlineEfxHook(R_Explosion);
	Fill_InlineEfxHook(R_RicochetSprite);
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

	g_pScheme->LoadSchemeFromFile("gfx/abc/ABCEnchance.res", "ABCEnchance");
	pScheme = g_pScheme->GetIScheme(g_pScheme->GetScheme("ABCEnchance"));
	if (pScheme)
	{
		m_hFont = pScheme->GetFont("MainShitFont", true);
	}
	else
	{
		pScheme = g_pScheme->GetIScheme(g_pScheme->GetDefaultScheme());
		if (pScheme)
		{
			m_hFont = pScheme->GetFont("Legacy_CreditsFont", true);
			if (!m_hFont)
				m_hFont = pScheme->GetFont("CreditsFont", true);
		}
	}

	gCVars.pPlayerTitle = gEngfuncs.pfnRegisterVariable("abc_playertitle", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHair = gEngfuncs.pfnRegisterVariable("abc_crosshair", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairL = gEngfuncs.pfnRegisterVariable("abc_crosshair_length", "32", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairW = gEngfuncs.pfnRegisterVariable("abc_crosshair_width", "4", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairO = gEngfuncs.pfnRegisterVariable("abc_crosshair_offset", "64", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pDynamicCrossHairM = gEngfuncs.pfnRegisterVariable("abc_crosshair_multiple", "4", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pBloodSpriteSpeed = gEngfuncs.pfnRegisterVariable("abc_bloodsprite_speed", "128", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pBloodSpriteNumber = gEngfuncs.pfnRegisterVariable("abc_bloodsprite_num", "32", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pExpSmokeNumber = gEngfuncs.pfnRegisterVariable("abc_explosion_smokenumr", "32", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pExpSmokeSpeed = gEngfuncs.pfnRegisterVariable("abc_explosion_smokespeed", "256", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pRicochetNumber = gEngfuncs.pfnRegisterVariable("abc_ricochet_sparknum", "24", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

	gExportfuncs.HUD_Init();
}

int HUD_Redraw(float time, int intermission)
{
	DrawPlayerTitle();
	RedrawCorssHair();
	return gExportfuncs.HUD_Redraw(time, intermission);
}


void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
	pClientData = client;
	gExportfuncs.HUD_TxferLocalOverrides(state, client);
}