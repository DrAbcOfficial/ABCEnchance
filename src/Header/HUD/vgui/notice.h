#ifndef CNOTICEPANEL_H
#define CNOTICEPANEL_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
}

class CNoticePanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CNoticePanel, vgui::EditablePanel);

	CNoticePanel(const char* szControlName);

	void ShowMessage(const wchar_t* message);

	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;
	virtual void OnThink() override;
private:
	vgui::Label* m_pMessage = nullptr;
	
	float m_flCloseTime = 0;
	float m_flKeepTime = 0;
	bool m_bKeepCenter = false;
};
#endif