#include <metahook.h>
typedef struct
{
	void		(*R_Blood)					(float* org, float* dir, int pcolor, int speed);
	void		(*R_BloodSprite)			(float* org, int colorindex, int modelIndex, int modelIndex2, float size);
	TEMPENTITY* (*CL_TempEntAllocHigh)		(float* org, struct model_s* model);
	TEMPENTITY* (*CL_TempEntAlloc)			(float* org, struct model_s* model);
}cl_refHookfunc_t;

extern cl_refHookfunc_t gHookFuncs;
extern cl_enginefunc_t gEngfuncs;
extern cl_exportfuncs_t gExportfuncs;
extern PVOID g_dwEngineBase;
extern DWORD g_dwEngineSize;
extern DWORD g_dwEngineBuildnum;;

void Sys_ErrorEx(const char* fmt, ...);
void FillDelegate();
void FillAddress();
void InstallHook();

void HUD_Init(void);

void R_Blood(float* org, float* dir, int pcolor, int speed);
void R_BloodSprite(float* org, int colorindex, int modelIndex, int modelIndex2, float size);

#define clamp(num, a, b) max(min(num, b),a)

#define GetCallAddress(addr) (addr + (*(int *)((addr)+1)) + 5)
#define Sig_NotFound(name) Sys_ErrorEx("Could not found: %s\nEngine buildnum£º%d", #name, g_dwEngineBuildnum);
#define Sig_FuncNotFound(name) if(!gRefFuncs.name) Sig_NotFound(name)
#define Sig_AddrNotFound(name) if(!addr) Sig_NotFound(name)
#define Sig_AddrFoundOrFill(name) Sig_AddrNotFound(name) else name = (decltype(name))addr;

#define Sig_Length(a) (sizeof(a)-1)
#define Search_Pattern(sig) g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, sig, Sig_Length(sig));
#define Search_Pattern_From(fn, sig) g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.fn, g_dwEngineSize - (DWORD)gRefFuncs.fn + g_dwEngineBase, sig, Sig_Length(sig));

#define Fill_DelegateFunc(fn) gHookFuncs.fn = *gEngfuncs.pEfxAPI->fn;
#define Install_InlineHook(fn) g_pMetaHookAPI->InlineHook((void *)gRefFuncs.fn, fn, (void **)&gRefFuncs.fn);
#define Install_InlineEfxHook(fn) g_pMetaHookAPI->InlineHook((void *)*gHookFuncs.fn, fn, (void **)&gHookFuncs.fn);
#define Fill_InlineEfxHook(fn) Fill_DelegateFunc(fn) Install_InlineEfxHook(fn)