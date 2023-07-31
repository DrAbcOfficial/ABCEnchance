#base "SchemeBase.res"

Scheme
{
	Fonts
	{
		"HealthBar"
		{
			"1"
			{
				"name"		"Liquid Crystal"
				"tall"		"24"
				"weight"	"700"
				"yres"		"480 599"
				"antialias"	"1"
			}
			"2"
			{
				"name"		"Liquid Crystal"
				"tall"		"28"
				"weight"	"700"
				"yres"		"600 767"
				"antialias"	"1"
			}
			"3"
			{
				"name"		"Liquid Crystal"
				"tall"		"30"
				"weight"	"700"
				"yres"		"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Liquid Crystal"
				"tall"		"32"
				"weight"	"700"
				"yres"		"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Liquid Crystal"
				"tall"		"42"
				"weight"	"700"
				"yres"		"1200 10000"
				"antialias"	"1"
			}
		}
	}
	BaseSettings
	{
		"DMGTiles.BgColor"		"Blank"
		//伤害类型图标颜色
		"DMGTiles.IconColor"		"SvenCoop"
		//伤害类型文本颜色
		"DMGTiles.TextColor"		"SvenCoop"
		"DMGTiles.ListViewBgColor"		"Blank"
		"HealthBar.BgColor"		"Blank"
		//生命值数值颜色
		"HealthBar.HealthFgColor"		"SvenCoop"
		//护甲数值颜色
		"HealthBar.ArmorFgColor"		"SvenCoop"
		//低生命值数值颜色
		"HealthBar.HealthDangerColor"		"255 0 0 255"
		//低护甲数值颜色
		"HealthBar.ArmorDangerColor"		"255 0 0 255"
	}

	CustomFontFiles
	{
		"1"		"abcenchance/font/LiquidCrystal-Bold.ttf"
	
	}
}
