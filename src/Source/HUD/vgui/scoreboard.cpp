#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/IInput.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/SectionedListPanel.h>
#include <vgui_controls/ImageList.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/MemoryBitmap.h>

#include "client_steam_context.h"
#include "local.h"
#include "vguilocal.h"

#include "core/resource/playerresource.h"

#include "Task.h"
#include "httpclient.h"

#include <FreeImage.h>

#include "CCustomHud.h"

#include "scoreboard.h"
#include "avatar_image.h"

//VoiceShit
#include "hud.h"
#include "voice_status.h"

#include "Viewport.h"

#include "plugins.h"
#include <mymathlib.h>

#define STEAM_PROFILE_URL "http://steamcommunity.com/profiles/"

#define NAME_LOCALIZE_TOKEN  "#Scores_PlayerName"
#define STEAMID_LOCALIZE_TOKEN "#Scores_SteamID"
#define HEALTH_LOCALIZE_TOKEN "#Scores_PlayerHealth"
#define HEALTHDEAD_LOCALIZE_TOKEN "#Scores_PlayerHealthDead"
#define ARMOR_LOCALIZE_TOKEN "#Scores_PlayerArmor"
#define SCORE_LOCALIZE_TOKEN "#Scores_PlayerScore"
#define DEATH_LOCALIZE_TOKEN "#Scores_PlayerDeath"
#define PING_LOCALIZE_TOKEN "#Scores_Ping"
#define PINGLOSS_LOCALIZE_TOKEN "#Scores_lPingLoss"
#define SPECTATOR_LOCALIZE_TOKEN "#Scores_PlayerSpec"
#define NA_LOCALIZE_TOKEN "#Scores_NA"
#define NEXTMAP_LOCALIZE_TOKEN "#Scores_NextMap"

#define FRAG_KEY "score_frags"
#define DEATH_KEY "score_dead"
#define HEALTH_KEY "score_health"
#define ARMOR_KEY "score_armor"
#define NAME_KEY "score_name"
#define STEAMID_KEY "score_steamid"
#define PING_KEY "score_ping"
#define AVATAR_KEY "score_avatar"
#define DONOR_KEY "score_donor"

#define DONOR_IMAGELIST_BASE 40

#define VIEWPORT_SCOREPANEL_NAME "ScorePanel"

using namespace vgui;

extern vgui::HScheme GetViewPortBaseScheme();

static int s_iMutedIconTexture = -1;
static int s_iFriendTexture = -1;
static int s_iDefaultAvatarTexture = -1;
class CPlayerImage : public IImage
{
public:
	virtual ~CPlayerImage() {
		ClearFrame();
	}
	void Reset() {
		if (m_pRequest.has_value() && m_pRequest.value() != nullptr)
			GetHttpClient()->Interrupt(m_pRequest.value());
		m_pRequest.reset();
		m_bRequsetedAnimatedAvatars = false;
		m_bDrawFriend = false;
		ClearFrame();
	}
	void ClearFrame() {
		for (auto iter = m_aryAnimatedAvatars.begin(); iter != m_aryAnimatedAvatars.end(); iter++) {
			(*iter)->Destroy();
		}
		m_aryAnimatedAvatars.clear();
	}
	void SetFrameTime(float time) {
		m_flAnimateTime = time;
	}
	void AddFrame(byte* bgra, int w, int h) {
		if (m_bIsAnimate) {
			MemoryBitmap* frame = new MemoryBitmap(bgra, w, h);
			m_aryAnimatedAvatars.push_back(frame);
		}
	}
	void ResetAvatarInfo() {
		if (m_bIsAnimate) {
			for (auto iter = m_aryAnimatedAvatars.begin(); iter != m_aryAnimatedAvatars.end(); iter++) {
				(*iter)->SetPos(m_iX, m_iY);
				(*iter)->SetSize(m_iWide, m_iTall);
				(*iter)->SetColor(m_DrawColor);
			}
		}
		else {
			m_pAvatar.SetPos(m_iX, m_iY);
			m_pAvatar.SetOffset(m_iOffX, m_iOffY);
			m_pAvatar.SetSize(m_iWide, m_iTall);
			m_pAvatar.SetColor(m_DrawColor);
		}
	}
	void SetAdminTexture(int tex) {
		iAdminTexture = tex;
	}

	void RequestAnimateAvatarCallback(EquippedProfileItems_t* arg, bool s) {
		if (arg->m_eResult == EResult::k_EResultOK && arg->m_bHasAnimatedAvatar) {
			const char* url = SteamFriends()->GetProfileItemPropertyString(arg->m_steamID, k_ECommunityProfileItemType_AnimatedAvatar, k_ECommunityProfileItemProperty_ImageSmall);
			if (url && V_strlen(url) > 0) {
				if (!m_pRequest.has_value()) {
					ClearFrame();
					m_pRequest = GetHttpClient()->Fetch(url, UtilHTTPMethod::Get)->OnRespond([](IUtilHTTPResponse* rep, CPlayerImage* pthis) {
						using gif = struct {
							byte* bgra;
							size_t w;
							size_t h;
							size_t frametime;
						};
						GetTaskManager()->Add<std::shared_ptr<std::vector<gif*>>>([](const char* data, size_t length) {
							std::shared_ptr<std::vector<gif*>> pgif = std::make_shared<std::vector<gif*>>();
							FIMEMORY* mem = FreeImage_OpenMemory(reinterpret_cast<BYTE*>(const_cast<char*>(data)), length);
							FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(mem);
							if (format == FIF_GIF) {
								FIMULTIBITMAP* multiBitmap = FreeImage_LoadMultiBitmapFromMemory(format, mem, GIF_PLAYBACK);
								size_t pageCount = FreeImage_GetPageCount(multiBitmap);
								for (size_t i = 0; i < pageCount; i++) {
									FIBITMAP* dib = FreeImage_LockPage(multiBitmap, i);
									if (dib) {
										BYTE* bits = FreeImage_GetBits(dib);
										size_t width = FreeImage_GetWidth(dib);
										size_t height = FreeImage_GetHeight(dib);
										FITAG* tag = nullptr;

										gif* image = new gif();
										image->bgra = new byte[width * height * 4];

										byte* head = image->bgra;
										// 遍历每个像素
										for (int y = height - 1; y >= 0; y--) {
											for (size_t x = 0; x < width; x++) {
												RGBQUAD color;
												FreeImage_GetPixelColor(dib, x, y, &color);
												head[0] = color.rgbRed;
												head[1] = color.rgbGreen;
												head[2] = color.rgbBlue;
												head[3] = color.rgbReserved;
												head += 4;
											}
										}
										image->w = width;
										image->h = height;
										if (FreeImage_GetMetadata(FIMD_ANIMATION, dib, "FrameTime", &tag))
											image->frametime = *(unsigned int*)FreeImage_GetTagValue(tag);
										pgif->push_back(image);
										FreeImage_UnlockPage(multiBitmap, dib, false);
									}
								}
								FreeImage_CloseMultiBitmap(multiBitmap);
							}
							FreeImage_CloseMemory(mem);
							return pgif;
						}, rep->GetPayload()->GetBytes(), rep->GetPayload()->GetLength())->ContinueWith([](std::shared_ptr<std::vector<gif*>> gifdata, CPlayerImage* pthis) {
							for (size_t i = 0; i < gifdata->size(); i++) {
								gif* image = gifdata->at(i);
								pthis->SetFrameTime(image->frametime);
								pthis->AddFrame(image->bgra, image->w, image->h);
								delete[] image->bgra;
								delete image;
							}
							gifdata.reset();
						}, pthis)->Start();
							pthis->m_pRequest = nullptr;
						}, this)->Start();
					m_bIsAnimate = true;
				}
				m_bRequsetedAnimatedAvatars = true;
			}
		}
	}

