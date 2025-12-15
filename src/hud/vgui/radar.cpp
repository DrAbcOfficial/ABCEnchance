#include <metahook.h>

#include <cmath>
#include <algorithm>

#include "IMetaRenderer.h"



#include "utility/util.h"

#include "utility/vgui_util.h"


#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/avatar_image.h"

#include "core/library/mymathlib.h"
#include "core/resource/playerresource.h"

#include "hud/Viewport.h"
#include "radar.h"

#undef clamp
#undef max

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
			vec4_t vColor4f = { 
				(m_DrawColor.r() / 255.0f) * m_flBrightness, 
				(m_DrawColor.g() / 255.0f) * m_flBrightness, 
				(m_DrawColor.b() / 255.0f) * m_flBrightness, 
				m_DrawColor.a() / 255.0f 
			};
			MetaRenderer()->DrawTexturedQuadMask(
				m_BaseTexture,
				m_MaskTexture,
				m_iX + 0,
				m_iY + m_iTall,
				m_iX + m_iWide,
				m_iY + 0,
				vColor4f,
				DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED | DRAW_TEXTURED_RECT_MASK_TEXTURE_ENABLED,
				"CRadarMapImage::Paint"
			);
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

	void SetBrightness(float brightness) {
		m_flBrightness = std::max(0.0f, brightness);
	}

private:
	int m_iX = 0, m_iY = 0;
	int m_iWide = 0, m_iTall = 0;

	uint m_BaseTexture{};
	uint m_MaskTexture{};

	Color m_DrawColor = Color(255, 255, 255, 255);
	float m_flBrightness = 1.0f;
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
	gCVars.pRadarZoom = CREATE_CVAR("hud_radar_zoom", "2.5", FCVAR_VALUE, nullptr);

	gCVars.pRadarAvatar = CREATE_CVAR("hud_radar_avatar", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		GetBaseViewPort()->GetRadarPanel()->SetAvatarVisible(cvar->value);
		});
	gCVars.pRadarAvatarSize = CREATE_CVAR("hud_radar_avatarsize", "20", FCVAR_VALUE, nullptr);

	m_pBackground = new vgui::ImagePanel(this, "Background");
	m_pRoundBackground = new vgui::ImagePanel(this, "RoundBackground");
	m_pMapground = new vgui::ImagePanel(this, "Mapground");
	m_pNorthground = new vgui::ImagePanel(this, "Northground");
	m_pViewangleground = new vgui::ImagePanel(this, "Viewangleground");
	for (size_t i = 0; i < 32; i++) {
		m_aryPlayerAvatars[i] = new vgui::CAvatarImagePanel(this, "Avatar");
		m_aryPlayerAvatars[i]->SetVisible(false);
		m_aryPlayerAvatars[i]->SetShouldScaleImage(true);
	}
	LoadControlSettings(VGUI2_ROOT_DIR "RadarPanel.res");

	m_iRadarMaskId = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_iRadarMaskId, "abcenchance/tga/radar_mask", true, false);
	m_iRadarRoundMaskId = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_iRadarRoundMaskId, "abcenchance/tga/radar_roundmask", true, false);

	m_pRadarImage = new CRadarMapImage();
	m_pRadarImage->SetBaseTexture(m_RadarFBO.s_hBackBufferTex);
	m_pRadarImage->SetBrightness(m_flBrightness);
	m_pMapground->SetImage(m_pRadarImage);
	
	m_vClearColor[0] = 0.0f;
	m_vClearColor[1] = 0.0f;
	m_vClearColor[2] = 0.0f;
	m_vClearColor[3] = 1.0f;
}

