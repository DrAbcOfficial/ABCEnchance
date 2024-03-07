#include <metahook.h>
#include "plugins.h"
#include <map>
#include <vector>
#include <string>
#include <regex>

#include "mathlib/vector.h"

#include "com_model.h"
#include "cl_entity.h"
#include "event_api.h"

#include "mymathlib.h"
#include "cvardef.h"
#include "extraprecache.h"
#include "exportfuncs.h"

#include "vguilocal.h"
#include "local.h"

#include "Viewport.h"
#include "itemhighlight.h"

#define ITEM_LIST_PATH "abcenchance/ItemHighLightList.txt"
#define VIEWPORT_ITEMHIGHLIGHT_NAME "ItemHighLightPanel"
#define VIEWPORT_ITEMNAME_NAME "ItemNamePanel"

extern vgui::HScheme GetViewPortBaseScheme();

CItemHighLightPanel::CItemHighLightPanel() : BaseClass(nullptr, VIEWPORT_ITEMHIGHLIGHT_NAME)
{
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	SetProportional(true);
	SetScheme(GetViewPortBaseScheme());

	gCVars.pItemHighLight = CREATE_CVAR("cl_itemhighlight", "1", FCVAR_VALUE, NULL);
	gCVars.pItemHighLightRange = CREATE_CVAR("cl_itemhighlightrange", "344", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = CMathlib::clamp<float>(cvar->value, 0.0f, 344.0f);
	});
	gCVars.pItemHighLightName = CREATE_CVAR("cl_itemhighlightname", "1", FCVAR_VALUE, NULL);

	LoadControlSettings(VGUI2_ROOT_DIR "ItemHighLightPanel.res");
	LoadItemList();

}
void CItemHighLightPanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(Color(0, 0, 0, 0));
}

void CItemHighLightPanel::Think()
{
	if (!gCVars.pItemHighLightName->value)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if (!local)
		return;
	//视角角度
	vec3_t vecView;
	Vector vecLength;
	float angledotResult;
	Vector vecEntityOrigin;
	Vector vecHUD;
	gEngfuncs.GetViewAngles(vecView);
	CMathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
	//计算我和目标的相对偏移
	for (auto &iter : m_aryHighLightPanel) {
		vecLength = *(iter.second->origin);
		vecLength -= local->curstate.origin;
		if (vecLength.Length() >= gCVars.pItemHighLightRange->value) {
			iter.second->ShowPanel(false);
			continue;
		}
		vecLength = vecLength.Normalize();
		angledotResult = vecLength.Dot(vecView);;
		//cos 60
		if (angledotResult > 0.5) {
			vecEntityOrigin = *(iter.second->origin);
			vecEntityOrigin.z += 2;
			VEC_WorldToScreen(vecEntityOrigin, vecHUD);
			iter.second->SetPos(vecHUD.x - iter.second->GetWide() / 2, vecHUD.y);
			iter.second->ShowPanel(true);
		}
		else {
			iter.second->ShowPanel(false);
			continue;
		}
	}
}

void CItemHighLightPanel::Reset() {

}

