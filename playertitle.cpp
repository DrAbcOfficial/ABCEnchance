#include <metahook.h>
#include <cmath>
#include "mymathlib.h"
#include "CVector.h"

#include "glew.h"
#include "gl_def.h"
#include "triangleapi.h"

#include "cvar_hook.h"
#include "local.h"
#include "vguilocal.h"

#include "gl_draw.h"
#include "hud.h"
#include "weapon.h"
#include "CCustomHud.h"

#include "exportfuncs.h"
#include "vgui_controls/Controls.h"

#include "playertitle.h"

CHudPlayerTitle m_HudPlayerTitle;
int CHudPlayerTitle::Init(void){
	
	gCVars.pPlayerTitle = CREATE_CVAR("cl_playertitle", "1", FCVAR_VALUE, nullptr);
	gCVars.pPlayerTitleLength = CREATE_CVAR("cl_playertitlelength", "196", FCVAR_VALUE, nullptr);
	gCVars.pPlayerTitleHeight = CREATE_CVAR("cl_playertitleheight", "48", FCVAR_VALUE, nullptr);
	gCVars.pPlayerTitleDangerHealth = CREATE_CVAR("cl_playertitledanger", "30", FCVAR_VALUE, nullptr);
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
		GLfloat nowX, nowY;
		CVector vecHUD;
		//夹角
		CVector vecAngle;
		//夹角点积
		float angledotResult = 0;
		//绘制高度
		CVector vecEntityOrigin;
		//绘制相关
		wchar_t wideName[mathlib::MAX_PLAYER_NAME_LENGTH];
		float* color;
		float flArmorRatio, flArmorLength;
		float flHealthRatio, flHealthLength;
		hud_player_info_t playerinfo = { 0 };
		float flTitleLength = gCVars.pPlayerTitleLength->value;
		float flTitleHeight = gCVars.pPlayerTitleHeight->value;
		//视角角度
		CVector vecView;
		gEngfuncs.GetViewAngles(vecView);
		mathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
		for (int i = 1; i <= 32; i++){
			cl_entity_t* entity = gEngfuncs.GetEntityByIndex(i);
			if (!entity || 
				entity->curstate.messagenum != local->curstate.messagenum || 
				!entity->player || !entity->model || entity == local)
				continue;
			//观察者，跳过
			if (gCustomHud.IsSpectator(i))
				continue;
			//计算我和目标的相对偏移
			mathlib::VectorSubtract(entity->curstate.origin, local->curstate.origin, vecAngle);
			color = gHookFuncs.GetClientColor(i);
			if (vecAngle.FLength() >= 1024 || color != localColor)
				continue;
			vecAngle = vecAngle.Normalize();
			angledotResult = DotProduct(vecAngle, vecView);
			//cos 60
			if (angledotResult > 0.5){
				vecEntityOrigin = entity->curstate.origin;
				vecEntityOrigin.z += 45;
				VEC_WorldToScreen(vecEntityOrigin, vecHUD);
				gEngfuncs.pfnGetPlayerInfo(i, &playerinfo);
				if (playerinfo.name){
					nowX = vecHUD.x - flTitleLength / 2;
					nowY = vecHUD.y - flTitleHeight / 2;
					if (gCVars.pPlayerTitle->value < 2){
						hud_playerinfo_t* info = gCustomHud.GetPlayerHUDInfo(i);
						flHealthRatio = mathlib::Q_clamp((info->health / 100.0f), 0.0f, 1.0f);
						flArmorRatio = mathlib::Q_clamp((info->armor / 100.0f), 0.0f, 1.0f);

						flHealthLength = nowX + (flTitleLength * flHealthRatio);
						flArmorLength = nowX + (flTitleLength * flArmorRatio);
						//背景
						glEnable(GL_TEXTURE_2D);
						glEnable(GL_BLEND);
							glColor4ub(255, 255, 255, 255);

							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glBind(iBackGroundTga);
							glBegin(GL_QUADS);
								glTexCoord2f(0, 0);
								glVertex3f(nowX, nowY, 0);
								glTexCoord2f(0, 1);
								glVertex3f(nowX, nowY + flTitleHeight, 0);
								glTexCoord2f(1, 1);
								glVertex3f(nowX + flTitleLength, nowY + flTitleHeight, 0);
								glTexCoord2f(1, 0);
								glVertex3f(nowX + flTitleLength, nowY, 0);
							glEnd();

							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glBind(iHealthBarTga);
							glBegin(GL_QUADS);
								glTexCoord2f(0, 0);
								glVertex3f(nowX, nowY, 0);
								glTexCoord2f(0, 1);
								glVertex3f(nowX, nowY + flTitleHeight, 0);
								glTexCoord2f(flHealthRatio, 1);
								glVertex3f(flHealthLength, nowY + flTitleHeight, 0);
								glTexCoord2f(flHealthRatio, 0);
								glVertex3f(flHealthLength, nowY, 0);
							glEnd();

							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glBind(iArmorBarTga);
							glBegin(GL_QUADS);
								glTexCoord2f(0, 0);
								glVertex3f(nowX, nowY, 0);
								glTexCoord2f(0, 1);
								glVertex3f(nowX, nowY + flTitleHeight, 0);
								glTexCoord2f(flArmorRatio, 1);
								glVertex3f(flArmorLength, nowY + flTitleHeight, 0);
								glTexCoord2f(flArmorRatio, 0);
								glVertex3f(flArmorLength, nowY, 0);
							glEnd();
						glDisable(GL_BLEND);
						
						vgui::surface()->DrawSetTexture(-1);
						if (info->health <= gCVars.pPlayerTitleDangerHealth->value || fabs(entity->curstate.maxs[2] - entity->curstate.mins[2]) < 64){
							vgui::surface()->DrawSetColor(255, 255, 255, 255);
							vgui::surface()->DrawSetTexture(
								info->health <= gCVars.pPlayerTitleDangerHealth->value ? (
									info->health <= 0 ? iDeathIconTga : iMedkitIconTga) : iCrouchIconTga);
							vgui::surface()->DrawTexturedRect(nowX, nowY, nowX + flTitleHeight / 2, nowY + flTitleHeight / 2);
						}
						nowX += flTitleHeight / 2;
					}
					vgui::localize()->ConvertANSIToUnicode(playerinfo.name, wideName, sizeof(wideName));
					vgui::HFont hFont = pSchemeData->GetFont("MainShitFont", true);
					DrawVGUI2String(wideName, nowX, nowY, color[0], color[1], color[2], hFont);
				}
			}
		}
	}
	return 0;
}

