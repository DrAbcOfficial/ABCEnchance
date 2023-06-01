#pragma once
class CHudPlayerTitle {
public:
	int Init(void);
	int Draw(float flTime);
	void Reset();
private:
	GLint iDeathIconTga;
	GLint iCrouchIconTga;
	GLint iMedkitIconTga;
	GLint iBackGroundTga;
	GLint iHealthBarTga;
	GLint iArmorBarTga;
};
extern CHudPlayerTitle m_HudPlayerTitle;