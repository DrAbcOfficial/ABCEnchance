#include <metahook.h>
#include "glew.h"

#include "CColor.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "weaponbank.h"
#include "cvardef.h"
#include "mathlib.h"
#include "gl_def.h"
#include "gl_draw.h"
#include "local.h"

#include "CCustomHud.h"

#include "historyresource.h"

HistoryResource gHR;
void HistoryResource::Init(void){
	AmmoIconColor = pScheme->GetColor("AmmoHUD.AmmoPickUpIconColor", gDefaultColor);
	AmmoTextColor = pScheme->GetColor("AmmoHUD.AmmoPickUpTextColor", gDefaultColor);
	WeaponPickUpColor = pScheme->GetColor("AmmoHUD.WeaponPickUpColor", gDefaultColor);
	WeaponPickUpEmptyColor = pScheme->GetColor("AmmoHUD.WeaponPickUpEmptyColor", gDefaultColor);
	ItemPickUpColor = pScheme->GetColor("AmmoHUD.ItemPickUpColor", gDefaultColor);

	vecAmmoPickUpPos[0] = GET_SCREEN_PIXEL(false, "AmmoHUD.AmmoPickUpX");
	vecAmmoPickUpPos[1] = GET_SCREEN_PIXEL(true, "AmmoHUD.AmmoPickUpY");
	vecWeaponPickUpPos[0] = GET_SCREEN_PIXEL(false, "AmmoHUD.WeaponPickUpX");
	vecWeaponPickUpPos[1] = GET_SCREEN_PIXEL(true, "AmmoHUD.WeaponPickUpY");
	vecItemPickUpPos[0] = GET_SCREEN_PIXEL(false, "AmmoHUD.ItemPickUpX");
	vecItemPickUpPos[1] = GET_SCREEN_PIXEL(true, "AmmoHUD.ItemPickUpY");

	hFont = pScheme->GetFont("HUDSmallShitFont", true);
	pHudDrawTime = CVAR_GET_POINTER("hud_drawhistory_time");
	Reset();
}
void HistoryResource::Reset(void){
	memset(rgHistory, 0, sizeof(rgHistory));
}
void HistoryResource::SetFreeSlot(int iType, int iId, int iCount){
	for (size_t i = 0; i < MAX_HISTORY; i++) {
		HIST_ITEM a = rgHistory[iType][MAX_HISTORY - 1];
		rgHistory[iType][MAX_HISTORY - 1] = rgHistory[iType][i];
		rgHistory[iType][i] = a;
	}
	rgHistory[iType][0].type = iType;
	rgHistory[iType][0].iId = iId;
	rgHistory[iType][0].iCount = iCount;
	rgHistory[iType][0].DisplayTime = gEngfuncs.GetClientTime() + pHudDrawTime->value;
}
float HistoryResource::GetAlphaScale(float a, float flTimediffer){
	return min(a, (flTimediffer * 80.0F)) / 255.0F;
}
void HistoryResource::AddToHistory(int iType, int iId, int iCount){
	if (iType == HISTSLOT_AMMO && !iCount)
		return; 
	SetFreeSlot(iType, iId, iCount);
}
void HistoryResource::AddToHistory(int iType, const char* szName, int iCount){
	if (iType != HISTSLOT_ITEM)
		return;
	int i = gCustomHud.GetSpriteIndex(szName);
	if (i == -1)
		return;
	SetFreeSlot(iType, i, iCount);
}
int HistoryResource::DrawAmmoHistory(float flTime){
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < MAX_HISTORY; j++) {
			HIST_ITEM* item = &rgHistory[i][j];
			if (item->DisplayTime < flTime)
				item->type = HISTSLOT_EMPTY;
			if (item->type == HISTSLOT_EMPTY)
				continue;
			int r, g, b, a;
			int xpos, ypos;
			switch (item->type) {
			case HISTSLOT_AMMO: {
				wrect_t rcPic;
				HSPRITE* spr = gWR.GetAmmoPicFromWeapon(item->iId, rcPic);
				wchar_t buf[16];
				int iTextWidth, iTextHeight;

				wsprintfW(buf, L"%d", item->iCount);
				GetStringSize(buf, &iTextWidth, &iTextHeight, hFont);
				int iIconWidth = rcPic.right - rcPic.left;
				xpos = vecAmmoPickUpPos[0] - iTextWidth - iIconWidth;
				ypos = vecAmmoPickUpPos[1] - (max(iTextHeight, (rcPic.bottom - rcPic.top)) * j);

				AmmoTextColor.GetColor(r, g, b, a);
				float scale = GetAlphaScale(a, item->DisplayTime - flTime);
				r *= scale;
				g *= scale;
				b *= scale;

				DrawVGUI2String(buf, xpos, ypos, r, g, b, hFont, true);
				xpos += iTextWidth + iIconWidth;
				if (spr && *spr) {
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
				WEAPON* weap = gWR.GetWeapon(item->iId);
				if (!weap)
					return 1;
				WeaponPickUpColor.GetColor(r, g, b, a);
				if (!gWR.HasAmmo(weap))
					WeaponPickUpEmptyColor.GetColor(r, g, b, a);
				float scale = GetAlphaScale(a, item->DisplayTime - flTime);
				r *= scale;
				g *= scale;
				b *= scale;
				xpos = vecWeaponPickUpPos[0] - (weap->rcInactive.right - weap->rcInactive.left);
				ypos = vecWeaponPickUpPos[1] - (float)(weap->rcInactive.bottom - weap->rcInactive.top) * 1.5F * j;
				SPR_Set(weap->hInactive, r, g, b);
				SPR_DrawAdditive(0, xpos, ypos, &weap->rcInactive);
				break;
			}
			case HISTSLOT_ITEM: {
				if (!item->iId)
					continue;
				wrect_t* rect = gCustomHud.GetSpriteRect(item->iId);
				ItemPickUpColor.GetColor(r, g, b, a);
				float scale = GetAlphaScale(a, item->DisplayTime - flTime);
				r *= scale;
				g *= scale;
				b *= scale;
				xpos = vecItemPickUpPos[0] + rect->right - rect->left;
				ypos = vecItemPickUpPos[1] - (rect->bottom - rect->top) * j;

				SPR_Set(gCustomHud.GetSprite(item->iId), r, g, b);
				SPR_DrawAdditive(0, xpos, ypos, rect);
				break;
			}
			}
		}
	}
	return 1;
}