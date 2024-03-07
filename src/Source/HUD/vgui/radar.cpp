#include <metahook.h>

#include <cmath>
#include "mymathlib.h"

#include "local.h"
#include "steamclientpublic.h"

#include "exportfuncs.h"

#include "hud.h"
#include "vguilocal.h"
#include "playertrace.h"

#include "glew.h"
#include "gl_utility.h"
#include "gl_def.h"
#include "gl_shader.h"
#include "gl_draw.h"

#include "vgui_controls/ImagePanel.h"
#include "avatar_image.h"

#include "player_info.h"
#include <CCustomHud.h>
#include "Viewport.h"
#include "radar.h"

#undef clamp

CRadarAvatarPanel::CRadarAvatarPanel(vgui::Panel* parent, int index) : BaseClass(parent, "") {
	m_iIndex = index + 1;
	m_pAvatar = new vgui::CAvatarImage();
	SetImage(m_pAvatar);
}
void CRadarAvatarPanel::Paint(){
	CPlayerInfo* pi = CPlayerInfo::GetPlayerInfo(m_iIndex);
	if (pi->IsValid()) {
		m_pAvatar->SetAvatarSteamID(*pi->GetSteamID());
		m_pAvatar->SetDrawFriend(true);
		m_pAvatar->SetPos(GetXPos(), GetYPos());
		m_pAvatar->SetSize(GetWide(), GetTall());
		m_pAvatar->Paint();
	}
}

class CRadarMapImage : public vgui::IImage_HL25 {
public:
	virtual void Destroy() {
		delete this;
	}

	virtual void SetAdditive(bool bIsAdditive) {

	}

	virtual void Paint() override{
		//计算屏幕绝对坐标
		int x = m_iX;
		int y = m_iY;
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
		glTexCoord2f(stx, sty);
		glVertex2f(x, y + m_iTall);
		glTexCoord2f(stx + w, sty);
		glVertex2f(x + m_iWide, y + m_iTall);
		glTexCoord2f(stx + w, sty + h);
		glVertex2f(x + m_iWide, y);
		glTexCoord2f(stx, sty + h);
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
	void SetRadius(float r) {
		m_flRoundRadius = r;
	}
	virtual ~CRadarMapImage() {
		if (m_hBufferTex)
			glDeleteTextures(1, &m_hBufferTex);
	}
private:
	float m_flRoundRadius = 0;
	int m_iOffX = 0, m_iOffY = 0;
	int m_iX = 0, m_iY = 0;
	int m_iWide = 0, m_iTall = 0;
	uint m_hBufferTex;
	Color m_DrawColor = Color(255, 255, 255, 255);
};

extern vgui::CViewport* g_pViewPort;
#define VIEWPORT_RADAR_NAME "RadarPanel"
extern vgui::HScheme GetViewPortBaseScheme();
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
	SetScheme(GetViewPortBaseScheme());

	ADD_COMMAND("+scale_radar", [](){g_pViewPort->GetRadarPanel()->SetScale(true); });
	ADD_COMMAND("-scale_radar", []() {g_pViewPort->GetRadarPanel()->SetScale(false); });

