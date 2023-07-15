//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <vgui/IInput.h>
#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <KeyValues.h>

#include <vgui_controls/Label.h>
#include <vgui_controls/LabelEx.h>
#include <vgui_controls/Image.h>
#include <vgui_controls/TextImageEx.h>
#include <vgui_controls/Controls.h>

using namespace vgui;

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(LabelEx, LabelEx);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LabelEx::LabelEx(Panel* parent, const char* panelName, const char* text) : BaseClass(parent, panelName, text){
	Init();
	if (_textImage)
		delete _textImage;
	_textImage = new TextImageEx(text);
	_textImage->SetColor(Color(0, 0, 0, 0));
	SetText(text);
	_textImageIndex = AddImage(_textImage, 0);

	REGISTER_COLOR_AS_OVERRIDABLE(_disabledFgColor2, "disabledfgcolor2_override");
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LabelEx::LabelEx(Panel* parent, const char* panelName, const wchar_t* wszText) : BaseClass(parent, panelName, wszText){
	Init();
	if (_textImage)
		delete _textImage;
	_textImage = new TextImageEx(wszText);
	_textImage->SetColor(Color(0, 0, 0, 0));
	SetText(wszText);
	_textImageIndex = AddImage(_textImage, 0);

	REGISTER_COLOR_AS_OVERRIDABLE(_disabledFgColor2, "disabledfgcolor2_override");
}