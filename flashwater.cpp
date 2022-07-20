#include <metahook.h>
#include "flashwater.h"
#include <vguilocal.h>

#define VIEWPORT_FLASHPANEL_NAME "FlashPanel"

CFlashPanel::CFlashPanel()
	: BaseClass(nullptr, VIEWPORT_FLASHPANEL_NAME){
	SetTitle("", true);
	SetCloseButtonVisible(false);
	SetMoveable(false);
	SetSizeable(false);
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "FlashPanelScheme.res", "FlashPanelScheme");
	SetScheme("FlashPanelScheme");

	m_pFlashSilder = new vgui::Slider(this, "FlashSlider");
	m_pWaterSilder = new vgui::Slider(this, "WaterSlider");

	LoadControlSettings(VGUI2_ROOT_DIR "ScorePanel.res");
	SetVisible(true);
}