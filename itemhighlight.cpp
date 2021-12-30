#include <metahook.h>
#include <map>
#include <vector>
#include "pm_defs.h"
#include "com_model.h"
#include "cl_entity.h"
#include "event_api.h"
#include "triangleapi.h"
#include "mathlib.h"
#include "cvardef.h"
#include "extraprecache.h"
#include "exportfuncs.h"

#include "vguilocal.h"
#include "local.h"
#include "myconst.h"

#include "itemhighlight.h"

#define ITEM_LIST_PATH "abcenchance/ItemHighLightList.txt"
using namespace mathlib;

std::vector<cl_hightlight_s*> aryHighLightList;

CHudItemHighLight m_HudItemHighLight;
char szItemPraseBuf[256];
void RangeSizeCallBack(cvar_t* cvar) {
	cvar->value = clamp(cvar->value, 0.0, 344.0);
}
int CHudItemHighLight::Init(){
	gCVars.pItemHighLight = CREATE_CVAR("cl_itemhighlight", "1", FCVAR_VALUE, NULL);
	gCVars.pItemHighLightRange = CREATE_CVAR("cl_itemhighlightrange", "344", FCVAR_VALUE, RangeSizeCallBack);
	LoadItemList();
	return 0;
}
void CHudItemHighLight::Reset(){
	m_mapHighLightTable.clear();
	m_mapToBeDraw.clear();
	m_mapRestoredTent.clear();
	for (int i = 0; i < (int)aryHighLightList.size(); i++) {
		aryHighLightList[i]->Index = gEngfuncs.pEventAPI->EV_FindModelIndex(aryHighLightList[i]->Name);
		if (aryHighLightList[i]->Index > 0)
			m_mapHighLightTable[aryHighLightList[i]->Index] = aryHighLightList[i];
	}
	m_iHighLightMdl = PrecacheExtraModel((char*)"abcenchance/mdl/item_highlight.mdl");
}
void HighLightTentCallBack(TEMPENTITY* ent, float frametime, float currenttime) {
	cl_entity_t* var = gEngfuncs.GetEntityByIndex(ent->clientIndex);
	if (!var) {
		ent->die = 0;
		return;
	}
	VectorCopy(var->curstate.origin, ent->entity.origin);
	VectorAdd(ent->entity.origin, ent->tentOffset, ent->entity.origin);
	ent->entity.angles[0] = var->curstate.angles[0];
	ent->entity.angles[2] = var->curstate.angles[2];
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
	vec3_t vecTemp;
	if (FVectorLength(var->curstate.mins) <= 3.2) {
		vecTemp[0] = vecTemp[1] = -16;
		vecTemp[2] = 0;
	}
	else
		VectorCopy(var->curstate.mins, vecTemp);
	VectorCopy(vecTemp, ent1->tentOffset);
	
	if (FVectorLength(var->curstate.maxs) <= 3.2)
		vecTemp[0] = vecTemp[1] = vecTemp[2] = 16;
	else
		VectorCopy(var->curstate.maxs, vecTemp);
	VectorCopy(vecTemp, ent2->tentOffset);
	ent2->entity.angles[1] = 180;
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
	//mdlÄ£ÐÍ
	if ((ent) && (ent->model) && (ent->model->type == mod_studio) &&
		m_mapHighLightTable.count(ent->curstate.modelindex) && 
		!m_mapToBeDraw.count(ent->index)) {
		m_mapToBeDraw[ent->index] = ent;
	}
}
void CHudItemHighLight::LoadItemList() {
	char* pfile = (char*)gEngfuncs.COM_LoadFile((char*)ITEM_LIST_PATH, 5, NULL);
	int i = 0, index = 0;
	if (!pfile){
		gEngfuncs.Con_DPrintf((char*)"CHudItemHighLight::LoadItemList: No item list file %s\n", ITEM_LIST_PATH);
		return;
	}
	while (true){
		pfile = gEngfuncs.COM_ParseFile(pfile, szItemPraseBuf);
		if (!pfile)
			break;
		if (i == 0) {
			cl_hightlight_s* item = new cl_hightlight_s();
			strcpy_s(item->Name, szItemPraseBuf);
			aryHighLightList.push_back(item);
		}
		else {
			aryHighLightList[index]->Type = clamp(atoi(szItemPraseBuf), 0, 2);
			index++;
		}
		i++;
		if (i >= 2)
			i = 0;
	}
	if (i != 0)
		Sys_ErrorEx("Error in parsing file:%s\nLine:%d\nBuf is not end with even.", ITEM_LIST_PATH, index);
	gEngfuncs.COM_FreeFile(pfile);
}