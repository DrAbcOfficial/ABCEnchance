#ifndef CAMMOSTACKPANEL_H
#define CAMMOSTACKPANEL_H
#include "basestackpanel.h"

class CAmmoStackItem : public CBaseSprStackItem {
public:
	DECLARE_CLASS_SIMPLE(CAmmoStackItem, CBaseSprStackItem);
	CAmmoStackItem(Panel* parent, int spridx, int value, int l, int r, int t, int b, float expire, float fi, float fo);
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PerformLayout() override;
	int GetValue() const;

private:
	vgui::Label* m_pText;
	int iValue;
};

class CAmmoStackPanel : public CBaseStackPanel{
public:
	DECLARE_CLASS_SIMPLE(CAmmoStackPanel, CBaseStackPanel);
	CAmmoStackPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PerformLayout() override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	void AddAmmoPickup(int id, int count);

	vgui::HFont m_hFontOverrride;
};
#endif