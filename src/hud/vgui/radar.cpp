#include <metahook.h>

#include <cmath>
#include "mymathlib.h"

#include "local.h"
#include "steamclientpublic.h"

#include "exportfuncs.h"

#include "vguilocal.h"
#include "playertrace.h"

#include "glew.h"
#include "gl_utility.h"
#include "gl_def.h"
#include "gl_shader.h"
#include "gl_draw.h"
#include <IMetaRenderer.h>

#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/avatar_image.h"

#include "core/resource/playerresource.h"
#include "hud/Viewport.h"
#include "radar.h"



#undef clamp

class CRadarMapImage : public vgui::IImage_HL25
{
public:
	void Destroy() override
	{
		delete this;
	}

	void SetAdditive(bool bIsAdditive) override
	{

	}

	void Paint() override
	{
		if (MetaRenderer())
		{
			vec4_t vColor4f = { m_DrawColor.r() / 255.0f, m_DrawColor.g() / 255.0f ,m_DrawColor.b() / 255.0f ,m_DrawColor.a() / 255.0f };

			MetaRenderer()->DrawTexturedQuadMask(
				m_BaseTexture,
				m_MaskTexture,
				m_iX + 0,
				m_iY + m_iTall,
				m_iX + m_iWide,
				m_iY + 0,
				vColor4f,
				DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED | DRAW_TEXTURED_RECT_MASK_TEXTURE_ENABLED,
				"CRadarMapImage::Paint");
		}
	}

	void SetPos(int x, int y) override {
		m_iX = x;
		m_iY = y;
	}

	void GetContentSize(int& wide, int& tall) override {
		wide = m_iWide;
		tall = m_iTall;
	}

	void GetSize(int& wide, int& tall) override {
		GetContentSize(wide, tall);
	}

	void SetSize(int wide, int tall) override {
		m_iWide = wide;
		m_iTall = tall;
	}

	void SetColor(Color col) override {
		m_DrawColor = col;
	}

	void SetBaseTexture(uint tex) {
		m_BaseTexture = tex;
	}

	void SetMaskTexture(uint tex) {
		m_MaskTexture = tex;
	}

private:
	int m_iX = 0, m_iY = 0;
	int m_iWide = 0, m_iTall = 0;

	uint m_BaseTexture{};
	uint m_MaskTexture{};

	Color m_DrawColor = Color(255, 255, 255, 255);
};

constexpr auto VIEWPORT_RADAR_NAME = "RadarPanel";

extern vgui::HScheme GetViewPortBaseScheme();

