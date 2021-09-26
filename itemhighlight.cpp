#include <metahook.h>
#include <map>
#include "cl_entity.h"
#include "event_api.h"
#include "com_model.h"
#include "triangleapi.h"
#include "mathlib.h"
#include "cvardef.h"

#include "vguilocal.h"
#include "local.h"
#include "myconst.h"

#include "itemhighlight.h"

//TODO: 用txt列表取代该固定玩意儿
#define HIGH_LIGHT_LIST_SIZE 55
cl_hightlight_t aryHighLightList[HIGH_LIGHT_LIST_SIZE] = {
	{"models/w_2uzis.mdl", 2},
	{"models/w_2uzis_gold.mdl", 2},
	{"models/w_9mmar.mdl", 2},
	{"models/w_9mmhandgun.mdl", 2},
	{"models/w_357.mdl", 2},
	{"models/w_bgrap.mdl", 2},
	{"models/w_crossbow.mdl", 2},
	{"models/w_crowbar.mdl", 2},
	{"models/w_desert_eagle.mdl", 2},
	{"models/w_displacer.mdl", 2},
	{"models/w_egon.mdl", 2},
	{"models/w_desert_eagle.mdl", 2},
	{"models/w_gauss.mdl", 2},
	{"models/w_hgun.mdl", 2},
	{"models/w_m16.mdl", 2},
	{"models/w_m40a1.mdl", 2},
	{"models/w_minigun.mdl", 2},
	{"models/w_mp5.mdl", 2},
	{"models/w_minigun.mdl", 2},
	{"models/w_pipe_wrench.mdl", 2},
	{"models/w_pmedkit.mdl", 2},
	{"models/w_pipe_wrench.mdl", 2},
	{"models/w_rpg.mdl", 2},
	{"models/w_saw.mdl", 2},
	{"models/w_shock.mdl", 2},
	{"models/w_shock_rifle.mdl", 2},
	{"models/w_shotgun.mdl", 2},
	{"models/w_spore_launcher.mdl", 2},
	{"models/w_squeak.mdl", 2},
	{"models/w_uzi.mdl", 2},
	{"models/w_uzi_gold.mdl", 2},

	{"models/w_9mmarclip.mdl", 1},
	{"models/w_9mmclip.mdl", 1},
	{"models/w_357ammo.mdl", 1},
	{"models/w_357ammobox.mdl", 1},
	{"models/w_argrenade.mdl", 1},
	{"models/w_chainammo.mdl", 1},
	{"models/w_crossbow_clip.mdl", 1},
	{"models/w_gaussammo.mdl", 1},
	{"models/w_grenade.mdl", 1},
	{"models/w_m40a1clip.mdl", 1},
	{"models/w_mp5_clip.mdl", 1},
	{"models/w_satchel.mdl", 1},
	{"models/w_rpgammo.mdl", 1},
	{"models/w_saw_clip.mdl", 1},
	{"models/w_shotbox.mdl", 1},
	{"models/w_tripmine.mdl", 1},
	{"models/w_uzi_clip.mdl", 1},

	{"models/w_adrenaline.mdl", 0},
	{"models/w_antidote.mdl", 0},
	{"models/w_battery.mdl", 0},
	{"models/w_longjump.mdl", 0},
	{"models/w_medkit.mdl", 0},
	{"models/w_oxygen.mdl", 0},
	{"models/w_rad.mdl", 0},
};
CHudItemHighLight m_HudItemHighLight;

void RangeSizeCallBack(cvar_t* cvar) {
	cvar->value = clamp(cvar->value, 0, 344);
}
int CHudItemHighLight::Init(){
	gCVars.pItemHighLight = CREATE_CVAR("cl_itemhighlight", "0", FCVAR_VALUE, NULL);
	gCVars.pItemHighLightRange = CREATE_CVAR("cl_itemhighlightrange", "344", FCVAR_VALUE, RangeSizeCallBack);
	return 0;
}

void CHudItemHighLight::Reset(){
	m_mapHighLightTable.clear();
	m_mapToBeDraw.clear();
	for (int i = 0; i < HIGH_LIGHT_LIST_SIZE; i++) {
		aryHighLightList[i].Index = gEngfuncs.pEventAPI->EV_FindModelIndex(aryHighLightList[i].Name);
		if (aryHighLightList[i].Index > 0)
			m_mapHighLightTable[aryHighLightList[i].Index] = &aryHighLightList[i];
	}
}

void CHudItemHighLight::Draw(float flTime){
	if (gCVars.pItemHighLight->value <= 0)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	std::map<int, cl_entity_t*>::iterator it;
	for (it = m_mapToBeDraw.begin(); it != m_mapToBeDraw.end(); ){
		cl_entity_t* var = (*it).second;
		if (!var) {
			it = m_mapToBeDraw.erase(it);
			continue;
		}
		vec3_t len;
		VectorSubtract(var->curstate.origin, local->curstate.origin, len);
		if (var->curstate.messagenum != local->curstate.messagenum || 
			VectorLength(len) > gCVars.pItemHighLightRange->value) {
			it = m_mapToBeDraw.erase(it);
		}
		else {
			//TODO: 换成高亮描边
			vec3_t vecHUD;
			gEngfuncs.pTriAPI->WorldToScreen(var->curstate.origin, vecHUD);
			vecHUD[0] = (1.0f + vecHUD[0]) * gScreenInfo.iWidth / 2;
			vecHUD[1] = (1.0f - vecHUD[1]) * gScreenInfo.iHeight / 2;
			int r = 0, g = 0, b = 0;
			switch (m_mapHighLightTable[var->curstate.modelindex]->Type)
			{
			case 0:r = 255; break;
			case 1:g = 255; break;
			case 2:b = 255; break;
			}
			gEngfuncs.pfnFillRGBABlend(vecHUD[0], vecHUD[1], 8, 8,  r, g, b, 80);
			it++;
		}
	}

}
void CHudItemHighLight::AddEntity(int type, cl_entity_s* ent, const char* modelname){
	if (gCVars.pItemHighLight->value <= 0)
		return;
	//mdl模型
	if (ent && ent->model->type == mod_studio && 
		m_mapHighLightTable.count(ent->curstate.modelindex) && 
		!m_mapToBeDraw.count(ent->index)) {
		m_mapToBeDraw[ent->index] = ent;
	}
}
