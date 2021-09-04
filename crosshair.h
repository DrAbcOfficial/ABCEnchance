#pragma once
class CHudCustomCrosshair : public CHudBase
{
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
private:
	cvar_t* pCvarDefaultFOV = NULL;
};
extern CHudCustomCrosshair m_HudCrosshair;