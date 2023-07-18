#ifndef CAMMOPANEL_H
#define CAMMOPANEL_H
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
	class ImageSprPanel;
	class CSPRImage;
}

class WEAPON;

class CAmmoPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CAmmoPanel, vgui::EditablePanel);

	CAmmoPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void SetWeapon(WEAPON* weapon);
private:
	enum WEAPONSTATE {
		NOTVALID = 0,
		VALID = 1 << 0,
		ONTARGET = 1 << 1,
		UZI = 1 << 2
	};
	vgui::ImagePanel* m_pBackground = nullptr;
	vgui::ImageSprPanel* m_Ammo1Icon = nullptr;
	vgui::Label* m_pAmmo1Label = nullptr;

	vgui::Label* m_pSlashLabel = nullptr;

	vgui::ImageSprPanel* m_Ammo2Icon = nullptr;
	vgui::Label* m_pAmmo2Label = nullptr;

	vgui::CSPRImage* m_pSpr1 = nullptr;
	vgui::CSPRImage* m_pSpr2 = nullptr;
};
#endif