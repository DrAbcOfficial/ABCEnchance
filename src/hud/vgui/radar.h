#ifndef CRADARPANEL_H
#define CRADARPANEL_H
#pragma once

#include <array>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include "IViewportPanel.h"
#include "core/opengl/gl_common.h"

namespace vgui {
	class CAvatarImagePanel;
}

class CRadarMapImage;

class CRadarPanel : public vgui::EditablePanel, public IViewportPanel {
public:
	DECLARE_CLASS_SIMPLE(CRadarPanel, vgui::EditablePanel);
	
	CRadarPanel();
	~CRadarPanel();
	virtual void PerformLayout() override;
	virtual void Paint() override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void RenderRadar();
	void SetScale(bool state);
	void SetAvatarVisible(bool state);
	void RefreshAvatars();
private:
	vgui::ImagePanel* m_pBackground{};
	vgui::ImagePanel* m_pRoundBackground{};
	vgui::ImagePanel* m_pMapground{};
	vgui::ImagePanel* m_pNorthground{};
	vgui::ImagePanel* m_pViewangleground{};
	CRadarMapImage* m_pRadarImage{};
	int m_iRadarMaskId{};
	int m_iRadarRoundMaskId{};

	std::array<vgui::CAvatarImagePanel*, 32> m_aryPlayerAvatars;

	FBO_Container_t m_RadarFBO{ };

	int m_iStartWidth{};
	int m_iStartTall{};
	int m_iScaledWidth{};
	int m_iScaledTall{};

	vec3_t m_vecOverViewOrigin{};
	vec3_t m_vecOverViewAngles{};
	
	vec4_t m_vClearColor{};

	float m_flBrightness{};
};
#endif