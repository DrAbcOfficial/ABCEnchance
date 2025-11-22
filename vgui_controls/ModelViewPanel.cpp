#include <metahook.h>

#include <algorithm>
#include "IMetaRenderer.h"

#include "exportfuncs.h"

#include "ModelViewPanel.h"

#undef clamp

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
ModelViewPanel::ModelViewPanel(Panel *parent, const char *name) : BaseClass(parent, name){
	m_pModelEntity = new cl_entity_t();
	m_pModelEntity->baseline.entityType = ENTITY_NORMAL;
	SetModelPos(0, 0, 0);
	SetModelRotate(0, 0, 0);
}

vgui::ModelViewPanel::~ModelViewPanel(){
	if (m_pModelEntity)
		delete m_pModelEntity;
	if (MetaRenderer())
	{
		MetaRenderer()->FreeFBO(&m_ModelFBO);
	}
}

void vgui::ModelViewPanel::LoadModel(const char* model){
	ChangeModel(model);
	SetSequnce(0);
	SetController(0, 0.0);
	SetController(1, 0.0);
	SetController(2, 0.0);
	SetController(3, 0.0);
	SetMouth(0);
	SetBlend(0, 0.0);
	SetBlend(0, 0.0);
}

void vgui::ModelViewPanel::ChangeModel(const char* model){
	if (!std::strcmp(m_szModel, model))
		return;
	std::strcpy(m_szModel, model);
	auto mdl = gEngineStudio.Mod_ForName(m_szModel, false);
	m_pModelEntity->model = mdl;
}


//     ^Z
//     |
//     |   /Y
//     |  /
//     |/
//     O---------->X
void vgui::ModelViewPanel::SetModelPos(float x, float y, float z){
	if (m_pModelEntity)
	{
		m_pModelEntity->origin[0] = x;
		m_pModelEntity->origin[1] = y;
		m_pModelEntity->origin[2] = z;
	}
}

/// <summary>
/// Rotate Model
/// </summary>
void vgui::ModelViewPanel::SetModelRotate(float pitch, float yaw, float roll){
	if (m_pModelEntity)
	{
		m_pModelEntity->angles[PITCH] = pitch;
		m_pModelEntity->origin[YAW] = yaw;
		m_pModelEntity->origin[ROLL] = roll;
	}
}

void vgui::ModelViewPanel::GetModelPos(float& x, float& y, float& z){
	if (m_pModelEntity)
	{
		x = m_pModelEntity->origin[0];
		y = m_pModelEntity->origin[1];
		z = m_pModelEntity->origin[2];
	}
}

void vgui::ModelViewPanel::GetModelRotate(float& pitch, float& yaw, float& roll){
	if (m_pModelEntity)
	{
		pitch = m_pModelEntity->angles[PITCH];
		yaw = m_pModelEntity->origin[YAW];
		roll = m_pModelEntity->origin[ROLL];
	}
}

float vgui::ModelViewPanel::GetFOV() const{
	return m_flFov;
}
void vgui::ModelViewPanel::SetFOV(float fov){
	m_flFov = fov;
}
bool vgui::ModelViewPanel::GetAnimate() const{
	return m_bAnimate;
}
void vgui::ModelViewPanel::SetAnimate(bool state){
	m_bAnimate = state;
}
int vgui::ModelViewPanel::GetFrame(){
	if (m_pModelEntity)
	{
		return m_pModelEntity->curstate.frame;
	}
	return 0;
}
void vgui::ModelViewPanel::SetFrame(int frame){
	if (m_pModelEntity)
	{
		m_pModelEntity->curstate.frame = frame;
	}
}
float vgui::ModelViewPanel::GetFrameRate(){
	if (m_pModelEntity)
	{
		return m_pModelEntity->curstate.framerate;
	}
	return 0;
}
void vgui::ModelViewPanel::SetFrameRate(float rate){
	if (m_pModelEntity)
	{
		m_pModelEntity->curstate.framerate = rate;
	}
}
int vgui::ModelViewPanel::GetSequence(){
	if (m_pModelEntity)
	{
		return m_pModelEntity->curstate.sequence;
	}
	return 0;
}
void vgui::ModelViewPanel::SetSequnce(int seq){
	if (m_pModelEntity)
	{
		m_pModelEntity->curstate.sequence = seq;
	}
}
int vgui::ModelViewPanel::GetSkin(){
	if (m_pModelEntity)
	{
		return m_pModelEntity->curstate.skin;
	}
	return 0;
}
void vgui::ModelViewPanel::SetSkin(int skin){
	if (m_pModelEntity)
	{
		m_pModelEntity->curstate.skin = skin;
	}
}
int vgui::ModelViewPanel::GetBodygroup(){
	if (m_pModelEntity)
	{
		return m_pModelEntity->curstate.body;
	}
	return 0;
}
void vgui::ModelViewPanel::SetBodygroup(int bodygroup){
	if (m_pModelEntity)
	{
		m_pModelEntity->curstate.body = bodygroup;
	}
}
float vgui::ModelViewPanel::GetBlend(size_t blend){
	if (blend >= 4)
		return 0.0f;
	if (m_pModelEntity)
	{
		return m_pModelEntity->curstate.blending[blend];
	}
	return 0;
}
void vgui::ModelViewPanel::SetBlend(size_t blend, float value){
	if (blend >= 4)
		return;
	if (m_pModelEntity)
	{
		m_pModelEntity->curstate.blending[blend] = value;
	}
}
float vgui::ModelViewPanel::GetController(size_t idx){
	if (idx >= 4)
		return 0;
	if(m_pModelEntity)
	{
		return m_pModelEntity->curstate.controller[idx];
	}
	return 0;
}

