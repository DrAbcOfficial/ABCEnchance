#pragma once
class CHudRadar{
public:
	void GLInit();
	int Init();
	void VidInit();
	void Reset();
	void Draw(float flTime);
	void Clear();
	void BlitFramebuffer();

	GLint BackGroundImg;
	GLint UpGroundImg;
	GLint NorthImg;
	GLint ViewAngleImg;
	GLint RoundBackGroundImg;

	float flFinishScaleTime;
private:
	void UpdateZmax(float flTime);

	GLuint m_hRadarBufferFBO;
	GLuint m_hRadarBufferTex;

	pmtrace_t m_hRadarTr;
	float flNextUpdateTrTime;

	size_t XOffset = 0;
	size_t YOffset = 0;
	size_t OutLineAlpha = 0;
	size_t MapAlpha = 0;
	size_t CenterAlpha = 0;
	size_t NorthPointerSize = 0;
	size_t ViewAngleSize = 0;
	
	size_t iOverviewR = 0;
	size_t iOverviewG = 0;
	size_t iOverviewB = 0;

	vec3_t m_oldViewOrg;
	vec3_t m_oldViewAng;
	GLint m_oldFrameBuffer;
};
extern CHudRadar m_HudRadar;
