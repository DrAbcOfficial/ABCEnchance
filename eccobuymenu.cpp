#include <metahook.h>
#include "parsemsg.h"

#include "glew.h"
#include "gl_utility.h"
#include "vguilocal.h"
#include "mathlib.h"

#include "com_model.h"
#include "gl_def.h"
#include "gl_shader.h"
#include "local.h"
#include "gl_draw.h"
#include "hud.h"
#include "weapon.h"
#include "cvardef.h"

#include "extraprecache.h"
#include "cl_entity.h"

#include "exportfuncs.h"
#include "CHudDelegate.h"

#include "eccobuymenu.h"

using namespace mathlib;

#define CAM_YAW 220
#define CAM_DIST 80
#define CAM_HEIGHT -40
#define CAM_RIGHT 50
#define ANIM_MODEL_MDEL "abcenchance/mdl/ecco_animation.mdl"

CHudEccoBuyMenu m_HudEccoBuyMenu;
enum MetaHookMsgType {
	MHSV_CMD_QUERY_PLUGIN = 1,
	MHSV_CMD_QUERY_CVAR,
	MHSV_CMD_ECCO_INFO = 7,
	MHSV_CMD_ECCO_MENU = 8
};
int __MsgFunc_MetaHook(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int type = READ_BYTE();
	switch (type) {
	case MHSV_CMD_ECCO_INFO: {
		buymenuitem_t item;
		item.id = READ_LONG();
		item.price = READ_LONG();
		item.modelindex = READ_LONG();
		item.rendermode = READ_BYTE();
		strcpy_s(item.name, READ_STRING());
		m_HudEccoBuyMenu.AddInfo(item);
		break;
	}
	case MHSV_CMD_ECCO_MENU: {
		memset(m_HudEccoBuyMenu.MenuList, -1, sizeof(m_HudEccoBuyMenu.MenuList));
		for (int l = READ_BYTE(); l > 0; l--) {
			m_HudEccoBuyMenu.MenuList[l - 1] = READ_LONG();
		}
		m_HudEccoBuyMenu.OpenMenu();
		break;
	}
	default:break;
	}
	return 0;
}

