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

	std::wstring MessagePrefix;
	std::wstring MessagePostfix;
	std::wstring DifferMessagePrefix;
	std::wstring DifferMessagePostfix;

	vgui::HFont HUDSmallFont;

	size_t MoneyBackGround;
	size_t YOffset;
	size_t BackgroundLength;
	size_t BackgroundHeight;
	Color MoneyColor;
	Color MoneyDecreseColor;
	Color MoneyIncreseColor;
};
extern CHudEccoMoney m_HudEccoMoney;