void vgui::ModelViewPanel::SetController(size_t idx, float value){
	if (idx >= 4)
		return;
	if (m_pModelEntity)
	{
		m_pModelEntity->curstate.controller[idx] = value;
	}
}

byte vgui::ModelViewPanel::GetMouth(){
	if (m_pModelEntity)
	{
		return m_pModelEntity->mouth.mouthopen;
	}
	return 0;
}

void vgui::ModelViewPanel::SetMouth(byte mouth){
	if (m_pModelEntity)
	{
		m_pModelEntity->mouth.mouthopen = mouth;
	}
}

void vgui::ModelViewPanel::SetAmbientLight(int light){

}

void vgui::ModelViewPanel::SetShadeLight(int light){

}

void vgui::ModelViewPanel::SetLightColor(int r, int g, int b){

}

void vgui::ModelViewPanel::SetLightOrigin(float x, float y, float z){

}

//-----------------------------------------------------------------------------
// Purpose: draws the graph
//-----------------------------------------------------------------------------
void ModelViewPanel::Paint(){
	if (MetaRenderer())
	{
		MetaRenderer()->BeginDebugGroup("ModelViewPanel::Paint");
		auto CurrentRenderingFBO = MetaRenderer()->GetCurrentRenderingFBO();
		MetaRenderer()->BindFrameBuffer(&m_ModelFBO);
		MetaRenderer()->SetCurrentSceneFBO(&m_ModelFBO);
		MetaRenderer()->PushProjectionMatrix();
		MetaRenderer()->PushWorldMatrix();

		int w, h;
		GetSize(w, h);
		//MetaRenderer()->SetViewport(0, 0, w, h);
		MetaRenderer()->SetCurrentEntity(m_pModelEntity);
		MetaRenderer()->DrawCurrentEntity(false);

		MetaRenderer()->PopProjectionMatrix();
		MetaRenderer()->PopWorldMatrix();
		MetaRenderer()->SetCurrentSceneFBO(CurrentRenderingFBO);
		MetaRenderer()->BindFrameBuffer(CurrentRenderingFBO);
		MetaRenderer()->EndDebugGroup();

		int absX{}, absY{};
		ipanel()->GetAbsPos(GetVPanel(), absX, absY);

		int screenW{}, screenH{};
		screenW = CurrentRenderingFBO->iWidth;
		screenH = CurrentRenderingFBO->iHeight;

		// 计算纹理坐标，只采样Panel所遮挡的区域
		float u0 = (float)absX / (float)screenW;
		float u1 = (float)(absX + w) / (float)screenW;
		// OpenGL纹理坐标原点在左下角，需要翻转v坐标
		float v0 = 1.0f - (float)absY / (float)screenH;
		float v1 = 1.0f - (float)(absY + h) / (float)screenH;

		Color BgColor = GetBgColor();
		vec4_t vColor4 = { BgColor.r() / 255.0f, BgColor.g() / 255.0f, BgColor.b() / 255.0f, BgColor.a() / 255.0f };

		// 使用DrawTexturedRect手动指定纹理坐标
		texturedrectvertex_t vertices[4];

		// 左下角 (屏幕坐标: 0, h)
		vertices[0].pos[0] = 0;
		vertices[0].pos[1] = h;
		vertices[0].texcoord[0] = u0;
		vertices[0].texcoord[1] = v1;  // 屏幕下方对应纹理的下方
		vertices[0].col[0] = vColor4[0];
		vertices[0].col[1] = vColor4[1];
		vertices[0].col[2] = vColor4[2];
		vertices[0].col[3] = vColor4[3];

		// 右下角 (屏幕坐标: w, h)
		vertices[1].pos[0] = w;
		vertices[1].pos[1] = h;
		vertices[1].texcoord[0] = u1;
		vertices[1].texcoord[1] = v1;  // 屏幕下方对应纹理的下方
		vertices[1].col[0] = vColor4[0];
		vertices[1].col[1] = vColor4[1];
		vertices[1].col[2] = vColor4[2];
		vertices[1].col[3] = vColor4[3];

		// 右上角 (屏幕坐标: w, 0)
		vertices[2].pos[0] = w;
		vertices[2].pos[1] = 0;
		vertices[2].texcoord[0] = u1;
		vertices[2].texcoord[1] = v0;  // 屏幕上方对应纹理的上方
		vertices[2].col[0] = vColor4[0];
		vertices[2].col[1] = vColor4[1];
		vertices[2].col[2] = vColor4[2];
		vertices[2].col[3] = vColor4[3];

		// 左上角 (屏幕坐标: 0, 0)
		vertices[3].pos[0] = 0;
		vertices[3].pos[1] = 0;
		vertices[3].texcoord[0] = u0;
		vertices[3].texcoord[1] = v0;  // 屏幕上方对应纹理的上方
		vertices[3].col[0] = vColor4[0];
		vertices[3].col[1] = vColor4[1];
		vertices[3].col[2] = vColor4[2];
		vertices[3].col[3] = vColor4[3];

		const uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		MetaRenderer()->DrawTexturedRect(m_ModelFBO.s_hBackBufferTex, vertices, 4, indices, 6, DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED, "ModelViewPanel::PaintBackground");
	}
}

