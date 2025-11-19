#pragma once
#ifndef CINDICATORPANEL_H
#define CINDICATORPANEL_H
#include <array>
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"
typedef struct cvar_s cvar_t;
namespace vgui {
	class ImagePanel;
}
class CIndicatorPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CIndicatorPanel, vgui::EditablePanel);

	CIndicatorPanel();
	virtual ~CIndicatorPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void SetHitIndicator(int damage, int armor, const float vecFrom[3]);
private:
	std::array<vgui::ImagePanel*, 3> m_aryImagePanels;
	size_t m_iIndex = 0;
	int m_iTex;
	float m_flFadeTime = 3.0f;
	cvar_t* m_pCvarIndicatorStyle = nullptr;
	float m_vecHitFrom[3]{};
	int m_iDamage = 0;
	int m_iArmor = 0;
};
#endif