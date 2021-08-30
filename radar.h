#pragma once
class CHudRadar
{
public:
	void GLInit();
	int Init();
	void Reset();
	void Draw(float flTime);
	void DrawRadarTexture();
	void UpdateZmax(float flTime);
	void PreRenderView(int a1);
	void Clear();

	int OutLineImg;
	int PlayerPointImg;
	int NorthImg;

private:
	cvar_t* pCVarDevOverview;
	cvar_t* pCVarDrawEntities;
	cvar_t* pCVarDrawDynamic;

	GLuint m_hRadarBufferFBO;
	GLuint m_hRadarBufferTex;
	GLuint m_hRadarBufferTexDepth;

	pmtrace_t m_hRadarTr;
	float flNextUpdateTrTime;

	float XOffset;
	float YOffset;
	float OutLineAlpha;
	GLubyte MapAlpha;
	float CenterAlpha;
	float NorthPointerSize;

	vec3_t m_oldViewOrg;
	vec3_t m_oldViewAng;
	GLint m_oldFrameBuffer;
};
extern CHudRadar m_HudRadar;
