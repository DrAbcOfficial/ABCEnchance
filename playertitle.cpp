#include <metahook.h>
#include <cmath>
#include "mathlib.h"

#include "glew.h"
#include "triangleapi.h"

#include "cvar_hook.h"
#include "local.h"
#include "vguilocal.h"
#include "myconst.h"

#include "hud.h"

#include "weapon.h"
#include "CHudDelegate.h"
#include "drawElement.h"

#include "exportfuncs.h"

#include "playertitle.h"

CHudPlayerTitle m_HudPlayerTitle;
int CHudPlayerTitle::Init(void){
	
	gCVars.pPlayerTitle = CREATE_CVAR("cl_playertitle", "1", FCVAR_VALUE, NULL);
	gCVars.pPlayerTitleLength = CREATE_CVAR("cl_playertitlelength", "196", FCVAR_VALUE, NULL);
	gCVars.pPlayerTitleHeight = CREATE_CVAR("cl_playertitleheight", "48", FCVAR_VALUE, NULL);
	Reset();
	return 0;
}
void CHudPlayerTitle::Reset(){
	VGUI_CREATE_NEWTGA_TEXTURE(iDeathIconTga, "abcenchance/tga/playertitle_deadicon");
	VGUI_CREATE_NEWTGA_TEXTURE(iCrouchIconTga, "abcenchance/tga/playertitle_crouchicon");
	VGUI_CREATE_NEWTGA_TEXTURE(iMedkitIconTga, "abcenchance/tga/playertitle_medkiticon");
	VGUI_CREATE_NEWTGA_TEXTURE(iBackGroundTga, "abcenchance/tga/playertitle_background");
	VGUI_CREATE_NEWTGA_TEXTURE(iHealthBarTga, "abcenchance/tga/playertitle_healthbar");
	VGUI_CREATE_NEWTGA_TEXTURE(iArmorBarTga, "abcenchance/tga/playertitle_armorbar");
}
int CHudPlayerTitle::Draw(float flTime){
	if (gCVars.pPlayerTitle->value > 0){
		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		float* localColor = gHookFuncs.GetClientColor(local->index);
		float dx, dy, dz;
		int nowX, nowY;
		vec3_t vecHUD;
		vec3_t vecOrg;
		//ÀÆ∆Ωº–Ω«
		float levelAngle = 0;
		//∏©—ˆΩ«
		float elevation = 0;
		//Ω«…´º–Ω«
		float fPlayerAngle_xy = 0;
		float fPlayerAngle_z = 0;
		//∆¡ƒªº–Ω«
		float fPaintAngle_xy = 0;
		float fPaintAngle_z = 0;
		//ªÊ÷∆œ‡πÿ
		wchar_t wideName[MAX_PLAYER_NAME_LENGTH];
		float* color;
		float fDistance;
		float flArmorRatio;
		float flHealthRatio;
		hud_player_info_t playerinfo = { 0 };
		int iTitleLength = gCVars.pPlayerTitleLength->value;
		int iTitleHeight = gCVars.pPlayerTitleHeight->value;
		// ”Ω«Ω«∂»
		vec3_t vecView;
		gEngfuncs.GetViewAngles(vecView);
		for (int i = 1; i <= 32; i++){
			cl_entity_t* entity = gEngfuncs.GetEntityByIndex(i);

			if (!entity || 
				entity->curstate.messagenum != local->curstate.messagenum || 
				!entity->player || !entity->model || entity == local)
				continue;
			//º∆À„Œ“∫Õƒø±Íµƒœ‡∂‘∆´“∆
			dx = entity->curstate.origin[0] - local->curstate.origin[0];
			dy = entity->curstate.origin[1] - local->curstate.origin[1];
			dz = entity->curstate.origin[2] - local->curstate.origin[2];
			fDistance = fsqrt(pow(dx, 2) + pow(dy, 2));
			color = gHookFuncs.GetClientColor(i);
			if (fDistance >= 1024 || color != localColor)
				continue;

			vecOrg[0] = entity->curstate.origin[0];
			vecOrg[1] = entity->curstate.origin[1];
			vecOrg[2] = entity->curstate.origin[2] + 32;

			levelAngle = vecView[1];
			elevation = vecView[0];
			//Ω«∂»≤π≥•  -180 180 => 0 360
			if (levelAngle < 0)
				levelAngle = levelAngle + PERIGON_ANGLE;
			elevation = -elevation;
			//º∆À„Ω«…´º–Ω«
			if (dx > 0){
				if (dy == 0)
					fPlayerAngle_xy = 0.0;
				else if (dy > 0)
					fPlayerAngle_xy = asin(dy / fDistance) * RADIAN_PER_DEGREE;
				else if (dy < 0)
					fPlayerAngle_xy = PERIGON_ANGLE + (asin(dy / fDistance) * RADIAN_PER_DEGREE);
			}
			//≈–∂œÃÿ ‚Ω«∂»
			else if (dx == 0){
				if (dy > 0)
					fPlayerAngle_xy = 90.0;
				else if (dy < 0)
					fPlayerAngle_xy = 270.0;
			}
			else if (dx < 0){
				if (dy == 0)
					fPlayerAngle_xy = FLAT_ANGLE;
				else if (dy > 0)
					fPlayerAngle_xy = FLAT_ANGLE - asin(dy / (fDistance)) * RADIAN_PER_DEGREE;
				else if (dy < 0)
					fPlayerAngle_xy = FLAT_ANGLE - asin(dy / (fDistance)) * RADIAN_PER_DEGREE;
			}
			// ”Ω«ÀÆ∆Ωº–Ω«
			fPaintAngle_xy = levelAngle - fPlayerAngle_xy;
			if (fPaintAngle_xy == FLAT_ANGLE || fPaintAngle_xy == -FLAT_ANGLE)
				fPaintAngle_xy = FLAT_ANGLE;
			else if (fPaintAngle_xy > FLAT_ANGLE)
				fPaintAngle_xy = fPaintAngle_xy - PERIGON_ANGLE;
			else if (fPaintAngle_xy < -FLAT_ANGLE)
				fPaintAngle_xy = fPaintAngle_xy + PERIGON_ANGLE;
			// ”Ω«∏©—ˆº–Ω«
			fPlayerAngle_z = asin(dz / (fsqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2)))) * RADIAN_PER_DEGREE;
			fPaintAngle_z = elevation - fPlayerAngle_z;
			if (fPaintAngle_xy > -45 && fPaintAngle_xy < 45 && fPaintAngle_z > -45 && fPaintAngle_z < 45){
				gEngfuncs.pTriAPI->WorldToScreen(vecOrg, vecHUD);
				vecHUD[0] = (1.0f + vecHUD[0]) * gScreenInfo.iWidth / 2;
				vecHUD[1] = (1.0f - vecHUD[1]) * gScreenInfo.iHeight / 2;

				gEngfuncs.pfnGetPlayerInfo(i, &playerinfo);
				if (playerinfo.name){
					flHealthRatio = clamp((gHudDelegate->m_Playerinfo[i].health / 100.0f), 0.0f, 1.0f);
					flArmorRatio = clamp((gHudDelegate->m_Playerinfo[i].armor / 100.0f), 0.0f, 1.0f);
					
					nowX = vecHUD[0] - iTitleLength / 3;
					nowY = vecHUD[1] - iTitleHeight / 2;
					if (gCVars.pPlayerTitle->value < 2){
						//±≥æ∞
						glEnable(GL_TEXTURE_2D);
						glEnable(GL_BLEND);
							glColor4ub(255, 255, 255, 255);

							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glBindTexture(GL_TEXTURE_2D, iBackGroundTga);
							glBegin(GL_QUADS);
								glTexCoord2f(0, 0);
								glVertex3f(nowX, nowY, 0);
								glTexCoord2f(0, 1);
								glVertex3f(nowX, nowY + iTitleHeight, 0);
								glTexCoord2f(1, 1);
								glVertex3f(nowX + iTitleLength, nowY + iTitleHeight, 0);
								glTexCoord2f(1, 0);
								glVertex3f(nowX + iTitleLength, nowY, 0);
							glEnd();

							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glBindTexture(GL_TEXTURE_2D, iHealthBarTga);
							glBegin(GL_QUADS);
								glTexCoord2f(0, 0);
								glVertex3f(nowX, nowY, 0);
								glTexCoord2f(0, 1);
								glVertex3f(nowX, nowY + iTitleHeight, 0);
								glTexCoord2f(flHealthRatio, 1);
								glVertex3f(nowX + iTitleLength * flHealthRatio, nowY + iTitleHeight, 0);
								glTexCoord2f(flHealthRatio, 0);
								glVertex3f(nowX + iTitleLength * flHealthRatio, nowY, 0);
							glEnd();

							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glBindTexture(GL_TEXTURE_2D, iArmorBarTga);
							glBegin(GL_QUADS);
								glTexCoord2f(0, 0);
								glVertex3f(nowX, nowY, 0);
								glTexCoord2f(0, 1);
								glVertex3f(nowX, nowY + iTitleHeight, 0);
								glTexCoord2f(flArmorRatio, 1);
								glVertex3f(nowX + iTitleLength * flArmorRatio, nowY + iTitleHeight, 0);
								glTexCoord2f(flArmorRatio, 0);
								glVertex3f(nowX + iTitleLength * flArmorRatio, nowY, 0);
							glEnd();
						
						gHudDelegate->surface()->DrawSetTexture(-1);
						if (flHealthRatio <= 0.45f || fabs(entity->curstate.maxs[2] - entity->curstate.mins[2]) < 64){
							gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255);
							gHudDelegate->surface()->DrawSetTexture(
								flHealthRatio < 0.4 ? (
									flHealthRatio <= 0 ? iDeathIconTga : iMedkitIconTga) : iCrouchIconTga);
							gHudDelegate->surface()->DrawTexturedRect(nowX, nowY, nowX + iTitleHeight / 2, nowY + iTitleHeight / 2);
						}
						nowX += iTitleHeight / 2;
					}
					pLocalize->ConvertANSIToUnicode(playerinfo.name, wideName, sizeof(wideName));
					vgui::HFont hFont = pScheme->GetFont("MainShitFont", true);
					DrawVGUI2String(wideName, nowX, nowY, color[0], color[1], color[2], hFont);
				}
			}
		}
	}
	return 0;
}

