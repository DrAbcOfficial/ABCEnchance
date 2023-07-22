#ifndef CGRENADEINDICATOR_H
#define CGRENADEINDICATOR_H
#include <vector>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include "IViewportPanel.h"

class CGIndicatorItem {
public:
	void SetExpire(float fl);
	bool IsExpire();
	cl_entity_t* GetEnt();
	void SetEnt(cl_entity_t* ent);
private:
	float m_flExpire;
	cl_entity_t* m_pEnt;
};

class CGenadeIndicatorPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CGenadeIndicatorPanel, vgui::EditablePanel);

	CGenadeIndicatorPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;
	virtual void OnThink() override;
	virtual void Paint() override;

	void AddEntity(cl_entity_s* ent, const char* modelname);
private:
	std::vector<CGIndicatorItem*> m_aryImages;
	Color m_cImageDrawColor;
	int m_iImageTexture;
	int m_iImageWide;
	int m_iImageTall;
};
#endif