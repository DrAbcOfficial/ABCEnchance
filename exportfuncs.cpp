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

#include <triangleapi.h>

#include <Color.h>
#include "hud.h"

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

vgui::IScheme* pScheme;

SCREENINFO_s gScreenInfo;

const clientdata_t* pClientData;
float* pClientEVPunchAngles;

cl_hookedHUD pHookedHUD;
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
	//Todo::创建时飙血
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
		int nColor = colorindex;

		nColor = clamp(nColor, 0, 256);

		pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
		pTemp->flags = FTENT_SPRANIMATE;

		pTemp->entity.curstate.rendercolor.r = base_palette1[nColor].r;
		pTemp->entity.curstate.rendercolor.g = base_palette1[nColor].g;
		pTemp->entity.curstate.rendercolor.b = base_palette1[nColor].b;
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
			pTemp->entity.curstate.rendercolor.r = base_palette1[nColor].r;
			pTemp->entity.curstate.rendercolor.g = base_palette1[nColor].g;
			pTemp->entity.curstate.rendercolor.b = base_palette1[nColor].b;
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
		gHookFuncs.R_BloodStream(nOrg, dir, nColor == 247 ? 70 : nColor, gEngfuncs.pfnRandomLong(4, gCVars.pBloodSpriteSpeed->value));
	}
}


