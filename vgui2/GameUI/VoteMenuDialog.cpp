#include <algorithm>

#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/ListPanel.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/GaussianBlurPanel.h"
#include "vgui_controls/avatar_image.h"
#include "vgui_controls/ImageList.h"

#include "player_info.h"

#include "VoteMenuDialog.h"

extern void ServerCmd(const char* x);
static vgui::DHANDLE<vgui::CVoteMenuDialog>s_hVoteMenuPanel;
vgui::ImageList* s_aryAvatars;

vgui::CBaseVotePage::CBaseVotePage(Panel* parent, const char* name) : BaseClass(parent, name)
{
	m_pList = new ListPanel(this, "List");
	m_pFilter = new TextEntry(this, "Filter");
	m_pFilterButton = new Button(this, "FilterButton", "#GameUI_ABC_SubmitVoteFilter", this, "Filter");

	m_pSelected = new Label(this, "Selected", "");
	m_pYesButton = new Button(this, "Yes", "#GameUI_ABC_SubmitVoteYes", this, "Yes");
	m_pNoButton = new Button(this, "No", "#GameUI_ABC_SubmitVoteNo", this, "No");

	m_pList->AddActionSignalTarget(this);
	m_pList->SetMultiselectEnabled(false);

	m_pFilterButton->AddActionSignalTarget(this);
	m_pYesButton->AddActionSignalTarget(this);
	m_pNoButton->AddActionSignalTarget(this);
}

void vgui::CBaseVotePage::OnPageShow()
{
	m_pSelected->SetText("");
	m_pFilter->SetText("");
	ResetList();
}

void vgui::CBaseVotePage::OnCommand(const char* cmd)
{
	if (!V_strcmp(cmd, "Filter")) {
		ResetList();
	}
	else if (!V_strcmp(cmd, "Yes")) {
		int itemID = m_pList->GetSelectedItem(0);
		if (itemID < 0)
			return;
		const char* buf = m_pList->GetItemData(itemID)->kv->GetString("name");
		if (V_strlen(buf) <= 0)
			return;
		SendYesCommand(buf);
	}
	else if (!V_strcmp(cmd, "No")) {
		CloseParent();
	}
}

void vgui::CBaseVotePage::SendYesCommand(const char* value){ }

void vgui::CBaseVotePage::ResetList()
{
}

void vgui::CBaseVotePage::CloseParent()
{
	if (s_hVoteMenuPanel == nullptr)
		return;
	s_hVoteMenuPanel->Close();
}

void vgui::CBaseVotePage::OnItemSelected() {
	int id = m_pList->GetSelectedItem(0);
	if (id < 0)
		return;
	const char* name = m_pList->GetItemData(id)->kv->GetString("name");
	m_pSelected->SetText(name);
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

	EnableApplyButton(false);
	SetCancelButtonVisible(false);
	SetOKButtonVisible(false);
	SetMaximizeButtonVisible(false);

	s_aryAvatars = new vgui::ImageList(true);
}

