#include <metahook.h>

#include <cmath>
#include "mathlib.h"
#include "Vector.h"

#include "local.h"
#include "pm_defs.h"
#include "event_api.h"

#include "exportfuncs.h"

#include "hud.h"
#include "vguilocal.h"

#include "glew.h"
#include "gl_utility.h"
#include "gl_def.h"
#include "gl_shader.h"
#include "gl_draw.h"

#include "weapon.h"
#include "CCustomHud.h"
#include "radar.h"

using namespace mathlib;

CHudRadar m_HudRadar;
void __UserCmd_StartSizeRadar(void){
	if (m_HudRadar.flFinishScaleTime <= 0)
		m_HudRadar.flFinishScaleTime = gEngfuncs.GetClientTime() + gCVars.pRadarSizeTime->value;
}
void __UserCmd_EndSizeRadar(void){
	if (m_HudRadar.flFinishScaleTime > 0)
		m_HudRadar.flFinishScaleTime = 0;
}

void CHudRadar::GLInit()
{
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);

	glGenFramebuffers(1, &m_hRadarBufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hRadarBufferFBO);
	m_hRadarBufferTex = GL_GenTextureRGBA8(gScreenInfo.iWidth, gScreenInfo.iHeight);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hRadarBufferTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}

int CHudRadar::Init(){
	ADD_COMMAND("+scaleradar", __UserCmd_StartSizeRadar);
	ADD_COMMAND("-scaleradar", __UserCmd_EndSizeRadar);

	gCVars.pCVarDevOverview = CVAR_GET_POINTER("dev_overview");
	gCVars.pCVarDrawEntities = CVAR_GET_POINTER("r_drawentities");
	gCVars.pCVarDrawViewModel = CVAR_GET_POINTER("r_drawviewmodel");
	gCVars.pCVarDrawDynamic = CVAR_GET_POINTER("r_dynamic");
	gCVars.pCVarFXAA = CVAR_GET_POINTER("r_fxaa");
	gCVars.pCVarWater = CVAR_GET_POINTER("r_water");
	gCVars.pCVarShadow = CVAR_GET_POINTER("r_shadow");

	gCVars.pRadar = CREATE_CVAR("cl_radar", "2", FCVAR_VALUE, NULL);
	gCVars.pRadarZoom = CREATE_CVAR("cl_radarzoom", "2.5", FCVAR_VALUE, NULL);
	gCVars.pRadarSize = CREATE_CVAR("cl_radarsize", "344", FCVAR_VALUE, NULL);
	gCVars.pRadarSizeTime = CREATE_CVAR("cl_radarsizetime", "0.25", FCVAR_VALUE, NULL);
	gCVars.pRadarGap = CREATE_CVAR("cl_radargap", "0.98", FCVAR_VALUE, NULL);
	gCVars.pRadarRoundRadius = CREATE_CVAR("cl_radarradius", "344", FCVAR_VALUE, NULL);
	gCVars.pRadarUpdateInterval = CREATE_CVAR("cl_radarupdateint", "1", FCVAR_VALUE, NULL);

	XOffset = GET_SCREEN_PIXEL(false, "Radar.XOffset");
	YOffset = GET_SCREEN_PIXEL(true, "Radar.YOffset");
	NorthPointerSize = GET_SCREEN_PIXEL(true, "Radar.NorthPointerSize");
	ViewAngleSize = GET_SCREEN_PIXEL(true, "Radar.ViewAngleSize");
	OutLineAlpha = atof(pScheme->GetResourceString("Radar.OutLineAlpha"));
	MapAlpha = (GLubyte)atof(pScheme->GetResourceString("Radar.MapAlpha"));
	CenterAlpha = atof(pScheme->GetResourceString("Radar.CenterAlpha"));
	
	return 1;
}
void CHudRadar::VidInit(){
	if(!gCVars.pCVarFXAA)
		gCVars.pCVarFXAA = CVAR_GET_POINTER("r_fxaa");
	if(!gCVars.pCVarWater)
		gCVars.pCVarWater = CVAR_GET_POINTER("r_water");
	if (!gCVars.pCVarShadow)
		gCVars.pCVarShadow = CVAR_GET_POINTER("r_shadow");
}
void CHudRadar::Reset(){
	VGUI_CREATE_NEWTGA_TEXTURE(BackGroundImg, "abcenchance/tga/radar_background");
	VGUI_CREATE_NEWTGA_TEXTURE(UpGroundImg, "abcenchance/tga/radar_upground");
	VGUI_CREATE_NEWTGA_TEXTURE(NorthImg, "abcenchance/tga/radar_north");
	VGUI_CREATE_NEWTGA_TEXTURE(ViewAngleImg, "abcenchance/tga/radar_viewangle");
	VGUI_CREATE_NEWTGA_TEXTURE(RoundBackGroundImg, "abcenchance/tga/radar_roundbackground");

	flNextUpdateTrTime = 0;
	flFinishScaleTime = 0;
	cvar_t* pCvarDevC = CVAR_GET_POINTER("dev_overview_color");
	if (pCvarDevC && g_metaplugins.renderer){
		sscanf_s(pCvarDevC->string, "%d %d %d", &iOverviewR, &iOverviewG, &iOverviewB);
		iOverviewR /= 255;
		iOverviewG /= 255;
		iOverviewB /= 255;
	}
}
void CHudRadar::Draw(float flTime){
	if (gCVars.pRadar->value <= 0)
		return;
	if (!gCustomHud.HasSuit())
		return;
	UpdateZmax(flTime);
	float size = gCVars.pRadarSize->value;
	int iStartY = YOffset;
	if (flFinishScaleTime > 0) {
		int iMaxSize = gScreenInfo.iHeight - 2 * iStartY;
		size = flFinishScaleTime > flTime ?
			(size + (iMaxSize - size) * (1 - (flFinishScaleTime - flTime) / gCVars.pRadarSizeTime->value)) :
			iMaxSize;
	}
	float sizeMap = size * gCVars.pRadarGap->value;
	float sizeGap = (size - sizeMap) / 2;
	int iStartX = XOffset;
	//计算纹理坐标
	float h, w, stx, sty;
	h = size / gScreenInfo.iHeight;
	w = size / gScreenInfo.iWidth;
	stx = (1 - w) / 2;
	sty = (1 - h) / 2;
	//绘制背景
	gCustomHud.surface()->DrawSetTexture(-1);
	gCustomHud.surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
	gCustomHud.surface()->DrawSetTexture(gCVars.pRadar->value > 1 ? RoundBackGroundImg : BackGroundImg);
	gCustomHud.surface()->DrawTexturedRect(iStartX - sizeGap, iStartY - sizeGap, iStartX + size, iStartX + size);
	//shader
	GL_UseProgram(pp_texround.program);
	if (gCVars.pRadar->value > 1) {
		GL_Uniform1f(pp_texround.rad, min(1.0f, fabs(gCVars.pRadarRoundRadius->value) / sizeMap));
		GL_Uniform3f(pp_texround.xys, iStartX, iStartY, sizeMap);
	}
	else
		GL_Uniform1f(pp_texround.rad, 0);

	glEnable(GL_TEXTURE_2D);
	glBind(m_hRadarBufferTex);
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
	glDisable(GL_BLEND);
	GL_UseProgram(0);
	//绘制前景
	gCustomHud.surface()->DrawSetTexture(-1);
	if (gCVars.pRadar->value == 1) {
		gCustomHud.surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
		gCustomHud.surface()->DrawSetTexture(UpGroundImg);
		gCustomHud.surface()->DrawTexturedRect(iStartX - sizeGap, iStartY - sizeGap, iStartX + size, iStartX + size);
	}
	//绘制箭头
	gCustomHud.surface()->DrawSetColor(255, 255, 255, 255);
	gCustomHud.surface()->DrawSetTexture(ViewAngleImg);
	gCustomHud.surface()->DrawTexturedRect(
		iStartX + (size - ViewAngleSize) / 2,
		iStartY + (size - ViewAngleSize) / 2,
		iStartX + (size + ViewAngleSize) / 2,
		iStartY + (size + ViewAngleSize) / 2);
	//绘制指北针
	float rotate = DEG2RAD(gEngfuncs.GetLocalPlayer()->curstate.angles[YAW]);
	h = gCVars.pRadar->value > 1 ? size / 2 : fsqrt(2 * pow(size, 2)) / 2;
	stx = clamp(((iStartX + size / 2) + h * cos(rotate)), (float)iStartX, (float)iStartX + size);
	sty = clamp(((iStartY + size / 2) + h * sin(rotate)), (float)iStartY, (float)iStartY + size);
	gCustomHud.surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
	gCustomHud.surface()->DrawSetTexture(NorthImg);
	w = NorthPointerSize / 2;
	gCustomHud.surface()->DrawTexturedRect(stx - w, sty - w, stx + w, sty + w);
}

