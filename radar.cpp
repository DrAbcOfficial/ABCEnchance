#include <metahook.h>
#include "local.h"
#include <cmath>
#include "mathlib.h"
#include "pm_defs.h"
#include "event_api.h"

#include "glew.h"
#include "glutility.h"
#include "gldef.h"
#include "gl_shader.h"

#include "exportfuncs.h"

#include "hud.h"
#include "vguilocal.h"
#include "myconst.h"
#include "utility.h"

#include "weapon.h"
#include "CHudDelegate.h"
#include "radar.h"

CHudRadar m_HudRadar;
SHADER_DEFINE(pp_radarlight);

void __UserCmd_StartSizeRadar(void){
	if (m_HudRadar.flFinishScaleTime <= 0)
		m_HudRadar.flFinishScaleTime = gEngfuncs.GetClientTime() + gCVars.pRadarSizeTime->value;
}
void __UserCmd_EndSizeRadar(void){
	if (m_HudRadar.flFinishScaleTime > 0)
		m_HudRadar.flFinishScaleTime = 0;
}

void CHudRadar::GLInit(){
	if(!g_metaplugins.renderer)
		glGenFramebuffersEXT(1, &m_hRadarBufferFBO);
	m_hRadarBufferTex = GL_GenTextureRGBA8(gScreenInfo.iWidth, gScreenInfo.iHeight);
	m_hRadarBufferTexDepth = GL_GenDepthStencilTexture(gScreenInfo.iWidth, gScreenInfo.iHeight);
}
int CHudRadar::Init(){
	gEngfuncs.pfnAddCommand("+scaleradar", __UserCmd_StartSizeRadar);
	gEngfuncs.pfnAddCommand("-scaleradar", __UserCmd_EndSizeRadar);

	gCVars.pRadar = CREATE_CVAR("cl_radar", "1", FCVAR_VALUE, NULL);
	gCVars.pRadarZoom = CREATE_CVAR("cl_radarzoom", "2.5", FCVAR_VALUE, NULL);
	gCVars.pRadarSize = CREATE_CVAR("cl_radarsize", "344", FCVAR_VALUE, NULL);
	gCVars.pRadarSizeTime = CREATE_CVAR("cl_radarsizetime", "0.25", FCVAR_VALUE, NULL);
	gCVars.pRadarGap = CREATE_CVAR("cl_radargap", "0.98", FCVAR_VALUE, NULL);
	gCVars.pRadarRoundRadius = CREATE_CVAR("cl_radarradius", "344", FCVAR_VALUE, NULL);

	gCVars.pRadarUpdateInterval = CREATE_CVAR("cl_radarupdateint", "1", FCVAR_VALUE, NULL);
	pCVarDevOverview = gEngfuncs.pfnGetCvarPointer("dev_overview");
	pCVarDrawDynamic = gEngfuncs.pfnGetCvarPointer("r_dynamic");
	pCVarDrawEntities = gEngfuncs.pfnGetCvarPointer("r_drawentities");
	pCVarGamma = gEngfuncs.pfnGetCvarPointer("gamma");
	
	XOffset = GET_SCREEN_PIXEL(false, "Radar.XOffset");
	YOffset = GET_SCREEN_PIXEL(true, "Radar.YOffset");
	NorthPointerSize = GET_SCREEN_PIXEL(true, "Radar.NorthPointerSize");
	ViewAngleSize = GET_SCREEN_PIXEL(true, "Radar.ViewAngleSize");
	OutLineAlpha = atof(pScheme->GetResourceString("Radar.OutLineAlpha"));
	MapAlpha = (GLubyte)atof(pScheme->GetResourceString("Radar.MapAlpha"));
	CenterAlpha = atof(pScheme->GetResourceString("Radar.CenterAlpha"));
	

	pp_radarlight.program = R_CompileShaderFile("abcenchance\\shader\\pp_brightpass.vsh", "abcenchance\\shader\\radar.fsh", NULL);
	if (pp_radarlight.program){
		SHADER_UNIFORM(pp_radarlight, rad, "rad");
		SHADER_UNIFORM(pp_radarlight, xys, "xys");
		SHADER_UNIFORM(pp_radarlight, gamma, "gamma");
	}
	return 1;
}
void CHudRadar::VidInit(){
	if(!pCVarFXAA)
		pCVarFXAA = gEngfuncs.pfnGetCvarPointer("r_fxaa");
	if(!pCVarWater)
		pCVarWater = gEngfuncs.pfnGetCvarPointer("r_water");
}
void CHudRadar::Reset(){
	VGUI_CREATE_NEWTGA_TEXTURE(BackGroundImg, "abcenchance/tga/radar_background");
	VGUI_CREATE_NEWTGA_TEXTURE(UpGroundImg, "abcenchance/tga/radar_upground");
	VGUI_CREATE_NEWTGA_TEXTURE(NorthImg, "abcenchance/tga/radar_north");
	VGUI_CREATE_NEWTGA_TEXTURE(ViewAngleImg, "abcenchance/tga/radar_viewangle");
	VGUI_CREATE_NEWTGA_TEXTURE(RoundBackGroundImg, "abcenchance/tga/radar_roundbackground");

	flNextUpdateTrTime = 0;
	flFinishScaleTime = 0;
	cvar_t* pCvarDevC = gEngfuncs.pfnGetCvarPointer("dev_overview_color");
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
	gHudDelegate->surface()->DrawSetTexture(-1);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
	gHudDelegate->surface()->DrawSetTexture(gCVars.pRadar->value > 1 ? RoundBackGroundImg : BackGroundImg);
	gHudDelegate->surface()->DrawTexturedRect(iStartX - sizeGap, iStartY - sizeGap, iStartX + size, iStartX + size);
	//shader
	GL_UseProgram(pp_radarlight.program);
	if (gCVars.pRadar->value > 1) {
		glUniform1f(pp_radarlight.rad, min(1.0f, fabs(gCVars.pRadarRoundRadius->value) / sizeMap));
		glUniform3f(pp_radarlight.xys, iStartX, iStartY, sizeMap);
	}
	else
		glUniform1f(pp_radarlight.rad, 0);
	if (g_metaplugins.renderer)
		glUniform1f(pp_radarlight.gamma, 1 / pCVarGamma->value);

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
	GL_UseProgram(0);
	//绘制前景
	if (gCVars.pRadar->value == 1) {
		gHudDelegate->surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
		gHudDelegate->surface()->DrawSetTexture(UpGroundImg);
		gHudDelegate->surface()->DrawTexturedRect(iStartX - sizeGap, iStartY - sizeGap, iStartX + size, iStartX + size);
	}
	//绘制箭头
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255);
	gHudDelegate->surface()->DrawSetTexture(ViewAngleImg);
	gHudDelegate->surface()->DrawTexturedRect(
		iStartX + (size - ViewAngleSize) / 2,
		iStartY + (size - ViewAngleSize) / 2,
		iStartX + (size + ViewAngleSize) / 2,
		iStartY + (size + ViewAngleSize) / 2);
	//绘制指北针
	float rotate = DEG2RAD(gEngfuncs.GetLocalPlayer()->curstate.angles[YAW]);
	h = gCVars.pRadar->value > 1 ? size / 2 : fsqrt(2 * pow(size, 2)) / 2;
	stx = clamp(((iStartX + size / 2) + h * cos(rotate)), iStartX, iStartX + size);
	sty = clamp(((iStartY + size / 2) + h * sin(rotate)), iStartY, iStartY + size);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, OutLineAlpha);
	gHudDelegate->surface()->DrawSetTexture(NorthImg);
	w = NorthPointerSize / 2;
	gHudDelegate->surface()->DrawTexturedRect(stx - w, sty - w, stx + w, sty + w);
}
void CHudRadar::PreRenderView(int a1){
	if (gCVars.pRadar->value <= 0)
		return;
	if(!a1)
		DrawRadarTexture();
}
void CHudRadar::DrawRadarTexture(){
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	if (!m_oldFrameBuffer && !g_metaplugins.renderer)
		glBindFramebuffer(GL_FRAMEBUFFER, m_hRadarBufferFBO);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hRadarBufferTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_hRadarBufferTexDepth, 0);
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
	gHudDelegate->m_flOverViewYaw = local->curstate.angles[YAW];

	gHudDelegate->m_iIsOverView = 1;
		vec5_t vecOldValue;
		vecOldValue[0] = pCVarDevOverview->value;
		vecOldValue[1] = pCVarDrawEntities->value;
		vecOldValue[2] = pCVarDrawDynamic->value;
		if (g_metaplugins.renderer){
			vecOldValue[3] = pCVarFXAA->value;
			vecOldValue[4] = pCVarWater->value;
		}
		pCVarDevOverview->value = 2;
		pCVarDrawEntities->value = 0;
		pCVarDrawDynamic->value = 0;
		GLint riFogMode;
		vec3_t vecFogControl;
		VectorClear(vecFogControl);
		vec4_t vecFogColor;
		bool bIsFog;
		if (g_metaplugins.renderer) {
			pCVarFXAA->value = 0;
			pCVarWater->value = 0;
			bIsFog = glIsEnabled(GL_FOG);
			if (bIsFog) {
				glGetIntegerv(GL_FOG_MODE, &riFogMode);
				if (riFogMode == GL_LINEAR) {
					glGetFloatv(GL_FOG_START, &vecFogControl[0]);
					glGetFloatv(GL_FOG_END, &vecFogControl[1]);
					glGetFloatv(GL_FOG_COLOR, vecFogColor);
				}
				else if (riFogMode == GL_EXP2) {
					glGetFloatv(GL_FOG_START, &vecFogControl[0]);
					glGetFloatv(GL_FOG_END, &vecFogControl[1]);
					glGetFloatv(GL_FOG_DENSITY, &vecFogControl[2]);
					glGetFloatv(GL_FOG_COLOR, vecFogColor);
				}
			}
		}
		gHookFuncs.R_RenderScene();
		pCVarDevOverview->value = vecOldValue[0];
		pCVarDrawEntities->value = vecOldValue[1];
		pCVarDrawDynamic->value = vecOldValue[2];
		if (g_metaplugins.renderer) {
			if (bIsFog) {
				glEnable(GL_FOG);
				glFogi(GL_FOG_MODE, riFogMode);
				if (riFogMode == GL_EXP2)
					glFogf(GL_FOG_DENSITY, vecFogControl[2]);
				glHint(GL_FOG_HINT, GL_NICEST);
				glFogfv(GL_FOG_COLOR, vecFogColor);
				glFogf(GL_FOG_START, vecFogControl[0]);
				glFogf(GL_FOG_END, vecFogControl[1]);
			}
			pCVarFXAA->value = vecOldValue[3];
			pCVarWater->value = vecOldValue[4];
		}
	gHudDelegate->m_iIsOverView = 0;

	VectorCopy(m_oldViewOrg, g_refdef->vieworg);
	VectorCopy(m_oldViewAng, g_refdef->viewangles);

	if (!m_oldFrameBuffer && !g_metaplugins.renderer)
		glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}
