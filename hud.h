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

typedef struct {
	int x, y;
} POSITION;

typedef struct {
	unsigned char r, g, b, a;
} RGBA;

typedef struct cvar_s cvar_t;
#define HUD_ACTIVE	1
#define HUD_INTERMISSION 2
#define	MAX_MOTD_LENGTH				1536

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