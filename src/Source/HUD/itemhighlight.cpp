#include <metahook.h>
#include "plugins.h"
#include <map>
#include <vector>
#include "pm_defs.h"
#include "com_model.h"
#include "cl_entity.h"
#include "event_api.h"
#include "triangleapi.h"
#include "mymathlib.h"
#include "CVector.h"
#include "cvardef.h"
#include "extraprecache.h"
#include "exportfuncs.h"

#include "vguilocal.h"
#include "local.h"

#include "itemhighlight.h"

#define ITEM_LIST_PATH "abcenchance/ItemHighLightList.txt"

std::vector<cl_hightlight_s*> aryHighLightList;
CHudItemHighLight g_HudItemHighLight;

int CHudItemHighLight::Init(){
	gCVars.pItemHighLight = CREATE_CVAR("cl_itemhighlight", "1", FCVAR_VALUE, NULL);
	gCVars.pItemHighLightRange = CREATE_CVAR("cl_itemhighlightrange", "344", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = CMathlib::clamp<float>(cvar->value, 0.0f, 344.0f);
	});
	LoadItemList();
	return 0;
}
void CHudItemHighLight::Reset(){
	m_mapHighLightTable.clear();
	m_mapRestoredTent.clear();
	for (size_t i = 0; i < (int)aryHighLightList.size(); i++) {
		aryHighLightList[i]->Index = gEngfuncs.pEventAPI->EV_FindModelIndex(aryHighLightList[i]->Name);
		if (aryHighLightList[i]->Index > 0)
			m_mapHighLightTable[aryHighLightList[i]->Index] = aryHighLightList[i];
	}
	m_iHighLightMdl = PrecacheExtraModel("abcenchance/mdl/item_highlight.mdl");
}
void HighLightTentCallBack(TEMPENTITY* ent, float frametime, float currenttime) {
	cl_entity_t* var = gEngfuncs.GetEntityByIndex(ent->clientIndex);
	auto erase = [&]() {
		g_HudItemHighLight.EraseHighLight(var);
		ent->die = 0;
	};
	if (!var) {
		erase();
		return;
	}
	else if (var->curstate.messagenum != gEngfuncs.GetLocalPlayer()->curstate.messagenum) {
		erase();
		return;
	}
	CMathlib::VectorCopy(var->curstate.origin, ent->entity.origin);
	CMathlib::VectorAdd(ent->entity.origin, ent->tentOffset, ent->entity.origin);
	ent->entity.angles[0] = var->curstate.angles[0];
	ent->entity.angles[2] = var->curstate.angles[2];
}
void CHudItemHighLight::CreateHighLight(cl_entity_t* var) {
	if (m_mapRestoredTent.find(var->index) != m_mapRestoredTent.end())
		return;
	model_t* mdl = gEngfuncs.hudGetModelByIndex(m_iHighLightMdl);
	if (!mdl)
		return;
	TEMPENTITY* ent1 = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(var->curstate.origin, mdl);
	TEMPENTITY* ent2 = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(var->curstate.origin, mdl);
	if ((!ent1) || (!ent2))
		return;
	ent1->flags = ent2->flags = FTENT_PERSIST | FTENT_CLIENTCUSTOM;
	ent1->clientIndex = ent2->clientIndex = var->index;
	ent1->die = ent2->die = gEngfuncs.GetClientTime() + 999.0f;

	CVector vecTemp;
	if (CMathlib::FVectorLength(var->curstate.mins) <= 3.2) {
		vecTemp.x = vecTemp.y = -16;
		vecTemp.z = 0;
	}
	else
		vecTemp = var->curstate.mins;
	CMathlib::VectorCopy(vecTemp, ent1->tentOffset);
	
	if (CMathlib::FVectorLength(var->curstate.maxs) <= 3.2)
		vecTemp[0] = vecTemp[1] = vecTemp[2] = 16;
	else
		CMathlib::VectorCopy(var->curstate.maxs, vecTemp);
	CMathlib::VectorCopy(vecTemp, ent2->tentOffset);

	ent2->entity.angles[1] = 180;
	ent1->entity.curstate.skin = ent2->entity.curstate.skin = m_mapHighLightTable[var->curstate.modelindex]->Type;

	ent1->callback = ent2->callback = HighLightTentCallBack;
	m_mapRestoredTent[var->index] = std::make_pair(ent1, ent2);
}
void CHudItemHighLight::EraseHighLight(cl_entity_t* var) {
	if (m_mapRestoredTent.find(var->index) == m_mapRestoredTent.end())
		return;
	m_mapRestoredTent[var->index].first->die = m_mapRestoredTent[var->index].second->die = 0;
	m_mapRestoredTent.erase(var->index);
}

void CHudItemHighLight::AddEntity(int type, cl_entity_s* ent, const char* modelname){
	if (gCVars.pItemHighLight->value <= 0)
		return;
	//mdlÄ£ÐÍ
	if ((ent) && (ent->model) && (ent->model->type == mod_studio) &&
		m_mapHighLightTable.find(ent->curstate.modelindex) != m_mapHighLightTable.end()) {
		CreateHighLight(ent);
	}
}
void CHudItemHighLight::LoadItemList() {
	char szItemPraseBuf[256];
	const char* pfile = const_cast<const char*>(reinterpret_cast<char*>(gEngfuncs.COM_LoadFile(ITEM_LIST_PATH, 5, nullptr)));
	int i = 0, index = 0;
	if (!pfile){
		gEngfuncs.Con_DPrintf("CHudItemHighLight::LoadItemList: No item list file %s\n", ITEM_LIST_PATH);
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
			aryHighLightList[index]->Type = CMathlib::clamp<int>(std::atoi(szItemPraseBuf), 0, 2);
			index++;
		}
		i++;
		if (i >= 2)
			i = 0;
	}
	if (i != 0)
		SYS_ERROR("Error in parsing file:%s\nLine:%d\nBuf is not end with even.", ITEM_LIST_PATH, index);
	gEngfuncs.COM_FreeFile(const_cast<void*>(reinterpret_cast<const void*>(pfile)));
}