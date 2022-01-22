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
#include "dlight.h"

#include "extraprecache.h"
#include "cl_entity.h"

#include "exportfuncs.h"
#include "CHudDelegate.h"

#include "eccobuymenu.h"

using namespace mathlib;

#define BASE_INDEX_TEMP 7355608
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
enum EccoRenderMode {
	INVALID = -4,
	NONE = -3,
	ITEM = -2
};
enum EccoMenuMode {
	MENU_INVALID = -1,
	MENU_BACK = -2,
	MENU_NEXTPAGE = -3,
	MENU_LASTPAGE = -4
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
		item.sequence = READ_LONG();
		strcpy_s(item.name, READ_STRING());
		m_HudEccoBuyMenu.AddInfo(item);
		break;
	}
	case MHSV_CMD_ECCO_MENU: {
		m_HudEccoBuyMenu.MenuList.clear();
		int pageLen = READ_BYTE();
		m_HudEccoBuyMenu.MenuList.resize((size_t)pageLen);
		for (int i = 0; i < pageLen; i++) {
			m_HudEccoBuyMenu.MenuList[i] = READ_LONG();
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

	TextColor = pScheme->GetColor("BuyMenu.TextColor", gDefaultColor);
	ButtonColor = pScheme->GetColor("BuyMenu.ButtonColor", gDefaultColor);

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
	MenuList.clear();
	iBackgroundSpr = SPR_Load("abcenchance/spr/buymenu_background.spr");
	VGUI_CREATE_NEWTGA_TEXTURE(iBackgroundTga, "abcenchance/tga/buymenu_background");
	pShowEnt = nullptr;
	pWeaponEnt = nullptr;
	pLight = nullptr;
}
int CHudEccoBuyMenu::GetMenuId(int i) {
	int realId = i + (iNextPageBase * 9);
	return realId >= 0 && realId < MenuList.size() ? MenuList[realId] : -1;
}
int CHudEccoBuyMenu::Draw(float flTime){
	if (!bOpenningMenu)
		return 1;
	
	float flAnimationRatio = min(1.0f, 1.0f - ((m_fAnimateTime - flTime) / BuyMenuAnimateTime));
	//大背景
	gHudDelegate->surface()->DrawSetTexture(-1);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255 * flAnimationRatio);
	gHudDelegate->surface()->DrawSetTexture(iBackgroundTga);
	gHudDelegate->surface()->DrawTexturedRect(0, 0, ScreenWidth / 2, ScreenHeight);
	int mousex, mousey;
	//获取鼠标指针位置
	gEngfuncs.GetMousePosition(&mousex, &mousey);
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
	int iChosenId = -1;
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
		int realId = GetMenuId(i);
		if (bIsChosen) {
			iChosenSlot = i + 1;
			iChosenId = realId;
		}
		//CABD
		ButtonColor.GetColor(r, g, b, a);
		DrawSPRIconPos(iBackgroundSpr, kRenderTransAdd, vecC, vecA, vecB, vecD, r, g, b, bIsChosen ? a : a * 0.5);
		xpos = (vecA[0] + vecB[0] + vecC[0] + vecD[0]) / 4;
		ypos = (vecA[1] + vecB[1] + vecC[1] + vecD[1]) / 4;

		int w, h;
		if (realId >= 0) {
			buymenuitem_t* item = &buymenuinfo[realId];
			wchar_t buf[48];
			pLocalize->ConvertANSIToUnicode(item->name, buf, sizeof(buf));
			GetStringSize(buf, &w, &h, hFont);
			TextColor.GetColor(r, g, b, a);
			DrawVGUI2String(buf, xpos - w / 2, ypos - h / 2, r, g, b, hFont);
			if (bIsChosen)
				DrawVGUI2String(buf, BuyMenuCenterX - w / 2, BuyMenuCenterY - h / 2, r, g, b, hFont);
		}
		else {
			wchar_t* buf = nullptr;
			switch (realId){
				case MENU_BACK: buf = pLocalize->Find("BuyMenu_LastMenu"); break;
				case MENU_NEXTPAGE: buf = pLocalize->Find("BuyMenu_NextPage"); break;
				case MENU_LASTPAGE: buf = pLocalize->Find("BuyMenu_LastPage"); break;
			}
			if (realId != MENU_INVALID) {
				GetStringSize(buf, &w, &h, hFont);
				TextColor.GetColor(r, g, b, a);
				DrawVGUI2String(buf, xpos - w / 2, ypos - h / 2, r, g, b, hFont);
			}
		}
	}
	iNowSelectedId = iChosenId;
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
	MenuList.clear();
	pShowEnt = nullptr;
	pWeaponEnt = nullptr;
	pLight = nullptr;
}
bool CHudEccoBuyMenu::AddEntity(int type, cl_entity_s* ent, const char* modelname){
	if (ent == gEngfuncs.GetLocalPlayer() && bOpenningMenu) {
		ClearTempEnt();
		if (!pLight)
			CreateLight();
		VectorCopy(ent->origin, pLight->origin);
		if (iNowSelectedId < 0)
			return true;
		buymenuitem_t* info = GetInfo(iNowSelectedId);
		switch (info->sequence) {
		case NONE:
		case INVALID: break;
		case ITEM: {
			model_t* pModel = gEngfuncs.hudGetModelByIndex(info->modelindex);
			if (!pModel)
				return true;
			if (!pShowEnt) {
				pShowEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(ent->origin, pModel);
				pShowEnt->flags = FTENT_NONE;
				pShowEnt->entity.curstate.effects = EF_FULLBRIGHT;
				pShowEnt->die = gEngfuncs.GetClientTime() + 99999.0f;
			}
			VectorCopy(ent->origin, pShowEnt->entity.origin);
			VectorCopy(ent->angles, pShowEnt->entity.angles);
			pShowEnt->entity.curstate.scale = 2;
			break;
		}
		default: {
			ent->curstate.rendermode = kRenderTransAdd;
			ent->curstate.renderamt = 0;
			ent->curstate.sequence = info->sequence;
			ent->curstate.gaitsequence = 0;
			if (!pShowEnt) {
				pShowEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(ent->origin, ent->model);
				pShowEnt->flags = FTENT_NONE;
				pShowEnt->entity.curstate.effects = EF_FULLBRIGHT;
				pShowEnt->entity.curstate.framerate = ent->curstate.framerate;
				pShowEnt->entity.curstate.sequence = info->sequence;
				pShowEnt->entity.curstate.renderfx = kRenderFxDeadPlayer;
				pShowEnt->entity.curstate.renderamt = ent->index;
				pShowEnt->entity.curstate.weaponmodel = info->modelindex;
				pShowEnt->entity.curstate.weaponanim = ent->curstate.weaponanim;
				pShowEnt->entity.curstate.framerate = ent->curstate.framerate;
				pShowEnt->entity.curstate.frame = ent->curstate.frame;
				pShowEnt->entity.curstate.colormap = ent->curstate.colormap;
				pShowEnt->die = gEngfuncs.GetClientTime() + 99999.0f;
			}
			if (!pWeaponEnt) {
				model_t* pModel = gEngfuncs.hudGetModelByIndex(info->modelindex);
				if (!pModel)
					return true;
				pWeaponEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(ent->origin, pModel);
				pWeaponEnt->entity.curstate.movetype = MOVETYPE_FOLLOW;
				pWeaponEnt->entity.curstate.aiment = ent->index;
			}
			VectorCopy(ent->origin, pShowEnt->entity.origin);
			VectorCopy(ent->angles, pShowEnt->entity.angles);
			return true;
		}
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
	iNextPageBase = 0;
	if (!bOpenningMenu) {
		flOldCamYaw = pCVarIdealYaw->value;
		flOldCamDist = pCVarIdealDist->value;
		flOldCamHeight = gCVars.pCamIdealHeight->value;
		flOldCamRight = gCVars.pCamIdealRight->value;
		flOldFollowAim = pCVarFollowAim->value;
		bOldCamThirdperson = gExportfuncs.CL_IsThirdPerson();
		CreateLight();
	}
	bOpenningMenu = true;
	gHudDelegate->m_iVisibleMouse = true;
}
void CHudEccoBuyMenu::CloseMenu() {
	if (!bOpenningMenu)
		return;
	ClearTempEnt();
	pCVarIdealYaw->value = flOldCamYaw;
	pCVarIdealDist->value = flOldCamDist;
	gCVars.pCamIdealHeight->value = flOldCamHeight;
	gCVars.pCamIdealRight->value = flOldCamRight;
	pCVarFollowAim->value = flOldFollowAim;
	if (!bOldCamThirdperson)
		EngineClientCmd("firstperson");
	iNextPageBase = 0;
	bOpenningMenu = false;
	gHudDelegate->m_iVisibleMouse = false;
}
bool CHudEccoBuyMenu::SelectMenu() {
	if (!bOpenningMenu)
		return false;
	ChangePage();
	char buf[8];
	snprintf(buf, sizeof(buf), "slot%d", iNowChosenSlot);
	EngineClientCmd(buf);
	return true;
}
void CHudEccoBuyMenu::SlotCallBack(int slot){
	if (!bOpenningMenu)
		return;
	iNowSelectedId = GetMenuId(slot);
	ChangePage();
}
void CHudEccoBuyMenu::CreateLight(){
	if (pLight)
		pLight->die = 0;
	pLight = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
	pLight->color = { 255, 255 ,255 };
	pLight->decay = 0;
	pLight->die = gEngfuncs.GetClientTime() + 9999.0f;
	pLight->radius = 48;
}
void CHudEccoBuyMenu::ClearTempEnt(){
	if (pShowEnt)
		pShowEnt->die = 0;
	if (pWeaponEnt)
		pWeaponEnt->die = 0;
	if (pLight)
		pLight->die = 0;
	pLight = nullptr;
	pShowEnt = nullptr;
	pWeaponEnt = nullptr;
}
void CHudEccoBuyMenu::ChangePage(){
	if (iNowSelectedId < 0) {
		switch (iNowSelectedId) {
		case MENU_NEXTPAGE: iNextPageBase = min(iNextPageBase + 1, ceil((float)MenuList.size() / 9.0f) - 1); break;
		case MENU_LASTPAGE: iNextPageBase = max(iNextPageBase - 1, 0); break;
		}
	}
}
