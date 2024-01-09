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

#include "vgui_controls/ImagePanel.h"
#include <CCustomHud.h>
#include "Viewport.h"
#include "radar.h"

class CRadarImage : public vgui::IImage {
public:
	virtual void Paint() override{
		//计算屏幕绝对坐标
		int x = m_iAbsX + m_iX;
		int y = m_iAbsY + m_iY;
		//shader
		GL_UseProgram(pp_texround.program);
		if (gCVars.pRadar->value > 1) {
			GL_Uniform1f(pp_texround.rad, min(1.0f, m_flRoundRadius / m_iWide));
			GL_Uniform3f(pp_texround.xys, x, y, m_iWide);
		}
		else
			GL_Uniform1f(pp_texround.rad, 0.0f);
		//计算纹理坐标
		float h = static_cast<float>(m_iTall) / gScreenInfo.iHeight;
		float w = static_cast<float>(m_iWide) / gScreenInfo.iWidth;
		float stx = (1.0f - w) / 2.0f;
		float sty = (1.0f - h) / 2.0f;
		glEnable(GL_TEXTURE_2D);
		glBind(m_hBufferTex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4ub(m_DrawColor.r(), m_DrawColor.g(), m_DrawColor.b(), m_DrawColor.a());
		glBegin(GL_QUADS);
		glTexCoord2f(stx, sty + h);
		glVertex2f(x, y + m_iTall);
		glTexCoord2f(stx + w, sty + h);
		glVertex2f(x + m_iWide, y + m_iTall);
		glTexCoord2f(stx + w, sty);
		glVertex2f(x + m_iWide, y);
		glTexCoord2f(stx, sty);
		glVertex2f(x, y);
		glEnd();
		glDisable(GL_BLEND);
		GL_UseProgram(0);
	}
	virtual void SetPos(int x, int y) override{
		m_iX = x;
		m_iY = y;
	}
	virtual void SetOffset(int x, int y){
		m_iOffX = x;
		m_iOffY = y;
	}
	virtual void GetContentSize(int& wide, int& tall) override{
		wide = m_iWide;
		tall = m_iTall;
	}
	virtual void GetSize(int& wide, int& tall) override{
		GetContentSize(wide, tall);
	}
	virtual void SetSize(int wide, int tall) override{
		m_iWide = wide;
		m_iTall = tall;
	}
	virtual void SetColor(Color col) override{
		m_DrawColor = col;
	}
	
	void SetTex(uint tex) {
		m_hBufferTex = tex;
	}
	void SetAbsPos(int x, int y) {
		m_iAbsX = x;
		m_iAbsY = y;
	}
	void SetRadius(float r) {
		m_flRoundRadius = r;
	}
	~CRadarImage() {
		if (m_hBufferTex)
			glDeleteTextures(1, &m_hBufferTex);
	}
private:
	float m_flRoundRadius = 0;
	int m_iOffX = 0, m_iOffY = 0;
	int m_iX = 0, m_iY = 0;
	int m_iAbsX = 0, m_iAbsY;
	int m_iWide = 0, m_iTall = 0;
	uint m_hBufferTex;
	Color m_DrawColor = Color(255, 255, 255, 255);
};

extern CViewport* g_pViewPort;
#define VIEWPORT_RADAR_NAME "RadarPanel"
CRadarPanel::CRadarPanel()
	: BaseClass(nullptr, VIEWPORT_RADAR_NAME) {
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);

	glGenFramebuffers(1, &m_hRadarBufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hRadarBufferFBO);
	auto tex = GL_GenTextureRGBA8(gScreenInfo.iWidth, gScreenInfo.iHeight);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(g_pViewPort->GetBaseScheme());

	ADD_COMMAND("+scaleradar", [](){g_pViewPort->GetRadarPanel()->SetScale(true); });
	ADD_COMMAND("-scaleradar", []() {g_pViewPort->GetRadarPanel()->SetScale(false); });

