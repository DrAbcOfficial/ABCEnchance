#include <metahook.h>
#include <parsemsg.h>
#include <math.h>

#include "CColor.h"  
#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>

#include "mathlib.h"
#include "msghook.h"
#include "hud.h"
#include "healthhud.h"
#include "vguilocal.h"
#include "drawElement.h"

#define DAMAGE_NAME "sprites/%d_dmg.spr"
int giDmgHeight, giDmgWidth;
int giDmgFlags[NUM_DMG_TYPES] =
{
	DMG_POISON,
	DMG_ACID,
	DMG_FREEZE | DMG_SLOWFREEZE,
	DMG_DROWN,
	DMG_BURN | DMG_SLOWBURN,
	DMG_NERVEGAS,
	DMG_RADIATION,
	DMG_SHOCK,
	DMG_CALTROP,
	DMG_TRANQ,
	DMG_CONCUSS,
	DMG_HALLUC
};
CHudArmorHealth m_HudArmorHealth;

int __MsgFunc_Health(const char* pszName, int iSize, void* pbuf)
{
	// TODO: update local health data
	BEGIN_READ(pbuf, iSize);
	int x = READ_BYTE();

	m_HudArmorHealth.m_iFlags |= HUD_ACTIVE;

	// Only update the fade if we've changed health
	if (x != m_HudArmorHealth.m_iHealth)
	{
		m_HudArmorHealth.m_fFade = FADE_TIME;
		m_HudArmorHealth.m_iHealth = x;
	}

	return 1;
}

int __MsgFunc_Damage(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int armor = READ_BYTE();	// armor
	int damageTaken = READ_BYTE();	// health
	long bitsDamage = READ_LONG(); // damage bits

	vec3_t vecFrom;

	for (int i = 0; i < 3; i++)
		vecFrom[i] = READ_COORD();

	// Actually took damage?
	if (damageTaken > 0 || armor > 0)
	{
		m_HudArmorHealth.m_takeDamage = damageTaken;
		m_HudArmorHealth.m_takeArmor = armor;
		m_HudArmorHealth.CalcDamageDirection(vecFrom);
	}
	return 1;
}

int __MsgFunc_Battery(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int x = READ_SHORT();

	if (x != m_HudArmorHealth.m_iBat)
	{
		m_HudArmorHealth.m_fFade = FADE_TIME;
		m_HudArmorHealth.m_iBat = x;
	}
	return 1;
}

int CHudArmorHealth::Init(void)
{
	HOOK_MESSAGE(Health);
	HOOK_MESSAGE(Damage);
	HOOK_MESSAGE(Battery);
	m_iHealth = 100;
	m_fFade = 0;
	m_iFlags = 0;
	m_bitsDamage = 0;
	m_takeDamage = 0;
	m_takeArmor = 0;
	m_iBat = 0;
	memset(m_dmg, 0, sizeof(DAMAGE_IMAGE) * NUM_DMG_TYPES);

	iHealthIcon = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-cross1.spr");
	iArmorIconNull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-shield.spr");
	iArmorIconFull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-armor-helmet.spr");
	iPainIndicator = gEngfuncs.pfnSPR_Load("abcenchance/spr/pain_indicator.spr");

	StartX = atof(pScheme->GetResourceString("HealthArmor.StartX"));
	IconSize = atof(pScheme->GetResourceString("HealthArmor.IconSize"));
	TextWidth = atof(pScheme->GetResourceString("HealthArmor.TextWidth"));
	IconSize = atof(pScheme->GetResourceString("HealthArmor.IconSize"));
	BarLength = atof(pScheme->GetResourceString("HealthArmor.BarLength"));
	BarWidth = atof(pScheme->GetResourceString("HealthArmor.BarWidth"));
	ElementGap = atof(pScheme->GetResourceString("HealthArmor.ElementGap"));
	BackGroundY = atof(pScheme->GetResourceString("HealthArmor.BackGroundY"));
	BackGroundLength = atof(pScheme->GetResourceString("HealthArmor.BackGroundLength"));
	BackGroundAlpha = atof(pScheme->GetResourceString("HealthArmor.BackGroundAlpha"));

	HealthIconColor = pScheme->GetColor("HealthArmor.HealthIconColor", gDefaultColor);
	HealthBarColor = pScheme->GetColor("HealthArmor.HealthBarColor", gDefaultColor);
	HealthTextColor = pScheme->GetColor("HealthArmor.HealthTextColor", gDefaultColor);
	ArmorIconColor = pScheme->GetColor("HealthArmor.ArmorIconColor", gDefaultColor);
	ArmorBarColor = pScheme->GetColor("HealthArmor.ArmorBarColor", gDefaultColor);
	ArmorTextColor = pScheme->GetColor("HealthArmor.ArmorTextColor", gDefaultColor);
	BackGroundColor = pScheme->GetColor("HealthArmor.BackGroundColor", gDefaultColor);

	HUDFont = pScheme->GetFont("HUDShitFont", true);
	HUDBigFont = pScheme->GetFont("HUDShitFont", true);
	return 1;
}

