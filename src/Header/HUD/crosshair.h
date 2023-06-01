#pragma once
class CHudCustomCrosshair{
public:
	int Init(void);
	int Draw(float flTime);
private:
	cvar_t* pCvarDefaultCrosshair = NULL;
	void DrawCrosshairSPR(int x, int y, int hPic, wrect_t hRc);
	void DrawDefaultCrosshair(float flTime, int x, int y);
};
extern CHudCustomCrosshair m_HudCrosshair;