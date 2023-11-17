#ifndef CPLAYERINFOPANEL_H
#define CPLAYERINFOPANEL_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui{
	class Label;
	class ImagePanel;
}

class CPlayerInfoPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CPlayerInfoPanel, vgui::EditablePanel);

	CPlayerInfoPanel();

	void SetId(int index);

	// Frame overrides
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void Think() override;

	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual bool IsVisible() override;
	virtual void SetParent(vgui::VPANEL parent) override;

private:
	vgui::Label* m_pNameLable = nullptr;
	vgui::ImagePanel* m_pBackgroundImagePanel = nullptr;
	vgui::ImagePanel* m_pHealthImagePanel = nullptr;
	vgui::ImagePanel* m_pArmorImagePanel = nullptr;
	vgui::ImagePanel* m_pCourchIconImagePanel = nullptr;
	vgui::ImagePanel* m_pMedikIconImagePanel = nullptr;
	vgui::ImagePanel* m_pDeadIconImagePanel = nullptr;

	size_t m_iPlayerIndex;

	/**
	 * Updates client's row in the scoreboard.
	 */
	void UpdateClientInfo();
};

#endif