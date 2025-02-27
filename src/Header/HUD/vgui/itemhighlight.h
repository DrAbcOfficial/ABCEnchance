#pragma once
#ifndef CITEMHIGHLIGHTPANEL_H
#define CITEMHIGHLIGHTPANEL_H

#include <unordered_map>
#include <vector>

#include "vgui_controls/EditablePanel.h"
#include "vgui/IViewportPanel.h"

namespace vgui {
	class Label;
}

class CItemHighLightPanel : public vgui::EditablePanel, public IViewportPanel{
public:
	using cl_highlight_t = struct cl_highlight_s {
		int ModelIndex;
		std::string Path;
		std::wstring Name;
		short R;
		short G;
		short B;
		short A;
	};

	DECLARE_CLASS_SIMPLE(CItemHighLightPanel, vgui::EditablePanel);
	CItemHighLightPanel();
	virtual void OnThink() override;

	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual bool IsVisible() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void AddEntity(int type, cl_entity_s* ent, const char* modelname);
	void EraseEntity(cl_entity_t* var, int modelindex);

	std::vector<vgui::Panel*> m_aryLookatPanels{};
private:
	void LoadItemList();
	vgui::Panel* m_pPickupPanel = nullptr;
	//EntityIndex, HighLightIndex
	std::unordered_map<int, int> m_dicStoredEntity{};
	//perm list
	bool m_bHasFilledIndex;
	std::vector<cl_highlight_t*> m_aryHighLightTable;
};
#endif // !CITEMHIGHLIGHTPANEL_H