Scheme
{
	Colors
	{
		"HealthArmor.HealthIconColor"		"245 230 195 255"
		"HealthArmor.HealthBarColor"		"245 230 195 255"
		"HealthArmor.HealthTextColor"		"245 230 195 255"
		"HealthArmor.HealthPainColor"		"255 0 0 255"
		"HealthArmor.HealthDangerColor"		"250 0 0 255"

		"HealthArmor.ArmorIconColor"		"245 230 195 255"
		"HealthArmor.ArmorBarColor"			"245 230 195 255"
		"HealthArmor.ArmorTextColor"		"245 230 195 255"
		"HealthArmor.ArmorPainColor"		"255 0 0 255"
		"HealthArmor.ArmorDangerColor"		"125 110 0 255"

		"HealthArmor.PainIndicatorColor"	"255 0 0 255"
		"HealthArmor.PainIndicatorColorA"	"255 255 255 255"
		"HealthArmor.BitDamageColor"		"0 255 0 255"

		"AmmoHUD.Ammo1IconColor"		"245 230 195 255"
		"AmmoHUD.Ammo1BigTextColor"		"245 230 195 255"
		"AmmoHUD.Ammo1TextColor"		"245 230 195 255"
		"AmmoHUD.Ammo2IconColor"		"245 230 195 255"
		"AmmoHUD.Ammo2BigTextColor"			"245 230 195 255"
		"AmmoHUD.Ammo2TextColor"		"122 115 87 255"
		"AmmoHUD.AmmoPickUpIconColor"		"245 230 195 255"
		"AmmoHUD.AmmoPickUpTextColor"		"245 230 195 255"
		"AmmoHUD.WeaponPickUpColor"		"200 200 200 255"
		"AmmoHUD.WeaponPickUpEmptyColor"		"255 0 0 255"
		"AmmoHUD.ItemPickUpColor"		"245 230 195 255"

		"AmmoHUD.SelectCyclerColor"			"106 102 96 255"
		"AmmoHUD.SelectCyclerRinColor"		"245 230 195 255"
		"AmmoHUD.SelectCyclerIconColor"		"245 230 195 255"
		"AmmoHUD.SelectCyclerEmptyColor"	"255 0 0 255"
		"AmmoHUD.SelectCyclerTextColor"		"245 230 195 255"
		"AmmoHUD.SelectCyclerPointerColor"		"255 128 0 255"

		"DeathMsg.BackGoundColor"		"180 180 180 255"

		"Vote.OutlineColor"		"255 255 255 180"
		"Vote.BackGoundColor"		"139 137 137 180"
	}

	BaseSettings
	{
		"HealthArmor.StartX"			"48"
		"HealthArmor.IconSize"			"0.5"
		"HealthArmor.BarLength"			"2"
		"HealthArmor.BarWidth"			"0.334"
		"HealthArmor.ElementGap"		"0.2"
		"HealthArmor.BackGroundY"		"0.95"
		"HealthArmor.BackGroundLength"	"3"
		"HealthArmor.DamageIconX"		"0"
		"HealthArmor.DamageIconY"		"0"
		"HealthArmor.DamageIconSize"	"96"
		"HealthArmor.PainColorTime"		"0.5"

		"AmmoHUD.StartX"			"32"
		"AmmoHUD.IconSize"			"0.5"
		"AmmoHUD.ElementGap"		"0.2"
		"AmmoHUD.BackGroundY"		"0.95"
		"AmmoHUD.BackGroundLength"	"3"

		"AmmoHUD.AmmoPickUpDisplayCount"	"8"
		"AmmoHUD.AmmoPickUpX"	"-128"
		"AmmoHUD.AmmoPickUpY"	"64"
		"AmmoHUD.WeaponPickUpX"	"0"
		"AmmoHUD.WeaponPickUpY"	"0"
		"AmmoHUD.ItemPickUpX"	"128"
		"AmmoHUD.ItemPickUpY"	"64"

		"AmmoHUD.SelectCyclerOffset"	"256"
		"AmmoHUD.SelectCyclerSize"	"96"
		"AmmoHUD.SelectCyclerPointerSize"	"32"
		"AmmoHUD.SelectCyclerRotate"	"0"
		"AmmoHUD.SelectCyclerAnimateTime"	"0.12"
		"AmmoHUD.SelectCyclerFadeTime"	"0.2"
		"AmmoHUD.SelectCyclerHoldTime"	"5"

		"Radar.XOffset"			"32"
		"Radar.YOffset"			"32"
		"Radar.NorthPointerSize"	"24"
		"Radar.OutLineAlpha"	"255"
		"Radar.MapAlpha"		"255"
		"Radar.CenterAlpha"		"255"

		"DeathMsg.XOffset"		"64"
		"DeathMsg.YOffset"		"256"
		"DeathMsg.GapOffset"	"9"
		"DeathMsg.BackGoundWidth"	"3"

		"Vote.XOffset"		"128"
		"Vote.YOffset"		"256"
		"Vote.Height"		"3"
		"Vote.Width"		"5"
	}

	Fonts
	{
		"MainShitFont"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"10"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"16"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"0"
				"range"		"0x0000 0x017F"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}
		"HUDShitFont" // for HUDFont
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"700"
				"yres"		"480 599"
				"antialias"	"1"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"28"
				"weight"	"700"
				"yres"		"600 767"
				"antialias"	"1"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"30"
				"weight"	"700"
				"yres"		"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"32"
				"weight"	"700"
				"yres"		"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"42"
				"weight"	"700"
				"yres"		"1200 10000"
				"antialias"	"1"
			}
		}
		"HUDSmallShitFont" // for HUDSmallSizeFont
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"700"
				"yres"		"480 599"
				"antialias"	"1"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"700"
				"yres"		"600 767"
				"antialias"	"1"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"26"
				"weight"	"700"
				"yres"		"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"28"
				"weight"	"700"
				"yres"		"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"36"
				"weight"	"700"
				"yres"		"1200 10000"
				"antialias"	"1"
			}
		}
	}
}
