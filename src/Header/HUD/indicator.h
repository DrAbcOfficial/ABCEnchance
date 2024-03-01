#pragma once
#define NUM_DMG_TYPES 8
#define NUM_MAX_INDICATOR 4


class CHudIndicator{
public:
	void GLInit();
	void Init(void);
	int Draw(float flTime);
	void Reset(void);
	int VidInit(void);
	void Clear();
	int DrawPain(float fTime);

	typedef struct indicatorinfo_s {
		int iDamage;
		int iArmor;
		vec3_t vecFrom;
		float flKeepTime;
		vec2_t vecHUDA, vecHUDB, vecHUDC, vecHUDD;
	}indicatorinfo_t;
	indicatorinfo_t aryIndicators[NUM_MAX_INDICATOR];

	void CalcDamageDirection(indicatorinfo_s& var);
	void AddIdicator(int dmg, int armor, vec3_t vecFrom);

	
	int iNowSelectIndicator = 0;
	int iPainIndicator;

	float flPainColorKeepTime = 0;

	float PainColorTime = 0.5;
	float PainIndicatorTime = 0;
	float ShockIndicatorTime = 0;
private:
	void CalcuPainFade(int& r, int& g, int& b, Color* c, float timeDiffer);
	
	indicatorinfo_t m_hScreenFilter;

	int m_hOldBuffer;
	size_t m_hFilterFBO;
	size_t m_hFilterTex;

	Color PainIndicatorColor;
	Color PainIndicatorColorA;
};
extern CHudIndicator m_HudIndicator;