	CCallResult< CPlayerImage, EquippedProfileItems_t > m_RTestCallResult;
	void SetAvatar(CSteamID* pSteamID)
	{
		if (!pSteamID)
			return;

		if (!m_bRequsetedAnimatedAvatars) 
			m_RTestCallResult.Set(SteamFriends()->RequestEquippedProfileItems(*pSteamID), this, &CPlayerImage::RequestAnimateAvatarCallback);
		m_pAvatar.SetAvatarSteamID(*pSteamID);
		SetFriend(SteamFriends()->GetFriendRelationship(*pSteamID) == k_EFriendRelationshipFriend);
		ResetAvatarInfo();
	}

	void SetMuted(bool state)
	{
		m_bIsMuted = state;
	}

	void SetFriend(bool state) {
		m_bDrawFriend = state;
	}

	void OnTick()
	{
		if (m_bIsAnimate && m_aryAnimatedAvatars.size() > 0 && system()->GetTimeMillis() >= m_flNextAnimateTime)
		{
			m_flNextAnimateTime = system()->GetTimeMillis() + m_flAnimateTime;
			m_iCurrentImage++;
			if (m_iCurrentImage >= m_aryAnimatedAvatars.size()) {
				m_iCurrentImage = 0;
			}
		}
	}

	// Call to Paint the image
	// Image will draw within the current panel context at the specified position
	virtual void Paint() override
	{
		if (m_aryAnimatedAvatars.size() > 0)
			m_aryAnimatedAvatars[m_iCurrentImage]->Paint();
		else {
			if (m_pAvatar.IsValid())
				m_pAvatar.Paint();
			else if (s_iDefaultAvatarTexture >= 0) {
				surface()->DrawSetTexture(s_iDefaultAvatarTexture);
				surface()->DrawSetColor(m_DrawColor);
				surface()->DrawTexturedRect(m_iX + m_iOffX, m_iY + m_iOffY,
					m_iX + m_iOffX + m_iWide, m_iY + m_iOffY + m_iTall);
			}
		}

		int x0 = m_iX + m_iOffX;
		int y0 = m_iY + m_iOffY;
		int x1 = m_iX + m_iOffX + m_iWide;
		int y1 = m_iY + m_iOffY + m_iTall;

		if (m_bIsMuted && s_iMutedIconTexture >= 0){
			surface()->DrawSetTexture(s_iMutedIconTexture);
			surface()->DrawSetColor(m_DrawColor);
			surface()->DrawTexturedRect(x0, y0, x1, y1);
		}

		if (iAdminTexture >= 0) {
			surface()->DrawSetTexture(iAdminTexture);
			surface()->DrawSetColor(m_DrawColor);
			surface()->DrawTexturedRect(x0, y0, x1, y1);
		}

		if (m_bDrawFriend) {
			surface()->DrawSetTexture(s_iFriendTexture);
			surface()->DrawSetColor(m_DrawColor);
			surface()->DrawTexturedRect(x0 - m_iWide / 4, y0, x0, y1);
		}
	}

	// Set the position of the image
	virtual void SetPos(int x, int y) override
	{
		m_iX = x;
		m_iY = y;
		ResetAvatarInfo();
	}

	virtual void SetOffset(int x, int y)
	{
		m_iOffX = x;
		m_iOffY = y;
		ResetAvatarInfo();
	}

	// Gets the size of the content
	virtual void GetContentSize(int& wide, int& tall) override
	{
		wide = m_iWide;
		tall = m_iTall;
	}

	// Get the size the image will actually draw in (usually defaults to the content size)
	virtual void GetSize(int& wide, int& tall) override
	{
		GetContentSize(wide, tall);
	}

	// Sets the size of the image
	virtual void SetSize(int wide, int tall) override
	{
		m_iWide = wide;
		m_iTall = tall;
		ResetAvatarInfo();
	}

	// Set the draw color
	virtual void SetColor(Color col) override
	{
		m_DrawColor = col;
		ResetAvatarInfo();
	}

private:
	int m_iX = 0, m_iY = 0;
	int m_iOffX = 0, m_iOffY = 0;
	int m_iWide = 0, m_iTall = 0;
	Color m_DrawColor = Color(255, 255, 255, 255);

	bool m_bDrawFriend;

	bool m_bIsAnimate = false;
	bool m_bRequsetedAnimatedAvatars = false;
	std::optional<CHttpClientItem*> m_pRequest = nullptr;

	std::vector<IImage_HL25*> m_aryAnimatedAvatars;
	size_t m_iCurrentImage;
	float m_flAnimateTime;
	float m_flNextAnimateTime;

	CAvatarImage m_pAvatar;


	bool m_bIsMuted = false;
	int iAdminTexture = 0;
};
class CDonorImage : public IImage {
public:
	void SetTexture(int level) {
		m_iLevelTexture = level;
	}
	virtual void Paint() override {
		if (m_iLevelTexture < 0)
			return;
		surface()->DrawSetTexture(m_iLevelTexture);
		surface()->DrawSetColor(m_DrawColor);
		surface()->DrawTexturedRect(m_iX + m_iOffX, m_iY + m_iOffY,
			m_iX + m_iOffX + m_iWide, m_iY + m_iOffY + m_iTall);
	}
	// Get the size the image will actually draw in (usually defaults to the content size)
	virtual void GetSize(int& wide, int& tall) override {
		GetContentSize(wide, tall);
	}

