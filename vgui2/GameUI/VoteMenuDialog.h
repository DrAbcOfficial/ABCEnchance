#pragma once
#include <vector>

#include "vgui_controls/PropertyPage.h"
#include "vgui_controls/PropertyDialog.h"

namespace vgui {
	class ListPanel;
	class Button;
	class TextEntry;
	class Label;
	class GaussianBlurPanel;

	class CBaseVotePage : public PropertyPage {
		DECLARE_CLASS_SIMPLE(CBaseVotePage, PropertyPage);
	public:
		CBaseVotePage(Panel* parent, const char* name, const char* list_title);
	protected:
		virtual void OnCommand(const char* cmd) override;
		virtual void SendYesCommand(const char* value);

		ListPanel* m_pList = nullptr;
		TextEntry* m_pFilter = nullptr;
		Button* m_pFilterButton = nullptr;
		
		Label* m_pSelected = nullptr;
		Button* m_pYesButton = nullptr;
		Button* m_pNoButton = nullptr;

		std::vector<std::string> m_aryList;
	};

	class CVoteBanPage : public CBaseVotePage {
		DECLARE_CLASS_SIMPLE(CVoteBanPage, CBaseVotePage);

		public:
			CVoteBanPage(Panel* parent);
		protected:
			virtual void SendYesCommand(const char* value) override;
	};

	class CVoteKickPage : public CBaseVotePage {
		DECLARE_CLASS_SIMPLE(CVoteKickPage, CBaseVotePage);

	public:
		CVoteKickPage(Panel* parent);
	protected:
		virtual void SendYesCommand(const char* value) override;
	};

	class CVoteKillPage : public CBaseVotePage {
		DECLARE_CLASS_SIMPLE(CVoteKillPage, CBaseVotePage);

	public:
		CVoteKillPage(Panel* parent);
	protected:
		virtual void SendYesCommand(const char* value) override;
	};

	class CVoteMapPage : public CBaseVotePage {
		DECLARE_CLASS_SIMPLE(CVoteMapPage, CBaseVotePage);

	public:
		CVoteMapPage(Panel* parent);
	protected:
		virtual void SendYesCommand(const char* value) override;
	};

	class CVoteMenuDialog : public PropertyDialog {
		DECLARE_CLASS_SIMPLE(CVoteMenuDialog, PropertyDialog);
	public:
		CVoteMenuDialog(Panel* parent);
	protected:
		virtual void ApplySchemeSettings(IScheme* pScheme) override;
	private:

		GaussianBlurPanel* m_pBlur;
		CVoteBanPage* m_pVoteBan;
		CVoteKickPage* m_pVoteKick;
		CVoteKillPage* m_pVoteKill;
		CVoteMapPage* m_pVoteMap;
	};
}