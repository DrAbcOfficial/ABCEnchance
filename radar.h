#pragma once
class CHudRadar{
public:
	void GLInit();
	int Init();
	void VidInit();
	void Reset();
	void Draw(float flTime);
	void PreRenderView(int a1);
	void Clear();

	int OutLineImg;
	int PlayerPointImg;
	int NorthImg;

	float flFinishScaleTime;
private:
	void DrawRadarTexture();
	void UpdateZmax(float flTime);

	cvar_t* pCVarDevOverview;
	cvar_t* pCVarDrawEntities;
	cvar_t* pCVarDrawDynamic;
	cvar_t* pCVarFXAA;
	cvar_t* pCVarWater;

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
	
	int iOverviewR;
	int iOverviewG;
	int iOverviewB;

	vec3_t m_oldViewOrg;
	vec3_t m_oldViewAng;
	GLint m_oldFrameBuffer;
};
extern CHudRadar m_HudRadar;
