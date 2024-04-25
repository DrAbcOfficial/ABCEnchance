#include <metahook.h>
#include <filesystem>
#include "plugins.h"

#include "mathlib/vector.h"
#include "com_model.h"

#include "event_api.h"
#include "triangleapi.h"

#include "mymathlib.h"
#include "cvardef.h"
#include "extraprecache.h"
#include "exportfuncs.h"
#include "playertrace.h"

#include "vguilocal.h"
#include "local.h"

#include "vgui_controls/Label.h"
#include "vgui_controls/GaussianBlurPanel.h"
#include "vgui_controls/AnimationController.h"

#include "Viewport.h"
#include "itemhighlight.h"

#define ITEM_LIST_PATH "abcenchance/ItemHighLightList.txt"
#define VIEWPORT_ITEMHIGHLIGHT_NAME "ItemHighLightPanel"
extern vgui::HScheme GetViewPortBaseScheme();

class CItemNamePanel : public vgui::Label {
public:
	using vgui::Label::Label;
	DECLARE_CLASS_SIMPLE(CItemNamePanel, vgui::Label);
	void Update(CItemHighLightPanel::cl_highlight_t* item, int ent) {
		SetText(item->Name.c_str());
		m_pItem = item;
		m_iEnt = ent;
	}
	virtual void OnThink() override {
		BaseClass::OnThink();
		auto ent = gEngfuncs.GetEntityByIndex(m_iEnt);
		auto local = gEngfuncs.GetLocalPlayer();
		if (!ent || !local || ent->curstate.messagenum != local->curstate.messagenum) {
			SetVisible(false);
			return;
		}
		Vector vecHUD;
		VEC_WorldToScreen(ent->curstate.origin, vecHUD);
		int w, h;
		GetSize(w, h);
		SetPos(static_cast<int>(vecHUD.x) - w / 2, static_cast<int>(vecHUD.y) - h / 2);
	}
	virtual void Reset() {
		m_pItem = nullptr;
		m_iEnt = -1;
	}
private:
	CItemHighLightPanel::cl_highlight_t* m_pItem = nullptr;
	int m_iEnt;
};
class CItemPickupPanel : public vgui::Panel {
	using Panel::Panel;
	DECLARE_CLASS_SIMPLE(CItemPickupPanel, vgui::Panel);
public:
	CItemPickupPanel(vgui::Panel* parent, const char* name) : BaseClass(parent, name) {
		m_pGaussian = new vgui::GaussianBlurPanel(this, "Gaussian");
		m_pText = new vgui::Label(this, "Text", "#GameUI_ABC_ItemPickupNotice");
	}
	virtual void SetVisible(bool state) override {
		if (state && !IsVisible()) {
			SetWide(0);
			vgui::GetAnimationController()->RunAnimationCommand(this, "wide", m_iOldWide, 0.0f, 0.2f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		}
		BaseClass::SetVisible(state);
	}
	virtual void PerformLayout() override {
		BaseClass::PerformLayout();
		int w, h;
		GetSize(w, h);
		m_pGaussian->SetSize(w, h);
		m_pText->SetSize(w, h);
	}
	virtual void ApplySettings(KeyValues* inResourceData) override {
		BaseClass::ApplySettings(inResourceData);
		const char* overrideFont = inResourceData->GetString("font", "");
		vgui::IScheme* pScheme = vgui::scheme()->GetIScheme(GetScheme());
		if (*overrideFont)
			m_pText->SetFont(pScheme->GetFont(overrideFont, IsProportional()));
		if (inResourceData->GetInt("dulltext", 0) == 1)
			m_pText->SetTextColorState(vgui::Label::CS_DULL);
		else if (inResourceData->GetInt("brighttext", 0) == 1)
			m_pText->SetTextColorState(vgui::Label::CS_BRIGHT);
		else
			m_pText->SetTextColorState(vgui::Label::CS_NORMAL);
		const char* alignmentString = inResourceData->GetString("textAlignment", "");
		int align = -1;
		if (!stricmp(alignmentString, "north-west"))
			align = vgui::Label::a_northwest;
		else if (!stricmp(alignmentString, "north"))
			align = vgui::Label::a_north;
		else if (!stricmp(alignmentString, "north-east"))
			align = vgui::Label::a_northeast;
		else if (!stricmp(alignmentString, "west"))
			align = vgui::Label::a_west;
		else if (!stricmp(alignmentString, "center"))
			align = vgui::Label::a_center;
		else if (!stricmp(alignmentString, "east"))
			align = vgui::Label::a_east;
		else if (!stricmp(alignmentString, "south-west"))
			align = vgui::Label::a_southwest;
		else if (!stricmp(alignmentString, "south"))
			align = vgui::Label::a_south;
		else if (!stricmp(alignmentString, "south-east"))
			align = vgui::Label::a_southeast;
		if (align != -1)
			m_pText->SetContentAlignment((vgui::Label::Alignment)align);
		m_pGaussian->SetBlurness(inResourceData->GetInt("blur", 3));
	}
	void UpdateName(const char* name) {
		std::string sz = m_szTemplate;
		const char* tkey = gEngfuncs.Key_LookupBinding("use");
		if (tkey != nullptr) {
			std::string key;
			std::transform(tkey, tkey + std::strlen(tkey), std::back_inserter(key), [](char c) {
				return std::toupper(c);
				});
			size_t pos = sz.find("%%USE_BINDKEY%%");
			if (pos != std::string::npos)
				sz = sz.replace(pos, std::strlen("%%USE_BINDKEY%%"), key);
			pos = sz.find("%%PICKUP_ITEM%%");
			if (pos != std::string::npos)
				sz = sz.replace(pos, std::strlen("%%PICKUP_ITEM%%"), name);
		}
		m_pText->SetText(sz.c_str());
	}
	void SetOldValue() {
		m_iOldWide = GetWide();
		char temp[256];
		m_pText->GetText(temp, 256);
		m_szTemplate = temp;
		
	}
private:
	vgui::GaussianBlurPanel* m_pGaussian = nullptr;
	vgui::Label* m_pText = nullptr;
	std::string m_szTemplate;
	size_t m_iOldWide = 0;
};
CItemHighLightPanel::CItemHighLightPanel() : BaseClass(nullptr, VIEWPORT_ITEMHIGHLIGHT_NAME){
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	SetProportional(true);
	SetScheme(GetViewPortBaseScheme());

	gCVars.pItemHighLight = CREATE_CVAR("cl_itemhighlight", "1", FCVAR_VALUE, NULL);
	gCVars.pItemHighLightRange = CREATE_CVAR("cl_itemhighlightrange", "344", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = CMathlib::clamp<float>(cvar->value, 0.0f, 344.0f);
	});
	gCVars.pItemHighLightName = CREATE_CVAR("cl_itemhighlightname", "1", FCVAR_VALUE, NULL);
	gCVars.pItemHighLightNameFOV = CREATE_CVAR("cl_itemhighlightfov", "20", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = fmodf(cvar->value, 360);
	});
	gCVars.pItemHighLightPickup = CREATE_CVAR("cl_itemhighlightpickup", "1", FCVAR_VALUE, NULL);
	m_pLookatPanel = new CItemNamePanel(this, "ItemNamePanel", "");
	m_pPickupPanel = new CItemPickupPanel(this, "Pickup");
	LoadControlSettings(VGUI2_ROOT_DIR "ItemHighLightPanel.res");

	reinterpret_cast<CItemPickupPanel*>(m_pPickupPanel)->SetOldValue();
	LoadItemList();
}
void CItemHighLightPanel::OnThink(){
	if (gCVars.pItemHighLightName->value <= 0 && gCVars.pItemHighLightPickup->value <= 0)
		return;
	//check
	float maxdot = 0.0f;
	cl_entity_t* maxdotent = nullptr;
	auto local = gEngfuncs.GetLocalPlayer();
	Vector vecView;
	gEngfuncs.GetViewAngles(vecView);
	CMathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
	Vector vecViewOfs;
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(vecViewOfs);
	for (auto iter = m_mapEntityRestored.begin(); iter != m_mapEntityRestored.end();) {
		auto& item = *iter;
		cl_entity_t* ent = gEngfuncs.GetEntityByIndex(item.first);
		if (!ent) {
			item.second.first->die = item.second.second->die = 0;
			iter = m_mapEntityRestored.erase(iter);
			continue;
		}
		Vector vecLength = ent->curstate.origin;
		vecLength -= local->curstate.origin;
		vecLength -= vecViewOfs;
		if (vecLength.Length() >= gCVars.pItemHighLightRange->value) {
			item.second.first->die = item.second.second->die = 0;
			iter = m_mapEntityRestored.erase(iter);
			continue;
		}
		float dot = vecLength.Normalize().Dot(vecView.Normalize());
		if (dot > maxdot) {
			maxdot = dot;
			maxdotent = ent;
		}
		iter++;
	}
	float needdot = cos(gCVars.pItemHighLightNameFOV->value * M_PI / 180);
	if (maxdotent != nullptr && maxdot >= needdot) {
		auto item = m_mapHighLightTable[maxdotent->curstate.modelindex];
		if (item != nullptr) {
			if (gCVars.pItemHighLightName->value > 0) {
				m_pLookatPanel->SetVisible(true);
				reinterpret_cast<CItemNamePanel*>(m_pLookatPanel)->Update(item, maxdotent->index);
			}
			if (gCVars.pItemHighLightPickup->value > 0) {
				Vector vecLength = maxdotent->curstate.origin;
				vecLength -= local->curstate.origin;
				if (vecLength.Length() <= 86) {
					reinterpret_cast<CItemPickupPanel*>(m_pPickupPanel)->UpdateName(item->Name.c_str());
					m_pPickupPanel->SetVisible(true);
				}
			}
		}
	}
	else {
		m_pLookatPanel->SetVisible(false);
		m_pPickupPanel->SetVisible(false);
	}
}
void CItemHighLightPanel::Reset() {
	m_iHighLightMdl.reset();
	m_mapEntityRestored.clear();
}
void CItemHighLightPanel::CreateHighLight(cl_entity_t* var) {
	if (!m_iHighLightMdl.has_value()) {
		std::unordered_map<int, cl_highlight_t*> temp = {};
		for (auto iter = m_mapHighLightTable.begin(); iter != m_mapHighLightTable.end(); iter++) {
			auto& item = (*iter).second;
			if (item == nullptr)
				continue;
			item->Index = gEngfuncs.pEventAPI->EV_FindModelIndex(item->Path.c_str());
			if (item->Index > -1)
				temp[item->Index] = item;
		}
		m_mapHighLightTable.clear();
		m_mapHighLightTable.swap(temp);
		m_iHighLightMdl = PrecacheExtraModel("abcenchance/mdl/item_highlight.mdl");
	}
	if (m_mapHighLightTable[var->curstate.modelindex] == nullptr)
		return;
	if (m_mapHighLightTable.find(var->curstate.modelindex) == m_mapHighLightTable.end())
		return;
	if (m_mapEntityRestored.find(var->index) != m_mapEntityRestored.end())
		return;
	model_t* mdl = gEngfuncs.hudGetModelByIndex(m_iHighLightMdl.value());
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
	ent1->callback = ent2->callback = [](TEMPENTITY* ent, float frametime, float currenttime) {
		int modelindex = ent->entity.curstate.iuser1;
		cl_entity_t* var = gEngfuncs.GetEntityByIndex(ent->clientIndex);
		static const auto erase = [&modelindex, &var, &ent]() {
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
	};
	m_mapEntityRestored[var->index] = std::make_pair(ent1, ent2);
}
void CItemHighLightPanel::EraseHighLight(cl_entity_t* var, int modelindex) {
	if (m_mapEntityRestored.find(var->index) == m_mapEntityRestored.end())
		return;
	m_mapEntityRestored[var->index].first->die = m_mapEntityRestored[var->index].second->die = 0;
	m_mapEntityRestored.erase(var->index);
}
void CItemHighLightPanel::AddEntity(int type, cl_entity_s* ent, const char* modelname){
	if (gCVars.pItemHighLight->value <= 0)
		return;
	//mdl模型
	if ((ent) && (ent->model) && (ent->model->type == mod_studio) && ent->baseline.movetype != MOVETYPE_FOLLOW)
		CreateHighLight(ent);
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
			item->Path = szItemPraseBuf;
			std::filesystem::path ph(item->Path);
			std::string phname = ph.filename().replace_extension().string();
			size_t pos = phname.find_first_of("_");
			if (pos != std::string::npos && pos + 1 < phname.size())
				phname = phname.substr(pos + 1);
			item->Name = phname;
			m_mapHighLightTable.emplace(std::make_pair(index, item));
		}
		else {
			if(m_mapHighLightTable.find(index) != m_mapHighLightTable.end())
				m_mapHighLightTable[index]->Type = CMathlib::clamp<int>(std::atoi(szItemPraseBuf), 0, 2);
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