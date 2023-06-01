#pragma once
typedef struct {
	float fExpire;
	float fBaseline;
	int	x, y;
} DAMAGE_IMAGE;

class CHudBattery : public CHudBase{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	int MsgFunc_Battery(const char* pszName, int iSize, void* pbuf);

	HSPRITE m_hSprite1;
	HSPRITE m_hSprite2;
	wrect_t* m_prc1;
	wrect_t* m_prc2;
	int	  m_iBat;
	int	  m_iBatMax;
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};

#define FADE_TIME 100

class CHudHealth : public CHudBase{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float fTime);
	virtual void Reset(void);
	int MsgFunc_Health(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_Damage(const char* pszName, int iSize, void* pbuf);
	int m_iHealth;
	int m_iDMGIconStart;
	int m_HUD_cross;
	float m_fAttackFront, m_fAttackRear, m_fAttackLeft, m_fAttackRight;
	void GetPainColor(int& r, int& g, int& b);
	float m_fFade;
	HSPRITE m_hSprite;
	HSPRITE m_hDamage;

	DAMAGE_IMAGE m_dmg[12];
	int	m_bitsDamage;
	int DrawPain(float fTime);
	int DrawDamage(float fTime);
	void CalcDamageDirection(vec3_t vecFrom);
	void UpdateTiles(float fTime, long bits);
};