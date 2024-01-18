//========= Copyright Š 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef KEYCODE2_H
#define KEYCODE2_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/KeyCode.h>

namespace vgui
{

	//-----------------------------------------------------------------------------
	// Purpose: this is a map for virtual key codes
	//			virtual key codes may exist outside this range for other languages
	//-----------------------------------------------------------------------------
	enum KeyCode2
	{
		BUTTON_CODE_INVALID = -1,
		BUTTON_CODE_NONE = 0,

		KEY_FIRST = 0,
		KEY_XBUTTON_UP = KEY_LAST,
		KEY_XBUTTON_DOWN,
		KEY_XBUTTON_LEFT,
		KEY_XBUTTON_RIGHT,
		KEY_XBUTTON_START,
		KEY_XBUTTON_BACK,
		KEY_XBUTTON_STICK1,
		KEY_XBUTTON_STICK2,
		KEY_XBUTTON_A,
		KEY_XBUTTON_B,
		KEY_XBUTTON_X,
		KEY_XBUTTON_Y,
		KEY_XBUTTON_BLACK,
		KEY_XBUTTON_WHITE,
		KEY_XBUTTON_LTRIGGER,
		KEY_XBUTTON_RTRIGGER,
		KEY_XSTICK1_UP,
		KEY_XSTICK1_DOWN,
		KEY_XSTICK1_LEFT,
		KEY_XSTICK1_RIGHT,
		KEY_XSTICK2_UP,
		KEY_XSTICK2_DOWN,
		KEY_XSTICK2_LEFT,
		KEY_XSTICK2_RIGHT
	};

	inline bool IsAlpha(KeyCode code)
	{
		return (code >= KEY_A) && (code <= KEY_Z);
	}

	inline bool IsAlphaNumeric(KeyCode code)
	{
		return (code >= KEY_0) && (code <= KEY_Z);
	}

	inline bool IsSpace(KeyCode code)
	{
		return (code == KEY_ENTER) || (code == KEY_TAB) || (code == KEY_SPACE);
	}

	inline bool IsKeypad(KeyCode code)
	{
		return (code >= KEY_PAD_0) && (code <= KEY_PAD_DECIMAL);
	}

	inline bool IsPunctuation(KeyCode code)
	{
		return (code >= KEY_0) && (code <= KEY_SPACE) && !IsAlphaNumeric(code) && !IsSpace(code) && !IsKeypad(code);
	}

	inline bool IsKeyCode(KeyCode code)
	{
		return (code >= KEY_FIRST) && (code <= KEY_LAST);
	}

}
#endif // KEYCODE2_H