int CHudEccoBuyMenu::Init(){
	gEngfuncs.pfnHookUserMsg("MetaHook", __MsgFunc_MetaHook);

	BuyMenuAnimateTime = atof(pScheme->GetResourceString("EccoBuyMenu.BuyMenuAnimateTime"));

	BuyMenuCenterX = GET_SCREEN_PIXEL(false, "EccoBuyMenu.BuyMenuCenterX");
	BuyMenuCenterY = GET_SCREEN_PIXEL(true, "EccoBuyMenu.BuyMenuCenterY");
	BuyMenuModelSize = GET_SCREEN_PIXEL(true, "EccoBuyMenu.BuyMenuModelSize");
	BuyMenuOffset = GET_SCREEN_PIXEL(true, "EccoBuyMenu.BuyMenuOffset");
	BuyMenuHeight = GET_SCREEN_PIXEL(true, "EccoBuyMenu.BuyMenuHeight");
	BuyMenuModelX = GET_SCREEN_PIXEL(false, "EccoBuyMenu.BuyMenuModelX");
	BuyMenuModelY = GET_SCREEN_PIXEL(true, "EccoBuyMenu.BuyMenuModelY");
	BuyMenuModelY = GET_SCREEN_PIXEL(true, "EccoBuyMenu.BuyMenuModelY");

	pCVarIdealYaw = CVAR_GET_POINTER("cam_idealyaw");
	pCVarIdealDist = CVAR_GET_POINTER("cam_idealdist");
	pCVarFollowAim = CVAR_GET_POINTER("cam_followaim");

	hFont = pScheme->GetFont("HUDSmallShitFont", true);
	return 0;
}
void CHudEccoBuyMenu::VidInit() {
	Reset();
}
void CHudEccoBuyMenu::Reset() {
	if (bOpenningMenu)
		CloseMenu();
	buymenuinfo.clear();
	iAnimModelIndex = PrecacheExtraModel((char*)ANIM_MODEL_MDEL);
	iBackgroundSpr = SPR_Load("abcenchance/spr/buymenu_background.spr");
	pAnimeEntity = nullptr;
	pShowEntity = nullptr;
	pWeaponEntity = nullptr;
}
int CHudEccoBuyMenu::Draw(float flTime){
	if (!bOpenningMenu)
		return 1;
	float flAnimationRatio = (1.0f - ((max(0.0f, m_fAnimateTime - flTime)) / BuyMenuAnimateTime) / 100);
	int mousex, mousey;
	//获取鼠标指针位置
	gEngfuncs.GetMousePosition(&mousex, &mousey);
	gHudDelegate->surface()->DrawSetTexture(-1);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255);
	gHudDelegate->surface()->DrawSetTexture(gHudDelegate->m_iCursorTga);
	gHudDelegate->surface()->DrawTexturedRect(mousex, mousey, mousex + gHudDelegate->m_flCursorSize, mousey + gHudDelegate->m_flCursorSize);
	int i;
	int r = 255, g = 255, b = 255, a = 255;
	float ac, as;
	float flOffset = flAnimationRatio * BuyMenuOffset;
	int iBackGroundHeight = BuyMenuHeight;
	int iXDiffer = ScreenWidth / 2 - BuyMenuCenterX;
	int iYDiffer = ScreenHeight / 2 - BuyMenuCenterY;
	int iChosenSlot = 10;
	vec2_t aryOut[9];
	vec2_t aryIn[9];
	vec2_t vecA, vecB, vecC, vecD;
	float xpos, ypos;
	//绘制九边形
	for (i = 0; i < 9; i++) {
		double rnd = DOUBLE_PI * i / 9;
		ac = cos(rnd);
		as = sin(rnd);

		aryIn[i][0] = flOffset * ac - iXDiffer;
		aryIn[i][1] = flOffset * as - iYDiffer;

		aryOut[i][0] = (flOffset + iBackGroundHeight) * ac - iXDiffer;
		aryOut[i][1] = (flOffset + iBackGroundHeight) * as - iYDiffer;
	}
	for (i = 0; i < 9; i++) {
		//CABD
		//↓→↑
		Vector2Copy(aryIn[i == 8 ? 0 : i + 1], vecA);
		Vector2Copy(aryIn[i], vecB);
		Vector2Copy(aryOut[i == 8 ? 0 : i + 1], vecC);
		Vector2Copy(aryOut[i], vecD);
		//变换为OpenGL屏幕坐标
		CenterPos2OpenGLPos(vecA, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecB, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecC, ScreenWidth, ScreenHeight);
		CenterPos2OpenGLPos(vecD, ScreenWidth, ScreenHeight);
		bool bIsChosen = PointInPolygen(vecC, vecA, vecB, vecD, mousex, mousey);
		if (bIsChosen) {
			iChosenSlot = i + 1;
			int iChosenId = -1;
			if (MenuList[i] >= 0)
				iNowSelectedId = MenuList[i];
		}
		//CABD
		DrawSPRIconPos(iBackgroundSpr, kRenderTransAdd, vecC, vecA, vecB, vecD, r, g, b, bIsChosen ? a : a * 0.5);
		if (MenuList[i] >= 0) {
			xpos = (vecA[0] + vecB[0] + vecC[0] + vecD[0]) / 4;
			ypos = (vecA[1] + vecB[1] + vecC[1] + vecD[1]) / 4;
			buymenuitem_t* item = &buymenuinfo[MenuList[i]];
			wchar_t buf[48];
			pLocalize->ConvertANSIToUnicode(item->name, buf, sizeof(buf));
			int w, h;
			GetStringSize(buf, &w, &h, hFont);
			DrawVGUI2String(buf, xpos - w / 2, ypos - h / 2, r, g, b, hFont);
		}
	}
	iNowChosenSlot = iChosenSlot;
	//调整第三人称
	if (!bOldCamThirdperson)
		EngineClientCmd("thirdperson");
	pCVarFollowAim->value = 0;
	pCVarIdealYaw->value = CAM_YAW;
	pCVarIdealDist->value = CAM_DIST;
	gCVars.pCamIdealHeight->value = CAM_HEIGHT;
	gCVars.pCamIdealRight->value = CAM_RIGHT;

	return 0;
}
void CHudEccoBuyMenu::Clear() {
	buymenuinfo.clear();
	pAnimeEntity = nullptr;
	pShowEntity = nullptr;
	pWeaponEntity = nullptr;
}
bool CHudEccoBuyMenu::AddEntity(int type, cl_entity_s* ent, const char* modelname){
	//TODO: So ugly so shit
	if (ent == gEngfuncs.GetLocalPlayer() && bOpenningMenu) {
		if (iNowSelectedId < 0)
			return true;
		model_t* pModel = gEngfuncs.hudGetModelByIndex(iAnimModelIndex);
		if (!pModel)
			return true;
		if (!pAnimeEntity) {
			pAnimeEntity = gHookFuncs.CL_TempEntAlloc(ent->origin, pModel);
			pAnimeEntity->flags = FTENT_NONE;
			pAnimeEntity->entity.baseline.effects = EF_NODRAW;
			pAnimeEntity->die = gEngfuncs.GetClientTime() + 99999.0f;
		}
		if (!pShowEntity) {
			pShowEntity = gHookFuncs.CL_TempEntAlloc(ent->origin, pModel);
			pShowEntity->flags = FTENT_NONE;
			pShowEntity->entity.baseline.effects = EF_NODRAW;
			pShowEntity->die = gEngfuncs.GetClientTime() + 99999.0f;
		}
		if (!pWeaponEntity) {
			pWeaponEntity = gHookFuncs.CL_TempEntAlloc(ent->origin, pModel);
			pWeaponEntity->flags = FTENT_NONE;
			pWeaponEntity->entity.baseline.effects = EF_NODRAW;
			pWeaponEntity->die = gEngfuncs.GetClientTime() + 99999.0f;
		}
		buymenuitem_t* info = GetInfo(iNowSelectedId);
		switch (info->rendermode) {
		case PISTOL:
		case RIFEL:
		case GRENADE:
		case KNIFE: {
			break;
		}
		case ITEM: {
			pShowEntity->entity.curstate.effects = 0;
			pShowEntity->entity.model = gEngfuncs.hudGetModelByIndex(info->modelindex);
			VectorCopy(ent->origin, pShowEntity->entity.origin);
			VectorCopy(ent->angles, pShowEntity->entity.angles);
			break;
		}
		case NONE:
		case INVALID:
		default: break;
		}
		return false;
	}
	return true;
}
void CHudEccoBuyMenu::AddInfo(buymenuitem_t item){
	buymenuinfo.push_back(item);
}
buymenuitem_t* CHudEccoBuyMenu::GetInfo(int index) {
	if (index >= 0 && index < buymenuinfo.size())
		return &buymenuinfo[index];
	return nullptr;
}
void CHudEccoBuyMenu::OpenMenu(){
	m_fAnimateTime = gEngfuncs.GetClientTime() + BuyMenuAnimateTime;
	if (!bOpenningMenu) {
		iPlayerModelIndex = gEngfuncs.GetLocalPlayer()->curstate.modelindex;
		flOldCamYaw = pCVarIdealYaw->value;
		flOldCamDist = pCVarIdealDist->value;
		flOldCamHeight = gCVars.pCamIdealHeight->value;
		flOldCamRight = gCVars.pCamIdealRight->value;
		flOldFollowAim = pCVarFollowAim->value;
		bOldCamThirdperson = gExportfuncs.CL_IsThirdPerson();
	}
	bOpenningMenu = true;
	gHudDelegate->m_iVisibleMouse = true;
}
void CHudEccoBuyMenu::CloseMenu() {
	if (!bOpenningMenu)
		return;
	if (pAnimeEntity)
		pAnimeEntity->die = 0;
	if (pShowEntity)
		pShowEntity->die = 0;
	if (pWeaponEntity)
		pWeaponEntity->die = 0;
	pCVarIdealYaw->value = flOldCamYaw;
	pCVarIdealDist->value = flOldCamDist;
	gCVars.pCamIdealHeight->value = flOldCamHeight;
	gCVars.pCamIdealRight->value = flOldCamRight;
	pCVarFollowAim->value = flOldFollowAim;
	gEngfuncs.GetLocalPlayer()->curstate.modelindex = iPlayerModelIndex;
	if (!bOldCamThirdperson)
		EngineClientCmd("firstperson");
	bOpenningMenu = false;
	gHudDelegate->m_iVisibleMouse = false;
}
bool CHudEccoBuyMenu::SelectMenu() {
	if (!bOpenningMenu)
		return false;
	char buf[8];
	snprintf(buf, sizeof(buf), "slot%d", iNowChosenSlot);
	EngineClientCmd(buf);
	return true;
}