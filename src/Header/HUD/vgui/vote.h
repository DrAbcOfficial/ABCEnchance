#ifndef CVOTEPANEL_H
#define CVOTEPANEL_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
	class CVotePanel : public EditablePanel, public IViewportPanel{
	public:
		DECLARE_CLASS_SIMPLE(CVotePanel, EditablePanel);

		CVotePanel();
		virtual void ApplySchemeSettings(IScheme* pScheme) override;
		// IViewportPanel overrides
		virtual const char* GetName() override;
		virtual void Reset() override;
		virtual void ShowPanel(bool state) override;
		virtual bool IsVisible() override;
		virtual VPANEL GetVPanel() override;
		virtual void SetParent(VPANEL parent) override;

		void StartVote(char* szContent, char* szYes, char* szNo, int iVoteType);
		void EndVote();
		bool IsVoteEnable();
	bool KeyCodeTyped(int code);

	int m_iYes;
	int m_iNo;
	private:
		Label* m_pTitleLable = nullptr;
		Panel* m_pContentPanel = nullptr;
		Label* m_pContentLable = nullptr;
		Label* m_pYesLable = nullptr;
		Label* m_pNoLable = nullptr;
		ImagePanel* m_pYesIconImagePanel = nullptr;
		ImagePanel* m_pNoImagePanel = nullptr;
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
		cvar_t* m_pHudVoteKeyYes = nullptr;
		cvar_t* m_pHudVoteKeyNo = nullptr;
	};
}
#endif