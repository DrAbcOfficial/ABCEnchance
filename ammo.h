#pragma once
#include "IWeaponSelect.h"
class CHudCustomAmmo{
public:
	void GLInit();
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	bool ShouldDraw();
	void Reset(void);
	void SlotInput(int iSlot, int fAdvance);
	void ChosePlayerWeapon(void);
	void ClientMove(struct playermove_s* ppmove, qboolean server);
	void IN_Accumulate();
	void Clear();

	bool m_bIsOnTarget = false;
	WEAPON* m_pWeapon = nullptr;
	IWeaponSelect* m_pNowSelectMenu = nullptr;

	bool m_bAcceptDeadMessage = false;
private:
	int DrawWList(float flTime);
	void SyncWeapon();
	float m_fNextSyncTime = 0;

	size_t ElementGap = 0;
	size_t BackGroundY = 0;
	size_t BackGroundLength = 0;

	GLint iBackGroundTga = 0;

	Color Ammo1IconColor;
	Color Ammo1BigTextColor;
	Color Ammo1TextColor;
	Color Ammo2IconColor;
	Color Ammo2BigTextColor;
	Color Ammo2TextColor;

	vgui::HFont HUDFont = 0;
	vgui::HFont HUDSmallFont = 0;
};
extern CHudCustomAmmo m_HudCustomAmmo;