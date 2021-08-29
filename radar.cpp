#include <metahook.h>
#include "local.h"
#include <math.h>
#include "mathlib.h"
#include "gldef.h"
#include "glew.h"
#include "exportfuncs.h"
#include "vguilocal.h"

#include "hud.h"
#include "weapon.h"
#include "CHudDelegate.h"
#include "radar.h"

CHudRadar m_HudRadar;

void CHudRadar::GLInit()
{
	glGenFramebuffersEXT(1, &m_hRadarBufferFBO);
	glGenTextures(1, &m_hRadarBufferTex);
	glBindTexture(GL_TEXTURE_2D, m_hRadarBufferTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gScreenInfo.iWidth, gScreenInfo.iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
int CHudRadar::Init()
{
	gCVars.pRadar = gEngfuncs.pfnRegisterVariable("cl_radar", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pRadarZoom = gEngfuncs.pfnRegisterVariable("cl_radarzoom", "5", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	gCVars.pRadarSize = gEngfuncs.pfnRegisterVariable("cl_radarsize", "344", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	pCVarDevOverview = gEngfuncs.pfnGetCvarPointer("dev_overview");
	pCVarDrawEntities = gEngfuncs.pfnGetCvarPointer("r_drawentities");

	XOffset = atof(pScheme->GetResourceString("Radar.XOffset"));
	YOffset = atof(pScheme->GetResourceString("Radar.YOffset"));
	OutLineAlpha = atof(pScheme->GetResourceString("Radar.OutLineAlpha"));
	MapAlpha = (GLubyte)atof(pScheme->GetResourceString("Radar.MapAlpha"));
	CenterAlpha = atof(pScheme->GetResourceString("Radar.CenterAlpha"));
	NorthPointerSize = atof(pScheme->GetResourceString("Radar.NorthPointerSize"));

	return 1;
}
void CHudRadar::Reset()
{
	OutLineImg = gHudDelegate->surface()->CreateNewTextureID();
	gHudDelegate->surface()->DrawSetTextureFile(OutLineImg, "abcenchance/tga/radar_background", true, false);

	PlayerPointImg = gHudDelegate->surface()->CreateNewTextureID();
	gHudDelegate->surface()->DrawSetTextureFile(PlayerPointImg, "abcenchance/tga/radar_upground", true, false);

	NorthImg = gHudDelegate->surface()->CreateNewTextureID();
	gHudDelegate->surface()->DrawSetTextureFile(NorthImg, "abcenchance/tga/radar_north", true, false);
}
void CHudRadar::Draw(float flTime)
{
	if (gCVars.pRadar->value <= 0)
		return;
	float size = gCVars.pRadarSize->value;
	float sizeMap = size * 0.95;
	float sizeGap = (size - sizeMap) / 2;
	int iStartX = XOffset;
	int iStartY = YOffset;
	//计算纹理坐标
	float h, w, stx, sty;
	h = size / gScreenInfo.iHeight;
	w = size / gScreenInfo.iWidth;
	stx = (1 - w) / 2;
	sty = (1 - h) / 2;
	//绘制背景
	gHudDelegate->surface()->DrawSetTexture(-1);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
	gHudDelegate->surface()->DrawSetTexture(OutLineImg);
	gHudDelegate->surface()->DrawTexturedRect(iStartX - sizeGap, iStartY - sizeGap, iStartX + size, iStartX + size);
	//绘制雷达
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_hRadarBufferTex);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(255, 255, 255, MapAlpha);
	glBegin(GL_QUADS);
		glTexCoord2f(stx, sty + h);
		glVertex2f(iStartX, iStartY);
		glTexCoord2f(stx + w, sty + h);
		glVertex2f(iStartX + sizeMap, iStartY);
		glTexCoord2f(stx + w, sty);
		glVertex2f(iStartX + sizeMap, iStartY + sizeMap);
		glTexCoord2f(stx, sty);
		glVertex2f(iStartX, iStartY + sizeMap);
	glEnd();
	//绘制前景
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
	gHudDelegate->surface()->DrawSetTexture(PlayerPointImg);
	gHudDelegate->surface()->DrawTexturedRect(iStartX - sizeGap, iStartY - sizeGap, iStartX + size, iStartX + size);
	//绘制指北针
	//圆
	float rotate = DEG2RAD(gEngfuncs.GetLocalPlayer()->curstate.angles[YAW]);
	h = sqrt(2 * pow(size, 2)) / 2;
	stx = (iStartX + size / 2) + h * cos(rotate);
	sty = (iStartY + size / 2) + h * sin(rotate);
	stx = clamp(stx, iStartX, iStartX + size);
	sty = clamp(sty, iStartY, iStartY + size);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
	gHudDelegate->surface()->DrawSetTexture(NorthImg);
	w = NorthPointerSize / 2;
	gHudDelegate->surface()->DrawTexturedRect(stx - w, sty - w, stx + w, sty + w);
}
void CHudRadar::PreRenderView(int a1)
{
	if (gCVars.pRadar->value <= 0)
		return;
	if(!a1)
		DrawRadarTexture();
}
void CHudRadar::DrawRadarTexture()
{
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hRadarBufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hRadarBufferTex, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	//设置背景透明
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glStencilMask(0xFF);
	glClearStencil(0);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0);

	VectorCopy(g_refdef->vieworg, m_oldViewOrg);
	VectorCopy(g_refdef->viewangles, m_oldViewAng);

	//设置到玩家脑袋上朝下看
	gHudDelegate->m_flOverViewScale = gCVars.pRadarZoom->value;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	gHudDelegate->m_vecOverViewOrg[0] = local->curstate.origin[0];
	gHudDelegate->m_vecOverViewOrg[1] = local->curstate.origin[1];
	gHudDelegate->m_vecOverViewOrg[2] = gDevOverview->origin[2];
	gHudDelegate->m_flOverViewYaw = local->curstate.angles[YAW];
	gHudDelegate->m_iIsOverView = 1;
	float oldValue = pCVarDevOverview->value;
	float oldEntity = pCVarDrawEntities->value;
	pCVarDevOverview->value = 2;
	pCVarDrawEntities->value = 0;
	gHookFuncs.R_RenderScene();
	pCVarDevOverview->value = oldValue;
	pCVarDrawEntities->value = oldEntity;
	gHudDelegate->m_iIsOverView = 0;

	VectorCopy(m_oldViewOrg, g_refdef->vieworg);
	VectorCopy(m_oldViewAng, g_refdef->viewangles);

	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}
void CHudRadar::Clear()
{
	if (m_hRadarBufferTex)
		glDeleteTextures(1, &m_hRadarBufferTex);
}