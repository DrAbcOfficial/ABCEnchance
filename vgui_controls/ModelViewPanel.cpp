#include <metahook.h>

#include <algorithm>
#include "IMetaRenderer.h"

#include "exportfuncs.h"
#include "utility/vgui_util.h"

#include "ModelViewPanel.h"

#undef clamp

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
ModelViewPanel::ModelViewPanel(Panel *parent, const char *name) : BaseClass(parent, name){
	m_pModelEntity = new cl_entity_t();
	m_pModelEntity->baseline.entityType = ENTITY_NORMAL;
	//m_pModelEntity->player = TRUE;
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
		//MetaRenderer()->BeginDebugGroup("ModelViewPanel::Paint");
		//auto oldFbo = MetaRenderer()->GetCurrentRenderingFBO();
		//int oldW = oldFbo->iWidth;
		//int oldH = oldFbo->iHeight;
		//
		//MetaRenderer()->BindFrameBuffer(&m_ModelFBO);
		//MetaRenderer()->SetCurrentSceneFBO(&m_ModelFBO);
		//
		//MetaRenderer()->SetViewport(0, 0, m_ModelFBO.iWidth, m_ModelFBO.iHeight);
		//MetaRenderer()->SetCurrentEntity(m_pModelEntity);
		//MetaRenderer()->DrawCurrentEntity(false);
		//MetaRenderer()->SetViewport(0, 0, oldW, oldH);
		//
		//MetaRenderer()->SetCurrentSceneFBO(oldFbo);
		//MetaRenderer()->BindFrameBuffer(oldFbo);
		//
		//MetaRenderer()->EndDebugGroup();
		//
		//surface()->DrawSetTexture(m_ModelFBO.s_hBackBufferTex);
		//surface()->DrawFilledRect(0, 0, m_ModelFBO.iWidth, m_ModelFBO.iHeight);
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
