#ifndef CRADARPANEL_H
#define CRADARPANEL_H
#pragma once

#include <array>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include "IViewportPanel.h"

class CAvatarImage;

class CRadarAvatarPanel : public vgui::ImagePanel {
	DECLARE_CLASS_SIMPLE(CRadarAvatarPanel, ImagePanel);
public:
	CRadarAvatarPanel(vgui::Panel* parent, int index);
	virtual void Paint() override;
private:
	CAvatarImage* m_pAvatar;
	int m_iIndex;
};

class CRadarPanel : public vgui::EditablePanel, public IViewportPanel {
public:
	DECLARE_CLASS_SIMPLE(CRadarPanel, vgui::EditablePanel);
	
	CRadarPanel();
	~CRadarPanel();
	virtual void PerformLayout() override;
	virtual void Paint() override;
	virtual void OnThink() override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void SetScale(bool state);
	void SetAvatarVisible(bool state);
	void BlitFramebuffer();
private:
	vgui::ImagePanel* m_pBackground;
	vgui::ImagePanel* m_pRoundBackground;
	vgui::ImagePanel* m_pMapground;
	vgui::ImagePanel* m_pUpground;
	vgui::ImagePanel* m_pNorthground;
	vgui::ImagePanel* m_pViewangleground;

	std::array<CRadarAvatarPanel*, 32> m_aryPlayerAvatars;

	float m_flRoundRadius;
	Color m_cOutline;
	Color m_cMap;

	uint m_hRadarBufferFBO;

	int m_iStartWidth;
	int m_iStartTall;

	float flNextUpdateTrTime;
	bool m_bInScale;

	size_t iOverviewR = 0;
	size_t iOverviewG = 0;
	size_t iOverviewB = 0;

	vec3_t m_oldViewOrg;
	vec3_t m_oldViewAng;
	int m_oldFrameBuffer;
};
#endif