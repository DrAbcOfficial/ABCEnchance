#include <metahook.h>

#include "CColor.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "weaponbank.h"
#include "cvardef.h"
#include "mathlib.h"

#include "drawElement.h"
#include "CHudDelegate.h"
#include "utility.h"

#include "historyresource.h"

HistoryResource gHR;
#define AMMO_PICKUP_GAP (gHR.iHistoryGap+5)
#define AMMO_PICKUP_PICK_HEIGHT	 (32 + (gHR.iHistoryGap * 2))
#define AMMO_PICKUP_HEIGHT_MAX (ScreenHeight - 100)
#define MAX_ITEM_NAME	32

void HistoryResource::Init(void){
	AmmoIconColor = pScheme->GetColor("AmmoHUD.AmmoPickUpIconColor", gDefaultColor);
	AmmoTextColor = pScheme->GetColor("AmmoHUD.AmmoPickUpTextColor", gDefaultColor);
	WeaponPickUpColor = pScheme->GetColor("AmmoHUD.WeaponPickUpColor", gDefaultColor);
	WeaponPickUpEmptyColor = pScheme->GetColor("AmmoHUD.WeaponPickUpEmptyColor", gDefaultColor);
	ItemPickUpColor = pScheme->GetColor("AmmoHUD.ItemPickUpColor", gDefaultColor);

	AmmoPickUpDisplayCount = (int)atof(pScheme->GetResourceString("AmmoHUD.AmmoPickUpDisplayCount"));

	vecAmmoPickUpPos[0] = GET_SCREEN_PIXEL(false, "AmmoHUD.AmmoPickUpX");
	vecAmmoPickUpPos[1] = GET_SCREEN_PIXEL(true, "AmmoHUD.AmmoPickUpY");
	vecWeaponPickUpPos[0] = GET_SCREEN_PIXEL(false, "AmmoHUD.WeaponPickUpX");
	vecWeaponPickUpPos[1] = GET_SCREEN_PIXEL(true, "AmmoHUD.WeaponPickUpY");
	vecItemPickUpPos[0] = GET_SCREEN_PIXEL(false, "AmmoHUD.ItemPickUpX");
	vecItemPickUpPos[1] = GET_SCREEN_PIXEL(true, "AmmoHUD.ItemPickUpY");

	hFont = pScheme->GetFont("HUDSmallShitFont", true);
	pHudDrawTime = gEngfuncs.pfnGetCvarPointer("hud_drawhistory_time");
	Reset();
}
void HistoryResource::Reset(void){
	memset(rgAmmoHistory, 0, sizeof rgAmmoHistory);
}
HIST_ITEM* HistoryResource::GetFreeSlot(){
	if ((((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) ||
		(iCurrentHistorySlot >= pHudDrawTime->value))
		iCurrentHistorySlot = 0;
	iCurrentHistorySlot++;
	//TODO: 重构，V社的这整个类都是忍者大师
	return &rgAmmoHistory[clamp(iCurrentHistorySlot, 0, MAX_HISTORY - 1)];
}
void HistoryResource::SetFreeSlot(int iType, int iId, int iCount){
	HIST_ITEM* freeslot = GetFreeSlot();
	freeslot->type = iType;
	freeslot->iId = iId;
	freeslot->iCount = iCount;
	freeslot->DisplayTime = gEngfuncs.GetClientTime() + pHudDrawTime->value;
}
void HistoryResource::AddToHistory(int iType, int iId, int iCount){
	if (iType == HISTSLOT_AMMO && !iCount)
		return; 
	SetFreeSlot(iType, iId, iCount);
}
void HistoryResource::AddToHistory(int iType, const char* szName, int iCount){
	if (iType != HISTSLOT_ITEM)
		return;
	int i = gHudDelegate->GetSpriteIndex(szName);
	if (i == -1)
		return;
	SetFreeSlot(iType, i, iCount);
}
void HistoryResource::CheckClearHistory(void){
	for (int i = 0; i < pHudDrawTime->value; i++){
		if (rgAmmoHistory[i].type)
			return;
	}
	iCurrentHistorySlot = 0;
}
int HistoryResource::DrawAmmoHistory(float flTime){
 	for (int i = 0; i < pHudDrawTime->value; i++){
 		if (rgAmmoHistory[i].type){
			rgAmmoHistory[i].DisplayTime = min(rgAmmoHistory[i].DisplayTime, 
				gEngfuncs.GetClientTime() + pHudDrawTime->value);
			if (rgAmmoHistory[i].DisplayTime <= flTime){
				memset(&rgAmmoHistory[i], 0, sizeof(HIST_ITEM));
				CheckClearHistory();
				return 0;
			}
			int r, g, b, a;
			int xpos, ypos;
			switch (rgAmmoHistory[i].type){
			case HISTSLOT_AMMO: {
				wrect_t rcPic;
				HSPRITE* spr = gWR.GetAmmoPicFromWeapon(rgAmmoHistory[i].iId, rcPic);
				ypos = vecAmmoPickUpPos[1];
				xpos = vecAmmoPickUpPos[0];
				wchar_t buf[16];
				wsprintfW(buf, L"%d", rgAmmoHistory[i].iCount);
				int iTextWidth;
				int iTextHeight;
				GetStringSize(buf, &iTextWidth, &iTextHeight, hFont);
				int iIconHeight = rcPic.bottom - rcPic.top;
				xpos -= iTextWidth;
				ypos -= max(iTextHeight, iIconHeight) * i;
				AmmoTextColor.GetColor(r, g, b, a);
				float scale = min(a, (rgAmmoHistory[i].DisplayTime - flTime) * 80) / 255;
				r *= scale;
				g *= scale;
				b *= scale;
				DrawVGUI2String(buf, xpos, ypos, r, g, b, hFont, true);
				xpos += iTextWidth + ScreenWidth / 40;
				if (spr && *spr){
					AmmoIconColor.GetColor(r, g, b, a);
					r *= scale;
					g *= scale;
					b *= scale;
					SPR_Set(*spr, r, g, b);
					SPR_DrawAdditive(0, xpos, ypos, &rcPic);
				}
				break;
			}
			case HISTSLOT_WEAP: {
				WEAPON* weap = gWR.GetWeapon(rgAmmoHistory[i].iId);
				if (!weap)
					return 1;
				WeaponPickUpColor.GetColor(r, g, b, a);
				if (!gWR.HasAmmo(weap))
					WeaponPickUpEmptyColor.GetColor(r, g, b, a);
				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				scale = min(a, scale) / 255;
				r *= scale;
				g *= scale;
				b *= scale;
				ypos = vecWeaponPickUpPos[1] - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				xpos = vecWeaponPickUpPos[0] - (weap->rcInactive.right - weap->rcInactive.left);
				SPR_Set(weap->hInactive, r, g, b);
				SPR_DrawAdditive(0, xpos, ypos, &weap->rcInactive);
				break;
			}
			case HISTSLOT_ITEM: {
				if (!rgAmmoHistory[i].iId)
					continue;
				wrect_t rect = gHudDelegate->GetSpriteRect(rgAmmoHistory[i].iId);
				ItemPickUpColor.GetColor(r, g, b, a);
				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				scale = min(a, scale) / 255;
				r *= scale;
				g *= scale;
				b *= scale;
				int iIconWidth = rect.right - rect.left;
				int iIconHeight = rect.bottom - rect.top;
				ypos = vecItemPickUpPos[1] - iIconHeight * i;
				xpos = vecItemPickUpPos[0] + iIconWidth;
				SPR_Set(gHudDelegate->GetSprite(rgAmmoHistory[i].iId), r, g, b);
				SPR_DrawAdditive(0, xpos, ypos, &rect);
				break;
			}
			}
		}
	}
	return 1;
}