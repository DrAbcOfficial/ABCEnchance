#include <metahook.h>
#include <map>
#include "pm_defs.h"
#include "com_model.h"
#include "cl_entity.h"
#include "event_api.h"
#include "triangleapi.h"
#include "mathlib.h"
#include "cvardef.h"
#include "extraprecache.h"

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
	m_iHighLightMdl = PrecacheExtraModel("abcenchance/mdl/item_highlight.mdl");
}
void HighLightTentCallBack(TEMPENTITY* ent, float frametime, float currenttime) {
	cl_entity_t* var = gEngfuncs.GetEntityByIndex(ent->clientIndex);
	if (!var) {
		ent->die = 0;
		return;
	}
	VectorCopy(var->angles, ent->entity.angles);
	VectorCopy(var->curstate.origin, ent->entity.origin);
	VectorAdd(ent->entity.origin, ent->tentOffset, ent->entity.origin);
	ent->entity.angles[1] += ent->bounceFactor;
}
void CHudItemHighLight::CreateHighLight(cl_entity_t* var) {
	if (m_mapRestoredTent.count(var))
		return;
	model_t* mdl = gEngfuncs.hudGetModelByIndex(m_iHighLightMdl);
	if (!mdl)
		return;
	TEMPENTITY* ent1 = gHookFuncs.CL_TempEntAllocHigh(var->curstate.origin, mdl);
	TEMPENTITY* ent2 = gHookFuncs.CL_TempEntAllocHigh(var->curstate.origin, mdl);
	if ((!ent1) || (!ent2))
		return;
	ent1->flags = ent2->flags = FTENT_FADEOUT | FTENT_CLIENTCUSTOM;
	ent1->clientIndex = ent2->clientIndex = var->index;
	ent1->die = ent2->die = gEngfuncs.GetClientTime() + 999.0f;
	VectorCopy(var->curstate.maxs, ent1->tentOffset);
	VectorCopy(var->curstate.mins, ent2->tentOffset);
	/*角度不对*/
	ent1->bounceFactor = -90;
	ent2->bounceFactor = 90;
	ent1->entity.curstate.skin = ent2->entity.curstate.skin = m_mapHighLightTable[var->curstate.modelindex]->Type;
	ent1->callback = ent2->callback = HighLightTentCallBack;
	m_mapRestoredTent[var].first = ent1;
	m_mapRestoredTent[var].second = ent2;
}
void CHudItemHighLight::EraseHighLight(cl_entity_t* var) {
	if (!m_mapRestoredTent.count(var))
		return;
	m_mapRestoredTent[var].first->die = m_mapRestoredTent[var].second->die = 0;
	m_mapRestoredTent.erase(var);
}
void CHudItemHighLight::Draw(float flTime){
	if (gCVars.pItemHighLight->value <= 0)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	std::map<int, cl_entity_t*>::iterator it;
	for (it = m_mapToBeDraw.begin(); it != m_mapToBeDraw.end(); ){
		cl_entity_t* var = (*it).second;
		if (!var) {
			EraseHighLight(var);
			it = m_mapToBeDraw.erase(it);
			continue;
		}
		vec3_t len;
		VectorSubtract(var->curstate.origin, local->curstate.origin, len);
		if (var->curstate.messagenum != local->curstate.messagenum || 
			VectorLength(len) > gCVars.pItemHighLightRange->value) {
			EraseHighLight(var);
			it = m_mapToBeDraw.erase(it);
		}
		else {
			CreateHighLight(var);
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