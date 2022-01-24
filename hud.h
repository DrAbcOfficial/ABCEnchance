#define WEAPON_SUIT			31

#ifndef _WIN32
#define _cdecl 
#endif

typedef int HSPRITE;	// handle to a graphic

#define DHN_DRAWZERO 1
#define DHN_2DIGITS  2
#define DHN_3DIGITS  4
#define MIN_ALPHA	 100	

#define		HUDELEM_ACTIVE	1

constexpr int HIDEHUD_WEAPONS = ( 1<<0 );
constexpr int HIDEHUD_FLASHLIGHT = ( 1<<1 );
constexpr int HIDEHUD_ALL = ( 1<<2 );
constexpr int HIDEHUD_HEALTH = ( 1<<3 );

typedef struct {
	int x, y;
} POSITION;

typedef struct {
	unsigned char r, g, b, a;
} RGBA;
struct message_parms_t{
	client_textmessage_t* pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};
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