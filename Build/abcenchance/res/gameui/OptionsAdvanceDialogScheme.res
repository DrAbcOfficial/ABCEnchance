#base "GameUISchemeBase.res"

Scheme
{
	Colors
	{	
		"ControlText"		"216 216 216 255"	// used in all text controls
		"BrightControlText"	"233 233 233 255"	// use for selected controls
		"ControlLightBG"	"180 180 180 80"
		"ControlBG"			"125 125 125 125"
		"ControlDarkBG"		"80 80 80 200"
		
		"LabelDimText"		"192 192 192 30"	// used for info text
		
		// titlebar colors
		"TitleText"		"232 232 232 255"
		"TitleDimText"		"80 80 80 255"
		"TitleBG"		"50 50 50 180"
		"TitleDimBG"		"60 60 60 180"
		
		// border colors
		"BorderBright"		"Blank"	// the lit side of a control
		"BorderDark"		"Blank"		// the dark/unlit side of a control
		"BorderSelection"	"Blank"		// the additional border color for displaying the default/selected button
	}
	
	BaseSettings
	{
		FrameTitleBar.TextColor	"ControlText"
		FrameTitleBar.BgColor	"ControlBG"
		
		ModelViewer
		{
			BgColor		"Blank"
			SliderBgColor "Blank"
			NameTagBgColor "ControlLightBG"
		}
		
		Slider
		{
			SliderFgColor		"ControlDarkBG"
		}
		
		OptionsAdvanceDialog
		{
			BgColor				"Blank"
			SheetBgColor		"Blank"
		}
	}
	Borders
	{
		DialogBorder
		{
		}
		SheetBorder
		{
		}
		ButtonDepressedBorder
		{
			"inset" "2 1 1 1"
			Left
			{
				"1"
				{
					"color" "ControlDarkBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlLightBG"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlDarkBG"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlLightBG"
					"offset" "0 0"
				}
			}
		}
	}
}