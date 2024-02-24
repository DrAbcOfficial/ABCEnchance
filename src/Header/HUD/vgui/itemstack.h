#ifndef CITEMSTACKPANEL_H
#define CITEMSTACKPANEL_H
#include "basestackpanel.h"

class CItemStackItem : public CBaseSprStackItem {
public:
	DECLARE_CLASS_SIMPLE(CItemStackItem, CBaseSprStackItem);
	CItemStackItem(vgui::Panel* parent, int spridx, int l, int r, int t, int b, float expire, float fi, float fo);
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
};

class CItemStackPanel : public CBaseStackPanel{
public:
	DECLARE_CLASS_SIMPLE(CItemStackPanel, CBaseStackPanel);
	CItemStackPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PaintBackground() override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	void AddItemPickup(int sprindex, int l, int r, int t, int b);
};
#endif