#pragma once
class CHudDelegate
{
public:
	virtual void HUD_Init(void);
	virtual void HUD_Draw(float flTime);
	virtual void HUD_Reset(void);
};
extern CHudDelegate* gHudDelegate;
