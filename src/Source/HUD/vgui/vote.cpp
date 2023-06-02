#pragma once
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>

#include "local.h"
#include "vguilocal.h"

#include "KeyCode.h"
#include "vgui_controls/ImagePanel.h"
#include "vote.h"
#include "Viewport.h"

#include "plugins.h"
#include <parsemsg.h>
#include <regex>

#define VIEWPORT_VOTE_NAME "VotePanel"
#define VOTE_TITLE_LOCALIZE_TOKEN "#Vote_DefaultMessage"
#define VOTE_YES_LOCALIZE_TOKEN "#Vote_DefaultYes"
#define VOTE_NO_LOCALIZE_TOKEN "#Vote_DefaultNo"
#define VOTE_KILL_LOCALIZE_TOKEN "Vote_VoteKill"
#define VOTE_KICK_LOCALIZE_TOKEN "Vote_VoteKick"
#define VOTE_BAN_LOCALIZE_TOKEN "Vote_VoteBan"
#define VOTE_MAP_LOCALIZE_TOKEN "Vote_VoteMap"

pfnUserMsgHook m_pfnVoteMenu;
pfnUserMsgHook m_pfnEndVote;

int CVotePanel::__MsgFunc_VoteMenu(const char* pszName, int iSize, void* pbuf) {
	if(!g_pViewPort->GetVotePanel()->IsVoteEnable())
		return m_pfnVoteMenu(pszName, iSize, pbuf);

	BEGIN_READ(pbuf, iSize);
	float flKeepTime = static_cast<float>(READ_BYTE());
	g_pViewPort->GetVotePanel()->StartVote(READ_STRING(), READ_STRING(), READ_STRING(), flKeepTime);
	return 1;
}
int CVotePanel::__MsgFunc_EndVote(const char* pszName, int iSize, void* pbuf){
	g_pViewPort->GetVotePanel()->EndVote();
	return m_pfnEndVote(pszName, iSize, pbuf);
}

CVotePanel::CVotePanel()
	: BaseClass(nullptr, VIEWPORT_VOTE_NAME){
	SetTitle(VOTE_TITLE_LOCALIZE_TOKEN, true);
	SetCloseButtonVisible(false);
	SetMoveable(false);
	SetSizeable(false);
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "VoteScheme.res", "VoteScheme");
	SetScheme("VoteScheme");

	// Header labels
	m_pContentLable = new vgui::Label(this, "VoteContent", "(None)");
	m_pYesLable = new vgui::Label(this, "VoteYesText", VOTE_YES_LOCALIZE_TOKEN);
	m_pNoLable = new vgui::Label(this, "VoteNoText", VOTE_NO_LOCALIZE_TOKEN);

	m_pYesIconImagePanel = new vgui::ImagePanel(this, "VoteYesIcon");
	m_pNoImagePanel = new vgui::ImagePanel(this, "VoteNoIcon");

	LoadControlSettings(VGUI2_ROOT_DIR "VotePanel.res");
	SetVisible(false);

	m_pHudVote = CREATE_CVAR("cl_hud_vote", "1", FCVAR_VALUE, nullptr);

	m_pfnVoteMenu = HOOK_MESSAGE(VoteMenu);
	m_pfnEndVote = HOOK_MESSAGE(EndVote);
}
const char* CVotePanel::GetName(){
	return VIEWPORT_VOTE_NAME;
}
void CVotePanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
	m_flKeepTime = 0;
}
void CVotePanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;

	if (state)
		Activate();
	else
		m_flKeepTime = 0;
	SetVisible(state);
}
bool CVotePanel::IsVisible(){
	return BaseClass::IsVisible();
}
void CVotePanel::OnKeyCodeTyped(vgui::KeyCode code){
	if (IsVisible()) {
		switch (code){
			case vgui::KEY_F1: {
				ServerCmd("voteyes");
				break;
			}
			case vgui::KEY_F2: {
				ServerCmd("voteno");
				break;
			}
			default:break;
		}
	}
}
void CVotePanel::StartVote(char* szContent, char* szYes, char* szNo, float flKeepTime){
	auto VotePatternCheck = [&](std::string& content, const char* szpattern, const char* localtoken) {
		std::regex pattern(szpattern);
		std::smatch result;
		if (std::regex_match(content, result, pattern) && result.size() >= 2) {
			std::string playername = result[1];
			wchar_t wPlayerName[32] = { 0 };
			wchar_t szContent[256] = { 0 };
			vgui::localize()->ConvertANSIToUnicode(playername.c_str(), wPlayerName, sizeof(wPlayerName));
			vgui::localize()->ConstructString(szContent, sizeof(szContent), vgui::localize()->Find(localtoken), 1, wPlayerName);
			m_pContentLable->SetText(szContent);
			m_pYesLable->SetText(VOTE_YES_LOCALIZE_TOKEN);
			m_pNoLable->SetText(VOTE_NO_LOCALIZE_TOKEN);
			return true;
		}
		return false;
	};
	const char* patterns[] = {
		"Would you like to kill \"(.*)\"\\?",
		"Would you like to kick \"(.*)\" from the server\\?",
		"Do you wish to ban \"(.*)\" from the server\\?",
		"Would you like to change the map to \"(.*)\"\\?"
	};
	const char* tokens[] = {
		VOTE_KILL_LOCALIZE_TOKEN,
		VOTE_KICK_LOCALIZE_TOKEN,
		VOTE_BAN_LOCALIZE_TOKEN,
		VOTE_MAP_LOCALIZE_TOKEN
	};
	m_flKeepTime = gEngfuncs.GetClientTime() + flKeepTime;
	std::string stdContent = szContent;
	for (size_t i = 0; i < 4; i++) {
		if (VotePatternCheck(stdContent, patterns[i], tokens[i])) {
			SetKeyBoardInputEnabled(true);
			ShowPanel(true);
			return;
		}
	}
	m_pContentLable->SetText(szContent);
	m_pYesLable->SetText(szYes);
	m_pNoLable->SetText(szNo);
	SetKeyBoardInputEnabled(true);
	ShowPanel(true);
}
void CVotePanel::EndVote(){
	SetKeyBoardInputEnabled(false);
	ShowPanel(false);
}
void CVotePanel::Think(){
	if (IsVisible() && gEngfuncs.GetClientTime() >= m_flKeepTime)
		EndVote();
}
bool CVotePanel::IsVoteEnable(){
	return m_pHudVote->value > 0;
}