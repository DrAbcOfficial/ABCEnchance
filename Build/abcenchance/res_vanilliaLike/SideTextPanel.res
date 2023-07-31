"abcenchance/res/SideTextPanel.res"
{
	"SidePanel"
	{
		"ControlName"		"CSidePanel"
		"fieldName"		"SidePanel"
		"xpos"		"0"
		"ypos"		"c-64"
		"wide"		"72"
		"tall"		"24"

		"autoResize"		"0"
		"pinCorner"		"0"

		"visible"		"0"
		"enabled"		"1"
		"tabPosition"		"0"
		//  左侧数值信息显示，修改template_text改变类型
		//  {MAXSPEED} Max speed
		//  {SPEED} Speed
		//  {SCORE} Score

		"template_text"		"${SCORE}"
	}
	"Message"
	{
		"ControlName"		"Label"
		"fieldName"		"Message"
		"xpos"		"12"
		"wide"		"60"
		"tall"		"24"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"wrap"			"1"
		"textAlignment" "center"
		"font"			"UiBold"
		"zpos"			"999"
	}
	"Image"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"Image"
		"wide"		"72"
		"tall"		"24"
		"visible"		"1"
		"tabPosition"		"0"
		"scaleImage"		"1"
		"image"			"abcenchance/tga/ecco_background"
		"drawColor"		"255 255 255 255"
	}
}
