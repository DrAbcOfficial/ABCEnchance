#ifndef CRADARPANEL_H
#define CRADARPANEL_H
#pragma once

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

class CRadarPanel : public vgui::EditablePanel, public IViewportPanel {
public:
	DECLARE_CLASS_SIMPLE(CRadarPanel, vgui::EditablePanel);
	
	CRadarPanel();
	~CRadarPanel();
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

	void SetScaleTime(float time);
	void BlitFramebuffer();

	float flFinishScaleTime;
private:
	int m_iBackground;
	int m_iRoundBackground;
	int m_iUpGround;
	int m_iNorth;
	int m_iViewAngle;

	float m_flRadarSclaeTime;
	float m_flRoundRadius;
	int m_iGapSize;
	int m_iAngleSize;
	int m_iNorthSize;

	char m_szBackgroundImg[MAX_PATH];
	char m_szBackgroundRoundImg[MAX_PATH];
	char m_szUpgroundImg[MAX_PATH];
	char m_szNorthImg[MAX_PATH];
	char m_szViewangleImg[MAX_PATH];
	
	Color m_cOutline;
	Color m_cMap;

	uint m_hRadarBufferFBO;
	uint m_hRadarBufferTex;

	float flNextUpdateTrTime;

	size_t iOverviewR = 0;
	size_t iOverviewG = 0;
	size_t iOverviewB = 0;

	vec3_t m_oldViewOrg;
	vec3_t m_oldViewAng;
	int m_oldFrameBuffer;
};
#endif