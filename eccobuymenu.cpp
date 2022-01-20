#include <metahook.h>

#include "parsemsg.h"
#include "glew.h"
#include "gl_utility.h"
#include "vguilocal.h"
#include "mathlib.h"

#include "gl_def.h"
#include "gl_shader.h"
#include "local.h"
#include "gl_draw.h"

#include "eccobuymenu.h"

using namespace mathlib;

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

void CHudEccoBuyMenu::GLInit(){
	glGenFramebuffersEXT(1, &m_hGaussianBufferVFBO);
	m_hGaussianBufferVTex = GL_GenTextureRGB8(ScreenWidth, ScreenHeight);
	glGenFramebuffersEXT(1, &m_hGaussianBufferHFBO);
	m_hGaussianBufferHTex = GL_GenTextureRGB8(ScreenWidth, ScreenHeight);
	glGenFramebuffersEXT(1, &m_hGaussianBufferModelFBO);
	m_hGaussianBufferModelTex = GL_GenTextureRGB8(ScreenWidth, ScreenHeight);
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

	hFont = pScheme->GetFont("HUDSmallShitFont", true);

	Reset();
	return 0;
}

int CHudEccoBuyMenu::Draw(float flTime){
	if (!bOpenningMenu)
		return 1;
	float flAnimationRatio = 1.0f;
	if (m_fAnimateTime > flTime)
		flAnimationRatio = (1 - ((m_fAnimateTime - flTime) / BuyMenuAnimateTime)) / 100;
	//背景模糊
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_hOldBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hGaussianBufferHFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hGaussianBufferHTex, 0);
	GL_BlitFrameBufferToFrameBufferColorOnly(m_hOldBuffer, m_hGaussianBufferHFBO, ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight);
	DrawGaussianBlur(m_hGaussianBufferVFBO, m_hGaussianBufferHTex, m_hGaussianBufferVTex, flAnimationRatio, true, ScreenWidth, ScreenHeight);
	DrawGaussianBlur(m_hGaussianBufferHFBO, m_hGaussianBufferVTex, m_hGaussianBufferHTex, flAnimationRatio, false, ScreenWidth, ScreenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hOldBuffer);
	glBindTexture(GL_TEXTURE_2D, m_hGaussianBufferHTex);
	DrawQuad(ScreenWidth, ScreenHeight);
	//变黑
	gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight, 0, 0, 0, 128 * flAnimationRatio);

	int i;
	int r = 255, g = 255, b = 255, a = 255;
	float ac, as;
	int iOffset = BuyMenuOffset;
	int iBackGroundHeight = BuyMenuHeight;
	int iXDiffer = ScreenWidth / 2 - BuyMenuCenterX;
	int iYDiffer = ScreenHeight / 2 - BuyMenuCenterY;
	vec2_t aryOut[9];
	vec2_t aryIn[9];
	vec2_t vecA, vecB, vecC, vecD;
	float xpos, ypos;
	//绘制九边形
	for (i = 0; i < 9; i++) {
		ac = cos(2 * M_PI * i / 9);
		as = sin(2 * M_PI * i / 9);

		aryIn[i][0] = iOffset * ac - iXDiffer;
		aryIn[i][1] = iOffset * as - iYDiffer;

		aryOut[i][0] = (iOffset + iBackGroundHeight) * ac - iXDiffer;
		aryOut[i][1] = (iOffset + iBackGroundHeight) * as - iYDiffer;
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
		//CABD
		DrawSPRIconPos(iBackgroundSpr, kRenderTransAdd, vecC, vecA, vecB, vecD, r, g, b, a * 0.5);
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

	//绘制模型

	return 0;
}

void CHudEccoBuyMenu::Reset() {
	buymenuinfo.clear();
	iBackgroundSpr = SPR_Load("abcenchance/spr/buymenu_background.spr");
}

void CHudEccoBuyMenu::Clear() {
	buymenuinfo.clear();
	if (m_hGaussianBufferVTex)
		glDeleteTextures(1, &m_hGaussianBufferVTex);
	if (m_hGaussianBufferHTex)
		glDeleteTextures(1, &m_hGaussianBufferHTex);
}

void CHudEccoBuyMenu::AddInfo(buymenuitem_t item){
	buymenuinfo.push_back(item);
}

void CHudEccoBuyMenu::OpenMenu(){
	m_fAnimateTime = gEngfuncs.GetClientTime() + BuyMenuAnimateTime;
	bOpenningMenu = true;
}