void CHudArmorHealth::Reset(void)
{
	iHealthIcon = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-cross1.spr");
	iArmorIconNull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-shield.spr");
	iArmorIconFull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-armor-helmet.spr");
	iPainIndicator = gEngfuncs.pfnSPR_Load("abcenchance/spr/pain_indicator.spr");

	m_iHealth = 100;
	m_fFade = 0;
	m_iFlags = 0;
	m_bitsDamage = 0;
	m_takeDamage = 0;
	m_takeArmor = 0;
	m_bitsDamage = 0;
	m_iBat = 0;
	for (int i = 0; i < NUM_DMG_TYPES; i++)
	{
		m_dmg[i].fExpire = 0;
	}
}

int CHudArmorHealth::Draw(void)
{
	int r, g, b, a;
	float iSizeStep = (float)gScreenInfo.iWidth / 3 / BackGroundAlpha;
	float i = 0, nowX = 0;
	float flBackGroundY = gScreenInfo.iHeight * BackGroundY;
	float flBackGroundHeight = gScreenInfo.iHeight - flBackGroundY;
	BackGroundColor.GetColor(r, g, b, a);
	for (i = BackGroundAlpha; i > 0.0f; i--)
	{
		gEngfuncs.pfnFillRGBABlend(nowX, flBackGroundY, iSizeStep, flBackGroundHeight, r, g, b, i);
		nowX += iSizeStep;
	}
	int iStartX = gScreenInfo.iWidth / StartX;
	int iIconSize = flBackGroundHeight * IconSize;
	int iTextWidth = flBackGroundHeight * TextWidth;
	int iTextHeight = flBackGroundHeight * TextHeight;
	int iBarLength = flBackGroundHeight * BarLength;
	int iBarWidth = flBackGroundHeight * BarWidth;
	int iElementGap = flBackGroundHeight * ElementGap;
	int iHealth = m_iHealth;
	int iBattery = m_iBat;
	//HP ICON
	if (!iHealthIcon)
		iHealthIcon = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-cross1.spr");
	HealthIconColor.GetColor(r, g, b, a);
	DrawSPRIcon(iHealthIcon, iStartX, flBackGroundY + (flBackGroundHeight - iIconSize) / 2, iIconSize, iIconSize, r, g, b, a);

	char numberString[16];
	itoa(iHealth, numberString, 10);
	wchar_t wideName[8];
	pLocalize->ConvertANSIToUnicode(numberString, wideName, sizeof(wideName));
	int iSzWidth = 0;
	GetStringSize(wideName, &iSzWidth, NULL, HUDFont);
	iStartX += iIconSize + iElementGap + iTextWidth - iSzWidth;
	HealthTextColor.GetColor(r, g, b, a);
	DrawVGUI2String(wideName, iStartX, flBackGroundY + (flBackGroundHeight - iTextHeight) / 2, r, g, b, HUDFont);
	iStartX += iSzWidth + iElementGap;

	HealthBarColor.GetColor(r, g, b, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flBackGroundY + (flBackGroundHeight - iBarWidth) / 2, iBarLength, iBarWidth, r / 2, g / 2, b / 2, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flBackGroundY + (flBackGroundHeight - iBarWidth) / 2, iBarLength * max(0, min(1, (float)iHealth / 100)), iBarWidth, r, g, b, a);
	iStartX += iBarLength + iElementGap * 4;

	//AP
	if (!iArmorIconNull || !iArmorIconFull)
		iArmorIconNull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-shield.spr");
	if (!iArmorIconFull)
		iArmorIconFull = gEngfuncs.pfnSPR_Load("abcenchance/spr/icon-armor-helmet.spr");
	ArmorIconColor.GetColor(r, g, b, a);
	DrawSPRIcon(iBattery > 0 ? iArmorIconFull : iArmorIconNull, iStartX, flBackGroundY + (flBackGroundHeight - iIconSize) / 2, iIconSize, iIconSize, r, g, b, a);

	itoa(iBattery, numberString, 10);
	pLocalize->ConvertANSIToUnicode(numberString, wideName, sizeof(wideName));
	GetStringSize(wideName, &iSzWidth, NULL, HUDFont);
	iStartX += iIconSize + iElementGap + iTextWidth - iSzWidth;
	ArmorTextColor.GetColor(r, g, b, a);
	DrawVGUI2String(wideName, iStartX, flBackGroundY + (flBackGroundHeight - iTextHeight) / 2, r, g, b, HUDFont);
	iStartX += iSzWidth + iElementGap;

	ArmorBarColor.GetColor(r, g, b, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flBackGroundY + (flBackGroundHeight - iBarWidth) / 2, iBarLength, iBarWidth, r / 2, g / 2, b / 2, a);
	gEngfuncs.pfnFillRGBABlend(iStartX, flBackGroundY + (flBackGroundHeight - iBarWidth) / 2, iBarLength * max(0, min(1, (float)iBattery / 100)), iBarWidth, r, g, b, a);
	iStartX += iBarLength + iElementGap * 2;
	return 1;
}