void CHudRadar::BlitFramebuffer()
{
	//Blit color from current framebuffer into texture

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);

	GL_BlitFrameBufferToFrameBufferColorOnly(m_oldFrameBuffer, m_hRadarBufferFBO, gScreenInfo.iWidth, gScreenInfo.iHeight, gScreenInfo.iWidth, gScreenInfo.iHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}

void CHudRadar::UpdateZmax(float flTime){
	if (flTime < flNextUpdateTrTime)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	Vector vecEndpos = local->curstate.origin;
	vecEndpos.z = 9999;
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(local->curstate.origin, vecEndpos, -1, PM_WORLD_ONLY, &m_hRadarTr);
	//16，去除大多数烦人的天花板灯泡
	gCustomHud.m_flOverViewZmax = m_hRadarTr.endpos[2] - 16;
	vecEndpos.z = -9999;
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(local->curstate.origin, vecEndpos, -1, PM_WORLD_ONLY, &m_hRadarTr);
	float flOldStart = m_hRadarTr.endpos[2] - 1;
	//半双人高，可以满足大多数地图的需求
	m_hRadarTr.endpos[2] -= 144;
	gEngfuncs.pEventAPI->EV_PlayerTrace(m_hRadarTr.endpos, vecEndpos, -1, PM_WORLD_ONLY, &m_hRadarTr);
	gCustomHud.m_flOverViewZmin = m_hRadarTr.startsolid ? m_hRadarTr.endpos[2] : flOldStart;
	flNextUpdateTrTime = flTime + abs(gCVars.pRadarUpdateInterval->value);
}
void CHudRadar::Clear()
{
	if (m_hRadarBufferFBO)
		glDeleteFramebuffers(1, &m_hRadarBufferFBO);

	if (m_hRadarBufferTex)
		glDeleteTextures(1, &m_hRadarBufferTex);
}