CRadarPanel::~CRadarPanel() {

	if (m_pRadarImage)
	{
		delete m_pRadarImage;
		m_pRadarImage = nullptr;
	}

	if (m_iRadarMaskId)
	{
		vgui::surface()->DeleteTextureByID(m_iRadarMaskId);
		m_iRadarMaskId = 0;
	}

	if (m_iRadarRoundMaskId)
	{
		vgui::surface()->DeleteTextureByID(m_iRadarRoundMaskId);
		m_iRadarRoundMaskId = 0;
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
	int gw = w * 0.01f;
	int gh = h * 0.01f;
	m_pMapground->SetSize(w - gw, h - gh);
	m_pMapground->SetPos(gw / 2, gh / 2);
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

		// Position north indicator
		int nw, nh;
		m_pNorthground->GetSize(nw, nh);
		int len = GetWide() - nw;
		float rotate = CMathlib::Q_DEG2RAD(local->curstate.angles[CMathlib::Q_YAW]);
		int hh = gCVars.pRadar->value > 1.0f ? (len / 2) : static_cast<int>(sqrt(2 * pow(len, 2.0f)) / 2.0f);
		int stx = std::clamp(((size / 2.0f) + hh * cos(rotate)), 0.0f, (float)len);
		int sty = std::clamp(((size / 2.0f) + hh * sin(rotate)), 0.0f, (float)len);
		m_pNorthground->SetPos(stx, sty);

		// Handle avatar display
		for (size_t i = 0; i < 32; i++) {
			auto iter = m_aryPlayerAvatars[i];
			if (gCVars.pRadarAvatar->value <= 0) {
				iter->SetVisible(false);
				continue;
			}
			
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
			
			cl_entity_t* entity = gEngfuncs.GetEntityByIndex(i + 1);
			if (!entity || entity->curstate.messagenum != local->curstate.messagenum || !entity->player || !entity->model || local == entity) {
				iter->SetVisible(false);
				continue;
			}
			iter->SetPlayer(i + 1, vgui::k_EAvatarSize32x32);
			iter->SetVisible(true);

			// Calculate position on radar
			float radarSize = GetWide();
			float radarCenter = radarSize / 2.0f;
			float avatarSize = gCVars.pRadarAvatarSize->value;
			float avatarHalf = avatarSize / 2.0f;
			
			// Get relative position vector
			Vector relativePos = entity->curstate.origin;
			relativePos -= local->curstate.origin;
			
			// Apply radar scale - this should match the radar rendering scale
			float zoom = gCVars.pRadarZoom->value;
			float worldSize = 4096.0f / zoom;
			float radarScale = radarSize / (2.0f * worldSize);
			
			// Convert world coordinates to radar coordinates
			// In Half-Life: X is forward/back, Y is left/right, Z is up/down
			// For radar: X should be left/right, Y should be forward/back (top-down view)
			// We invert radarY to fix north/south direction
			float radarX = -relativePos.y;   // World Y (left/right) becomes radar X
			float radarY = relativePos.x;  // World X (forward/back) becomes radar Y (inverted for correct north/south)
			
			// Rotate based on local player's yaw angle
			// We need to rotate the relative position so that "forward" is always up on the radar
			float localYaw = CMathlib::Q_DEG2RAD(-local->curstate.angles[CMathlib::Q_YAW]);
			float cosYaw = cos(localYaw);
			float sinYaw = sin(localYaw);
			
			// Apply rotation matrix to align with player's view direction
			float rotatedX = radarX * cosYaw - radarY * sinYaw;
			float rotatedY = radarX * sinYaw + radarY * cosYaw;
			
			// Apply radar scale to match the radar rendering scale
			float scaledX = rotatedX * radarScale;
			float scaledY = rotatedY * radarScale;
			
			// Convert to screen coordinates (center of radar + offset)
			// Note: Screen Y axis points down, so we invert rotatedY
			float screenX = radarCenter + scaledX - avatarHalf;
			float screenY = radarCenter - scaledY - avatarHalf;
			
			// Clamp to radar bounds
			float maxPos = radarSize - avatarSize;
			screenX = std::clamp(screenX, 0.0f, maxPos);
			screenY = std::clamp(screenY, 0.0f, maxPos);
			
			// Handle circular radar clipping
			if (gCVars.pRadar->value > 1) {
				float dx = screenX + avatarHalf - radarCenter;
				float dy = screenY + avatarHalf - radarCenter;
				float distance = sqrt(dx * dx + dy * dy);
				float maxRadius = radarCenter - avatarHalf;
				
				if (distance > maxRadius) {
					// Scale position to fit within circle
					float ratio = maxRadius / distance;
					screenX = radarCenter + dx * ratio - avatarHalf;
					screenY = radarCenter + dy * ratio - avatarHalf;
				}
			}
			
			iter->SetPos(static_cast<int>(screenX), static_cast<int>(screenY));
			iter->SetSize(avatarSize, avatarSize);
		}
	}
	if(gCVars.pRadar->value > 1)
		m_pRadarImage->SetMaskTexture(m_iRadarRoundMaskId);
	else
		m_pRadarImage->SetMaskTexture(m_iRadarMaskId);
	BaseClass::Paint();
}

void CRadarPanel::ApplySettings(KeyValues* inResourceData)
{
	BaseClass::ApplySettings(inResourceData);
	GetSize(m_iStartWidth, m_iStartTall);
	m_iScaledWidth = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("scaled_wide", 300));
	m_iScaledTall = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("scaled_tall", 300));
	m_flBrightness = inResourceData->GetFloat("radar_brightness", 1.0f);
	if (m_pRadarImage) {
		m_pRadarImage->SetBrightness(m_flBrightness);
	}
}

