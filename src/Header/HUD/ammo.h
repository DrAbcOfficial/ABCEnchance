#pragma once
class WEAPON;
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

	bool m_bSelectBlock = false;

	bool m_bAcceptDeadMessage = false;

	WEAPON* GetCurWeapon();
	void SetCurWeapon(WEAPON* wp);
private:
	int DrawWList(float flTime);

	WEAPON* m_pWeapon;
};
extern CHudCustomAmmo m_HudCustomAmmo;