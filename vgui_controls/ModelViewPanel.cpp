#include <metahook.h>

#include <algorithm>
#include <vector>
#include "IMetaRenderer.h"
#include "com_model.h"
#include "studio.h"

#include "core/library/mymathlib.h"
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
		m_pModelEntity->angles[YAW] = yaw;
		m_pModelEntity->angles[ROLL] = roll;
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
		yaw = m_pModelEntity->angles[YAW];
		roll = m_pModelEntity->angles[ROLL];
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
	m_iAmbientLight = light;
}

void vgui::ModelViewPanel::SetShadeLight(int light){
	m_iShadeLight = light;
}

void vgui::ModelViewPanel::SetLightColor(int r, int g, int b){
	m_iLightColor[0] = r;
	m_iLightColor[1] = g;
	m_iLightColor[2] = b;
}

void vgui::ModelViewPanel::SetLightOrigin(float x, float y, float z){
	m_flLightOrigin[0] = x;
	m_flLightOrigin[1] = y;
	m_flLightOrigin[2] = z;
}

//-----------------------------------------------------------------------------
// Purpose: draws the graph
//-----------------------------------------------------------------------------
static void BuildModelMatrix(float out[4][4], const vec3_t origin, const vec3_t angles)
{
	CMathlib::Matrix4x4_CreateFromEntity(out, angles, origin, 1.0f);
}

static void BuildViewMatrix(float out[4][4], const vec3_t viewOrigin, const vec3_t viewAngles)
{
	float viewMatrix[4][4];
	float invViewMatrix[4][4];
	CMathlib::Matrix4x4_CreateFromEntity(viewMatrix, viewAngles, viewOrigin, 1.0f);
	CMathlib::InvertMatrix((float*)viewMatrix, (float*)out);
}

static void BuildProjMatrix(float out[4][4], float fov, float aspect, float zNear, float zFar)
{
	memset(out, 0, sizeof(float) * 16);
	float f = 1.0f / tanf(fov * 0.5f * (mathlib::Q_PI / 180.0f));
	out[0][0] = f / aspect;
	out[1][1] = f;
	out[2][2] = (zFar + zNear) / (zNear - zFar);
	out[2][3] = (2.0f * zFar * zNear) / (zNear - zFar);
	out[3][2] = -1.0f;
}

static void TransformVec3ByMat4(vec3_t out, const vec3_t v, const float m[4][4])
{
	float x = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + m[3][0];
	float y = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + m[3][1];
	float z = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + m[3][2];
	float w = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + m[3][3];
	out[0] = x;
	out[1] = y;
	out[2] = z;
	if (w != 0.0f) {
		out[0] /= w;
		out[1] /= w;
		out[2] /= w;
	}
}

