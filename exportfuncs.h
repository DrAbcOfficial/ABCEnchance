#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>
#include "enginedef.h"
#include "r_studioint.h"

extern cl_enginefunc_t gEngfuncs;
extern cl_exportfuncs_t gExportfuncs;
extern engine_studio_api_t gEngineStudio;

extern const clientdata_t* gClientData;
extern float m_hfov;

extern overviewInfo_t* gDevOverview;
extern int* g_iVisibleMouse;
extern baseweapon_t* (*g_rgBaseSlots)[10][26];
extern refdef_t* g_refdef;
extern metaplugins_t g_metaplugins;

void Sys_ErrorEx(const char* fmt, ...);
void CheckOtherPlugin();
void FillEfxAddress();
void FillAddress();
void InstallHook();
void MSG_Init(void);

void GL_Init(void);
void HUD_Init(void);
int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio);
int HUD_VidInit(void);
int HUD_Redraw(float time, int intermission);
void HUD_Reset(void);
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client);
int HUD_UpdateClientData(struct client_data_s* c, float f);
void HUD_ClientMove(struct playermove_s* ppmove, qboolean server);
void HUD_Clear(void);
void V_CalcRefdef(struct ref_params_s* pparams);
void IN_MouseEvent(int mstate);
void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active);
int HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding);
void IN_Accumulate(void);
int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname);

void R_NewMap(void);
void R_RenderView(int a1);

#define Fill_Sig(sig, base, size, dst) {gHookFuncs.dst = (decltype(gHookFuncs.dst))g_pMetaHookAPI->SearchPattern(base, size, sig, Sig_Length(sig));Sig_FuncNotFound(dst);}
#define GetCallAddress(addr) (addr + (*(int *)((addr)+1)) + 5)
#define Sig_NotFound(name) Sys_ErrorEx("Could not found: %s\nEngine buildnum£º%d", #name, g_dwEngineBuildnum);
#define Sig_FuncNotFound(name) if(!gHookFuncs.name) Sig_NotFound(name)
#define Sig_AddrNotFound(name) if(!addr) Sig_NotFound(name)
#define Sig_AddrFoundOrFill(name) Sig_AddrNotFound(name) else name = (decltype(name))addr;

#define Sig_Length(a) (sizeof(a)-1)
#define Search_Pattern(sig) g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, sig, Sig_Length(sig));
#define Search_Pattern_From(fn, sig) g_pMetaHookAPI->SearchPattern((void *)fn, ((PUCHAR)g_dwEngineTextBase + g_dwEngineTextSize) - (PUCHAR)fn, sig, Sig_Length(sig))

#define Fill_EfxFunc(fn) gHookFuncs.fn = *gEngfuncs.pEfxAPI->fn
#define Fill_EngFunc(fn) gHookFuncs.fn = *gEngfuncs.fn
#define Install_InlineHook(fn) g_pMetaHookAPI->InlineHook((void *)gRefFuncs.fn, fn, (void **)&gRefFuncs.fn)
#define Install_InlineEngHook(fn) g_pMetaHookAPI->InlineHook((void *)*gHookFuncs.fn, fn, (void **)&gHookFuncs.fn)
#define Fill_InlineEfxHook(fn) Fill_EfxFunc(fn);Install_InlineEngHook(fn)