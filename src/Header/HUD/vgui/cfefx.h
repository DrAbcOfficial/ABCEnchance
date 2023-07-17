#pragma once
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"


class CKillMarkPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CKillMarkPanel, vgui::EditablePanel);

	CKillMarkPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;
	//virtual void OnThink() override;

	void ShowKillMark(int* iDmg);
	void StartFade(bool state, vgui::ImagePanel* panel, float delaytime, float fadetime);
	void ShowDmgMark(vgui::ImagePanel* panel, bool state);

private:
	vgui::ImagePanel* m_pKillMarkPoint = nullptr;
	vgui::ImagePanel* m_pDmgMarkOne = nullptr;
	vgui::ImagePanel* m_pDmgMarkTwo = nullptr;
	vgui::ImagePanel* m_pDmgMarkThree = nullptr;
	vgui::ImagePanel* m_pDmgMarkFour = nullptr;
	vgui::ImagePanel* m_pDmgMarkFive = nullptr;
	vgui::ImagePanel* m_pDmgStar = nullptr;
	vgui::ImagePanel* m_pDmgEffect = nullptr;

	int m_ioffestYPos;

};

