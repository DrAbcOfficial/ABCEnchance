#pragma once
#include <metahook.h>
#include <algorithm> 
#include <array>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/AnimationController.h>
#include "mymathlib.h"
#include "vguilocal.h"
#include "local.h"
#include "gl_draw.h"
#include "indicator.h"
#undef max

constexpr auto VIEWPORT_INDICATOR_NAME = "IndicatorPanel";
constexpr auto CVAR_STYLE_NAME = "hud_indicator_style";

using namespace vgui;

enum IndicatorStyle{
	Hide = 0,
	GoldSrc = 1,
	New = 2
};
class IndicatorImage : public IImage {
public:
    virtual void Paint() override {
        float alpha_multi = surface()->DrawGetAlphaMultiplier();
        if(alpha_multi <= 0.0f)
			return;
        if (m_iTexture < 0)
            return;
        int cvar_value = static_cast<int>(CVAR_GET_FLOAT(CVAR_STYLE_NAME));
        if (cvar_value <= Hide)
            return;
        cl_entity_t* local = gEngfuncs.GetLocalPlayer();
        if (!local)
            return;
        Vector player_origin = local->curstate.origin;
        Vector vecFinal = m_vecHitFrom - player_origin;
        CMathlib::VectorAngles(vecFinal, vecFinal);
        vecFinal[CMathlib::Q_YAW] -= local->curstate.angles[CMathlib::Q_YAW];
        float deg_angle = fmod(vecFinal[CMathlib::Q_YAW], 360.0f);
        if (deg_angle < 0.0f) {
            deg_angle += 360.0f;
        }
        if (cvar_value == GoldSrc) {
            if ((deg_angle >= 0.0f && deg_angle <= 45.0f) || (deg_angle >= 315.0f && deg_angle <= 360.0f))
                deg_angle = 0.0f;
            else if (deg_angle > 45.0f && deg_angle <= 135.0f)
                deg_angle = 90.0f;
            else if (deg_angle > 135.0f && deg_angle <= 225.0f)
                deg_angle = 180.0f;
            else if (deg_angle > 225.0f && deg_angle <= 315.0f)
                deg_angle = 270.0f;
        }
        float angle = CMathlib::Q_DEG2RAD(deg_angle); 
        float ca = cos(angle);
        float sa = sin(angle);
        const float img_width = static_cast<float>(std::max(m_iWide, 1));
        const float img_height = static_cast<float>(std::max(m_iTall, 1));
        const float img_half_w = img_width / 2.0f;
        const float img_half_h = img_height / 2.0f;
        const float y_base = m_iContainerTall / 2.0f - m_iOffY;
        const float y_top = y_base;
        const float y_bottom = y_base + img_height;
        auto rotateAndOffset = [&](float out[2], float x, float y) {
            float local_x = x + static_cast<float>(m_iX);
			float local_y = y + static_cast<float>(m_iY);

            float rot_x = local_x * ca - local_y * sa;
            float rot_y = local_x * sa + local_y * ca;

            out[0] = rot_x + m_iContainerWide / 2.0f;
            out[1] = m_iContainerTall / 2.0f - rot_y;
            };
        vec2_t vecHUD[4]{};
        rotateAndOffset(vecHUD[0], -img_half_w, y_bottom);
        rotateAndOffset(vecHUD[1], img_half_w, y_bottom);
        rotateAndOffset(vecHUD[2], img_half_w, y_top);
        rotateAndOffset(vecHUD[3], -img_half_w, y_top);

        DrawTexturePos(m_iTexture, kRenderTransAdd, vecHUD[0], vecHUD[1], vecHUD[2], vecHUD[3],
            m_DrawColor.r(), m_DrawColor.g(), m_DrawColor.b(), m_DrawColor.a() * alpha_multi);
    }

    virtual void GetSize(int& wide, int& tall) override {
        GetContentSize(wide, tall);
    }

    virtual void SetSize(int wide, int tall) override {
        m_iWide = wide;
        m_iTall = tall;
    }

    virtual void SetPos(int x, int y) override {
        m_iX = x;
        m_iY = y;
    }

    virtual void SetOffset(int x, int y) {
        m_iOffX = x;
        m_iOffY = y;
    }

    virtual void GetContentSize(int& wide, int& tall) override {
        wide = m_iWide;
        tall = m_iTall;
    }

    virtual void SetColor(Color col) override {
        m_DrawColor = col;
    }

    void SetContainerSize(int w, int h) {
        m_iContainerWide = w;
        m_iContainerTall = h;
    }

    void SetHitFrom(const vec3_t org) {
        m_vecHitFrom.x = org[0];
		m_vecHitFrom.y = org[1];
		m_vecHitFrom.z = org[2];
    }

