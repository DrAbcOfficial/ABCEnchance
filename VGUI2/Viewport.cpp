#include <metahook.h>
#include <vgui/VGUI.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/IEngineVGui.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Label.h>
#include "BaseUI.h"
#include "viewport.h"
#include "MemPool.h"
#include "exportfuncs.h"
#include <stdexcept>

using namespace vgui;

CViewport *g_pViewPort = NULL;

CViewport::CViewport(void) : Panel(NULL, "ABCEnchanceViewport")
{
	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	MakePopup(false, true);

	SetScheme("ABCEnchanceScheme");
	SetBounds(0, 0, swide, stall);
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	SetProportional(true);
	m_szLevelName[0] = 0;
}

CViewport::~CViewport(void)
{
	
}

void CViewport::Start(void)
{
	//m_pSubtitlePanel = new SubtitlePanel(NULL);
	SetVisible(false);
}

void CViewport::SetParent(VPANEL vPanel)
{
	BaseClass::SetParent(vPanel);

	//m_pSubtitlePanel->SetParent(this);
}

void CViewport::Think(void)
{
	auto levelname = gEngfuncs.pfnGetLevelName();
	if (!levelname || !levelname[0])
		return;

}

void CViewport::VidInit(void)
{
	m_szLevelName[0] = 0;

}

void CViewport::Init(void)
{

}

void CViewport::ActivateClientUI(void)
{
	SetVisible(true);
}

void CViewport::HideClientUI(void)
{
	SetVisible(false);
}

void CViewport::Paint(void)
{
	BaseClass::Paint();

	//m_HudMessage.Draw();
}