#include <metahook.h>
#include <vector>
#include <algorithm>
#include "glew.h"
#include "hud.h"
#include "weapon.h"
#include "mymathlib.h"
#include "CVector.h"
#include "local.h"
#include "triangleapi.h"
#include "vguilocal.h"
#include "event_api.h"
#include "CCustomHud.h"
#include "grenadeindicator.h"

#include "vgui_controls/Controls.h"

#define GRENADE_MODEL_PATH "models/w_grenade.mdl"
CHudGrenadeIndicator m_HudGrenadeIndicator;

int CHudGrenadeIndicator::Init(){
	gCVars.pGrenadeIndicator = CREATE_CVAR("cl_grenadeindicator", "1", FCVAR_VALUE, nullptr);
	gCVars.pGrenadeIndicatorRange = CREATE_CVAR("cl_grenadeindicator_range", "256", FCVAR_VALUE, nullptr);
	gCVars.pGrenadeInicatorTime = CREATE_CVAR("cl_grenadeindicator_time", "4", FCVAR_VALUE, nullptr);
	return 0;
}
void CHudGrenadeIndicator::VidInit(){
	IconHalfSize = GET_SCREEN_PIXEL(false, "GrenadeIndicator.Size") / 2;
}
int CHudGrenadeIndicator::Draw(float flTime){
	if (gCVars.pGrenadeIndicator->value <= 0)
		return 1;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	for (auto it = aryGrenades.begin(); it != aryGrenades.end(); ) {
		cl_entity_t* ent = (*it);
		if (!ent || flTime >= ent->curstate.msg_time + gCVars.pGrenadeInicatorTime->value) {
			it = aryGrenades.erase(it);
			continue;
		}
		CVector len;
		mathlib::VectorSubtract(local->curstate.origin, ent->curstate.origin, len);
		CVector vecView;
		gEngfuncs.GetViewAngles(vecView);
		gEngfuncs.pfnAngleVectors(vecView, vecView, nullptr, nullptr);
		vecView = vecView.Normalize();
		CVector vecAngle = len.Normalize();
		if(DotProduct(vecAngle, vecView) < 0.5 && len.FLength() < gCVars.pGrenadeIndicatorRange->value) {
			//Draw icon
			CVector vecHUD;
			VEC_WorldToScreen(ent->curstate.origin, vecHUD);
			vecHUD.x = mathlib::clamp<int>(vecHUD.x, IconHalfSize, ScreenWidth - IconHalfSize);
			vecHUD.y = mathlib::clamp<int>(vecHUD.y, IconHalfSize, ScreenHeight - IconHalfSize);
			vgui::surface()->DrawSetTexture(-1);
			vgui::surface()->DrawSetColor(255, 255, 255, 255);
			vgui::surface()->DrawSetTexture(GrenadeIconTga);
			vgui::surface()->DrawTexturedRect(vecHUD.x - IconHalfSize, vecHUD.y - IconHalfSize, vecHUD.x + IconHalfSize, vecHUD.y + IconHalfSize);
			it++;
		}
		else
			it = aryGrenades.erase(it);
	}
	return 0;
}
void CHudGrenadeIndicator::Reset(){
	aryGrenades.clear();
	VGUI_CREATE_NEWTGA_TEXTURE(GrenadeIconTga, "abcenchance/tga/grenadeicon");
	GrenadeIndex = gEngfuncs.pEventAPI->EV_FindModelIndex(GRENADE_MODEL_PATH);
}
void CHudGrenadeIndicator::Clear() {
	aryGrenades.clear();
}
int CHudGrenadeIndicator::HUD_AddEntity(int type, cl_entity_s* ent, const char* modelname) {
	if (gCVars.pGrenadeIndicator->value <= 0)
		return true;
	if (ent->curstate.modelindex != GrenadeIndex || ent->curstate.sequence == 0 || ent->model->type != mod_studio)
		return true;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if (ent->curstate.messagenum != local->curstate.messagenum)
		return true;
	CVector len;
	mathlib::VectorSubtract(local->curstate.origin, ent->curstate.origin, len);
	if (len.FLength() > gCVars.pGrenadeIndicatorRange->value)
		return true;
	if(std::none_of(aryGrenades.begin(), aryGrenades.end(), [ent](cl_entity_t* v) {
		if (v == ent)
			return true;
		return  false;
	}))
		aryGrenades.push_back(ent);
	return true;
}