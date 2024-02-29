#ifndef CMOTDPANEL_H
#define CMOTDPANEL_H
#include <string>
#include <vector>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class HTML;
}

class CMotdPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CMotdPanel, vgui::EditablePanel);

	CMotdPanel();
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

	void AppendMotd(char* message);
	void ShowMotd();

	void ForceAddPage();
private:
	vgui::Label* m_pMessage = nullptr;
	vgui::HTML* m_pHTML = nullptr;
	vgui::Panel* m_pProgressBar = nullptr;
	
	std::string m_szBuffer;
	std::vector<std::string> m_aryClipedPage;
	size_t m_iNowPage = 0;
	void LoadMissionBrief();
	void AddBuffer(std::string szBuffer);
	void BuildPageInternal();
	void BuildPage();
	void ChangePage(size_t iNewPage);

	bool m_bLoadedMissionBrief = false;
	bool m_bBuiledPages = false;
	float m_flShowTime = 0;
	float m_flKeepTime = 0;
	float m_flFadeTime = 0;
};
#endif