	// Sets the size of the image
	virtual void SetSize(int wide, int tall) override {
		m_iWide = wide;
		m_iTall = tall;
	}
	// Set the position of the image
	virtual void SetPos(int x, int y) override {
		m_iX = x;
		m_iY = y;
	}
	virtual void SetOffset(int x, int y) {
		m_iOffX = x;
		m_iOffY = y;
	}
	// Gets the size of the content
	virtual void GetContentSize(int& wide, int& tall) override {
		wide = m_iWide;
		tall = m_iTall;
	}
	// Set the draw color
	virtual void SetColor(Color col) override {
		m_DrawColor = col;
	}
private:
	int m_iLevelTexture = 0;
	int m_iX = 0, m_iY = 0;
	int m_iOffX = 0, m_iOffY = 0;
	int m_iWide = 0, m_iTall = 0;
	Color m_DrawColor = Color(255, 255, 255, 255);
};


CScorePanel::CScorePanel()
	: BaseClass(nullptr, VIEWPORT_SCOREPANEL_NAME)
{
	SetTitle("", true);
	SetCloseButtonVisible(false);
	SetMoveable(false);
	SetSizeable(false);
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());

	hud_scoreboard_showavatars = CREATE_CVAR("hud_scoreboard_showavatars", "1", FCVAR_ARCHIVE, nullptr);
	hud_scoreboard_showloss = CREATE_CVAR("hud_scoreboard_showloss", "1", FCVAR_ARCHIVE, nullptr);
	hud_scoreboard_showsteamid = CREATE_CVAR("hud_scoreboard_showsteamid", "1", FCVAR_ARCHIVE, nullptr);
	hud_scoreboard_showsteamidtype = CREATE_CVAR("hud_scoreboard_showsteamidtype", "1", FCVAR_ARCHIVE, nullptr);
	hud_scoreboard_showrealname = CREATE_CVAR("hud_scoreboard_showrealname", "1", FCVAR_ARCHIVE, nullptr);

	hud_scoreboard_size = CREATE_CVAR("hud_scoreboard_size", "0", FCVAR_ARCHIVE, nullptr);
	hud_scoreboard_spacing_normal = CREATE_CVAR("hud_scoreboard_spacing_normal", "0", FCVAR_ARCHIVE, nullptr);
	hud_scoreboard_spacing_compact = CREATE_CVAR("hud_scoreboard_spacing_compact", "0", FCVAR_ARCHIVE, nullptr);

	// Header labels
	m_pServerNameLabel = new Label(this, "ServerName", "Sven Co-op Server");
	m_pMapNameLabel = new Label(this, "MapName", "");
	m_pPlayerCountLabel = new Label(this, "PlayerCount", "");
	m_pNextMapLable = new Label(this, "NextMap", "");
	m_pTimeEndLable = new Label(this, "TimeEnd", "");

	// Player list
	m_pPlayerList = new SectionedListPanel(this, "PlayerList");
	m_pPlayerList->SetMouseInputEnabled(true);
	m_pPlayerList->SetVerticalScrollbar(false);
	wchar_t* specTag = localize()->Find(SPECTATOR_LOCALIZE_TOKEN);
	if (specTag)
		Q_UnicodeToUTF8(specTag, m_szSpectatorTag, sizeof(m_szSpectatorTag));

	// Image list
	m_pImageList = new ImageList(true);
	m_pPlayerList->SetImageList(m_pImageList, true);

	for (int i = 1; i <= SC_MAX_PLAYERS; i++)
	{
		CPlayerImage* pImg = new CPlayerImage();
		pImg->SetSize(32, 32);
		m_pImageList->SetImageAtIndex(i, pImg);
		CDonorImage* pDonorImg = new CDonorImage();
		pDonorImg->SetSize(24, 24);
		m_pImageList->SetImageAtIndex(i + DONOR_IMAGELIST_BASE, pDonorImg);

	}

	m_iMutedIconTexture = -1;
	m_iFriendTexture = -1;

	CreatePlayerMenu();

	LoadControlSettings(VGUI2_ROOT_DIR "ScorePanel.res");
	SetVisible(false);
}

void CScorePanel::UpdateTimeEndInternal() {
	if (m_iTimeEndCount <= 0)
		return;
	float flTime = gEngfuncs.GetClientTime();
	if (flTime >= m_flLastUpdateTimeEndTime) {
		long iMin = m_iTimeEndCount / 60;
		long iSec = m_iTimeEndCount % 60;
		char buf[64];
		snprintf(buf, sizeof(buf), "%.1d:%.2d", iMin, iSec);
		wchar_t wbuf[64];
		localize()->ConvertANSIToUnicode(buf, wbuf, sizeof(wbuf));
		m_pTimeEndLable->SetText(wbuf);
		m_iTimeEndCount -= (flTime - m_flLastUpdateTimeEndTime);
		m_flLastUpdateTimeEndTime = flTime + 1.0f;
	}
}

void CScorePanel::UpdateTimeEnd() {
	m_iTimeEndCount = GetBaseViewPort()->GetTimeEnd();
	m_flLastUpdateTimeEndTime = gEngfuncs.GetClientTime();
}
void CScorePanel::UpdateServerName(){
	wchar_t wbuf[MAX_SERVERNAME_LENGTH];
	localize()->ConvertANSIToUnicode(GetBaseViewPort()->GetServerName(), wbuf, sizeof(wbuf));
	m_pServerNameLabel->SetText(wbuf);
}
void CScorePanel::UpdateNextMap() {
	std::wstring buf = localize()->Find(NEXTMAP_LOCALIZE_TOKEN);;
	wchar_t wbuf[MAX_SERVERNAME_LENGTH];
	localize()->ConvertANSIToUnicode(GetBaseViewPort()->GetNextMap(), wbuf, sizeof(wbuf));
	buf += wbuf;
	m_pNextMapLable->SetText(buf.c_str());
}

void CScorePanel::EnableMousePointer(bool bEnable){
	if (bEnable && !IsVisible())
		return;
	SetMouseInputEnabled(bEnable);
	SetKeyBoardInputEnabled(false);

	int x = gEngfuncs.GetWindowCenterX();
	int y = gEngfuncs.GetWindowCenterY();
	vgui::input()->SetCursorPos(x, y);
}

void CScorePanel::UpdateOnPlayerInfo(int client)
{
	if (!IsVisible())
		return;

	RestoreSize();
	UpdateClientInfo(client);
	UpdateScoresAndCounts();
	Resize();
}

