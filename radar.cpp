#include <metahook.h>
#include "local.h"
#include <cmath>
#include "mathlib.h"
#include "pm_defs.h"
#include "event_api.h"

#include "glew.h"
#include "glutility.h"
#include "gldef.h"

#include "exportfuncs.h"

#include "gl_shader.h"

#include "hud.h"
#include "vguilocal.h"
#include "weapon.h"
#include "CHudDelegate.h"
#include "radar.h"

CHudRadar m_HudRadar;
SHADER_DEFINE(pp_radarlight);

void CHudRadar::GLInit()
{
	if(!g_metaplugins.renderer)
		glGenFramebuffersEXT(1, &m_hRadarBufferFBO);
	m_hRadarBufferTex = GL_GenTextureRGBA8(gScreenInfo.iWidth, gScreenInfo.iHeight);
	m_hRadarBufferTexDepth = GL_GenDepthStencilTexture(gScreenInfo.iWidth, gScreenInfo.iHeight);
}
int CHudRadar::Init()
{
	gCVars.pRadar = CREATE_CVAR("cl_radar", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, NULL);
	gCVars.pRadarZoom = CREATE_CVAR("cl_radarzoom", "2.5", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, NULL);
	gCVars.pRadarSize = CREATE_CVAR("cl_radarsize", "344", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, NULL);
	gCVars.pRadarGap = CREATE_CVAR("cl_radargap", "0.98", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, NULL);
	gCVars.pRadarUpdateInterval = CREATE_CVAR("cl_radarupdateint", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, NULL);
	pCVarDevOverview = gEngfuncs.pfnGetCvarPointer("dev_overview");
	pCVarDrawDynamic = gEngfuncs.pfnGetCvarPointer("r_dynamic");
	pCVarDrawEntities = gEngfuncs.pfnGetCvarPointer("r_drawentities");

	XOffset = atof(pScheme->GetResourceString("Radar.XOffset"));
	YOffset = atof(pScheme->GetResourceString("Radar.YOffset"));
	OutLineAlpha = atof(pScheme->GetResourceString("Radar.OutLineAlpha"));
	MapAlpha = (GLubyte)atof(pScheme->GetResourceString("Radar.MapAlpha"));
	CenterAlpha = atof(pScheme->GetResourceString("Radar.CenterAlpha"));
	NorthPointerSize = atof(pScheme->GetResourceString("Radar.NorthPointerSize"));

	pp_radarlight.program = R_CompileShaderFile("abcenchance\\shader\\pp_brightpass.vsh", "abcenchance\\shader\\pp_brightpass.fsh", NULL);
	if (pp_radarlight.program)
	{
		SHADER_UNIFORM(pp_radarlight, tex, "tex");
		SHADER_UNIFORM(pp_radarlight, ovc, "ovc");
	}
	return 1;
}
void CHudRadar::VidInit()
{
	if(!pCVarFXAA)
		pCVarFXAA = gEngfuncs.pfnGetCvarPointer("r_fxaa");
}
void CHudRadar::Reset()
{
	VGUI_CREATE_NEWTGA_TEXTURE(OutLineImg, "abcenchance/tga/radar_background");
	VGUI_CREATE_NEWTGA_TEXTURE(PlayerPointImg, "abcenchance/tga/radar_upground");
	VGUI_CREATE_NEWTGA_TEXTURE(NorthImg, "abcenchance/tga/radar_north");

	flNextUpdateTrTime = 0;
	cvar_t* pCvarDevC = gEngfuncs.pfnGetCvarPointer("dev_overview_color");
	if (pCvarDevC && g_metaplugins.renderer)
	{
		sscanf_s(pCvarDevC->string, "%d %d %d", &iOverviewR, &iOverviewG, &iOverviewB);
		iOverviewR /= 255;
		iOverviewG /= 255;
		iOverviewB /= 255;
	}
}
void CHudRadar::Draw(float flTime)
{
	if (gCVars.pRadar->value <= 0)
		return;
	UpdateZmax(flTime);
	float size = gCVars.pRadarSize->value;
	float sizeMap = size * gCVars.pRadarGap->value;
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

	if (g_metaplugins.renderer) {
		GL_UseProgram(pp_radarlight.program);
		glUniform1i(pp_radarlight.tex, 0);
		glUniform3f(pp_radarlight.ovc, iOverviewR, iOverviewG, iOverviewB);
	}
	
	glBindTexture(GL_TEXTURE_2D, m_hRadarBufferTex);
	if (!g_metaplugins.renderer) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
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

	if (g_metaplugins.renderer)
		GL_UseProgram(0);

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
	if (!m_oldFrameBuffer)
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
		vec4_t vecOldValue;
		vecOldValue[0] = pCVarDevOverview->value;
		vecOldValue[1] = pCVarDrawEntities->value;
		vecOldValue[2] = pCVarDrawDynamic->value;
		if (g_metaplugins.renderer)
			vecOldValue[3] = pCVarFXAA->value;
		pCVarDevOverview->value = 2;
		pCVarDrawEntities->value = 0;
		pCVarDrawDynamic->value = 0;
		if (g_metaplugins.renderer)
			pCVarFXAA->value = 0;

		gHookFuncs.R_RenderScene();

		pCVarDevOverview->value = vecOldValue[0];
		pCVarDrawEntities->value = vecOldValue[1];
		pCVarDrawDynamic->value = vecOldValue[2];
		if (g_metaplugins.renderer)
			pCVarFXAA->value = vecOldValue[3];
	gHudDelegate->m_iIsOverView = 0;

	VectorCopy(m_oldViewOrg, g_refdef->vieworg);
	VectorCopy(m_oldViewAng, g_refdef->viewangles);

	if (!m_oldFrameBuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}
void CHudRadar::UpdateZmax(float flTime)
{
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
void CHudRadar::Clear()
{
	if (m_hRadarBufferTex)
		glDeleteTextures(1, &m_hRadarBufferTex);
}