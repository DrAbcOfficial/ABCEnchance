#pragma once
#ifndef CITEMHIGHLIGHTPANEL_H
#define CITEMHIGHLIGHTPANEL_H

#include <unordered_map>
#include <optional>

#include "vgui_controls/EditablePanel.h"
#include "vgui/IViewportPanel.h"

namespace vgui {
	class Label;
}

class CItemHighLightPanel : public vgui::EditablePanel, public IViewportPanel{
public:
	using cl_highlight_t = struct cl_highlight_s {
		std::string Name;
		//0 Item 1 Ammo 2 Weapon
		int Type = 0;
		int Index = -1;
	};

	DECLARE_CLASS_SIMPLE(CItemHighLightPanel, vgui::EditablePanel);
	CItemHighLightPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void OnThink() override;

	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual bool IsVisible() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void CreateHighLight(cl_entity_t* var);
	void AddEntity(int type, cl_entity_s* ent, const char* modelname);
	void EraseHighLight(cl_entity_t* var, int modelindex);
private:
	void LoadItemList();
	std::optional<int> m_iHighLightMdl;

	vgui::Panel* m_pLookatPanel;
	//temp entity
	std::unordered_map<int, std::pair<TEMPENTITY*, TEMPENTITY*>> m_mapEntityRestored;
	//perm list
	std::unordered_map<int, cl_highlight_t*> m_mapHighLightTable;
};
#endif // !CITEMHIGHLIGHTPANEL_H