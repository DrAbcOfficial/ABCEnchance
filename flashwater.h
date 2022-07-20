#pragma once
#include "MessageMap.h"
#include "vgui_controls/Slider.h"
#include "vgui_controls/Frame.h"
#include "IViewportPanel.h"

class CFlashPanel : public vgui::Frame, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CFlashPanel, vgui::Frame);
	CFlashPanel();

private:
	vgui::Slider* m_pFlashSilder = nullptr;
	vgui::Slider* m_pWaterSilder = nullptr;

};