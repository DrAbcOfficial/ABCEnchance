#include <metahook.h>

#include <cmath>
#include "mymathlib.h"
#include <CVector.h>

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

#include <CCustomHud.h>
#include "Viewport.h"
#include "radar.h"


void __UserCmd_StartSizeRadar(void){
	if (g_pViewPort->GetRadarPanel()->flFinishScaleTime <= 0)
		g_pViewPort->GetRadarPanel()->SetScaleTime(-1);
}
void __UserCmd_EndSizeRadar(void){
	if (g_pViewPort->GetRadarPanel()->flFinishScaleTime > 0)
		g_pViewPort->GetRadarPanel()->SetScaleTime(0);
}

extern CViewport* g_pViewPort;
#define VIEWPORT_RADAR_NAME "RadarPanel"
CRadarPanel::CRadarPanel()
	: BaseClass(nullptr, VIEWPORT_RADAR_NAME) {
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);

	glGenFramebuffers(1, &m_hRadarBufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hRadarBufferFBO);
	m_hRadarBufferTex = GL_GenTextureRGBA8(gScreenInfo.iWidth, gScreenInfo.iHeight);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hRadarBufferTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);

	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(g_pViewPort->GetBaseScheme());

	ADD_COMMAND("+scaleradar", __UserCmd_StartSizeRadar);
	ADD_COMMAND("-scaleradar", __UserCmd_EndSizeRadar);

	gCVars.pRadar = CREATE_CVAR("cl_radar", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		g_pViewPort->GetRadarPanel()->ShowPanel(cvar->value);
	});
	gCVars.pRadarZoom = CREATE_CVAR("cl_radarzoom", "2.5", FCVAR_VALUE, nullptr);

	LoadControlSettings(VGUI2_ROOT_DIR "EffectPanel.res");

	VGUI_CREATE_NEWTGA_TEXTURE(m_iBackground, m_szBackgroundImg);
	VGUI_CREATE_NEWTGA_TEXTURE(m_iUpGround, m_szUpgroundImg);
	VGUI_CREATE_NEWTGA_TEXTURE(m_iNorth, m_szNorthImg);
	VGUI_CREATE_NEWTGA_TEXTURE(m_iViewAngle, m_szViewangleImg);
	VGUI_CREATE_NEWTGA_TEXTURE(m_iRoundBackground, m_szBackgroundRoundImg);
}

CRadarPanel::~CRadarPanel(){
	if (m_hRadarBufferFBO)
		glDeleteFramebuffers(1, &m_hRadarBufferFBO);
	if (m_hRadarBufferTex)
		glDeleteTextures(1, &m_hRadarBufferTex);
}

void CRadarPanel::Paint(){
	if (!g_pViewPort->HasSuit())
		return;
	float flTime = gEngfuncs.GetClientTime();
	int size = GetWide();
	int iStartX;
	int iStartY;
	GetPos(iStartX, iStartY);
	if (flFinishScaleTime > 0) {
		int iMaxSize = gScreenInfo.iHeight - 2 * iStartY;
		size = flFinishScaleTime > flTime ?
			(size + (iMaxSize - size) * (1 - (flFinishScaleTime - flTime) / m_flRadarSclaeTime)) :
			iMaxSize;
	}
	float sizeMap = size * m_iGapSize;
	float sizeGap = (size - sizeMap) / 2;
	
	//计算纹理坐标
	float h, w, stx, sty;
	h = size / gScreenInfo.iHeight;
	w = size / gScreenInfo.iWidth;
	stx = (1 - w) / 2;
	sty = (1 - h) / 2;
	//绘制背景
	vgui::surface()->DrawSetTexture(-1);
	vgui::surface()->DrawSetColor(m_cOutline);
	vgui::surface()->DrawSetTexture(gCVars.pRadar->value > 1 ? m_iRoundBackground : m_iBackground);
	vgui::surface()->DrawTexturedRect(iStartX - sizeGap, iStartY - sizeGap, iStartX + size, iStartX + size);
	//shader
	GL_UseProgram(pp_texround.program);
	if (gCVars.pRadar->value > 1) {
		GL_Uniform1f(pp_texround.rad, min(1.0f, m_flRoundRadius / sizeMap));
		GL_Uniform3f(pp_texround.xys, iStartX, iStartY, sizeMap);
	}
	else
		GL_Uniform1f(pp_texround.rad, 0);

	glEnable(GL_TEXTURE_2D);
	glBind(m_hRadarBufferTex);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(m_cMap.r(), m_cMap.g(), m_cMap.b(), m_cMap.a());
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
	vgui::surface()->DrawSetTexture(-1);
	if (gCVars.pRadar->value == 1) {
		vgui::surface()->DrawSetColor(m_cOutline);
		vgui::surface()->DrawSetTexture(m_iUpGround);
		vgui::surface()->DrawTexturedRect(iStartX - sizeGap, iStartY - sizeGap, iStartX + size, iStartX + size);
	}
	//绘制箭头
	vgui::surface()->DrawSetColor(GetFgColor());
	vgui::surface()->DrawSetTexture(m_iViewAngle);
	vgui::surface()->DrawTexturedRect(
		iStartX + (size - m_iAngleSize) / 2,
		iStartY + (size - m_iAngleSize) / 2,
		iStartX + (size + m_iAngleSize) / 2,
		iStartY + (size + m_iAngleSize) / 2);
	//绘制指北针
	float rotate = mathlib::Q_DEG2RAD(gEngfuncs.GetLocalPlayer()->curstate.angles[Q_YAW]);
	h = gCVars.pRadar->value > 1 ? size / 2 : mathlib::fsqrt(2 * pow(size, 2)) / 2;
	stx = mathlib::clamp(((iStartX + size / 2) + h * cos(rotate)), (float)iStartX, (float)iStartX + size);
	sty = mathlib::clamp(((iStartY + size / 2) + h * sin(rotate)), (float)iStartY, (float)iStartY + size);
	vgui::surface()->DrawSetColor(m_cOutline);
	vgui::surface()->DrawSetTexture(m_iNorth);
	w = m_iNorthSize / 2;
	vgui::surface()->DrawTexturedRect(stx - w, sty - w, stx + w, sty + w);
}