void vgui::CVoteMenuDialog::OnTick()
{
	BaseClass::OnTick();
	SetMouseInputEnabled(true);
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

void vgui::CVoteMenuDialog::Activate()
{
	BaseClass::Activate();
	vgui::ivgui()->AddTickSignal(this->GetVPanel());
}

vgui::CVoteBanPage::CVoteBanPage(Panel* parent) : BaseClass(parent, "VoteBanPage")
{
	LoadControlSettings("abcenchance/res/gameui/VoteBanPage.res");
	m_pList->AddColumnHeader(0, "avatar", "", 32, ListPanel::ColumnFlags_e::COLUMN_FIXEDSIZE | ListPanel::ColumnFlags_e::COLUMN_IMAGE);
	m_pList->AddColumnHeader(1, "index", "#GameUI_ABC_VotePageIndex", 32, ListPanel::ColumnFlags_e::COLUMN_FIXEDSIZE);
	m_pList->AddColumnHeader(2, "name", "#GameUI_ABC_VotePageName", 0, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->AddColumnHeader(3, "real_name", "#GameUI_ABC_VotePageRealName", 128, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->AddColumnHeader(4, "steamid", "#GameUI_ABC_VotePageSteamID", 256, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->SetImageList(s_aryAvatars, false);
}

void vgui::CVoteBanPage::SendYesCommand(const char* value)
{
	char buf[MAX_PATH];
	V_snprintf(buf, MAX_PATH, "voteban %s", value);
	ServerCmd(buf);
	CloseParent();
}

void vgui::CVoteBanPage::ResetList()
{
	m_pList->ClearSelectedItems();
	m_pList->RemoveAll();
	m_pList->ResetScrollBar();
	
	auto& infos = CPlayerInfo::GetPlayerInfos();
	char buf[MAX_PATH];
	m_pFilter->GetText(buf, MAX_PATH);
	for (auto iter = infos.begin(); iter != infos.end(); iter++) {
		auto p = *iter;
		if (!p.IsValid())
			continue;

		//update or create player avatar
		if (s_aryAvatars->IsValidIndex(p.GetIndex())) {
			CAvatarImage* img = reinterpret_cast<CAvatarImage*>(s_aryAvatars->GetImage(p.GetIndex()));
			img->SetAvatarSteamID(*p.GetSteamID());
		}
		else {
			CAvatarImage* img = new CAvatarImage();
			img->SetAvatarSteamID(*p.GetSteamID());
			s_aryAvatars->SetImageAtIndex(p.GetIndex(), img);
		}
		m_pList->SetImageList(s_aryAvatars, false);

		if (std::strlen(buf) > 0) {
			std::string flt = buf;
			std::transform(flt.begin(), flt.end(), flt.begin(), ::tolower);
			std::string game_name = p.GetName();
			std::transform(game_name.begin(), game_name.end(), game_name.begin(), ::tolower);
			if ((flt.size() == 0) || (game_name.find(flt) != std::string::npos)) {
				
				KeyValues kv(p.GetSteamIDString64());
				kv.SetInt("avatar", p.GetIndex());
				kv.SetInt("index", p.GetIndex());
				kv.SetString("name", p.GetName());
				kv.SetString("real_name", p.GetRealName());
				kv.SetString("steamid", p.GetSteamIDString64());
				m_pList->AddItem(&kv, 0, false, false);
			}
		}
		else {
			KeyValues kv(p.GetSteamIDString64());
			kv.SetInt("avatar", p.GetIndex());
			kv.SetInt("index", p.GetIndex());
			kv.SetString("name", p.GetName());
			kv.SetString("real_name", p.GetRealName());
			kv.SetString("steamid", p.GetSteamIDString64());
			m_pList->AddItem(&kv, 0, false, false);
		}
	}

	m_pList->SortList();
}

vgui::CVoteKickPage::CVoteKickPage(Panel* parent) : BaseClass(parent, "VoteKickPage")
{
	LoadControlSettings("abcenchance/res/gameui/VoteKickPage.res");
	m_pList->AddColumnHeader(0, "avatar", "", 32, ListPanel::ColumnFlags_e::COLUMN_FIXEDSIZE | ListPanel::ColumnFlags_e::COLUMN_IMAGE);
	m_pList->AddColumnHeader(1, "index", "#GameUI_ABC_VotePageIndex", 32, ListPanel::ColumnFlags_e::COLUMN_FIXEDSIZE);
	m_pList->AddColumnHeader(2, "name", "#GameUI_ABC_VotePageName", 0, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->AddColumnHeader(3, "real_name", "#GameUI_ABC_VotePageRealName", 128, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->AddColumnHeader(4, "steamid", "#GameUI_ABC_VotePageSteamID", 256, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->SetImageList(s_aryAvatars, false);
}

void vgui::CVoteKickPage::SendYesCommand(const char* value)
{
	char buf[MAX_PATH];
	V_snprintf(buf, MAX_PATH, "votekick %s", value);
	ServerCmd(buf);
	CloseParent();
}

void vgui::CVoteKickPage::ResetList()
{
	m_pList->ClearSelectedItems();
	m_pList->RemoveAll();
	m_pList->ResetScrollBar();

	auto& infos = CPlayerInfo::GetPlayerInfos();
	char buf[MAX_PATH];
	m_pFilter->GetText(buf, MAX_PATH);
	for (auto iter = infos.begin(); iter != infos.end(); iter++) {
		auto p = *iter;
		if (!p.IsValid())
			continue;
		//update or create player avatar
		if (s_aryAvatars->IsValidIndex(p.GetIndex())) {
			CAvatarImage* img = reinterpret_cast<CAvatarImage*>(s_aryAvatars->GetImage(p.GetIndex()));
			img->SetAvatarSteamID(*p.GetSteamID());
		}
		else {
			CAvatarImage* img = new CAvatarImage();
			img->SetAvatarSteamID(*p.GetSteamID());
			s_aryAvatars->SetImageAtIndex(p.GetIndex(), img);
		}
		m_pList->SetImageList(s_aryAvatars, false);
		if (std::strlen(buf) > 0) {
			std::string flt = buf;
			std::transform(flt.begin(), flt.end(), flt.begin(), ::tolower);
			std::string game_name = p.GetName();
			std::transform(game_name.begin(), game_name.end(), game_name.begin(), ::tolower);
			if ((flt.size() == 0) || (game_name.find(flt) != std::string::npos)) {
				KeyValues kv(p.GetSteamIDString64());
				kv.SetInt("avatar", p.GetIndex());
				kv.SetInt("index", p.GetIndex());
				kv.SetString("name", p.GetName());
				kv.SetString("real_name", p.GetRealName());
				kv.SetString("steamid", p.GetSteamIDString64());
				m_pList->AddItem(&kv, 0, false, false);
			}
		}
		else {
			KeyValues kv(p.GetSteamIDString64());
			kv.SetInt("avatar", p.GetIndex());
			kv.SetInt("index", p.GetIndex());
			kv.SetString("name", p.GetName());
			kv.SetString("real_name", p.GetRealName());
			kv.SetString("steamid", p.GetSteamIDString64());
			m_pList->AddItem(&kv, 0, false, false);
		}
	}

	m_pList->SortList();
}

vgui::CVoteKillPage::CVoteKillPage(Panel* parent) : BaseClass(parent, "VoteKillPage")
{
	LoadControlSettings("abcenchance/res/gameui/VoteKillPage.res");
	m_pList->AddColumnHeader(0, "avatar", "", 32, ListPanel::ColumnFlags_e::COLUMN_FIXEDSIZE | ListPanel::ColumnFlags_e::COLUMN_IMAGE);
	m_pList->AddColumnHeader(1, "index", "#GameUI_ABC_VotePageIndex", 32, ListPanel::ColumnFlags_e::COLUMN_FIXEDSIZE);
	m_pList->AddColumnHeader(2, "name", "#GameUI_ABC_VotePageName", 0, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->AddColumnHeader(3, "real_name", "#GameUI_ABC_VotePageRealName", 128, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->AddColumnHeader(4, "steamid", "#GameUI_ABC_VotePageSteamID", 256, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pList->SetImageList(s_aryAvatars, false);
}

void vgui::CVoteKillPage::SendYesCommand(const char* value)
{
	char buf[MAX_PATH];
	V_snprintf(buf, MAX_PATH, "votekill %s", value);
	ServerCmd(buf);
	CloseParent();
}

void vgui::CVoteKillPage::ResetList()
{
	m_pList->ClearSelectedItems();
	m_pList->RemoveAll();
	m_pList->ResetScrollBar();

	auto& infos = CPlayerInfo::GetPlayerInfos();
	char buf[MAX_PATH];
	m_pFilter->GetText(buf, MAX_PATH);
	for (auto iter = infos.begin(); iter != infos.end(); iter++) {
		auto p = *iter;
		if (!p.IsValid())
			continue;
		//update or create player avatar
		if (s_aryAvatars->IsValidIndex(p.GetIndex())) {
			CAvatarImage* img = reinterpret_cast<CAvatarImage*>(s_aryAvatars->GetImage(p.GetIndex()));
			img->SetAvatarSteamID(*p.GetSteamID());
		}
		else {
			CAvatarImage* img = new CAvatarImage();
			img->SetAvatarSteamID(*p.GetSteamID());
			s_aryAvatars->SetImageAtIndex(p.GetIndex(), img);
		}
		m_pList->SetImageList(s_aryAvatars, false);
		if (std::strlen(buf) > 0) {
			std::string flt = buf;
			std::transform(flt.begin(), flt.end(), flt.begin(), ::tolower);
			std::string game_name = p.GetName();
			std::transform(game_name.begin(), game_name.end(), game_name.begin(), ::tolower);
			if ((flt.size() == 0) || (game_name.find(flt) != std::string::npos)) {
				KeyValues kv(p.GetSteamIDString64());
				kv.SetInt("avatar", p.GetIndex());
				kv.SetInt("index", p.GetIndex());
				kv.SetString("name", p.GetName());
				kv.SetString("real_name", p.GetRealName());
				kv.SetString("steamid", p.GetSteamIDString64());
				m_pList->AddItem(&kv, 0, false, false);
			}
		}
		else {
			KeyValues kv(p.GetSteamIDString64());
			kv.SetInt("avatar", p.GetIndex());
			kv.SetInt("index", p.GetIndex());
			kv.SetString("name", p.GetName());
			kv.SetString("real_name", p.GetRealName());
			kv.SetString("steamid", p.GetSteamIDString64());
			m_pList->AddItem(&kv, 0, false, false);
		}
	}

	m_pList->SortList();
}

vgui::CVoteMapPage::CVoteMapPage(Panel* parent) : BaseClass(parent, "VoteMapPage")
{
	LoadControlSettings("abcenchance/res/gameui/VoteMapPage.res");
	m_pList->AddColumnHeader(0, "name", "#GameUI_ABC_VotePageName", 0, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
}

void vgui::CVoteMapPage::SendYesCommand(const char* value)
{
	char buf[MAX_PATH];
	V_snprintf(buf, MAX_PATH, "votemap %s", value);
	ServerCmd(buf);
	CloseParent();
}

void vgui::CVoteMapPage::ResetList()
{
	m_pList->ClearSelectedItems();
	m_pList->RemoveAll();
	m_pList->ResetScrollBar();
	extern std::vector<std::string>& GetGameMapList();
	auto& maplist = GetGameMapList();
	for (auto iter = maplist.begin(); iter != maplist.end(); iter++) {
		std::string& m = *iter;
		KeyValues kv(m.c_str(), "name", m.c_str());
		m_pList->AddItem(&kv, 0, false, false);
	}
	m_pList->SortList();
}

namespace vgui {
	class CViewport;
}

void OpenVoteMenuDialog() {
	extern vgui::CViewport* GetBaseViewPort();
	vgui::CViewport* pViewPort = GetBaseViewPort();
	if (pViewPort == nullptr)
		return;
	if (s_hVoteMenuPanel == nullptr) {
		s_hVoteMenuPanel = new vgui::CVoteMenuDialog(reinterpret_cast<vgui::Panel*>(pViewPort));
	}
	s_hVoteMenuPanel->Activate();
}