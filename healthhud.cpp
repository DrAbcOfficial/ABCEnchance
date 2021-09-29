#include <metahook.h>
#include <parsemsg.h>
#include <cmath>
#include "mathlib.h"
#include "msghook.h"
#include "glew.h"

#include "hud.h"
#include "weapon.h"
#include "vguilocal.h"
#include "myconst.h"

#include "CHudDelegate.h"
#include "drawElement.h"
#include "utility.h"

#include "healthhud.h"

#define DAMAGE_NAME "sprites/%d_dmg.spr"
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
		for (int i = 0; i < 3; i++) {
			m_HudArmorHealth.vecDamageFrom[i] = READ_COORD();
		}
		m_HudArmorHealth.m_takeDamage = damageTaken;
		m_HudArmorHealth.m_takeArmor = armor;
		m_HudArmorHealth.flPainIndicatorKeepTime = gEngfuncs.GetClientTime() + m_HudArmorHealth.PainIndicatorTime;
		m_HudArmorHealth.flPainColorKeepTime = gEngfuncs.GetClientTime() + m_HudArmorHealth.PainColorTime;
	}
	return m_pfnDamage(pszName, iSize, pbuf);
}
int __MsgFunc_Battery(const char* pszName, int iSize, void* pbuf){
	BEGIN_READ(pbuf, iSize);
	int x = READ_SHORT();
	if (x != m_HudArmorHealth.m_iBat)
		m_HudArmorHealth.m_iBat = x;
	return m_pfnBattery(pszName, iSize, pbuf);
}

void CHudArmorHealth::Init(void){
	m_pfnHealth = HOOK_MESSAGE(Health);
	m_pfnDamage = HOOK_MESSAGE(Damage);
	m_pfnBattery = HOOK_MESSAGE(Battery);

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
	m_HUD_dmg_bio = gHudDelegate->GetSpriteIndex("dmg_bio") + 1;
	return 1;
}
void CHudArmorHealth::Reset(void){
	iHealthIcon = SPR_Load("abcenchance/spr/icon-cross1.spr");
	iArmorIconNull = SPR_Load("abcenchance/spr/icon-shield.spr");
	iArmorIconFull = SPR_Load("abcenchance/spr/icon-armor-helmet.spr");
	iLongjumpIcon = SPR_Load("abcenchance/spr/icon-longjump.spr");
	iPainIndicator = SPR_Load("abcenchance/spr/pain_indicator.spr");
	VGUI_CREATE_NEWTGA_TEXTURE(iHealthBarBackground, "abcenchance/tga/healthbar_background");
	m_iHealth = 100;
	m_bitsDamage = 0;
	m_takeDamage = 0;
	m_takeArmor = 0;
	m_iBat = 0;
	flPainIndicatorKeepTime = 0.0f;
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
	if (flTime < flPainIndicatorKeepTime)
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
	int iBattery = m_iBat;
	//HP ICON
	if (iHealth <= DAGER_HEALTH)
		HealthDangerColor.GetColor(r, g, b, a);
	else
		HealthIconColor.GetColor(r, g, b, a);
	DrawSPRIcon(iHealthIcon, iStartX, flCenterY - IconSize / 2, IconSize, IconSize, r, g, b, a);

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
	DrawSPRIcon(iBattery > 0 ? iArmorIconFull : iArmorIconNull, iStartX, flCenterY - IconSize / 2, IconSize, IconSize, r, g, b, a);

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
		DrawSPRIcon(iLongjumpIcon, iStartX, flCenterY - IconSize / 2, IconSize, IconSize, r, g, b, a);
	}
	return DrawDamage(flTime);
}
void CHudArmorHealth::CalcDamageDirection(){
	vec3_t vecFinal;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	vecFinal[0] = vecDamageFrom[0] - local->curstate.origin[0];
	vecFinal[1] = vecDamageFrom[1] - local->curstate.origin[1];
	vecFinal[2] = vecDamageFrom[2] - local->curstate.origin[2];
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
	vec2_t vecA, vecB, vecC, vecD;
	vecA[0] = -sprWidth * ca - y2 * sa;
	vecA[1] = -sprWidth * sa + y2 * ca;
	vecB[0] = sprWidth * ca - y2 * sa;
	vecB[1] = sprWidth * sa + y2 * ca;
	vecC[0] = -sprWidth * ca - y1 * sa;
	vecC[1] = -sprWidth * sa + y1 * ca;
	vecD[0] = sprWidth * ca - y1 * sa;
	vecD[1] = sprWidth * sa + y1 * ca;
	//变换为OpenGL屏幕坐标
	int halfWidth = gScreenInfo.iWidth / 2;
	int halfHeight = gScreenInfo.iHeight / 2;
	vecA[0] += halfWidth;
	vecA[1] = halfHeight - vecA[1];
	vecB[0] += halfWidth;
	vecB[1] = halfHeight - vecB[1];
	vecC[0] += halfWidth;
	vecC[1] = halfHeight - vecC[1];
	vecD[0] += halfWidth;
	vecD[1] = halfHeight - vecD[1];

	Vector2Copy(vecA, vecPainIndicatorA);
	Vector2Copy(vecB, vecPainIndicatorB);
	Vector2Copy(vecC, vecPainIndicatorC);
	Vector2Copy(vecD, vecPainIndicatorD);
}
int CHudArmorHealth::DrawPain(float flTime){
	int r, g, b, a;
	if(m_takeDamage <= 0 && m_takeArmor <= 0)
		PainIndicatorColorA.GetColor(r, g, b, a);
	else
		PainIndicatorColor.GetColor(r, g, b, a);
	CalcDamageDirection();
	DrawSPRIconPos(iPainIndicator, vecPainIndicatorA, vecPainIndicatorC, vecPainIndicatorD, vecPainIndicatorB, 
		r, g, b, 
		(flPainIndicatorKeepTime - flTime) / PainIndicatorTime * a);
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
			SPR_Set(gHudDelegate->GetSprite(m_HUD_dmg_bio + i), r, g, b);
			SPR_DrawAdditive(0, pdmg->x, pdmg->y, &gHudDelegate->GetSpriteRect(m_HUD_dmg_bio + i));
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