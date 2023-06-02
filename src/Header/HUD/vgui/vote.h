#ifndef CPLAYERINFOPANEL_H
#define CPLAYERINFOPANEL_H
#include <string>

#include <vgui_controls/Frame.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
	enum KeyCode;
}

class CVotePanel : public vgui::Frame, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CVotePanel, vgui::Frame);

	CVotePanel();
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual void OnKeyCodeTyped(vgui::KeyCode code) override;

	static int __MsgFunc_VoteMenu(const char* pszName, int iSize, void* pbuf);
	static int __MsgFunc_EndVote(const char* pszName, int iSize, void* pbuf);
	void StartVote(char* szContent, char* szYes, char* szNo, float flKeepTime);
	void EndVote();

	void Think();
	bool IsVoteEnable();
private:
	vgui::Label* m_pContentLable = nullptr;
	vgui::Label* m_pYesLable = nullptr;
	vgui::Label* m_pNoLable = nullptr;
	vgui::ImagePanel* m_pYesIconImagePanel = nullptr;
	vgui::ImagePanel* m_pNoImagePanel = nullptr;

	cvar_t* m_pHudVote = nullptr;

	float m_flKeepTime;
};

#endif