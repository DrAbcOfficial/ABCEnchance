#pragma once
#ifndef CCROSSHAIRPANEL_H
#define CCROSSHAIRPANEL_H
#include <string>
#include <array>

#include <VGUI2/Border.h>
#include <vgui_controls/Panel.h>
#include "IViewportPanel.h"

typedef struct cvar_s cvar_t;
#ifndef __AMMO_H__
class WEAPON;
#endif

namespace vgui {
	class CSPRImage;
}

class CCrosshairPanel : public vgui::Panel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CCrosshairPanel, vgui::Panel);

	CCrosshairPanel();
	virtual void PaintBackground() override;
	virtual void OnThink() override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void SetWeapon(WEAPON* weapon);
private:
	cvar_t* pCvarDefaultCrosshair = nullptr;

	cvar_t* pDynamicCrossHairAH = nullptr;
	cvar_t* pDynamicCrossHairL = nullptr;
	cvar_t* pDynamicCrossHairW = nullptr;
	cvar_t* pDynamicCrossHairO = nullptr;
	cvar_t* pDynamicCrossHairM = nullptr;
	cvar_t* pDynamicCrossHairA = nullptr;
	cvar_t* pDynamicCrossHairCR = nullptr;
	cvar_t* pDynamicCrossHairCG = nullptr;
	cvar_t* pDynamicCrossHairCB = nullptr;
	cvar_t* pDynamicCrossHairOA = nullptr;
	cvar_t* pDynamicCrossHairOR = nullptr;
	cvar_t* pDynamicCrossHairOG = nullptr;
	cvar_t* pDynamicCrossHairOB = nullptr;
	cvar_t* pDynamicCrossHairOTD = nullptr;
	cvar_t* pDynamicCrossHairOTDW = nullptr;
	cvar_t* pDynamicCrossHairT = nullptr;
	cvar_t* pDynamicCrossHairD = nullptr;
	cvar_t* pDynamicCrossHairAnimInterval = nullptr;

	vgui::CSPRImage* m_pSprImage;
	WEAPON* m_pHandledWeapon = nullptr;
	std::array<vgui::Panel*, 5> m_aryCrosshair;
	std::array<vgui::Panel*, 5> m_aryCrosshairBorder;

	int m_iCenterX;
	int m_iCenterY;
	int m_iPreOffest = 0;
	float m_flAnimTime = 0;

	void HideDynamicCrossHair();
	void SetCrosshairSPR(int x, int y, int hPic, wrect_t* hRc);
	void DrawDefaultCrosshair(int x, int y);
};
#endif