CRadarPanel::CRadarPanel()
	: BaseClass(nullptr, VIEWPORT_RADAR_NAME) {

	if (MetaRenderer())
	{
		m_RadarFBO.iWidth = 1024;
		m_RadarFBO.iHeight = 1024;
		MetaRenderer()->GenFrameBuffer(&m_RadarFBO, "m_RadarFBO");
		MetaRenderer()->FrameBufferColorTexture(&m_RadarFBO, GL_RGBA8);
		MetaRenderer()->FrameBufferDepthTexture(&m_RadarFBO, GL_DEPTH24_STENCIL8);
	}

	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());

	ADD_COMMAND("+scale_radar", []() {GetBaseViewPort()->GetRadarPanel()->SetScale(true); });
	ADD_COMMAND("-scale_radar", []() {GetBaseViewPort()->GetRadarPanel()->SetScale(false); });

	gCVars.pRadar = CREATE_CVAR("hud_radar", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		GetBaseViewPort()->GetRadarPanel()->ShowPanel(cvar->value);
		});

	gCVars.pRadarZMin = CREATE_CVAR("hud_radar_zmin", "256", FCVAR_VALUE, nullptr);
	gCVars.pRadarZMax = CREATE_CVAR("hud_radar_zmax", "20", FCVAR_VALUE, nullptr);
	gCVars.pRadarZoom = CREATE_CVAR("hud_radar_zoom", "2.5", FCVAR_VALUE, nullptr);

	gCVars.pRadarAvatar = CREATE_CVAR("hud_radar_avatar", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		GetBaseViewPort()->GetRadarPanel()->SetAvatarVisible(cvar->value);
		});
	gCVars.pRadarAvatarSize = CREATE_CVAR("hud_radar_avatarsize", "20", FCVAR_VALUE, nullptr);
	gCVars.pRadarAvatarScale = CREATE_CVAR("hud_radar_avatarscale", "0.2", FCVAR_VALUE, nullptr);

	m_pBackground = new vgui::ImagePanel(this, "Background");
	m_pRoundBackground = new vgui::ImagePanel(this, "RoundBackground");
	m_pMapground = new vgui::ImagePanel(this, "Mapground");
	m_pUpground = new vgui::ImagePanel(this, "Upground");
	m_pNorthground = new vgui::ImagePanel(this, "Northground");
	m_pViewangleground = new vgui::ImagePanel(this, "Viewangleground");
	for (size_t i = 0; i < 32; i++) {
		m_aryPlayerAvatars[i] = new vgui::CAvatarImagePanel(this, "Avatar");
		m_aryPlayerAvatars[i]->SetVisible(true);
		m_aryPlayerAvatars[i]->SetShouldScaleImage(true);
	}
	LoadControlSettings(VGUI2_ROOT_DIR "RadarPanel.res");

	m_iRadarRoundBackgroundTextureId = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_iRadarRoundBackgroundTextureId, "abcenchance/tga/radar_background", true, false);

	m_pRadarImage = new CRadarMapImage();
	m_pRadarImage->SetBaseTexture(m_RadarFBO.s_hBackBufferTex);
	m_pRadarImage->SetMaskTexture(m_iRadarRoundBackgroundTextureId);

	int w{}, h{};
	m_pMapground->GetSize(w, h);
	m_pRadarImage->SetSize(w, h);

	m_pMapground->SetImage(m_pRadarImage);
}

CRadarPanel::~CRadarPanel() {

	if (m_pRadarImage)
	{
		delete m_pRadarImage;
		m_pRadarImage = nullptr;
	}

	if (m_iRadarRoundBackgroundTextureId)
	{
		vgui::surface()->DeleteTextureByID(m_iRadarRoundBackgroundTextureId);
		m_iRadarRoundBackgroundTextureId = 0;
	}

	if (MetaRenderer() && m_RadarFBO.s_hBackBufferFBO)
	{
		MetaRenderer()->FreeFBO(&m_RadarFBO);
	}
}

