#include <metahook.h>
#include "hud.h"
#include "cvardef.h"
#include "local.h"
#include "vguilocal.h"
#include <cmath>
#include "mathlib.h"
#include "parsemsg.h"
#include "msghook.h"
#include "weapon.h"
#include "CHudDelegate.h"
#include "drawElement.h"
#include "triangleapi.h"
#include "exportfuncs.h"

#include "playertitle.h"

CHudPlayerTitle m_HudPlayerTitle;
pfnUserMsgHook m_pfnScoreInfo;

int __MsgFunc_ScoreInfo(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	//wtf is not this shit
	if (clientIndex >= 1 && clientIndex <= 32) {
		m_HudPlayerTitle.playerinfo[clientIndex].index = clientIndex;
		m_HudPlayerTitle.playerinfo[clientIndex].frags = READ_FLOAT();
		m_HudPlayerTitle.playerinfo[clientIndex].death = READ_LONG();
		m_HudPlayerTitle.playerinfo[clientIndex].health = READ_FLOAT();
		m_HudPlayerTitle.playerinfo[clientIndex].armor = READ_FLOAT();
		m_HudPlayerTitle.playerinfo[clientIndex].team = READ_BYTE();
		m_HudPlayerTitle.playerinfo[clientIndex].donors = READ_CHAR();
		m_HudPlayerTitle.playerinfo[clientIndex].admin = READ_CHAR();
	}
	return m_pfnScoreInfo(pszName, iSize, pbuf);
}
int CHudPlayerTitle::Init(void)
{
	m_pfnScoreInfo = HOOK_MESSAGE(ScoreInfo);
	gCVars.pPlayerTitle = gEngfuncs.pfnRegisterVariable("cl_playertitle", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	Reset();
	return 0;
}
void CHudPlayerTitle::Reset()
{
	iDeathIconTga = 0;
	VGUI_CREATE_NEWTGA_TEXTURE(iBackGroundTga, "abcenchance/tga/playertitle_background");
	iHealthBarTga = 0;
	iArmorBarTga = 0;
	iIconBackGroundTga = 0;
	memset(playerinfo, 0, sizeof(playerinfo));
}
int CHudPlayerTitle::Draw(float flTime)
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
			float fDistance = fsqrt(pow(dx, 2) + pow(dy, 2));
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
			fPlayerAngle_z = asin(dz / (fsqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2)))) * RADIAN_PER_DEGREE;
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
	return 0;
}

