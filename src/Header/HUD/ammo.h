#pragma once
class WEAPON;
class CHudCustomAmmo{
public:
	int Init(void);
	int VidInit(void);
	//�Ƿ����ڻ���ammo
	bool ShouldDraw();
	//�赲һ��+attack
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