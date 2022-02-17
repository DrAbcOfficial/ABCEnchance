#pragma once
enum WATERLEVEL {
	WATERLEVEL_DRY = 0,
	WATERLEVEL_FEET,
	WATERLEVEL_WAIST,
	WATERLEVEL_HEAD
};
class CHudEfx {
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
private:
	GLint DarkconerImg;
	GLint WaterLeftSpr;
	GLint WaterRightSpr;

	bool bDyingFlincDir = false;
	int iDyingFlinc;
	float flDyingFlincAdvanceTime;

	int iOldWaterType;
	size_t iFrameLeft;
	size_t iFrameRight;
	float flFrameLeftAdvanceTime;
	float flFrameRightAdvanceTime;
	float flWaterFaceDisapearTime;
};
extern CHudEfx m_HudEfx;