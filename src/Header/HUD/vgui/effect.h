#pragma once
#ifndef EFFECTPANEL_H
#define EFFECTPANEL_H

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"


namespace vgui {
	class ImagePanel;
	class ImageSprPanel;
}

class CEffectPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CEffectPanel, vgui::EditablePanel);

	CEffectPanel();
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	virtual void OnThink() override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;

	void SetHealth(int health);

	enum class WaterType {
		NONE = 0,
		UNDER_WATER = 1,
		OUT_WATER = 12
	};
	enum WATERLEVEL {
		WATERLEVEL_DRY = 0,
		WATERLEVEL_FEET,
		WATERLEVEL_WAIST,
		WATERLEVEL_HEAD
	};
	void SetWater(WaterType type);
private:
	float m_flAlphaAinmeTime = 0;
	float m_flWaterKeepTime = 0;

	vgui::ImageSprPanel* m_pLeftWater = nullptr;
	vgui::ImageSprPanel* m_pRightWater = nullptr;
	vgui::ImagePanel* m_pUnderWater = nullptr;
	vgui::ImagePanel* m_pDangerPanel = nullptr;

	int m_iOldWaterStatue = 0;

	WaterType m_iNowWaterType;
};
#endif