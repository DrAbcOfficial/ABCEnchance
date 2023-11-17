#ifndef CTEXTMENU_H
#define CTEXTMENU_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class LabelEx;
}

class CTextMenu : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CTextMenu, vgui::EditablePanel);

	CTextMenu();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	void ApplySettings(KeyValues* inResourceData) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	virtual void OnThink() override;
	void SelectMenuItem(int slot);
	bool MsgShowMenu(const char* pszName, int iSize, void* pbuf);
private:
	void SetContent(const char* szMenu);

	void StartFade(bool state);

	vgui::LabelEx* m_pMenu = nullptr;

	int m_bitsValidSlots;
	bool m_bIsASMenu;

	std::string m_szMenuString;

	float m_flShutoffTime;

	float m_flFadeAnimateTime;
};
#endif