//PLAYER TITLE
int GetHudFontHeight(vgui::HFont m_hFont)
{
	if (!m_hFont)
		return 0;

	return pSurface->GetFontTall(m_hFont);
}
void GetStringSize(const wchar_t* string, int* width, int* height, vgui::HFont m_hFont)
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
		*height = GetHudFontHeight(m_hFont);
	}
}
int DrawVGUI2String(wchar_t* msg, int x, int y, float r, float g, float b, vgui::HFont m_hFont)
{
	if (r > 1.0)
		r /= 255;
	if (g > 1.0)
		g /= 255;
	if (b > 1.0)
		b /= 255;
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

		GetStringSize(line, &iWidth, &iHeight, m_hFont);

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
		if (pClientData->health <= 0)
			return;
		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		int iCenterX = gScreenInfo.iWidth / 2;
		int iCenterY = gScreenInfo.iHeight / 2;
		int iOffset = gCVars.pDynamicCrossHairO->value;
		int iDrift = fabs(pClientEVPunchAngles[0]) + fabs(pClientEVPunchAngles[1]);
		if (iDrift <= 0)
			iDrift = fabs(pClientData->punchangle[0]) + fabs(pClientData->punchangle[1]);
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
			gEngfuncs.pfnFillRGBABlend(iCenterX - iFinalOffset - iLength - iOutLineWidth, iCenterY - iWidthOffset, iOutLineWidth, iWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX - iFinalOffset, iCenterY - iWidthOffset, iOutLineWidth, iWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX - iFinalOffset - iLength - iOutLineWidth, iCenterY - iWidthOffset - iOutLineWidth, iLength + 2 * iOutLineWidth, iOutLineWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX - iFinalOffset - iLength - iOutLineWidth, iCenterY + iWidthOffset, iLength + 2 * iOutLineWidth, iOutLineWidth, 0, 0, 0, a);
			//上
			if (!gCVars.pDynamicCrossHairT->value) {
				gEngfuncs.pfnFillRGBABlend(iCenterX - iWidthOffset, iCenterY - iFinalOffset - iLength - iOutLineWidth, iWidth, iOutLineWidth, 0, 0, 0, a);
				gEngfuncs.pfnFillRGBABlend(iCenterX - iWidthOffset, iCenterY - iFinalOffset, iWidth, iOutLineWidth, 0, 0, 0, a);
				gEngfuncs.pfnFillRGBABlend(iCenterX - iWidthOffset - iOutLineWidth, iCenterY - iFinalOffset - iLength - iOutLineWidth, iOutLineWidth, iLength + 2 * iOutLineWidth, 0, 0, 0, a);
				gEngfuncs.pfnFillRGBABlend(iCenterX + iWidthOffset, iCenterY - iFinalOffset - iLength - iOutLineWidth, iOutLineWidth, iLength + 2 * iOutLineWidth, 0, 0, 0, a);
			}
			//右
			gEngfuncs.pfnFillRGBABlend(iCenterX + iFinalOffset + iLength, iCenterY - iWidthOffset, iOutLineWidth, iWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX + iFinalOffset - iOutLineWidth, iCenterY - iWidthOffset, iOutLineWidth, iWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX + iFinalOffset - iOutLineWidth, iCenterY - iWidthOffset - iOutLineWidth, iLength + 2 * iOutLineWidth, iOutLineWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX + iFinalOffset - iOutLineWidth, iCenterY + iWidthOffset, iLength + 2 * iOutLineWidth, iOutLineWidth, 0, 0, 0, a);
			//下
			gEngfuncs.pfnFillRGBABlend(iCenterX - iWidthOffset, iCenterY + iFinalOffset + iLength, iWidth, iOutLineWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX - iWidthOffset, iCenterY + iFinalOffset - iOutLineWidth, iWidth, iOutLineWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX - iWidthOffset - iOutLineWidth, iCenterY + iFinalOffset - iOutLineWidth, iOutLineWidth, iLength + 2 * iOutLineWidth, 0, 0, 0, a);
			gEngfuncs.pfnFillRGBABlend(iCenterX + iWidthOffset, iCenterY + iFinalOffset - iOutLineWidth, iOutLineWidth, iLength + 2 * iOutLineWidth, 0, 0, 0, a);
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

//HUD
cl_CustomVars pHudSetting;
void InitHealthArmorHUDSprIcon()
{
	pHudSetting.iHealthIcon = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-cross1.spr");
	pHudSetting.iArmorIconNull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-shield.spr");
	pHudSetting.iArmorIconFull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-armor-helmet.spr");

	pHudSetting.StartX = atof(pScheme->GetResourceString("HealthArmor.StartX"));
	pHudSetting.IconSize = atof(pScheme->GetResourceString("HealthArmor.IconSize"));
	pHudSetting.TextWidth = atof(pScheme->GetResourceString("HealthArmor.TextWidth"));
	pHudSetting.IconSize = atof(pScheme->GetResourceString("HealthArmor.IconSize"));
	pHudSetting.BarLength = atof(pScheme->GetResourceString("HealthArmor.BarLength"));
	pHudSetting.BarWidth = atof(pScheme->GetResourceString("HealthArmor.BarWidth"));
	pHudSetting.ElementGap = atof(pScheme->GetResourceString("HealthArmor.ElementGap"));
	pHudSetting.BackGroundY = atof(pScheme->GetResourceString("HealthArmor.BackGroundY"));
	pHudSetting.BackGroundLength = atof(pScheme->GetResourceString("HealthArmor.BackGroundLength"));
	pHudSetting.BackGroundAlpha = atof(pScheme->GetResourceString("HealthArmor.BackGroundAlpha"));

	Color defaultColor = Color(245,230,195,255);
	pHudSetting.HealthIconColor = pScheme->GetColor("HealthArmor.HealthIconColor", defaultColor);
	pHudSetting.HealthBarColor = pScheme->GetColor("HealthArmor.HealthBarColor", defaultColor);
	pHudSetting.HealthTextColor = pScheme->GetColor("HealthArmor.HealthTextColor", defaultColor);
	pHudSetting.ArmorIconColor = pScheme->GetColor("HealthArmor.ArmorIconColor", defaultColor);
	pHudSetting.ArmorBarColor = pScheme->GetColor("HealthArmor.ArmorBarColor", defaultColor);
	pHudSetting.ArmorTextColor = pScheme->GetColor("HealthArmor.ArmorTextColor", defaultColor);
	pHudSetting.BackGroundColor = pScheme->GetColor("HealthArmor.BackGroundColor", defaultColor);

	pHudSetting.HUDFont = pScheme->GetFont("HUDShitFont", true);
	pHudSetting.HUDBigFont = pScheme->GetFont("HUDShitFont", true);
}
void DrawSPRIcon(int SprHandle, float x, float y, float w, float h, int r, int g, int b, int a)
{
	gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)gEngfuncs.GetSpritePointer(SprHandle, SprHandle), 0);
	gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(x, y, 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(x, y + h, 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(x + w, y + h, 0);
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(x + w, y,0);
	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}
void DrawHealthArmorHUD()
{
	if (gCVars.pDynamicHUD <= 0)
		return;

	int r, g, b, a;
	float iSizeStep = (float)gScreenInfo.iWidth / 3 / pHudSetting.BackGroundAlpha;
	float i = 0, nowX = 0;
	float flBackGroundY = gScreenInfo.iHeight * pHudSetting.BackGroundY;
	float flBackGroundHeight = gScreenInfo.iHeight - flBackGroundY;
	pHudSetting.BackGroundColor.GetColor(r, g, b, a);
	for (i = pHudSetting.BackGroundAlpha; i > 0.0f; i--)
	{
		gEngfuncs.pfnFillRGBABlend(nowX, flBackGroundY, iSizeStep, flBackGroundHeight, r, g, b, i);
		nowX += iSizeStep;
	}
	int iStartX = gScreenInfo.iWidth / pHudSetting.StartX;
	int iIconSize = flBackGroundHeight * pHudSetting.IconSize;
	int iTextWidth = flBackGroundHeight * pHudSetting.TextWidth;
	int iTextHeight = flBackGroundHeight * pHudSetting.TextHeight;
	int iBarLength = flBackGroundHeight * pHudSetting.BarLength;
	int iBarWidth = flBackGroundHeight * pHudSetting.BarWidth;
	int iElementGap = flBackGroundHeight * pHudSetting.ElementGap;
	int iHealth = pHookedHUD.pHUDHealth->m_iHealth;
	int iBattery = pHookedHUD.pHUDBattery->m_iBat;
	//HP ICON
	if(!pHudSetting.iHealthIcon)
		pHudSetting.iHealthIcon = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-cross1.spr");
	pHudSetting.HealthIconColor.GetColor(r, g, b, a);
	DrawSPRIcon(pHudSetting.iHealthIcon, iStartX, flBackGroundY + (flBackGroundHeight - iIconSize) / 2, iIconSize, iIconSize,r, g, b, a);

	char numberString[16];
	itoa(iHealth, numberString, 10);
	wchar_t wideName[8];
	pLocalize->ConvertANSIToUnicode(numberString, wideName, sizeof(wideName));
	int iSzWidth = 0;
	GetStringSize(wideName, &iSzWidth, NULL, pHudSetting.HUDFont);
	iStartX += iIconSize + iElementGap + iTextWidth - iSzWidth;
	pHudSetting.HealthTextColor.GetColor(r, g, b, a);
	DrawVGUI2String(wideName, iStartX, flBackGroundY + (flBackGroundHeight - iTextHeight) / 2, r, g, b, pHudSetting.HUDFont);
	iStartX += iSzWidth + iElementGap;

	pHudSetting.HealthBarColor.GetColor(r, g, b, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flBackGroundY + (flBackGroundHeight - iBarWidth) / 2, iBarLength, iBarWidth, r/2, g/2, b/2, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flBackGroundY + (flBackGroundHeight - iBarWidth) / 2, iBarLength * max(0, min(1, (float)iHealth / 100)), iBarWidth, r, g, b, a);
	iStartX += iBarLength + iElementGap * 4;

	//AP
	if (!pHudSetting.iArmorIconNull || !pHudSetting.iArmorIconFull)
		pHudSetting.iArmorIconNull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-shield.spr");
	if (!pHudSetting.iArmorIconFull)
		pHudSetting.iArmorIconFull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-armor-helmet.spr");
	pHudSetting.ArmorIconColor.GetColor(r, g, b, a);
	DrawSPRIcon(iBattery > 0 ? pHudSetting.iArmorIconFull : pHudSetting.iArmorIconNull, iStartX, flBackGroundY + (flBackGroundHeight - iIconSize) / 2, iIconSize, iIconSize, r, g, b, a);

	itoa(iBattery, numberString, 10);
	pLocalize->ConvertANSIToUnicode(numberString, wideName, sizeof(wideName));
	GetStringSize(wideName, &iSzWidth, NULL, pHudSetting.HUDFont);
	iStartX += iIconSize + iElementGap + iTextWidth - iSzWidth;
	pHudSetting.ArmorTextColor.GetColor(r, g, b, a);
	DrawVGUI2String(wideName, iStartX, flBackGroundY + (flBackGroundHeight - iTextHeight) / 2, r, g, b, pHudSetting.HUDFont);
	iStartX += iSzWidth + iElementGap;

	pHudSetting.ArmorBarColor.GetColor(r, g, b, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flBackGroundY + (flBackGroundHeight - iBarWidth) / 2, iBarLength, iBarWidth, r/2, g/2, b/2, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flBackGroundY + (flBackGroundHeight - iBarWidth) / 2, iBarLength * max(0, min(1, (float)iBattery / 100)), iBarWidth, r, g, b, a);
	iStartX += iBarLength + iElementGap * 2;
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

	gCVars.pDynamicCrossHair = gEngfuncs.pfnRegisterVariable("cl_dynamiccrosshair", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
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

	InitHealthArmorHUDSprIcon();

	gExportfuncs.HUD_Init();
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
				pHookedHUD.pHUDBattery = dynamic_cast<CHudBattery*>(pHudList->p);
				pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			else if (dynamic_cast<CHudHealth*>(pHudList->p) != NULL)
			{
				pHookedHUD.pHUDHealth = dynamic_cast<CHudHealth*>(pHudList->p);
				pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			else if (dynamic_cast<CHudAmmo*>(pHudList->p) != NULL)
			{
				pHookedHUD.pHUDAmmo = dynamic_cast<CHudAmmo*>(pHudList->p);
				//pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			else if (dynamic_cast<CHudAmmoSecondary*>(pHudList->p) != NULL)
			{
				pHookedHUD.pHudAmmoSecondary = dynamic_cast<CHudAmmoSecondary*>(pHudList->p);
				//pHudList->p->m_iFlags &= ~HUD_ACTIVE;
			}
			pHudList = pHudList->pNext;
		}
	}
	DrawPlayerTitle();
	RedrawCorssHair();
	DrawHealthArmorHUD();
	return gExportfuncs.HUD_Redraw(time, intermission);
}


void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
	pClientData = client;
	gExportfuncs.HUD_TxferLocalOverrides(state, client);
}