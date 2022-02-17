#pragma once
class IWeaponSelect {
public:
	virtual void Init() = 0;
	virtual int DrawWList(float flTime) = 0;
	virtual void Reset() = 0;
	virtual void Select() = 0;

	float m_fFade = 0;
	bool m_bSelectMenuDisplay = false;
	bool m_bSetedCursor = false;

	size_t SelectOffset = 0;
	size_t SelectSize = 0;
	size_t SelectRotate = 0;
	size_t SelectPointerSize = 0;
	float SelectAnimateTime = 0;
	float SelectFadeTime = 0;
	float SelectHoldTime = 0;
protected:
	float m_fAnimateTime = 0;

	Color SelectColor;
	Color SelectRinColor;
	Color SelectIconColor;
	Color SelectTextColor;
	Color SelectEmptyColor;
	Color SelectPointerColor;

	vgui::HFont HUDFont = 0;
	vgui::HFont HUDSmallFont = 0;
};