void HighLightTentCallBack(TEMPENTITY* ent, float frametime, float currenttime) {
	int modelindex = ent->entity.curstate.iuser1;
	cl_entity_t* var = gEngfuncs.GetEntityByIndex(ent->clientIndex);
	auto erase = [&]() {
		g_pViewPort->EraseHighLight(var, modelindex);
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
void CItemHighLightPanel::CreateHighLight(cl_entity_t* var) {
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

	Vector vecTemp;
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
	//把索引传进回调
	ent1->entity.curstate.iuser1 = ent2->entity.curstate.iuser1 = var->curstate.modelindex;
	ent1->callback = ent2->callback = HighLightTentCallBack;
	m_mapRestoredTent[var->index] = std::make_pair(ent1, ent2);

	if (!gCVars.pItemHighLightName->value) {
		m_aryHighLightPanel[var->curstate.modelindex] = new CItemNamePanel(GetVPanel());
		//坐标数据的地址
		m_aryHighLightPanel[var->curstate.modelindex]->origin = &var->curstate.origin;
		m_aryHighLightPanel[var->curstate.modelindex]->item = m_mapHighLightTable[var->curstate.modelindex];
		m_aryHighLightPanel[var->curstate.modelindex]->SetText();
	}
}
void CItemHighLightPanel::ItemHighLightReset()
{
	m_mapHighLightTable.clear();
	m_mapRestoredTent.clear();
	for (size_t i = 0; i < (int)aryHighLightList.size(); i++) {
		auto list = aryHighLightList[i];
		list->Index = gEngfuncs.pEventAPI->EV_FindModelIndex(list->Name);
		if (list->Index > -1) {
			m_mapHighLightTable[list->Index] = list;
		}
	}
	m_iHighLightMdl = PrecacheExtraModel("abcenchance/mdl/item_highlight.mdl");
}
void CItemHighLightPanel::EraseHighLight(cl_entity_t* var, int modelindex) {
	if (m_mapRestoredTent.find(var->index) == m_mapRestoredTent.end())
		return;
	if (m_aryHighLightPanel[modelindex]) {
		m_aryHighLightPanel[modelindex]->DeletePanel();
		m_aryHighLightPanel.erase(modelindex);
	}
	m_mapRestoredTent[var->index].first->die = m_mapRestoredTent[var->index].second->die = 0;
	m_mapRestoredTent.erase(var->index);
}
void CItemHighLightPanel::AddEntity(int type, cl_entity_s* ent, const char* modelname){
	if (gCVars.pItemHighLight->value <= 0)
		return;
	//mdl模型
	if ((ent) && (ent->model) && (ent->model->type == mod_studio) &&
		m_mapHighLightTable.find(ent->curstate.modelindex) != m_mapHighLightTable.end()) {
		CreateHighLight(ent);
	}
}
void CItemHighLightPanel::LoadItemList() {
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
			cl_highlight_s* item = new cl_highlight_s();
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
const char* CItemHighLightPanel::GetName() {
	return VIEWPORT_ITEMHIGHLIGHT_NAME;
}
void CItemHighLightPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
vgui::VPANEL CItemHighLightPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
bool CItemHighLightPanel::IsVisible() {
	return BaseClass::IsVisible();
}
void CItemHighLightPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

CItemNamePanel::CItemNamePanel(vgui::VPANEL parent) : BaseClass(nullptr, VIEWPORT_ITEMNAME_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());
	SetParent(parent);

	m_pNameLabel = new vgui::Label(this, "ItemName", "Unknown");
	m_pNameLabel->SetSize(GetWide(), GetTall());
	LoadControlSettings(VGUI2_ROOT_DIR "ItemHighLightPanel.res");
	ShowPanel(false);
}
void CItemNamePanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(Color(0, 0, 0, 0));
}
void CItemNamePanel::SetSize(int w, int t) {
	BaseClass::SetSize(w, t);
	m_pNameLabel->SetSize(w, t);
}
void CItemNamePanel::Reset() {
	DeletePanel();
}
void CItemNamePanel::Think()
{

}
void CItemNamePanel::SetText() {
	if (!m_szNameString.empty() || !item->Name)
		return;
	m_szNameString += item->Name;
	std::regex r("\\.mdl"); //匹配所有的“.mdl”
	std::regex r1(".*/w_"); //匹配“/w_”及其前面的所有字符
	std::regex r2(".*/wp_"); //匹配“/wp_”及其前面的所有字符
	m_szNameString = std::regex_replace(m_szNameString, r, ""); //用空字符串替换掉匹配的部分
	m_szNameString = std::regex_replace(m_szNameString, r1, "");
	m_szNameString = std::regex_replace(m_szNameString, r2, ""); 
	m_pNameLabel->SetText(m_szNameString.c_str());
}
void CItemNamePanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
	m_pNameLabel->SetVisible(state);
}
bool CItemNamePanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CItemNamePanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CItemNamePanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}
const char* CItemNamePanel::GetName() {
	return VIEWPORT_ITEMNAME_NAME;
}