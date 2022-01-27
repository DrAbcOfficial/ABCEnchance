Scheme
{
	Colors
	{
        //生命值图标颜色
		"HealthArmor.HealthIconColor"		"0 255 0 255"
		//血条颜色
		"HealthArmor.HealthBarColor"		"0 255 0 255"
		//生命值数值颜色
		"HealthArmor.HealthTextColor"		"0 255 0 255"
		//生命值受伤变色
		"HealthArmor.HealthPainColor"		"255 0 0 255"
		//生命值濒死颜色
		"HealthArmor.HealthDangerColor"		"250 0 0 255"

		//护甲图标颜色
		"HealthArmor.ArmorIconColor"		"0 255 0 255"
		//护甲条颜色
		"HealthArmor.ArmorBarColor"			"0 255 0 255"
		//护甲文字颜色
		"HealthArmor.ArmorTextColor"		"0 255 0 255"
		//护甲受伤变色
		"HealthArmor.ArmorPainColor"		"255 0 0 255"
		//护甲危险颜色
		"HealthArmor.ArmorDangerColor"		"0 125 0 255"

        //长跳图标颜色
		"HealthArmor.LongjumpIconColor"		"0 255 0 255"

        //伤害指示器颜色
		"HealthArmor.PainIndicatorColor"	"255 0 0 255"
		//伤害指示器仅减少护甲颜色
		"HealthArmor.PainIndicatorColorA"	"255 255 255 255"
		//伤害类型突变颜色
		"HealthArmor.BitDamageColor"		"0 255 0 255"

        //主要子弹图标颜色
		"AmmoHUD.Ammo1IconColor"		"0 255 0 255"
		//弹匣文字颜色
		"AmmoHUD.Ammo1BigTextColor"		"0 255 0 255"
		//后备子弹文字颜色
		"AmmoHUD.Ammo1TextColor"		"0 255 0 255"
		//次要子弹图标颜色
		"AmmoHUD.Ammo2IconColor"		"0 255 0 255"
		//次要子弹弹匣文字颜色
		"AmmoHUD.Ammo2BigTextColor"			"0 255 0 255"
		//次要子弹后备文字颜色
		"AmmoHUD.Ammo2TextColor"		"0 255 0 255"
		//拾取子弹图标颜色
		"AmmoHUD.AmmoPickUpIconColor"		"0 255 0 255"
		//拾取子弹数量颜色
		"AmmoHUD.AmmoPickUpTextColor"		"0 255 0 255"
		//拾取武器图标颜色
		"AmmoHUD.WeaponPickUpColor"		"0 255 0 255"
		//拾取空武器图标颜色
		"AmmoHUD.WeaponPickUpEmptyColor"		"255 0 0 255"
		//拾取物品图标颜色
		"AmmoHUD.ItemPickUpColor"		"0 255 0 255"

        //环状菜单背景颜色
		"WMenuAnnular.SelectColor"			"106 102 96 255"
		//环状菜单选中框颜色
		"WMenuAnnular.SelectRinColor"		"0 255 0 255"
		//环状菜单图标颜色
		"WMenuAnnular.SelectIconColor"		"128 255 0 255"
		//环状菜单空武器图标颜色
		"WMenuAnnular.SelectEmptyColor"		"255 0 0 255"
		//环状菜单文字颜色
		"WMenuAnnular.SelectTextColor"		"128 255 0 255"
		//环状菜单指针颜色
		"WMenuAnnular.SelectPointerColor"		"255 128 0 255"

        //经典菜单颜色
		"WMenuBucket.SelectColor"			"0 255 0 255"
		//经典菜单图标颜色
		"WMenuBucket.SelectIconColor"		"255 255 0 255"
		//经典菜单空武器颜色
		"WMenuBucket.SelectEmptyColor"		"255 0 0 255"

        //死亡消息背景颜色
		"DeathMsg.BackGoundColor"		"180 180 180 255"
		//死亡消息外框颜色
		"DeathMsg.BackGoundOutLineColor"		"255 182 193 255"
		//死亡消息受害者文字颜色
		"DeathMsg.VictimColor"		"0 178 178 255"
		//死亡消息攻击者文字颜色
		"DeathMsg.AttackerColor"		"204 0 0 255"
		//死亡消息攻击武器颜色
		"DeathMsg.InflictorColor"		"178 178 178 255"

        //投票菜单外框颜色
		"Vote.OutlineColor"		"255 255 255 180"
		//投票菜单背景颜色
		"Vote.BackGoundColor"		"139 137 137 180"

        //分数，金钱增加颜色
		"Ecco.MoneyIncreseColor"		"0 255 0 255"
		//分数，金钱减少颜色
		"Ecco.MoneyDecreseColor"		"255 0 0 255"
		//分数，金钱颜色
		"Ecco.MoneyColor"		"0 255 0 255"

        //购买菜单文字颜色
		"BuyMenu.TextColor"		"255 255 255 255"
		//购买菜单背景颜色
		"BuyMenu.ButtonColor"		"125 125 125 125"
	}

    //标记为[长度]的量以屏幕左上角宽高为（0,0），屏幕右下角宽高为（1,1）计算
    //标记为[尺寸]的量以屏幕高计算
    //时间单位为秒
	BaseSettings
	{
		"Version"						"100015"
		"Language"                      ""
		//[尺寸]鼠标光标大小
		"Common.CursorSize"             "0.03"

        //[长度]血条护甲条起始宽
		"HealthArmor.StartX"			"0.013"
		//[尺寸]图标大小
		"HealthArmor.IconSize"			"0.03"
		//[尺寸]血条长度
		"HealthArmor.BarLength"			"0.07"
		//[尺寸]血条高度
		"HealthArmor.BarWidth"			"0.02"
		//[尺寸]图标，文字，血条各元素间隔
		"HealthArmor.ElementGap"		"0.01"
		//[长度]血条护甲条背景图片起始高
		"HealthArmor.BackGroundY"		"0.95"
		//[尺寸]背景图片长度
		"HealthArmor.BackGroundLength"	"0.333"
		//[长度]伤害类型图标起始宽
		"HealthArmor.DamageIconX"		"0"
		//[长度]伤害类型图标起始高
		"HealthArmor.DamageIconY"		"0.85"
		//[尺寸]伤害类型图标尺寸
		"HealthArmor.DamageIconSize"	"0.08"
		//受伤变色持续时间
		"HealthArmor.PainColorTime"		"0.5"
		//伤害指示器持续时间
		"HealthArmor.PainIndicatorTime"	"3"
		//色差伤害指示器持续时间
		"HealthArmor.ShockIndicatorTime""0.2"

        //子弹图标大小
		"AmmoHUD.IconSize"			"0.03"
		//文字图标间隔
		"AmmoHUD.ElementGap"		"0.01"
		//背景图片起始高
		"AmmoHUD.BackGroundY"		"0.95"
		//背景图片长度
		"AmmoHUD.BackGroundLength"	"0.2"

        //拾取子弹起始宽
		"AmmoHUD.AmmoPickUpX"	"0.45"
		//拾取子弹起始高
		"AmmoHUD.AmmoPickUpY"	"0.55"
		//拾取武器图标起始宽
		"AmmoHUD.WeaponPickUpX"	"1"
		//拾取武器图标起始高
		"AmmoHUD.WeaponPickUpY"	"0.85"
		//拾取物品起始宽
		"AmmoHUD.ItemPickUpX"	"0.55"
		//拾取物品起始高
		"AmmoHUD.ItemPickUpY"	"0.55"

        //环状菜单空心长度
		"WMenuAnnular.SelectOffset"	"0.25"
		//环状菜单按钮长度
		"WMenuAnnular.SelectSize"	"0.1"
		//环状菜单指针尺寸
		"WMenuAnnular.SelectPointerSize"	"0.03"
		//环状菜单起始选址度数（角度）
		"WMenuAnnular.SelectRotate"	"0"
		//环状菜单开启动画时间
		"WMenuAnnular.SelectAnimateTime"	"0.12"
		//环状菜单消失动画时间
		"WMenuAnnular.SelectFadeTime"	"0.2"
		//环状菜单保持时间
		"WMenuAnnular.SelectHoldTime"	"5"

        //经典菜单起始宽
		"WMenuBucket.SelectXOffset"	"0.005"
		//经典菜单起始高
		"WMenuBucket.SelectYOffset"	"0.01"
		//经典菜单图标间隔宽
		"WMenuBucket.SelectXGap"	"0.0025"
		//经典菜单图标间隔高
		"WMenuBucket.SelectYGap"	"0.005"
		//经典菜单保持时间
		"WMenuBucket.SelectHoldTime"	"5"
		//经典菜单开启动画时间
		"WMenuBucket.SelectAnimateTime"	"0.12"
		//经典菜单消失动画时间
		"WMenuBucket.SelectFadeTime"	"0.2"

        //雷达起始宽
		"Radar.XOffset"			"0.012"
		//雷达起始高
		"Radar.YOffset"			"0.0225"
		//指北针N图标尺寸
		"Radar.NorthPointerSize"	"0.0233"
		//雷达中心图标尺寸
		"Radar.ViewAngleSize"	"0.125"
		//雷达灰色背景图标不透明度
		"Radar.OutLineAlpha"	"255"
		//雷达地图不透明度
		"Radar.MapAlpha"		"255"
		//雷达中心图标不透明度
		"Radar.CenterAlpha"		"255"

        //死亡消息起始宽
		"DeathMsg.XOffset"		"0.93"
		//死亡消息起始高
		"DeathMsg.YOffset"		"0.25"
		//两条死亡消息间隔
		"DeathMsg.GapOffset"	"0.012"
		//死亡消息背景高度
		"DeathMsg.BackGoundWidth"	"0.0070"

        //投票菜单起始宽
		"Vote.XOffset"		"0.9"
		//投票菜单起始高
		"Vote.YOffset"		"0.233"
		//投票菜单高
		"Vote.Height"		"0.33"
		//投票菜单宽
		"Vote.Width"		"0.2"

        //分数金钱起始高
		"Ecco.YOffset"		"0.4"
		//分数金钱背景长度
		"Ecco.BackgroundLength"		"0.11"
		//分数金钱背景高度
		"Ecco.BackgroundHeight"		"0.0325"

        //购买菜单进入动画时间
		"EccoBuyMenu.BuyMenuAnimateTime"		"0.1"
		//购买菜单圆心起始宽
		"EccoBuyMenu.BuyMenuCenterX"		"0.3"
		//购买菜单圆心起始高
		"EccoBuyMenu.BuyMenuCenterY"		"0.5"
		//购买菜单圆尺寸
		"EccoBuyMenu.BuyMenuHeight"		"0.2"
		//购买菜单圆空心尺寸
		"EccoBuyMenu.BuyMenuOffset"		"0.15"
		//购买菜单物品模型缩放
		"EccoBuyMenu.BuyMenuModelSize"		"1.3"
		//购买菜单第三人称摄像机旋转角度
		"EccoBuyMenu.BuyMenuCamYaw"		"220"
		//购买菜单第三人称摄像机距离
		"EccoBuyMenu.BuyMenuCamDist"		"80"
		//购买菜单第三人你摄像机高度
		"EccoBuyMenu.BuyMenuCamHeight"		"-40"
		//购买菜单第三人称摄像机左右平移
		"EccoBuyMenu.BuyMenuCamRight"		"50"
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
		"VoteShitFont"
		{
			"1"
			{
				"name"		"Arial"
				"tall"		"14"
				"weight"	"0"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Arial"
				"tall"		"15"
				"weight"	"0"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Arial"
				"tall"		"16"
				"weight"	"0"
				"yres"	"768 1023"
			}
			"4"
			{
				"name"		"Arial"
				"tall"		"18"
				"weight"	"0"
				"yres"	"1024 1199"
			}
			"5"
			{
				"name"		"Arial"
				"tall"		"20"
				"weight"	"0"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}
		"BuyMenuShitFont"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"14"
				"weight"	"0"
				"yres"	"480 599"
			}
			"2"
			{
				"name"		"Verdana"
				"tall"		"15"
				"weight"	"0"
				"yres"	"600 767"
			}
			"3"
			{
				"name"		"Verdana"
				"tall"		"16"
				"weight"	"0"
				"yres"	"768 1023"
			}
			"4"
			{
				"name"		"Verdana"
				"tall"		"18"
				"weight"	"0"
				"yres"	"1024 1199"
			}
			"5"
			{
				"name"		"Verdana"
				"tall"		"20"
				"weight"	"0"
				"yres"	"1200 6000"
				"antialias"	"1"
			}
		}
	}
}
