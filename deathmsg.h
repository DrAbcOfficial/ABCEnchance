#pragma once
class CHudDeathMsg : public CHudBase
{
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
};
extern CHudDeathMsg m_HudDeathMsg;