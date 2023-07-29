"abcenchance/res/FlashLightPanel.res"
{
	"FlashLightPanel"
	{
		"ControlName"		"CFlashLightPanel"
		"fieldName"		"FlashLightPanel"
		"xpos"		"r55"
		"ypos"		"18"
		"wide"		"50"
		"tall"		"12"

		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"

		"fade_time"		"0.5"
		"icon_hue"		"170"
	}
	"Message"
	{
		"ControlName"		"Label"
		"fieldName"		"Message"
		"wide"		"26"
		"tall"		"12"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"textAlignment" "center"
		"font"			"DefaultSmall"
		"zpos"			"999"
	}
	"OffImage"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"OffImage"
		"xpos"		"27"
		"wide"		"23"
		"tall"		"12"
		"visible"		"1"
		"tabPosition"		"0"
		"scaleImage"		"1"
		"image"			"abcenchance/tga/flashlight_off"
		"drawColor"		"255 255 255 255"
		"zpos"			"998"
	}
	"OnImage"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"OnImage"
		"xpos"		"27"
		"wide"		"23"
		"tall"		"12"
		"visible"		"0"
		"tabPosition"		"0"
		"scaleImage"		"1"
		"image"			"abcenchance/tga/flashlight_on"
		"drawColor"		"255 255 255 255"
		"zpos"			"999"
	}
	"PowerImage"
	{
		"ControlName"		"ImageClipPanel"
		"fieldName"		"PowerImage"
		"xpos"		"28"
		"ypos"		"2"
		"wide"		"18"
		"tall"		"8"
		"visible"		"1"
		"tabPosition"		"0"
		"scaleImage"		"1"
		"image"			"abcenchance/tga/flashlight_power"
		"zpos"			"0"
	}
}
