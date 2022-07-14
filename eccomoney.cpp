#include <metahook.h>
#include <cmath>
#include <string>
#include "hud.h"
#include "weapon.h"
#include "Color.h"
#include "mymathlib.h"
#include "glew.h"

#include "local.h"
#include "vguilocal.h"
#include "gl_def.h"
#include "gl_draw.h"
#include "CCustomHud.h"

#include "enginedef.h"
#include "exportfuncs.h"

#include "vgui_controls/Controls.h"

#include "eccomoney.h"

CHudEccoMoney m_HudEccoMoney;
int CHudEccoMoney::Init(void){
	YOffset = GET_SCREEN_PIXEL(true, "Ecco.YOffset");
	BackgroundLength = GET_SCREEN_PIXEL(false, "Ecco.BackgroundLength");
	BackgroundHeight = GET_SCREEN_PIXEL(true, "Ecco.BackgroundHeight");

	gCVars.pEccoCheckInfo = CREATE_CVAR("cl_eccocheck", "0", FCVAR_VALUE, nullptr);
	gCVars.pEccoEnable = CREATE_CVAR("cl_eccoenable", "1", FCVAR_VALUE, nullptr);

	MoneyColor = pSchemeData->GetColor("Ecco.MoneyColor", gDefaultColor);
	MoneyDecreseColor = pSchemeData->GetColor("Ecco.MoneyDecreseColor", gDefaultColor);
	MoneyIncreseColor = pSchemeData->GetColor("Ecco.MoneyIncreseColor", gDefaultColor);

	HUDSmallFont = pSchemeData->GetFont("HUDSmallShitFont", true);

	MessagePrefix = vgui::localize()->Find("Ecco_MessagePrefix");
	MessagePostfix = vgui::localize()->Find("Ecco_MessagePostfix");
	DifferMessagePrefix = vgui::localize()->Find("Ecco_DifferMessagePrefix");
	DifferMessagePostfix = vgui::localize()->Find("Ecco_DifferMessagePostfix");

	Reset();
	return 0;
}
void CHudEccoMoney::Reset() {
	VGUI_CREATE_NEWTGA_TEXTURE(MoneyBackGround, "abcenchance/tga/ecco_background");
	flStoreMoney = 0;
	iDifferMoney = 0;
	flNextUpdateTime = 0;
	flDifferDisapearTime = 0;
}
int CHudEccoMoney::Draw(float flTime){
	if (gCVars.pEccoEnable->value <= 0)
		return 1;
	if (gCustomHud.IsHudHide(HUD_HIDEALL | HUD_HIDEHEALTH))
		return 1;
	if (!gCustomHud.HasSuit())
		return 1;
	int iEcco = 0;
	if (gCVars.pEccoCheckInfo->value > 0) {
		iEcco = mathlib::fatoi(gEngfuncs.PhysInfo_ValueForKey("ecco_value"));
		if (iEcco == 0)
			iEcco = gCustomHud.GetPlayerHUDInfo(gEngfuncs.GetLocalPlayer()->index)->frags;
	}
	else if (gCVars.pEccoEnable->value == 2)
		return 1;
	else
		iEcco = gCustomHud.GetPlayerHUDInfo(gEngfuncs.GetLocalPlayer()->index)->frags;
	vgui::surface()->DrawSetTexture(-1);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawSetTexture(MoneyBackGround);
	int nowX = 0;
	int nowY = YOffset + BackgroundHeight;
	vgui::surface()->DrawTexturedRect(nowX, YOffset, BackgroundLength, nowY);
	int iTextHeight;
	int r, g, b, a;
	std::wstring buf = MessagePrefix + std::to_wstring(iEcco) + MessagePostfix;
	GetStringSize(buf.c_str(), NULL, &iTextHeight, HUDSmallFont);
	nowX = BackgroundLength / 8;
	nowY -= (BackgroundHeight + iTextHeight) / 2;
	MoneyColor.GetColor(r, g, b, a);
	DrawVGUI2String((wchar_t*)buf.c_str(), nowX, nowY, r, g, b, HUDSmallFont, true);
	if (iEcco != flStoreMoney && flTime > flNextUpdateTime) {
		iDifferMoney = iEcco - flStoreMoney;
		flStoreMoney = iEcco;
		flDifferDisapearTime = flTime + 1.0F;
		flNextUpdateTime = flTime + 0.2F;
	}
	if (flTime < flDifferDisapearTime) {
		buf = (iDifferMoney >= 0 ? L'+' : L'-') + 
			DifferMessagePrefix + std::to_wstring(iDifferMoney) + DifferMessagePostfix;
		GetStringSize(buf.c_str(), NULL, &iTextHeight, HUDSmallFont);
		nowY -= iTextHeight + BackgroundHeight / 2;
		if (iDifferMoney > 0)
			MoneyIncreseColor.GetColor(r, g, b, a);
		else
			MoneyDecreseColor.GetColor(r, g, b, a);
		a *= flDifferDisapearTime - flTime;
		mathlib::ColorCalcuAlpha(r, g, b, a);
		DrawVGUI2String((wchar_t*)buf.c_str(), nowX, nowY, r, g, b, HUDSmallFont, true);
	}
	return 0;
}
