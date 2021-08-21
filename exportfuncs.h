#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>

extern cl_enginefunc_t gEngfuncs;
extern cl_exportfuncs_t gExportfuncs;

extern vgui::ISchemeManager* g_pScheme;
extern vgui::ISurface* pSurface;
extern vgui::ILocalize* pLocalize;
extern vgui::HFont m_hFont;
extern vgui::IScheme* pScheme;

extern SCREENINFO_s gScreenInfo;
extern const clientdata_t* gClientData;

void Sys_ErrorEx(const char* fmt, ...);
void FillDelegate();
void FillAddress();
void InstallHook();
void MSG_Init(void);

void HUD_Init(void);
int HUD_VidInit(void);
int HUD_Redraw(float time, int intermission);
void HUD_Reset(void);
void HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client);
int HUD_UpdateClientData(struct client_data_s* c, float f);

void R_NewMap(void);

#define GetCallAddress(addr) (addr + (*(int *)((addr)+1)) + 5)
#define Sig_NotFound(name) Sys_ErrorEx("Could not found: %s\nEngine buildnum£º%d", #name, g_dwEngineBuildnum);
#define Sig_FuncNotFound(name) if(!gHookFuncs.name) Sig_NotFound(name)
#define Sig_AddrNotFound(name) if(!name) Sig_NotFound(name)
#define Sig_AddrFoundOrFill(name) Sig_AddrNotFound(name) else name = (decltype(name))addr;

#define Sig_Length(a) (sizeof(a)-1)
#define Search_Pattern(sig) g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, sig, Sig_Length(sig));
#define Search_Pattern_From(fn, sig) g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.fn, g_dwEngineSize - (DWORD)gRefFuncs.fn + g_dwEngineBase, sig, Sig_Length(sig));

#define Fill_DelegateFunc(fn) gHookFuncs.fn = *gEngfuncs.pEfxAPI->fn;
#define Install_InlineHook(fn) g_pMetaHookAPI->InlineHook((void *)gRefFuncs.fn, fn, (void **)&gRefFuncs.fn);
#define Install_InlineEfxHook(fn) g_pMetaHookAPI->InlineHook((void *)*gHookFuncs.fn, fn, (void **)&gHookFuncs.fn);
#define Fill_InlineEfxHook(fn) Fill_DelegateFunc(fn) Install_InlineEfxHook(fn)