#pragma once
#define NUM_DMG_TYPES 8
#define NUM_MAX_INDICATOR 4
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
typedef struct indicatorinfo_s {
	int iDamage;
	int iArmor;
	vec3_t vecFrom;
	float flKeepTime;
	vec2_t vecHUDA, vecHUDB, vecHUDC, vecHUDD;
}indicatorinfo_t;
typedef struct{
	float fExpire;
	float fBaseline;
	int	x, y;
} DAMAGE_IMAGE;

class CHudArmorHealth{
public:
	void GLInit();
	void Init(void);
	int Draw(float flTime);
	void Reset(void);
	int VidInit(void);
	int DrawPain(float fTime);
	int DrawDamage(float fTime);
	void CalcDamageDirection(indicatorinfo_s& var);
	void AddIdicator(int dmg, int armor, vec3_t vecFrom);
	void UpdateTiles(float fTime, long bits);

	indicatorinfo_t aryIndicators[NUM_MAX_INDICATOR];
	int iNowSelectIndicator = 0;

	int m_iHealth;
	int m_iBattery;

	GLint iHealthIcon;
	GLint iArmorIconNull;
	GLint iArmorIconFull;
	GLint iLongjumpIcon;
	GLint iPainIndicator;
	GLint iHealthBarBackground;

	float flPainColorKeepTime = 0;

	int	m_bitsDamage = 0;

	float PainColorTime = 0.5;
	float PainIndicatorTime = 0;
	float ShockIndicatorTime = 0;
private:
	void CalcuPainFade(int& r, int& g, int& b, Color* c, float timeDiffer);
	int m_iDMGIconStart;
	DAMAGE_IMAGE m_dmg[NUM_DMG_TYPES];
	HSPRITE m_hSprite;
	HSPRITE m_hDamage;

	GLint m_hOldBuffer;
	GLuint m_hFilterFBO;
	GLuint m_hFilterTex;

	indicatorinfo_t m_hScreenFilter;

	float StartX = 48;
	float IconSize = 0.5;
	float BarLength = 2;
	float BarWidth = 0.334;
	float ElementGap = 0.2;
	float BackGroundY = 0.95;
	float BackGroundLength = 3;
	float DamageIconX = 0;
	float DamageIconY = 0;
	float DamageIconSize = 96;

	Color HealthIconColor;
	Color HealthBarColor;
	Color HealthTextColor;
	Color HealthPainColor;
	Color HealthDangerColor;

	Color ArmorIconColor;
	Color ArmorBarColor;
	Color ArmorTextColor;
	Color ArmorPainColor;
	Color ArmorDangerColor;

	Color LongjumpIconColor;

	Color PainIndicatorColor;
	Color PainIndicatorColorA;
	Color BitDamageColor;

	vgui::HFont HUDFont;
};
extern CHudArmorHealth m_HudArmorHealth;