void CHudRadar::UpdateZmax(float flTime){
	if (flTime < flNextUpdateTrTime)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	vec3_t vecEndpos;
	VectorCopy(local->curstate.origin, vecEndpos);
	vecEndpos[2] = 9999;
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(local->curstate.origin, vecEndpos, -1, PM_WORLD_ONLY, &m_hRadarTr);
	//16，去除大多数烦人的天花板灯泡
	gHudDelegate->m_flOverViewZmax = m_hRadarTr.endpos[2] - 16;
	vecEndpos[2] = -9999;
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(local->curstate.origin, vecEndpos, -1, PM_WORLD_ONLY, &m_hRadarTr);
	float flOldStart = m_hRadarTr.endpos[2] - 1;
	//半双人高，可以满足大多数地图的需求
	m_hRadarTr.endpos[2] -= 144;
	gEngfuncs.pEventAPI->EV_PlayerTrace(m_hRadarTr.endpos, vecEndpos, -1, PM_WORLD_ONLY, &m_hRadarTr);
	gHudDelegate->m_flOverViewZmin = m_hRadarTr.startsolid ? m_hRadarTr.endpos[2] : flOldStart;
	flNextUpdateTrTime = flTime + abs(gCVars.pRadarUpdateInterval->value);
}
void CHudRadar::Clear(){
	if (m_hRadarBufferTex)
		glDeleteTextures(1, &m_hRadarBufferTex);
	if (m_hRadarBufferTexDepth)
		glDeleteTextures(1, &m_hRadarBufferTexDepth);
}