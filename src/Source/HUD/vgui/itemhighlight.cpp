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
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/GaussianBlurPanel.h"
#include "vgui_controls/AnimationController.h"

#include "Viewport.h"
#include "itemhighlight.h"

constexpr auto ITEM_LIST_PATH = "abcenchance/ItemHighLightList.txt";
constexpr auto VIEWPORT_ITEMHIGHLIGHT_NAME = "ItemHighLightPanel";
extern vgui::HScheme GetViewPortBaseScheme();

class CItemNamePanel : public vgui::EditablePanel {
	DECLARE_CLASS_SIMPLE(CItemNamePanel, vgui::EditablePanel);
public:
	CItemNamePanel(vgui::Panel* parent) : BaseClass(parent, "ItemNamePanel") {
		SetMouseInputEnabled(false);
		SetKeyBoardInputEnabled(false);
		SetProportional(true);
		SetScheme(GetViewPortBaseScheme());

		m_pLabel = new vgui::Label(this, "Text", "");
		m_pImage = new vgui::ImagePanel(this, "Image");
		LoadControlSettings(VGUI2_ROOT_DIR "ItemNamePanel.res");

		GetSize(m_iOW, m_iOH);

		m_pLabel->SetAutoDelete(true);
		m_pImage->SetAutoDelete(true);
	}
	
