#include <metahook.h>
#include "cvardef.h"
#include "glew.h"
#include "gl_def.h"
#include "mathlib.h"

#include "local.h"
#include "vguilocal.h"
#include "gl_draw.h"

#include "hud.h"
#include "weapon.h"
#include "healthhud.h"
#include "CHudDelegate.h"


#include "exportfuncs.h"

#include "efxhud.h"

#define FRAME_ADVANCE_INTERVAL 0.05F

CHudEfx m_HudEfx;
int CHudEfx::Init(void){
	gCVars.pHudEfx = CREATE_CVAR("cl_efxhud", "1", FCVAR_VALUE, NULL);
	Reset();
	return 0;
}
int CHudEfx::Draw(float flTime){
	if (gCVars.pHudEfx->value < 1)
		return 0;
	//观察者
	if (gHudDelegate->IsInSpectate())
		return 0;
	//低血量
	gHudDelegate->surface()->DrawSetTexture(-1);
	if (m_HudArmorHealth.m_iHealth < gCVars.pDangerHealth->value) {
		if ((double)flTime - flDyingFlincAdvanceTime >= FRAME_ADVANCE_INTERVAL) {
			if (iDyingFlinc >= 60)
				bDyingFlincDir = true;
			else if (iDyingFlinc <= 0)
				bDyingFlincDir = false;
			iDyingFlinc += bDyingFlincDir ? -1 : 1;
			flDyingFlincAdvanceTime = flTime;
		}
		gHudDelegate->surface()->DrawSetColor(255, 25, 25,
			mathlib::clamp((int)(255 * (gCVars.pDangerHealth->value - m_HudArmorHealth.m_iHealth) / gCVars.pDangerHealth->value + iDyingFlinc - 30), 0, 255));
		gHudDelegate->surface()->DrawSetTexture(DarkconerImg);
		gHudDelegate->surface()->DrawTexturedRect(0, 0, ScreenWidth, ScreenHeight);
	}
	//水下
	if (iOldWaterType == WATERLEVEL_HEAD && gClientData->waterlevel != WATERLEVEL_HEAD) {
		flWaterFaceDisapearTime = flTime + 1.5f;
		flFrameLeftAdvanceTime = flFrameRightAdvanceTime = flTime;
	}
	if (flWaterFaceDisapearTime > flTime) {
		int a = (flWaterFaceDisapearTime - flTime) * 255;
		int frame;
		if ((double)flTime - flFrameLeftAdvanceTime >= FRAME_ADVANCE_INTERVAL) {
			frame = SPR_Frames(WaterLeftSpr);
			iFrameLeft++;
			if (iFrameLeft >= frame)
				iFrameLeft = 0;
			flFrameLeftAdvanceTime = flTime;
		}
		gHudDelegate->surface()->DrawSetTexture(-1);
		DrawSPRIcon(WaterLeftSpr, kRenderTransAdd, 
			0, 0, ScreenWidth / 3, ScreenHeight,
			255, 255, 255, a, iFrameLeft);
		if ((double)flTime - flFrameRightAdvanceTime >= FRAME_ADVANCE_INTERVAL) {
			frame = SPR_Frames(WaterRightSpr);
			iFrameRight++;
			if (iFrameRight >= frame)
				iFrameRight = 0;
			flFrameRightAdvanceTime = flTime;
		}
		DrawSPRIcon(WaterRightSpr, kRenderTransAdd, 
			ScreenWidth * 0.666, 0, ScreenWidth / 3, ScreenHeight,
			255, 255, 255, a, iFrameRight);
	}
	if (gClientData->waterlevel == WATERLEVEL_HEAD) {
		gHudDelegate->surface()->DrawSetTexture(-1);
		gHudDelegate->surface()->DrawSetColor(0, 0, 0, 125);
		gHudDelegate->surface()->DrawSetTexture(DarkconerImg);
		gHudDelegate->surface()->DrawTexturedRect(0, 0, ScreenWidth, ScreenHeight);
	}
	iOldWaterType = gClientData->waterlevel;
	return 1;
}
void CHudEfx::Reset(void){
	WaterLeftSpr = SPR_Load("abcenchance/spr/face_rainleft.spr");
	WaterRightSpr = SPR_Load("abcenchance/spr/face_rainright.spr");
	VGUI_CREATE_NEWTGA_TEXTURE(DarkconerImg, "abcenchance/tga/efx_darkround");
	iOldWaterType = 0;
	flWaterFaceDisapearTime = 0.0f;
	flFrameLeftAdvanceTime = flFrameRightAdvanceTime = 0.0f;
	flDyingFlincAdvanceTime = 0.0f;
	iFrameLeft = 0;
	iFrameRight = 0;

	iDyingFlinc = 0;
	bDyingFlincDir = false;
}