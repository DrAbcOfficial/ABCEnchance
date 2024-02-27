#pragma once
class WEAPON;
class CHudCustomAmmo{
public:
	int Init(void);
	int VidInit(void);
	//是否正在绘制ammo
	bool ShouldDraw();
	//阻挡一次+attack
	bool BlockAttackOnce();
	void Select();
	void Reset(void);
	void SlotInput(int iSlot, int fAdvance, bool bWheel = false);

	bool m_bSelectBlock = false;

	bool m_bAcceptDeadMessage = false;

	WEAPON* GetCurWeapon();
	void SetCurWeapon(WEAPON* wp);

	WEAPON* m_pWeapon;
};
extern CHudCustomAmmo m_HudCustomAmmo;