void ModelViewPanel::Paint(){
	IMetaRenderer* pRenderer = MetaRenderer();
	if (!pRenderer || !m_pModelEntity || !m_pModelEntity->model)
		return;
	if (m_ModelFBO.iWidth <= 0 || m_ModelFBO.iHeight <= 0)
		return;

	auto* studiohdr = (studiohdr_t*)gEngineStudio.Mod_Extradata(m_pModelEntity->model);
	if (!studiohdr || studiohdr->id != 0x54534449)
		return;

	static bool s_bDebugOnce = true;

	float aspect = (float)m_ModelFBO.iWidth / (float)m_ModelFBO.iHeight;

	float dist = 120.0f;
	float camYaw = CMathlib::Q_DEG2RAD(m_pModelEntity->angles[YAW]);
	float camPitch = CMathlib::Q_DEG2RAD(-15.0f);

	vec3_t viewOrigin;
	viewOrigin[0] = m_pModelEntity->origin[0] + dist * cosf(camPitch) * sinf(camYaw);
	viewOrigin[1] = m_pModelEntity->origin[1] - dist * cosf(camPitch) * cosf(camYaw);
	viewOrigin[2] = m_pModelEntity->origin[2] + dist * sinf(camPitch);

	vec3_t viewTarget;
	CMathlib::VectorSubtract(m_pModelEntity->origin, viewOrigin, viewTarget);
	vec3_t viewAngles;
	CMathlib::VectorAngles(viewTarget, viewAngles);

	float modelMatrix[4][4];
	CMathlib::Matrix4x4_CreateFromEntity(modelMatrix, m_pModelEntity->origin,
		mathlib::vecZero, 1.0f);

	auto ptexture = (mstudiotexture_t*)((byte*)studiohdr + studiohdr->textureindex);
	auto pskinref = (short*)((byte*)studiohdr + studiohdr->skinindex);

	float**** pbonetransform = gEngineStudio.StudioGetBoneTransform ?
		gEngineStudio.StudioGetBoneTransform() : nullptr;

	if (s_bDebugOnce)
	{
		gEngfuncs.Con_Printf("[ModelView] FBO: %dx%d  FOV: %.1f\n", m_ModelFBO.iWidth, m_ModelFBO.iHeight, m_flFov);
		gEngfuncs.Con_Printf("[ModelView] Model: %s  bodyparts:%d  textures:%d  bones:%d\n",
			m_pModelEntity->model->name, studiohdr->numbodyparts, studiohdr->numtextures, studiohdr->numbones);
		gEngfuncs.Con_Printf("[ModelView] Origin: (%.1f,%.1f,%.1f)  Angles: (%.1f,%.1f,%.1f)\n",
			m_pModelEntity->origin[0], m_pModelEntity->origin[1], m_pModelEntity->origin[2],
			m_pModelEntity->angles[0], m_pModelEntity->angles[1], m_pModelEntity->angles[2]);
		gEngfuncs.Con_Printf("[ModelView] CamPos: (%.1f,%.1f,%.1f)  CamAng: (%.1f,%.1f,%.1f)\n",
			viewOrigin[0], viewOrigin[1], viewOrigin[2], viewAngles[0], viewAngles[1], viewAngles[2]);
		gEngfuncs.Con_Printf("[ModelView] Bonetransform: %s\n", pbonetransform ? "VALID" : "NULL");
	}

	float viewMatrix[4][4];
	BuildViewMatrix(viewMatrix, viewOrigin, viewAngles);

	float projMatrix[4][4];
	BuildProjMatrix(projMatrix, m_flFov, aspect, 1.0f, 4096.0f);

	float mvp[4][4];
	CMathlib::Matrix4x4_ConcatTransforms(mvp, viewMatrix, modelMatrix);
	float mvpFinal[4][4];
	CMathlib::Matrix4x4_ConcatTransforms(mvpFinal, projMatrix, mvp);

	pRenderer->BeginDebugGroup("ModelViewPanel::Paint");

	auto* oldSceneFBO = pRenderer->GetCurrentSceneFBO();
	auto* oldRenderingFBO = pRenderer->GetCurrentRenderingFBO();
	int oldW = 0, oldH = 0;
	if (oldRenderingFBO) {
		oldW = oldRenderingFBO->iWidth;
		oldH = oldRenderingFBO->iHeight;
	}

	pRenderer->BindFrameBuffer(&m_ModelFBO);
	pRenderer->SetCurrentSceneFBO(&m_ModelFBO);
	pRenderer->SetViewport(0, 0, m_ModelFBO.iWidth, m_ModelFBO.iHeight);

	float clearColor[4] = {0.15f, 0.15f, 0.15f, 1.0f};
	pRenderer->ClearColor(clearColor);

	int totalTris = 0;
	bool bAnyDraw = false;

	for (int i = 0; i < studiohdr->numbodyparts; i++)
	{
		auto bodypart = (mstudiobodyparts_t*)((byte*)studiohdr + studiohdr->bodypartindex) + i;
		if (!bodypart->modelindex || !bodypart->nummodels)
			continue;

		for (int j = 0; j < bodypart->nummodels; j++)
		{
			auto submodel = (mstudiomodel_t*)((byte*)studiohdr + bodypart->modelindex) + j;

			auto pverts = (const vec3_t*)((byte*)studiohdr + submodel->vertindex);
			auto pnorms = (const vec3_t*)((byte*)studiohdr + submodel->normindex);
			auto pvertbone = ((byte*)studiohdr + submodel->vertinfoindex);

			for (int k = 0; k < submodel->nummesh; k++)
			{
				auto pmesh = (mstudiomesh_t*)((byte*)studiohdr + submodel->meshindex) + k;

				int texIndex = pskinref[pmesh->skinref];
				int texid = ptexture[texIndex].index;
				if (texid <= 0)
					continue;

				float texW = (float)ptexture[texIndex].width;
				float texH = (float)ptexture[texIndex].height;
				if (texW <= 0.0f) texW = 1.0f;
				if (texH <= 0.0f) texH = 1.0f;

				auto ptricmds = (short*)((byte*)studiohdr + pmesh->triindex);

				std::vector<texturedrectvertex_t> verts;
				std::vector<uint32_t> indices;

				while (int trisLeft = *ptricmds++)
				{
					if (trisLeft < 0)
					{
						trisLeft = -trisLeft;
						for (int t = 0; t < trisLeft; t++)
						{
							auto idx = (uint32_t)verts.size();
							verts.push_back({});

							verts.back().texcoord[0] = (float)ptricmds[2] / texW;
							verts.back().texcoord[1] = (float)ptricmds[3] / texH;
							verts.back().col[0] = 1.0f;
							verts.back().col[1] = 1.0f;
							verts.back().col[2] = 1.0f;
							verts.back().col[3] = 1.0f;

							vec3_t bonedPos;
							int bone = pvertbone[ptricmds[0]];
							if (pbonetransform && (*pbonetransform) && (*pbonetransform)[bone])
							{
								auto boneMat = (*pbonetransform)[bone];
								bonedPos[0] = pverts[ptricmds[0]][0] * boneMat[0][0] + pverts[ptricmds[0]][1] * boneMat[0][1] + pverts[ptricmds[0]][2] * boneMat[0][2] + boneMat[0][3];
								bonedPos[1] = pverts[ptricmds[0]][0] * boneMat[1][0] + pverts[ptricmds[0]][1] * boneMat[1][1] + pverts[ptricmds[0]][2] * boneMat[1][2] + boneMat[1][3];
								bonedPos[2] = pverts[ptricmds[0]][0] * boneMat[2][0] + pverts[ptricmds[0]][1] * boneMat[2][1] + pverts[ptricmds[0]][2] * boneMat[2][2] + boneMat[2][3];
							}
							else
							{
								CMathlib::VectorCopy(pverts[ptricmds[0]], bonedPos);
							}

							vec3_t worldPos;
							TransformVec3ByMat4(worldPos, bonedPos, mvpFinal);

							float hw = (float)m_ModelFBO.iWidth * 0.5f;
							float hh = (float)m_ModelFBO.iHeight * 0.5f;
							verts.back().pos[0] = (worldPos[0] + 1.0f) * hw;
							verts.back().pos[1] = (1.0f - worldPos[1]) * hh;

							ptricmds += 4;

							if (t >= 2)
							{
								indices.push_back(idx - 2);
								indices.push_back(idx - 1);
								indices.push_back(idx);
							}
						}
					}
					else
					{
						for (int t = 0; t < trisLeft; t++)
						{
							auto idx = (uint32_t)verts.size();
							verts.push_back({});

							verts.back().texcoord[0] = (float)ptricmds[2] / texW;
							verts.back().texcoord[1] = (float)ptricmds[3] / texH;
							verts.back().col[0] = 1.0f;
							verts.back().col[1] = 1.0f;
							verts.back().col[2] = 1.0f;
							verts.back().col[3] = 1.0f;

							vec3_t bonedPos;
							int bone = pvertbone[ptricmds[0]];
							if (pbonetransform && (*pbonetransform) && (*pbonetransform)[bone])
							{
								auto boneMat = (*pbonetransform)[bone];
								bonedPos[0] = pverts[ptricmds[0]][0] * boneMat[0][0] + pverts[ptricmds[0]][1] * boneMat[0][1] + pverts[ptricmds[0]][2] * boneMat[0][2] + boneMat[0][3];
								bonedPos[1] = pverts[ptricmds[0]][0] * boneMat[1][0] + pverts[ptricmds[0]][1] * boneMat[1][1] + pverts[ptricmds[0]][2] * boneMat[1][2] + boneMat[1][3];
								bonedPos[2] = pverts[ptricmds[0]][0] * boneMat[2][0] + pverts[ptricmds[0]][1] * boneMat[2][1] + pverts[ptricmds[0]][2] * boneMat[2][2] + boneMat[2][3];
							}
							else
							{
								CMathlib::VectorCopy(pverts[ptricmds[0]], bonedPos);
							}

							vec3_t worldPos;
							TransformVec3ByMat4(worldPos, bonedPos, mvpFinal);

							float hw = (float)m_ModelFBO.iWidth * 0.5f;
							float hh = (float)m_ModelFBO.iHeight * 0.5f;
							verts.back().pos[0] = (worldPos[0] + 1.0f) * hw;
							verts.back().pos[1] = (1.0f - worldPos[1]) * hh;

							ptricmds += 4;

							if (t >= 2)
							{
								uint32_t a = idx - 2, b = idx - 1, c = idx;
								if ((t - 2) % 2 == 0)
								{
									indices.push_back(a);
									indices.push_back(b);
								}
								else
								{
									indices.push_back(b);
									indices.push_back(a);
								}
								indices.push_back(c);
							}
						}
					}
					if (ptricmds[0] == 0)
						break;
				}

				if (!verts.empty() && !indices.empty())
				{
					totalTris += (int)indices.size() / 3;
					bAnyDraw = true;
					pRenderer->DrawTexturedRect(texid, verts.data(), verts.size(),
						indices.data(), indices.size(),
						DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED,
						"ModelViewPanel");
				}
			}
		}
	}

	if (s_bDebugOnce)
	{
		s_bDebugOnce = false;
		gEngfuncs.Con_Printf("[ModelView] Model Matrix:\n");
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", modelMatrix[0][0],modelMatrix[0][1],modelMatrix[0][2],modelMatrix[0][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", modelMatrix[1][0],modelMatrix[1][1],modelMatrix[1][2],modelMatrix[1][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", modelMatrix[2][0],modelMatrix[2][1],modelMatrix[2][2],modelMatrix[2][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", modelMatrix[3][0],modelMatrix[3][1],modelMatrix[3][2],modelMatrix[3][3]);
		gEngfuncs.Con_Printf("[ModelView] Proj Matrix:\n");
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", projMatrix[0][0],projMatrix[0][1],projMatrix[0][2],projMatrix[0][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", projMatrix[1][0],projMatrix[1][1],projMatrix[1][2],projMatrix[1][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", projMatrix[2][0],projMatrix[2][1],projMatrix[2][2],projMatrix[2][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", projMatrix[3][0],projMatrix[3][1],projMatrix[3][2],projMatrix[3][3]);
		gEngfuncs.Con_Printf("[ModelView] MVP Final:\n");
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", mvpFinal[0][0],mvpFinal[0][1],mvpFinal[0][2],mvpFinal[0][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", mvpFinal[1][0],mvpFinal[1][1],mvpFinal[1][2],mvpFinal[1][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", mvpFinal[2][0],mvpFinal[2][1],mvpFinal[2][2],mvpFinal[2][3]);
		gEngfuncs.Con_Printf("[ModelView]   [%.2f %.2f %.2f %.2f]\n", mvpFinal[3][0],mvpFinal[3][1],mvpFinal[3][2],mvpFinal[3][3]);
		gEngfuncs.Con_Printf("[ModelView] DrawCalls: %s  TotalTris: %d\n",
			bAnyDraw ? "YES" : "NO", totalTris);
		gEngfuncs.Con_Printf("[ModelView] FBO tex id: %d\n", m_ModelFBO.s_hBackBufferTex);
		for (int t = 0; t < studiohdr->numtextures && t < 4; t++)
		{
			gEngfuncs.Con_Printf("[ModelView]   Tex[%d]: %s id=%d %dx%d\n",
				t, ptexture[t].name, ptexture[t].index,
				ptexture[t].width, ptexture[t].height);
		}
		vec3_t testModelPos = {0, 30, -15};
		vec3_t testScreen;
		TransformVec3ByMat4(testScreen, testModelPos, mvpFinal);
		gEngfuncs.Con_Printf("[ModelView] Model(0,30,-15) -> clip: (%.2f,%.2f,%.2f)\n",
			testScreen[0], testScreen[1], testScreen[2]);
		float hw = (float)m_ModelFBO.iWidth * 0.5f;
		float hh = (float)m_ModelFBO.iHeight * 0.5f;
		gEngfuncs.Con_Printf("[ModelView] Model -> screen: (%.0f,%.0f) [FBO: %dx%d]\n",
			(testScreen[0] + 1.0f) * hw, (1.0f - testScreen[1]) * hh,
			m_ModelFBO.iWidth, m_ModelFBO.iHeight);
	}

	pRenderer->DrawFilledQuad(0, 0, m_ModelFBO.iWidth, m_ModelFBO.iHeight,
		clearColor, 0, "debug_bg");

	{
		std::vector<filledrectvertex_t> debugVerts(3);
		std::vector<uint32_t> debugIdx = {0, 1, 2};

		vec3_t p0 = {0, 0, 50};
		vec3_t p1 = {50, 0, 0};
		vec3_t p2 = {0, 50, 0};
		vec3_t s0, s1, s2;
		TransformVec3ByMat4(s0, p0, mvpFinal);
		TransformVec3ByMat4(s1, p1, mvpFinal);
		TransformVec3ByMat4(s2, p2, mvpFinal);

		float hw = (float)m_ModelFBO.iWidth * 0.5f;
		float hh = (float)m_ModelFBO.iHeight * 0.5f;
		debugVerts[0].pos[0] = (s0[0] + 1.0f) * hw; debugVerts[0].pos[1] = (1.0f - s0[1]) * hh;
		debugVerts[1].pos[0] = (s1[0] + 1.0f) * hw; debugVerts[1].pos[1] = (1.0f - s1[1]) * hh;
		debugVerts[2].pos[0] = (s2[0] + 1.0f) * hw; debugVerts[2].pos[1] = (1.0f - s2[1]) * hh;
		debugVerts[0].col[0] = debugVerts[0].col[1] = 1.0f; debugVerts[0].col[2] = 0.0f; debugVerts[0].col[3] = 1.0f;
		debugVerts[1].col[0] = debugVerts[1].col[1] = 1.0f; debugVerts[1].col[2] = 0.0f; debugVerts[1].col[3] = 1.0f;
		debugVerts[2].col[0] = debugVerts[2].col[1] = 1.0f; debugVerts[2].col[2] = 0.0f; debugVerts[2].col[3] = 1.0f;

		pRenderer->DrawFilledRect(debugVerts.data(), 3, debugIdx.data(), 3, 0, "debug_tri");
	}

	pRenderer->SetCurrentSceneFBO(oldSceneFBO);
	pRenderer->BindFrameBuffer(oldRenderingFBO);
	if (oldRenderingFBO) {
		pRenderer->SetViewport(0, 0, oldW, oldH);
	}

	surface()->DrawSetTexture(m_ModelFBO.s_hBackBufferTex);
	surface()->DrawFilledRect(0, 0, m_ModelFBO.iWidth, m_ModelFBO.iHeight);

	surface()->DrawSetColor(255, 0, 0, 255);
	surface()->DrawFilledRect(0, 0, 10, 10);
	surface()->DrawSetColor(0, 255, 0, 255);
	surface()->DrawFilledRect(10, 0, 20, 10);

	pRenderer->EndDebugGroup();
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
