#include <metahook.h>
#include "cl_entity.h"
#include "cvardef.h"
#include "playertitle.h"
#include "exportfuncs.h"
#include "math.h"
#include "malloc.h"
#include "VGUI_local.h"

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