	void Update(CItemHighLightPanel::cl_highlight_t* item, int ent, int length) {
		//���Ϊ10
#undef clamp
		float ratio = std::clamp<float>(1.0f - (gCVars.pItemHighLightRange->value - length) / (gCVars.pItemHighLightRange->value),
								0.5f, 1.0f);
		int nw = m_iOW * ratio;
		int nh = m_iOH * ratio;
		m_pImage->SetSize(nw, nh);
		m_pImage->SetPos((m_iOW - nw) / 2, (m_iOH - nh) / 2);
		Color c = Color(item->R, item->G, item->B, item->A);
		m_pLabel->SetFgColor(c);
		m_pLabel->SetText(item->Name.c_str());
		m_pImage->SetDrawColor(c);
		m_pItem = item;
		m_iEnt = ent;

	}
	virtual void OnThink() override {
		BaseClass::OnThink();
		auto ent = gEngfuncs.GetEntityByIndex(m_iEnt);
		auto local = gEngfuncs.GetLocalPlayer();
		if (!ent || !local || ent->curstate.messagenum != local->curstate.messagenum) {
			auto parent = reinterpret_cast<CItemHighLightPanel*>(GetParent());
			std::erase_if(parent->m_aryLookatPanels, [this](vgui::Panel* panel) {return panel == this;});
			DeletePanel();
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
	int m_iEnt;
private:
	CItemHighLightPanel::cl_highlight_t* m_pItem = nullptr;
	vgui::Label* m_pLabel = nullptr;
	vgui::ImagePanel* m_pImage = nullptr;
	int m_iOW;
	int m_iOH;
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
			vgui::GetAnimationController()->CancelAnimationsForPanel(GetParent());
			vgui::GetAnimationController()->StartAnimationSequence(GetParent(), "ItemPickupNoticePopUp");
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
	void UpdateName(const wchar_t* name) {
		std::wstring sz = m_szTemplate;
		const char* tkey = gEngfuncs.Key_LookupBinding("use");
		if (tkey != nullptr) {
			std::wstring key;
			std::transform(tkey, tkey + std::strlen(tkey), std::back_inserter(key), [](char c) {
				return static_cast<wchar_t>(std::toupper(c));
			});
			size_t pos = sz.find(L"%%USE_BINDKEY%%");
			if (pos != std::wstring::npos)
				sz = sz.replace(pos, std::wcslen(L"%%USE_BINDKEY%%"), key);
			pos = sz.find(L"%%PICKUP_ITEM%%");
			if (pos != std::wstring::npos)
				sz = sz.replace(pos, std::wcslen(L"%%PICKUP_ITEM%%"), name);
		}
		m_pText->SetText(sz.c_str());
	}
	void GetTemplateText() {
		wchar_t temp[256];
		m_pText->GetText(temp, 256);
		m_szTemplate = temp;
		
	}
private:
	vgui::GaussianBlurPanel* m_pGaussian = nullptr;
	vgui::Label* m_pText = nullptr;
	std::wstring m_szTemplate;
};
CItemHighLightPanel::CItemHighLightPanel() : BaseClass(nullptr, VIEWPORT_ITEMHIGHLIGHT_NAME){
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	SetProportional(true);
	SetScheme(GetViewPortBaseScheme());

	gCVars.pItemHighLight = CREATE_CVAR("hud_highlight_item", "1", FCVAR_VALUE, NULL);
	gCVars.pItemHighLightRange = CREATE_CVAR("hud_highlight_item_range", "344", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = CMathlib::clamp<float>(cvar->value, 0.0f, 344.0f);
	});
	gCVars.pItemHighLightName = CREATE_CVAR("hud_highlight_item_name", "1", FCVAR_VALUE, NULL);
	gCVars.pItemHighLightNameFOV = CREATE_CVAR("hud_highlight_item_fov", "90", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = fmodf(cvar->value, 360.0f);
	});
	gCVars.pItemHighLightAimFOV = CREATE_CVAR("hud_highlight_item_aimfov", "20", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = fmodf(cvar->value, 360.0f);
	});
	gCVars.pItemHighLightPickup = CREATE_CVAR("hud_highlight_item_pickup", "1", FCVAR_VALUE, NULL);
	m_pPickupPanel = new CItemPickupPanel(this, "Pickup");
	LoadControlSettings(VGUI2_ROOT_DIR "ItemHighLightPanel.res");

	reinterpret_cast<CItemPickupPanel*>(m_pPickupPanel)->GetTemplateText();
	LoadItemList();
}
void CItemHighLightPanel::OnThink(){
	if (gCVars.pItemHighLightName->value <= 0 && gCVars.pItemHighLightPickup->value <= 0)
		return;
	float maxdot = 0.0f;
	float needdot = cos(gCVars.pItemHighLightNameFOV->value * M_PI / 180.0f);
	cl_entity_t* maxdotent = nullptr;
	size_t highlightindex = 0;
	auto local = gEngfuncs.GetLocalPlayer();
	Vector vecView;
	gEngfuncs.GetViewAngles(vecView);
	CMathlib::AngleVectors(vecView, vecView, nullptr, nullptr);
	Vector vecViewOfs;
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(vecViewOfs);
	for (auto iter = m_dicStoredEntity.begin(); iter != m_dicStoredEntity.end();) {
		auto& item = *iter;
		int try_find = item.first;
		cl_entity_t* ent = gEngfuncs.GetEntityByIndex(try_find);
		auto lookat = std::find_if(m_aryLookatPanels.begin(), m_aryLookatPanels.end(), [&try_find](vgui::Panel* panel) {
			return reinterpret_cast<CItemNamePanel*>(panel)->m_iEnt == try_find;
		});
		CItemNamePanel* name_panel = (lookat == m_aryLookatPanels.end()) ? 
			nullptr : reinterpret_cast<CItemNamePanel*>(*lookat);
		if (!ent || ent->curstate.messagenum != local->curstate.messagenum) {
			if (name_panel != nullptr) {
				std::erase_if(m_aryLookatPanels, [&name_panel](vgui::Panel* panel) {return panel == name_panel; });
				name_panel->DeletePanel();
			}
			iter = m_dicStoredEntity.erase(iter);
			continue;
		}
		Vector vecLength = ent->curstate.origin;
		vecLength -= local->curstate.origin;
		vecLength -= vecViewOfs;
		if (vecLength.Length() >= gCVars.pItemHighLightRange->value) {
			if (name_panel != nullptr) {
				std::erase_if(m_aryLookatPanels, [&name_panel](vgui::Panel* panel) {return panel == name_panel; });
				name_panel->DeletePanel();
			}
			iter = m_dicStoredEntity.erase(iter);
			continue;
		}
		float dot = vecLength.Normalize().Dot(vecView.Normalize());
		if (dot > maxdot) {
			maxdot = dot;
			maxdotent = ent;
			highlightindex = item.second;
		}
		if (dot >= needdot) {
			if (name_panel == nullptr) {
				name_panel = new CItemNamePanel(this);
				m_aryLookatPanels.push_back(name_panel);
			}
			name_panel->Update(m_aryHighLightTable[item.second], try_find, vecLength.Length());
		}
		else if (name_panel != nullptr) {
			std::erase_if(m_aryLookatPanels, [&name_panel](vgui::Panel* panel) {return panel == name_panel; });
			name_panel->DeletePanel();
		}	
		iter++;
	}

	float needaimdot = cos(gCVars.pItemHighLightAimFOV->value * M_PI / 180.0f);
	if (maxdotent != nullptr && maxdot >= needaimdot) {
		auto item = m_aryHighLightTable[highlightindex];
		if (item != nullptr) {
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
	else
		m_pPickupPanel->SetVisible(false);
}
void CItemHighLightPanel::Reset() {
	m_dicStoredEntity.clear();
	m_bHasFilledIndex = false;
}
void CItemHighLightPanel::AddEntity(int type, cl_entity_s* ent, const char* modelname){
	if (gCVars.pItemHighLight->value <= 0)
		return;
	//mdlģ��
	if ((ent) && (ent->model) && (ent->model->type == mod_studio) &&
		ent->baseline.movetype != MOVETYPE_FOLLOW && 
		((ent->curstate.effects & EF_NODRAW) == 0)) {
		if (!m_bHasFilledIndex) {
			for (auto iter = m_aryHighLightTable.begin(); iter != m_aryHighLightTable.end(); iter++) {
				auto& item = (*iter);
				if (item == nullptr)
					continue;
				item->ModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex(item->Path.c_str());
			}
			m_bHasFilledIndex = true;
		}
		int index = -1;
		for (size_t i = 0; i < m_aryHighLightTable.size(); i++){
			if (m_aryHighLightTable[i]->ModelIndex == ent->curstate.modelindex) {
				index = i;
				break;
			}
		}
		if (index > -1)
			m_dicStoredEntity.emplace(std::make_pair(ent->index, index));
	}
}
void CItemHighLightPanel::LoadItemList() {
	char szItemPraseBuf[256];
	const char* pfile = const_cast<const char*>(reinterpret_cast<char*>(gEngfuncs.COM_LoadFile(ITEM_LIST_PATH, 5, nullptr)));
	int i = 0;
	cl_highlight_s* pItem = nullptr;
	if (!pfile){
		gEngfuncs.Con_DPrintf("CHudItemHighLight::LoadItemList: No item list file %s\n", ITEM_LIST_PATH);
		return;
	}
	while (true){
		pfile = gEngfuncs.COM_ParseFile(pfile, szItemPraseBuf);
		if (!pfile)
			break;
		if (i == 0) {
			pItem = new cl_highlight_s();
			pItem->Path = szItemPraseBuf;
			m_aryHighLightTable.push_back(pItem);
		}
		else {
			if (pItem == nullptr)
				continue;

			auto pszName = vgui::localize()->Find(szItemPraseBuf);

			switch (i){
			case 1:pItem->Name = pszName ? pszName : L""; break;
				case 2:pItem->R = atoi(szItemPraseBuf); break;
				case 3:pItem->G = atoi(szItemPraseBuf); break;
				case 4:pItem->B = atoi(szItemPraseBuf); break;
				case 5:pItem->A = atoi(szItemPraseBuf); break;
			}
		}
		i++;
		if (i > 5)
			i = 0;
	}
	if (i != 0)
		SYS_ERROR("Error in parsing file:%s\nBuf is not end with even.", ITEM_LIST_PATH);
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