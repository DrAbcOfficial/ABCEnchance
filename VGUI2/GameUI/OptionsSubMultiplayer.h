#ifndef OPTIONSSUBMULTIPLAYER_H
#define OPTIONSSUBMULTIPLAYER_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>

class CLabeledCommandComboBox;

class CCvarToggleCheckButton;
class CCvarTextEntry;
class CCvarSlider;

class CMultiplayerAdvancedDialog;

namespace vgui {
	class CBitmapImagePanel;
}

class COptionsSubMultiplayer : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsSubMultiplayer, vgui::PropertyPage);

public:
	COptionsSubMultiplayer(vgui::Panel *parent);
	~COptionsSubMultiplayer(void);

public:
	virtual vgui::Panel *CreateControlByName(const char *controlName);
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;

protected:
	virtual void OnPageShow(void);
	virtual void OnResetData(void);
	virtual void OnApplyChanges(void);
	virtual void OnCommand(const char *command);

private:
	MESSAGE_FUNC_PTR(OnTextChanged, "TextChanged", panel);
	MESSAGE_FUNC_PARAMS(OnSliderMoved, "SliderMoved", data);
	MESSAGE_FUNC(OnApplyButtonEnable, "ControlModified");

private:

private:
	CCvarTextEntry *m_pNameTextEntry;
	vgui::Button* m_pCancelButton;
	vgui::Button* m_pOkButton;
	vgui::Button* m_pApplyButton;
};

#endif