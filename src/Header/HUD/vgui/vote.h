#ifndef CVOTEPANEL_H
#define CVOTEPANEL_H
#include <string>

#include <vgui_controls/Frame.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
}

class CVotePanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CVotePanel, vgui::EditablePanel);

	CVotePanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void StartVote(char* szContent, char* szYes, char* szNo, int iVoteType);
	void EndVote();
	bool IsVoteEnable();
	void KeyCodeTyped(int code);
private:
	vgui::Label* m_pTitleLable = nullptr;
	vgui::Panel* m_pContentPanel = nullptr;
	vgui::Label* m_pContentLable = nullptr;
	vgui::Label* m_pYesLable = nullptr;
	vgui::Label* m_pNoLable = nullptr;
	vgui::ImagePanel* m_pYesIconImagePanel = nullptr;
	vgui::ImagePanel* m_pNoImagePanel = nullptr;

	enum VoteType {
		NONE = 0,
		KILL = 1,
		KICK = 2,
		BAN = 3,
		MAP = 4,
		SURVIVAL = 5,
		DONKNOW = 6,
		CUSTOM = 7
	};

	cvar_t* m_pHudVote = nullptr;
};
#endif