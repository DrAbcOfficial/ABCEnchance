#include <metahook.h>
#include "cvardef.h"
#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>

typedef struct
{
	void		(*R_Blood)					(float* org, float* dir, int pcolor, int speed);
	void		(*R_BloodSprite)			(float* org, int colorindex, int modelIndex, int modelIndex2, float size);
	void		(*R_Explosion)				(float* pos, int model, float scale, float framerate, int flags);
	void		(*R_RicochetSprite)			(float* pos, struct model_s* pmodel, float duration, float scale);
	void		(*R_SparkEffect)			(float* pos, int count, int velocityMin, int velocityMax);
	void		(*R_BloodStream)			(float* org, float* dir, int pcolor, int speed);
	float* (*GetClientColor)			(int clientIndex);

	TEMPENTITY* (*CL_TempEntAllocHigh)		(float* org, struct model_s* model);
	TEMPENTITY* (*CL_TempEntAlloc)			(float* org, struct model_s* model);
}cl_refHookfunc_t;
typedef struct
{
	cvar_t* pBloodSpriteSpeed = NULL;
	cvar_t* pBloodSpriteNumber = NULL;
	cvar_t* pExpSmokeNumber = NULL;
	cvar_t* pExpSmokeSpeed = NULL;
	cvar_t* pRicochetNumber = NULL;

	cvar_t* pPlayerTitle = NULL;

	cvar_t* pDynamicCrossHair = NULL;
	cvar_t* pDynamicCrossHairL = NULL;
	cvar_t* pDynamicCrossHairW = NULL;
	cvar_t* pDynamicCrossHairO = NULL;
	cvar_t* pDynamicCrossHairM = NULL;

}cl_cvars_t;

extern cl_refHookfunc_t gHookFuncs;
extern cl_cvars_t gCVars;
extern cl_enginefunc_t gEngfuncs;
extern cl_exportfuncs_t gExportfuncs;

extern PVOID g_dwEngineBase;
extern DWORD g_dwEngineSize;
extern DWORD g_dwEngineBuildnum;;
extern HINSTANCE g_hEngineModule;
extern PVOID g_dwClientBase;
extern DWORD g_dwClientSize;

extern vgui::ISchemeManager* g_pScheme;
extern vgui::ISurface* pSurface;
extern vgui::ILocalize* pLocalize;
extern vgui::HFont m_hFont;
extern vgui::IScheme* pScheme;

extern SCREENINFO_s gScreenInfo;

void Sys_ErrorEx(const char* fmt, ...);
void FillDelegate();
void FillAddress();
void InstallHook();

void HUD_Init(void);
int HUD_Redraw(float time, int intermission);
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client);

void R_Blood(float* org, float* dir, int pcolor, int speed);
void R_BloodSprite(float* org, int colorindex, int modelIndex, int modelIndex2, float size);
void R_Explosion(float* pos, int model, float scale, float framerate, int flags);
void R_RicochetSprite (float* pos, struct model_s* pmodel, float duration, float scale);

#define clamp(num, a, b) max(min(num, b),a)

#define GetCallAddress(addr) (addr + (*(int *)((addr)+1)) + 5)
#define Sig_NotFound(name) Sys_ErrorEx("Could not found: %s\nEngine buildnum£º%d", #name, g_dwEngineBuildnum);
#define Sig_FuncNotFound(name) if(!gHookFuncs.name) Sig_NotFound(name)
#define Sig_AddrNotFound(name) if(!addr) Sig_NotFound(name)
#define Sig_AddrFoundOrFill(name) Sig_AddrNotFound(name) else name = (decltype(name))addr;

#define Sig_Length(a) (sizeof(a)-1)
#define Search_Pattern(sig) g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, sig, Sig_Length(sig));
#define Search_Pattern_From(fn, sig) g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.fn, g_dwEngineSize - (DWORD)gRefFuncs.fn + g_dwEngineBase, sig, Sig_Length(sig));

#define Fill_DelegateFunc(fn) gHookFuncs.fn = *gEngfuncs.pEfxAPI->fn;
#define Install_InlineHook(fn) g_pMetaHookAPI->InlineHook((void *)gRefFuncs.fn, fn, (void **)&gRefFuncs.fn);
#define Install_InlineEfxHook(fn) g_pMetaHookAPI->InlineHook((void *)*gHookFuncs.fn, fn, (void **)&gHookFuncs.fn);
#define Fill_InlineEfxHook(fn) Fill_DelegateFunc(fn) Install_InlineEfxHook(fn)


#pragma once
#ifndef MAX_PLAYER_NAME_LENGTH
#define MAX_PLAYER_NAME_LENGTH 128
#endif
#ifndef RADIAN_PER_DEGREE
#define RADIAN_PER_DEGREE 57.29577951308232087684
#endif
#ifndef DEGREE_PER_RADIAN
#define DEGREE_PER_RADIAN 0.017453292519943295769
#endif
#ifndef PERIGON_ANGLE
#define PERIGON_ANGLE 360.0
#endif
#ifndef FLAT_ANGLE
#define FLAT_ANGLE 180.0
#endif