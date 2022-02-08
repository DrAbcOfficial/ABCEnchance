constexpr int WEAPON_SUIT = 31;

typedef int HSPRITE;	// handle to a graphic

const enum {
	DHN_DRAWZERO = (1 << 0),
	DHN_2DIGITS = (1 << 1),
	DHN_3DIGITS = (1 << 2)
};
const enum {
	HUD_HIDEWEAPONS = (1 << 0),
	HUD_HIDEFLASHLIGHT = (1 << 1),
	HUD_HIDEALL = (1 << 2),
	HUD_HIDEHEALTH = (1 << 3),
	HUD_HIDESELECTION = (1 << 4),
	HUD_HIDEBATTERY = (1 << 5),
	HUD_HIDECUSTOM1 = (1 << 6),
	HUD_HIDECUSTOM2 = (1 << 7)
};
constexpr auto MIN_ALPHA = 100	;
constexpr auto HUDELEM_ACTIVE = 1;

typedef struct {
	int x, y;
} POSITION;
typedef struct {
	unsigned char r, g, b, a;
} RGBA;
typedef struct cvar_s cvar_t;
#define HUD_ACTIVE			1
#define HUD_INTERMISSION	2
#define	MAX_MOTD_LENGTH		1536
#define maxHUDMessages		16

class CHudBase{
public:
	POSITION  m_pos;
	int   m_type;
	int	  m_iFlags; // active, moving, 
	virtual		~CHudBase() {}
	virtual int Init(void) { return 0; }
	virtual int VidInit(void) { return 0; }
	virtual int Draw(float flTime) { return 0; }
	virtual void Think(void) { return; }
	virtual void Reset(void) { return; }
	virtual void InitHUDData(void) {}		// called every time a server is connected to

};
struct HUDLIST {
	CHudBase* p;
	HUDLIST* pNext;
};