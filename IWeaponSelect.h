#pragma once
class IWeaponSelect {
public:
	virtual void Init() = 0;
	virtual int DrawWList(float flTime) = 0;
	virtual void ClientMove(struct playermove_s* ppmove, int server) = 0;
	virtual void IN_Accumulate() = 0;
	virtual void Reset() = 0;
	virtual void Select() = 0;

	float m_fFade;
	bool m_bOpeningMenu = false;
	bool m_bSelectMenuDisplay = false;
	bool m_bSetedCursor = false;

	float SelectOffset = 0;
	float SelectSize = 0;
	float SelectRotate = 0;
	float SelectAnimateTime = 0;
	float SelectFadeTime = 0;
	float SelectPointerSize = 0;
	float SelectHoldTime = 0;
protected:
	float m_fAnimateTime = 0;
	float m_fCursorAngle = 0;

	Color SelectColor;
	Color SelectRinColor;
	Color SelectIconColor;
	Color SelectTextColor;
	Color SelectEmptyColor;
	Color SelectPointerColor;

	vgui::HFont HUDFont = 0;
	vgui::HFont HUDSmallFont = 0;
};