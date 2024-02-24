#ifndef CAMMOSTACKPANEL_H
#define CAMMOSTACKPANEL_H
#include <vector>
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class ImageSprPanel;
}

class CAmmoStackItem : public vgui::EditablePanel {
public:
	DECLARE_CLASS_SIMPLE(CAmmoStackItem, vgui::EditablePanel);
	CAmmoStackItem(Panel* parent, const char* text, const char* icon, int value);

	virtual void PerformLayout() override;
	const char* GetIconKey();
	int GetValue();

	void SetIconColor(Color in);
	void SetTextColor(Color in);
	void SetImage(const char* image);
	void Show(float flTime);

	void Reset();
	void SetExpire(float f);
	void CheckExpire();
private:
	vgui::Label* m_pText;
	vgui::ImageSprPanel* m_pPanel;

	char szIconKey[32];
	int iValue;
	float fExpire;
};

class CAmmoStackPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CAmmoStackPanel, vgui::EditablePanel);

	CAmmoStackPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void OnThink() override;
	virtual void PaintBackground() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

private:
	float m_flKeepTime;
	std::vector<CAmmoStackItem*> m_aryAmmos;

	Color m_cItemBgColor;
	Color m_cItemIconColor;
	Color m_cItemTextColor;
};
#endif