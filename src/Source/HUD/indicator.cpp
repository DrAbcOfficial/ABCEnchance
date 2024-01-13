#include <metahook.h>
#include <parsemsg.h>
#include <cmath>
#include "mymathlib.h"
#include "glew.h"

#include "hud.h"
#include "weapon.h"
#include "vguilocal.h"
#include "gl_def.h"
#include "gl_draw.h"
#include "gl_utility.h"
#include "gl_shader.h"
#include "local.h"

#include "CCustomHud.h"
#include "vgui_controls/Controls.h"

#include "indicator.h"

CHudIndicator m_HudIndicator;


void CHudIndicator::GLInit() {
	glGenFramebuffersEXT(1, &m_hFilterFBO);
	m_hFilterTex = GL_GenTextureRGBA8(ScreenWidth, ScreenHeight);
}
void CHudIndicator::Init(void){
	Reset();
}
int CHudIndicator::VidInit(void){
	PainColorTime = atof(pSchemeData->GetResourceString("HealthArmor.PainColorTime"));
	PainIndicatorTime = atof(pSchemeData->GetResourceString("HealthArmor.PainIndicatorTime"));
	ShockIndicatorTime = atof(pSchemeData->GetResourceString("HealthArmor.ShockIndicatorTime"));

	PainIndicatorColor = pSchemeData->GetColor("HealthArmor.PainIndicatorColor", gDefaultColor);
	PainIndicatorColorA = pSchemeData->GetColor("HealthArmor.PainIndicatorColorA", gDefaultColor);
	return 1;
}
void CHudIndicator::Reset(void){
	iPainIndicator = SPR_Load("abcenchance/spr/pain_indicator.spr");
	memset(&m_hScreenFilter, 0, sizeof(m_hScreenFilter));
	memset(aryIndicators, 0, sizeof(aryIndicators));
	iNowSelectIndicator = 0;
	flPainColorKeepTime = 0.0f;
}
void CHudIndicator::Clear() {
	if (m_hFilterTex)
		glDeleteTextures(1, &m_hFilterTex);
	if (m_hFilterFBO)
		glDeleteFramebuffersEXT(1, &m_hFilterFBO);
}
void CHudIndicator::CalcuPainFade(int& r, int& g, int& b, Color* c,float timeDiffer){
	vec3_t hsv,thsv;
	int tr, tg, tb, ta;
	c->GetColor(tr, tg, tb, ta);
	mathlib::RGBToHSV(r, g, b, hsv[0], hsv[1], hsv[2]);
	mathlib::RGBToHSV(tr, tg, tb, thsv[0], thsv[1], thsv[2]);
	for (size_t i = 0; i < 3; i++) {
		thsv[i] -= (thsv[i] - hsv[i]) * timeDiffer / PainColorTime;
	}
	mathlib::HSVToRGB(thsv[0], thsv[1], thsv[2], r, g, b);
}
int CHudIndicator::Draw(float flTime) {
	if (gCustomHud.IsInSpectate())
		return 1;
	if (gCustomHud.IsHudHide(HUD_HIDEALL))
		return 1;
	DrawPain(flTime);
}
void CHudIndicator::AddIdicator(int dmg, int armor, vec3_t vecFrom) {
	float flTime = gEngfuncs.GetClientTime();
	for (indicatorinfo_t& var : m_HudIndicator.aryIndicators) {
		if (var.flKeepTime < flTime)
			continue;
		if (mathlib::VectorCompare(var.vecFrom, vecFrom)) {
			var.flKeepTime = flTime + m_HudIndicator.PainColorTime;
			return;
		}
	}
	if (gCVars.pDamageScreenFilter->value > 0) {
		m_hScreenFilter.iDamage = dmg;
		m_hScreenFilter.iArmor = armor;
		mathlib::VectorCopy(vecFrom, m_hScreenFilter.vecFrom);
		m_hScreenFilter.flKeepTime = gEngfuncs.GetClientTime() + ShockIndicatorTime;
	}
	indicatorinfo_t* pTarget = &aryIndicators[iNowSelectIndicator];
	pTarget->iDamage = dmg;
	pTarget->iArmor = armor;
	mathlib::VectorCopy(vecFrom, pTarget->vecFrom);
	pTarget->flKeepTime = gEngfuncs.GetClientTime() + PainIndicatorTime;
	iNowSelectIndicator++;
	if (iNowSelectIndicator >= NUM_MAX_INDICATOR)
		iNowSelectIndicator = 0;
	flPainColorKeepTime = gEngfuncs.GetClientTime() + PainColorTime;
}
void CHudIndicator::CalcDamageDirection(indicatorinfo_s &var){
	vec3_t vecFinal;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	vecFinal[0] = var.vecFrom[0] - local->curstate.origin[0];
	vecFinal[1] = var.vecFrom[1] - local->curstate.origin[1];
	vecFinal[2] = var.vecFrom[2] - local->curstate.origin[2];
	mathlib::VectorAngles(vecFinal, vecFinal);
	vecFinal[Q_YAW] -= local->curstate.angles[Q_YAW];
	float angle = mathlib::Q_DEG2RAD(vecFinal[Q_YAW]);
	float ca = cos(angle);
	float sa = sin(angle);
	//以屏幕中心为坐标轴的坐标系
	float sprWidth = gScreenInfo.iHeight * 0.1667;
	float y1 = gScreenInfo.iHeight / 4;
	float y2 = y1 + sprWidth;
	/*  旋转变换
	*                ^
	*                |y
	*           A----------B
	*           |    |     |
	*           C----------D
	*                |
	*  --------------+----------------->x
	*/
	//x2 = x1 * cos(alpha) - y1 * sin(alpha);
	//y2 = x1 * sin(alpha) + y1 * cos(alpha);
	mathlib::Vector2RotateCASA(var.vecHUDA, -sprWidth, y2, ca, sa);
	mathlib::Vector2RotateCASA(var.vecHUDB, sprWidth, y2, ca, sa);
	mathlib::Vector2RotateCASA(var.vecHUDC, -sprWidth, y1, ca, sa);
	mathlib::Vector2RotateCASA(var.vecHUDD, sprWidth, y1, ca, sa);
	//变换为OpenGL屏幕坐标
	mathlib::CenterPos2OpenGLPos(var.vecHUDA, ScreenWidth, ScreenHeight);
	mathlib::CenterPos2OpenGLPos(var.vecHUDB, ScreenWidth, ScreenHeight);
	mathlib::CenterPos2OpenGLPos(var.vecHUDC, ScreenWidth, ScreenHeight);
	mathlib::CenterPos2OpenGLPos(var.vecHUDD, ScreenWidth, ScreenHeight);
}
int CHudIndicator::DrawPain(float flTime){
	int r, g, b, a;
	if (gCVars.pDamageScreenFilter->value > 0 && 
		m_hScreenFilter.flKeepTime > flTime && 
		m_hScreenFilter.iDamage >= gCVars.pDamageScreenBase->value) {

		float fa = (m_hScreenFilter.flKeepTime - flTime) / ShockIndicatorTime;
		float damagefactor = gCVars.pDamageScreenFactor->value * (m_hScreenFilter.iDamage / max(gCVars.pDamageScreenBase->value, 1));
		int SizedScreenW = ScreenWidth * (1 + damagefactor) * (damagefactor * fa + 1);
		int SizedScreenH = ScreenHeight * (1 + damagefactor) * (damagefactor * fa + 1);
		fa *= 0.7;
		int wDiffer = SizedScreenW - ScreenWidth;
		int hDiffer = SizedScreenH - ScreenHeight;
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_hOldBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_hFilterFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hFilterTex, 0);
		GL_BlitFrameBufferToFrameBufferColorOnly(m_hOldBuffer, m_hFilterFBO, ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, m_hOldBuffer);
		glEnable(GL_TEXTURE_2D);
		glBind(m_hFilterTex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4ub(255, 255, 255, 255);
		GL_UseProgram(pp_colorlize.program);
		GL_Uniform2f(pp_colorlize.ha, 0, fa);
			DrawQuadPos(-wDiffer, -hDiffer, SizedScreenW, SizedScreenH);
		GL_Uniform2f(pp_colorlize.ha, 0.3, fa);
			DrawQuadPos(0, -hDiffer, SizedScreenW, SizedScreenH);
		GL_Uniform2f(pp_colorlize.ha, 0.6, fa);
			DrawQuadPos(-wDiffer, 0, SizedScreenW, SizedScreenH);
		GL_UseProgram(0);
		glDisable(GL_BLEND);
	}
	for (indicatorinfo_t var : aryIndicators) {
		if (var.flKeepTime <= flTime)
			continue;
		if (var.iDamage <= 0 && var.iArmor <= 0)
			PainIndicatorColorA.GetColor(r, g, b, a);
		else
			PainIndicatorColor.GetColor(r, g, b, a);
		CalcDamageDirection(var);
		DrawSPRIconPos(iPainIndicator, kRenderTransAdd, var.vecHUDA, var.vecHUDC, var.vecHUDD, var.vecHUDB,
			r, g, b, (var.flKeepTime - flTime) / PainIndicatorTime * a);
	}
	return 1;
}