void CHudArmorHealth::GetPainColor(int& r, int& g, int& b)
{
}
void CHudArmorHealth::CalcDamageDirection(vec3_t vecFrom)
{
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	vec3_t vecCalc;
	vecCalc[0] = vecFrom[0] - local->curstate.origin[0];
	vecCalc[1] = vecFrom[1] - local->curstate.origin[1];
	vecCalc[2] = vecFrom[2] - local->curstate.origin[2];
	vec3_t vecAngles;
	VectorAngles(vecCalc, vecAngles);
	float yaw = vecAngles[1] - local->curstate.angles[1];
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
	vec2_t vecA, vecB, vecC, vecD;
	vecA[0] = -sprWidth * cos(yaw) + y2 * sin(yaw);
	vecA[1] = sprWidth * sin(yaw) + y2 * cos(yaw);
	vecB[0] = sprWidth * cos(yaw) + y2 * sin(yaw);
	vecB[1] = -sprWidth * sin(yaw) + y2 * cos(yaw);
	vecC[0] = -sprWidth * cos(yaw) + y1 * sin(yaw);
	vecC[1] = sprWidth * sin(yaw) + y1 * cos(yaw);
	vecD[0] = sprWidth * cos(yaw) + y1 * sin(yaw);
	vecD[1] = -sprWidth * sin(yaw) + y1 * cos(yaw);
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

int CHudArmorHealth::DrawPain(float flTime)
{
	return 1;
}
int CHudArmorHealth::DrawDamage(float flTime)
{
	int r, g, b, a;
	DAMAGE_IMAGE* pdmg;

	if (!m_bitsDamage)
		return 1;
	return 1;
}
void CHudArmorHealth::UpdateTiles(float flTime, long bitsDamage)
{
	DAMAGE_IMAGE* pdmg;
	// damage bits are only turned on here;  they are turned off when the draw time has expired (in DrawDamage())
	m_bitsDamage |= bitsDamage;
}