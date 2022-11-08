#include <metahook.h>
#include <map>
#include <string>
#include <vector>

#include "mymathlib.h"
#include "glew.h"

#include "hud.h"
#include "weapon.h"
#include "Color.h"
#include "vguilocal.h"
#include "local.h"
#include "gl_def.h"
#include "gl_draw.h"

#include "cvardef.h"
#include "weaponbank.h"

#include "ammo.h"
#include "CCustomHud.h"
#include "wmenu_slot.h"

CWeaponMenuSlot m_HudWMenuSlot;

void CWeaponMenuSlot::Init(){
	
}
void CWeaponMenuSlot::VidInit(){
	SelectColor = pSchemeData->GetColor("WMenuBucket.SelectColor", gDefaultColor);
	SelectIconColor = pSchemeData->GetColor("WMenuBucket.SelectIconColor", gDefaultColor);
	SelectEmptyColor = pSchemeData->GetColor("WMenuBucket.SelectEmptyColor", gDefaultColor);

	SelectXOffset = GET_SCREEN_PIXEL(false, "WMenuBucket.SelectXOffset");
	SelectYOffset = GET_SCREEN_PIXEL(true, "WMenuBucket.SelectYOffset");
	SelectXGap = GET_SCREEN_PIXEL(false, "WMenuBucket.SelectXGap");
	SelectYGap = GET_SCREEN_PIXEL(true, "WMenuBucket.SelectYGap");
	SelectAnimateTime = atof(pSchemeData->GetResourceString("WMenuBucket.SelectAnimateTime"));
	SelectFadeTime = atof(pSchemeData->GetResourceString("WMenuBucket.SelectFadeTime"));
	SelectHoldTime = atof(pSchemeData->GetResourceString("WMenuBucket.SelectHoldTime"));

	iBucket0Spr = gCustomHud.GetSpriteIndex("bucket1");
	iSelectionSpr = gCustomHud.GetSpriteIndex("selection");
	pRcSelection = gCustomHud.GetSpriteRect(iSelectionSpr);
	pBucketSpr = gCustomHud.GetSprite(iBucket0Spr);
	SelectBucketWidth = gCustomHud.GetSpriteRect(iBucket0Spr)->right -
		gCustomHud.GetSpriteRect(iBucket0Spr)->left;
	SelectBucketHeight = gCustomHud.GetSpriteRect(iBucket0Spr)->bottom -
		gCustomHud.GetSpriteRect(iBucket0Spr)->top;
	if (ScreenWidth >= 640) {
		SelectABWidth = 20;
		SelectABHeight = 4;
	}
	else {
		SelectABWidth = 10;
		SelectABHeight = 2;
	}
}
int CWeaponMenuSlot::DrawBar(int x, int y, int width, int height, float f){
	int r = 0, g = 0, b = 0, a = 0;
	f = mathlib::clamp(f, 0.0f, 1.0f);
	if (f){
		int w = f * width;
		if (w <= 0)
			w = 1;
		SelectIconColor.GetColor(r, g, b, a);
		FillRGBA(x, y, w, height, r, g, b, a);
		x += w;
		width -= w;
	}
	a = 128;
	FillRGBA(x, y, width, height, r, g, b, a);
	return (x + width);
}
void CWeaponMenuSlot::DrawAmmoBar(WEAPON* p, int x, int y, int width, int height){
	if (!p)
		return;
	if (p->iAmmoType != -1){
		if (!gWR.CountAmmo(p->iAmmoType))
			return;

		float f = (float)gWR.CountAmmo(p->iAmmoType) / (float)p->iMax1;
		x = DrawBar(x, y, width, height, f);
		// Do we have secondary ammo too?
		if (p->iAmmo2Type != -1){
			f = (float)gWR.CountAmmo(p->iAmmo2Type) / (float)p->iMax2;
			x += 5; //!!!
			DrawBar(x, y, width, height, f);
		}
	}
}
int CWeaponMenuSlot::DrawWList(float flTime){
	if (m_fFade <= flTime) {
		if (m_bSelectMenuDisplay) {
			m_bSelectMenuDisplay = false;
			m_fAnimateTime = 0;
		}
		return 1;
	}
	float flAnimationRatio = 1.0f;
	if (!m_bSelectMenuDisplay)
		m_fAnimateTime = flTime + SelectAnimateTime;
	if (m_fAnimateTime > flTime && SelectHoldTime > SelectAnimateTime)
		flAnimationRatio = 1 - ((m_fAnimateTime - flTime) / SelectAnimateTime);
	int r, g, b, a, dummy;
	int x, y;
	int iXStart = SelectXOffset;
	int iYStart = SelectYOffset;
	int iXGap = SelectXGap;
	int iYGap = SelectYGap;
	float flAlphaRatio = 1.0f;
	float flTimeDiffer = m_fFade - flTime;
	if (flTimeDiffer < SelectFadeTime)
		flAlphaRatio = flTimeDiffer / SelectFadeTime;

	x = iXStart;
	y = iYStart;
	//绘制顶端1~10
	for (size_t i = 0; i < MAX_WEAPON_SLOT; i++){
		int iWidth;
		if (gWR.m_iNowSlot == i)
			a = 255 * flAlphaRatio;
		else
			a = 192 * flAlphaRatio;
		SelectColor.GetColor(r, g, b, dummy);
		SPR_Set(gCustomHud.GetSprite(iBucket0Spr + i), r, g, b);
		// make active slot wide enough to accomodate gun pictures
		if (i == gWR.m_iNowSlot){
			WEAPON* p = gWR.GetFirstPos(gWR.m_iNowSlot);
			if (p && p->iId > 0)
				iWidth = p->rcActive.right - p->rcActive.left;
			else
				iWidth = SelectBucketWidth;
		}
		else
			iWidth = SelectBucketWidth;
		SPR_DrawAdditive(0, x, y, gCustomHud.GetSpriteRect(iBucket0Spr + i));
		x += iWidth + (iXGap);
	}
	x = iXStart;
	// Draw all of the buckets
	for (int i = 0; i < MAX_WEAPON_SLOT; i++){
		y = SelectBucketHeight + (iYGap * 2 * flAnimationRatio);
		// If this is the active slot, draw the bigger pictures,
		// otherwise just draw boxes
		if (i == gWR.m_iNowSlot){
			WEAPON* p = gWR.GetFirstPos(i);
			int iWidth = SelectBucketWidth;
			if (p && p->iId > 0)
				iWidth = p->rcActive.right - p->rcActive.left;
			for(auto iter = gWR.GetOwnedData()->PosBegin(i); iter != gWR.GetOwnedData()->PosEnd(i);iter++){
				p = iter->second;
				if (gWR.HasAmmo(p))
					SelectColor.GetColor(r, g, b, a);
				else
					SelectEmptyColor.GetColor(r, g, b, a);
				// if active, then we must have ammo.
				if (gWR.m_aryDrawMenu[i] == p->iSlotPos){
					a = 255 * flAlphaRatio;
					ScaleColors(r, g, b, a);
					SPR_Set(p->hActive, r, g, b);
					SPR_DrawAdditive(0, x, y, &p->rcActive);
					SPR_Set(gCustomHud.GetSprite(iSelectionSpr), r, g, b);
					SPR_DrawAdditive(0, x, y, gCustomHud.GetSpriteRect(iSelectionSpr));
				}
				else{
					// Draw Weapon if Red if no ammo
					if (gWR.HasAmmo(p)){
						a = 192 * flAlphaRatio;
						ScaleColors(r, g, b, a);
					}
					else{
						SelectEmptyColor.GetColor(r, g, b, dummy);
						a = 128 * flAlphaRatio;
						ScaleColors(r, g, b, a);
					}
					SPR_Set(p->hInactive, r, g, b);
					SPR_DrawAdditive(0, x, y, &p->rcInactive);
				}
				// Draw Ammo Bar
				DrawAmmoBar(p, x + SelectABWidth / 2, y, SelectABWidth, SelectABHeight);
				y += (p->rcActive.bottom - p->rcActive.top + iYGap) * flAnimationRatio;
			}
			x += iWidth + (iXGap);
		}
		else{
			// Draw Row of weapons.
			for (auto iter = gWR.GetOwnedData()->PosBegin(i); iter != gWR.GetOwnedData()->PosEnd(i); iter++) {
				WEAPON* p = iter->second;
				if (gWR.HasAmmo(p)){
					SelectColor.GetColor(r, g, b, dummy);
					a = 128 * flAlphaRatio;
				}
				else{
					SelectEmptyColor.GetColor(r, g, b, dummy);
					a = 96 * flAlphaRatio;
				}
				FillRGBA(x, y, SelectBucketWidth, SelectBucketHeight, r, g, b, a);
				y += (SelectBucketHeight + iYGap) * flAnimationRatio;
			}
			x += SelectBucketWidth + (iXGap);
		}
	}
	//绘制完毕，修改展示状态
	m_bSelectMenuDisplay = true;
	return 1;
}
bool CWeaponMenuSlot::IsVisible() {
	return m_bSelectMenuDisplay;
}
void CWeaponMenuSlot::Reset(){
	m_fFade = 0;
	m_fAnimateTime = 0;
	m_bSelectMenuDisplay = false;
	m_bSetedCursor = false;
}
void CWeaponMenuSlot::Select() {
	m_fFade = 0;
	m_bSelectMenuDisplay = false;
}