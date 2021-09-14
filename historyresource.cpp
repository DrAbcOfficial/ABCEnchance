#include <metahook.h>
#include "CColor.h"
#include "vguilocal.h"
#include "hud.h"
#include "weapon.h"
#include "weaponbank.h"
#include "drawElement.h"
#include "CHudDelegate.h"
#include "historyresource.h"


HistoryResource gHR;

#define AMMO_PICKUP_GAP (gHR.iHistoryGap+5)
#define AMMO_PICKUP_PICK_HEIGHT		(32 + (gHR.iHistoryGap * 2))
#define AMMO_PICKUP_HEIGHT_MAX		(ScreenHeight - 100)

#define MAX_ITEM_NAME	32
int HISTORY_DRAW_TIME = 5;

struct ITEM_INFO{
	wrect_t rect;
};

void HistoryResource::AddToHistory(int iType, int iId, int iCount){
	if (iType == HISTSLOT_AMMO && !iCount)
		return;  // no amount, so don't add

	if ((((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= iAmmoDisplayCount)){	// the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM* freeslot = &rgAmmoHistory[iCurrentHistorySlot++];  // default to just writing to the first slot
	HISTORY_DRAW_TIME = CVAR_GET_FLOAT("hud_drawhistory_time");

	freeslot->type = iType;
	freeslot->iId = iId;
	freeslot->iCount = iCount;
	freeslot->DisplayTime = gEngfuncs.GetClientTime() + HISTORY_DRAW_TIME;
}

void HistoryResource::AddToHistory(int iType, const char* szName, int iCount){
	if (iType != HISTSLOT_ITEM)
		return;

	if ((((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= iAmmoDisplayCount)){	// the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM* freeslot = &rgAmmoHistory[iCurrentHistorySlot++];  // default to just writing to the first slot

	// I am really unhappy with all the code in this file

	int i = gHudDelegate->GetSpriteIndex(szName);
	if (i == -1)
		return;  // unknown sprite name, don't add it to history

	freeslot->iId = i;
	freeslot->type = iType;
	freeslot->iCount = iCount;

	HISTORY_DRAW_TIME = CVAR_GET_FLOAT("hud_drawhistory_time");
	freeslot->DisplayTime = gEngfuncs.GetClientTime() + HISTORY_DRAW_TIME;
}


void HistoryResource::CheckClearHistory(void){
	for (int i = 0; i < iAmmoDisplayCount; i++){
		if (rgAmmoHistory[i].type)
			return;
	}

	iCurrentHistorySlot = 0;
}

//
// Draw Ammo pickup history
//
int HistoryResource::DrawAmmoHistory(float flTime){
 	for (int i = 0; i < iAmmoDisplayCount; i++){
 		if (rgAmmoHistory[i].type){
			rgAmmoHistory[i].DisplayTime = min(rgAmmoHistory[i].DisplayTime, gEngfuncs.GetClientTime() + HISTORY_DRAW_TIME);

			if (rgAmmoHistory[i].DisplayTime <= flTime){
				memset(&rgAmmoHistory[i], 0, sizeof(HIST_ITEM));
				CheckClearHistory();
				return 0;
			}
			switch (rgAmmoHistory[i].type){
			case HISTSLOT_AMMO: {
				wrect_t rcPic;
				HSPRITE* spr = gWR.GetAmmoPicFromWeapon(rgAmmoHistory[i].iId, rcPic);
				int r, g, b, a;
				int ypos = ScreenHeight / 2 + vecAmmoPickUpPos[1];
				int xpos = ScreenWidth / 2 + vecAmmoPickUpPos[0];
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
				int r, g, b, a;
				WeaponPickUpColor.GetColor(r, g, b, a);
				if (!gWR.HasAmmo(weap))
					WeaponPickUpEmptyColor.GetColor(r, g, b, a);
				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				scale = min(a, scale) / 255;
				r *= scale;
				g *= scale;
				b *= scale;
				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i)) + vecWeaponPickUpPos[1];
				int xpos = ScreenWidth - (weap->rcInactive.right - weap->rcInactive.left) + vecWeaponPickUpPos[0];
				SPR_Set(weap->hInactive, r, g, b);
				SPR_DrawAdditive(0, xpos, ypos, &weap->rcInactive);
				break;
			}
			case HISTSLOT_ITEM: {
				if (!rgAmmoHistory[i].iId)
					continue;
				wrect_t rect = gHudDelegate->GetSpriteRect(rgAmmoHistory[i].iId);
				int r, g, b, a;
				ItemPickUpColor.GetColor(r, g, b, a);
				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				scale = min(a, scale) / 255;
				r *= scale;
				g *= scale;
				b *= scale;
				int iIconWidth = rect.right - rect.left;
				int iIconHeight = rect.bottom - rect.top;
				int ypos = ScreenHeight / 2  - iIconHeight * i + vecItemPickUpPos[1];
				int xpos = ScreenWidth / 2 + iIconWidth + vecItemPickUpPos[0];
				SPR_Set(gHudDelegate->GetSprite(rgAmmoHistory[i].iId), r, g, b);
				SPR_DrawAdditive(0, xpos, ypos, &rect);
				break;
			}
			}
		}
	}
	return 1;
}