void vgui::ModelViewPanel::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
	if (MetaRenderer())
	{
		int w, h;
		GetSize(w, h);
		m_ModelFBO.iWidth = w;
		m_ModelFBO.iHeight = h;
		MetaRenderer()->GenFrameBuffer(&m_ModelFBO, "ModelViewPanelFBO");
		MetaRenderer()->FrameBufferColorTexture(&m_ModelFBO, GL_RGBA8);
		MetaRenderer()->FrameBufferDepthTexture(&m_ModelFBO, GL_DEPTH24_STENCIL8);
	}

	LoadModel(inResourceData->GetString("model", ""));

	SetFOV(inResourceData->GetFloat("fov", 90));
	SetAnimate(inResourceData->GetBool("animate", false));
	SetFrame(inResourceData->GetInt("frame", 0));
	SetSequnce(inResourceData->GetInt("sequence", 0));
	SetSkin(inResourceData->GetInt("skin", 0));
	SetBodygroup(inResourceData->GetInt("bodygroup", 0));
	SetBlend(0, inResourceData->GetFloat("blend_0", 0));
	SetBlend(1, inResourceData->GetFloat("blend_1", 0));
	SetBlend(2, inResourceData->GetFloat("blend_2", 0));
	SetBlend(3, inResourceData->GetFloat("blend_3", 0));
	SetController(0, inResourceData->GetFloat("controller_0", 0));
	SetController(1, inResourceData->GetFloat("controller_1", 0));
	SetController(2, inResourceData->GetFloat("controller_2", 0));
	SetController(3, inResourceData->GetFloat("controller_3", 0));
	SetMouth(inResourceData->GetInt("mouth", 0));
	const char* buf = inResourceData->GetString("origin", "0 0 0");
	float a, b, c;
	int result = std::sscanf(buf, "%f %f %f", &a, &b, &c);
	if (result != EOF)
		SetModelPos(a, b, c);
	
	buf = inResourceData->GetString("rotate", "0 0 0");
	result = std::sscanf(buf, "%f %f %f", &a, &b, &c);
	if (result != EOF)
		SetModelRotate(a, b, c);

	int rr, gg, bb;
	buf = inResourceData->GetString("light_color", "255 255 255");
	result = std::sscanf(buf, "%d %d %d", &rr, &gg, &bb);
	if (result != EOF)
		SetLightColor(rr, gg, bb);

	SetAmbientLight(inResourceData->GetInt("ambient_light", 32));
	SetShadeLight(inResourceData->GetInt("shade_light", 192));

	buf = inResourceData->GetString("light_origin", "0 0 0");
	result = std::sscanf(buf, "%f %f %f", &a, &b, &c);
	if (result != EOF)
		SetLightOrigin(a, b, c);
}