	gCVars.pRadar = CREATE_CVAR("cl_radar", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		g_pViewPort->GetRadarPanel()->ShowPanel(cvar->value);
	});
	gCVars.pRadarZoom = CREATE_CVAR("cl_radarzoom", "2.5", FCVAR_VALUE, nullptr);
	gCVars.pRadarAvatar = CREATE_CVAR("cl_radar_avatar", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		g_pViewPort->GetRadarPanel()->SetAvatarVisible(cvar->value);
	});
	gCVars.pRadarAvatarSize = CREATE_CVAR("cl_radar_avatarsize", "20", FCVAR_VALUE, nullptr);
	gCVars.pRadarAvatarScale = CREATE_CVAR("cl_radar_avatarscale", "0.2", FCVAR_VALUE, nullptr);
	gCVars.pRadarZMin = CREATE_CVAR("cl_radar_zmin", "256", FCVAR_VALUE, nullptr);
	gCVars.pRadarZMax = CREATE_CVAR("cl_radar_zmax", "20", FCVAR_VALUE, nullptr);

	m_pBackground = new vgui::ImagePanel(this, "Background");
	m_pRoundBackground = new vgui::ImagePanel(this, "RoundBackground");
	m_pMapground = new vgui::ImagePanel(this, "Mapground");
	m_pUpground = new vgui::ImagePanel(this, "Upground");
	m_pNorthground = new vgui::ImagePanel(this, "Northground");
	m_pViewangleground = new vgui::ImagePanel(this, "Viewangleground");
	for (size_t i = 0; i < 32; i++){
		m_aryPlayerAvatars[i] = new CRadarAvatarPanel(this, i);
		m_aryPlayerAvatars[i]->SetVisible(true);
		m_aryPlayerAvatars[i]->SetShouldScaleImage(true);
	}
	LoadControlSettings(VGUI2_ROOT_DIR "RadarPanel.res");
	auto radarimg = new CRadarMapImage();
	radarimg->SetTex(tex);
	int w, h;
	m_pMapground->GetSize(w, h);
	radarimg->SetSize(w, h);
	radarimg->SetRadius(m_flRoundRadius);
	m_pMapground->SetImage(radarimg);
}

