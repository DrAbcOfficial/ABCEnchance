#ifndef CWEAPONSTACKPANEL_H
#define CWEAPONSTACKPANEL_H
#include "basestackpanel.h"

class CWeaponStackItem : public CBaseSprStackItem {
public:
	DECLARE_CLASS_SIMPLE(CWeaponStackItem, CBaseSprStackItem);
	CWeaponStackItem(vgui::Panel* parent, int spridx, int l, int r, int t, int b, float expire, float fi, float fo, size_t wi);
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void CheckExpire() override;
private:
	size_t m_iWeapon;
	Color m_cEmpty;
	Color m_cDraw;
};

class CWeaponStackPanel : public CBaseStackPanel{
public:
	DECLARE_CLASS_SIMPLE(CWeaponStackPanel, CBaseStackPanel);
	CWeaponStackPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PaintBackground() override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	void AddItemPickup(int wepindex);
};
#endif