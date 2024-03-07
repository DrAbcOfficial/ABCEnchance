#pragma once
#include <map>
#include <vector>
#include "Label.h"
#include "EditablePanel.h"
#include "IViewportPanel.h"

extern class CItemNamePanel;

typedef struct cl_highlight_s
{
	char Name[64];
	//0 Item 1 Ammo 2 Weapon
	int Type = 0;
	int Index = -1;
}cl_highlight_t;

class CItemHighLightPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CItemHighLightPanel, vgui::EditablePanel);
	CItemHighLightPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void Think() override;

	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual bool IsVisible() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void CreateHighLight(cl_entity_t* var);
	void ItemHighLightReset();
	void AddEntity(int type, cl_entity_s* ent, const char* modelname);
	void EraseHighLight(cl_entity_t* var, int modelindex);
private:
	void LoadItemList();
	int m_iHighLightMdl = 0;
	std::vector<cl_highlight_t*> aryHighLightList;
	std::map<int, CItemNamePanel*> m_aryHighLightPanel;
	std::map<int, cl_highlight_t*> m_mapHighLightTable;
	std::map<int, std::pair<TEMPENTITY*, TEMPENTITY*>> m_mapRestoredTent;
};

class CItemNamePanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CItemNamePanel, vgui::EditablePanel);
	CItemNamePanel(vgui::VPANEL parent);
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme)override;
	virtual void SetSize(int w, int t);
	virtual void Reset() override;
	virtual void Think() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel()override;
	virtual void SetParent(vgui::VPANEL parent)override;
	virtual const char* GetName()override;

	void SetText();

	cl_highlight_t* item = nullptr;
	vec3_t* origin = nullptr;
private:
	std::string m_szNameString;
	vgui::Label* m_pNameLabel = nullptr;
};
