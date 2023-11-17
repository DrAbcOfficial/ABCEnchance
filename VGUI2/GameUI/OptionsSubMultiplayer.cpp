#include "metahook.h"
#include <vgui_controls/BitmapImagePanel.h>
#include "GameUI/OptionsSubMultiplayer.h"
#include "GameUI/MultiplayerAdvancedDialog.h"
#include <stdio.h>

#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>
#include "tier1/KeyValues.h"
#include <vgui_controls/Label.h>
#include <vgui/ISystem.h>
#include <vgui/ISurface.h>
#include <vgui/Cursor.h>
#include <vgui_controls/RadioButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/FileOpenDialog.h>
#include <vgui_controls/MessageBox.h>
#include <vgui/IVgui.h>
#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <vgui_controls/MessageBox.h>

#include <vguilocal.h>

#include "GameUI/CvarTextEntry.h"
#include "GameUI/CvarToggleCheckButton.h"
#include "GameUI/CvarSlider.h"
#include "GameUI/LabeledCommandComboBox.h"
#include "FileSystem.h"
#include "BitmapImagePanel.h"
#include "UtlBuffer.h"
#include "GameUI/ModInfo.h"
#include "GameUI/LogoFile.h"
#include "GameUI/BMPUtils.h"

#include <memory>

struct ColorItem_t
{
	const char *name;
	int r, g, b;
};

static ColorItem_t itemlist[] =
{
	{ "#Valve_Orange", 255, 120, 24 },
	{ "#Valve_Yellow", 225, 180, 24 },
	{ "#Valve_Blue", 0, 60, 255 },
	{ "#Valve_Ltblue", 0, 167, 255 },
	{ "#Valve_Green", 0, 167, 0 },
	{ "#Valve_Red", 255, 43, 0 },
	{ "#Valve_Brown", 123, 73, 0 },
	{ "#Valve_Ltgray", 100, 100, 100 },
	{ "#Valve_Dkgray", 36, 36, 36 },
};

COptionsSubMultiplayer::COptionsSubMultiplayer(vgui::Panel *parent) : vgui::PropertyPage(parent, "OptionsSubMultiplayer")
{
	m_pCancelButton = new vgui::Button(this, "Cancel", "#GameUI_Cancel", this, "Close");
	m_pOkButton = new vgui::Button(this, "OK", "#GameUI_OK", this, "Ok");
	m_pApplyButton = new vgui::Button(this, "Apply", "#GameUI_Apply", this, "Apply");

	m_pNameTextEntry = new CCvarTextEntry(this, "NameEntry", "name");

	if(parent)
		SetSize(parent->GetWide(), parent->GetTall());

	LoadControlSettings(VGUI2_ROOT_DIR "OptionsSubMultiplayer.res");
}

COptionsSubMultiplayer::~COptionsSubMultiplayer(void)
{
}

void COptionsSubMultiplayer::OnCommand(const char *command)
{
	BaseClass::OnCommand(command);
}

void COptionsSubMultiplayer::OnTextChanged(vgui::Panel *panel)
{
	if (panel == m_pNameTextEntry)
		return;
}

void COptionsSubMultiplayer::OnSliderMoved(KeyValues *data)
{
}

void COptionsSubMultiplayer::OnApplyButtonEnable(void)
{
	PostMessage(GetParent(), new KeyValues("ApplyButtonEnable"));
	InvalidateLayout();
}

void COptionsSubMultiplayer::OnPageShow(void)
{
}

void COptionsSubMultiplayer::OnResetData(void)
{
	m_pNameTextEntry->Reset();
	m_pNameTextEntry->GotoTextEnd();
}

void COptionsSubMultiplayer::OnApplyChanges(void)
{
	m_pNameTextEntry->ApplyChanges();

	
}
vgui::Panel *COptionsSubMultiplayer::CreateControlByName(const char *controlName)
{
	return BaseClass::CreateControlByName(controlName);
}