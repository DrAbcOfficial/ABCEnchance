"abcenchance/res/VotePanel.res"
{
"VotePanel"
{
	"ControlName"		"CVotePanel"
	"fieldName"		"VotePanel"
	"xpos"	"c+127"
	"ypos"	"c-127"
	"wide"	"162"
	"tall"	"243"

	"autoResize"		"0"
	"pinCorner"		"0"

	"visible"		"0"
	"enabled"		"1"
	"tabPosition"		"0"

	"proportionalToParent" "1"
	"PaintBackgroundType"	"2"
}
"VoteContentPanel"
{
	"ControlName"		"Panel"
	"fieldName"		"VoteContentPanel"
	"ypos"	"14"
	"wide"	"162"
	"tall"	"229"
	"visible"		"1"
	
	"proportionalToParent" "1"
	"PaintBackgroundType"	"2"
}
"VoteTitle"
{
	"ControlName"		"Label"
	"fieldName"		"VoteTitle"
	"xpos"	"1"
	"ypos"	"1"
	"wide"	"159"
	"tall"	"13"
	"visible"		"1"
	"enabled"		"1"
	"labelText"		"#Vote_DefaultMessage"
	"textAlignment"		"west"
	"brighttext"		"0"
	"font"		"Default"
	"wrap"		"0"
}
"VoteContent"
{
	"ControlName"		"Label"
	"fieldName"		"VoteContent"
	"xpos"	"5"
	"ypos"	"18"
	"wide"	"151"
	"tall"	"191"
	"visible"		"1"
	"enabled"		"1"
	"labelText"		"(None)"
	"brighttext"		"0"
	"font"		"DefaultSmall"
	"wrap"		"1"
	"textAlignment"		"north-west"
}
"VoteYesText"
{
	"ControlName"		"Label"
	"fieldName"		"VoteYesText"
	"xpos"	"42"
	"ypos"	"193"
	"wide"	"133"
	"tall"	"13"
	"visible"		"1"
	"enabled"		"1"
	"labelText"		"(None)"
	"textAlignment"		"west"
	"brighttext"		"1"
	"font"		"Default"
	"wrap"		"0"
}
"VoteNoText"
{
	"ControlName"		"Label"
	"fieldName"		"VoteYesText"
	"xpos"	"42"
	"ypos"	"214"
	"wide"	"133"
	"tall"	"13"
	"visible"		"1"
	"enabled"		"1"
	"labelText"		"(None)"
	"textAlignment"		"west"
	"brighttext"		"1"
	"font"		"Default"
	"wrap"		"0"
}
"VoteYesIcon"
{
	"ControlName"		"ImagePanel"
	"fieldName"		"VoteYesIcon"
	"xpos"	"27"
	"ypos"	"193"
	"wide"	"13"
	"tall"	"13"
	"visible"		"1"
	"tabPosition"		"0"
	"scaleImage"		"1"
	"image"			"abcenchance/tga/vote_yes"
	"drawColor"		"255 255 255 255"
}
"VoteNoIcon"
{
	"ControlName"		"ImagePanel"
	"fieldName"		"VoteNoIcon"
	"xpos"	"27"
	"ypos"	"214"
	"wide"	"13"
	"tall"	"13"
	"visible"		"1"
	"tabPosition"		"0"
	"scaleImage"		"1"
	"image"			"abcenchance/tga/vote_no"
	"drawColor"		"255 255 255 255"
}
}