void CRadarPanel::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	auto color = GetSchemeColor("Radar.MapColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	m_pRadarImage->SetColor(color);
	m_pRadarImage->SetBrightness(m_flBrightness);
	SetFgColor(GetSchemeColor("Radar.FgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	SetBgColor(GetSchemeColor("Radar.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	
	auto clearColor = GetSchemeColor("Radar.ClearColor", Color(0, 0, 0, 255), pScheme);
	m_vClearColor[0] = clearColor.r() / 255.0f;
	m_vClearColor[1] = clearColor.g() / 255.0f;
	m_vClearColor[2] = clearColor.b() / 255.0f;
	m_vClearColor[3] = clearColor.a() / 255.0f;
}

const char* CRadarPanel::GetName()
{
	return VIEWPORT_RADAR_NAME;
}

void CRadarPanel::Reset()
{
	SetScale(false);
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
	if (gCVars.pRadar->value <= 0)
		return;
	if (MetaRenderer())
	{
		MetaRenderer()->BeginDebugGroup("CRadarPanel::RenderRadar");
		MetaRenderer()->BindFrameBuffer(&m_RadarFBO);
		MetaRenderer()->SetCurrentSceneFBO(&m_RadarFBO);

		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		VectorCopy(local->curstate.origin, m_vecOverViewOrigin);
		m_vecOverViewOrigin[2] += 72;

		VectorCopy(local->curstate.angles, m_vecOverViewAngles);
		m_vecOverViewAngles[0] = 90;  // Look straight down
		m_vecOverViewAngles[2] = 0;   // No roll

		float arySaveCvars[] = {
			gCVars.pCVarDrawEntities->value,
			gCVars.pCVarDrawViewModel->value,
			gCVars.pCVarDrawDynamic->value,
			gCVars.pCVarWater ? gCVars.pCVarWater->value : 0.0f,
			gCVars.pCVarShadow ? gCVars.pCVarShadow->value : 0.0f,
			gCVars.pCVarDeferredLighting ? gCVars.pCVarDeferredLighting->value : 0.0f,
		};
		gCVars.pCVarDrawEntities->value = 0;
		gCVars.pCVarDrawViewModel->value = 0;
		gCVars.pCVarDrawDynamic->value = 0;
		if (gCVars.pCVarWater)
			gCVars.pCVarWater->value = 0;
		if (gCVars.pCVarShadow)
			gCVars.pCVarShadow->value = 0;
		if (gCVars.pCVarDeferredLighting)
			gCVars.pCVarDeferredLighting->value = 0;

		MetaRenderer()->PushRefDef();

		MetaRenderer()->SetMultiviewEnabled(true);

		auto oldDrawClassify = MetaRenderer()->GetDrawClassify();
		MetaRenderer()->SetDrawClassify(DRAW_CLASSIFY_WORLD | DRAW_CLASSIFY_LIGHTMAP);

		MetaRenderer()->SetRefDefViewOrigin(m_vecOverViewOrigin);
		MetaRenderer()->SetRefDefViewAngles(m_vecOverViewAngles);

		MetaRenderer()->UpdateRefDef();

		MetaRenderer()->LoadIdentityForProjectionMatrix();
		//MetaRenderer()->SetupOrthoProjectionMatrix(-1024 / 2, 1024 / 2, -1024 / 2, 1024 / 2, 2048, -2048, true);

		float zoom = gCVars.pRadarZoom->value;
		float worldSize = 4096.0f / zoom;
		MetaRenderer()->SetupOrthoProjectionMatrix(
			-worldSize,
			worldSize,
			-worldSize,
			worldSize,
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

		MetaRenderer()->ClearColorDepthStencil(m_vClearColor, 1, 0, STENCIL_MASK_ALL);
		
		MetaRenderer()->RenderScene();

		MetaRenderer()->SetDrawClassify(oldDrawClassify);

		MetaRenderer()->SetMultiviewEnabled(false);

		MetaRenderer()->PopRefDef();

		gCVars.pCVarDrawEntities->value = arySaveCvars[0];
		gCVars.pCVarDrawViewModel->value = arySaveCvars[1];
		gCVars.pCVarDrawDynamic->value = arySaveCvars[2];
		gCVars.pCVarWater->value = arySaveCvars[3];
		gCVars.pCVarShadow->value = arySaveCvars[4];
		gCVars.pCVarDeferredLighting->value = arySaveCvars[5];

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
		if (!state) {
			(*iter)->SetVisible(false);
		}
	}
}

void CRadarPanel::RefreshAvatars()
{
	for (auto iter = m_aryPlayerAvatars.begin(); iter != m_aryPlayerAvatars.end(); iter++) {
		(*iter)->ClearAvatar();
	}
}