	gCVars.pRadar = CREATE_CVAR("cl_radar", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		g_pViewPort->GetRadarPanel()->ShowPanel(cvar->value);
	});
	gCVars.pRadarZoom = CREATE_CVAR("cl_radarzoom", "2.5", FCVAR_VALUE, nullptr);

	m_pBackground = new vgui::ImagePanel(this, "Background");
	m_pRoundBackground = new vgui::ImagePanel(this, "RoundBackground");
	m_pMapground = new vgui::ImagePanel(this, "Mapground");
	m_pUpground = new vgui::ImagePanel(this, "Upground");
	m_pNorthground = new vgui::ImagePanel(this, "Northground");
	m_pViewangleground = new vgui::ImagePanel(this, "Viewangleground");
	LoadControlSettings(VGUI2_ROOT_DIR "RadarPanel.res");
	auto radarimg = new CRadarImage();
	radarimg->SetTex(tex);
	int x, y;
	m_pMapground->GetPos(x, y);
	radarimg->SetAbsPos(x, y);
	int w, h;
	m_pMapground->GetSize(w, h);
	radarimg->SetSize(w, h);
	radarimg->SetRadius(m_flRoundRadius);
	m_pMapground->SetImage(radarimg);
}

CRadarPanel::~CRadarPanel(){
	if (m_hRadarBufferFBO)
		glDeleteFramebuffers(1, &m_hRadarBufferFBO);
	delete m_pMapground->GetImage();
}

void CRadarPanel::PerformLayout(){
	BaseClass::PerformLayout();
	int w, h;
	GetSize(w, h);
	m_pBackground->SetSize(w, h);
	m_pRoundBackground->SetSize(w, h);
	m_pMapground->SetSize(w - 4, h - 4);
	m_pUpground->SetSize(w, h);
	int vw, vh;
	m_pViewangleground->GetSize(vw, vh);
	m_pViewangleground->SetPos((w - vw) / 2, (h - vh) / 2);
}

void CRadarPanel::Paint(){
	if (!g_pViewPort->HasSuit())
		return;
	auto local = gEngfuncs.GetLocalPlayer();
	if (local) {
		if (gCVars.pRadar->value > 0) {
			if (gCVars.pRadar->value == 1) {
				m_pRoundBackground->SetVisible(false);
				m_pBackground->SetVisible(true);
			}
			else {
				m_pRoundBackground->SetVisible(true);
				m_pBackground->SetVisible(false);
			}
		}

		int size = GetWide();
		//指北针
		int nw, nh;
		m_pNorthground->GetSize(nw, nh);
		int len = GetWide() - nw;
		float rotate = mathlib::Q_DEG2RAD(local->curstate.angles[Q_YAW]);
		int hh = gCVars.pRadar->value > 1 ? len / 2 : mathlib::fsqrt(2 * pow(len, 2)) / 2;
		int stx = mathlib::clamp(((size / 2) + hh * cos(rotate)), 0.0f, (float)len);
		int sty = mathlib::clamp(((size / 2) + hh * sin(rotate)), 0.0f, (float)len);
		m_pNorthground->SetPos(stx, sty);
	}
	BaseClass::Paint();
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
	BaseClass::ApplySettings(inResourceData);
	m_flRoundRadius = inResourceData->GetFloat("radar_roundradius", 344.0f);
	GetSize(m_iStartWidth, m_iStartTall);
}
void CRadarPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	m_cOutline = GetSchemeColor("Radar.OutlineColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cMap = GetSchemeColor("Radar.MapColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	SetFgColor(GetSchemeColor("Radar.FgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	SetBgColor(Color(0, 0, 0, 0));
}
const char* CRadarPanel::GetName(){
	return VIEWPORT_RADAR_NAME;
}
void CRadarPanel::Reset(){
	flNextUpdateTrTime = 0;
	SetScale(false);
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
void CRadarPanel::SetScale(bool state){
	m_bInScale = state;
	if (state)
		SetSize(m_iStartWidth * gCVars.pRadarZoom->value, m_iStartTall * gCVars.pRadarZoom->value);
	else
		SetSize(m_iStartWidth, m_iStartTall);
}
void CRadarPanel::BlitFramebuffer(){
	//Blit color from current framebuffer into texture
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	GL_BlitFrameBufferToFrameBufferColorOnly(m_oldFrameBuffer, m_hRadarBufferFBO, gScreenInfo.iWidth, gScreenInfo.iHeight, gScreenInfo.iWidth, gScreenInfo.iHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}
