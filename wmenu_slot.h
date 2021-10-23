#pragma once
class CWeaponMenuSlot : public IWeaponSelect {
public:
	void Init();
	void VidInit();
	int DrawWList(float flTime);
	void ClientMove(struct playermove_s* ppmove, int server);
	void IN_Accumulate();
	void Reset();
	void Select();
private:
	int DrawBar(int x, int y, int width, int height, float f);
	void DrawAmmoBar(WEAPON* p, int x, int y, int width, int height);
	int m_HUD_bucket0;
	int m_HUD_selection;
	wrect_t* m_HUD_rcSelection;
	int giBucketHeight, giBucketWidth, giABHeight, giABWidth; // Ammo Bar width and height
	HSPRITE ghsprBuckets; // Sprite for top row of weapons menu
};
extern CWeaponMenuSlot m_HudWMenuSlot;