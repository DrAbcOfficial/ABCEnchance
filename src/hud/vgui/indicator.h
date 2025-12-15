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
	virtual void PerformLayout() override;
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
	virtual void OnThink() override;
private:
	std::array<vgui::ImagePanel*, 4> m_aryImagePanels;
	std::array<float, 4> m_flAnimationStartTime{};
	size_t m_iIndex = 0;
	int m_iTex;
	float m_flFadeTime = 1.0f;
	float m_flKeepTime = 1.0f;
	cvar_t* m_pCvarIndicatorStyle = nullptr;
	cvar_t* m_pDamageScreenFilter = nullptr;
	cvar_t* m_pDamageScreenFactor = nullptr;
	cvar_t* m_pDamageScreenBase = nullptr;
	float m_vecHitFrom[3]{};
	int m_iDamage = 0;
	int m_iArmor = 0;
};
#endif