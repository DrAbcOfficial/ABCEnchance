#pragma once
class CWeaponMenuSlot : public IWeaponSelect {
public:
	void Init();
	void VidInit();
	int DrawWList(float flTime);
	void Reset();
	void Select();
private:
	int DrawBar(int x, int y, int width, int height, float f);
	void DrawAmmoBar(WEAPON* p, int x, int y, int width, int height);

	float SelectXOffset;
	float SelectYOffset;
	float SelectXGap;
	float SelectYGap;
	int SelectBucketHeight;
	int SelectBucketWidth;
	int SelectABHeight;
	int SelectABWidth;

	int iBucket0Spr;
	int iSelectionSpr;
	wrect_t* pRcSelection;
	HSPRITE pBucketSpr; // Sprite for top row of weapons menu
};
extern CWeaponMenuSlot m_HudWMenuSlot;