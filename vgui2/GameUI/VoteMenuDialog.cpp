#include <algorithm>

#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/ListPanel.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/GaussianBlurPanel.h"

#include "VoteMenuDialog.h"

vgui::CBaseVotePage::CBaseVotePage(Panel* parent, const char* name, const char* list_title) : BaseClass(parent, name)
{
	m_pList = new ListPanel(this, "List");
	m_pFilter = new TextEntry(this, "Filter");
	m_pFilterButton = new Button(this, "FilterButton", "#GameUI_ABC_SubmitVoteFilter", this, "Filter");

	m_pSelected = new Label(this, "Selected", "");
	m_pYesButton = new Button(this, "Yes", "#GameUI_ABC_SubmitVoteYes", this, "Yes");
	m_pNoButton = new Button(this, "No", "#GameUI_ABC_SubmitVoteNo", this, "No");

	m_pList->AddActionSignalTarget(this);
	m_pList->SetMultiselectEnabled(false);
	m_pList->AddColumnHeader(0, "name", list_title, 0, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);

	m_pFilterButton->AddActionSignalTarget(this);
	m_pYesButton->AddActionSignalTarget(this);
	m_pNoButton->AddActionSignalTarget(this);
}

void vgui::CBaseVotePage::OnCommand(const char* cmd)
{
	if (!V_strcmp(cmd, "Filter")) {
		m_pList->ClearSelectedItems();
		m_pList->RemoveAll();
		m_pList->ResetScrollBar();
		std::vector<std::string> tempary{};
		char buf[MAX_PATH];
		m_pFilter->GetText(buf, MAX_PATH);
		if (std::strlen(buf) > 0) {
			std::string flt = buf;
			std::transform(flt.begin(), flt.end(), flt.begin(), ::tolower);
			for (auto iter = m_aryList.begin(); iter != m_aryList.end(); iter++) {
				std::string str = *iter;
				std::transform(str.begin(), str.end(), str.begin(), ::tolower);
				if ((flt.size() == 0) || (str.find(flt) != std::string::npos)) {
					KeyValues kv(str.c_str(), "name", (*iter).c_str());
					m_pList->AddItem(&kv, 0, false, false);
				}
			}
		}
		else {
			for (auto iter = m_aryList.begin(); iter != m_aryList.end(); iter++) {
				const char* str = (*iter).c_str();
				KeyValues kv(str, "name", str);
				m_pList->AddItem(&kv, 0, false, false);
			}
		}
	}
	else if (!V_strcmp(cmd, "Yes")) {
		char buf[MAX_PATH];
		m_pFilter->GetText(buf, MAX_PATH);
		SendYesCommand(buf);
	}
	else if (!V_strcmp(cmd, "No")) {
	}
}

void vgui::CBaseVotePage::SendYesCommand(const char* value)
{

}

vgui::CVoteMenuDialog::CVoteMenuDialog(Panel* parent) : BaseClass(parent, "VoteMenuDialog")
{
	SetDeleteSelfOnClose(true);
	SetSizeable(false);
	SetMoveable(false);
	SetProportional(true);
	SetScheme("VoteMenuDialogScheme");
	m_pBlur = new GaussianBlurPanel(this, "BlurPanel");
	LoadControlSettings("abcenchance/res/gameui/VoteMenuDialog.res");

	m_pVoteBan = new CVoteBanPage(this);
	m_pVoteKick = new CVoteKickPage(this);
	m_pVoteKill = new CVoteKillPage(this);
	m_pVoteMap = new CVoteMapPage(this);
	AddPage(m_pVoteBan, "#GameUI_ABC_VoteBanPage");
	AddPage(m_pVoteKick, "#GameUI_ABC_VoteKickPage");
	AddPage(m_pVoteKill, "#GameUI_ABC_VoteKillPage");
	AddPage(m_pVoteMap, "#GameUI_ABC_VoteMapPage");

	EnableApplyButton(true);
}

void vgui::CVoteMenuDialog::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetBgColor(GetSchemeColor("VoteMenuDialog/BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	SetBorder(pScheme->GetBorder("DialogBorder"));

	auto sheet = GetPropertySheet();
	sheet->SetBgColor(GetSchemeColor("VoteMenuDialog/SheetBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	sheet->SetBorder(pScheme->GetBorder("SheetBorder"));

	m_pVoteBan->SetBgColor(GetSchemeColor("VoteMenuDialog/SheetBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pVoteBan->SetBorder(pScheme->GetBorder("SheetBorder"));

	m_pVoteKick->SetBgColor(GetSchemeColor("VoteMenuDialog/SheetBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pVoteKick->SetBorder(pScheme->GetBorder("SheetBorder"));

	m_pVoteKill->SetBgColor(GetSchemeColor("VoteMenuDialog/SheetBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pVoteKill->SetBorder(pScheme->GetBorder("SheetBorder"));

	m_pVoteMap->SetBgColor(GetSchemeColor("VoteMenuDialog/SheetBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pVoteMap->SetBorder(pScheme->GetBorder("SheetBorder"));

	//ofc
	m_pBlur->SetBgColor(Color(255, 255, 255, 255));
}

vgui::CVoteBanPage::CVoteBanPage(Panel* parent) : BaseClass(parent, "VoteBanPage", "#GameUI_ABC_VoteBanPage")
{
}

void vgui::CVoteBanPage::SendYesCommand(const char* value)
{

}

vgui::CVoteKickPage::CVoteKickPage(Panel* parent) : BaseClass(parent, "VoteKickPage", "#GameUI_ABC_VoteKickPage")
{
}

void vgui::CVoteKickPage::SendYesCommand(const char* value)
{
}

vgui::CVoteKillPage::CVoteKillPage(Panel* parent) : BaseClass(parent, "VoteKillPage", "#GameUI_ABC_VoteKillPage")
{
}

void vgui::CVoteKillPage::SendYesCommand(const char* value)
{
}

vgui::CVoteMapPage::CVoteMapPage(Panel* parent) : BaseClass(parent, "VoteMapPage", "#GameUI_ABC_VoteMapPage")
{
}

void vgui::CVoteMapPage::SendYesCommand(const char* value)
{
}

namespace vgui {
	class CViewport;
}
static vgui::DHANDLE<vgui::CVoteMenuDialog>s_hVoteMenuPanel;
void OpenVoteMenuDialog() {
	extern vgui::CViewport* GetBaseViewPort();
	if (s_hVoteMenuPanel == nullptr) {
		s_hVoteMenuPanel = new vgui::CVoteMenuDialog(reinterpret_cast<vgui::Panel*>(GetBaseViewPort()));
	}
	s_hVoteMenuPanel->Activate();
}