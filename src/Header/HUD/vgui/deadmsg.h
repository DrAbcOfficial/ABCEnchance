#ifndef CDEATHMSGPANEL_H
#define CDEATHMSGPANEL_H
#include <vector>
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
}
class CDeathMsgItem : public vgui::EditablePanel {
public:
	DECLARE_CLASS_SIMPLE(CDeathMsgItem, vgui::EditablePanel);
	CDeathMsgItem(Panel* parent, const wchar_t* victim, const wchar_t* attacker, const wchar_t* inflictor, vgui::IScheme* scheme);

	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PaintBackground() override;
private:
	vgui::Label* m_pAttacker;
	vgui::Label* m_pInflictor;
	vgui::Label* m_pVictim;
	vgui::Panel* m_pBackGround;
};
class CDeathMsgPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CDeathMsgPanel, vgui::EditablePanel);

	CDeathMsgPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void PaintBackground() override;
	void AddItem(const wchar_t* victim, const wchar_t* attacker, const wchar_t* inflictor);
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;
private:
	std::vector<CDeathMsgItem*> m_aryDeath;
	cvar_t* m_pDeathMsgCount = nullptr;
	vgui::IScheme* m_pScheme = nullptr;
};
#endif