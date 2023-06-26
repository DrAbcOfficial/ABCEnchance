#ifndef CTEXTMENU_H
#define CTEXTMENU_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ListViewPanel;
}

class CTextMenu : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CTextMenu, vgui::EditablePanel);

	CTextMenu();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	void ApplySettings(KeyValues* inResourceData) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void AddMenu(char* szMenu);
	void SetTitle(char* szTitle);
private:
	vgui::Label* m_pTitle = nullptr;
	vgui::ListViewPanel* m_pMenu = nullptr;

	float m_flDisplayTime = 0.0f;
};
#endif