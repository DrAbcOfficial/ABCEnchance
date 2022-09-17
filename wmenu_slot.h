#pragma once
class CWeaponMenuSlot : public IWeaponSelect {
public:
	void Init();
	void VidInit();
	int DrawWList(float flTime);
	bool IsVisible();
	void Select();
	void Reset();
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

	GLint iBucket0Spr;
	GLint iSelectionSpr;
	wrect_t* pRcSelection;
	HSPRITE pBucketSpr; // Sprite for top row of weapons menu
};
extern CWeaponMenuSlot m_HudWMenuSlot;