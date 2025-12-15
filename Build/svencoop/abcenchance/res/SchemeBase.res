Scheme
{
	Colors
	{
		"Default"			"0 255 0 255"
		"DefaultDark"		"0 120 0 255"
		"DefaultTransparent"	"0 255 0 125"
		// base colors
		"White"				"255 255 255 255"
		"OffWhite"			"216 216 216 255"
		"DullWhite"			"142 142 142 255"
		
		"Orange"			"255 176 0 255"
		"OrangeDim"			"255 176 0 120"
		"LightOrange"		"188 112 0 128"
		
		"TransparentBlack"		"0 0 0 128"
		"Black"				"0 0 0 255"

		"Blank"				"0 0 0 0"

		"Red"				"255 0 0 255"
		
		// Color code colors
		"ColorCode1"		"255 0 0 255"
		"ColorCode2"		"0 255 0 255"
		"ColorCode3"		"255 255 0 255"
		"ColorCode4"		"0 0 255 255"
		"ColorCode5"		"0 255 255 255"
		"ColorCode6"		"255 0 255 255"
		"ColorCode7"		"136 136 136 255"
		"ColorCode8"		"255 255 255 255"

		"SvenCoop"			"100 130 200 255"
		"SvenCoopDark"			"60 90 160 255"
		
		// Killfeed (deathnotice) colors
		"KF_Background"			"0 0 0 64"
		"KF_Background_Kill"	"255 119 0 96"
		"KF_Background_Death"	"255 0 0 96"
		"KF_Icon"				"255 80 0 255"
		"KF_Icon_Kill"			"241 237 212 255"
		"KF_Icon_Death"			"241 237 212 255"
		"KF_Icon_Teamkiill"		"10 240 10 255"
	}
	BaseSettings
	{
		"ThisPlayerBgColor"			"LightOrange"
		"KillerBgColor"				"255 0 0 128"
	
		// vgui_controls color specifications
		Border.Bright					"200 200 200 196"	// the lit side of a control
		Border.Dark						"40 40 40 196"		// the dark/unlit side of a control
		Border.Selection				"0 0 0 196"			// the additional border color for displaying the default/selected button

		Button.TextColor				"White"
		Button.BgColor					"Blank"
		Button.ArmedTextColor			"White"
		Button.ArmedBgColor				"Blank"
		Button.DepressedTextColor		"White"
		Button.DepressedBgColor			"Blank"
		Button.FocusBorderColor			"Black"
		
		CheckButton.TextColor			"OffWhite"
		CheckButton.SelectedTextColor	"White"
		CheckButton.BgColor				"TransparentBlack"
		CheckButton.Border1  			"Border.Dark" 		// the left checkbutton border
		CheckButton.Border2  			"Border.Bright"		// the right checkbutton border
		CheckButton.Check				"White"				// color of the check itself

		ComboBoxButton.ArrowColor		"DullWhite"
		ComboBoxButton.ArmedArrowColor	"White"
		ComboBoxButton.BgColor			"Blank"
		ComboBoxButton.DisabledBgColor	"Blank"

		Frame.TitleTextInsetX			16
		Frame.ClientInsetX				8
		Frame.ClientInsetY				6
		//Frame.BgColor					"160 160 160 128"
		Frame.BgColor					"0 0 0 64"
		Frame.OutOfFocusBgColor			"0 0 0 32"
		Frame.FocusTransitionEffectTime	"0.3"	// time it takes for a window to fade in/out on focus/out of focus
		Frame.TransitionEffectTime		"0.3"	// time it takes for a window to fade in/out on open/close
		Frame.AutoSnapRange				"0"
		FrameGrip.Color1				"200 200 200 196"
		FrameGrip.Color2				"0 0 0 196"
		FrameTitleButton.FgColor		"200 200 200 196"
		FrameTitleButton.BgColor		"Blank"
		FrameTitleButton.DisabledFgColor	"255 255 255 192"
		FrameTitleButton.DisabledBgColor	"Blank"
		FrameSystemButton.FgColor		"Blank"
		FrameSystemButton.BgColor		"Blank"
		FrameSystemButton.Icon			""
		FrameSystemButton.DisabledIcon	""
		FrameTitleBar.Font				"UiBold"
		FrameTitleBar.TextColor			"White"
		FrameTitleBar.BgColor			"Blank"
		FrameTitleBar.DisabledTextColor	"255 255 255 192"
		FrameTitleBar.DisabledBgColor	"Blank"

		GraphPanel.FgColor				"White"
		GraphPanel.BgColor				"TransparentBlack"

		Label.TextDullColor				"DullWhite"
		Label.TextColor					"OffWhite"
		Label.TextBrightColor			"White"
		Label.SelectedTextColor			"White"
		Label.BgColor					"Blank"
		Label.DisabledFgColor1			"Blank"
		Label.DisabledFgColor2			"102 102 102 255"

		ListPanel.TextColor					"OffWhite"
		ListPanel.TextBgColor				"Blank"
		ListPanel.BgColor					"TransparentBlack"
		ListPanel.SelectedTextColor			"Black"
		ListPanel.SelectedBgColor			"Orange"
		ListPanel.SelectedOutOfFocusBgColor	"255 155 0 128"
		ListPanel.EmptyListInfoTextColor	"OffWhite"

		Menu.TextColor					"White"
		Menu.BgColor					"160 160 160 128"
		Menu.ArmedTextColor				"Black"
		Menu.ArmedBgColor				"Orange"
		Menu.TextInset					"6"

		Panel.FgColor					"DullWhite"
		Panel.BgColor					"Transparent"

		ProgressBar.FgColor				"White"
		ProgressBar.BgColor				"TransparentBlack"

		PropertySheet.TextColor			"OffWhite"
		PropertySheet.SelectedTextColor	"White"
		PropertySheet.TransitionEffectTime	"0.25"	// time to change from one tab to another

		RadioButton.TextColor			"DullWhite"
		RadioButton.SelectedTextColor	"White"

		RichText.TextColor				"OffWhite"
		RichText.BgColor				"TransparentBlack"
		RichText.SelectedTextColor		"Black"
		RichText.SelectedBgColor		"Orange"

		ScrollBar.Wide					17

		ScrollBarButton.FgColor				"White"
		ScrollBarButton.BgColor				"Blank"
		ScrollBarButton.ArmedFgColor		"White"
		ScrollBarButton.ArmedBgColor		"Blank"
		ScrollBarButton.DepressedFgColor	"White"
		ScrollBarButton.DepressedBgColor	"Blank"

		ScrollBarSlider.FgColor				"Blank"			// nob color
		ScrollBarSlider.BgColor				"255 255 255 64"	// slider background color

		SectionedListPanel.HeaderTextColor	"White"
		SectionedListPanel.HeaderBgColor	"Blank"
		SectionedListPanel.DividerColor		"Blank"
		SectionedListPanel.TextColor		"DullWhite"
		SectionedListPanel.BrightTextColor	"White"
		SectionedListPanel.BgColor			"TransparentBlack"
		SectionedListPanel.SelectedTextColor			"Black"
		SectionedListPanel.SelectedBgColor				"Orange"
		SectionedListPanel.OutOfFocusSelectedTextColor	"Black"
		SectionedListPanel.OutOfFocusSelectedBgColor	"255 155 0 128"

		ScorePanel.BgColor		"Blank"

		Slider.NobColor				"108 108 108 255"
		Slider.TextColor			"180 180 180 255"
		Slider.TrackColor			"31 31 31 255"
		Slider.DisabledTextColor1	"117 117 117 255"
		Slider.DisabledTextColor2	"30 30 30 255"

		TextEntry.TextColor			"OffWhite"
		TextEntry.BgColor			"TransparentBlack"
		TextEntry.CursorColor		"OffWhite"
		TextEntry.DisabledTextColor	"DullWhite"
		TextEntry.DisabledBgColor	"Blank"
		TextEntry.SelectedTextColor	"Black"
		TextEntry.SelectedBgColor	"Orange"
		TextEntry.OutOfFocusSelectedBgColor	"255 155 0 128"
		TextEntry.FocusEdgeColor	"0 0 0 196"

		ToggleButton.SelectedTextColor	"White"

		Tooltip.TextColor			"0 0 0 196"
		Tooltip.BgColor				"Orange"

		TreeView.BgColor			"TransparentBlack"

		WizardSubPanel.BgColor		"Blank"

		// scheme-specific colors
		MainMenu.TextColor			"White"
		MainMenu.ArmedTextColor		"200 200 200 255"
		MainMenu.DepressedTextColor	"192 186 80 255"
		MainMenu.MenuItemHeight		"30"
		MainMenu.Inset				"32"
		MainMenu.Backdrop			"0 0 0 156"

		Console.TextColor			"OffWhite"
		Console.DevTextColor		"White"

		NewGame.TextColor			"White"
		NewGame.FillColor			"0 0 0 255"
		NewGame.SelectionColor		"Orange"
		NewGame.DisabledColor		"128 128 128 196"
		
		
		"AmmoBar.BgColor"			"Blank"
		//主要弹药数值颜色
		"AmmoBar.Ammo1FgColor"		"Default"
		//次要子弹数值颜色
		"AmmoBar.Ammo2FgColor"		"Default"
		//主次弹药分划线颜色
		"AmmoBar.SlashFgColor"		"255 0 0 255"
		//主要弹药图标颜色
		"AmmoBar.Ammo1IconColor"	"Default"
		//次要弹药图标颜色
		"AmmoBar.Ammo2IconColor"	"Default"
		
		//拾取子弹背景颜色
		"AmmoStack.BgColor"				"Blank"
		//拾取子弹行颜色
		"AmmoStack.ListViewBgColor"		"Blank"
		//拾取子弹图标颜色
		"AmmoStack.IconColor"			"Default"
		//拾取子弹文字颜色
		"AmmoStack.TextColor"			"Default"
		"AmmoStack.Font"				"Default"

		//拾取物品背景颜色
		"ItemStack.BgColor"				"Blank"
		//拾取物品行颜色
		"ItemStack.ListViewBgColor"		"Blank"
		//拾取物品图标颜色
		"ItemStack.IconColor"			"Default"

		//拾取武器背景颜色
		"WeaponStack.BgColor"			"Blank"
		//拾取武器行颜色
		"WeaponStack.ListViewBgColor"	"Blank"
		//拾取武器图标颜色
		"WeaponStack.IconColor"			"Default"
		//拾取无子弹武器图标颜色
		"WeaponStack.IconEmptyColor"			"Red"

		//选择武器背景颜色
		"WeaponChoose.BgColor"			"Blank"
		//选择武器行颜色
		"WeaponChoose.ItemBgColor"	"Blank"
		//选择武器图标颜色
		"WeaponChoose.ActiveColor"			"Default"
		"WeaponChoose.InactiveColor"			"DefaultTransparent"
		//选择无子弹武器图标颜色
		"WeaponChoose.EmptyColor"			"Red"
		"WeaponChoose.EmptyInactiveColor"			"125 0 0 80"
		//选择武器子弹条背景颜色
		"WeaponChoose.AmmoBarBgColor"			"80 80 80 125"
		//选择武器子弹条颜色
		"WeaponChoose.AmmoBarColor"			"255 255 0 125"
		//选择武器子弹条颜色
		"WeaponChoose.AmmoBarEmptyColor"			"Red"
		"WeaponChoose.SlotColor"			"DefaultTransparent"
		"WeaponChoose.BucketColor"			"255 255 0 255"

		
		Console.TextColor			"255 255 255 200"
		Console.ErrorTextColor			"255 0 0 200"
		Console.WarnColor			"255 255 0 200"
		Console.DevTextColor		"180 180 180 200"
		Console.BgColor			"125 125 125 125"
		Console.FgColor		"0 0 0 125"
		Console.TextAreaColor		"0 0 0 60"
		Console.ButtonColor		"125 125 125 125"

		"FrameTitleButton.FgColor"	"255 255 255 255"
		"FrameTitleButton.BgColor"	"125 125 125 125"
		
		"DeathMsgPanel.BgColor"					"Blank"
		//击杀信息提示框颜色
		"DeathMsgPanel.ItemBgColor"				"125 125 125 125"
		//击杀者文本颜色
		"DeathMsgPanel.ItemAttackerColor"		"255 0 0 255"
		//击杀事件文本颜色
		"DeathMsgPanel.ItemInflictorColor"		"White"
		//被击杀者文本颜色
		"DeathMsgPanel.ItemVictimColor"			"100 200 130 255"
		"DeathMsgPanel.Font"			"Default"
		
		"EffectPanel.BgColor"		"Blank"
		"EffectPanel.DangerColor"	"Red"
		"EffectPanel.UnderWaterColor"	"80 80 80 255"
		
		"FlashLight.BgColor"		"Blank"
		"FlashLight.TextBgColor"		"Blank"
		//手电筒百分比颜色
		"FlashLight.TextFgColor"		"Default"
		//手电筒电量图标颜色
		"FlashLight.IconFgColor"	"DefaultTransparent"
		
		"GIndicator.BgColor"			"Blank"
		//手雷指示器图标颜色
		"GIndicator.DrawColor"		"DefaultTransparent"
		
		"DMGTiles.BgColor"		"Blank"
		//伤害类型图标颜色
		"DMGTiles.IconColor"		"Default"
		//伤害类型文本颜色
		"DMGTiles.TextColor"		"Default"
		"DMGTiles.ListViewBgColor"		"Blank"
		"DMGTiles.Font"			"Default"
		
		"HealthBar.BgColor"		"Blank"
		//生命图标、条、条背景、数值、低数值条颜色
		"HealthBar.HealthIconFgColor"		"Default"
		"HealthBar.HealthBarFgColor"		"Default"
		"HealthBar.HealthBarBgColor"		"255 255 255 125"
		"HealthBar.HealthFgColor"		"Default"
		"HealthBar.HealthDangerColor"		"Red"
		//护甲图标、条、条背景、数值、低数值条颜色
		"HealthBar.ArmorIconFgColor"		"Default"
		"HealthBar.ArmorBarFgColor"		"Default"
		"HealthBar.ArmorBarBgColor"		"255 255 255 125"
		"HealthBar.ArmorFgColor"		"Default"
		"HealthBar.ArmorDangerColor"		"Red"
		//长跳包图片颜色
		"HealthBar.LongJumpIconFgColor"		"Default"
		//背景图片颜色
		"HealthBar.ImageBgColor"		"255 255 255 255"
		
		//Motd背景颜色
		"Motd.BgColor"		"0 0 0 64"
		"Motd.MessageBgColor"		"Blank"
		//Motd进度条背景颜色
		"Motd.ProgressBgColor"		"0 112 188 128"
		
		//Music文字
		"Music.MusicFgColor"		"255 255 255 255"
		//Artitst文字
		"Music.ArtistFgColor"		"200 200 200 255"
		//Number文字
		"Music.NumberFgColor"		"255 255 255 255"
		//Lyric文字
		"Music.LyricFgColor"		"255 255 255 255"
		//TransLyric文字
		"Music.TransLyricFgColor"		"200 200 200 255"
		//Lyric播放文字
		"Music.LyricHighLightFgColor"		"Default"
		//TransLyric播放文字
		"Music.TransLyricHighLightFgColor"		"DefaultDark"
		//投票菜单背景颜色
		"Music.BackGoundColor"		"80 80 80 60"
		"Music.ProgressFgColor"		"255 255 255 255"
		"Music.ProgressBgColor"		"255 255 255 255"
		"Music.AlbumImageColor"		"255 255 255 255"
		
		"Notice.BgColor"		"Blank"
		"Notice.TextBgColor"		"Blank"
		//HUD文字信息颜色
		"Notice.TextFgColor"		"Default"
		
		"PlayerBoardPanel.BgColor"	"Blank"
		"PlayerBoardName.BgColor"		"Blank"
		
		//背景
		"PopNumber.BackGoundColor"		"Blank"
		
		"SideText.BgColor"		"Blank"
		"SideText.TextBgColor"		"Blank"
		//左侧数值颜色
		"SideText.TextFgColor"		"Default"
		
		"TextMenu.BgColor"		"0 0 0 64"
		//左侧菜单栏文字颜色
		"TextMenu.TextColor"	"222 222 222 222"
		//菜单字体
		"TextMenu.Font"			"TextMenu"
		
		//标题背景
		"Vote.TitleBgColor"		"0 0 0 0"
		//标题文字
		"Vote.TitleFgColor"		"255 255 255 255"
		//正文文字
		"Vote.ContentFgColor"		"255 255 255 255"
		"Vote.ContentBgColor"		"0 0 0 64"
		//Yes文字
		"Vote.YesFgColor"		"255 255 255 255"
		//No文字
		"Vote.NoFgColor"		"255 255 255 255"
		//投票菜单背景颜色
		"Vote.BackGoundColor"		"Blank"
		
		"Radar.OutlineColor"		"255 255 255 255"
		"Radar.MapColor"		"255 255 255 255"
		"Radar.FgColor"			"255 255 255 255"
		//雷达背景颜色
		"Radar.BgColor"		"Blank"
		//雷达清除颜色 (RGBA格式)
		"Radar.ClearColor"		"0 0 0 255"
		
		"Indicator.FgColor"		"255 0 0 255"
		"Indicator.BgColor"		"Blank"
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
		// fonts are used in order that they are listed
		"Default"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"600"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"13"
				"weight"	"600"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"600"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"600"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"600"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}
		"DefaultUnderline"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"12"
				"weight"	"500"
				"underline" "1"
			}
		}
		"DefaultSmall"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"0"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"13"
				"weight"	"0"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"0"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"0"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"0"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}
		"DefaultVerySmall"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"600"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"13"
				"weight"	"600"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"600"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"600"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"600"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}
		// Used by scoreboard and spectator UI for names which don't map in the normal fashion
		"DefaultVerySmallFallBack"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"10"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"yres"		"480 599"
				"antialias"	"1"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"yres"		"600 1199"
				"antialias"	"1"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"15"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"yres"		"1200 6000"
				"antialias"	"1"
			}
		}
		"UiBold"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"10"
				"weight"	"1000"
			}
		}
		"Title"
		{
			"1"
			{
				"name"		"Verdana Bold"
				"tall"		"18"
				"weight"	"500"
			}
			"2"
			{
				"name"		"Arial"
				"tall"		"16"
				"weight"	"600"
			}
		}
		"TitleSmall"
		{
			"1"
			{
				"name"		"Verdana Bold"
				"tall"		"14"
				"weight"	"500"
			}
			"2"
			{
				"name"		"Arial"
				"tall"		"12"
				"weight"	"600"
			}
		}
		// Credits ------------------------------------------------
		"CreditsTitle"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"26"
				"weight"	"800"
			}
			"2"
			{
				"name"		"Arial"
				"tall"		"26"
				"weight"	"700"
			}
		}
		"CreditsStudioTitle"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"24"
				"weight"	"800"
			}
			"2"
			{
				"name"		"Arial"
				"tall"		"24"
				"weight"	"700"
			}
		}
		"CreditsIndividualTitle"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"22"
				"weight"	"800"
			}
			"2"
			{
				"name"		"Arial"
				"tall"		"22"
				"weight"	"700"
			}
		}
		"CreditsNormal"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"20"
				"weight"	"800"
			}
			"2"
			{
				"name"		"Arial"
				"tall"		"20"
				"weight"	"700"
			}
		}
		// --------------------------------------------------------
		"DialogTitle"
		{
			"1"
			{
				"name"		"Verdana Bold"
				"tall"		"14"
				"weight"	"500"
			}
			"2"
			{
				"name"		"Arial"
				"tall"		"13"
				"weight"	"600"
			}
		}

		// this is the symbol font
		"Marlett"
		{
			"1"
			{
				"name"		"Marlett"
				"tall"		"11"
				"weight"	"0"
				"symbol"	"1"
			}
		}
		
		"ConsoleFont"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"12"
				"weight"	"600"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Tahoma"
				"tall"		"13"
				"weight"	"600"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Tahoma"
				"tall"		"14"
				"weight"	"600"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Tahoma"
				"tall"		"20"
				"weight"	"600"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Tahoma"
				"tall"		"24"
				"weight"	"600"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}

		"TextMenu"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"16"
				"weight"	"600"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"18"
				"weight"	"600"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"22"
				"weight"	"600"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"600"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"28"
				"weight"	"600"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}

		"AmmoBar"
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
		"HealthBar"
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
		"PlayerInfoSmall"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"8"
				"weight"	"600"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"9"
				"weight"	"600"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"10"
				"weight"	"600"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"11"
				"weight"	"600"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"12"
				"weight"	"600"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}
		"CompactaBold"
		{
			"1"
			{
				"name"		"Compacta Bold Plain"
				"tall"		"12"
				"weight"	"600"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Compacta Bold Plain"
				"tall"		"13"
				"weight"	"600"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Compacta Bold Plain"
				"tall"		"14"
				"weight"	"600"
				"yres"	"768 1023"
				"antialias"	"1"
			}
			"4"
			{
				"name"		"Compacta Bold Plain"
				"tall"		"20"
				"weight"	"600"
				"yres"	"1024 1199"
				"antialias"	"1"
			}
			"5"
			{
				"name"		"Compacta Bold Plain"
				"tall"		"24"
				"weight"	"600"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}
		"ClassicAmmoBar"
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
		"ClassicHealthBar"
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

	//
	//////////////////// BORDERS //////////////////////////////
	//
	// describes all the border types
	Borders
	{
		BaseBorder		DepressedBorder
		ButtonBorder	RaisedBorder
		ComboBoxBorder	DepressedBorder
		MenuBorder		RaisedBorder
		BrowserBorder	DepressedBorder
		PropertySheetBorder	RaisedBorder

		FrameBorder
		{
			// rounded corners for frames
			"backgroundtype" "2"
		}

		DepressedBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "Border.Bright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "Border.Bright"
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
					"color" "Border.Bright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Border.Bright"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "0 0"
				}
			}
		}
		
		TitleButtonBorder
		{
			"backgroundtype" "0"
		}

		TitleButtonDisabledBorder
		{
			"backgroundtype" "0"
		}

		TitleButtonDepressedBorder
		{
			"backgroundtype" "0"
		}

		ScrollBarButtonBorder
		{
			"inset" "2 2 0 0"
			Left
			{
				"1"
				{
					"color" "Border.Bright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Border.Bright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "Border.Dark"
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
					"color" "Border.Dark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "Border.Bright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "Border.Bright"
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
					"color" "Border.Bright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Border.Bright"
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
					"color" "Border.Bright"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Border.Bright"
					"offset" "0 0"
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
					"color" "Border.Dark"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "Border.Dark"
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
					"color" "Border.Selection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "Border.Bright"
					"offset" "0 1"
				}
			}
			Top
			{
				"1"
				{
					"color" "Border.Selection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "Border.Bright"
					"offset" "1 0"
				}
			}
			Right
			{
				"1"
				{
					"color" "Border.Selection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "Border.Dark"
					"offset" "1 0"
				}
			}
			Bottom
			{
				"1"
				{
					"color" "Border.Selection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "Border.Dark"
					"offset" "0 0"
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
					"color" "Border.Dark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "Border.Bright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "Border.Dark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "Border.Bright"
					"offset" "0 0"
				}
			}
		}
	}

	//////////////////////// CUSTOM FONT FILES /////////////////////////////
	//
	// specifies all the custom (non-system) font files that need to be loaded to service the above described fonts
	CustomFontFiles
	{
		"1"		"abcenchance/font/CompactaBold.ttf"
		"2"		"abcenchance/font/LiquidCrystal-Bold.ttf"
	}
	
}