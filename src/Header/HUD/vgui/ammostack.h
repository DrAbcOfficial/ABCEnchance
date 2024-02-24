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
	CAmmoStackItem(Panel* parent, int spridx, int value, int l, int r, int t, int b, float expire, float fi, float fo, float is);

	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PerformLayout() override;
	int GetValue();
	void SetImage(const char* image);
	void Show(float flTime);

	void SetExpire(float f);
	void CheckExpire();
private:
	vgui::Label* m_pText;
	vgui::ImageSprPanel* m_pPanel;

	int iSprIdx;
	int iValue;
	float fKeepTime;
	float fIn;
	float fOut;
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

	void AddAmmoPickup(int id, int count);

private:
	float m_flFadeinTime;
	float m_flFadeoutTime;
	float m_flKeepTime;
	float m_flIconSize;
	std::vector<CAmmoStackItem*> m_aryAmmos;
};
#endif