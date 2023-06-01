#pragma once
class WEAPON;
typedef int HSPRITE;

class CWeaponMenuSlot {
public:
	void VidInit();
	int DrawWList(float flTime);
	bool IsVisible();
	void Select();
	void Reset();

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
private:
	int DrawBar(int x, int y, int width, int height, float f);
	void DrawAmmoBar(WEAPON* p, int x, int y, int width, int height);

	size_t SelectXOffset;
	size_t SelectYOffset;
	size_t SelectXGap;
	size_t SelectYGap;
	size_t SelectBucketHeight;
	size_t SelectBucketWidth;
	size_t SelectABHeight;
	size_t SelectABWidth;

	float m_fAnimateTime = 0;

	Color SelectColor;
	Color SelectRinColor;
	Color SelectIconColor;
	Color SelectTextColor;
	Color SelectEmptyColor;
	Color SelectPointerColor;

	vgui::HFont HUDFont = 0;
	vgui::HFont HUDSmallFont = 0;

	int iBucket0Spr;
	int iSelectionSpr;
	wrect_t* pRcSelection;
	HSPRITE pBucketSpr; // Sprite for top row of weapons menu
};
extern CWeaponMenuSlot m_HudWMenuSlot;