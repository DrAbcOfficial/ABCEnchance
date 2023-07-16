#pragma once
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"


class CKillMarkPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CKillMarkPanel, vgui::EditablePanel);

	CKillMarkPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;
	//virtual void OnThink() override;
	void ShowKillMark(int iValue);

private:
	vgui::ImagePanel* m_pKillMark = nullptr;
};

/*TODO

class CDmgMarkPanel : public vgui::EditablePanel, public IViewportPanel
{

public:
	DECLARE_CLASS_SIMPLE(CDmgMarkPanel, vgui::EditablePanel);

	CDmgMarkPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;
	virtual void OnThink() override;

private:
	vgui::ImagePanel* m_pDmgMark = nullptr;
};
*/