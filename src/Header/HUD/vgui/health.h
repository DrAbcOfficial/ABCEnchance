#ifndef CHEALTHPANEL_H
#define CHEALTHPANEL_H
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
	class ImagePanel;
	class ImageClipPanel;
	class CSPRImage;
}

class CHealthPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CHealthPanel, vgui::EditablePanel);

	CHealthPanel();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;

	void UpdateTiles(long bits);
	void SetHealth(int health);
	void SetArmor(int armor);

	void SetLongJump(bool lj);
	void SetArmorVisible(bool state);
	void SetHealthVisible(bool state);

	enum DMG_IMAGE {
		DMG_IMAGE_POISON = 0,
		DMG_IMAGE_ACID,
		DMG_IMAGE_COLD,
		DMG_IMAGE_LIFE = 2,	// seconds that image is up
		DMG_IMAGE_DROWN,
		DMG_IMAGE_BURN,
		DMG_IMAGE_NERVE,
		DMG_IMAGE_RAD,
		DMG_IMAGE_SHOCK
	};
	enum DMG_TYPE {
		DMG_GENERIC = 0, // Generic damage.
		DMG_CRUSH = 1, // Crush damage.By falling or moving object.
		DMG_BULLET = 2, // Bullet.Shot.
		DMG_SLASH = 4, // Slash.Cut, clawed, stabbed.
		DMG_BURN = 8, // Burn.Heat burned.
		DMG_FREEZE = 16, // Freeze.Frozen.
		DMG_FALL = 32, // Fall.Fell too far.
		DMG_BLAST = 64, // Blast.Explosive blast damage.
		DMG_CLUB = 128, // Club.Crowbar, punch, headbutt.
		DMG_SHOCK = 256, // Shock.Electric shock.
		DMG_SONIC = 512, // Sonic.Sound pulse shockwave.
		DMG_ENERGYBEAM = 1024, // Energy beam.Laser or other high energy beam.
		DMG_NEVERGIB = 4096, // Never gib.With this bit OR'd in, no damage type will be able to gib victims upon death. 
		DMG_ALWAYSGIB = 8192, // Always gib.With this bit OR'd in, any damage type can be made to gib victims upon death. 
		DMG_LAUNCH = 67108864, // Launch.Launch into air.
		DMG_DROWN = 16384, // Drown.Drowning.
		DMG_TIMEBASED = 16760832, // Mash for time - based damage.
		DMG_PARALYZE = 32768, // Paralyze.Slows affected creature down.
		DMG_NERVEGAS = 65536, // Nerve gas.Nerve toxins, very bad.
		DMG_POISON = 131072, // Poison.Blood poisoning.
		DMG_RADIATION = 262144, // Radiation.Radiation exposure.
		DMG_DROWNRECOVER = 524288, // Drown recover.Drowning recovery.
		DMG_ACID = 1048576, // Acid.Toxic chemicals or acid burns.
		DMG_SLOWBURN = 2097152, // Slow burn.In an oven.
		DMG_SLOWFREEZE = 4194304, // Slow freeze.In a subzero freezer.
		DMG_MORTAR = 8388608, // Mortar.Hit by air raid(done to distinguish grenade from mortar)
		DMG_SNIPER = 16777216, // Sniper.Hit by a Sniper.
		DMG_MEDKITHEAL = 33554432, // Medkit heal.Medkit healing.
		DMG_SHOCK_GLOW = 134217728, // Shock glow.Flag for shock glow effect.
		DMG_GIB_CORPSE = 993, // Gib corpse.These are the damage types that are allowed to gib corpses.
		DMG_SHOWNHUD = 7815448 // Shown on HUD.These are the damage types that have clien hud art.
	};

	struct dmgimageitem_t {
		const char szName[32];
		const char szIconKey[32];
		const int iDmg;
		float fExpire;
		int iIndex;
	};
private:
	int m_iRestoredHealWide = 0;
	int m_iRestoredArmorWide = 0;

	Color m_cRestoredHealth;
	Color m_cRestoredArmor;

	Color m_cHealthDanger;
	Color m_cArmorDanger;

	vgui::ImagePanel* m_Backround = nullptr;

	vgui::ImagePanel* m_HealthIcon = nullptr;
	vgui::Label* m_pHealthLable = nullptr;
	vgui::ImagePanel* m_pHealthImagePanel = nullptr;
	vgui::ImagePanel* m_HealthImageBackround = nullptr;

	vgui::ImagePanel* m_ArmorIcon = nullptr;
	vgui::ImagePanel* m_pArmorImagePanel = nullptr;
	vgui::Label* m_pArmorLable = nullptr;
	vgui::ImagePanel* m_ArmorImageBackround = nullptr;

	vgui::ImagePanel* m_pLongJumpImagePanel = nullptr;

	vgui::ListViewPanel* m_pDmgImages = nullptr;

	std::vector<dmgimageitem_t> m_aryDmgImageList;
};
#endif