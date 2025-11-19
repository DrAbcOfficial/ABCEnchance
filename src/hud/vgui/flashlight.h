#ifndef CFLASHLIGHTPANEL_H
#define CFLASHLIGHTPANEL_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
	class ImageClipPanel;
}

class CFlashLightPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CFlashLightPanel, vgui::EditablePanel);

	CFlashLightPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void SetFlashLight(bool on, int battery);
	void SetFlashBattery(int battery);

private:
	vgui::Label* m_pMessage = nullptr;
	vgui::ImagePanel* m_pOffImage = nullptr;
	vgui::ImagePanel* m_pOnImage = nullptr;
	vgui::ImageClipPanel* m_pPowerImage = nullptr;

	Color m_cIconColor;

	int m_iBattery = 0;
};
#endif