void CRadarPanel::PerformLayout()
{
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

void CRadarPanel::Paint()
{
	if (!GetBaseViewPort()->HasSuit())
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

		int nw, nh;
		m_pNorthground->GetSize(nw, nh);
		int len = GetWide() - nw;
		float rotate = CMathlib::Q_DEG2RAD(local->curstate.angles[CMathlib::Q_YAW]);
		int hh = gCVars.pRadar->value > 1.0f ? (len / 2) : static_cast<int>(sqrt(2 * pow(len, 2.0f)) / 2.0f);
		int stx = CMathlib::clamp(((size / 2.0f) + hh * cos(rotate)), 0.0f, (float)len);
		int sty = CMathlib::clamp(((size / 2.0f) + hh * sin(rotate)), 0.0f, (float)len);
		m_pNorthground->SetPos(stx, sty);

		if (gCVars.pRadarAvatar->value > 0) {
			float size = GetWide();
			float Length = size / 2;
			float ww = gCVars.pRadarAvatarSize->value;
			float w = ww / 2;
			for (size_t i = 0; i < 32; i++) {
				auto iter = m_aryPlayerAvatars[i];
				PlayerInfo* pi = gPlayerRes.GetPlayerInfo(i + 1);
				if (!pi->IsValid()) {
					iter->SetVisible(false);
					continue;
				}
				PlayerInfo* lpi = gPlayerRes.GetLocalPlayerInfo();
				if (pi->m_iTeamNumber != lpi->m_iTeamNumber) {
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

				Vector vecLength = entity->curstate.origin;
				vecLength -= local->curstate.origin;
				Vector vecAngle;
				CMathlib::VectorAngles(vecLength, vecAngle);
				float nyaw = CMathlib::Q_DEG2RAD(vecAngle[CMathlib::Q_YAW] - local->curstate.angles[CMathlib::Q_YAW] + 90);

				std::swap(vecLength.x, vecLength.y);
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

void CRadarPanel::ApplySettings(KeyValues* inResourceData)
{
	BaseClass::ApplySettings(inResourceData);
	GetSize(m_iStartWidth, m_iStartTall);

	m_flRoundRadius = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetFloat("radar_roundradius", 100.0f));
	m_iScaledWidth = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("scaled_wide", 300));
	m_iScaledTall = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("scaled_tall", 300));
}

void CRadarPanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	m_cOutline = GetSchemeColor("Radar.OutlineColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cMap = GetSchemeColor("Radar.MapColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	SetFgColor(GetSchemeColor("Radar.FgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	SetBgColor(GetSchemeColor("Radar.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}

const char* CRadarPanel::GetName()
{
	return VIEWPORT_RADAR_NAME;
}

void CRadarPanel::Reset()
{
	SetScale(false);
	cvar_t* pCvarDevC = CVAR_GET_POINTER("dev_overview_color");
	if (pCvarDevC && MetaRenderer()) {
		sscanf_s(pCvarDevC->string, "%d %d %d", &iOverviewR, &iOverviewG, &iOverviewB);
		iOverviewR /= 255;
		iOverviewG /= 255;
		iOverviewB /= 255;
	}
}

void CRadarPanel::ShowPanel(bool state)
{
	if (state == IsVisible())
		return;

	SetVisible(state);
}

bool CRadarPanel::IsVisible()
{
	return BaseClass::IsVisible();
}

vgui::VPANEL CRadarPanel::GetVPanel()
{
	return BaseClass::GetVPanel();
}

void CRadarPanel::SetParent(vgui::VPANEL parent)
{
	BaseClass::SetParent(parent);
}

void CRadarPanel::RenderRadar()
{
	if (MetaRenderer())
	{
		MetaRenderer()->BeginDebugGroup("CRadarPanel::RenderRadar");
		MetaRenderer()->BindFrameBuffer(&m_RadarFBO);
		MetaRenderer()->SetCurrentSceneFBO(&m_RadarFBO);

		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		VectorCopy(local->curstate.origin, m_vecOverViewOrigin);
		m_vecOverViewOrigin[2] += 72;

		VectorCopy(local->curstate.angles, m_vecOverViewAngles);
		m_vecOverViewAngles[0] = 90;
		m_vecOverViewAngles[2] = 0;

		float arySaveCvars[] = {
			gCVars.pCVarDrawEntities->value,
			gCVars.pCVarDrawViewModel->value,
			gCVars.pCVarDrawDynamic->value,
			gCVars.pCVarFXAA ? gCVars.pCVarFXAA->value : 0.0f,
			gCVars.pCVarWater ? gCVars.pCVarWater->value : 0.0f,
			gCVars.pCVarShadow ? gCVars.pCVarShadow->value : 0.0f,
			gCVars.pCVarDeferredLighting ? gCVars.pCVarDeferredLighting->value : 0.0f,
			gCVars.pCVarGammaBlend ? gCVars.pCVarGammaBlend->value : 0.0f,
		};
		gCVars.pCVarDrawEntities->value = 0;
		gCVars.pCVarDrawViewModel->value = 0;
		gCVars.pCVarDrawDynamic->value = 0;
		if (gCVars.pCVarFXAA)
			gCVars.pCVarFXAA->value = 0;
		if (gCVars.pCVarWater)
			gCVars.pCVarWater->value = 0;
		if (gCVars.pCVarShadow)
			gCVars.pCVarShadow->value = 0;
		if (gCVars.pCVarDeferredLighting)
			gCVars.pCVarDeferredLighting->value = 0;
		if (gCVars.pCVarGammaBlend)
			gCVars.pCVarGammaBlend->value = 1;

		MetaRenderer()->PushRefDef();

		MetaRenderer()->SetMultiviewEnabled(true);

		auto oldDrawClassify = MetaRenderer()->GetDrawClassify();
		MetaRenderer()->SetDrawClassify(DRAW_CLASSIFY_WORLD | DRAW_CLASSIFY_LIGHTMAP);

		MetaRenderer()->SetRefDefViewOrigin(m_vecOverViewOrigin);
		MetaRenderer()->SetRefDefViewAngles(m_vecOverViewAngles);

		MetaRenderer()->UpdateRefDef();

		MetaRenderer()->LoadIdentityForProjectionMatrix();
		//MetaRenderer()->SetupOrthoProjectionMatrix(-1024 / 2, 1024 / 2, -1024 / 2, 1024 / 2, 2048, -2048, true);

		float scale = gCVars.pRadarZoom->value;
		MetaRenderer()->SetupOrthoProjectionMatrix(
			-(4096.0 / scale),
			(4096.0 / scale),
			-(4096.0 / scale),
			(4096.0 / scale),
			2048, 
			-2048,
			true);

		MetaRenderer()->LoadIdentityForWorldMatrix();
		MetaRenderer()->SetupPlayerViewWorldMatrix(m_vecOverViewOrigin, m_vecOverViewAngles);

		MetaRenderer()->SetViewport(0, 0, 1024, 1024);

		camera_ubo_t CameraUBO{};
		MetaRenderer()->SetupCameraView(&CameraUBO.views[0]);
		CameraUBO.numViews = 1;

		MetaRenderer()->UploadCameraUBOData(&CameraUBO);

		vec4_t vClearColor = { 0, 0, 0, 1 };
		MetaRenderer()->ClearColorDepthStencil(vClearColor, 1, 0, STENCIL_MASK_ALL);

		MetaRenderer()->RenderScene();

		MetaRenderer()->SetDrawClassify(oldDrawClassify);

		MetaRenderer()->SetMultiviewEnabled(false);

		MetaRenderer()->PopRefDef();

		gCVars.pCVarDrawEntities->value = arySaveCvars[0];
		gCVars.pCVarDrawViewModel->value = arySaveCvars[1];
		gCVars.pCVarDrawDynamic->value = arySaveCvars[2];
		if (gCVars.pCVarFXAA)
			gCVars.pCVarFXAA->value = arySaveCvars[3];
		if (gCVars.pCVarWater)
			gCVars.pCVarWater->value = arySaveCvars[4];
		if (gCVars.pCVarShadow)
			gCVars.pCVarShadow->value = arySaveCvars[5];
		if (gCVars.pCVarDeferredLighting)
			gCVars.pCVarDeferredLighting->value = arySaveCvars[6];
		if (gCVars.pCVarGammaBlend)
			gCVars.pCVarGammaBlend->value = arySaveCvars[7];

		MetaRenderer()->EndDebugGroup();
	}
}

void CRadarPanel::SetScale(bool state)
{
	if (state)
		SetSize(m_iScaledWidth, m_iScaledTall);
	else
		SetSize(m_iStartWidth, m_iStartTall);
	InvalidateLayout();
}

void CRadarPanel::SetAvatarVisible(bool state)
{
	for (auto iter = m_aryPlayerAvatars.begin(); iter != m_aryPlayerAvatars.end(); iter++) {
		(*iter)->SetVisible(state);
	}
}
