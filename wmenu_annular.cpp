#include <metahook.h>

#include "mathlib.h"

#include "hud.h"
#include "weapon.h"
#include "CColor.h"
#include "vguilocal.h"
#include "local.h"

#include "glew.h"
#include "gl_shader.h"
#include "gl_draw.h"
#include "gl_utility.h"
#include "gl_def.h"

#include "cvardef.h"
#include "weaponbank.h"

#include "ammo.h"
#include "CHudDelegate.h"
#include "wmenu_annular.h"

using namespace mathlib;

CWeaponMenuAnnular m_HudWMenuAnnular;
void __UserCmd_OpenAnnularMenu(void) {
	if (!m_HudWMenuAnnular.m_bOpeningMenu && !m_HudWMenuAnnular.m_bSelectMenuDisplay) {
		if (m_HudWMenuAnnular.m_fFade <= gEngfuncs.GetClientTime())
			PlaySoundByName("common/wpn_hudon.wav", 1);
		m_HudWMenuAnnular.m_bOpeningMenu = true;
		gHudDelegate->m_iVisibleMouse = true;
	}
}
void __UserCmd_CloseAnnularMenu(void) {
	if (m_HudWMenuAnnular.m_bOpeningMenu && m_HudWMenuAnnular.m_bSelectMenuDisplay) {
		m_HudWMenuAnnular.m_bOpeningMenu = false;
		m_HudWMenuAnnular.m_fFade = 0;
		gHudDelegate->m_iVisibleMouse = false;
		m_HudWMenuAnnular.m_bSetedCursor = false;
		m_HudWMenuAnnular.m_bSelectMenuDisplay = false;
	}
}
void CWeaponMenuAnnular::Select() {
	if (m_fFade > gEngfuncs.GetClientTime()) {
		if (m_HudCustomAmmo.m_bAcceptDeadMessage)
			return;
		m_HudCustomAmmo.ChosePlayerWeapon();
	}
	if (!m_bOpeningMenu) {
		gWR.iNowSlot = 0;
		m_fFade = 0;
		m_bSelectMenuDisplay = false;
	}
}
void CWeaponMenuAnnular::GLInit() {
	glGenFramebuffersEXT(1, &m_hGaussianBufferVFBO);
	m_hGaussianBufferVTex = GL_GenTextureRGB8(gScreenInfo.iWidth, gScreenInfo.iHeight);
	glGenFramebuffersEXT(1, &m_hGaussianBufferHFBO);
	m_hGaussianBufferHTex = GL_GenTextureRGB8(gScreenInfo.iWidth, gScreenInfo.iHeight);
}
void CWeaponMenuAnnular::Clear(){
	if (m_hGaussianBufferVTex)
		glDeleteTextures(1, &m_hGaussianBufferVTex);
	if (m_hGaussianBufferHTex)
		glDeleteTextures(1, &m_hGaussianBufferHTex);
}
void CWeaponMenuAnnular::Init() {
	ADD_COMMAND("+annularmenu", __UserCmd_OpenAnnularMenu);
	ADD_COMMAND("-annularmenu", __UserCmd_CloseAnnularMenu);

	SelectColor = pScheme->GetColor("WMenuAnnular.SelectColor", gDefaultColor);
	SelectRinColor = pScheme->GetColor("WMenuAnnular.SelectRinColor", gDefaultColor);
	SelectIconColor = pScheme->GetColor("WMenuAnnular.SelectIconColor", gDefaultColor);
	SelectTextColor = pScheme->GetColor("WMenuAnnular.SelectTextColor", gDefaultColor);
	SelectEmptyColor = pScheme->GetColor("WMenuAnnular.SelectEmptyColor", gDefaultColor);
	SelectPointerColor = pScheme->GetColor("WMenuAnnular.SelectPointerColor", gDefaultColor);

	SelectOffset = GET_SCREEN_PIXEL(true, "WMenuAnnular.SelectOffset");
	SelectSize = GET_SCREEN_PIXEL(true, "WMenuAnnular.SelectSize");
	SelectPointerSize = GET_SCREEN_PIXEL(true, "WMenuAnnular.SelectPointerSize");
	SelectRotate = atof(pScheme->GetResourceString("WMenuAnnular.SelectRotate"));
	SelectAnimateTime = atof(pScheme->GetResourceString("WMenuAnnular.SelectAnimateTime"));
	SelectFadeTime = atof(pScheme->GetResourceString("WMenuAnnular.SelectFadeTime"));
	SelectHoldTime = atof(pScheme->GetResourceString("WMenuAnnular.SelectHoldTime"));

	HUDFont = pScheme->GetFont("HUDShitFont", true);
	HUDSmallFont = pScheme->GetFont("HUDSmallShitFont", true);

	if (SelectHoldTime <= 0)
		SelectHoldTime = 5;
}
void CWeaponMenuAnnular::DrawSelectIcon(WEAPON* wp, int a, int xpos, int ypos, int index) {
	wchar_t buf[64];
	int iTextWidth;
	int r, g, b, dummy;
	float h, s, v;
	int iHeight = wp->rcActive.bottom - wp->rcActive.top;
	int iWidth = wp->rcActive.right - wp->rcActive.left;
	if (gWR.HasAmmo(wp) && gWR.HasWeapon(wp))
		SelectIconColor.GetColor(r, g, b, dummy);
	else
		SelectEmptyColor.GetColor(r, g, b, dummy);
	if (gCVars.pAmmoMenuDrawRainbow->value > 0) {
		RGBToHSV(r, g, b, h, s, v);
		h += 36 * index + 120;
		HSVToRGB(h, s, v, r, g, b);
	}
	ColorCalcuAlpha(r, g, b, a);
	SPR_Set(wp->hInactive, r, g, b);
	SPR_DrawAdditive(0, xpos - iWidth / 2, ypos - iHeight / 2, &wp->rcInactive);
	SelectTextColor.GetColor(r, g, b, dummy);
	if (gCVars.pAmmoMenuDrawRainbow->value > 0) {
		RGBToHSV(r, g, b, h, s, v);
		h += 36 * index + 120;
		HSVToRGB(h, s, v, r, g, b);
	}
	ColorCalcuAlpha(r, g, b, a);
	if (gCVars.pAmmoMenuDrawPos->value > 0) {
		wsprintfW(buf, L"·%d", wp->iSlotPos);
		GetStringSize(buf, &iTextWidth, NULL, HUDSmallFont);
		DrawVGUI2String(buf, xpos - iTextWidth - iWidth / 2, ypos - iHeight / 2, r, g, b, HUDSmallFont, true);
	}

	if (wp->iAmmoType >= 0) {
		if (wp->iClip >= 0)
			wsprintfW(buf, L"%d/%d", wp->iClip, gWR.CountAmmo(wp->iAmmoType));
		else
			wsprintfW(buf, L"%d", gWR.CountAmmo(wp->iAmmoType));
	}
	else
		wsprintfW(buf, L"");
	GetStringSize(buf, &iTextWidth, NULL, HUDFont);
	DrawVGUI2String(buf, xpos - iTextWidth / 2, ypos + iHeight, r, g, b, HUDFont, true);
}
int CWeaponMenuAnnular::DrawWList(float flTime) {
	if (m_fFade <= flTime) {
		if (m_bSelectMenuDisplay) {
			m_bSelectMenuDisplay = false;
			m_fAnimateTime = 0;
		}
		return 1;
	}
	gWR.FillMenuGrid();
	float flTimeDiffer = m_fFade - flTime;
	float flStartRot = SelectRotate;
	int iBackGroundHeight = SelectSize;
	int iOffset = SelectOffset;
	float flAnimationRatio = 1.0f;
	if (!m_bSelectMenuDisplay)
		m_fAnimateTime = flTime + SelectAnimateTime;
	if (m_fAnimateTime > flTime && SelectHoldTime > SelectAnimateTime) {
		flAnimationRatio = 1 - ((m_fAnimateTime - flTime) / SelectAnimateTime);
		iOffset *= flAnimationRatio;
		flAnimationRatio /= 100;
	}
	int i;
	float ac, as;
	vec2_t aryOut[10];
	vec2_t aryIn[10];
	WEAPON* wp;
	int halfWidth = ScreenWidth / 2;
	int halfHeight = ScreenHeight / 2;
	int xpos;
	int ypos;
	vec2_t vecA, vecB, vecC, vecD;
	int a = 255;
	int r, g, b, dummy;
	//填充十边形坐标数组
	for (i = 0; i < 10; i++) {
		ac = cos(2 * M_PI * i / 10 + flStartRot);
		as = sin(2 * M_PI * i / 10 + flStartRot);

		aryIn[i][0] = iOffset * ac;
		aryIn[i][1] = iOffset * as;

		aryOut[i][0] = (iOffset + iBackGroundHeight) * ac;
		aryOut[i][1] = (iOffset + iBackGroundHeight) * as;
	}

	if (flTimeDiffer < SelectFadeTime)
		a *= flTimeDiffer / SelectFadeTime;

	if (m_bOpeningMenu) {
		//模糊
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_hOldBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_hGaussianBufferHFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hGaussianBufferHTex, 0);
		GL_BlitFrameBufferToFrameBufferColorOnly(m_hOldBuffer, m_hGaussianBufferHFBO, ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight);
		DrawGaussianBlur(m_hGaussianBufferVFBO, m_hGaussianBufferHTex, m_hGaussianBufferVTex, flAnimationRatio, true, ScreenWidth, ScreenHeight);
		DrawGaussianBlur(m_hGaussianBufferHFBO, m_hGaussianBufferVTex, m_hGaussianBufferHTex, flAnimationRatio, false, ScreenWidth, ScreenHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, m_hOldBuffer);
		glBind(m_hGaussianBufferHTex);
		DrawQuad(ScreenWidth, ScreenHeight);
		//绘制鼠标指针
		SelectPointerColor.GetColor(r, g, b, dummy);
		ac = cos(m_fCursorAngle - 0.45 * M_PI);
		as = sin(m_fCursorAngle - 0.45 * M_PI);
		vecA[0] = -SelectPointerSize * ac - (iOffset - SelectPointerSize) * as;
		vecA[1] = -SelectPointerSize * as + (iOffset - SelectPointerSize) * ac;
		vecB[0] = SelectPointerSize * ac - (iOffset - SelectPointerSize) * as;
		vecB[1] = SelectPointerSize * as + (iOffset - SelectPointerSize) * ac;
		vecC[0] = -SelectPointerSize * ac - iOffset * as;
		vecC[1] = -SelectPointerSize * as + iOffset * ac;
		vecD[0] = SelectPointerSize * ac - iOffset * as;
		vecD[1] = SelectPointerSize * as + iOffset * ac;
		vecA[0] += halfWidth;
		vecA[1] = halfHeight - vecA[1];
		vecB[0] += halfWidth;
		vecB[1] = halfHeight - vecB[1];
		vecC[0] += halfWidth;
		vecC[1] = halfHeight - vecC[1];
		vecD[0] += halfWidth;
		vecD[1] = halfHeight - vecD[1];
		DrawSPRIconPos(iSelectCyclerCursorPointer, kRenderTransAdd, vecC, vecA, vecB, vecD, r, g, b, a);
	}
	//绘制十边形
	for (i = 0; i < 10; i++) {
		//CABD
		//↓→↑
		Vector2Copy(aryIn[i == 9 ? 0 : i + 1], vecA);
		Vector2Copy(aryIn[i], vecB);
		Vector2Copy(aryOut[i == 9 ? 0 : i + 1], vecC);
		Vector2Copy(aryOut[i], vecD);
		//变换为OpenGL屏幕坐标
		CenterPos2OpenGLPos(vecA, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecB, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecC, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecD, ScreenWidth, ScreenHeight);
		//CABD
		SelectColor.GetColor(r, g, b, dummy);
		if (gCVars.pAmmoMenuDrawRainbow->value > 1) {
			float h, s, v;
			RGBToHSV(r, g, b, h, s, v);
			h += 36 * i;
			s = 1;
			HSVToRGB(h, s, v, r, g, b);
		}
		DrawSPRIconPos(iSelectCyclerSpr, kRenderTransAdd, vecC, vecA, vecB, vecD, r, g, b, a * 0.5);
		if (gWR.gridDrawMenu[i].iId <= 0 || i == gWR.iNowSlot)
			continue;
		wp = gWR.GetWeapon(gWR.gridDrawMenu[i].iId);
		if (!wp)
			continue;
		xpos = (vecA[0] + vecB[0] + vecC[0] + vecD[0]) / 4;
		ypos = (vecA[1] + vecB[1] + vecC[1] + vecD[1]) / 4;
		DrawSelectIcon(wp, a, xpos, ypos, i);
	}
	//绘制已选
	if (gWR.gridDrawMenu[gWR.iNowSlot].iId > -1 && gWR.iNowSlot >= 0) {
		wp = gWR.GetWeapon(gWR.gridDrawMenu[gWR.iNowSlot].iId);
		Vector2Copy(aryIn[gWR.iNowSlot == 9 ? 0 : gWR.iNowSlot + 1], vecA);
		Vector2Copy(aryIn[gWR.iNowSlot], vecB);
		Vector2Copy(aryOut[gWR.iNowSlot == 9 ? 0 : gWR.iNowSlot + 1], vecC);
		Vector2Copy(aryOut[gWR.iNowSlot], vecD);
		CenterPos2OpenGLPos(vecA, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecB, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecC, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecD, ScreenWidth, ScreenHeight);
		xpos = (vecA[0] + vecB[0] + vecC[0] + vecD[0]) / 4;
		ypos = (vecA[1] + vecB[1] + vecC[1] + vecD[1]) / 4;
		DrawSelectIcon(wp, a, xpos, ypos, gWR.iNowSlot);
		SelectRinColor.GetColor(r, g, b, dummy);
		DrawSPRIconPos(iSelectCyclerRinSpr, kRenderTransAdd, vecC, vecA, vecB, vecD, r, g, b, a);
	}
	//绘制完毕，修改展示状态
	m_bSelectMenuDisplay = true;
	return 1;
}
void CWeaponMenuAnnular::Reset(){
	m_fFade = 0;
	m_fAnimateTime = 0;
	m_bSelectMenuDisplay = false;
	m_bOpeningMenu = false;
	m_bSetedCursor = false;

	iSelectCyclerSpr = SPR_Load("abcenchance/spr/select_cycler.spr");
	iSelectCyclerRinSpr = SPR_Load("abcenchance/spr/selected_rin.spr");
	iSelectCyclerCursorPointer = SPR_Load("abcenchance/spr/select_pointer.spr");
}