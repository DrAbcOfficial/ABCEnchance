#define CVAR_GET_FLOAT(x) gEngfuncs.pfnGetCvarFloat(x)
#define CVAR_GET_STRING(x) gEngfuncs.pfnGetCvarString(x)
#define SPR_Load (*gEngfuncs.pfnSPR_Load)
#define SPR_Set (*gEngfuncs.pfnSPR_Set)
#define SPR_Frames (*gEngfuncs.pfnSPR_Frames)
#define SPR_GetList(x, y) (*gEngfuncs.pfnSPR_GetList)((char*)x, y)
// SPR_Draw  draws a the current sprite as solid
#define SPR_Draw (*gEngfuncs.pfnSPR_Draw)
// SPR_DrawHoles  draws the current sprites,  with color index255 not drawn (transparent)
#define SPR_DrawHoles (*gEngfuncs.pfnSPR_DrawHoles)
// SPR_DrawAdditive  adds the sprites RGB values to the background  (additive transulency)
#define SPR_DrawAdditive (*gEngfuncs.pfnSPR_DrawAdditive)
// SPR_EnableScissor  sets a clipping rect for HUD sprites.  (0,0) is the top-left hand corner of the screen.
#define SPR_EnableScissor (*gEngfuncs.pfnSPR_EnableScissor)
// SPR_DisableScissor  disables the clipping rect
#define SPR_DisableScissor (*gEngfuncs.pfnSPR_DisableScissor)
#define FillRGBA (*gEngfuncs.pfnFillRGBA)
// ScreenHeight returns the height of the screen, in pixels
#define ScreenHeight (gScreenInfo.iHeight)
// ScreenWidth returns the width of the screen, in pixels
#define ScreenWidth (gScreenInfo.iWidth)
#define GetScreenInfo (*gEngfuncs.pfnGetScreenInfo)
#define ServerCmd(x) (*gEngfuncs.pfnServerCmd)((char*)x)
#define EngineClientCmd (*gEngfuncs.pfnClientCmd)
#define SetCrosshair (*gEngfuncs.pfnSetCrosshair)
#define PlaySoundByName(x, y) (*gEngfuncs.pfnPlaySoundByName)((char*)x, y)

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