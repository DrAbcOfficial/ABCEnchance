#pragma once
class CHudCustomAmmo{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	//是否正在绘制ammo
	bool IsVisible();
	bool ShouldDraw();
	//阻挡一次+attack
	bool BlockAttackOnce();
	void Select();
	void Reset(void);
	void SlotInput(int iSlot, int fAdvance, bool bWheel = false);
	void ChosePlayerWeapon(void);
	void Clear();

	bool m_bIsOnTarget = false;
	bool m_bSelectBlock = false;

	WEAPON* m_pWeapon = nullptr;

	bool m_bAcceptDeadMessage = false;
private:
	int DrawWList(float flTime);

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