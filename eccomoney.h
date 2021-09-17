#pragma once
class CHudEccoMoney {
public:
	int Init(void);
	int Draw(float flTime);
	void Reset();
private:
	float flStoreMoney = 0;
	float flDifferDisapearTime = 0;
	float flNextUpdateTime = 0;
	int iDifferMoney = 0;

	vgui::HFont HUDSmallFont;
	GLint MoneyBackGround;
	float YOffset;
	float BackgroundLength;
	float BackgroundHeight;
	Color MoneyColor;
	Color MoneyDecreseColor;
	Color MoneyIncreseColor;
};
extern CHudEccoMoney m_HudEccoMoney;