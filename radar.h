#pragma once
class CHudRadar
{
public:
	void GLInit();
	int Init();
	void Reset();
	void Draw(float flTime);
	void DrawRadarTexture();
	void PreRenderView(int a1);
	void Clear();
private:
	cvar_t* pCVarDevOverview;
	cvar_t* pCVarDrawEntities;
	GLuint m_hRadarBufferFBO;
	GLuint m_hRadarBufferTex;

	vec3_t m_oldViewOrg;
	vec3_t m_oldViewAng;
	GLint m_oldFrameBuffer;
};
extern CHudRadar m_HudRadar;