void CRadarPanel::OnThink(){
	float flTime = gEngfuncs.GetClientTime();
	if (flTime < flNextUpdateTrTime)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	if (local == nullptr)
		return;
	pmtrace_t m_hRadarTr;
	CVector vecEndpos = local->curstate.origin;
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
	flNextUpdateTrTime = flTime + 1.0f;
}
void CRadarPanel::ApplySettings(KeyValues* inResourceData){
	m_flRadarSclaeTime = inResourceData->GetFloat("radar_scaletime", 2.0f);
	m_flRoundRadius = inResourceData->GetFloat("radar_roundradius", 344.0f);
	m_iGapSize = inResourceData->GetInt("radar_gapsize", 2);
	m_iAngleSize = inResourceData->GetInt("radar_anglesize", 2);
	m_iNorthSize = inResourceData->GetInt("radar_northsize", 2);

	strcpy(m_szBackgroundImg, inResourceData->GetString("radar_backgroundimg", "abcenchance/tga/radar_background"));
	strcpy(m_szUpgroundImg, inResourceData->GetString("radar_upgroundimg", "abcenchance/tga/radar_upground"));
	strcpy(m_szNorthImg, inResourceData->GetString("radar_northimg", "abcenchance/tga/radar_north"));
	strcpy(m_szViewangleImg, inResourceData->GetString("radar_viewangleimg", "abcenchance/tga/radar_viewangle"));
	strcpy(m_szBackgroundRoundImg, inResourceData->GetString("radar_roundbackgroundimg", "abcenchance/tga/radar_roundbackground"));
}
void CRadarPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	m_cOutline = GetSchemeColor("Radar.OutlineColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cMap = GetSchemeColor("Radar.MapColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	SetFgColor(GetSchemeColor("Radar.FgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
}
const char* CRadarPanel::GetName(){
	return VIEWPORT_RADAR_NAME;
}
void CRadarPanel::Reset(){
	flNextUpdateTrTime = 0;
	flFinishScaleTime = 0;
	cvar_t* pCvarDevC = CVAR_GET_POINTER("dev_overview_color");
	if (pCvarDevC && g_metaplugins.renderer) {
		sscanf_s(pCvarDevC->string, "%d %d %d", &iOverviewR, &iOverviewG, &iOverviewB);
		iOverviewR /= 255;
		iOverviewG /= 255;
		iOverviewB /= 255;
	}
}
void CRadarPanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CRadarPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CRadarPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CRadarPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}
void CRadarPanel::SetScaleTime(float time){
	if (time < 0)
		flFinishScaleTime = gEngfuncs.GetClientTime() + m_flRadarSclaeTime;
	else
		flFinishScaleTime = time;
}
void CRadarPanel::BlitFramebuffer(){
	//Blit color from current framebuffer into texture
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	GL_BlitFrameBufferToFrameBufferColorOnly(m_oldFrameBuffer, m_hRadarBufferFBO, gScreenInfo.iWidth, gScreenInfo.iHeight, gScreenInfo.iWidth, gScreenInfo.iHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}
