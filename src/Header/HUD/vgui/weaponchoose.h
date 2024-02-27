#ifndef CWEAPONCHOOSEPANEL_H
#define CWEAPONCHOOSEPANEL_H
#include <array>
#include <list>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"
#ifndef __AMMO_H__
class WEAPON;
#endif
namespace vgui {
	class ImageSprPanel;
	class ImagePanel;
}
class CWeaponChooseItem : public vgui::Panel {
public:
	DECLARE_CLASS_SIMPLE(CWeaponChooseItem, vgui::Panel);
	CWeaponChooseItem(vgui::Panel* parent, WEAPON* wep);

	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PerformLayout() override;

	void SetBounds(int x, int y, int w, int h);

	void UpdateColor();
	void SetActivate(bool state);
	WEAPON* GetWeapon();
private:
	vgui::ImageSprPanel* m_pWeaponPanel;
	vgui::ImageSprPanel* m_pWeaponInactivePanel;
	vgui::Panel* m_pAmmoBarBg1;
	vgui::Panel* m_pAmmoBar1;
	vgui::Panel* m_pAmmoBarBg2;
	vgui::Panel* m_pAmmoBar2;

	WEAPON* m_pWeapon = nullptr;

	Color m_cInactiveColor;
	Color m_cFgColor;

	Color m_cEmptyInactiveColor;
	Color m_cEmptyColor;
	Color m_cAmmoBarBgColor;
	Color m_cAmmoBarColor;
	Color m_cAmmoBarEmptyColor;
};

class CWeaponChoosePanel : public vgui::EditablePanel, public IViewportPanel{
public:
	DECLARE_CLASS_SIMPLE(CWeaponChoosePanel, vgui::EditablePanel);

	CWeaponChoosePanel();
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PerformLayout() override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void SelectWeapon();
	void ChooseWeapon(WEAPON* weapon);
	void InsertWeapon(WEAPON* weapon);
	void RemoveWeapon(WEAPON* weapon);
private:
	std::array<std::list<CWeaponChooseItem*>, 10> m_aryPanelList;
	vgui::PHandle m_pHandledWeapon;

	std::array<vgui::ImagePanel*, 10> m_arySlotPanel;
	vgui::ImagePanel* m_pSelectBucket;

	int m_iItemInactiveWide;
	int m_iItemInactiveHeight;
	int m_iItemWide;
	int m_iItemHeight;
	int m_iItemXGap;
	int m_iItemYGap;
};
#endif