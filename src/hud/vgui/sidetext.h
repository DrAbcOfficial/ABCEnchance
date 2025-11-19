#ifndef CSIDEPANEL_H
#define CSIDEPANEL_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
}

class CSidePanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CSidePanel, vgui::EditablePanel);

	CSidePanel();
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
private:
	vgui::Label* m_pMessage = nullptr;
	vgui::ImagePanel* m_pImage = nullptr;
	std::string m_szTeplate;

	float m_flMaxSpeed = 0;
};
#endif