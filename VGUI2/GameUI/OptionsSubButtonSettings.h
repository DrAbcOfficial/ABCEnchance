//========= Copyright ?1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef OPTIONS_SUB_BUTTONSETTINGS_H
#define OPTIONS_SUB_BUTTONSETTINGS_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>
#include <vector>
#include <string>

#define TOUCH_FL_HIDE			(1U << 0)
#define TOUCH_FL_NOEDIT			(1U << 1)
#define TOUCH_FL_CLIENT			(1U << 2)
#define TOUCH_FL_MP				(1U << 3)
#define TOUCH_FL_SP				(1U << 4)
#define TOUCH_FL_DEF_SHOW		(1U << 5)
#define TOUCH_FL_DEF_HIDE		(1U << 6)
#define TOUCH_FL_DRAW_ADDITIVE	(1U << 7)
#define TOUCH_FL_STROKE			(1U << 8)
#define TOUCH_FL_PRECISION		(1U << 9)

//-----------------------------------------------------------------------------
// Purpose: Touch Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class COptionsSubButtonSettings : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsSubButtonSettings, vgui::PropertyPage);

public:
	COptionsSubButtonSettings(vgui::Panel *parent);
	~COptionsSubButtonSettings();
	void Com_EscapeCommand(char* newCommand, const char* oldCommand, int len);
	void UpdateProfileList();
	void UpdateButtonList();

	virtual void OnResetData();
	virtual void OnApplyChanges();
	virtual void OnCommand(const char* command);

private:
	MESSAGE_FUNC( OnControlModified, "ControlModified" );
	MESSAGE_FUNC_PTR_CHARPTR( OnTextChanged, "TextChanged", panel, text );
	MESSAGE_FUNC( OnTick, "Tick" );
	struct
	{
		char szName[128];
		char szTexture[128];
		char szCommand[128];
		byte bColors[4];
		int  iFlags;
	} buttons[1024];

	vgui::Slider* m_pButtonRSlider;
	vgui::Slider* m_pButtonGSlider;
	vgui::Slider* m_pButtonBSlider;
	vgui::Slider* m_pButtonASlider;

	vgui::CheckButton* m_pButtonHide;
	vgui::CheckButton* m_pButtonSp;
	vgui::CheckButton* m_pButtonMp;
	vgui::CheckButton* m_pButtonAdditive;
	vgui::CheckButton* m_pButtonPrecision;
	vgui::CheckButton* m_pButtonLock;

	vgui::Button* m_pButtonSave;
	vgui::Button* m_pButtonDelete;
	vgui::Button* m_pButtonEditor;
	vgui::Button* m_pButtonReset;
	vgui::Button* m_pProfileActivate;
	vgui::Button* m_pProfileDelete;

	vgui::TextEntry* m_pCommend;
	vgui::TextEntry* m_pTexture;
	vgui::TextEntry* m_pButtonName;

	vgui::ComboBox* m_pTextureList;
	vgui::ComboBox* m_pButtonList;
	vgui::ComboBox* m_pButtonProfileList;

	std::vector<std::string> m_vecProfiles;
};



#endif // OPTIONS_SUB_AUDIO_H