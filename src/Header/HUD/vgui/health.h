#ifndef CHEALTHPANEL_H
#define CHEALTHPANEL_H
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
	class ImageClipPanel;
	class CSPRImage;
}

class CHealthPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CHealthPanel, vgui::EditablePanel);

	CHealthPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void SetHealth(int health);
	void SetArmor(int armor);

	void SetLongJump(bool lj);
	void SetArmorVisible(bool state);
	void SetHealthVisible(bool state);
private:
	int m_iRestoredHealWide = 0;
	int m_iRestoredArmorWide = 0;

	Color m_cRestoredHealth;
	Color m_cRestoredArmor;

	Color m_cHealthDanger;
	Color m_cArmorDanger;

	vgui::ImagePanel* m_Backround = nullptr;

	vgui::ImagePanel* m_HealthIcon = nullptr;
	vgui::Label* m_pHealthLable = nullptr;
	vgui::ImagePanel* m_pHealthImagePanel = nullptr;
	vgui::ImagePanel* m_HealthImageBackround = nullptr;

	vgui::ImagePanel* m_ArmorIcon = nullptr;
	vgui::ImagePanel* m_pArmorImagePanel = nullptr;
	vgui::Label* m_pArmorLable = nullptr;
	vgui::ImagePanel* m_ArmorImageBackround = nullptr;

	vgui::ImagePanel* m_pLongJumpImagePanel = nullptr;
};
#endif