void CScorePanel::DeathMsg(int killer, int victim)
{
	if (victim == gPlayerRes.GetLocalPlayerInfo()->m_iIndex)
	{
		// if we were the one killed, set the scoreboard to indicate killer
		m_flKillerHighlightStart = gEngfuncs.GetClientTime();
		m_iKillerIndex = killer;
	}
}

void CScorePanel::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_pPlayerList->SetBorder(pScheme->GetBorder("FrameBorder"));
	m_pPlayerList->SetPaintBackgroundType(0);

	m_ThisPlayerBgColor = pScheme->GetColor("ThisPlayerBgColor", Color(0, 0, 0, 0));
	m_KillerBgColor = pScheme->GetColor("KillerBgColor", Color(0, 0, 0, 0));

	SetBgColor(GetSchemeColor("ScorePanel.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));

	s_iMutedIconTexture = m_iMutedIconTexture;
	s_iFriendTexture = m_iFriendTexture;
	s_iDefaultAvatarTexture = m_iDefaultAvatarTexture;
}

void CScorePanel::OnThink(){
	if (m_iKillerIndex != 0 && m_PlayerData[m_iKillerIndex].nItemID != -1)
		UpdateClientInfo(m_iKillerIndex);
	if (m_flLastUpdateTime + UPDATE_PERIOD <= gEngfuncs.GetAbsoluteTime())
		UpdateAllClients();
	for (int i = 1; i <= SC_MAX_PLAYERS; i++){
		CPlayerImage* pImg = reinterpret_cast<CPlayerImage*>(m_pImageList->GetImage(i));
		pImg->OnTick();
	}
	UpdateTimeEndInternal();
}

void CScorePanel::MenuMute() {
	OnPlayerMenuCommand(MenuAction::Mute);
}
void CScorePanel::MenuSteamProfile() {
	OnPlayerMenuCommand(MenuAction::SteamProfile);
}
void CScorePanel::MenuSteamURL() {
	OnPlayerMenuCommand(MenuAction::SteamURL);
}
void CScorePanel::MenuCopyName() {
	OnPlayerMenuCommand(MenuAction::CopyName);
}
void CScorePanel::MenuCopyNameRaw() {
	OnPlayerMenuCommand(MenuAction::CopyNameRaw);
}
void CScorePanel::MenuCopySteamID() {
	OnPlayerMenuCommand(MenuAction::CopySteamID);
}
void CScorePanel::MenuCopySteamID64() {
	OnPlayerMenuCommand(MenuAction::CopySteamID64);
}

const char* CScorePanel::GetName()
{
	return VIEWPORT_SCOREPANEL_NAME;
}

void CScorePanel::Reset()
{
	if (IsVisible())
	{
		ShowPanel(false);
	}
	for (int i = 1; i <= SC_MAX_PLAYERS; i++) {
		CPlayerImage* pImg = reinterpret_cast<CPlayerImage*>(m_pImageList->GetImage(i));
		pImg->Reset();
	}
	m_iKillerIndex = 0;
	m_flKillerHighlightStart = 0;
}

void CScorePanel::ShowPanel(bool state)
{
	if (state == IsVisible())
		return;

	if (state)
	{
		FullUpdate();
		Activate();
	}
	else
	{
		SetMouseInputEnabled(false);
		SetKeyBoardInputEnabled(false);
		m_pPlayerMenu->SetVisible(false);
	}

	SetVisible(state);
}

VPANEL CScorePanel::GetVPanel()
{
	return BaseClass::GetVPanel();
}

bool CScorePanel::IsVisible()
{
	return BaseClass::IsVisible();
}

void CScorePanel::SetParent(VPANEL parent)
{
	BaseClass::SetParent(parent);
}

void CScorePanel::FullUpdate(){
	UpdateServerName();
	UpdateMapName();
	UpdateNextMap();
	RefreshItems();
}

void CScorePanel::UpdateMapName()
{
	char buf[64];
	wchar_t wbuf[64];
	V_FileBase(gEngfuncs.pfnGetLevelName(), buf, sizeof(buf));
	localize()->ConvertANSIToUnicode(buf, wbuf, sizeof(wbuf));
	m_pMapNameLabel->SetText(wbuf);
}

void CScorePanel::RefreshItems()
{
	std::fill(m_TeamData.begin(), m_TeamData.end(), TeamData());
	std::fill(m_PlayerData.begin(), m_PlayerData.end(), PlayerData());
	std::fill(m_IsTeamSectionCreated.begin(), m_IsTeamSectionCreated.end(), false);
	m_pPlayerList->RemoveAll();
	m_pPlayerList->RemoveAllSections();

	RestoreSize();

	// Assign player teams, calculate team scores
	for (int i = 1; i <= SC_MAX_PLAYERS; i++)
	{
		PlayerInfo* pi = gPlayerRes.GetPlayerInfo(i)->Update();
		if (!pi)
			continue;
		PlayerData& pd = m_PlayerData[i];
		pd.bIsConnected = pi->m_bIsConnected;

		if (!pd.bIsConnected)
			continue;

		pd.nItemID = -1;
		pd.nTeamID = GetPlayerTeam(pi);

		TeamData& td = m_TeamData[pd.nTeamID];
		td.iFrags += pi->m_iFrags;
		td.iDeaths += pi->m_iDeaths;
		td.iPlayerCount++;
	}

	// Override team scores and add them to sorting list
	int iTeamCount = 0;

	for (int i = 0; i < PREDEFINED_TEAM_COUNT; i++)
	{
		TeamData& td = m_TeamData[i];

		if (td.iPlayerCount == 0)
			continue;

		TeamInfo* ti = gTeamRes.GetTeamInfo(i);

		if (ti->IsScoreOverriden())
		{
			td.iFrags = ti->GetFrags();
			td.iDeaths = ti->GetDeaths();
		}

		m_SortedTeamIDs[iTeamCount] = i;
		iTeamCount++;
	}

	// Sort teams based on the score
	std::sort(m_SortedTeamIDs.begin(), m_SortedTeamIDs.begin() + iTeamCount, [&](int ilhs, int irhs) {
		const TeamData& lhs = m_TeamData[ilhs];
		const TeamData& rhs = m_TeamData[irhs];

		// Compare kills
		if (lhs.iFrags > rhs.iFrags)
			return true;
		else if (lhs.iFrags < rhs.iFrags)
			return false;

		// Compare deaths if kills are equal
		if (lhs.iDeaths < rhs.iDeaths)
			return true;
		else if (lhs.iDeaths > rhs.iDeaths)
			return false;

		// Compare idx if everything is equal
		return ilhs > irhs;
		});

	// Create header before any other sections
	CreateSection(HEADER_SECTION_ID);

	// Create sections for teams
	if (iTeamCount != 1 || m_SortedTeamIDs[0] != 0)
	{
		for (int i = 0; i < iTeamCount; i++)
		{
			CreateSection(m_SortedTeamIDs[i]);
		}
	}
	UpdateAllClients();
}

void CScorePanel::CreateSection(int nTeamID){
	if (nTeamID < 0)
		return;
	if (m_IsTeamSectionCreated[nTeamID])
		return;

	m_IsTeamSectionCreated[nTeamID] = true;

	TeamData& td = m_TeamData[nTeamID];

	m_pPlayerList->AddSection(nTeamID, "", StaticPlayerSortFuncByFrags);

	if (nTeamID == HEADER_SECTION_ID){
		m_pPlayerList->SetSectionAlwaysVisible(nTeamID);
		m_pPlayerList->SetSectionDividerColor(nTeamID, Color(0, 0, 0, 0));
	}

	// Avatar
	m_pPlayerList->AddColumnToSection(nTeamID, AVATAR_KEY, "",
		SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::COLUMN_CENTER,
		m_iColumnWidthAvatar);

	//Donor
	m_pPlayerList->AddColumnToSection(nTeamID, DONOR_KEY, "",
		SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::COLUMN_CENTER,
		m_iColumnWidthDonor);
	
	// Name
	const char* nameCol;

	if (nTeamID == HEADER_SECTION_ID)
		nameCol = NAME_LOCALIZE_TOKEN;
	else
		nameCol = NA_LOCALIZE_TOKEN;

	m_pPlayerList->AddColumnToSection(nTeamID, NAME_KEY, nameCol,
		SectionedListPanel::COLUMN_BRIGHT | SectionedListPanel::COLUMN_COLORED,
		GetNameColumnWidth());

	// SteamID
	if (hud_scoreboard_showsteamid->value > 0)
	{
		m_pPlayerList->AddColumnToSection(nTeamID, STEAMID_KEY, nTeamID == HEADER_SECTION_ID ? STEAMID_LOCALIZE_TOKEN : "",
			SectionedListPanel::COLUMN_BRIGHT,
			m_iColumnWidthSteamID);
	}

	// Frags
	m_pPlayerList->AddColumnToSection(nTeamID, HEALTH_KEY, nTeamID == HEADER_SECTION_ID ? HEALTH_LOCALIZE_TOKEN : NA_LOCALIZE_TOKEN,
		SectionedListPanel::COLUMN_BRIGHT,
		m_iColumnWidthHealth);

	// Frags
	m_pPlayerList->AddColumnToSection(nTeamID, ARMOR_KEY, nTeamID == HEADER_SECTION_ID ? ARMOR_LOCALIZE_TOKEN : NA_LOCALIZE_TOKEN,
		SectionedListPanel::COLUMN_BRIGHT,
		m_iColumnWidthArmor);

	// Frags
	m_pPlayerList->AddColumnToSection(nTeamID, FRAG_KEY, nTeamID == HEADER_SECTION_ID ? SCORE_LOCALIZE_TOKEN : NA_LOCALIZE_TOKEN,
		SectionedListPanel::COLUMN_BRIGHT,
		m_iColumnWidthFrags);

	// Deaths
	m_pPlayerList->AddColumnToSection(nTeamID, DEATH_KEY, nTeamID == HEADER_SECTION_ID ? DEATH_LOCALIZE_TOKEN : NA_LOCALIZE_TOKEN,
		SectionedListPanel::COLUMN_BRIGHT,
		m_iColumnWidthDeaths);

	// Ping
	const char* pingLabel;

	if (nTeamID == HEADER_SECTION_ID)
		pingLabel = hud_scoreboard_showloss->value > 0 ? PINGLOSS_LOCALIZE_TOKEN : PING_LOCALIZE_TOKEN;
	else
		pingLabel = "";

	m_pPlayerList->AddColumnToSection(nTeamID, PING_KEY, pingLabel,
		SectionedListPanel::COLUMN_BRIGHT,
		m_iColumnWidthPing);
}

void CScorePanel::UpdateAllClients()
{
	for (int i = 1; i <= SC_MAX_PLAYERS; i++)
	{
		UpdateClientInfo(i);
	}

	UpdateScoresAndCounts();
	Resize();

	m_flLastUpdateTime = gEngfuncs.GetAbsoluteTime();
}

void CScorePanel::UpdateClientInfo(int client)
{
	PlayerInfo* pi = gPlayerRes.GetPlayerInfo(client);
	PlayerData& pd = m_PlayerData[client];
	if (pi->m_bIsConnected && !pd.bIsConnected){
		// Player just connected
		pd.bIsConnected = true;

		if (pd.nItemID != -1 && m_pPlayerList->IsItemIDValid(pd.nItemID)) {
			m_pPlayerList->RemoveItem(pd.nItemID);
			pi->Reset();
		}
		pd.nItemID = -1;
		pd.nTeamID = static_cast<int>(pi->m_iTeamNumber);
	}
	else if (!pi->m_bIsConnected && pd.bIsConnected){
		// Player disconnected
		pd.bIsConnected = false;

		m_pPlayerList->RemoveItem(pd.nItemID);
		pd.nItemID = -1;
		pd.nTeamID = 0;

		pi->Reset();
	}
	Assert(pd.bIsConnected == pi->m_bIsConnected);
	// Skip unconnected players
	if (!pi->IsValid())
		return;
	if (GetPlayerTeam(pi) != pd.nTeamID){
		// Player changed team
		m_pPlayerList->RemoveItem(pd.nItemID);
		pd.nItemID = -1;
		pd.nTeamID = static_cast<int>(pi->m_iTeamNumber);
	}

	// Create section for player's team if need to
	CreateSection(pd.nTeamID);
	KeyValues* playerKv = new KeyValues("data");
	KeyValues* colorKv = new KeyValues("colorData");
	{
		char buf[128];
		playerKv->SetInt("client", client);
		if (pi->IsThisPlayer())
			playerKv->SetBool("thisplayer", true);
		// Avatar
		UpdateClientIcon(pi);
		UpdatePlayerAdmin(pi);
		playerKv->SetInt(AVATAR_KEY, client);
		//Donor
		UpdatePlayerDonor(pi);
		playerKv->SetInt(DONOR_KEY, client + DONOR_IMAGELIST_BASE);
		// Name
		if (hud_scoreboard_showrealname->value > 0 && pi->m_szRealName[0] != '\0')
				snprintf(buf, sizeof(buf), "%s (%s)", pi->GetName(), pi->m_szRealName.c_str());
		else
			snprintf(buf, sizeof(buf), "%s", pi->GetName());
		playerKv->SetString(NAME_KEY, buf);
		// SteamID
		playerKv->SetString(STEAMID_KEY, hud_scoreboard_showsteamidtype->value > 0 ? pi->m_szSteamID64 :  pi->m_pSteamId.Render());

		if (pi->m_bIsSpectate) {
			colorKv->SetInt(HEALTH_KEY,COLOR_GREY);
			playerKv->SetString(HEALTH_KEY, m_szSpectatorTag);
			colorKv->SetInt(ARMOR_KEY, COLOR_GREY);
			playerKv->SetString(ARMOR_KEY, NA_LOCALIZE_TOKEN);
		}
		else {
			int iHealth = pi->m_iHealth;
			if(iHealth < 0)
				playerKv->SetString(HEALTH_KEY, HEALTHDEAD_LOCALIZE_TOKEN);
			else
				playerKv->SetInt(HEALTH_KEY, iHealth);
			colorKv->SetInt(HEALTH_KEY, COLOR_PERCENT);

			int iArmor = pi->m_iArmor;
			if (iArmor < 0)
				playerKv->SetString(ARMOR_KEY, NA_LOCALIZE_TOKEN);
			else
				playerKv->SetInt(ARMOR_KEY, iArmor);
			colorKv->SetInt(ARMOR_KEY, COLOR_PERCENT);
		}
		
		// Frags & deaths
		playerKv->SetInt(FRAG_KEY, pi->m_iFrags);
		playerKv->SetInt(DEATH_KEY, pi->m_iDeaths);

		// Ping
		// ForceUpdatePing
		pi->UpdatePing();
		if (hud_scoreboard_showloss->value > 0){
			snprintf(buf, sizeof(buf), "%d/%d", pi->m_iPing, pi->m_iLoss);
			playerKv->SetString(PING_KEY, buf);
		}
		else
			playerKv->SetInt(PING_KEY, pi->m_iPing);
		colorKv->SetInt(PING_KEY, COLOR_PING);
	}

	Color SectionColor = GetBaseViewPort()->GetPlayerColor(client);
	if (pd.nItemID == -1){
		// Create player's row
		pd.nItemID = m_pPlayerList->AddItem(pd.nTeamID, playerKv);
		m_pPlayerList->SetItemFgColor(pd.nItemID, SectionColor);
		m_pPlayerList->SetItemColorData(pd.nItemID, colorKv);
		m_pPlayerList->InvalidateLayout();
	}
	else{
		// Update player's row
		m_pPlayerList->ModifyItem(pd.nItemID, pd.nTeamID, playerKv);
	}
	SectionColor = GetPlayerBgColor(pi);
	SectionColor.SetColor(SectionColor.r(), SectionColor.g(), SectionColor.b(), 80);
	m_pPlayerList->SetItemBgColor(pd.nItemID, SectionColor);

	playerKv->deleteThis();
	colorKv->deleteThis();
}

void CScorePanel::UpdatePlayerDonor(PlayerInfo* pi) {
	CDonorImage* pImg = static_cast<CDonorImage*>(m_pImageList->GetImage(pi->m_iIndex + DONOR_IMAGELIST_BASE));
	int iTex = -1;
	switch (pi->m_eDonor) {
	case PlayerInfo::DONOR::ELECTRIC_CROWBAR:iTex = m_iDonor1IconTexture; break;
	case PlayerInfo::DONOR::GOLDED_UZI:iTex = m_iDonor2IconTexture; break;
	case PlayerInfo::DONOR::GOLED_DOLLAR:iTex = m_iDonor3IconTexture; break;
	case PlayerInfo::DONOR::TESTER:iTex = m_iDonor4IconTexture; break;
	case PlayerInfo::DONOR::ARTIST:iTex = m_iDonor5IconTexture; break;
	case PlayerInfo::DONOR::DEVELOEPR:iTex = m_iDonor6IconTexture; break;
	}
	pImg->SetTexture(iTex);
}

void CScorePanel::UpdatePlayerAdmin(PlayerInfo* pi) {
	CPlayerImage* pImg = static_cast<CPlayerImage*>(m_pImageList->GetImage(pi->m_iIndex));
	int tex = -1;
	switch (pi->m_iAdmin) {
	case PlayerInfo::ADMIN::OPRATER:tex = m_iAdminIconTexture; break;
	case PlayerInfo::ADMIN::SERVER_OWNER:tex = m_iServerIconTexture; break;
	}
	//Update admin 
	pImg->SetAdminTexture(tex);
}

void CScorePanel::UpdateClientIcon(PlayerInfo* pi){
	CPlayerImage* pImg = static_cast<CPlayerImage*>(m_pImageList->GetImage(pi->m_iIndex));
	// Update size
	int size = GetClientIconSize();
	pImg->SetSize(size, size);
	// Update muted state
	pImg->SetMuted(GetClientVoiceMgr()->IsPlayerBlocked(pi->m_iIndex));
	// Update avatar
	CSteamID* steamID = &pi->m_pSteamId;
	if (hud_scoreboard_showavatars->value > 0 && steamID->IsValid())
		pImg->SetAvatar(steamID);
	else
		pImg->SetAvatar(nullptr);
}

void CScorePanel::UpdateScoresAndCounts(){
	// Reset team data
	for (int i = 1; i <= PREDEFINED_TEAM_COUNT; i++){
		TeamData& td = m_TeamData[i];
		td.iPlayerCount = 0;
		td.iFrags = 0;
		td.iDeaths = 0;
	}

	// Refresh scores
	int iPlayerCount = 0;

	for (int i = 1; i <= SC_MAX_PLAYERS; i++){
		PlayerInfo* pi = gPlayerRes.GetPlayerInfo(i);

		if (!pi->IsValid())
			continue;

		TeamData& td = m_TeamData[static_cast<int>(pi->m_iTeamNumber)];
		td.iFrags += pi->m_iFrags;
		td.iDeaths += pi->m_iDeaths;
		td.iPlayerCount++;

		iPlayerCount++;
	}

	char buf[128];
	wchar_t wbuf[128] = {};

	auto fnUpdateTeamHeader = [&](const char* pszTeamName, int nTeamID) {
		TeamData& td = m_TeamData[nTeamID];

		// Team name and player count
		wchar_t wbuf2[128];
		wchar_t* localizedName = nullptr;
		if (!(localizedName = localize()->Find(pszTeamName))){
			// set localizedName to pszTeamName converted to WString
			localize()->ConvertANSIToUnicode(pszTeamName, wbuf2, sizeof(wbuf2));
			localizedName = wbuf2;
		}

		swprintf_s(wbuf, 128, L"%s (%d/%d)", localizedName, td.iPlayerCount, iPlayerCount);
		m_pPlayerList->ModifyColumn(nTeamID, NAME_KEY, wbuf);

		m_pPlayerList->ModifyColumn(nTeamID, HEALTH_KEY, L"");
		m_pPlayerList->ModifyColumn(nTeamID, ARMOR_KEY, L"");

		// Team frags
		snprintf(buf, sizeof(buf), "%d", td.iFrags);
		localize()->ConvertANSIToUnicode(buf, wbuf, sizeof(wbuf));
		m_pPlayerList->ModifyColumn(nTeamID, FRAG_KEY, wbuf);

		// Team deaths
		snprintf(buf, sizeof(buf), "%d", td.iDeaths);
		localize()->ConvertANSIToUnicode(buf, wbuf, sizeof(wbuf));
		m_pPlayerList->ModifyColumn(nTeamID, DEATH_KEY, wbuf);
	};

	// Update team score and player count
	for (int i = 1; i <= PREDEFINED_TEAM_COUNT; i++){
		TeamData& td = m_TeamData[i];

		if (td.iPlayerCount == 0)
			continue;

		TeamInfo* ti = gTeamRes.GetTeamInfo(i);

		if (ti->IsScoreOverriden()){
			td.iFrags = ti->GetFrags();
			td.iDeaths = ti->GetDeaths();
		}

		fnUpdateTeamHeader(ti->GetName(), i);
	}
	// Update total player count
	snprintf(buf, sizeof(buf), "%d/%d", iPlayerCount, gEngfuncs.GetMaxClients());
	m_pPlayerCountLabel->SetText(buf);
}

int CScorePanel::GetNameColumnWidth()
{
	int w = m_iColumnWidthName;

	if (hud_scoreboard_showsteamid->value <= 0)
		w += m_iColumnWidthSteamID;

	return w;
}

int CScorePanel::GetPlayerTeam(PlayerInfo* pi)
{
	return static_cast<int>(pi->m_iTeamNumber);
}

Color CScorePanel::GetPlayerBgColor(PlayerInfo* pi)
{
	if (pi->IsThisPlayer())
	{
		return m_ThisPlayerBgColor;
	}
	else
	{
		return GetBaseViewPort()->GetPlayerColor(pi->m_iIndex);
	}

	return Color(0, 0, 0, 0);
}

int CScorePanel::GetClientIconSize()
{
	return CMathlib::clamp(m_pPlayerList->GetLineSpacing() - 2, 0, 32);
}

void CScorePanel::CreatePlayerMenu(){
	m_pPlayerMenu = new Menu(this, "PlayerMenu");
	m_pPlayerMenu->SetVisible(false);
	m_pPlayerMenu->AddActionSignalTarget(this);

	m_MenuData.nMuteItemID = m_pPlayerMenu->AddMenuItem("Mute", "#Scores_MenuMute", new KeyValues("MenuMute"), this);
	m_pPlayerMenu->AddSeparator();

	m_MenuData.nProfilePageItemID = m_pPlayerMenu->AddMenuItem("SteamProfile", "#Scores_MenuSteamProfile", new KeyValues("MenuSteamProfile"), this);
	m_MenuData.nProfileUrlItemID = m_pPlayerMenu->AddMenuItem("SteamURL", "#Scores_MenuSteamURL", new KeyValues("MenuSteamURL"), this);
	m_pPlayerMenu->AddSeparator();

	m_pPlayerMenu->AddMenuItem("CopyName", "#Scores_MenuCopyName", new KeyValues("MenuCopyName"), this);
	m_pPlayerMenu->AddMenuItem("CopyNameRaw", "#Scores_MenuCopyNameRaw", new KeyValues("MenuCopyNameRaw"), this);
	m_pPlayerMenu->AddMenuItem("CopySteamID", "#Scores_MenuCopySteamID", new KeyValues("MenuCopySteamID"), this);
	m_pPlayerMenu->AddMenuItem("CopySteamID64", "#Scores_MenuCopySteamID64", new KeyValues("MenuCopySteamID64"), this);
}

void CScorePanel::OpenPlayerMenu(int itemID){
	// Set menu info
	m_MenuData.nItemID = itemID;
	m_MenuData.nClient = 0;
	KeyValues* kv = m_pPlayerList->GetItemData(itemID);
	if (!kv)
		return;
	m_MenuData.nClient = kv->GetInt("client", 0);
	if (m_MenuData.nClient == 0)
		return;

	// SteamID64
	m_MenuData.nSteamID64 = gPlayerRes.GetPlayerInfo(m_MenuData.nClient)->m_pSteamId.ConvertToUint64();
	if (m_MenuData.nSteamID64 != 0){
		m_pPlayerMenu->SetItemEnabled(m_MenuData.nProfilePageItemID, true);
		m_pPlayerMenu->SetItemEnabled(m_MenuData.nProfileUrlItemID, true);
	}
	else{
		m_pPlayerMenu->SetItemEnabled(m_MenuData.nProfilePageItemID, false);
		m_pPlayerMenu->SetItemEnabled(m_MenuData.nProfileUrlItemID, false);
	}

	// Player muting
	bool thisPlayer = kv->GetBool("thisplayer", 0);
	if (thisPlayer){
		// Can't mute yourself
		m_pPlayerMenu->UpdateMenuItem(m_MenuData.nMuteItemID, "#Scores_MenuMute", new KeyValues("MenuMute"));
		m_pPlayerMenu->SetItemEnabled(m_MenuData.nMuteItemID, false);
	}
	else{
		m_pPlayerMenu->SetItemEnabled(m_MenuData.nMuteItemID, true);
		if (GetClientVoiceMgr()->IsPlayerBlocked(m_MenuData.nClient))
			m_pPlayerMenu->UpdateMenuItem(m_MenuData.nMuteItemID, "#Scores_MenuUnmute", new KeyValues("MenuMute"));
		else
			m_pPlayerMenu->UpdateMenuItem(m_MenuData.nMuteItemID, "#Scores_MenuMute", new KeyValues("MenuMute"));
	}
	m_pPlayerMenu->PositionRelativeToPanel(this, Menu::CURSOR, 0, true);
}

void CScorePanel::OnItemContextMenu(int itemID) {
	OpenPlayerMenu(itemID);
}

void CScorePanel::OnPlayerMenuCommand(MenuAction command)
{
	PlayerInfo* pi = gPlayerRes.GetPlayerInfo(m_MenuData.nClient);

	if (!pi->m_bIsConnected)
		return;

	switch (command)
	{
	case MenuAction::Mute:
	{
		if (pi->IsThisPlayer())
			return;
		if(GetClientVoiceMgr()->IsPlayerBlocked(pi->m_iIndex))
			GetClientVoiceMgr()->SetPlayerBlockedState(pi->m_iIndex, false);
		else
			GetClientVoiceMgr()->SetPlayerBlockedState(pi->m_iIndex, true);
		// Muting one player may mute others (if they have identical Unique IDs)
		UpdateAllClients();

		break;
	}
	case MenuAction::SteamProfile:
	{
		if (SteamFriends())
			SteamFriends()->ActivateGameOverlayToUser("steamid", pi->m_pSteamId);
		else
		{
			// Open in browser
			std::string url = STEAM_PROFILE_URL + std::to_string(m_MenuData.nSteamID64);
			system()->ShellExecute("open", url.c_str());
		}

		break;
	}
	case MenuAction::SteamURL:
	{
		std::string url = STEAM_PROFILE_URL + std::to_string(m_MenuData.nSteamID64);
		system()->SetClipboardText(url.c_str(), url.size());
		break;
	}
	case MenuAction::CopyName:
	{
		wchar_t name[MAX_PLAYERNAME_LENGTH + 1];
		localize()->ConvertANSIToUnicode(pi->m_szRealName.c_str(), name, sizeof(name));
		system()->SetClipboardText(name, wcslen(name));
		break;
	}
	case MenuAction::CopyNameRaw:
	{
		wchar_t name[MAX_PLAYERNAME_LENGTH + 1];
		localize()->ConvertANSIToUnicode(pi->GetName(), name, sizeof(name));
		system()->SetClipboardText(name, wcslen(name));
		break;
	}
	case MenuAction::CopySteamID:
	{
		auto steamid = pi->m_pSteamId.Render();
		system()->SetClipboardText(steamid, strlen(steamid));
		break;
	}
	case MenuAction::CopySteamID64:
	{
		auto steamid = pi->m_szSteamID64;
		system()->SetClipboardText(steamid, strlen(steamid));
		break;
	}
	}
}

CScorePanel::SizeMode CScorePanel::GetSizeMode()
{
	return (SizeMode)CMathlib::clamp((int)hud_scoreboard_size->value, 0, 2);
}

int CScorePanel::GetLineSpacingForHeight(int h)
{
	if (h < 600)
		return 18; // (0; 600)
	if (h < 720)
		return 22; // [600; 720]
	if (h < 800)
		return 24; // [720; 800)
	if (h < 1024)
		return 26; // [800; 1024)
	if (h < 1080)
		return 28; // [1024; 1080)

	return 28; // >= 1080
}

int CScorePanel::GetLineSpacingForNormal()
{
	if (hud_scoreboard_spacing_normal->value > 0)
		return hud_scoreboard_spacing_normal->value;
	return 0;
}

int CScorePanel::GetLineSpacingForCompact()
{
	if (hud_scoreboard_spacing_compact->value > 0)
		return hud_scoreboard_spacing_compact->value;
	return GetLineSpacingForHeight(ScreenHeight());
}

void CScorePanel::RestoreSize()
{
	if (GetSizeMode() == SizeMode::Compact)
		m_pPlayerList->SetLineSpacingOverride(GetLineSpacingForCompact());
	else
		m_pPlayerList->SetLineSpacingOverride(GetLineSpacingForNormal());
}

void CScorePanel::Resize()
{
	SizeMode mode = GetSizeMode();

	// Returns true if scrollbar was enabled
	auto fnUpdateSize = [&](int& height) {
		int wide, tall, x, y;
		int listHeight = 0, addHeight = 0;
		bool bIsOverflowed = false;
		height = 0;

		m_pPlayerList->GetPos(x, y);
		addHeight = y + m_iBottomPadding; // Distance on the top and on the bottom of the player list
		height += addHeight;
		m_pPlayerList->GetContentSize(wide, tall);
		listHeight = max(m_iMinHeight, tall);
		height += listHeight;

		if ((int)ScreenHeight() - height < m_iVerticalMargin * 2)
		{
			// It didn't fit
			height = (int)ScreenHeight() - m_iVerticalMargin * 2;
			listHeight = height - addHeight;
			m_pPlayerList->SetVerticalScrollbar(true);
			bIsOverflowed = true;
		}
		else
		{
			m_pPlayerList->SetVerticalScrollbar(false);
		}

		m_pPlayerList->GetSize(wide, tall);
		m_pPlayerList->SetSize(wide, listHeight);

		return bIsOverflowed;
	};

	int wide, tall, x, y;
	int height;
	if (fnUpdateSize(height) && mode == SizeMode::Auto)
	{
		// Content overflowed, scrollbar is now visible. Set comapct line spacing
		m_pPlayerList->SetLineSpacingOverride(GetLineSpacingForCompact());

		// Refresh player info to update avatar sizes
		for (int i = 1; i <= SC_MAX_PLAYERS; i++)
		{
			UpdateClientInfo(i);
		}

		// Resize again
		fnUpdateSize(height);
	}

	GetSize(wide, tall);
	SetSize(wide, height);

	// Move to center
	GetPos(x, y);
	y = (ScreenHeight() - height) / 2;
	SetPos(x, y);
}

bool CScorePanel::StaticPlayerSortFuncByFrags(SectionedListPanel* list, int itemID1, int itemID2)
{
	KeyValues* it1 = list->GetItemData(itemID1);
	KeyValues* it2 = list->GetItemData(itemID2);
	Assert(it1 && it2);
	int frags1 = it1->GetInt(FRAG_KEY);
	int frags2 = it2->GetInt(FRAG_KEY);
	int death1 = it1->GetInt(DEATH_KEY);
	int death2 = it2->GetInt(DEATH_KEY);
	int ping1 = it1->GetInt(PING_KEY);
	int ping2 = it2->GetInt(PING_KEY);
	//Ping Zero??
	if (frags1 + death1 + ping1 == 0)
		return false;
	else if (frags2 + death2 + ping2 == 0)
		return true;
	// first compare frags
	if (frags1 > frags2)
		return true;
	else if (frags1 < frags2)
		return false;
	// next compare deaths
	if (death1 > death2)
		return false;
	else if (death1 < death2)
		return true;
	// the same, so compare itemID's (as a sentinel value to get deterministic sorts)
	return itemID1 < itemID2;
}