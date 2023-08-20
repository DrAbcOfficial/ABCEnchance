#include "GameUI/OptionsDialog.h"

#include "vgui_controls/Button.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/QueryBox.h"

#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"

#include "vguilocal.h"

#include "GameUI/OptionsSubMultiplayer.h"
#include "GameUI/OptionsSubKeyboard.h"
#include "GameUI/OptionsSubMouse.h"
#include "GameUI/OptionsSubAudio.h"
#include "GameUI/OptionsSubVoice.h"
#include "GameUI/OptionsSubVideo.h"
#include "GameUI/OptionsSubAdvanced.h"
#include "GameUI/ModInfo.h"

#include "KeyValues.h"


COptionsDialog::COptionsDialog(vgui::Panel *parent) : PropertyDialog(parent, "OptionsDialog")
{
	SetBounds(0, 0, 512, 406);
	SetSizeable(false);
	SetMoveable(false);
	SetMenuButtonVisible(false);

	SetTitle("#GameUI_Options", true);
	SetSize(gScreenInfo.iWidth / 3 * 2, gScreenInfo.iHeight);

	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "OptionDialogScheme.res", "OptionDialogScheme");
	SetScheme("OptionDialogScheme");

	if ((ModInfo().IsMultiplayerOnly() && !ModInfo().IsSinglePlayerOnly()) || (!ModInfo().IsMultiplayerOnly() && !ModInfo().IsSinglePlayerOnly()))
		m_pOptionsSubMultiplayer = new COptionsSubMultiplayer(this);

	m_pOptionsSubKeyboard = new COptionsSubKeyboard(this);
	m_pOptionsSubMouse = new COptionsSubMouse(this);
	m_pOptionsSubAudio = new COptionsSubAudio(this);
	//m_pOptionsSubVideo = new COptionsSubVideo(this);
	m_pOptionsSubAdvanced = new COptionsSubAdvanced(this);
	if (!ModInfo().IsSinglePlayerOnly())
	{
		m_pOptionsSubVoice = new COptionsSubVoice(this);
	}

	AddPage(m_pOptionsSubMultiplayer, "#GameUI_Multiplayer");
	AddPage(m_pOptionsSubKeyboard, "#GameUI_Keyboard");
	AddPage(m_pOptionsSubMouse, "#GameUI_Mouse");
	AddPage(m_pOptionsSubAudio, "#GameUI_Audio");
	//AddPage(m_pOptionsSubVideo, "#GameUI_Video");
	AddPage(m_pOptionsSubVoice, "#GameUI_Voice");
	AddPage(m_pOptionsSubAdvanced, "#GameUI_Advanced");

	SetApplyButtonVisible(true);
	GetPropertySheet()->SetTabWidth(84);
}


COptionsDialog::~COptionsDialog(void)
{
}

void COptionsDialog::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetBgColor(GetSchemeColor("PropertyDialog.BgColor", GetSchemeColor("Frame.BgColor", pScheme), pScheme));
	SetFgColor(GetSchemeColor("PropertyDialog.FgColor", GetSchemeColor("Frame.FgColor", pScheme), pScheme));
	InvalidateLayout();
}

void COptionsDialog::Activate(void)
{
	BaseClass::Activate();
	
	if (m_pOptionsSubMultiplayer)
	{
		if (GetActivePage() != m_pOptionsSubMultiplayer)
			GetPropertySheet()->SetActivePage(m_pOptionsSubMultiplayer);
	}
	else
	{
		if (GetActivePage() != m_pOptionsSubKeyboard)
			GetPropertySheet()->SetActivePage(m_pOptionsSubKeyboard);
	}
	ResetAllData();
	EnableApplyButton(false);
	SetSize(gScreenInfo.iWidth / 3 * 2, gScreenInfo.iHeight);
	
	SetPos(gScreenInfo.iWidth, 0);
	m_flAnimeTime = vgui::system()->GetCurrentTime() + 0.5f;
	m_bPushIn = true;
	m_bPushOut = false;
}

void COptionsDialog::Deactivate(){
	m_flAnimeTime = vgui::system()->GetCurrentTime() + 0.5f;
	m_bPushIn = false;
	m_bPushOut = true;
}

void COptionsDialog::Run(void)
{
	SetTitle("#GameUI_Options", true);
	Activate();
}

void COptionsDialog::OnClose(void)
{
	BaseClass::OnClose();
}

void COptionsDialog::OnThink(){
	if (m_bPushIn) {
		int xpos, ypos;
		int iHalfScreen = gScreenInfo.iWidth / 3 * 2;
		GetPos(xpos, ypos);
		if (xpos <= iHalfScreen) {
			SetPos(iHalfScreen, ypos);
			m_bPushIn = false;
			return;
		}
		float flRatio = (m_flAnimeTime - vgui::system()->GetCurrentTime()) / 0.5f;
		SetPos(iHalfScreen + iHalfScreen * flRatio, ypos);
	}
	else if (m_bPushOut) {
		int xpos, ypos;
		int iHalfScreen = gScreenInfo.iWidth / 3 * 2;
		GetPos(xpos, ypos);
		if (xpos >= gScreenInfo.iWidth) {
			SetVisible(false);
			m_bPushOut = false;
			return;
		}
		float flRatio = 1 - ((m_flAnimeTime - vgui::system()->GetCurrentTime()) / 0.5f);
		SetPos(iHalfScreen + iHalfScreen * flRatio, ypos);
	}
}

void COptionsDialog::OnGameUIHidden(void)
{
	for (int i = 0; i < GetChildCount(); i++)
	{
		Panel *pChild = GetChild(i);

		if (pChild)
			PostMessage(pChild, new KeyValues("GameUIHidden"));
	}
}

static COptionsDialog* s_OptDialog;
COptionsDialog* CeateOptionDialog(vgui::Panel* parent) {
	s_OptDialog = new COptionsDialog(parent);
	return s_OptDialog;
}
COptionsDialog* OptionsDialog(){
	return s_OptDialog;
}