CRadarPanel::~CRadarPanel(){
	if (m_hRadarBufferFBO)
		glDeleteFramebuffers(1, &m_hRadarBufferFBO);
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
		float rotate = CMathlib::Q_DEG2RAD(local->curstate.angles[CMathlib::Q_YAW]);
		int hh = gCVars.pRadar->value > 1 ? len / 2 : sqrt(2 * pow(len, 2)) / 2;
		int stx = CMathlib::clamp(((size / 2) + hh * cos(rotate)), 0.0f, (float)len);
		int sty = CMathlib::clamp(((size / 2) + hh * sin(rotate)), 0.0f, (float)len);
		m_pNorthground->SetPos(stx, sty);

		if (gCVars.pRadarAvatar->value > 0) {
			float size = GetWide();
			float Length = size / 2;
			float ww = gCVars.pRadarAvatarSize->value;
			float w = ww / 2;
			for (size_t i = 0; i < 32; i++) {
				auto iter = m_aryPlayerAvatars[i];
				CPlayerInfo* pi = CPlayerInfo::GetPlayerInfo(i + 1);
				if (!pi->IsValid()) {
					iter->SetVisible(false);
					continue;
				}
				CPlayerInfo* lpi = CPlayerInfo::GetThisPlayerInfo();
				if (pi->GetTeamNumber() != lpi->GetTeamNumber()) {
					iter->SetVisible(false);
					continue;
				}
				//Avatar
				cl_entity_t* entity = gEngfuncs.GetEntityByIndex(i + 1);
				if (!entity || entity->curstate.messagenum != local->curstate.messagenum || !entity->player || !entity->model || local == entity) {
					iter->SetVisible(false);
					continue;
				}
				iter->SetVisible(true);
				Vector vecLength;
				//与目标距离
				CMathlib::VectorSubtract(entity->curstate.origin, local->curstate.origin, vecLength);
				Vector vecAngle;
				CMathlib::VectorAngles(vecLength, vecAngle);
				float nyaw = CMathlib::Q_DEG2RAD(vecAngle[CMathlib::Q_YAW] - local->curstate.angles[CMathlib::Q_YAW] + 90);
				//缩放比率暂定0.2，交换取反符合屏幕坐标系
				swap(vecLength.x, vecLength.y);
				vecLength *= (-1.0f * gCVars.pRadarAvatarScale->value);
				vecLength.z = 0;
				float vlen = vecLength.Length();
				int ale = GetWide() - ww;
				int ahh = gCVars.pRadar->value > 1 ? vlen / 2 : sqrt(2 * pow(vlen, 2)) / 2;
				int atx = CMathlib::clamp((Length - w + ahh * cos(nyaw)), 0.0f, static_cast<float>(ale));
				int aty = CMathlib::clamp((Length - w + ahh * sin(nyaw)), 0.0f, static_cast<float>(ale));
				aty = ale - aty;
				iter->SetPos(atx, aty);
				iter->SetSize(ww, ww);
			}
		}
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

	gCustomHud.m_flOverViewZmax = GetPlayerTrace()->Get(CPlayerTrace::TRACE_TYPE::HEAD)->endpos[2] - gCVars.pRadarZMax->value;
	gCustomHud.m_flOverViewZmin = GetPlayerTrace()->Get(CPlayerTrace::TRACE_TYPE::FOOT)->endpos[2] - gCVars.pRadarZMin->value;
	flNextUpdateTrTime = flTime + 1.0f;
}
void CRadarPanel::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
	GetSize(m_iStartWidth, m_iStartTall);

	m_flRoundRadius = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetFloat("radar_roundradius", 100.0f));
	m_iScaledWidth = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("scaled_wide", 300));
	m_iScaledTall = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("scaled_tall", 300));
}
void CRadarPanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	m_cOutline = GetSchemeColor("Radar.OutlineColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cMap = GetSchemeColor("Radar.MapColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	SetFgColor(GetSchemeColor("Radar.FgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	SetBgColor(GetSchemeColor("Radar.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
const char* CRadarPanel::GetName(){
	return VIEWPORT_RADAR_NAME;
}
void CRadarPanel::Reset(){
	flNextUpdateTrTime = 0;
	SetScale(false);
	cvar_t* pCvarDevC = CVAR_GET_POINTER("dev_overview_color");
	if (pCvarDevC && g_metaplugins.renderer.has) {
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
bool g_bInRenderRadar = false;
void CRadarPanel::RenderRadar(){
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hRadarBufferFBO);
	//设置到玩家脑袋上朝下看
	gCustomHud.m_flOverViewScale = gCVars.pRadarZoom->value;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	gCustomHud.m_vecOverViewOrg[0] = local->curstate.origin[0];
	gCustomHud.m_vecOverViewOrg[1] = local->curstate.origin[1];
	gCustomHud.m_flOverViewYaw = local->curstate.angles[CMathlib::Q_YAW];
	std::vector<std::pair<cvar_t*, float>> arySaveCvars = {
		{gCVars.pCVarDevOverview, 0.0f},
		{gCVars.pCVarDrawEntities, 0.0f},
		{gCVars.pCVarDrawViewModel, 0.0f},
		{gCVars.pCVarDrawDynamic, 0.0f},
		{gCVars.pCVarFXAA, 0.0f},
		{gCVars.pCVarWater, 0.0f},
		{gCVars.pCVarShadow, 0.0f}
	};
	for (auto iter = arySaveCvars.begin(); iter != arySaveCvars.end(); iter++) {
		if ((*iter).first)
			(*iter).second = (*iter).first->value;
	}
	gCVars.pCVarDevOverview->value = 2;
	gCVars.pCVarDrawEntities->value = 0;
	gCVars.pCVarDrawViewModel->value = 0;
	gCVars.pCVarDrawDynamic->value = 0;
	if (gCVars.pCVarFXAA)
		gCVars.pCVarFXAA->value = 0;
	if (gCVars.pCVarWater)
		gCVars.pCVarWater->value = 0;
	if (gCVars.pCVarShadow)
		gCVars.pCVarShadow->value = 0;
	g_bInRenderRadar = true;
	gHookFuncs.R_RenderScene();
	g_bInRenderRadar = false;
	for (auto iter = arySaveCvars.begin(); iter != arySaveCvars.end(); iter++) {
		if ((*iter).first)
			(*iter).first->value = (*iter).second;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}
void CRadarPanel::SetScale(bool state){
	if (state)
		SetSize(m_iScaledWidth, m_iScaledTall);
	else
		SetSize(m_iStartWidth, m_iStartTall);
	InvalidateLayout();
}
void CRadarPanel::SetAvatarVisible(bool state){
	for (auto iter = m_aryPlayerAvatars.begin(); iter != m_aryPlayerAvatars.end(); iter++) {
		(*iter)->SetVisible(state);
	}
}
