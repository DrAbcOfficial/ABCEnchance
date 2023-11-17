"abcenchance/res/HealthPanel.res"
{
	"HealthPanel"
	{
		"ControlName"		"CHealthPanel"
		"fieldName"		"HealthPanel"
		"xpos"		"0"
		"ypos"		"rs1.0"
		"wide"		"300"
		"tall"		"30"

		"autoResize"		"1"
		"visible"		"1"
		"enabled"		"1"

		"icon_poison"	"abcenchance/tga/icon_poison"
		"icon_acid"	"abcenchance/tga/icon_acid"
		"icon_freeze"	"abcenchance/tga/icon_freeze"
		"icon_drown"	"abcenchance/tga/icon_drown"
		"icon_burn"	"abcenchance/tga/icon_burn"
		"icon_gas"	"abcenchance/tga/icon_gas"
		"icon_radiation"	"abcenchance/tga/icon_radiation"
		"icon_shock"	"abcenchance/tga/icon_shock"
	}

	"Background"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"Background"
		"xpos"		"0"
		"ypos"		"rs1.0"
		"wide"		"f0"
		"tall"		"f0"
		"visible"		"1"
		"scaleImage"		"1"
		"proportionalToParent" "1"
		"image"			""
		"drawColor"		"255 255 255 125"
	}

	"HealthIcon"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"HealthIcon"
		"xpos"		"6"
		"ypos"		"cs-0.5"
		"wide"		"15"
		"tall"		"15"
		"visible"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/healthbar_icon"
		"scaleImage"		"1"
		//生命值图标颜色
		"drawColor"		"SvenCoop"
	}

	"Health"
	{
		"ControlName"		"Label"
		"fieldName"		"Health"
		"xpos"		"21"
		"ypos"		"cs-0.5"
		"wide"		"48"
		"tall"		"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"0"
		"proportionalToParent" "1"
		"textAlignment"		"center"
		"font"		"ClassicHealthBar"
		"wrap"		"0"
	}

	"ArmorIcon"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ArmorIcon"
		"xpos"		"120"
		"ypos"		"cs-0.5"
		"wide"		"15"
		"tall"		"15"
		"visible"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/armorbar_icon"
		"scaleImage"		"1"
		//护甲图标颜色
		"drawColor"		"SvenCoop"
	}
	"Armor"
	{
		"ControlName"		"Label"
		"fieldName"		"Armor"
		"xpos"		"135"
		"ypos"		"cs-0.5"
		"wide"		"48"
		"tall"		"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"0"
		"proportionalToParent" "1"
		"textAlignment"		"center"
		"font"		"ClassicHealthBar"
		"wrap"		"0"
	}
	
	"Longjump"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"Longjump"
		"xpos"		"200"
		"ypos"		"cs-0.5"
		"wide"		"20"
		"tall"		"20"
		"visible"		"1"
		"proportionalToParent" "1"
		"scaleImage"		"1"
		"image"			"abcenchance/tga/longjump"
		//长跳图标颜色
		"drawColor"		"SvenCoop"
	}

	"DMGImages"
	{
		"ControlName"		"ListViewPanel"
		"fieldName"		"DMGImages"
		"xpos"		"4"
		"ypos"		"rs2.0"
		"wide"		"80"
		"tall"		"144"
		"visible"		"1"
		"enabled"		"1"
		"proportionalToParent" "1"
		"scaleImage"		"1"
	}
}