    int m_iTexture = 0;
private:
    int m_iX = 0, m_iY = 0;
    int m_iOffX = 0, m_iOffY = 0;
    int m_iWide = 0, m_iTall = 0;
	int m_iContainerWide = 0, m_iContainerTall = 0;
    Color m_DrawColor = Color(255, 255, 255, 255);
    Vector m_vecHitFrom{};
};

class IndicatorImagePanel : public ImagePanel {
public:
    using ImagePanel::ImagePanel;
    virtual void ApplySettings(KeyValues* inResourceData) override
    {
        ImagePanel::ApplySettings(inResourceData);
        m_iOffset = inResourceData->GetInt("offset", 0);
    }
    void SetImage(IndicatorImage* image) {
        ImagePanel::SetImage(image);
        image->SetOffset(0, m_iOffset);
    }
private:
    int m_iOffset = 0;
};

extern vgui::HScheme GetViewPortBaseScheme();
CIndicatorPanel::CIndicatorPanel()
	: BaseClass(nullptr, VIEWPORT_INDICATOR_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());

    m_aryImagePanels = {
        new IndicatorImagePanel(this, "IndicatorImage1"),
        new IndicatorImagePanel(this, "IndicatorImage2"),
        new IndicatorImagePanel(this, "IndicatorImage3"),
        new IndicatorImagePanel(this, "IndicatorImage4")
    };
    m_iTex = surface()->CreateNewTextureID();

	LoadControlSettings(VGUI2_ROOT_DIR "IndicatorPanel.res");

	m_pCvarIndicatorStyle = CREATE_CVAR(CVAR_STYLE_NAME, "2", FCVAR_VALUE, nullptr);
    m_pDamageScreenFilter = CREATE_CVAR("hud_damageshock", "1", FCVAR_VALUE, nullptr);
    m_pDamageScreenFactor = CREATE_CVAR("hud_damageshock_factor", "0.015", FCVAR_VALUE, nullptr);
    m_pDamageScreenBase = CREATE_CVAR("hud_damageshock_base", "15", FCVAR_VALUE, nullptr);
}

CIndicatorPanel::~CIndicatorPanel()
{
    if (m_iTex)
        surface()->DeleteTextureByID(m_iTex);
}

void CIndicatorPanel::PerformLayout()
{
    BaseClass::PerformLayout();
    for (auto& panel : m_aryImagePanels) {
        int w, h;
        panel->GetSize(w, h);
		auto img = reinterpret_cast<IndicatorImage*>(panel->GetImage());
        img->SetContainerSize(w, h);
    }
}

void CIndicatorPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
    auto color = GetSchemeColor("Indicator.FgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
    SetFgColor(color);
	SetBgColor(GetSchemeColor("Indicator.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
    for (auto& panel : m_aryImagePanels) {
        panel->SetDrawColor(color);
    }
}
void CIndicatorPanel::ApplySettings(KeyValues* inResourceData)
{
    BaseClass::ApplySettings(inResourceData);
    auto texture = inResourceData->GetString("image", "");
    vgui::surface()->DrawSetTextureFile(m_iTex, texture, true, false);
    for (auto& panel : m_aryImagePanels) {
        auto img = new IndicatorImage();
        img->m_iTexture = m_iTex;
        panel->SetImage(img);
    }
    m_flFadeTime = inResourceData->GetFloat("fade_time", 1.0f);
    m_flKeepTime = inResourceData->GetFloat("keep_time", 2.0f);
}

const char* CIndicatorPanel::GetName() {
	return VIEWPORT_INDICATOR_NAME;
}
void CIndicatorPanel::Reset() {
	if (IsVisible())
		ShowPanel(false);
	memset(m_vecHitFrom, 0, sizeof(m_vecHitFrom));
	m_iDamage = 0;
	m_iArmor = 0;
}
void CIndicatorPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CIndicatorPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CIndicatorPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CIndicatorPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

void CIndicatorPanel::SetHitIndicator(int damage, int armor, const float vecFrom[3]){
    m_iDamage = damage;
    m_iArmor = armor;
    auto panel = m_aryImagePanels[m_iIndex];
    auto img = reinterpret_cast<IndicatorImage*>(panel->GetImage());
    if (panel && img) {
        img->SetHitFrom(vecFrom);
        if (!panel->IsVisible())
            panel->SetVisible(true);
        vgui::GetAnimationController()->CancelAnimationsForPanel(panel);
		panel->SetAlpha(255);
        vgui::GetAnimationController()->RunAnimationCommand(panel, "alpha", 0, m_flKeepTime, m_flFadeTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
    }
    m_iIndex++;
    if (m_iIndex >= m_aryImagePanels.size())
        m_iIndex = 0;
}
