//
// CLIENT SCHEME RESOURCE FILE
//
// sections:
//		colors		- all the colors used by the scheme
//		basesettings	- contains settings for app to use to draw controls
//		fonts		- list of all the fonts used by app
//		borders		- description of all the borders
//
// notes:
// 		hit ctrl-alt-shift-R in the app to reload this file
//
Scheme
{
	//////////////////////// COLORS ///////////////////////////
	Colors
	{
		// base colors
		"Blank"				"0 0 0 0"
		
		"BaseText"		"200 200 200 255"	// used in text windows, lists
		"BrightBaseText"	"255 255 255 255"	// brightest text
		"SelectedText"		"255 255 255 255"	// selected text
		"DimBaseText"		"216 232 248 255"	// dim base text
		"LabelDimText"		"192 192 192 200"	// used for info text
		"ControlText"		"216 232 248 255"	// used in all text controls
		"BrightControlText"	"180 220 255 255"	// use for selected controls
		"DisabledText1"		"192 192 192 150"	// disabled text
		"DisabledText2"		"100 100 100 120"	// overlay color for disabled text (to give that inset look)
		"DimListText"		"115 115 115 255"	// offline friends, unsubscribed games, etc.

		// background colors
		"ControlBG"		"50 54 69 255"		// background color of controls
		"ControlDarkBG"		"43 47 60 255"		// darker background color; used for background of scrollbars
		"WindowBG"		"50 54 69 0"		// background color of text edit panes (chat, text entries, etc.)
		"SelectionBG"		"175 190 210 120"	// background color of any selected text or menu item
		"SelectionBG2"		"136 140 145 180"	// selection background in window w/o focus
		"ListBG"		"43 47 60 50"		// background of server browser, buddy list, etc.

		// titlebar colors
		"TitleText"		"216 232 248 255"
		"TitleDimText"		"125 130 150 255"
		"TitleBG"		"50 54 69 200"
		"TitleDimBG"		"50 54 69 200"

		// slider tick colors
		"SliderTickColor"	"255 255 255 255"
		"SliderTrackColor"	"100 100 100 255"

		// border colors
		"BorderBright"		"120 125 150 255"	// the lit side of a control
		"BorderDark"		"75 80 106 255"		// the dark/unlit side of a control
		"BorderSelection"	"0 0 0 255"		// the additional border color for displaying the default/selected button
	}

	///////////////////// BASE SETTINGS ////////////////////////
	// default settings for all panels
	// controls use these to determine their settings
	BaseSettings
	{
		"FgColor"			"ControlText"
		"BgColor"			"ControlBG"
		"LabelBgColor"			"TitleDimBG"
		"SubPanelBgColor"		"ControlBG"

		"DisabledFgColor1"		"DisabledText1"
		"DisabledFgColor2"		"DisabledText2"		// set this to the BgColor if you don't want it to draw

		"TitleBarFgColor"		"TitleText"
		"TitleBarDisabledFgColor"	"TitleDimText"
		"TitleBarBgColor"		"TitleBG"
		"TitleBarDisabledBgColor"	"TitleDimBG"

		"TitleBarIcon"			"resource/icon_sc"
		"TitleBarDisabledIcon"		"resource/icon_sc_disabled"

		"TitleButtonFgColor"		"BorderBright"
		"TitleButtonBgColor"		"ControlBG"
		"TitleButtonDisabledFgColor"	"TitleDimText"
		"TitleButtonDisabledBgColor"	"TitleDimBG"

		"TextCursorColor"		"BaseText"			// color of the blinking text cursor in text entries
		"URLTextColor"			"BrightBaseText"		// color that URL's show up in chat window

		Menu
		{
			"FgColor"			"DimBaseText"
			"BgColor"			"ControlBG"
			"ArmedFgColor"			"BrightBaseText"
			"ArmedBgColor"			"SelectionBG"
			"DividerColor"			"BorderDark"

			"TextInset"			"6"
		}

		MenuButton	  // the little arrow on the side of boxes that triggers drop down menus
		{
			"ButtonArrowColor"		"DimBaseText"		// color of arrows
		   	"ButtonBgColor"			"WindowBG"		// bg color of button. same as background color of text edit panes
			"ArmedArrowColor"		"BrightBaseText"	// color of arrow when mouse is over button
			"ArmedBgColor"			"DimBaseText"		// bg color of button when mouse is over button
		}

		Slider
		{
			"SliderFgColor"			"ControlBG"		// handle with which the slider is grabbed
			"SliderBgColor"			"ControlDarkBG"		// area behind handle
		}

		ScrollBarSlider
		{
			"BgColor"			"ControlBG"		// this isn't really used
			"ScrollBarSliderFgColor"	"ControlBG"		// handle with which the slider is grabbed
			"ScrollBarSliderBgColor"	"ControlDarkBG"		// area behind handle
			"ButtonFgColor"			"DimBaseText"		// color of arrows
		}


		// text edit windows
		"WindowFgColor"			"BaseText"		// off-white
		"WindowBgColor"			"WindowBG"		// redundant. can we get rid of WindowBgColor and just use WindowBG?
		"WindowDisabledFgColor"		"DimBaseText"
		"WindowDisabledBgColor"		"ListBG"		// background of chat conversation
		"SelectionFgColor"		"SelectedText"		// fg color of selected text
		"SelectionBgColor"		"SelectionBG"
		"ListSelectionFgColor"		"SelectedText"
		"ListBgColor"			"ListBG"		// background of server browser control, etc
		"BuddyListBgColor"		"ListBG"		// background of buddy list pane

		// App-specific stuff
		"ChatBgColor"			"WindowBG"

		// status selection
		"StatusSelectFgColor"		"BrightBaseText"
		"StatusSelectFgColor2"		"BrightControlText"	// this is the color of the friends status

		// checkboxes
		"CheckButtonBorder1"   		"BorderDark"		// the left checkbutton border
		"CheckButtonBorder2"   		"BorderBright"		// the right checkbutton border
		"CheckButtonCheck"		"BrightControlText"	// color of the check itself
		"CheckBgColor"			"ListBG"

		// buttons (default fg/bg colors are used if these are not set)
//		"ButtonArmedFgColor"
//		"ButtonArmedBgColor"
//		"ButtonDepressedFgColor"	"BrightControlText"
//		"ButtonDepressedBgColor"

		// buddy buttons
		BuddyButton
		{
			"FgColor1"			"ControlText"
			"FgColor2"			"DimListText"
			"ArmedFgColor1"			"BrightBaseText"
			"ArmedFgColor2"			"BrightBaseText"
			"ArmedBgColor"			"SelectionBG"
		}

		Chat
		{
			"TextColor"			"BrightControlText"
			"SelfTextColor"			"BaseText"
			"SeperatorTextColor"		"DimBaseText"
		}

		InGameDesktop
		{
			"MenuColor"			"180 220 255 255"
			"ArmedMenuColor"		"255 255 255 255"
			"DepressedMenuColor"		"216 232 248 255"
			"WidescreenBarColor" "0 0 0 0"
			"MenuItemVisibilityRate"	"1.0"			// time it takes for one menu item to appear
			"MenuItemHeight"		"80"
			"GameMenuInset"			"120"
		}

		"SectionTextColor"		"BrightControlText"	// text color for IN-GAME, ONLINE, OFFLINE sections of buddy list
		"SectionDividerColor"		"BorderDark"		// color of line that runs under section name in buddy list
	}

	//
	//////////////////////// FONTS /////////////////////////////
	//
	// describes all the fonts
	Fonts
	{
		// fonts are used in order that they are listed
		// fonts listed later in the order will only be used if they fulfill a range not already filled
		// if a font fails to load then the subsequent fonts will replace
		"Default"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"16"
				"weight"	"0"
			}
		}
		"DefaultBold"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"16"
				"weight"	"500"
			}
		}
		"DefaultUnderline"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"16"
				"weight"	"0"
				"underline"	"1"
			}
		}
		"DefaultSmall"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"13"
				"weight"	"0"
			}
		}
		"DefaultSmallBold"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"12"
				"weight"	"600"
			}
		}
		"DefaultVerySmall"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"12"
				"weight"	"0"
			}
		}
		"MenuLarge"
		{
			"1"
			{
				"name"		"Michroma"
				"tall"		"24"
				"weight"	"1000"
				"antialias"	"1"
			}
		}
		"UiHeadline"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"16"
				"weight"	"1000"
				"antialias"	"0"
			}
		}

		// this is the symbol font
		"Marlett"
		{
			"1"
			{
				"name"		"Marlett"
				"tall"		"14"
				"weight"	"0"
				"symbol"	"1"
			}
		}

		"EngineFont"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"600"
				"yres"		"480 599"
				"dropshadow"	"1"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"13"
				"weight"	"600"
				"yres"		"600 767"
				"dropshadow"	"1"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"600"
				"yres"		"768 1023"
				"dropshadow"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"600"
				"yres"		"1024 1199"
				"dropshadow"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"600"
				"yres"		"1200 6000"
				"dropshadow"	"1"
			}
		}

		"CreditsFont"
		{
			"1"
			{
				"name"		"Ariel"
				"tall"		"12"
				"weight"	"600"
				"antialias"	"1"
			}
		}

		"Legacy_CreditsFont" // Added to accommodate 3rd party server plug-ins, etc. This version should not scale.
		{
			"1"
			{
				"name"		"Ariel"
				"tall"		"20"
				"weight"	"700"
				"antialias"	"1"
				"yres"		"1 10000"
			}
		}

		"EngineFontMono"
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"14"
				"weight"	"600"
				"yres"		"480 599"
				"dropshadow"	"1"
			}
			"2"
			{
				"name"		"Courier New"
				"tall"		"15"
				"weight"	"600"
				"yres"		"600 767"
				"dropshadow"	"1"
			}
			"3"
			{
				"name"		"Courier New"
				"tall"		"16"
				"weight"	"600"
				"yres"		"768 1023"
				"dropshadow"	"1"
			}
			"4"
			{
				"name"		"Courier New"
				"tall"		"20"
				"weight"	"600"
				"yres"		"1024 1199"
				"dropshadow"	"1"
			}
			"5"
			{
				"name"		"Courier New"
				"tall"		"24"
				"weight"	"600"
				"yres"		"1200 6000"
				"dropshadow"	"1"
			}
		}

		"Legacy_EngineFontMono"
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"16"
				"weight"	"600"
				"dropshadow"	"1"
			}
		}

		"GameConsole_Mono"
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"16"
				"weight"	"0"
			}
		}
	}


	//
	//////////////////// BORDERS //////////////////////////////
	//
	// describes all the border types
	Borders
	{
		// references to other borders
		BaseBorder		"InsetBorder"
		ComboBoxBorder		"InsetBorder"
		BrowserBorder		"InsetBorder"
		ButtonBorder		"RaisedBorder"
		FrameBorder		"RaisedBorder"
		TabBorder		"RaisedBorder"
		MenuBorder		"RaisedBorder"

		// standard borders
		InsetBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		RaisedBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		// special border types
		TitleButtonBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"4"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		TitleButtonDisabledBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BgColor"
					"offset" "1 0"
				}
			}
			Top
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 0"
				}
			}
		}

		TitleButtonDepressedBorder
		{
			"inset" "1 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		ScrollBarButtonBorder
		{
			"inset" "1 0 0 0"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		ScrollBarButtonDepressedBorder
		{
			"inset" "2 2 0 0"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		ButtonBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		FrameBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		TabBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		TabActiveBorder
		{
			"inset" "0 0 1 0"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "6 2"
				}
			}
		}


		ToolTipBorder
		{
			"inset" "0 0 1 0"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		// this is the border used for default buttons (the button that gets pressed when you hit enter)
		ButtonKeyFocusBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}
			Top
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}
			Right
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}
			Bottom
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderDark"
					"offset" "1 1"
				}
			}
		}

		ButtonDepressedBorder
		{
			"inset" "2 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		ComboBoxBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		MenuBorder
		{
			"inset" "1 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}
		BrowserBorder
		{
			"inset" "0 0 0 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}
	}
}