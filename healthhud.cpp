#include <metahook.h>
#include <parsemsg.h>
#include <cmath>
#include "mathlib.h"
#include "glew.h"

#include "hud.h"
#include "weapon.h"
#include "vguilocal.h"
#include "gl_def.h"
#include "gl_draw.h"
#include "gl_utility.h"
#include "gl_shader.h"
#include "local.h"

#include "CHudDelegate.h"

#include "healthhud.h"

#define DAMAGE_NAME "sprites/%d_dmg.spr"
using namespace mathlib;

int aryDmgFlags[NUM_DMG_TYPES] ={
	DMG_POISON,
	DMG_ACID,
	DMG_FREEZE | DMG_SLOWFREEZE,
	DMG_DROWN,
	DMG_BURN | DMG_SLOWBURN,
	DMG_NERVEGAS,
	DMG_RADIATION,
	DMG_SHOCK
};
CHudArmorHealth m_HudArmorHealth;

pfnUserMsgHook m_pfnHealth;
pfnUserMsgHook m_pfnDamage;
pfnUserMsgHook m_pfnBattery;

int __MsgFunc_Health(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int x = READ_LONG();
	if (x != m_HudArmorHealth.m_iHealth){
		m_HudArmorHealth.m_iHealth = x;
		gHudDelegate->m_iPlayerHealth = x;
	}
	return m_pfnHealth(pszName, iSize, pbuf);
}
int __MsgFunc_Damage(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int armor = READ_BYTE();
	int damageTaken = READ_BYTE();
	m_HudArmorHealth.UpdateTiles(gEngfuncs.GetClientTime(), READ_LONG());
	if (damageTaken > 0 || armor > 0) {
		vec3_t vecFrom;
		for (int i = 0; i < 3; i++) {
			vecFrom[i] = READ_COORD();
		}
		float flTime = gEngfuncs.GetClientTime();
		for (indicatorinfo_t& var : m_HudArmorHealth.aryIndicators) {
			if (var.flKeepTime < flTime)
				continue;
			if (VectorCompare(var.vecFrom, vecFrom)) {
				var.flKeepTime = flTime + m_HudArmorHealth.PainColorTime;
				return m_pfnDamage(pszName, iSize, pbuf);
			}
		}
		m_HudArmorHealth.AddIdicator(damageTaken, armor, vecFrom);
	}
	return m_pfnDamage(pszName, iSize, pbuf);
}
int __MsgFunc_Battery(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int x = READ_SHORT();
	if (x != m_HudArmorHealth.m_iBattery)
		m_HudArmorHealth.m_iBattery = x;
	return m_pfnBattery(pszName, iSize, pbuf);
}
void CHudArmorHealth::GLInit() {
	glGenFramebuffersEXT(1, &m_hFilterFBO);
	m_hFilterTex = GL_GenTextureRGBA8(ScreenWidth, ScreenHeight);
}
void CHudArmorHealth::Init(void){
	m_pfnHealth = HOOK_MESSAGE(Health);
	m_pfnDamage = HOOK_MESSAGE(Damage);
	m_pfnBattery = HOOK_MESSAGE(Battery);

	gCVars.pDamageScreenFilter = CREATE_CVAR("cl_damageshock", "0", FCVAR_VALUE, nullptr);
	gCVars.pDamageScreenFactor = CREATE_CVAR("cl_damageshock_factor", "0.015", FCVAR_VALUE, nullptr);
	gCVars.pDamageScreenBase = CREATE_CVAR("cl_damageshock_base", "30", FCVAR_VALUE, nullptr);

	StartX = GET_SCREEN_PIXEL(false, "HealthArmor.StartX");
	IconSize = GET_SCREEN_PIXEL(true, "HealthArmor.IconSize");
	BarLength = GET_SCREEN_PIXEL(false, "HealthArmor.BarLength");
	BarWidth = GET_SCREEN_PIXEL(true, "HealthArmor.BarWidth");
	ElementGap = GET_SCREEN_PIXEL(true, "HealthArmor.ElementGap");
	BackGroundY = GET_SCREEN_PIXEL(true, "HealthArmor.BackGroundY");
	BackGroundLength = GET_SCREEN_PIXEL(false, "HealthArmor.BackGroundLength");
	DamageIconX = GET_SCREEN_PIXEL(false, "HealthArmor.DamageIconX");
	DamageIconY = GET_SCREEN_PIXEL(true, "HealthArmor.DamageIconY");
	DamageIconSize = GET_SCREEN_PIXEL(true, "HealthArmor.DamageIconSize");

	PainColorTime = atof(pScheme->GetResourceString("HealthArmor.PainColorTime"));
	PainIndicatorTime = atof(pScheme->GetResourceString("HealthArmor.PainIndicatorTime"));
	ShockIndicatorTime = atof(pScheme->GetResourceString("HealthArmor.ShockIndicatorTime"));

	HealthIconColor = pScheme->GetColor("HealthArmor.HealthIconColor", gDefaultColor);
	HealthBarColor = pScheme->GetColor("HealthArmor.HealthBarColor", gDefaultColor);
	HealthTextColor = pScheme->GetColor("HealthArmor.HealthTextColor", gDefaultColor);
	BitDamageColor = pScheme->GetColor("HealthArmor.BitDamageColor", gDefaultColor);
	HealthPainColor = pScheme->GetColor("HealthArmor.HealthPainColor", gDefaultColor);
	HealthDangerColor = pScheme->GetColor("HealthArmor.HealthDangerColor", gDefaultColor);

	ArmorIconColor = pScheme->GetColor("HealthArmor.ArmorIconColor", gDefaultColor);
	ArmorBarColor = pScheme->GetColor("HealthArmor.ArmorBarColor", gDefaultColor);
	ArmorTextColor = pScheme->GetColor("HealthArmor.ArmorTextColor", gDefaultColor);
	ArmorPainColor = pScheme->GetColor("HealthArmor.ArmorPainColor", gDefaultColor);
	ArmorDangerColor = pScheme->GetColor("HealthArmor.ArmorDangerColor", gDefaultColor);

	LongjumpIconColor = pScheme->GetColor("HealthArmor.LongjumpIconColor", gDefaultColor);

	PainIndicatorColor = pScheme->GetColor("HealthArmor.PainIndicatorColor", gDefaultColor);
	PainIndicatorColorA = pScheme->GetColor("HealthArmor.PainIndicatorColorA", gDefaultColor);

	HUDFont = pScheme->GetFont("HUDShitFont", true);

	memset(m_dmg, 0, sizeof(DAMAGE_IMAGE) * NUM_DMG_TYPES);

	Reset();
}
int CHudArmorHealth::VidInit(void){
	m_hSprite = 0;
	m_iDMGIconStart = gHudDelegate->GetSpriteIndex("dmg_bio") + 1;
	return 1;
}
void CHudArmorHealth::Reset(void){
	iHealthIcon = SPR_Load("abcenchance/spr/icon-cross1.spr");
	iArmorIconNull = SPR_Load("abcenchance/spr/icon-shield.spr");
	iArmorIconFull = SPR_Load("abcenchance/spr/icon-armor-helmet.spr");
	iLongjumpIcon = SPR_Load("abcenchance/spr/icon-longjump.spr");
	iPainIndicator = SPR_Load("abcenchance/spr/pain_indicator.spr");
	VGUI_CREATE_NEWTGA_TEXTURE(iHealthBarBackground, "abcenchance/tga/healthbar_background");

	memset(aryIndicators, 0, sizeof(aryIndicators));
	iNowSelectIndicator = 0;

	m_iHealth = 100;
	m_iBattery = 0;
	m_bitsDamage = 0;
	flPainColorKeepTime = 0.0f;
	for (int i = 0; i < NUM_DMG_TYPES; i++){
		m_dmg[i].fExpire = 0;
	}
}
void CHudArmorHealth::CalcuPainFade(int& r, int& g, int& b, Color* c,float timeDiffer){
	vec3_t hsv,thsv;
	int tr, tg, tb, ta;
	c->GetColor(tr, tg, tb, ta);
	RGBToHSV(r, g, b, hsv[0], hsv[1], hsv[2]);
	RGBToHSV(tr, tg, tb, thsv[0], thsv[1], thsv[2]);
	for (int i = 0; i < 3; i++) {
		thsv[i] -= (thsv[i] - hsv[i]) * timeDiffer / PainColorTime;
	}
	HSVToRGB(thsv[0], thsv[1], thsv[2], r, g, b);
}
int CHudArmorHealth::Draw(float flTime){
	if (gHudDelegate->IsInSpectate())
		return 1;
	DrawPain(flTime);
	int r, g, b, a;
	float flBackGroundY = BackGroundY;
	gHudDelegate->surface()->DrawSetTexture(-1);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255);
	gHudDelegate->surface()->DrawSetTexture(iHealthBarBackground);
	gHudDelegate->surface()->DrawTexturedRect(0, flBackGroundY, BackGroundLength, gScreenInfo.iHeight);

	float flBackGroundHeight = gScreenInfo.iHeight - flBackGroundY;
	float flCenterY = gScreenInfo.iHeight - flBackGroundHeight / 2;
	int iStartX = StartX;
	int iHealth = m_iHealth;
	int iBattery = m_iBattery;
	//HP ICON
	if (iHealth <= DAGER_HEALTH)
		HealthDangerColor.GetColor(r, g, b, a);
	else
		HealthIconColor.GetColor(r, g, b, a);
	DrawSPRIcon(iHealthIcon, kRenderTransAdd, iStartX, flCenterY - IconSize / 2, IconSize, IconSize, r, g, b, a);

	wchar_t wideName[8];
	wsprintfW(wideName, L"%d", iHealth);
	int iTextWidth, iTextHeight;
	GetStringSize(wideName, &iTextWidth, &iTextHeight, HUDFont);
	iStartX += IconSize + ElementGap;
	if(iHealth <= DAGER_HEALTH)
		HealthDangerColor.GetColor(r, g, b, a);
	else
		HealthTextColor.GetColor(r, g, b, a);
	DrawVGUI2String(wideName, iStartX, flCenterY - iTextHeight / 2, r, g, b, HUDFont);
	iStartX += iTextWidth + ElementGap;
	if (flTime < flPainColorKeepTime){
		HealthPainColor.GetColor(r, g, b, a);
		CalcuPainFade(r, g, b, &HealthBarColor, flPainColorKeepTime - flTime);
	}
	else{
		if (iHealth <= DAGER_HEALTH)
			HealthDangerColor.GetColor(r, g, b, a);
		else
			HealthBarColor.GetColor(r, g, b, a);
	}
	gEngfuncs.pfnFillRGBABlend(iStartX, flCenterY - BarWidth / 2,
		BarLength, BarWidth, r / 2, g / 2, b / 2, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flCenterY - BarWidth / 2,
		BarLength * clamp((float)iHealth / 100, 0.0f, 1.0f), BarWidth, r, g, b, a);
	iStartX += BarLength + ElementGap * 4;

	//AP
	if (iBattery <= DAGER_HEALTH)
		ArmorDangerColor.GetColor(r, g, b, a);
	else
		ArmorIconColor.GetColor(r, g, b, a);
	DrawSPRIcon(iBattery > 0 ? iArmorIconFull : iArmorIconNull, kRenderTransAdd, iStartX, flCenterY - IconSize / 2, IconSize, IconSize, r, g, b, a);

	wsprintfW(wideName, L"%d", iBattery);
	GetStringSize(wideName, &iTextWidth, &iTextHeight, HUDFont);
	iStartX += IconSize + ElementGap;
	if (iBattery <= DAGER_HEALTH)
		ArmorDangerColor.GetColor(r, g, b, a);
	else
		ArmorTextColor.GetColor(r, g, b, a);
	DrawVGUI2String(wideName, iStartX, flCenterY - iTextHeight / 2, r, g, b, HUDFont);
	iStartX += iTextWidth + ElementGap;

	if (flTime < flPainColorKeepTime){
		ArmorPainColor.GetColor(r, g, b, a);
		CalcuPainFade(r, g, b, &ArmorBarColor, flPainColorKeepTime - flTime);
	}
	else{
		if (iBattery <= DAGER_HEALTH)
			ArmorDangerColor.GetColor(r, g, b, a);
		else
			ArmorBarColor.GetColor(r, g, b, a);
	}
	gEngfuncs.pfnFillRGBABlend(iStartX, flCenterY - BarWidth / 2,
		BarLength, BarWidth, r / 2, g / 2, b / 2, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flCenterY - BarWidth / 2,
		BarLength * clamp((float)iBattery / 100, 0.0f, 1.0f), BarWidth, r, g, b, a);
	if (gHudDelegate->m_bPlayerLongjump) {
		iStartX += BarLength + ElementGap * 2;
		LongjumpIconColor.GetColor(r, g, b, a);
		DrawSPRIcon(iLongjumpIcon, kRenderTransAdd, iStartX, flCenterY - IconSize / 2, IconSize, IconSize, r, g, b, a);
	}
	return DrawDamage(flTime);
}
void CHudArmorHealth::AddIdicator(int dmg, int armor, vec3_t vecFrom) {
	if (gCVars.pDamageScreenFilter->value > 0) {
		m_hScreenFilter.iDamage = dmg;
		m_hScreenFilter.iArmor = armor;
		VectorCopy(vecFrom, m_hScreenFilter.vecFrom);
		m_hScreenFilter.flKeepTime = gEngfuncs.GetClientTime() + ShockIndicatorTime;
	}
	indicatorinfo_t* pTarget = &aryIndicators[iNowSelectIndicator];
	pTarget->iDamage = dmg;
	pTarget->iArmor = armor;
	VectorCopy(vecFrom, pTarget->vecFrom);
	pTarget->flKeepTime = gEngfuncs.GetClientTime() + PainIndicatorTime;
	iNowSelectIndicator++;
	if (iNowSelectIndicator >= NUM_MAX_INDICATOR)
		iNowSelectIndicator = 0;
	flPainColorKeepTime = gEngfuncs.GetClientTime() + PainColorTime;
}
void CHudArmorHealth::CalcDamageDirection(indicatorinfo_s &var){
	vec3_t vecFinal;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	vecFinal[0] = var.vecFrom[0] - local->curstate.origin[0];
	vecFinal[1] = var.vecFrom[1] - local->curstate.origin[1];
	vecFinal[2] = var.vecFrom[2] - local->curstate.origin[2];
	VectorAngles(vecFinal, vecFinal);
	vecFinal[YAW] -= local->curstate.angles[YAW];
	float angle = DEG2RAD(vecFinal[YAW]);
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
	Vector2RotateCASA(var.vecHUDA, -sprWidth, y2, ca, sa);
	Vector2RotateCASA(var.vecHUDB, sprWidth, y2, ca, sa);
	Vector2RotateCASA(var.vecHUDC, -sprWidth, y1, ca, sa);
	Vector2RotateCASA(var.vecHUDD, sprWidth, y1, ca, sa);
	//变换为OpenGL屏幕坐标
	CenterPos2OpenGLPos(var.vecHUDA, ScreenWidth, ScreenHeight);
	CenterPos2OpenGLPos(var.vecHUDB, ScreenWidth, ScreenHeight);
	CenterPos2OpenGLPos(var.vecHUDC, ScreenWidth, ScreenHeight);
	CenterPos2OpenGLPos(var.vecHUDD, ScreenWidth, ScreenHeight);
}
int CHudArmorHealth::DrawPain(float flTime){
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
		glBindTexture(GL_TEXTURE_2D, m_hFilterTex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GL_UseProgram(pp_coloraddictive.program);
		GL_Uniform2f(pp_coloraddictive.ha, 0, fa);
			DrawQuadPos(-wDiffer, -hDiffer, SizedScreenW, SizedScreenH);
		GL_Uniform2f(pp_coloraddictive.ha, 0.3, fa);
			DrawQuadPos(0, -hDiffer, SizedScreenW, SizedScreenH);
		GL_Uniform2f(pp_coloraddictive.ha, 0.6, fa);
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
int CHudArmorHealth::DrawDamage(float flTime){
	int r, g, b, a;
	DAMAGE_IMAGE* pdmg;
	if (!m_bitsDamage)
		return 1;
	BitDamageColor.GetColor(r, g, b, a);
	a = (int)(fabs(sin(flTime * 2)) * 256.0);
	int i;
	for (i = 0; i < NUM_DMG_TYPES; i++){
		if (m_bitsDamage & aryDmgFlags[i]){
			pdmg = &m_dmg[i];
			SPR_Set(gHudDelegate->GetSprite(m_iDMGIconStart + i), r, g, b);
			SPR_DrawAdditive(0, pdmg->x, pdmg->y, gHudDelegate->GetSpriteRect(m_iDMGIconStart + i));
		}
	}
	for (i = 0; i < NUM_DMG_TYPES; i++){
		DAMAGE_IMAGE* pdmg = &m_dmg[i];
		if (m_bitsDamage & aryDmgFlags[i]){
			pdmg->fExpire = min(flTime + DMG_IMAGE_LIFE, pdmg->fExpire);
			if (pdmg->fExpire <= flTime	&& a < 40){
				pdmg->fExpire = 0;
				int y = pdmg->y;
				pdmg->x = pdmg->y = 0;
				for (int j = 0; j < NUM_DMG_TYPES; j++){
					pdmg = &m_dmg[j];
					if ((pdmg->y) && (pdmg->y < y))
						pdmg->y += DamageIconSize;
				}
				m_bitsDamage &= ~aryDmgFlags[i];
			}
		}
	}
	return 1;
}
void CHudArmorHealth::UpdateTiles(float flTime, long bitsDamage){
	DAMAGE_IMAGE* pdmg;
	long bitsOn = ~m_bitsDamage & bitsDamage;
	for (int i = 0; i < NUM_DMG_TYPES; i++){
		pdmg = &m_dmg[i];
		if (m_bitsDamage & aryDmgFlags[i]){
			pdmg->fExpire = flTime + DMG_IMAGE_LIFE;
			if (!pdmg->fBaseline)
				pdmg->fBaseline = flTime;
		}
		if (bitsOn & aryDmgFlags[i]){
			pdmg->x = DamageIconX;
			pdmg->y = DamageIconY - DamageIconSize * 2;
			pdmg->fExpire = flTime + DMG_IMAGE_LIFE;
			for (int j = 0; j < NUM_DMG_TYPES; j++){
				if (j == i)
					continue;
				pdmg = &m_dmg[j];
				if (pdmg->y)
					pdmg->y -= DamageIconSize;

			}
		}
	}
	m_bitsDamage |= bitsDamage;
}