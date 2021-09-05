#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>
#include "enginedef.h"

extern cl_enginefunc_t gEngfuncs;
extern cl_exportfuncs_t gExportfuncs;

extern const clientdata_t* gClientData;
extern float m_hfov;

extern overviewInfo_t* gDevOverview;
extern int* g_iVisibleMouse;
extern baseweapon_t* (*g_rgBaseSlots)[10][26];
extern refdef_t* g_refdef;
extern metaplugins_t g_metaplugins;

void Sys_ErrorEx(const char* fmt, ...);
void CheckOtherPlugin();
void FillDelegate();
void FillAddress();
void InstallHook();
void MSG_Init(void);

void GL_Init(void);
void HUD_Init(void);
int HUD_VidInit(void);
int HUD_Redraw(float time, int intermission);
void HUD_Reset(void);
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client);
int HUD_UpdateClientData(struct client_data_s* c, float f);
void HUD_ClientMove(struct playermove_s* ppmove, qboolean server);
void HUD_Clear(void);
void V_CalcRefdef(struct ref_params_s* pparams);
void IN_MouseEvent(int mstate);
int HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding);
void IN_Accumulate(void);
void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active);

void R_NewMap(void);
void R_RenderView(int a1);

#define GetCallAddress(addr) (addr + (*(int *)((addr)+1)) + 5)
#define Sig_NotFound(name) Sys_ErrorEx("Could not found: %s\nEngine buildnum£º%d", #name, g_dwEngineBuildnum);
#define Sig_FuncNotFound(name) if(!gHookFuncs.name) Sig_NotFound(name)
#define Sig_AddrNotFound(name) if(!addr) Sig_NotFound(name)
#define Sig_AddrFoundOrFill(name) Sig_AddrNotFound(name) else name = (decltype(name))addr;

#define Sig_Length(a) (sizeof(a)-1)
#define Search_Pattern(sig) g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, sig, Sig_Length(sig));
#define Search_Pattern_From(fn, sig) g_pMetaHookAPI->SearchPattern((void *)fn, ((PUCHAR)g_dwEngineTextBase + g_dwEngineTextSize) - (PUCHAR)fn, sig, Sig_Length(sig))

#define Fill_DelegateFunc(fn) gHookFuncs.fn = *gEngfuncs.pEfxAPI->fn;
#define Install_InlineHook(fn) g_pMetaHookAPI->InlineHook((void *)gRefFuncs.fn, fn, (void **)&gRefFuncs.fn);
#define Install_InlineEfxHook(fn) g_pMetaHookAPI->InlineHook((void *)*gHookFuncs.fn, fn, (void **)&gHookFuncs.fn);
#define Fill_InlineEfxHook(fn) Fill_DelegateFunc(fn) Install_InlineEfxHook(fn)